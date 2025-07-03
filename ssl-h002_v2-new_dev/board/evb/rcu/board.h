/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      board.h
* @brief     header file of Keypad demo.
* @details
* @author    chenjie_jin
* @date      2020-02-24
* @version   v1.3
* *********************************************************************************************************
*/


#ifndef _BOARD_H_
#define _BOARD_H_

#include "otp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H_DEMO_RCU              0  /* H Demo RCU */
#define R_DEMO_RCU              1  /* R Demo RCU */
#define G_MIN_DEMO_RCU          2  /* Google Minimal Demo RCU */
#define COMB_DEMO_RCU           3

#define RCU_HD_PLATFORM_SEL     COMB_DEMO_RCU  /* RCU Hardware Platform selection */

/*******************************************************
*                 Log Config
*******************************************************/
#define IS_RELEASE_VERSION                0  /* 1: close log   0: open log */

#define IS_PRINT_REMAINING_RAM_SIZE       1  /* 1: print   0: not print */

/*******************************************************
*                 GAP Parameter Config
*******************************************************/
#if (RCU_HD_PLATFORM_SEL == R_DEMO_RCU)
#define C_DEVICE_NAME           'B','e','e','3',' ','R',' ','R','C','U'
#define C_DEVICE_NAME_LEN       (10+1)  /* sizeof(C_DEVICE_NAME) + 1 */

#elif (RCU_HD_PLATFORM_SEL == COMB_DEMO_RCU)
#define C_DEVICE_NAME           'E','V','E','L','-','E','L','2'
#define C_DEVICE_NAME_LEN       (8+1)  /* sizeof(C_DEVICE_NAME) + 1 */

#endif

/*******************************************************
*                 OTA Config
*******************************************************/
#define SUPPORT_SILENT_OTA          1  /* set 1 to enable silent OTA */
#define SUPPORT_NORMAL_OTA          0  /* set 1 to enable normal OTA */

#define DFU_BUFFER_CHECK_ENABLE     1  /* set 1 to enable buffer check feature */
#define DFU_TEMP_BUFFER_SIZE        2048  /* dfu max buffer size */

/*******************************************************
*                 RCU Feature Config
*******************************************************/
#define FEAUTRE_SUPPORT_FLASH_2_BIT_MODE                1  /* set 1 to enable flash 2-bit mode */

#define FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD             0  /* set 1 to enable multimedia keyboard */

#define FEATURE_SUPPORT_REPORT_MULTIPLE_HID_KEY_CODES   0  /* set 1 to support to report multiple HID key codes */

#define FEATURE_SUPPORT_UNDIRECT_PROMPT_ADV             0  /* set 1 to enable UNDIRECT_PROMPT_ADV */

#define FEATURE_SUPPORT_NO_ACTION_DISCONN               1  /* set 1 to enable NO_ACTION_DISCONN after timeout */

#define FEATURE_SUPPORT_ANY_KEY_TRIG_PAIRING_ADV        0  /* set 1 to enable any key pressed event to trigger paring adv */

#define FEATURE_SUPPORT_POWER_ON_ADV                    0  /* set 1 to enable power on adv */

#define FEATURE_SUPPORT_REMOVE_LINK_KEY_BEFORE_PAIRING  1 /* set 1 to enable remove link key before paring */

#define FEATURE_SUPPORT_RECOVER_PAIR_INFO               0  /* set 1 to enable the function of recovering pairing information */

#define FEATURE_SUPPORT_MP_TEST_MODE                    0  /* set 1 to enable MP test */

#define FEATURE_SUPPORT_PRIVACY                         0  /* set 1 to enable privacy feature */

#define FEATURE_SUPPORT_DATA_LENGTH_EXTENSION           1  /* set 1 to enable data length extension feature */

#define FEATURE_SUPPORT_HIDS_CHAR_AUTHEN_REQ            0  /* set 1 to enable HIDS char authentication request */

#define FEATURE_SUPPORT_KEY_LONG_PRESS_PROTECT          0  /*set 1 to stop scan when press one key too long*/

#define FEATURE_SUPPORT_NO_PAIRING_MODE                 0  /* set 1 to enable no pairing mode feature*/

