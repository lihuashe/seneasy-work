/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     mp_test.c
* @brief
* @details
* @author   chenjie jin
* @date     2020-03-03
* @version  v2.0
*********************************************************************************************************
*/

/*============================================================================*
 *                                  Header Files
 *============================================================================*/
#include "rtl876x.h"
#include "trace.h"
#include "gap.h"
#include "board.h"
#include "mp_test.h"
#include "test_mode.h"
#include "gap_storage_le.h"
#include "mem_types.h"
#include "ftl.h"
#include "os_mem.h"
#include "gap_bond_le.h"
#include <string.h>
#include "key_handle.h"
#include <rtl876x_rcc.h>
#include <rtl876x_pinmux.h>
#include <rtl876x_gpio.h>

#if FEATURE_SUPPORT_MP_TEST_MODE

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
/* Fast Pairing Local Mac Address Config List */
static const uint8_t mp_fp_local_addr_config_list[FP_MAX_LINE_NUM][FP_MAC_ADDR_LEN] =
{
    {0x01, 0x88, 0x23, 0x4c, 0xe0, 0x00},
    {0x02, 0x88, 0x23, 0x4c, 0xe0, 0x00},
    {0x03, 0x88, 0x23, 0x4c, 0xe0, 0x00},
    {0x04, 0x88, 0x23, 0x4c, 0xe0, 0x00},
    {0x05, 0x88, 0x23, 0x4c, 0xe0, 0x00},
};

/* Fast Pairing Remote Mac Address Config List */
static const uint8_t mp_fp_remote_addr_config_list[FP_MAX_LINE_NUM][FP_MAC_ADDR_LEN] =
{
    {0x87, 0x99, 0x23, 0x4c, 0xe0, 0x00},
    {0x87, 0x99, 0x23, 0x4c, 0xe0, 0x00},
    {0x87, 0x99, 0x23, 0x4c, 0xe0, 0x00},
    {0x87, 0x99, 0x23, 0x4c, 0xe0, 0x00},
    {0x87, 0x99, 0x23, 0x4c, 0xe0, 0x00},
};

/* Fast Pairing Link Key Config List */
static const uint8_t mp_fp_link_key_config_list[FP_MAX_LINE_NUM][FP_LINK_KEY_LEN] =
{
    {0xcb, 0x84, 0xaa, 0x4d, 0x66, 0x42, 0xd5, 0xa2, 0x33, 0xa1, 0x6a, 0x51, 0x9a, 0x50, 0xb5, 0xac},
    {0xcb, 0x84, 0xaa, 0x4d, 0x66, 0x42, 0xd5, 0xa2, 0x33, 0xa1, 0x6a, 0x51, 0x9a, 0x50, 0xb5, 0xac},
    {0xcb, 0x84, 0xaa, 0x4d, 0x66, 0x42, 0xd5, 0xa2, 0x33, 0xa1, 0x6a, 0x51, 0x9a, 0x50, 0xb5, 0xac},
    {0xcb, 0x84, 0xaa, 0x4d, 0x66, 0x42, 0xd5, 0xa2, 0x33, 0xa1, 0x6a, 0x51, 0x9a, 0x50, 0xb5, 0xac},
    {0xcb, 0x84, 0xaa, 0x4d, 0x66, 0x42, 0xd5, 0xa2, 0x33, 0xa1, 0x6a, 0x51, 0x9a, 0x50, 0xb5, 0xac},
};

/* Fast Pairing CCCD Information */
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
static const uint8_t mp_fp_cccd_info[FP_CCCD_DATA_LEN] =
{
    0x04, 0x00, 0x02, 0x00, 0x11, 0x00, 0x01, 0x00,
    0x30, 0x00, 0x01, 0x00, 0x34, 0x00, 0x01, 0x00,
    0x3b, 0x00, 0x01, 0x00, 0x5b, 0x00, 0x01, 0x00,
    0x5e, 0x00, 0x01, 0x00, 0x64, 0x00, 0x01, 0x00
};
#else
static const uint8_t mp_fp_cccd_info[FP_CCCD_DATA_LEN] =
{
    0x3c, 0x00, 0x01, 0x00, 0x30, 0x00, 0x01, 0x00,
    0x34, 0x00, 0x01, 0x00
};
#endif

uint32_t test_flag_ftl_value = MP_TEST_MODE_FLG_DISABLE_VALUE;

/*============================================================================*
*                              Local Functions
*============================================================================*/


