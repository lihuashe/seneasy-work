/*******************************************************************************************
*                    Copyright (c) 2021, XIHAO Technology Co., Ltd.
* @file        app_atv.h
* @author      Reborn    
* @version     V1.0    
* @date        2020-12-30
* @brief       atv 语音操作文件
*********************************************************************************************/

#ifndef __APP_ATV_H
#define __APP_ATV_H
 
#include "board_config.h"


#define atv_ntf_audio(dat,len)		atv_ntf_data(ATVVS_IDX_AUDIO_VAL,dat,len)
#define atv_ntf_cmd(dat,len)		atv_ntf_data(ATVVS_IDX_CONTROL_VAL,dat,len)
#define atv_ntf_key(dat,len)		hid_gatt_report_notify(CONSUMER_HANDLE, dat, len)

enum {
    ATVV_VOICE_START = 0,
    ATVV_VOICE_STOP = 1,
};

extern unsigned char atvv_transmit_flag;

void google_voice_start(void);
void google_key_release(void);
void atv_ui_enable_mic (int en);
void atv_disconnect_mic_close(void);
void atvv_char_tx_handle(unsigned char* dat, int len);
void atv_audio_init(void);


#endif //__APP_ATV_H

