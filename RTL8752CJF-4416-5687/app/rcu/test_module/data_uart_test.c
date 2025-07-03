/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file      data_uart_test.c
* @brief    This file provides data uart test.
* @details
* @author  chenjie
* @date     2018-05-29
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "board.h"
#include <data_uart_test.h>
#include <version.h>
#include <patch_header_check.h>
#include <gap.h>
#include <string.h>
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"
#include "mp_test.h"
#include "rcu_application.h"
#include "voice.h"
#include "voice_driver.h"
#include "rtl876x_codec.h"
#include "rtl876x_uart.h"
#include "rcu_gap.h"
#include "gap_bond_le.h"
#if MP_TEST_MODE_SUPPORT_AUTO_K_RF
#include "auto_k_rf_bonding_dut.h"
#include "auto_k_rf.h"
#endif
#if MP_TEST_MODE_SUPPORT_DATA_UART_DOWNLOAD
#include "flash_adv_cfg.h"
#endif
#include "os_sched.h"

#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST

#define TEST_DEVICE_TYPE 0x21  /* Bee2 DUT device type */

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
static bool uart_test_is_dlps_allowed = true;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
static void uart_test_get_cmd_func(UART_PacketTypeDef *p_data);
static void uart_test_read_patch_version(void *p_data);
static void uart_test_read_app_version(void *p_data);
static void uart_test_read_mac_addr(void *p_data);
static void uart_test_enter_fast_pair_mode(void *p_data);
static void uart_test_get_dev_state(void *p_data);
static void uart_test_voice_test_start(void *p_data);
static void uart_test_voice_test_stop(void *p_data);
static void uart_test_set_voice_config(void *p_data);
static void uart_test_get_voice_config(void *p_data);
static void uart_test_enter_dlps(void *p_data);
static void uart_test_start_stop_adv(void *p_data);
static void uart_test_enter_hci_mode(void *p_data);
static void uart_test_disable_test_mode_flag(void *p_data);
static void uart_test_enable_test_mode_flag(void *p_data);
static void uart_test_erase_pair_info(void *p_data);
static void uart_test_change_baudrate(void *p_data);
static void uart_test_direct_k_rf_freq(void *p_data);
static void uart_test_get_dut_info(void *p_data);
static void uart_test_auto_k_rf_freq(void *p_data);
static void uart_test_find_device_type(void *p_data);
static void uart_test_reboot_device(void *p_data);
static void uart_test_enter_single_tone(void *p_data);
static void uart_test_terminate_connect(void *p_data);
static void uart_test_enter_hci_download_mode(void *p_data);
#if SUPPORT_CODEC_EQ_CONFIG_FEATURE
static void uart_test_set_codec_eq_config(void *p_data);
static void uart_test_get_codec_eq_config(void *p_data);
#endif

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
/**<  Array of all used test function informations */
const T_UART_TEST_PROTOCOL uart_test_func_map[UART_TEST_SUPPORT_NUM] =
{
    /* Opcode, Parameter Length, Function */
    {READ_PATCH_VERSION_CMD, 0, uart_test_read_patch_version},
    {READ_APP_VERSION_CMD, 0, uart_test_read_app_version},
    {READ_MAC_ADDR_CMD, 0, uart_test_read_mac_addr},
    {ENTER_FAST_PAIR_MODE_CMD, 1, uart_test_enter_fast_pair_mode},
    {GET_DEVICE_STATE_CMD, 0, uart_test_get_dev_state},
    {VOICE_TEST_START_CMD, 0, uart_test_voice_test_start},
    {VOICE_TEST_STOP_CMD, 0, uart_test_voice_test_stop},
    {SET_VOICE_CONFIG_CMD, 16, uart_test_set_voice_config},
    {GET_VOICE_CONFIG_CMD, 0, uart_test_get_voice_config},
    {ENTER_DLPS_TEST_MODE_CMD, 0, uart_test_enter_dlps},
    {START_STOP_ADV_CMD, 1, uart_test_start_stop_adv},
    {START_IR_TEST_MODE_CMD, 0, NULL},
    {ENTER_HCI_TEST_MODE_CMD, 0, uart_test_enter_hci_mode},
    {DISABLE_TEST_MODE_FLG_CMD, 0, uart_test_disable_test_mode_flag},
    {ENABLE_TEST_MODE_FLG_CMD, 0, uart_test_enable_test_mode_flag},
    {ERASE_PAIR_INFO_CMD, 0, uart_test_erase_pair_info},
    {CHANGE_BAUDRATE_CMD, 1, uart_test_change_baudrate},
    {DIRECT_K_RF_FREQ_CMD, 2, uart_test_direct_k_rf_freq},
    {GET_GLODEN_INFO_CMD, 0, NULL},
    {GET_DUT_INFO_CMD, 32, uart_test_get_dut_info},
    {VERIFY_DUT_INFO_CMD, 0, NULL},
    {AUTO_K_RF_FREQ_CMD, 18, uart_test_auto_k_rf_freq},
    {FIND_DEVICE_TYPE_CMD, 0, uart_test_find_device_type},
    {REBOOT_DEVICE_CMD, 0, uart_test_reboot_device},
    {UPDATE_MAC_ADDR_CMD, 0, NULL},
    {ENTER_SINGLE_TONE_MODE_CMD, 1, uart_test_enter_single_tone},
    {READ_HARDWARE_VERSION_CMD, 0, NULL},
    {TERMINATE_CONNECT_CMD, 0, uart_test_terminate_connect},
    {MANUAL_K_RF_FREQ_CMD, 1, NULL},
    {ENTER_HCI_DOWNLOAD_MODE_CMD, 0, uart_test_enter_hci_download_mode},
#if SUPPORT_CODEC_EQ_CONFIG_FEATURE
    {SET_CODEC_EQ_CONFIG_CMD, 22, uart_test_set_codec_eq_config},
    {GET_CODEC_EQ_CONFIG_CMD, 1, uart_test_get_codec_eq_config},
#else
    {SET_CODEC_EQ_CONFIG_CMD, 0, NULL},
    {GET_CODEC_EQ_CONFIG_CMD, 0, NULL},
#endif
    //Add more command here,Please store in order according to opcode!
};