/*============================================================================*
*                              Global Functions
*============================================================================*/
/******************************************************************
 * @brief   MP Test initialize data.
 * @param   void
 * @return  void
 * @retval  none
 */
void mp_test_init_data(void)
{
    uint32_t ftl_res = 0;

    ftl_res = ftl_load(&test_flag_ftl_value, MP_TEST_FTL_PARAMS_TEST_MODE_FLG_OFFSET,
                       MP_TEST_FTL_PARAMS_TEST_MODE_FLG_LEN);

    if (ftl_res == FTL_READ_ERROR_READ_NOT_FOUND)
    {
        APP_PRINT_WARN0("[mp_test_is_test_mode_flag_en] test mode ftl flag is invalid, reset to enabled!");
        mp_test_enable_test_mode_flag();
        ftl_res = ftl_load(&test_flag_ftl_value, MP_TEST_FTL_PARAMS_TEST_MODE_FLG_OFFSET,
                           MP_TEST_FTL_PARAMS_TEST_MODE_FLG_LEN);
    }

    APP_PRINT_INFO2("[mp_test_is_test_mode_flag_en] ftl_res is %d, value is 0x%08X", ftl_res,
                    test_flag_ftl_value);
}

/******************************************************************
 * @brief   MP Test enable test mode flag.
 * @param   none
 * @return  result
 * @retval  true of false
 */
bool mp_test_enable_test_mode_flag(void)
{
    uint32_t result = false;
    test_flag_ftl_value = MP_TEST_MODE_FLG_ENABLE_VALUE;

    result = ftl_save(&test_flag_ftl_value, MP_TEST_FTL_PARAMS_TEST_MODE_FLG_OFFSET,
                      MP_TEST_FTL_PARAMS_TEST_MODE_FLG_LEN);

    return (result == 0);
}

/******************************************************************
 * @brief   MP Test disable test mode flag.
 * @param   none
 * @return  result
 * @retval  true of false
 */
bool mp_test_disable_test_mode_flag(void)
{
    uint32_t result = false;
    test_flag_ftl_value = MP_TEST_MODE_FLG_DISABLE_VALUE;

    result = ftl_save(&test_flag_ftl_value, MP_TEST_FTL_PARAMS_TEST_MODE_FLG_OFFSET,
                      MP_TEST_FTL_PARAMS_TEST_MODE_FLG_LEN);

    return (result == 0);
}

/******************************************************************
 * @brief   MP Test check if test mode is enabled or not.
 * @param   none
 * @return  result
 * @retval  true of false
 */
bool mp_test_is_test_mode_flag_en(void)
{
    return (test_flag_ftl_value == MP_TEST_MODE_FLG_ENABLE_VALUE);
}

/******************************************************************
 * @brief   MP Test Set Fast Pair Info.
 * @param   index
 * @return  result
 * @retval  true of false
 */
bool mp_test_set_fast_pair_info(uint8_t index)
{
    bool result = false;
    uint8_t ltk_length;
    uint8_t mp_local_bd[8] = {0};
    uint8_t remote_mac_addr[6] = {0};
    T_LE_KEY_TYPE link_key_type = LE_KEY_UNAUTHEN;
    uint8_t local_ltk[FP_LINK_KEY_LEN];
    T_GAP_REMOTE_ADDR_TYPE remote_addr_type;
    uint8_t ccc_bits_count;
    T_LE_CCCD *p_cccd_data;

    if (index >= FP_MAX_LINE_NUM)
    {
        return false;
    }

    /* set local bd addr info */
    memcpy(mp_local_bd, mp_fp_local_addr_config_list[index], FP_MAC_ADDR_LEN);
    result = ftl_save(mp_local_bd, MP_TEST_FTL_PARAMS_LOCAL_BD_ADDR_OFFSET,
                      MP_TEST_FTL_PARAMS_LOCAL_BD_ADDR_LEN);
    if (result != 0)
    {
        return false;  /* ftl save failed */
    }

    /* set remote mac addr info */
    remote_addr_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    memcpy(remote_mac_addr, mp_fp_remote_addr_config_list[index], FP_MAC_ADDR_LEN);

    /* set link key info */
    ltk_length = FP_LINK_KEY_LEN;
    memcpy(local_ltk, mp_fp_link_key_config_list[index], FP_LINK_KEY_LEN);

    /* set cccd info */
    ccc_bits_count = FP_CCCD_BITS_CNT;
    p_cccd_data = os_mem_alloc(RAM_TYPE_DATA_ON, 4 + ccc_bits_count * 4);
    p_cccd_data->data_length = FP_CCCD_DATA_LEN;
    memcpy(p_cccd_data->data, mp_fp_cccd_info, FP_CCCD_DATA_LEN);

    /* generate bond dev info */
    result = le_gen_bond_dev(remote_mac_addr, remote_addr_type, GAP_LOCAL_ADDR_LE_PUBLIC,
                             ltk_length, local_ltk, link_key_type, p_cccd_data);

    os_mem_free(p_cccd_data);

    return result;
}

