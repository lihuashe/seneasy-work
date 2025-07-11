/********************************************************************************************************
 * @file     app.c
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
#include "rf_ble_module/app_att.h"
#include "app_ui.h"
#include "rf_ble_module/app_buffer.h"

#include "key_module/key_driver.h"
#include "keyboard_rcu.h"
#include "ir_module/ir_driver.h"
#include "charge_module/lp4081b_driver.h"

#define 	ADV_IDLE_ENTER_DEEP_TIME			60  //60 s
#define 	CONN_IDLE_ENTER_DEEP_TIME			60  //60 s


#define MY_APP_ADV_CHANNEL			BLT_ENABLE_ADV_ALL
#define MY_ADV_INTERVAL_MIN			ADV_INTERVAL_30MS
#define MY_ADV_INTERVAL_MAX			ADV_INTERVAL_35MS

#define MY_RF_POWER_INDEX			RF_POWER_P2p87dBm

#define MY_DIRECT_ADV_TMIE			10000000

#define	BLE_DEVICE_ADDRESS_TYPE 	BLE_DEVICE_ADDRESS_PUBLIC

u32	advertise_begin_tick;

#if (PM_DEEPSLEEP_ENABLE)
int device_in_connection_state;
u8  sendTerminate_before_enterDeep = 0;
u32 latest_user_event_tick;
#endif

#if (BATT_CHECK_ENABLE)
u32	lowBattDet_tick   = 0;
#endif

own_addr_type_t app_own_address_type = OWN_ADDRESS_PUBLIC;

/**
 * @brief	BLE Advertising data
 */
const u8	tbl_advData[] = {
	 12, DT_COMPLETE_LOCAL_NAME, 				'S', 'Y', '-', 'K', 'e', 'y', 'B', 'o', 'a', 'r', 'd',
	 2,	 DT_FLAGS, 								0x05, 					// BLE limited discoverable mode and BR/EDR not supported
	 3,  DT_APPEARANCE, 						0x80, 0x01, 			// 384, Generic Remote Control, Generic category
	 5,  DT_INCOMPLT_LIST_16BIT_SERVICE_UUID,	0x12, 0x18, 0x0F, 0x18,	// incomplete list of service class UUIDs (0x1812, 0x180F)
};

/**
 * @brief	BLE Scan Response Packet data
 */
const u8	tbl_scanRsp [] = {
	 12,  DT_COMPLETE_LOCAL_NAME, 				'S', 'Y', '-', 'K', 'e', 'y', 'B', 'o', 'a', 'r', 'd',
};


/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_ADV_DURATION_TIMEOUT"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void app_switch_to_indirect_adv(u8 e, u8 *p, int n)
{
	bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
						ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
						0,  NULL,
						MY_APP_ADV_CHANNEL,
						ADV_FP_NONE);

	bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //must: set adv enable
}



/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_CONNECT"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void task_connect(u8 e, u8 *p, int n)
{

	bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 99, CONN_TIMEOUT_4S);  // 1 S

	#if (PM_DEEPSLEEP_ENABLE)
		latest_user_event_tick = clock_time();
		device_in_connection_state = 1;
	#endif

	#if (UI_LED_ENABLE)
		gpio_write(GPIO_LED_1, LED_ON_LEVAL);
	#endif

#if (OTA_BLE_ENABLE)
	printf("--->>> [work_debug] task_connect !!!\r\n");
	rcu_ble_pair_flag = 0x00;
	app_led_blink_timer = 0;
	gpio_write(GPIO_LED_1,LED_ON_LEVAL);
	app_led_on_timer = clock_time() | 1;
	lowBattDet_tick = clock_time();
#endif
}