/**
  * @brief Read patch command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_read_patch_version(void *p_data)
{
    uint16_t patch_version = 0;
    uint8_t patch_version_bytes[2] = {0};

    T_IMG_HEADER_FORMAT *p_header;
    uint32_t addr = get_header_addr_by_img_id(RomPatch);
    p_header = (T_IMG_HEADER_FORMAT *)addr;

    if (p_header)
    {
        patch_version = p_header->git_ver.ver_info.version;
    }
    else
    {
        patch_version = 0x00;
    }

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_read_patch_version] patch version: %d", 1,
                    patch_version);

    patch_version_bytes[0] = (uint8_t)(patch_version & 0x00FF);
    patch_version_bytes[1] = (uint8_t)(patch_version >> 8);

    /* Response data */
    UARTCmd_Response(READ_PATCH_VERSION_CMD, UART_TEST_SUCCESS, patch_version_bytes, 2);
}

/**
  * @brief Read App command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_read_app_version(void *p_data)
{
    uint8_t app_version_bytes[4] = {0};
    app_version_bytes[0] = (uint8_t)(VERSION_MAJOR);
    app_version_bytes[1] = (uint8_t)(VERSION_MINOR);
    app_version_bytes[2] = (uint8_t)(VERSION_REVISION);
    app_version_bytes[3] = (uint8_t)(VERSION_BUILDNUM);

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_read_app_version] app version: %d.%d.%d.%d", 4,
                    app_version_bytes[0], app_version_bytes[1], app_version_bytes[2], app_version_bytes[3]);

    /* Response data */
    UARTCmd_Response(READ_APP_VERSION_CMD, UART_TEST_SUCCESS, app_version_bytes, 4);
}

/**
  * @brief Read Mac address command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_read_mac_addr(void *p_data)
{
    uint8_t mac_addr[6];
    gap_get_param(GAP_PARAM_BD_ADDR, mac_addr);

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_read_mac_addr] MAC Addr: %02X %02X %02X %02X %02X %02X",
                    6, mac_addr[5], mac_addr[4], mac_addr[3], mac_addr[2], mac_addr[1], mac_addr[0]);

    /* Response data */
    UARTCmd_Response(READ_MAC_ADDR_CMD, UART_TEST_SUCCESS, mac_addr, 6);
}

/**
  * @brief Enter fast pair mode command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_enter_fast_pair_mode(void *p_data)
{
    uint8_t mode_index = *((uint8_t *)p_data + 3);

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_enter_fast_pair_mode] mode index is %d", 1,
                    mode_index);

    if (mode_index < FP_MAX_LINE_NUM)
    {
        if (mp_test_set_fast_pair_info(mode_index))
        {
            DBG_DIRECT("[uart_test_enter_fast_pair_mode] Switch to Fast Pair %d test mode!", (mode_index + 1));

            UARTCmd_Response(ENTER_FAST_PAIR_MODE_CMD, UART_TEST_SUCCESS, NULL, 0);

            switch_to_test_mode(AUTO_PAIR_WITH_FIX_ADDR_MODE);
        }
    }

    UARTCmd_Response(ENTER_FAST_PAIR_MODE_CMD, UART_TEST_ERROR, NULL, 0);
}

/**
  * @brief Enter fast pair mode command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_get_dev_state(void *p_data)
{
    uint8_t dev_state[1];

    dev_state[0] = app_global_data.rcu_status;

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_get_dev_state] dev_state is %d", 1,
                    dev_state[0]);

    UARTCmd_Response(GET_DEVICE_STATE_CMD, UART_TEST_SUCCESS, dev_state, 1);
}

/**
  * @brief Voice test start command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_voice_test_start(void *p_data)
{
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_voice_test_start] start voice test", 0);

#if RCU_VOICE_EN
    UARTCmd_Response(VOICE_TEST_START_CMD, UART_TEST_SUCCESS, NULL, 0);
    os_delay(50);
    voice_handle_start_mic();
#endif
}

/**
  * @brief Voice test stop command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_voice_test_stop(void *p_data)
{
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_voice_test_stop] start voice stop", 0);

#if RCU_VOICE_EN
    voice_handle_stop_mic();
    os_delay(50);
    UARTCmd_Response(VOICE_TEST_STOP_CMD, UART_TEST_SUCCESS, NULL, 0);
#endif
}

/**
  * @brief Voice test set voice config command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_set_voice_config(void *p_data)
{
    bool result = true;
    uint8_t mic_type = *((uint8_t *)p_data + 3);
    uint8_t mic_input_type = *((uint8_t *)p_data + 4);
    uint8_t mic_bias_voltage = *((uint8_t *)p_data + 5);
    uint8_t amic_analog_gain = *((uint8_t *)p_data + 6);
    uint8_t amic_digital_gain = *((uint8_t *)p_data + 7);
    uint8_t voice_enc_type = *((uint8_t *)p_data + 8);
    uint8_t codec_sample_rate = *((uint8_t *)p_data + 9);

#if RCU_VOICE_EN
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_set_voice_config] voice config %d %d %d %d %d %d %d", 7,
                    mic_type, mic_input_type, mic_bias_voltage, amic_analog_gain,
                    amic_digital_gain, voice_enc_type, codec_sample_rate);
    /* check paramster is valid or not */
    if ((mic_type != VOICE_MIC_TYPE)
        || (mic_input_type != AMIC_INPUT_TYPE_SEL)
        || (mic_bias_voltage > UART_TEST_MIC_BIAS_VOL_ENUM_GUARD)
        || (amic_analog_gain > UART_TEST_AMIC_ANALOG_GAIN_ENUM_GUARD)
        || (voice_enc_type != UART_TEST_VOICE_ENC_TYPE_RAW)
        || (codec_sample_rate > UART_TEST_VOICE_ENC_TYPE_ENUM_GUARD)
       )
    {
        UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_set_voice_config] parameter invalid!", 0);
        result = false;
    }
    else
    {
        voice_driver_codec_params.amic_bias_voltage = ((uint32_t)(mic_bias_voltage <<
                                                                  CODEC_MICBIAS_VSET_POS));
        voice_driver_codec_params.amic_analog_gain = ((uint32_t)(amic_analog_gain <<
                                                                 CODEC_MICBST_GAIN_POS));
        voice_driver_codec_params.amic_digital_gain = amic_digital_gain;

        if (codec_sample_rate == UART_TEST_CODEC_SAMPLE_RATE_16K)
        {
            voice_driver_codec_params.codec_sample_rate = SAMPLE_RATE_16KHz;
        }
        else if (codec_sample_rate == UART_TEST_CODEC_SAMPLE_RATE_8K)
        {
            voice_driver_codec_params.codec_sample_rate = SAMPLE_RATE_8KHz;
        }
        result = true;
    }
#else
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_set_voice_config] voice module disabled, voice config %d %d %d %d %d %d %d", 7,
                    mic_type, mic_input_type, mic_bias_voltage, amic_analog_gain,
                    amic_digital_gain, voice_enc_type, codec_sample_rate);
#endif

    if (result == true)
    {
        UARTCmd_Response(SET_VOICE_CONFIG_CMD, UART_TEST_SUCCESS, NULL, 0);
    }
    else
    {
        UARTCmd_Response(SET_VOICE_CONFIG_CMD, UART_TEST_ERROR, NULL, 0);
    }
}