#define FEATURE_SUPPORT_DEEP_SLEEP_STATE                0  /* set 1 to enable deep sleep state*/
                                                           /* dss模式，开启后可降低休眠功耗 */

/*******************************************************
*                 FTL Address Config
*******************************************************/
/*note: the range of FTL address here should be less than (FTL_REAL_LOGIC_ADDR_SIZE -1K)*/
#define FTL_MP_TEST_PARAMS_BASE_ADDR        0
#define FTL_MP_TEST_PARAMS_MAX_LEN          12

#define FTL_PAIRED_INFO_OFFSET              20
#define FTL_PAIRED_INFO_LEN                 212

#define FTL_DEVICE_SN_ADDR                  240
#define FTL_DEVICE_SN_LEN                   32  

#define FTL_PRODUCT_TEST_ADDR               280
#define FTL_PRODUCT_TEST_LEN                4  
/*******************************************************
*                 MP Test Config
*******************************************************/

#if FEATURE_SUPPORT_MP_TEST_MODE

#define MP_TEST_FTL_PARAMS_TEST_MODE_FLG_OFFSET (FTL_MP_TEST_PARAMS_BASE_ADDR)
#define MP_TEST_FTL_PARAMS_TEST_MODE_FLG_LEN    4

#define MP_TEST_FTL_PARAMS_LOCAL_BD_ADDR_OFFSET (MP_TEST_FTL_PARAMS_TEST_MODE_FLG_OFFSET + MP_TEST_FTL_PARAMS_TEST_MODE_FLG_LEN)
#define MP_TEST_FTL_PARAMS_LOCAL_BD_ADDR_LEN    8

#define MP_TEST_MODE_SUPPORT_HCI_UART_TEST      0  /* set 1 to support HCI Uart Test Mode */
#define MP_TEST_MODE_SUPPORT_DATA_UART_TEST     0  /* set 1 to support Data Uart Test Mode */
#define MP_TEST_MODE_SUPPORT_SINGLE_TONE_TEST   1  /* set 1 to support SingleTone Test Mode */
#define MP_TEST_MODE_SUPPORT_FAST_PAIR_TEST     0  /* set 1 to support Fast Pair Test */
#define MP_TEST_MODE_SUPPORT_AUTO_K_RF          0  /* set 1 to support Auto K RF */
#define MP_TEST_MODE_SUPPORT_DATA_UART_DOWNLOAD 0  /* set 1 to support Data UART download */

#define MP_TEST_MODE_TRIG_BY_GPIO               0x0001  /* GPIO signal while power on to trigger MP test mode */
#define MP_TEST_MODE_TRIG_BY_COMBINE_KEYS       0x0002  /* Combine keys to trigger MP test mode */

#define MP_TEST_MODE_TRIG_SEL                   (MP_TEST_MODE_TRIG_BY_GPIO | MP_TEST_MODE_TRIG_BY_COMBINE_KEYS)

#if (MP_TEST_MODE_TRIG_SEL & MP_TEST_MODE_TRIG_BY_GPIO)

#if (RCU_HD_PLATFORM_SEL == R_DEMO_RCU)
#define MP_TEST_TRIG_PIN_1          P3_2
#define MP_TEST_TRIG_PIN_2          P3_3

#elif (RCU_HD_PLATFORM_SEL == H_DEMO_RCU)
#define MP_TEST_TRIG_PIN_1          P5_1
#define MP_TEST_TRIG_PIN_2          P5_2

#elif (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
#define MP_TEST_TRIG_PIN_1          P3_2
#define MP_TEST_TRIG_PIN_2          P3_3

#elif (RCU_HD_PLATFORM_SEL == COMB_DEMO_RCU)
#define MP_TEST_TRIG_PIN_1          P3_2
#define MP_TEST_TRIG_PIN_2          P3_3

#endif

#endif

#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
#define MP_TEST_UART_TX_PIN         P3_0
#define MP_TEST_UART_RX_PIN         P3_1
#endif

#endif

/*******************************************************
*                 Voice Module Config
*******************************************************/
#define SUPPORT_VOICE_FEATURE               0  /* set 1 to support voice feature */

#if SUPPORT_VOICE_FEATURE

#define SUPPORT_MIC_BIAS_OUTPUT             0  /* set 1 to enable MIC bias output */