/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_TERMINATE"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void task_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    printf("ble_remote_terminate  bond_number=%1x,*p=%1x\r\n",bond_number,*p);

	#if (PM_DEEPSLEEP_ENABLE)
		device_in_connection_state = 0;
	#endif

	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}
	else{

	}

	#if (PM_DEEPSLEEP_ENABLE)
		 //user has push terminate pkt to ble TX buffer before deepsleep
		if(sendTerminate_before_enterDeep == 1){
			sendTerminate_before_enterDeep = 2;
		}
	#endif


	#if (UI_LED_ENABLE)
		gpio_write(GPIO_LED_1, !LED_ON_LEVAL);  //light off
	#endif

	advertise_begin_tick = clock_time();

	printf("--->>> 2.4G start_reboot!\r\n");
	start_reboot(); 
}


/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_EXIT"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void task_suspend_exit (u8 e, u8 *p, int n)
{
	rf_set_power_level_index (MY_RF_POWER_INDEX);
}


/**
 * @brief      power management code for application
 * @param[in]  none
 * @return     none
 */
void blt_pm_proc(void)
{
	#if(BLE_APP_PM_ENABLE)
		#if (PM_DEEPSLEEP_RETENTION_ENABLE)
			bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
		#else
			bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
		#endif
		#if (BLE_OTA_SERVER_ENABLE)
			if(ota_is_working)
			{
				bls_pm_setSuspendMask(SUSPEND_DISABLE);
			}
		#endif
		#if(UI_KEYBOARD_ENABLE)
			if(scan_pin_need || key_not_released )
			{
				bls_pm_setManualLatency(0);
			}
		#endif


		#if (PM_DEEPSLEEP_ENABLE)   //test connection power, should disable deepSleep
			if(sendTerminate_before_enterDeep == 2){  //Terminate OK
				cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepSleep
			}


			if(  !blc_ll_isControllerEventPending() ){  //no controller event pending
				//adv 60s, deepsleep
				if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && !sendTerminate_before_enterDeep && \
						clock_time_exceed(advertise_begin_tick , ADV_IDLE_ENTER_DEEP_TIME * 1000000))
				{
					cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
				}
				//conn 60s no event(key/voice/led), enter deepsleep
				else if( device_in_connection_state && \
						clock_time_exceed(latest_user_event_tick, CONN_IDLE_ENTER_DEEP_TIME * 1000000) )
				{

					bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
					bls_ll_setAdvEnable(0);   //disable adv
					sendTerminate_before_enterDeep = 1;
				}
			}
		#endif  //end of PM_DEEPSLEEP_ENABLE
	#endif//END of  BLE_APP_PM_ENABLE
}


/**
 * @brief		user initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return      none
 */
