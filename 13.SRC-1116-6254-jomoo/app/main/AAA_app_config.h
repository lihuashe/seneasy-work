/********************************************************************************************************
 * @file     AAA_app_config.h
 *
 * @brief    This is the header file for KMD SDK
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

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define DEBUG_MODE	1 //DEBUG printf mode
#define BATT_CHECK_ENABLE       1
#define MCU_CORE_B80 1

#define OTA_UPDATE_ENABLE               1
#define RF_24G_FACTORY_ENABLE 		    1
#define RF_EMI_RSSI_ENABLE 		        1
#define DEVICE_WACK_ENABLE 		 	    1   // 休眠功能
#define STUCK_KEY_PROCESS_ENABLE 	    1   // 卡键功能
#define STUCK_KEY_ENTERDEEP_TIME     	15
#define STUCK_KEY_COMFIGURATION         1   // 卡键自定义

#if(DEBUG_MODE)
#define PRINT_BAUD_RATE			115200	//1M baud rate,should Not bigger than 1Mb/s
#define DEBUG_INFO_TX_PIN		GPIO_PA7
#endif

#define SINGLE_TONE_PIN		GPIO_PB5

/* Define system clock */
#define CLOCK_SYS_CLOCK_HZ  	24000000//48000000

#if(MCU_CORE_B89)
#if(CLOCK_SYS_CLOCK_HZ==12000000)
	#define SYS_CLK  	SYS_CLK_12M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ==16000000)
	#define SYS_CLK  	SYS_CLK_16M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ==24000000)
	#define SYS_CLK  	SYS_CLK_24M_Crystal
#endif
#else
#if(CLOCK_SYS_CLOCK_HZ==12000000)
	#define SYS_CLK  	SYS_CLK_12M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ==16000000)
	#define SYS_CLK  	SYS_CLK_16M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ==24000000)
	#define SYS_CLK  	SYS_CLK_24M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ==32000000)
	#define SYS_CLK  	SYS_CLK_32M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ==48000000)
	#define SYS_CLK  	SYS_CLK_48M_Crystal
#endif
#endif

enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};

/////////////////// watchdog  //////////////////////////////
#if _CHIP_IS_OTP_  // ECLIPS SET
#define MODULE_WATCHDOG_ENABLE			0
#define MODULE_32K_WATCHDOG_ENABLE		1
#else
#define MODULE_WATCHDOG_ENABLE			1
#define MODULE_32K_WATCHDOG_ENABLE		0
#endif
#define WATCHDOG_INIT_TIMEOUT			1200  //ms

#define LED1     		        GPIO_PA4
#define LED2     		        GPIO_PA5
#define LED3     		        GPIO_PA6
#define LED4     		        GPIO_PA7

#define	UI_KEYBOARD_ENABLE						1
#if (UI_KEYBOARD_ENABLE)   // if test pure power, kyeScan GPIO setting all disabled
	//---------------  KeyMatrix PA0/PD4/PF0/PF1 -----------------------------
	#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
	#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K

	#define	KB_LINE_HIGH_VALID				0   //dirve pin output 0 when keyscan, scanpin read 0 is valid

	#define			CR_VOL_UP				0xf0
	#define			CR_VOL_DN				0xf1

	/**
	 *  @brief  Normal keyboard map
	 */
	#define		KB_MAP_NORMAL	{{1,2,3},{4,5,6},{7,8,9},{10,11,12}}

	//////////////////// KEY CONFIG (EVK board) ///////////////////////////
	#define  KB_DRIVE_PINS  {GPIO_PB6,GPIO_PB7,GPIO_PC0}
	#define  KB_SCAN_PINS   {GPIO_PD4,GPIO_PB2,GPIO_PB4,GPIO_PB5}

	//drive pin as gpio
	#define	PB6_FUNC				AS_GPIO
	#define	PB7_FUNC				AS_GPIO
	#define	PC0_FUNC				AS_GPIO
	#define	PC1_FUNC				AS_GPIO

	//drive pin need 100K pulldown
	#define	PULL_WAKEUP_SRC_PB6		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PB7		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PC0   	MATRIX_ROW_PULL
	//drive pin open input to read gpio wakeup level
	#define PB6_INPUT_ENABLE		1
	#define PB7_INPUT_ENABLE		1
	#define PC0_INPUT_ENABLE		1

	//scan pin as gpio
	#define	PD4_FUNC				AS_GPIO
	#define	PB2_FUNC				AS_GPIO
	#define	PB4_FUNC				AS_GPIO
	#define	PB5_FUNC				AS_GPIO

	//scan  pin need 10K pullup
	#define	PULL_WAKEUP_SRC_PD4		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PB2		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PB4 	MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PB5		MATRIX_COL_PULL

	//scan pin open input to read gpio level
	#define PD4_INPUT_ENABLE		1
	#define PB2_INPUT_ENABLE		1
	#define PB4_INPUT_ENABLE		1
	#define PB5_INPUT_ENABLE		1

	#define		KB_MAP_NUM		    KB_MAP_NORMAL
	#define		KB_MAP_FN			KB_MAP_NORMAL