/******************************************************************
 * @brief   MP Test load fast pair mac addr.
 * @param   none
 * @return  result
 * @retval  true of false
 */
bool mp_test_load_fp_mac_addr(void)
{
    bool result = false;
    uint8_t mp_mac[8] = {0};

    if (0 == ftl_load(mp_mac, MP_TEST_FTL_PARAMS_LOCAL_BD_ADDR_OFFSET, sizeof(mp_mac)))
    {
        memcpy((uint8_t *)0x00200197, mp_mac, 6);
        APP_PRINT_INFO1("[mp_test_load_fp_mac_addr] MP MAC Addr: %b", TRACE_BINARY(6, mp_mac));
        result = true;
    }
    else
    {
        APP_PRINT_ERROR0("[mp_test_load_fp_mac_addr] load mp mac addr failed!");
        result = false;
    }

    return result;
}

/******************************************************************
 * @brief   MP Test clear bond information
 * @param   none
 * @return  result
 * @retval  true of false
 */
bool mp_test_clear_bond_info(void)
{
    bool result = true;

    /* clear all keys */
    le_bond_clear_all_keys();

    result = (le_get_bond_dev_num() > 0) ? false : true;

    APP_PRINT_INFO1("[mp_test_clear_bond_info] result: %d", result);

    return result;
}

/******************************************************************
 * @brief   MP Test handle combine keys timer callback
 * @param   combine_status
 * @return  result
 * @retval  true or false
 */
bool mp_test_handle_comb_keys_timer_cb(uint32_t combine_status)
{
    bool result = true;

    switch (combine_status)
    {
#if MP_TEST_MODE_SUPPORT_HCI_UART_TEST
    case HCI_UART_TEST_COMBINE_KEYS_BIT_MASK:
        {
            DBG_DIRECT("[mp_test_handle_comb_keys_timer_cb] Switch to HCI UART test mode!");
            switch_to_hci_mode();
        }
        break;
#endif
#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
    case DATA_UART_TEST_COMBINE_KEYS_BIT_MASK:
        {
            DBG_DIRECT("[mp_test_handle_comb_keys_timer_cb] Switch to Data UART test mode!");
            switch_to_test_mode(DATA_UART_TEST_MODE);
        }
        break;
#endif
#if MP_TEST_MODE_SUPPORT_SINGLE_TONE_TEST
    case SINGLE_TONE_TEST_COMBINE_KEYS_BIT_MASK:
        {
            DBG_DIRECT("[mp_test_handle_comb_keys_timer_cb] Switch to Single Tone test mode!");
            switch_to_test_mode(SINGLE_TONE_MODE);
        }
        break;
#endif
#if MP_TEST_MODE_SUPPORT_FAST_PAIR_TEST
    case FAST_PAIR_1_COMBINE_KEYS_BIT_MASK:
        {
            if (mp_test_set_fast_pair_info(0) == true)
            {
                DBG_DIRECT("[mp_test_handle_comb_keys_timer_cb] Switch to Fast Pair 1 test mode!");
                switch_to_test_mode(AUTO_PAIR_WITH_FIX_ADDR_MODE);
            }
            else
            {
                APP_PRINT_ERROR0("[mp_test_handle_comb_keys_timer_cb] Failed to switch to Fast Pair 1 mode!");
            }
        }
        break;
    case FAST_PAIR_2_COMBINE_KEYS_BIT_MASK:
        {
            if (mp_test_set_fast_pair_info(1) == true)
            {
                DBG_DIRECT("[mp_test_handle_comb_keys_timer_cb] Switch to Fast Pair 2 test mode!");
                switch_to_test_mode(AUTO_PAIR_WITH_FIX_ADDR_MODE);
            }
            else
            {
                APP_PRINT_ERROR0("[mp_test_handle_comb_keys_timer_cb] Failed to switch to Fast Pair 2 mode!");
            }
        }
        break;
    case FAST_PAIR_3_COMBINE_KEYS_BIT_MASK:
        {
            if (mp_test_set_fast_pair_info(2) == true)
            {
                DBG_DIRECT("[mp_test_handle_comb_keys_timer_cb] Switch to Fast Pair 3 test mode!");
                switch_to_test_mode(AUTO_PAIR_WITH_FIX_ADDR_MODE);
            }
            else
            {
                APP_PRINT_ERROR0("[mp_test_handle_comb_keys_timer_cb] Failed to switch to Fast Pair 3 mode!");
            }
        }
        break;
    case FAST_PAIR_4_COMBINE_KEYS_BIT_MASK:
        {
            if (mp_test_set_fast_pair_info(3) == true)
            {
                DBG_DIRECT("[mp_test_handle_comb_keys_timer_cb] Switch to Fast Pair 4 test mode!");
                switch_to_test_mode(AUTO_PAIR_WITH_FIX_ADDR_MODE);
            }
            else
            {
                APP_PRINT_ERROR0("[mp_test_handle_comb_keys_timer_cb] Failed to switch to Fast Pair 4 mode!");
            }
        }
        break;
    case FAST_PAIR_5_COMBINE_KEYS_BIT_MASK:
        {
            if (mp_test_set_fast_pair_info(4) == true)
            {
                DBG_DIRECT("[mp_test_handle_comb_keys_timer_cb] Switch to Fast Pair 5 test mode!");
                switch_to_test_mode(AUTO_PAIR_WITH_FIX_ADDR_MODE);
            }
            else
            {
                APP_PRINT_ERROR0("[mp_test_handle_comb_keys_timer_cb] Failed to switch to Fast Pair 5 mode!");
            }
        }
        break;
#endif
    default:
        {
            result = false;
        }
        break;
    }

    return result;
}

