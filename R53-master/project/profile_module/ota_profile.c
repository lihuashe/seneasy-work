/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ota_profile.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "ota_profile.h"
#include "stack/ble/ble.h"
#include "../led_module/led_driver.h"
#include "../app_ui.h"
#include "app_ota.h"
#include "../app_utility/app_custom.h"
#include "app_rcu.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
const u16 userdesc_UUID         = GATT_UUID_CHAR_USER_DESC;
const u8 my_OtaUUID[16]         = {TELINK_SPP_DATA_OTA};
const u8 my_OtaServiceUUID[16]  = TELINK_OTA_UUID_SERVICE;
const u8  my_OtaName[3]         = {'O', 'T', 'A'};
u8 my_OtaData = 0x00;
_attribute_data_retention_ u16 my_Otaccc = 0;

/*============================================================================*
 *                             OTA attribute values
 *============================================================================*/
const u8 my_OtaCharVal[19] = {
    CHAR_PROP_READ | CHAR_PROP_NOTIFY | CHAR_PROP_WRITE_WITHOUT_RSP,
    U16_LO(OTA_CMD_OUT_DP_H), U16_HI(OTA_CMD_OUT_DP_H),
    TELINK_SPP_DATA_OTA,
};

int app_necir_or_dataupdate(void *p);
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  app_ota_handle
 * @param  p
 * @return int
 * @retval void
 */
int app_ota_handle(void * p)
{
    int result=0;
    app_necir_or_dataupdate(p);
    result = app_ota_write(p);

    return result;
}

/******************************************************************
 * @brief  app_necir_or_dataupdate
 * @param  p
 * @return int
 * @retval void
 */
int app_necir_or_dataupdate(void *p)
{
    u16 handle,len;
    u16 printf_len;

    rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;
    handle = pw->handle;

    //printf("handle = %x\r\n",handle);
    //printf("pw->l2cap=%x\r\n",pw->l2cap-3);
    //printf("app_nec_ir_table_callback handle=%x\r\n",handle);

    len = pw->l2cap-3;//pw->rf_len-7;
    printf_len = len;

    #if 0
    printf("len=%x\r\n",len);
    for(u8 i=0;i<printf_len;i++)
        printf(" %x",pw->dat[i]);
    #endif

#if APP_IR_OVER_BLE
    ir_nec_ir_table_process(pw->dat,len);
#endif

#if APP_DATA_UPDATE_OVER_BLE
    app_custom_data_update_process(pw->dat,len);
#endif

    return 0;
}


void app_enter_ota_mode_1(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] app_enter_ota_mode_1\r\n");
    #endif
#if APP_RCU_FLASH_LOCK_ENABLE
    u8 try_count = 10;
    while (try_count--)
    {
        if(bls_ll_requestConnBrxEventDisable() > 5)
        {
            bls_ll_disableConnBrxEvent();
            app_rcu_flash_unlock_init();
            bls_ll_restoreConnBrxEvent();
        }
        sleep_ms(5);
    }
#endif  

    ota_is_working = 1;
    #if (BLT_APP_LED_ENABLE)
        app_dcdc_enable();
        device_led_setup(led_cfg[LED_OTA_START]);
    #endif
    bls_ota_setTimeout(150 * 1000 * 1000); //set OTA timeout  150 seconds
    bls_pm_setManualLatency(0);
    // bls_pm_setSuspendMask(SUSPEND_DISABLE);

}
void app_debug_ota_result_1(int result)
{
    printf("ota result: %d\n",result);
    #if(1 && BLT_APP_LED_ENABLE)  //this is only for debug
        gpio_set_output_en(GPIO_LED, 1);
        if(result == OTA_SUCCESS){  //led for debug: OTA success
          
        }
        else{  //OTA fail

            #if 1 //this is only for debug,  can not use this in application code
               /* irq_disable();
                WATCHDOG_DISABLE;
                while(1){
                    gpio_write(GPIO_LED, 1);
                    sleep_us(200000);
                    gpio_write(GPIO_LED, 0);
                    sleep_us(200000);
                }*/
                printf("ota fail ........\r\n");
               // gpio_write(GPIO_LED, 0);
                device_led_setup(led_cfg[LED_LONG_OFF]);
                ota_is_working=0;
            #endif
        }
        app_dcdc_delay_disable();
        gpio_set_output_en(GPIO_LED, 0);
    #endif
}


/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