#define SUPPORT_UART_DUMP_FEATURE           0  /* set 1 to support UART dump PCM data */

#define SUPPORT_SW_EQ                       1  /* set 1 to support software equalizer */

#if SUPPORT_UART_DUMP_FEATURE
#if (RCU_HD_PLATFORM_SEL == H_DEMO_RCU)
#define VOICE_UART_TEST_TX          P0_1
#else
#define VOICE_UART_TEST_TX          P3_4
#endif
#endif

/* mic type definitions */
#define AMIC_TYPE                   0  /* Analog MIC */
#define DMIC_TYPE                   1  /* digital MIC */

/* amic input type definitions */
#define AMIC_INPUT_TYPE_DIFF        0  /* differential analog input */
#define AMIC_INPUT_TYPE_SINGLE      1  /* single-end analog input */

/* dmic data latch type definitions */
#define DMIC_DATA_LATCH_FALLING_EDGE    0  /* falling clock edge */
#define DMIC_DATA_LATCH_RISING_EDGE     1  /* rising clock edge */

/* codec sample rate type definitions */
#define CODEC_SAMPLE_RATE_8KHz        1  /* 8KHz codec sample rate */
#define CODEC_SAMPLE_RATE_16KHz       2  /* 16KHz codec sample rate */

#define CODEC_SAMPLE_RATE_SEL       CODEC_SAMPLE_RATE_16KHz

#define VOICE_MIC_TYPE             AMIC_TYPE

#if (VOICE_MIC_TYPE == AMIC_TYPE)
#define AMIC_MIC_N_PIN              P2_6  /* MIC_N is fixed to P2_6 */
#define AMIC_MIC_P_PIN              P2_7  /* MIC_P is fixed to P2_7 */
#define AMIC_MIC_BIAS_PIN           H_0   /* MICBIAS is fixed to H_0 */
#define AMIC_INPUT_TYPE_SEL         AMIC_INPUT_TYPE_DIFF
#elif (VOICE_MIC_TYPE == DMIC_TYPE)
#if (RCU_HD_PLATFORM_SEL == H_DEMO_RCU)
#define DMIC_CLK_PIN                P2_6  /* DMIC clock PIN, support PINMUX */
#define DMIC_DATA_PIN               P2_7  /* DMIC data PIN, support PINMUX */
#else
#define DMIC_CLK_PIN                P2_0  /* DMIC clock PIN, support PINMUX */
#define DMIC_DATA_PIN               P2_1  /* DMIC data PIN, support PINMUX */
#endif
#define DMIC0_DATA_LATCH_TYPE       DMIC_DATA_LATCH_FALLING_EDGE
#endif

/* voice flow type, default IFLYTEK_VOICE_FLOW */
#define IFLYTEK_VOICE_FLOW          0
#define HIDS_GOOGLE_VOICE_FLOW      1
#define ATV_GOOGLE_VOICE_FLOW       2
#define RTK_GATT_VOICE_FLOW         3

#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
#define VOICE_FLOW_SEL              ATV_GOOGLE_VOICE_FLOW
#else
#define VOICE_FLOW_SEL              IFLYTEK_VOICE_FLOW
#endif

/* voice encode type */
#define SW_MSBC_ENC                 1  /* software msbc encode */
#define SW_SBC_ENC                  2  /* software sbc encode */
#define SW_IMA_ADPCM_ENC            3  /* software IMA/DVI adpcm encode */

/* voice encode config */
#if ((VOICE_FLOW_SEL == IFLYTEK_VOICE_FLOW) || (VOICE_FLOW_SEL == HIDS_GOOGLE_VOICE_FLOW))
#define VOICE_ENC_TYPE              SW_MSBC_ENC

#elif (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
/* ATV_GOOGLE_VOICE_FLOW must use SW_IMA_ADPCM_ENC */
#define VOICE_ENC_TYPE              SW_IMA_ADPCM_ENC

#elif (VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW)
#define VOICE_ENC_TYPE              SW_SBC_ENC
#endif

/* ATV Google voice config */
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
/* ATV Google voice version type */
#define ATV_VERSION_0_4             1  /* v0.4 */
#define ATV_VERSION_1_0             2  /* v1.0 */

