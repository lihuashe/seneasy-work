/********************************************************************************************************
 * @file     app_ui.c
 *
 * @brief    This is the source file for BLE SDK
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

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "application/application.h"

#include "app.h"
#include "rf_ble_module/app_att.h"
#include "app_ui.h"





/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_ENTER"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void app_set_kb_wakeup(u8 e, u8 *p, int n)
{
	#if (BLE_APP_PM_ENABLE)
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN
		&& ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 *CLOCK_16M_SYS_TIMER_CLK_1MS ){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio CORE wakeup suspend
	}
	#endif
}


/*----------------------------------------------------------------------------*/
/*------------- OTA  Function                                 ----------------*/
/*----------------------------------------------------------------------------*/
#if (BLE_OTA_SERVER_ENABLE)

_attribute_data_retention_ int 	ota_is_working = 0;

/**
 * @brief      this function is used to register the function for OTA start.
 * @param[in]  none
 * @return     none
 */
void app_enter_ota_mode(void)
{
	printf("app_enter_ota_mode --- >>> \r\n");
#if APP_RCU_FLASH_LOCK_ENABLE
    u8 try_count = 10;
    while (try_count--)
    {
        if(bls_ll_requestConnBrxEventDisable() > 5)
        {
            bls_ll_disableConnBrxEvent();
            app_rcu_flash_unlock_init();
            bls_ll_restoreConnBrxEvent();
        }
        sleep_ms(5);
    }
    sleep_ms(5);
#endif 
    bls_ota_setTimeout(150 * 1000 * 1000); //set OTA timeout  150 seconds
    bls_pm_setManualLatency(0);
	ota_is_working = 1;
}


/**
 * @brief       no matter whether the OTA result is successful or fail.
 *              code will run here to tell user the OTA result.
 * @param[in]   result    OTA result:success or fail(different reason)
 * @return      none
 */
void app_ota_result(int result)
{
	printf("app_ota_result: %d\r\n",result);
	#if (0)  //this is only for debug, should disable in mass production code
		if(result == OTA_SUCCESS){  //OTA success
			gpio_write(GPIO_LED_BLUE, 0);
			sleep_us(1000000);  //led off for 1 second
			gpio_write(GPIO_LED_BLUE, 1);
			sleep_us(1000000);  //led on for 1 second
		}
		else{  //OTA fail

		#if 0 //this is only for debug, can not use this in application code
			irq_disable();

			while(1)
			{
				gpio_toggle(GPIO_LED_BLUE);
				sleep_us(1000000);  //led on for 1 second
			}
		#endif

		}
	#endif
}
#endif

/* Wake up source related functions */
_attribute_data_retention_ static u8 s_app_ui_wakeup_src = APP_WAKEUP_SRC_NONE;
static void app_ui_set_wakeup_src(APP_UI_WAKEUP_SRC_T wakeup_src){

    if(wakeup_src >= APP_WAKEUP_SRC_NOT_USE) return;
    s_app_ui_wakeup_src = wakeup_src;

    printf("wakeup source: {%s} !!!! \n",
                (APP_WAKEUP_SRC_POWER_ON==s_app_ui_wakeup_src)?"POWER_ON":
                (APP_WAKEUP_SRC_DEEP_PAD==s_app_ui_wakeup_src)?"DEEP_PAD":
                ((APP_WAKEUP_SRC_DEEP_TIMER==s_app_ui_wakeup_src)?"DEEP_TIMER":"SOFT_RESET"));
}

APP_UI_WAKEUP_SRC_T app_ui_get_wakeup_src(void){

    return s_app_ui_wakeup_src;
}

void app_ui_check_wakeup_src(u8 deepRetWakeUp){
    if((analog_read(USED_DEEP_ANA_REG) & POWER_ON_FLG) == 0){
        app_ui_set_wakeup_src(APP_WAKEUP_SRC_POWER_ON);
    }else if(!deepRetWakeUp){

        u8 my_power_flags = analog_read(0X44);
        //printf(" my_power_flags: 0x%x \n", my_power_flags);
        if(WAKEUP_STATUS_PAD == (my_power_flags & (WAKEUP_STATUS_TIMER|WAKEUP_STATUS_PAD))){
            app_ui_set_wakeup_src(APP_WAKEUP_SRC_DEEP_PAD);
        }else if(my_power_flags & APP_WAKEUP_SRC_DEEP_TIMER){
            app_ui_set_wakeup_src(APP_WAKEUP_SRC_DEEP_TIMER);
        }else{
            app_ui_set_wakeup_src(APP_WAKEUP_SRC_SOFT_RESET);
        }
    }

    /* Keep the POWER_ON_FLG bit as 1. This bit will be cleared when power on */
    analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) | POWER_ON_FLG);
}