void user_init_normal(void)
{
	/* random number generator must be initiated here( in the beginning of user_init_nromal).
	 * When deepSleep retention wakeUp, no need initialize again */
	random_generator_init();  //this is must


	/*****************************************************************************************
	 Note: battery check must do before any flash write/erase operation, cause flash write/erase
		   under a low or unstable power supply will lead to error flash operation

		   Some module initialization may involve flash write/erase, include: OTA initialization,
				SMP initialization, ..
				So these initialization must be done after  battery check
	*****************************************************************************************/
	#if (BATT_CHECK_ENABLE)  //battery check must do before OTA relative operation
		u8 battery_check_returnVaule = 0;
		if(analog_read(USED_DEEP_ANA_REG) & LOW_BATT_FLG){
			battery_check_returnVaule = app_battery_power_check(VBAT_ALRAM_THRES_MV + 200);  //2.2 V
		}
		else{
			battery_check_returnVaule = app_battery_power_check(VBAT_ALRAM_THRES_MV);  //2.0 V
		}
		if(battery_check_returnVaule){
			analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG)&(~LOW_BATT_FLG));  //clr
		}
		else{
			#if (UI_LED_ENABLE)  //led indicate
				for(int k=0;k<3;k++){
					gpio_write(GPIO_LED_1, LED_ON_LEVAL);
					sleep_us(200000);
					gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
					sleep_us(200000);
				}
			#endif
			analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG) | LOW_BATT_FLG);  //mark
			GPIO_WAKEUP_FEATURE_LOW;

			cpu_set_gpio_wakeup (GPIO_WAKEUP_FEATURE, Level_High, 1);  //drive pin pad high wakeup deepsleep

			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
		}
	#endif

	//////////////////////////// BLE stack Initialization  Begin //////////////////////////////////
	u8 mac_public[6];
	u8 mac_random_static[6];
	blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);


	#if(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_PUBLIC)
		app_own_address_type = OWN_ADDRESS_PUBLIC;
	#elif(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_RANDOM_STATIC)
		app_own_address_type = OWN_ADDRESS_RANDOM;
		blc_ll_setRandomAddr(mac_random_static);
	#endif

	//////////// LinkLayer Initialization  Begin /////////////////////////



	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(mac_public);				//mandatory
	blc_ll_initAdvertising_module();//adv module: mandatory for BLE slave,
	blc_ll_initSlaveRole_module();//slave module: mandatory for BLE slave,

	blc_ll_setAclConnMaxOctetsNumber(ACL_CONN_MAX_RX_OCTETS, ACL_CONN_MAX_TX_OCTETS);
	blc_ll_initAclConnTxFifo(app_acl_txfifo, ACL_TX_FIFO_SIZE, ACL_TX_FIFO_NUM);
	blc_ll_initAclConnRxFifo(app_acl_rxfifo, ACL_RX_FIFO_SIZE, ACL_RX_FIFO_NUM);

	u8 check_status = blc_controller_check_appBufferInitialization();
	if(check_status != BLE_SUCCESS){
		/* here user should set some log to know which application buffer incorrect */

		while(1);
	}
	//////////// LinkLayer Initialization  End /////////////////////////



	//////////// HCI Initialization  Begin /////////////////////////

	//////////// HCI Initialization  End /////////////////////////


	//////////// Controller Initialization  End /////////////////////////




	//////////// Host Initialization  Begin /////////////////////////
	/* GAP initialization must be done before any other host feature initialization !!! */
	blc_gap_peripheral_init();    //gap initialization

	/* GATT Initialization */
	my_gatt_init();


	/* L2CAP Initialization */
	blc_l2cap_register_handler(blc_l2cap_packet_receive);
	blc_l2cap_initRxDataBuffer(app_l2cap_rx_fifo, L2CAP_RX_BUFF_SIZE);

	/* SMP Initialization */
	/* SMP Initialization may involve flash write/erase(when one sector stores too much information,
	 *   is about to exceed the sector threshold, this sector must be erased, and all useful information
	 *   should re_stored) , so it must be done after battery check */
	#if (APP_SECURITY_ENABLE)
		bls_smp_configpairingSecurityInfoStorageAddr(FLASH_ADR_SMP_PAIRING);
		blc_smp_param_setBondingDeviceMaxNumber(4);  	//default is 4, can not bigger than this value
												    	//and this func must call before bls_smp_enableParing
		blc_smp_peripheral_init();
		blc_smp_setSecurityLevel(Unauthenticated_Pairing_with_Encryption);

	#else
		blc_smp_setSecurityLevel(No_Security);
	#endif


	//////////// Host Initialization  End /////////////////////////

//////////////////////////// BLE stack Initialization  End //////////////////////////////////





