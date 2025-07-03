/**
 ****************************************************************************************
 *
 * @file shell.h
 *
 * @brief Simple CLI shell header
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef AUDIO_TEST_H_
#define AUDIO_TEST_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "peripheral.h"
#include "board_config.h"

#if (BLE_AUDIO_ENABLE)
extern unsigned char audio_encode_data[];
extern unsigned char audio_pcm_data[];

void ui_enable_mic (int en, unsigned char ctl_type);
void app_audio_open(void);
void app_audio_close(void);
void audio_raw_data_in(uint8_t *pdata, uint16_t len);
void app_audio_init(void);
void disconnect_mic_close(void);
#endif


#endif // SHELL_H_
