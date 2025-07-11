/******************************************************************************
 * @file     app_audio.h
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

#ifndef APP_AUDIO_H_
#define APP_AUDIO_H_

#define APP_AUDIO_BT_OPEN    0x01
#define APP_AUDIO_BT_CLOSE   0x00
#define APP_AUDIO_BT_CONFIG  0x02

extern    unsigned int         key_voice_pressTick;
extern    unsigned char        ui_mic_enable;
extern    unsigned char        key_voice_press;

void dmic_gpio_reset (void);
void amic_gpio_reset (void);
void ui_enable_mic (int en);
void voice_press_proc(void);
void task_audio (void);
void blc_checkConnParamUpdate(void);
void proc_audio(void);
int server2client_auido_proc(void* p);
void audio_state_check(void);
void voice_key_release(void);
void voice_key_press(void);


#endif /* APP_AUDIO_H_ */