/**
  * @brief Voice test get voice config command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_get_voice_config(void *p_data)
{
    uint8_t voice_params[16] = {0};

#if RCU_VOICE_EN
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_get_voice_config] codec config %d %d %d %d %d %d %d", 7,
                    voice_driver_codec_params.mic_type,
                    voice_driver_codec_params.amic_input_mode,
                    voice_driver_codec_params.amic_bias_voltage,
                    voice_driver_codec_params.amic_analog_gain,
                    voice_driver_codec_params.amic_digital_gain,
                    VOICE_ENC_TYPE,
                    voice_driver_codec_params.codec_sample_rate
                   );

    voice_params[0] = (uint8_t)VOICE_MIC_TYPE;
    voice_params[1] = (uint8_t)AMIC_INPUT_TYPE_SEL;
    voice_params[2] = (uint8_t)(voice_driver_codec_params.amic_bias_voltage >> CODEC_MICBIAS_VSET_POS);
    voice_params[3] = (uint8_t)(voice_driver_codec_params.amic_analog_gain >> CODEC_MICBST_GAIN_POS);
    voice_params[4] = (uint8_t)voice_driver_codec_params.amic_digital_gain;
    voice_params[5] = (uint8_t)UART_TEST_VOICE_ENC_TYPE_RAW;
    if (voice_driver_codec_params.codec_sample_rate == SAMPLE_RATE_16KHz)
    {
        voice_params[6] = (uint8_t)UART_TEST_CODEC_SAMPLE_RATE_16K;
    }
    else if (voice_driver_codec_params.codec_sample_rate == SAMPLE_RATE_8KHz)
    {
        voice_params[6] = (uint8_t)UART_TEST_CODEC_SAMPLE_RATE_8K;
    }
#else
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_get_voice_config] voice module disabled", 0);
#endif

    UARTCmd_Response(GET_VOICE_CONFIG_CMD, UART_TEST_SUCCESS, voice_params, 16);
}

/**
  * @brief Enter DLPS command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_enter_dlps(void *p_data)
{
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_enter_dlps] enter DLPS", 0);

    UARTCmd_Response(ENTER_DLPS_TEST_MODE_CMD, UART_TEST_SUCCESS, NULL, 0);

    uart_test_is_dlps_allowed = true;
}

/**
  * @brief Start and stop advertisemnet command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_start_stop_adv(void *p_data)
{
    bool result = false;
    uint8_t cmd_type = *((uint8_t *)p_data + 3);

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_start_stop_adv] cmd_type is %d", 1, cmd_type);

    switch (cmd_type)
    {
    case ADV_IDLE:
        {
            result = rcu_stop_adv(STOP_ADV_REASON_UART_CMD);
        }
        break;
    case ADV_UNDIRECT_PAIRING:
        {
            result = rcu_start_adv(ADV_UNDIRECT_PAIRING);
        }
        break;
    case ADV_DIRECT_HDC:
    case ADV_UNDIRECT_PROMPT:
    case ADV_UNDIRECT_POWER:
        {
            if (app_global_data.is_link_key_existed == true)
            {
                result = rcu_start_adv((T_ADV_TYPE)cmd_type);
            }
        }
        break;
    default:
        break;
    }

    if (result == true)
    {
        UARTCmd_Response(START_STOP_ADV_CMD, UART_TEST_SUCCESS, NULL, 0);
    }
    else
    {
        UARTCmd_Response(START_STOP_ADV_CMD, UART_TEST_ERROR, NULL, 0);
    }
}

/**
  * @brief Enter HCI uart test mode command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_enter_hci_mode(void *p_data)
{
    DBG_DIRECT("[uart_test_enter_hci_mode] Switch to HCI UART test mode!");

    UARTCmd_Response(ENTER_HCI_TEST_MODE_CMD, UART_TEST_SUCCESS, NULL, 0);

    switch_to_hci_mode();
}

/**
  * @brief Disable test mode flag command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_disable_test_mode_flag(void *p_data)
{
    bool result = false;
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_disable_test_mode_flag] disable test mode flag",
                    0);

    result = mp_test_disable_test_mode_flag();

    if (result == true)
    {
        UARTCmd_Response(DISABLE_TEST_MODE_FLG_CMD, UART_TEST_SUCCESS, NULL, 0);
    }
    else
    {
        UARTCmd_Response(DISABLE_TEST_MODE_FLG_CMD, UART_TEST_ERROR, NULL, 0);
    }
}

/**
  * @brief Enable test mode flag command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_enable_test_mode_flag(void *p_data)
{
    bool result = false;
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_enable_test_mode_flag] enable test mode flag",
                    0);

    result = mp_test_enable_test_mode_flag();

    if (result == true)
    {
        UARTCmd_Response(ENABLE_TEST_MODE_FLG_CMD, UART_TEST_SUCCESS, NULL, 0);
    }
    else
    {
        UARTCmd_Response(ENABLE_TEST_MODE_FLG_CMD, UART_TEST_ERROR, NULL, 0);
    }
}

/**
  * @brief Erase pair info command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_erase_pair_info(void *p_data)
{
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_erase_pair_info] erase pair info", 0);

    app_global_data.is_link_key_existed = false;
    le_bond_clear_all_keys();

    if (le_get_bond_dev_num() == 0)
    {
        UARTCmd_Response(ERASE_PAIR_INFO_CMD, UART_TEST_SUCCESS, NULL, 0);
    }
    else
    {
        UARTCmd_Response(ERASE_PAIR_INFO_CMD, UART_TEST_ERROR, NULL, 0);
    }
}

/**
  * @brief Change baudrate command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_change_baudrate(void *p_data)
{
    bool result = false;
    uint8_t baudrate_index = *((uint8_t *)p_data + 3);

    if ((baudrate_index == CHANGE_BAUDRATE_OPTION_115200) ||
        (baudrate_index == CHANGE_BAUDRATE_OPTION_1M) ||
        (baudrate_index == CHANGE_BAUDRATE_OPTION_2M))
    {
        result = true;
    }

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_change_baudrate] baudrate_index is %d, result is %d", 2, baudrate_index, result);

    if (result == true)
    {
        UARTCmd_Response(CHANGE_BAUDRATE_CMD, UART_TEST_SUCCESS, NULL, 0);
        DataUARTInit(baudrate_index);
    }
    else
    {
        UARTCmd_Response(CHANGE_BAUDRATE_CMD, UART_TEST_ERROR, NULL, 0);
    }
}

/**
  * @brief Terminate connection command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_terminate_connect(void *p_data)
{
    bool result = false;

    result = rcu_terminate_connection(DISCONN_REASON_UART_CMD);

    if (result == true)
    {
        UARTCmd_Response(TERMINATE_CONNECT_CMD, UART_TEST_SUCCESS, NULL, 0);
    }
    else
    {
        UARTCmd_Response(TERMINATE_CONNECT_CMD, UART_TEST_ERROR, NULL, 0);
    }
}

/**
  * @brief Enter HCI uart test mode command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_enter_hci_download_mode(void *p_data)
{
#if MP_TEST_MODE_SUPPORT_DATA_UART_DOWNLOAD
    DBG_DIRECT("[uart_test_enter_hci_download_mode] Switch to HCI UART download mode!");

    flash_set_block_protect_locked(0);
    flash_erase_locked(FLASH_ERASE_SECTOR, get_header_addr_by_img_id(OTA));

    UARTCmd_Response(ENTER_HCI_DOWNLOAD_MODE_CMD, UART_TEST_SUCCESS, NULL, 0);

    WDG_SystemReset(RESET_ALL, UART_CMD_RESET);
#else
    UARTCmd_Response(ENTER_HCI_DOWNLOAD_MODE_CMD, UART_TEST_ERROR, NULL, 0);
#endif
}

#if SUPPORT_CODEC_EQ_CONFIG_FEATURE
/**
  * @brief Set codec eq config command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_set_codec_eq_config(void *p_data)
{
    bool result = false;
    uint8_t eq_channel_index = *((uint8_t *)p_data + 3) - 1;
    uint8_t eq_status = *((uint8_t *)p_data + 4);
    uint32_t eq_coef_H0 = (uint32_t)(*((uint8_t *)p_data + 5))
                          + ((uint32_t)(*((uint8_t *)p_data + 6) << 8))
                          + ((uint32_t)(*((uint8_t *)p_data + 7) << 16))
                          + ((uint32_t)(*((uint8_t *)p_data + 8) << 24));
    uint32_t eq_coef_B1 = (uint32_t)(*((uint8_t *)p_data + 9))
                          + ((uint32_t)(*((uint8_t *)p_data + 10) << 8))
                          + ((uint32_t)(*((uint8_t *)p_data + 11) << 16))
                          + ((uint32_t)(*((uint8_t *)p_data + 12) << 24));
    uint32_t eq_coef_B2 = (uint32_t)(*((uint8_t *)p_data + 13))
                          + ((uint32_t)(*((uint8_t *)p_data + 14) << 8))
                          + ((uint32_t)(*((uint8_t *)p_data + 15) << 16))
                          + ((uint32_t)(*((uint8_t *)p_data + 16) << 24));
    uint32_t eq_coef_A1 = (uint32_t)(*((uint8_t *)p_data + 17))
                          + ((uint32_t)(*((uint8_t *)p_data + 18) << 8))
                          + ((uint32_t)(*((uint8_t *)p_data + 19) << 16))
                          + ((uint32_t)(*((uint8_t *)p_data + 20) << 24));
    uint32_t eq_coef_A2 = (uint32_t)(*((uint8_t *)p_data + 21))
                          + ((uint32_t)(*((uint8_t *)p_data + 22) << 8))
                          + ((uint32_t)(*((uint8_t *)p_data + 23) << 16))
                          + ((uint32_t)(*((uint8_t *)p_data + 24) << 24));

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_set_codec_eq_config] %d %d 0x%X 0x%X 0x%X 0x%X 0x%X", 7,
                    eq_channel_index, eq_status, eq_coef_H0, eq_coef_B1, eq_coef_B1, eq_coef_A1, eq_coef_A2);

    /* check parmester validation */
    if ((eq_channel_index >= CODEC_EQ_MAX_NUM)
        || (eq_status >= UART_TEST_CODEC_EQ_STATUS_ENUM_GUARD))
    {
        result = false;
    }
    else
    {
        if (eq_status == UART_TEST_CODEC_EQ_STATUS_DISABLE)
        {
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQChCmd = EQ_CH_Cmd_DISABLE;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefH0 = 0;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB1 = 0;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB2 = 0;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA1 = 0;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA2 = 0;
        }
        else if (eq_status == UART_TEST_CODEC_EQ_STATUS_ENABLE)
        {
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQChCmd = EQ_CH_Cmd_ENABLE;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefH0 = eq_coef_H0;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB1 = eq_coef_B1;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB2 = eq_coef_B2;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA1 = eq_coef_A1;
            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA2 = eq_coef_A2;
        }
        result = true;
    }

    if (result == true)
    {
        UARTCmd_Response(SET_CODEC_EQ_CONFIG_CMD, UART_TEST_SUCCESS, NULL, 0);
    }
    else
    {
        UARTCmd_Response(SET_CODEC_EQ_CONFIG_CMD, UART_TEST_ERROR, NULL, 0);
    }
}