//////////////////////////// User Configuration for BLE application ////////////////////////////
	#if(APP_SECURITY_ENABLE)
		u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber(); 		//get bonded device number
		smp_param_save_t  bondInfo;
		if(bond_number)   //at least 1 bonding device exist
		{
			//get the latest bonding device (index: bond_number-1 )
			bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);
		}
		printf("--->>> [work_debug] bond_number: %d\r\n",bond_number);

		if(bond_number)//set direct adv
		{
			//set direct adv
			u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
											ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
											bondInfo.peer_addr_type,  bondInfo.peer_addr,
											MY_APP_ADV_CHANNEL,
											ADV_FP_NONE);
			//debug: ADV setting err
			if(status != BLE_SUCCESS) { while(1); }

			//it is recommended that direct adv only last for several seconds, then switch to indirect adv
			bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE, 1);
			//bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &app_switch_to_indirect_adv);
		
			bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
			bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

			bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //adv enable
		}
		else//set indirect ADV
	#endif
		{
			/*
			u8 status = bls_ll_setAdvParam(  MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
											 ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
											 0,  NULL,
											 MY_APP_ADV_CHANNEL,
											 ADV_FP_NONE);
			if(status != BLE_SUCCESS) { 	while(1); }
			*/
		}

	/* set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend */
	rf_set_power_level_index (MY_RF_POWER_INDEX);


	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &task_suspend_exit);



	#if(BLE_OTA_SERVER_ENABLE)
		/* OTA module initialization must be called after "blc_ota_setNewFirmwwareStorageAddress"(if used), and before any other OTA API.*/
		//blc_ota_initOtaServer_module();
		//blc_ota_setOtaProcessTimeout(60); //set OTA whole process timeout
		blc_ota_registerOtaStartCmdCb(app_enter_ota_mode);
		blc_ota_registerOtaResultIndicationCb(app_ota_result);
	#endif



	///////////////////// Power Management initialization///////////////////
	#if(BLE_APP_PM_ENABLE)
		#if (SAVE_RAM_CODE_ENABLE)
			blc_ll_initPowerManagement_module_save_ram();        //pm module:      	 optional
		#else
			blc_ll_initPowerManagement_module();        //pm module:      	 optional
		#endif
		#if (PM_DEEPSLEEP_RETENTION_ENABLE)
			#if (!SAVE_RAM_CODE_ENABLE)
				blc_ll_initDeepsleepRetention_module();//Remove it if need save ramcode, and add DeepsleepRetentionEarlyWakeupTiming.
																	  //Note:The user needs to DeepsleepRetentionEarlyWakeupTiming according to the timing from boot to the end of the user_init_deepRetn.Refer to handbook for details.
			#endif
			bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
			blc_pm_setDeepsleepRetentionThreshold(95, 95);
			blc_pm_setDeepsleepRetentionEarlyWakeupTiming(750);
		#else
			bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
		#endif
		bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &app_set_kb_wakeup);
	#else
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
	#endif

	#if (UI_KEYBOARD_ENABLE)
		key_wakeup_init();
	#endif

	////////////////////////////////////////////////////////////////////////////////////////////////

	advertise_begin_tick = clock_time();
}


#if (PM_DEEPSLEEP_RETENTION_ENABLE)
/**
 * @brief		user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_
void user_init_deepRetn(void)
{


	//////////// LinkLayer Initialization  Begin /////////////////////////
	blc_ll_initBasicMCU();                      //mandatory

//////////////////////////// User Configuration for BLE application ////////////////////////////

	/* set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend */
	rf_set_power_level_index (MY_RF_POWER_INDEX);
	blc_ll_recoverDeepRetention();
	irq_enable();
	#if (UI_KEYBOARD_ENABLE)
		key_wakeup_init();
	#endif
////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif

/**
 * @brief     BLE main loop
 * @param[in]  none.
 * @return     none.
 */