/* ATV Google voice assistant interaction type */
#define ATV_INTERACTION_MODEL_ON_REQUEST            1  /* on request */
#define ATV_INTERACTION_MODEL_PRESS_TO_TALK         2  /* PTT */
#define ATV_INTERACTION_MODEL_HOLD_TO_TALK          3  /* HTT */

#define ATV_VOICE_VERSION           ATV_VERSION_1_0  /* default version is v1.0 */
#define ATV_VOICE_INTERACTION_MODEL ATV_INTERACTION_MODEL_HOLD_TO_TALK  /* default assistant interaction is HTT */
#endif

#endif

/*******************************************************
*                 RCU Keyscan Config
*******************************************************/
/* if set KEYSCAN_FIFO_LIMIT larger than 3, need to caution ghost key issue */
#define KEYSCAN_FIFO_LIMIT    3  /* value range from 1 to 8 */

/* keypad row and column */
#if (RCU_HD_PLATFORM_SEL == R_DEMO_RCU)
#define KEYPAD_ROW_SIZE       5
#define KEYPAD_COLUMN_SIZE    4

#define ROW0                  P4_3
#define ROW1                  P4_2
#define ROW2                  P4_1
#define ROW3                  P4_0
#define ROW4                  P0_6

#define COLUMN0               P3_0
#define COLUMN1               P3_1
#define COLUMN2               P3_2
#define COLUMN3               P3_3




#elif (RCU_HD_PLATFORM_SEL == COMB_DEMO_RCU)
#define KEYPAD_ROW_SIZE       1
#define KEYPAD_COLUMN_SIZE    2

#define KEY_IO0               P1_1
#define KEY_IO1               P0_4


#endif
/*******************************************************
*                 IR Module Config
*******************************************************/
#define SUPPORT_IR_TX_FEATURE               0

#if SUPPORT_IR_TX_FEATURE
#define SUPPORT_IR_LEARN_FEATURE            1

#if (RCU_HD_PLATFORM_SEL == R_DEMO_RCU)
#define IR_SEND_PIN             P2_3

#endif

#if SUPPORT_IR_LEARN_FEATURE

#if (RCU_HD_PLATFORM_SEL == R_DEMO_RCU)
#define IR_LEARN_PIN            P2_2

#elif (RCU_HD_PLATFORM_SEL == COMB_DEMO_RCU)
#define IR_LEARN_PIN            P2_2
#endif

/* IR_LEARN_TRIG_MODE need to be configured according to hardware */
#define IR_LEARN_TRIG_RISING_EDGE   0
#define IR_LEARN_TRIG_FALL_EDGE     1

#if (RCU_HD_PLATFORM_SEL == R_DEMO_RCU)
#define IR_LEARN_TRIG_MODE          IR_LEARN_TRIG_FALL_EDGE
#endif

/*note: FTL_IR_LEARN_MODULE_LEN cannot be less than IR_LEARN_MAX_KEY_NUM * sizeof(T_IR_LEARN_STORAGE_INFO)*/
#define  IR_LEARN_MAX_KEY_NUM       0x03  /* max key number can be storaged */
#define  IR_WAVE_DATA_BASE_ADDR     FTL_IR_LEARN_MODULE_ADDR

#endif

#endif

/*******************************************************
*              LED Module Config
*******************************************************/
#define SUPPORT_LED_INDICATION_FEATURE          1

#if SUPPORT_LED_INDICATION_FEATURE

#if (RCU_HD_PLATFORM_SEL == R_DEMO_RCU)
#define  LED_NUM_MAX    0x01
#define  LED_INDEX(n)   (n<<8)
/*uint16_t, first byte led index, last byte led pin*/
#define  LED_1          (LED_INDEX(0) | P2_4)

#elif (RCU_HD_PLATFORM_SEL == H_DEMO_RCU)
#define  LED_NUM_MAX    0x01
#define  LED_INDEX(n)   (n<<8)
/*uint16_t, first byte led index, last byte led pin*/
#define  LED_1          (LED_INDEX(0) | P1_0)

#elif (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
#define  LED_NUM_MAX    0x01
#define  LED_INDEX(n)   (n<<8)
/*uint16_t, first byte led index, last byte led pin*/
#define  LED_1          (LED_INDEX(0) | P2_4)

