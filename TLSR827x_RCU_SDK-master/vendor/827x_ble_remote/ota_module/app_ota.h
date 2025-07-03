/******************************************************************************
 * @file     app_ota.h
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

#ifndef APP_OTA_H_
#define APP_OTA_H_

#if (GOOGLE_OTA)

#define CMD_REPORT_VERSION                    0xfe00
#define CMD_CONTROL                           0xfe01
#define CMD_OTA_RESULT                        0xfe02

#define OTA_PROTECT_VOLTAGE                   2300    //mv


#define CLIENT_REPORT_CMD                     0xfe

//report type
enum{
    TYPE_VERSION = 0,
    TYPE_SINATURE_DATA,
    TYPE_CONTROL_DATA,
    TYPE_OTA_RESULT,
};

// control data
enum{
    OTA_CTRL_PAUSE = 0,
    OTA_CTRL_CONTINUE,
};

typedef enum{
    IDLE_MODE = 0,
    OTA_VER_CHECK,    //Includes battery voltage detection
    OTA_AUTHENTICATION,
    OTA_PROCESS,
}ota_status_type_t;

void bls_ota_init_handle(u16 handle);

ota_status_type_t app_get_ota_status();

void app_ota_set_en_ecdsa(u8 en);

void bls_ota_set_random(u8 *random);
void bls_ota_get_random(u8 *random);
char app_ota_pause();
char app_ota_continue();

void app_ota_terminate(u8 reason);

int app_ota_write(void * p);
void app_ota_proc(void);

//extern u16 ota_ctl_flag;

#endif//#if (GOOGLE_OTA)


#endif /* APP_OTA_H_ */
