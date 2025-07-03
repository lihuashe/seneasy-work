/**
*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     voice.h
* @brief    This is the header file of user code which the voice module resides in.
* @details
* @author   chenjie jin
* @date     2018-05-03
* @version  v1.1
*********************************************************************************************************
*/

#ifndef _VOICE_HANDLE_H_
#define _VOICE_HANDLE_H_

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "stdint.h"
#include "os_mem.h"
#include "rtl876x_gpio.h"
#include "board.h"
#include "swtimer.h"
#include "atvv_service.h"

#ifdef __cplusplus
extern "C" {
#endif
#if SUPPORT_VOICE_FEATURE
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define VOICE_QUEUE_MAX_BUFFER_SIZE (6 * 1024)  /* unit: Byte */
#define VOICE_TRANSFER_TIMEOUT 30000  /* 30 sec */
#define VOICE_ACTIVE_REMOTE_TIMEOUT 60000  /* 60 sec */
#define VOICE_GDMA_INT_SKIP_CNT 10  /* skip first several GDMA interrupts, to wait codec stable */

#define VOICE_AUDIO_SYNC_PERIOD  20  /* ATV audio sync period in packet munber */

/*============================================================================*
 *                         Types
 *============================================================================*/
/*voice msg type*/
typedef enum
{
    VOICE_MSG_INVALID              = 0,
    VOICE_MSG_BT_SEND_COMPLETE     = 1,
    VOICE_MSG_BT_WRITE_CMD         = 2,
    VOICE_MSG_PERIPHERAL_GDMA      = 3,
} T_VOICE_MSG_TYPE;

#if (VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW)
typedef enum
{
    VOICE_PACKET_TYPE_VOICE_CTRL = 0,
    VOICE_PACKET_TYPE_VOICE_DATA,
} T_VOICE_GATT_PACKET_TYPE;

typedef enum
{
    GATT_VOICE_STOP = 0,
    GATT_VOICE_START = 1
} T_VOICE_GATT_CTRL_CMD;

typedef enum
{
    VOICE_GATT_CODEC_SBC = 0,
    VOICE_GATT_CODEC_INVALID = 1
} T_VOICE_GATT_CODEC_TYPE;
#endif

typedef struct
{
    bool is_allowed_to_notify_voice_data;  /* to indicate whether is allowed to notify voice data or not */
    bool is_pending_to_stop_recording;   /* to indicate whether is pending to stop recording */
    bool is_encoder_init;  /* to indicate whether encoder is initialized or not */
    uint32_t voice_gdma_int_cnt;  /* to indicate the count of voice GDMA interrupt */
} T_VOICE_GLOBAL_DATA;

/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern T_VOICE_GLOBAL_DATA voice_global_data;
#if (VOICE_ENC_TYPE == SW_MSBC_ENC)
extern void msbc_init(void);
extern void msbc_deinit(void);
extern int msbc_encode(void *input, int input_len, void *output, int output_len, int *written);
#endif
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
extern TimerHandle_t voice_exception_timer;
#endif

/*============================================================================*
 *                         Functions
 *============================================================================*/
bool voice_handle_mic_key_pressed(void);
void voice_handle_mic_key_released(void);
bool voice_handle_messages(T_VOICE_MSG_TYPE msg_type, void *p_data);
bool voice_handle_start_mic(void);
void voice_handle_stop_mic(void);
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
bool voice_handle_atvv_audio_start(ATV_AUDIO_START_REASON reason);
bool voice_handle_atvv_audio_stop(ATV_AUDIO_STOP_REASON reason);
bool voice_handle_atvv_mic_open_error(ATV_MIC_OPEN_ERROR reason);
void voice_handle_atvv_init_data(void);
void voice_handle_atv_audio_sync(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif
