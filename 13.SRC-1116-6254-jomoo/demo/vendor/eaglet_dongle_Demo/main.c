/********************************************************************************************************
 * @file     main.c
 *
 * @brief    This is the source file for KMD SDK
 *
 * @author	 KMD GROUP
 * @date         01,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "calibration.h"
#include "AAA_project_config.h"

OUTPUT_DEV_INFO_AAA output_dev_info =
{
    0,//u32 bin_crc;
    0x01020304, //u32 fw version
};

extern void user_init();
extern void main_loop (void);

void generateRandomNum(int len, unsigned char *data)
{
	int i;
	unsigned int randNums = 0;
    /* if len is odd */
	for (i=0; i<len; i++ ) {
		if( (i & 3) == 0 ){
			randNums = rand();
		}
		data[i] = randNums & 0xff;
		randNums >>=8;
	}
}

/**
 * @brief		This function serves to handle the interrupt of MCU
 * @param[in] 	none
 * @return 		none
 */
_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
	u32 src = reg_irq_src;
	
	if(src & FLD_IRQ_TMR1_EN)
	{
		irq_host_timer1();
		reg_tmr_sta = FLD_TMR_STA_TMR1;//write 1 to clear
	}

	unsigned short rf_irq_src = rf_irq_src_get();

   	if (rf_irq_src)
	{
       	if (rf_irq_src & FLD_RF_IRQ_TX)
		{
			PIN_DEBUG_RF_TX_LEVEL(0);
			rf_tx_irq_handle();
			rf_irq_clr_src(FLD_RF_IRQ_TX);
		}

		if (rf_irq_src & FLD_RF_IRQ_RX)
		{
			PIN_DEBUG_RF_RX_IRQ_TOGGLE;
			rf_rx_irq_handle();
			rf_irq_clr_src(FLD_RF_IRQ_RX);
		}
    }
}

#if (_CHIP_IS_OTP_)
enum{
    FLD_VDD1V2      = BIT_RNG(3, 5),
};
#endif

/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
int main (void)   //must on ramcode
{
	blc_pm_select_internal_32k_crystal(); //Use internal 32K clock

#if (MCU_CORE_B85)
	cpu_wakeup_init();
#elif (MCU_CORE_B87)
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);
#elif (MCU_CORE_B80) || (MCU_CORE_B89)
	cpu_wakeup_init(EXTERNAL_XTAL_24M);
#endif

#if(MCU_CORE_B80||MCU_CORE_B89)
	wd_32k_stop();
#endif

#if (MCU_CORE_B85) || (MCU_CORE_B87)
	//Note: This function must be called, otherwise an abnormal situation may occur.
	//Called immediately after cpu_wakeup_init, set in other positions, some calibration values may not take effect.
	user_read_flash_value_calib();
#elif (MCU_CORE_B89)
	//Note: This function must be called, otherwise an abnormal situation may occur.
	//Called immediately after cpu_wakeup_init, set in other positions, some calibration values may not take effect.
	user_read_otp_value_calib();
#elif (MCU_CORE_B80)
	//Note: This function must be called, otherwise an abnormal situation may occur.
	//Called immediately after cpu_wakeup_init, set in other positions, some calibration values may not take effect.
#if(PACKAGE_TYPE == OTP_PACKAGE)
	user_read_otp_value_calib();
#elif(PACKAGE_TYPE == FLASH_PACKAGE)
	user_read_flash_value_calib();
#endif
#endif
	//int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp
    
	clock_init(SYS_CLK);  //Set the main frequency to 48M
	
#if 0//(_CHIP_IS_OTP_)
	analog_write(0x03, (analog_read(0x03) & (~FLD_VDD1V2))|(0x02<<3));
#endif

	gpio_init(1);

#if HW_IS_FLASH
	random_generator_init();

	#if OTA_ENABLE_AAA
		run_app_code();
	#endif
#endif

	deep_flag = analog_read(SYS_DEEP_ANA_REG);
	// printf("---deep_flag=%d.\r\n",deep_flag);

	custom_init(); //Dongle information is initialized
	
	user_init();
		
#if (MODULE_WATCHDOG_ENABLE)
    wd_set_interval_ms(WATCHDOG_INIT_TIMEOUT, CLOCK_SYS_CLOCK_1MS);
    wd_start();
#elif (MODULE_32K_WATCHDOG_ENABLE)
    wd_32k_set_interval_ms(WATCHDOG_INIT_TIMEOUT);
    wd_32k_start();
#endif
	printf("---eaglet dongle init---\n");

	while (1) {
	#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
	#endif

		main_loop ();
		
	#if DEBUG_MODE
		static u32 tick = 0;
		static u8 loop_cnt = 0;
		if (clock_time_exceed(tick, 3000000))
		{
			loop_cnt++;
			tick = clock_time();
			printf("---loop=%d.\n",loop_cnt);
		}
	#endif

	#if (MODULE_32K_WATCHDOG_ENABLE)
		wd_32k_stop();
		wd_32k_set_interval_ms(WATCHDOG_INIT_TIMEOUT);
		wd_32k_start();
	#endif
	}

	return 0;
}