void main_loop (void)
{

	////////////////////////////////////// BLE entry /////////////////////////////////
	blc_sdk_main_loop();

	////////////////////////////////////// UI entry /////////////////////////////////
	#if (BATT_CHECK_ENABLE)
		if(battery_get_detect_enable() && clock_time_exceed(lowBattDet_tick, 500000) ){
			lowBattDet_tick = clock_time();
			u8 battery_check_returnVaule;
			if(analog_read(USED_DEEP_ANA_REG) & LOW_BATT_FLG){
				battery_check_returnVaule=app_battery_power_check(VBAT_ALRAM_THRES_MV + 200);  //2.2 V
			}
			else{
				battery_check_returnVaule=app_battery_power_check(VBAT_ALRAM_THRES_MV);  //2.0 V
			}
			if(battery_check_returnVaule){
				analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG)&(~LOW_BATT_FLG));  //clr
			}
			else{
				#if (UI_LED_ENABLE)  //led indicate
					for(int k=0;k<3;k++){
						gpio_write(GPIO_LED_1, LED_ON_LEVAL);
						sleep_us(200000);
						gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
						sleep_us(200000);
					}
				#endif
				analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG) | LOW_BATT_FLG);  //mark
				GPIO_WAKEUP_FEATURE_LOW;

				cpu_set_gpio_wakeup (GPIO_WAKEUP_FEATURE, Level_High, 1);  //drive pin pad high wakeup deepsleep

				cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
			}
		}
	#endif

#if (OTA_BLE_ENABLE)
	ble_main_loop ();
#endif

	////////////////////////////////////// PM Process /////////////////////////////////
	blt_pm_proc();

	#if (UI_KEYBOARD_ENABLE)
		proc_keyboard (0, 0, 0);
		keyscan_scan_func();
	#endif  
	#if (IR_MODULE_ENABLE)
	// 如果红外有发码,延迟一个周期后再检测电量
	if (ir_send_ctrl.is_sending) {
		lowBattDet_tick = clock_time();
	}
	#endif
	if(lowBattDet_tick && clock_time_exceed(lowBattDet_tick, 500*1000) ){
		lowBattDet_tick = clock_time();
		#if (CHARGE_ENABLE)
		get_battery_voltage();
		#endif
	}
}

#if (OTA_BLE_ENABLE)
#include "led_module/led_driver.h"
u8 rcu_ble_pair_flag = 0x00;
/******************************************************************
 * @brief  ble_main_loop
 * @param  none
 * @return none
 * @retval void
 */
void ble_main_loop(void)
{
    if(rcu_ble_pair_flag && app_led_blink_timer && clock_time_exceed(app_led_blink_timer, app_led_blink_time_ms*1000))
    { 
		app_led_blink_timer = 0; 
		app_led_blink_timer = clock_time() | 1;
		static bool led_blink_flag = false;
		led_blink_flag = !led_blink_flag;
		if(led_blink_flag)
			gpio_write(GPIO_LED_1,LED_ON_LEVAL);
		else
			gpio_write(GPIO_LED_1,!LED_ON_LEVAL);
    }
}

/******************************************************************
 * @brief  rcu_ble_enter_pair
 * @param  none
 * @return none
 * @retval void
 */
void rcu_ble_enter_pair(void)
{
    printf("--->>> [work_debug] rcu_ble_enter_pair \r\n");
	led_1_driver_init();
	app_led_blink_time_ms = 250;
	app_led_blink_timer = clock_time() | 1;
	rcu_ble_pair_flag = 0x01;
	bls_smp_eraseAllpairingInformation();
	WaitMs(100);

	u8 status = bls_ll_setAdvParam(  MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
									ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
									0,  NULL,
									MY_APP_ADV_CHANNEL,
									ADV_FP_NONE);
	if(status != BLE_SUCCESS) { 	while(1); }
				bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
			bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

			bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //adv enable
}
#endif

