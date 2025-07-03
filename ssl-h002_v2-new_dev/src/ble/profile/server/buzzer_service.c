/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     buzzer_service.c
* @brief    Buzzer service source file.
* @details  Interfaces to access Buzzer service.
* @author
* @date     2020-03-11
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include "string.h"
#include "trace.h"
#include "gatt.h"
#include "profile_server.h"
#include "buzzer_service.h"
#include "gap.h"
#include "mem_config.h"
#include "app_section.h"
#include "overlay_mgr.h"

/*============================================================================*
 *                          Global Variables
 *============================================================================*/
uint8_t buzzer_svc_handshake_values[16] = {0};

/*============================================================================*
 *                          Local Variables
 *============================================================================*/
static P_FUN_SERVER_GENERAL_CB pfn_buzzer_svc_cb = NULL;
static const uint8_t GATT_UUID_BUZZER_SERVICE[2] = {0xF0, 0xFF};

/**< @brief  profile/service definition.
*   here is an example of OTA service table
*   including Write
*/
const T_ATTRIB_APPL buzzer_svc_bas_attr_tbl[] =
{
    /*--------------------------Buzzer Service ---------------------------*/
    /* <<Primary Service>>, .. 0*/
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),  /* wFlags     */
        {
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),     /* bTypeValue */
        },
        UUID_16BIT_SIZE,                     /* bValueLen     */
        (void *)GATT_UUID_BUZZER_SERVICE,        /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>> 1 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* characteristic value 2 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BUZZER_HANDSHAKE),
            HI_WORD(GATT_UUID_CHAR_BUZZER_HANDSHAKE),
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    },

    // /* client characteristic configuration 3 */
    // {
    //     ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* wFlags */
    //     {   /* bTypeValue */
    //         LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
    //         HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
    //         /* NOTE: this value has an instantiation for each client, a write to */
    //         /* this attribute does not modify this default value:                */
    //         LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
    //         HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
    //     },
    //     2,                                          /* bValueLen */
    //     NULL,
    //     (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    // },

    /* <<Characteristic>> 4 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /* characteristic value 5 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                          /* type_value */
            LO_WORD(GATT_UUID_CHAR_BUZZER_TEST_MODE),
            HI_WORD(GATT_UUID_CHAR_BUZZER_TEST_MODE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE                             /* permissions */
    },
};

/*============================================================================*
 *                          Local Functions
 *============================================================================*/
/**
 * @brief write characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param iAttribIndex       Attribute index of characteristic.
 * @param wLength            length of value to be written.
 * @param pValue             value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT  buzzer_svc_attr_write_cb(uint8_t conn_id, uint8_t service_id, uint16_t attrib_index,
                                       T_WRITE_TYPE write_type,
                                       uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    bool handle = true;
    T_BUZZER_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;

    APP_PRINT_INFO1("[buzzer_svc_attr_write_cb] attrib_index is %d", attrib_index);

    if (BLE_SERVICE_CHAR_BUZZER_HANDSHAKE_INDEX == attrib_index)
    {
        callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
        callback_data.msg_data.write_msg.write_type = BUZZER_WRITE_HANDSHAKE;
        callback_data.msg_data.write_msg.write_parameter.report_data.len = length;
        callback_data.msg_data.write_msg.write_parameter.report_data.report = p_value;
    }
    else if (BLE_SERVICE_CHAR_BUZZER_TEST_MODE_INDEX == attrib_index)
    {
        callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
        callback_data.msg_data.write_msg.write_type = BUZZER_WRITE_TEST_MODE;
        callback_data.msg_data.write_msg.write_parameter.report_data.len = length;
        callback_data.msg_data.write_msg.write_parameter.report_data.report = p_value;
    }
    else
    {
        handle = false;
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }

    if ((pfn_buzzer_svc_cb) && (handle == true))
    {
        pfn_buzzer_svc_cb(service_id, (void *)&callback_data);
    }

    return cause;

}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void buzzer_svc_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index,
                               uint16_t ccc_bits)
{
    bool handle = true;
    T_BUZZER_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;

    APP_PRINT_INFO2("[buzzer_svc_cccd_update_cb] index = %d, ccc_bits %x", index, ccc_bits);
    switch (index)
    {
    case GATT_SVC_BUZZER_HANDSHAKE_CHAR_CCCD_INDEX:
        {
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indification_index = BUZZER_NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indification_index = BUZZER_NOTIFY_DISABLE;
            }
            break;
        }
    default:
        {
            handle = false;
            break;
        }
    }

    if (pfn_buzzer_svc_cb && (handle == true))
    {
        /* Notify Application */
        pfn_buzzer_svc_cb(service_id, (void *)&callback_data);
    }

    return;
}

/**
 * @brief Buzzer Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS buzzer_svc_cbs =
{
    NULL,   // Read callback function pointer
    buzzer_svc_attr_write_cb,  // Write callback function pointer
    buzzer_svc_cccd_update_cb                    // CCCD update callback function pointer
};

/*============================================================================*
 *                          Global Functions
 *============================================================================*/
/**
  * @brief       Add buzzer service to the BLE stack database.
  *
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval Others Service id assigned by stack.
  *
  * <b>Example usage</b>
  * \code{.c}
     void profile_init()
     {
         server_init(1);
         bas_id = buzzer_svc_add_service(app_handle_profile_message);
     }
  * \endcode
  */
uint8_t buzzer_svc_add_service(void *p_func)
{
    uint8_t service_id;
    if (false == server_add_service(&service_id,
                                    (uint8_t *)buzzer_svc_bas_attr_tbl,
                                    sizeof(buzzer_svc_bas_attr_tbl),
                                    buzzer_svc_cbs))
    {
        APP_PRINT_INFO1("buzzer_svc_add_service: service_id %d", service_id);
        service_id = 0xff;
        return service_id;
    }
    pfn_buzzer_svc_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return service_id;
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
