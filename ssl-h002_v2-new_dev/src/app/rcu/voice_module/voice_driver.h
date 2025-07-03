/**
*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     voice_driver.h
* @brief    This is the header file of user code which the voice driver resides in.
* @details
* @author   chenjie jin
* @date     2018-05-03
* @version  v1.1
*********************************************************************************************************
*/

#ifndef _VOICE_DRIVER_H_
#define _VOICE_DRIVER_H_

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "stdint.h"
#include "stdbool.h"
#include "board.h"
#include "rtl876x_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Macros
 *============================================================================*/
/*voice config*/
#if (VOICE_ENC_TYPE == SW_MSBC_ENC)
#define VOICE_PCM_FRAME_SIZE 240
#define VOICE_PCM_FRAME_CNT 2
#define VOICE_GDMA_FRAME_SIZE (VOICE_PCM_FRAME_SIZE * VOICE_PCM_FRAME_CNT)
#define VOICE_FRAME_SIZE_AFTER_ENC 57
#define VOICE_REPORT_FRAME_SIZE ((VOICE_FRAME_SIZE_AFTER_ENC + 3) * VOICE_PCM_FRAME_CNT)  /* 3 bytes header per frame */

#elif (VOICE_ENC_TYPE == SW_SBC_ENC)
#define VOICE_PCM_FRAME_CNT              2
#define VOICE_PCM_FRAME_SIZE             256
#define VOICE_FRAME_SIZE_AFTER_ENC       (2 * BIT_POOL_SIZE + 8)
#define VOICE_REPORT_FRAME_SIZE          (VOICE_FRAME_SIZE_AFTER_ENC * VOICE_PCM_FRAME_CNT)
#define VOICE_GDMA_FRAME_SIZE            (VOICE_PCM_FRAME_SIZE * VOICE_PCM_FRAME_CNT)
#define BIT_POOL_SIZE                    14

#elif (VOICE_ENC_TYPE == SW_IMA_ADPCM_ENC)
#define VOICE_PCM_FRAME_CNT              1
#define VOICE_PCM_FRAME_SIZE             512
#define VOICE_FRAME_SIZE_AFTER_ENC       128
#define VOICE_FRAME_HEADER_SIZE          6
#define VOICE_REPORT_FRAME_SIZE          (VOICE_FRAME_SIZE_AFTER_ENC * VOICE_PCM_FRAME_CNT + VOICE_FRAME_HEADER_SIZE)
#define VOICE_GDMA_FRAME_SIZE            (VOICE_PCM_FRAME_SIZE * VOICE_PCM_FRAME_CNT)

#endif

/* Voice GDMA definitions */
#define VOICE_GDMA_Channel             GDMA_Channel0
#define VOICE_GDMA_Channel_NUM         0
#define VOICE_GDMA_Channel_IRQn        GDMA0_Channel0_IRQn
#define VOICE_GDMA_Channel_Handler     GDMA0_Channel0_Handler

/*============================================================================*
 *                         Types
 *============================================================================*/
/**
 * @brief Loop queue data struct
 */
typedef struct
{
    uint8_t buf0[VOICE_GDMA_FRAME_SIZE];
    uint8_t buf1[VOICE_GDMA_FRAME_SIZE];
} T_GDMA_BUF_TYPE_DEF __attribute__((aligned(4)));

typedef struct
{
    bool is_allowed_to_enter_dlps;  /* to indicate whether to allow to enter dlps or not */
    bool is_voice_driver_working;  /* to indicate whether voice driver is working or not */

    uint8_t current_bibuff_index;  /* to indicate which buffer the voice using now */
    T_GDMA_BUF_TYPE_DEF gdma_buffer;  /* to indicate the buffer used by voice gdma */
} T_VOICE_DRIVER_GLOBAL_DATA;

typedef struct
{
    /* basic parameters section */
    bool is_mic_bias_output_en;  /* to indicate whether to enable mic bias voltage or not */
    uint32_t mic_bias_voltage;  /* to inidiate the MICBIAS voltage */
    uint32_t codec_sample_rate;  /* to indicate the codec sample rate */
    uint32_t amic_bst_gain;  /* to indicate the MICBST gain */
    uint32_t amic_bst_mode;  /* to indicte the MICBST mode */
    uint32_t dmic_clock;  /* to indicate the dmic clock */
    uint32_t codec_i2s_format;  /* to indicate the I2S format of codec part */
    uint32_t codec_i2s_data_width;  /* to indicate the I2S data width of codec part */
    uint32_t codec_i2s_ch_sequence;  /* to indicate the I2S channel sequence */

    /* MIC channel 0 initialization parameters section */
    uint32_t codec_ch0_mute;  /* to indicate the mute state of codec channel 0 */
    uint32_t codec_ch0_mic_type;  /* to indicate the mic type of codec channel 0 */
    uint32_t codec_ch0_dmic_data_latch;  /* to indicate the dmic data latch type of codec channel 0 */
    uint32_t codec_ch0_ad_gain;  /* to indicate the ADC digital volume of codec channel 0 */
    uint32_t codec_ch0_boost_gain;  /* to indicate the boost gain of codec channel 0 */
    uint32_t codec_ch0_zero_det_timeout;  /* to indicate the zero detection timeout of channel 0 */
} T_VOICE_DRIVER_CODEC_PARAMS;

/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern T_VOICE_DRIVER_GLOBAL_DATA voice_driver_global_data;
extern T_VOICE_DRIVER_CODEC_PARAMS voice_driver_codec_params;

/*============================================================================*
 *                         Functions
 *============================================================================*/
void voice_driver_init_data(void);
void voice_driver_init_codec_params(void);
void voice_driver_init(void);
void voice_driver_deinit(void);
bool voice_driver_dlps_check(void);

#ifdef __cplusplus
}
#endif

#endif