/**
  * @brief Get codec eq config command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_get_codec_eq_config(void *p_data)
{
    uint8_t eq_channel_index = *((uint8_t *)p_data + 3) - 1;
    uint8_t eq_config_buff[21] = {0};

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_set_codec_eq_config] %d %d %d %d %d %d %d", 7,
                    eq_channel_index,
                    voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQChCmd,
                    voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefH0,
                    voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB1,
                    voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB2,
                    voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA1,
                    voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA2
                   );

    if (voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQChCmd == EQ_CH_Cmd_DISABLE)
    {
        eq_config_buff[0] = 0;
    }
    else if (voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQChCmd ==
             EQ_CH_Cmd_ENABLE)
    {
        eq_config_buff[0] = 1;
    }

    eq_config_buff[1] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefH0);
    eq_config_buff[2] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefH0 >> 8);
    eq_config_buff[3] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefH0 >> 16);
    eq_config_buff[4] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefH0 >> 24);

    eq_config_buff[5] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB1);
    eq_config_buff[6] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB1 >> 8);
    eq_config_buff[7] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB1 >> 16);
    eq_config_buff[8] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB1 >> 24);

    eq_config_buff[9] = (uint8_t)(
                            voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB2);
    eq_config_buff[10] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB2 >> 8);
    eq_config_buff[11] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB2 >> 16);
    eq_config_buff[12] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefB2 >> 24);

    eq_config_buff[13] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA1);
    eq_config_buff[14] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA1 >> 8);
    eq_config_buff[15] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA1 >> 16);
    eq_config_buff[16] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA1 >> 24);

    eq_config_buff[17] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA2);
    eq_config_buff[18] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA2 >> 8);
    eq_config_buff[19] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA2 >> 16);
    eq_config_buff[20] = (uint8_t)(
                             voice_driver_codec_params.codec_eq_params[eq_channel_index].CODEC_EQCoefA2 >> 24);

    UARTCmd_Response(GET_CODEC_EQ_CONFIG_CMD, UART_TEST_SUCCESS, eq_config_buff, 21);
}
#endif

/**
  * @brief Reboot device command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_reboot_device(void *p_data)
{
    DBG_DIRECT("[uart_test_reboot_device] reboot device!");

    UARTCmd_Response(REBOOT_DEVICE_CMD, UART_TEST_SUCCESS, NULL, 0);

    WDG_SystemReset(RESET_ALL, UART_CMD_RESET);
}

/**
  * @brief Enter Single Tone test command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_enter_single_tone(void *p_data)
{
    DBG_DIRECT("[uart_test_enter_single_tone] Switch to Single Tone test mode!");

    UARTCmd_Response(ENTER_SINGLE_TONE_MODE_CMD, UART_TEST_SUCCESS, NULL, 0);

    switch_to_test_mode(SINGLE_TONE_MODE);
}

/**
  * @brief Direct K RF frequency command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_direct_k_rf_freq(void *p_data)
{
#if MP_TEST_MODE_SUPPORT_AUTO_K_RF
    uint8_t cal_xtal_result;
    uint8_t rx_channel = *((uint8_t *)p_data + 3);
    uint8_t drift_threshold = *((uint8_t *)p_data + 4);

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_direct_k_rf_freq] rx_channel is %d, drift_threshold is %d",
                    2, rx_channel, drift_threshold);

    if (Auto_K_RF(rx_channel, drift_threshold, &cal_xtal_result))
    {
        UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,  "[uart_test_direct_k_rf_freq] Cal RF freqency failed", 0);
        UARTCmd_Response(AUTO_K_RF_FREQ_CMD, UART_TEST_ERROR, NULL, 0);
    }
    else
    {
        bool write_xtal_ret;

#ifdef WRITE_XTAL_TO_EFUSE
        write_xtal_ret = WriteXtalToEfuse(cal_xtal_result);
#else
        write_xtal_ret = WriteXtalToConfig(cal_xtal_result);
#endif
        if (write_xtal_ret == true)
        {
            UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,  "[uart_test_direct_k_rf_freq] Cal RF freqency successed",
                            0);
            UARTCmd_Response(DIRECT_K_RF_FREQ_CMD, UART_TEST_SUCCESS, &cal_xtal_result, 1);
        }
        else
        {
            UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,  "[uart_test_direct_k_rf_freq] Cal RF freqency failed", 0);
            UARTCmd_Response(DIRECT_K_RF_FREQ_CMD, UART_TEST_ERROR, NULL, 0);
        }
    }
#endif
}

/**
  * @brief Get DUT information command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_get_dut_info(void *p_data)
{
#if MP_TEST_MODE_SUPPORT_AUTO_K_RF
    bool result = false;
    uint8_t golden_info[32];
    uint8_t dut_info[32];

    memset(dut_info, 0, 32);
    memcpy(golden_info, (uint8_t *)p_data + 3, 32);
    result = dut_info_get(golden_info, dut_info);

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_get_dut_info] result is %d", 1, result);

    if (result == true)
    {
        UARTCmd_Response(GET_DUT_INFO_CMD, UART_TEST_SUCCESS, dut_info, 32);
    }
    else
    {
        UARTCmd_Response(GET_DUT_INFO_CMD, UART_TEST_ERROR, NULL, 0);
    }
#else
    UARTCmd_Response(GET_DUT_INFO_CMD, UART_TEST_ERROR, NULL, 0);
#endif
}

/**
  * @brief Get DUT information command.
  * @param p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_auto_k_rf_freq(void *p_data)
{
#if MP_TEST_MODE_SUPPORT_AUTO_K_RF
    bool result = false;
    uint8_t result_info[16];
    uint8_t cal_xtal_result;

    uint8_t rx_channel = *((uint8_t *)p_data + 19);
    uint8_t drift_threshold = *((uint8_t *)p_data + 20);

    memcpy(result_info, (uint8_t *)p_data + 3, 16);
    result = check_verify_result(result_info);

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                    "[uart_test_auto_k_rf_freq] rx_channel is %d, drift_threshold is %d, check_result is %d",
                    3, rx_channel, drift_threshold, result);

    if (result == true)
    {
        if (Auto_K_RF(rx_channel, drift_threshold, &cal_xtal_result))
        {
            UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,  "[uart_test_auto_k_rf_freq] Cal RF freqency failed", 0);
            UARTCmd_Response(AUTO_K_RF_FREQ_CMD, UART_TEST_ERROR, NULL, 0);
        }
        else
        {
            bool write_xtal_ret;

#ifdef WRITE_XTAL_TO_EFUSE
            write_xtal_ret = WriteXtalToEfuse(cal_xtal_result);
#else
            write_xtal_ret = WriteXtalToConfig(cal_xtal_result);
#endif
            if (write_xtal_ret == true)
            {
                UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,  "[uart_test_auto_k_rf_freq] Cal RF freqency successed", 0);
                UARTCmd_Response(AUTO_K_RF_FREQ_CMD, UART_TEST_SUCCESS, &cal_xtal_result, 1);
            }
            else
            {
                UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,  "[uart_test_auto_k_rf_freq] Cal RF freqency failed", 0);
                UARTCmd_Response(AUTO_K_RF_FREQ_CMD, UART_TEST_ERROR, NULL, 0);
            }
        }
    }
    else
    {
        UARTCmd_Response(AUTO_K_RF_FREQ_CMD, UART_TEST_ERROR, NULL, 0);
    }
#else
    UARTCmd_Response(AUTO_K_RF_FREQ_CMD, UART_TEST_ERROR, NULL, 0);
#endif
}

/**
  * @brief  Find device type command.
  * @param  p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_find_device_type(void *p_data)
{
    uint8_t dev_type = TEST_DEVICE_TYPE;

    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO,  "[uart_test_find_device_type] dev_type is %d", 1,
                    dev_type);
    UARTCmd_Response(FIND_DEVICE_TYPE_CMD, UART_TEST_SUCCESS, &dev_type, 1);
}

/**
  * @brief  Get the spefied uart command.
  * @param  p_data: point to UART packet struct.
  * @retval None.
  */