#if (MP_TEST_MODE_TRIG_SEL & MP_TEST_MODE_TRIG_BY_GPIO)
/******************************************************************
 * @brief   Test Mode check trigger GPIO status
 * @param   none
 * @return  none
 * @retval  void
 */
void mp_test_check_trig_gpio_status(void)
{
    uint8_t gpio_status_1;
    uint8_t gpio_status_2;

    /* Enable GPIO clock */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
    /* pinmux configuration */
    Pinmux_Config(MP_TEST_TRIG_PIN_1, DWGPIO);
    Pinmux_Config(MP_TEST_TRIG_PIN_2, DWGPIO);
    /* pad configuration */
    Pad_Config(MP_TEST_TRIG_PIN_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(MP_TEST_TRIG_PIN_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);

    GPIO_InitTypeDef Gpio_Struct;
    GPIO_StructInit(&Gpio_Struct);
    Gpio_Struct.GPIO_Pin = GPIO_GetPin(MP_TEST_TRIG_PIN_1) | GPIO_GetPin(MP_TEST_TRIG_PIN_2);
    Gpio_Struct.GPIO_Mode = GPIO_Mode_IN;
    Gpio_Struct.GPIO_ITCmd = DISABLE;
    GPIO_Init(&Gpio_Struct);

    gpio_status_1 =  GPIO_ReadInputDataBit(GPIO_GetPin(MP_TEST_TRIG_PIN_1));
    gpio_status_2 =  GPIO_ReadInputDataBit(GPIO_GetPin(MP_TEST_TRIG_PIN_2));

    /* reset pad configuration to save power */
    Pad_Config(MP_TEST_TRIG_PIN_1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(MP_TEST_TRIG_PIN_2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);

    APP_PRINT_INFO2("[mp_test_check_trig_gpio_status] gpio_status_1 = %d, gpio_status_2 = %d",
                    gpio_status_1, gpio_status_2);
    if ((gpio_status_1 == 0) && (gpio_status_2 == 0))
    {
        DBG_DIRECT("[mp_test_check_trig_gpio_status] Switch to Single Tone test mode!");
        switch_to_test_mode(SINGLE_TONE_MODE);
    }
    else if ((gpio_status_1 == 1) && (gpio_status_2 == 0))
    {
        DBG_DIRECT("[mp_test_check_trig_gpio_status] Switch to Data UART test mode!");
        switch_to_test_mode(DATA_UART_TEST_MODE);
    }
    else if ((gpio_status_1 == 0) && (gpio_status_2 == 1))
    {
        DBG_DIRECT("[mp_test_check_trig_gpio_status] Switch to HCI UART test mode!");
        switch_to_hci_mode();
    }
    else
    {
        /* normal mode */
    }
}
#endif

#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