#include "sdk/chip/B80/drivers/flash/flash_type.h"
/******************************************************************
 * @brief  app_rcu_flash_lock_init
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_flash_lock_init(void)
{
#if (APP_RCU_FLASH_LOCK_ENABLE)
    unsigned int mid = flash_read_mid();
    printf("--->>> [app_rcu_flash_lock_init] mid: 0x%04x\r\n",mid);
    switch(mid)
    {
        case 0x1160c8:
            flash_lock_mid1160c8(FLASH_LOCK_ALL_128K_MID1160C8);
            break;
        case 0x1360c8:
            flash_lock_mid1360c8(FLASH_LOCK_ALL_512K_MID1360C8);
            break;
        case 0x114485:
            flash_lock_mid114485(FLASH_LOCK_ALL_128K_MID114485);
            break;
        case 0x136085:
            flash_lock_mid136085(FLASH_LOCK_ALL_512K_MID136085);
            break;
        default:
            break;
    }
#endif
}

/******************************************************************
 * @brief  app_rcu_flash_unlock_init
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_flash_unlock_init(void)
{
#if (APP_RCU_FLASH_LOCK_ENABLE)
    unsigned int mid = flash_read_mid();
    printf("--->>> [app_rcu_flash_unlock_init] mid: 0x%04x\r\n",mid);
    switch(mid)
    {
        case 0x1160c8:
            flash_unlock_mid1160c8();
            break;
        case 0x1360c8:
            flash_unlock_mid1360c8();
            break;
        case 0x114485:
            flash_unlock_mid114485();
            break;
        case 0x136085:
            flash_unlock_mid136085();
            break;
        default:
            break;
    }
#endif
}

/******************************************************************
 * @brief  ota_firmware_erase_sector
 * @param  none
 * @return none
 * @retval void
 */
void ota_firmware_erase_sector(void)
{
    static u8 bin_start_address_01 = 0xff;
    static u8 bin_start_address_02 = 0xff;
    flash_read_page(0x00000 + 0x08, sizeof(bin_start_address_01), (u8 *)&bin_start_address_01);
    flash_read_page(0x40000 + 0x08, sizeof(bin_start_address_02), (u8 *)&bin_start_address_02);
	//printf("---> bin_start_address_01: 0x%01x\r\n", bin_start_address_01);
    //printf("---> bin_start_address_02: 0x%01x\r\n", bin_start_address_02);

	if((bin_start_address_01 == 0x4B) && (bin_start_address_02 != 0xFF))
	{
	#if(BLE_OTA_SERVER_ENABLE)
		/* OTA module initialization must be called after "blc_ota_setNewFirmwwareStorageAddress"(if used), and before any other OTA API.*/
		blc_ota_initOtaServer_module();
		blc_ota_setFirmwareSizeAndBootAddress(100, MULTI_BOOT_ADDR_0x40000);
		blc_ota_setOtaProcessTimeout(60); //set OTA whole process timeout
		blc_ota_registerOtaStartCmdCb(app_enter_ota_mode);
		blc_ota_registerOtaResultIndicationCb(app_ota_result);
	#endif
		printf("--->>> [work_debug] OTA-FINISH erase bin 02 Flash\r\n");
		app_rcu_flash_unlock_init();
        for (int i = 0; i < 400; i++)
            flash_erase_sector(OTA_FLASH_START_ADD_FIRM_1 + i * OTA_ERASE_BYTE_SIZE);
		//app_rcu_flash_lock_init();
	}
	if((bin_start_address_01 != 0xFF) && (bin_start_address_02 == 0x4B))
	{
	#if(BLE_OTA_SERVER_ENABLE)
		/* OTA module initialization must be called after "blc_ota_setNewFirmwwareStorageAddress"(if used), and before any other OTA API.*/
		blc_ota_initOtaServer_module();
		blc_ota_setFirmwareSizeAndBootAddress(100, MULTI_BOOT_ADDR_0x40000);
		blc_ota_setOtaProcessTimeout(60); //set OTA whole process timeout
		blc_ota_registerOtaStartCmdCb(app_enter_ota_mode);
		blc_ota_registerOtaResultIndicationCb(app_ota_result);
	#endif
		printf("--->>> [work_debug] OTA-FINISH erase bin 01 Flash\r\n");
		app_rcu_flash_unlock_init();
        for (int i = 0; i < 400; i++)
            flash_erase_sector(OTA_FLASH_START_ADD_FIRM_0 + i * OTA_ERASE_BYTE_SIZE);
		//app_rcu_flash_lock_init();
	}
}