void uart_test_get_cmd_func(UART_PacketTypeDef *p_data)
{
    uint16_t opcode = (p_data->Buf[2] << 8) + p_data->Buf[1];

    UART_DBG_BUFFER(MODULE_UART, LEVEL_INFO, "[uart_test_get_cmd_func] opcode is 0x%04X", 1, opcode);

    /* uart test command */
    if ((opcode >= UART_FN_BEGIN) && (opcode < UART_FN_END))
    {
        if (uart_test_func_map[opcode & UART_FN_MASK].fncb != NULL)
        {
            uart_test_func_map[opcode & UART_FN_MASK].fncb(p_data);
        }
        else
        {
            UART_DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "No uart test cmd function!", 0);
        }
    }
    else
    {
        //Other command
    }
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/**
  * @brief initialize uart test function.
  * @param None.
  * @retval None.
  */
void uart_test_init(void)
{
    UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "[uart_test_init] initialize uart test mode", 0);
    UartTransport_Init();
    uart_test_is_dlps_allowed = false;
}

/**
  * @brief Uart Test Check DLPS allowed or not.
  * @param voide.
  * @retval true or false.
  */
bool uart_test_check_dlps_allowed(void)
{
    return uart_test_is_dlps_allowed;
}

/**
 * @brief handle UART message
 * @param io_driver_msg_recv - recieved io message
 * @return none
 * @retval void
 */