#elif (RCU_HD_PLATFORM_SEL == COMB_DEMO_RCU)
#define  LED_NUM_MAX    0x01
#define  LED_INDEX(n)   (n<<8)
/*uint16_t, first byte led index, last byte led pin*/
#define  LED_1          (LED_INDEX(0) | P2_6)

#endif

/* voltage level to trigger LED On action */
#define LED_ON_LEVEL_HIGH   0
#define LED_ON_LEVEL_LOW    1

#define LED_ON_LEVEL_TRIG   LED_ON_LEVEL_HIGH

#endif

/*******************************************************
*              Battery Module Config
*******************************************************/
#define SUPPORT_BAT_DETECT_FEATURE  1  /* set 1 to enable batttery detect feature */

#if SUPPORT_BAT_DETECT_FEATURE

#define SUPPORT_BAT_KEY_PRESS_DETECT_FEATURE 1  /* set 1 to enable key press battery detect feature */

#define SUPPORT_BAT_PERIODIC_DETECT_FEATURE  0  /* set 1 to enable periodic battery detect feature */

/* Note: make sure BAT_ENTER_NORMAL_MODE_THRESHOLD > BAT_ENTER_NORMAL_MODE_THRESHOLD */
#define BAT_ENTER_LOW_POWER_THRESHOLD     3200  /* enter low power mode threshold, unit: mV */
#define BAT_ENTER_NORMAL_MODE_THRESHOLD   3300  /* enter normal mode threshold, unit: mV */
#define BAT_ENTER_OTA_MODE_THRESHOLD      3500  /* enter ota mode threshold, unit: mV */

#define SUPPORT_BAT_LPC_FEATURE     0  /* set 1 to enable LPC */
#if SUPPORT_BAT_LPC_FEATURE
#define BAT_LPC_COMP_VALUE          LPC_1840_mV /* lpc detect bat threshold value */
#endif

#endif

/*******************************************************
*                 DLPS Module Config
*******************************************************/
#define DLPS_EN             1

/** @defgroup IO Driver Config
  * @note user must config it firstly!! Do not change macro names!!
  * @{
  */
/* if use user define dlps enter/dlps exit callback function */
#define USE_USER_DEFINE_DLPS_EXIT_CB    1
#define USE_USER_DEFINE_DLPS_ENTER_CB   1

/* if use any peripherals below, #define it 1 */
#define USE_I2C0_DLPS       0
#define USE_I2C1_DLPS       0
#if (ROM_WATCH_DOG_ENABLE == 1)
#define USE_TIM_DLPS        1 //must be 1 if enable watch dog
#else
#define USE_TIM_DLPS        0
#endif
#define USE_QDECODER_DLPS   0
#define USE_IR_DLPS         1
#define USE_ADC_DLPS        0
#define USE_CTC_DLPS        0
#define USE_SPI0_DLPS       0
#define USE_SPI1_DLPS       0
#define USE_SPI2W_DLPS      0
#define USE_KEYSCAN_DLPS    1
#define USE_GPIO_DLPS       1
#define USE_CODEC_DLPS      0
#define USE_I2S0_DLPS       0
#define USE_ENHTIM_DLPS     0
#define USE_UART0_DLPS      0
#define USE_UART1_DLPS      0

/* do not modify USE_IO_DRIVER_DLPS macro */
#define USE_IO_DRIVER_DLPS  (USE_I2C0_DLPS | USE_I2C1_DLPS | USE_TIM_DLPS | USE_QDECODER_DLPS\
                             | USE_IR_DLPS | USE_ADC_DLPS | USE_CTC_DLPS | USE_SPI0_DLPS\
                             | USE_SPI1_DLPS | USE_SPI2W_DLPS | USE_KEYSCAN_DLPS\
                             | USE_GPIO_DLPS | USE_CODEC_DLPS | USE_I2S0_DLPS\
                             | USE_ENHTIM_DLPS | USE_UART0_DLPS | USE_UART1_DLPS\
                             | USE_USER_DEFINE_DLPS_ENTER_CB\
                             | USE_USER_DEFINE_DLPS_EXIT_CB)

#ifdef __cplusplus
}
#endif

#endif  /* _BOARD_H_ */

