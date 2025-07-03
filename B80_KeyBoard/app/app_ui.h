/********************************************************************************************************
 * @file     app_ui.h
 *
 * @brief    This is the header file for BLE SDK
 *
 * @author	 BLE GROUP
 * @date         12,2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#ifndef APP_UI_H_
#define APP_UI_H_



extern int	button_detect_en;
extern u32	button_detect_tick;
extern int	button_not_released;
extern int 	ota_is_working;



/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_ENTER"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void app_set_kb_wakeup(u8 e, u8 *p, int n);


/**
 * @brief		this function is used to detect if button pressed or released.
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void proc_button (u8 e, u8 *p, int n);


/**
 * @brief      this function is used to register the function for OTA start.
 * @param[in]  none
 * @return     none
 */
void app_enter_ota_mode(void);


/**
 * @brief       no matter whether the OTA result is successful or fail.
 *              code will run here to tell user the OTA result.
 * @param[in]   result    OTA result:success or fail(different reason)
 * @return      none
 */
void app_ota_result(int result);

typedef enum {

    APP_WAKEUP_SRC_NONE = 0,        // invalid value
    APP_WAKEUP_SRC_POWER_ON,        // Triggered by inserting battery
    APP_WAKEUP_SRC_DEEP_PAD,        // Under normal circumstances, after entering deep, it is triggered by the pad
    APP_WAKEUP_SRC_DEEP_TIMER,      // Under normal circumstances, after entering deep, it is triggered by the timer
    APP_WAKEUP_SRC_SOFT_RESET,
    //    APP_WAKEUP_SRC_DEEP_RETENTION,    // Generally, after entering deep retention, it is triggered by timer, and it may also be triggered by key press
    APP_WAKEUP_SRC_NOT_USE,
}APP_UI_WAKEUP_SRC_T;

void app_ui_check_wakeup_src(u8 deepRetWakeUp);
APP_UI_WAKEUP_SRC_T app_ui_get_wakeup_src(void);

#endif /* APP_UI_H_ */