#endif

#if 0//degug rf timer
#define PB2_INPUT_ENABLE	0
#define PB2_OUTPUT_ENABLE	1
#define PB2_DATA_OUT	0
#define PIN_DEBUG_RF_RX_IRQ_TOGGLE   	gpio_toggle(GPIO_PB2)
									
#define PB3_INPUT_ENABLE	0
#define PB3_OUTPUT_ENABLE	1
#define PB3_DATA_OUT	0
#define PIN_DEBUG_RF_RX_CRC_OK_TOGGLE   gpio_toggle(GPIO_PB3)
									
#define PB4_INPUT_ENABLE	0
#define PB4_OUTPUT_ENABLE	1
#define PB4_DATA_OUT	0
#define PIN_DEBUG_RF_RX_DATA_OK_TOGGLE   	gpio_toggle(GPIO_PB4)

#define PB5_INPUT_ENABLE	0
#define PB5_OUTPUT_ENABLE	1
#define PB5_DATA_OUT	0
#define PIN_DEBUG_RF_TX_LEVEL(x)   	gpio_write(GPIO_PB5,x)

#define PD7_INPUT_ENABLE	0
#define PD7_OUTPUT_ENABLE	1
#define PD7_DATA_OUT	0
#define PIN_DEBUG_RF_CHN_NEXT_TOGGLE   	gpio_toggle(GPIO_PD7)

#else
#define PIN_DEBUG_RF_RX_IRQ_TOGGLE
#define PIN_DEBUG_RF_RX_CRC_OK_TOGGLE
#define PIN_DEBUG_RF_RX_DATA_OK_TOGGLE
#define PIN_DEBUG_RF_TX_LEVEL(x)
#define PIN_DEBUG_RF_CHN_NEXT_TOGGLE
#endif

#ifndef USER_DEBUG
#define USER_DEBUG   0
#endif

#if USER_DEBUG
#define IO_PRINT				1
#define USB_PRINT				2
#define PRINT_MODE 				IO_PRINT

#define D1_LED_PIN  GPIO_PA2
#define D2_LED_PIN  GPIO_PA3
#define D3_LED_PIN  GPIO_PA4
#define D4_LED_PIN  GPIO_PB0
#define D5_LED_PIN  GPIO_PB1
#define RF_CHAN_CHANGE_PIN  GPIO_PC7


#define BNT_1_PIN  GPIO_PD5
#define BNT_2_PIN  GPIO_PD6
#endif

#define PIN_DEBUG_RF_TX_LEVEL(x)
#define PIN_DEBUG_RF_RX_LEVEL(x)
#define PIN_DEBUG_RF_RX_CRC_OK_LEVEL(x)
#define PIN_DEBUG_RF_RX_DATA_OK_LEVEL(x)
#define PIN_DEBUG_RF_RX_TIMEOUT_LEVEL(x)
#define PIN_DEBUG_RF_CHN_NEXT_TOGGLE
#define PIN_DEBUG_UI_TIME_LEVEL(x)




/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
