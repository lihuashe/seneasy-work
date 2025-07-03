/********************************************************************************************************
 * @file     main.c
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

#include "app.h"
#include "app_ui.h"
#include "keyboard_rcu.h"
#include "rf_24g_module/rf_24g_driver.h"
#include "rf_24g_module/rf_24g_handle.h"
#include "factory_module/factory_profile.h"
#include "touch_module/ft3308_driver.h"
#include "ir_module/ir_driver.h"
#include "ota_module/ota_handle.h"
#include "touch_module/ft3308_upgrade.h"

/**
 * @brief   IRQ handler
 * @param   none.
 * @return  none.
 */
_attribute_ram_code_ void irq_handler(void)
{
#if (OTA_BLE_ENABLE)
	if (app_func_mode == RF_BLE_MODE) {
		blc_sdk_irq_handler();	
	}
	else if (app_func_mode == RF_24G_MODE)
#endif
	{
#if (TOUCH_ENABLE)
	    touch_irq_handler();
#endif
		rf_24g_irq_handle();
	}

#if (IR_MODULE_ENABLE)
	rc_ir_irq_prc();
#endif
}

/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
#if (PM_DEEPSLEEP_RETENTION_ENABLE)
_attribute_ram_code_sec_noinline_
#endif
int main(void)
{
	#if (BLE_APP_PM_ENABLE)
		#if (SAVE_RAM_CODE_ENABLE)
			blc_pm_select_internal_32k_crystal_save_ram();
		#else
			blc_pm_select_internal_32k_crystal();
		#endif
	#endif
	#if (PM_APP_ENABLE)
		blc_pm_select_internal_32k_crystal();
	#endif

	#if (BLE_OTA_SERVER_ENABLE && (FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_512K))
		blc_ota_setFirmwareSizeAndBootAddress(100, MULTI_BOOT_ADDR_0x40000);
	#endif

	cpu_wakeup_init(EXTERNAL_XTAL_24M);
	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

	clock_init(SYS_CLK_TYPE);
	deepRetWakeUp = 0;
	gpio_init( !deepRetWakeUp );  //analog resistance will keep available in deepSleep mode, so no need initialize again

    /* Check and save wake up source */
    app_ui_check_wakeup_src(deepRetWakeUp);
	//int wakeup_src = app_ui_get_wakeup_src();
	//printf("--->>> [work_debug] wakeup_src: %d\r\n",wakeup_src);

	/* load customized freq_offset CAP value and TP value. */
	blc_app_loadCustomizedParameters();

#if (FACTORY_ENABLE)
   	u8 rcu_mode_flash[2] = {0};
    flash_read_page (CFG_BLE_24G_ADDR, 2, rcu_mode_flash);
	if(rcu_mode_flash[0] == 0x02 && rcu_mode_flash[1] == 0x02)
	{
		app_rcu_flash_unlock_init();//UNLOCK
		rcu_mode_flash[0] = 0x00;
		rcu_mode_flash[1] = 0x00;
		flash_erase_sector(CFG_BLE_24G_ADDR);
		flash_write_page (CFG_BLE_24G_ADDR, 2, rcu_mode_flash);
		//app_rcu_flash_lock_init();  //LOCK
		#if (FACTORY_ENABLE)
		factory_rcu_enter_single_tone();
		#endif
	}
#endif

#if (OTA_BLE_ENABLE)
   	u8 rcu_mode_flash[2] = {0};
    flash_read_page (CFG_BLE_24G_ADDR, 2, rcu_mode_flash);
    if(rcu_mode_flash[0] == 0x01 && rcu_mode_flash[1] == 0x01)
	{
		printf("--->>> [work_debug] app_func_mode: RF_BLE_MODE !!!\r\n");
		app_func_mode = RF_BLE_MODE;

		app_rcu_flash_unlock_init();//UNLOCK
		rcu_mode_flash[0] = 0x00;
		rcu_mode_flash[1] = 0x00;
		flash_erase_sector(CFG_BLE_24G_ADDR);
		flash_write_page (CFG_BLE_24G_ADDR, 2, rcu_mode_flash);
		//app_rcu_flash_lock_init();  //LOCK
	}
    else if(rcu_mode_flash[0] == 0x02 && rcu_mode_flash[1] == 0x02)
	{
		app_rcu_flash_unlock_init();//UNLOCK
		rcu_mode_flash[0] = 0x00;
		rcu_mode_flash[1] = 0x00;
		flash_erase_sector(CFG_BLE_24G_ADDR);
		flash_write_page (CFG_BLE_24G_ADDR, 2, rcu_mode_flash);
		//app_rcu_flash_lock_init();  //LOCK
		#if (FACTORY_ENABLE)
		factory_rcu_enter_single_tone();
		#endif
	}
	else
	{
		//printf("--->>> [work_debug] app_func_mode: RF_24G_MODE !!!\r\n");
		app_func_mode = RF_24G_MODE;
		flash_read_page (CFG_DEVICE_MODE_ADDR, 4, (u8*)&flash_dev_info.dongle_id);
		//printf("--->>> [work_debug] flash_dev_info.dongle_id: 0x%04x\r\n",flash_dev_info.dongle_id);
	}

	if (app_func_mode == RF_BLE_MODE) {

		rf_ble_1m_param_init();
		user_init_normal();
		keyboard_rcu_init();
		#if (OTA_BLE_ENABLE)
			rcu_ble_enter_pair();
		#endif
	}
	else if (app_func_mode == RF_24G_MODE) 
#else
	flash_read_page (CFG_DEVICE_MODE_ADDR, 4, (u8*)&flash_dev_info.dongle_id);
#endif
	{
		if(!deepRetWakeUp)
		{
			pair_flag = analog_read(DEEP_ANA_REG1);
			rf_24g_config();

			if(user_cfg.internal_cap != 0xff || USE_EXTERNAL_CAP)
				blc_app_setExternalCrystalCapEnable(1);
			/* load customized freq_offset CAP value and TP value. */
			blc_app_loadCustomizedParameters();
		}
		rf_24g_driver_init();
		if(!deepRetWakeUp){
			rf_24g_user_init();
		}
		keyboard_rcu_init();
	}

#if (OTA_BLE_ENABLE)
	ota_firmware_erase_sector();
#endif
#if (OTA_DFU_ENABLE)
	ota_fw_init();
#endif
#if (TOUCH_ENABLE)
	fts_fwupg_auto_upgrade();
#endif

#if APP_RCU_FLASH_LOCK_ENABLE
    if(APP_WAKEUP_SRC_DEEP_PAD != app_ui_get_wakeup_src()){
        app_rcu_flash_lock_init();
    }
#endif

	#if (MODULE_WATCHDOG_ENABLE)
		wd_set_interval_ms(WATCHDOG_INIT_TIMEOUT,CLOCK_SYS_CLOCK_1MS);
		wd_start();
	#endif

    irq_enable();

	while (1) {
		#if (MODULE_WATCHDOG_ENABLE)
			wd_clear(); //clear watch dog
		#endif

#if (OTA_BLE_ENABLE)
		if (app_func_mode == RF_BLE_MODE) 
			main_loop ();
		else if (app_func_mode == RF_24G_MODE) 
#endif
			rf_24g_main_loop();
	}
}