void uart_test_handle_uart_msg(T_IO_MSG io_driver_msg_recv)
{
    UART_PacketTypeDef *pUartTestPacket = (UART_PacketTypeDef *)(io_driver_msg_recv.u.buf);

    if (Packet_Decode(pUartTestPacket))
    {
        uart_test_get_cmd_func(pUartTestPacket);
    }
}

/**
 * @brief handle GDMA message
 * @param io_driver_msg_recv - recieved io message
 * @return none
 * @retval void
 */
void uart_test_handle_gdma_msg(T_IO_MSG io_driver_msg_recv)
{
    uint8_t blk_cnt, remainder;
    uint8_t *p_buf = io_driver_msg_recv.u.buf;

    blk_cnt = (VOICE_GDMA_FRAME_SIZE) / UART_TX_FIFO_SIZE;
    remainder = (VOICE_GDMA_FRAME_SIZE) % UART_TX_FIFO_SIZE;
    /* send voice data through uart */
    for (int i = 0; i < blk_cnt; i++)
    {
        /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
        UART_SendData(UART, p_buf, 16);
        /* wait tx fifo empty */
        while (UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
        p_buf += 16;
    }

    /* send left bytes */
    UART_SendData(UART, p_buf, remainder);
    /* wait tx fifo empty */
    while (UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
}

#endif

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

