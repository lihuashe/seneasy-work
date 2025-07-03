/********************************************************************************************************
 * @file     app_config.h
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

#pragma once


///////////////////////// Feature Configuration////////////////////////////////////////////////
/**
 *  @brief  Feature select in BLE Sample project
 */
#define FLASH_SIZE_OPTION							FLASH_SIZE_OPTION_512K //very important, user need confirm !!!
#define APP_SECURITY_ENABLE      					1
#define	BLE_OTA_SERVER_ENABLE						0
#define TEST_CONN_CURRENT_ENABLE					0		//test connection current, disable UI to have a pure power
#define SAVE_RAM_CODE_ENABLE						0		//Enable it if need save RAM code.
															//Note: When this is enabled, the average power consumption increases.If user use deepsleep retention, user need to add DeepsleepRetentionEarlyWakeupTiming.

/* firmware signature check */
#define FIRMWARES_SIGNATURE_ENABLE     		 		0

/* Power Management */
#define BLE_APP_PM_ENABLE							0
#define PM_DEEPSLEEP_ENABLE            				0 		//test connection power, should disable deepSleep
#define PM_DEEPSLEEP_RETENTION_ENABLE               0
#define PM_APP_ENABLE								1		//24G PM

/**
 *  @brief  APP Function Configuration
 */
enum{
	RF_BLE_MODE = 0,
	RF_24G_MODE = 1,
};

#if (TEST_CONN_CURRENT_ENABLE)
	#define UI_LED_ENABLE          	 				0
	#define	UI_KEYBOARD_ENABLE						0
#else
	#define UI_LED_ENABLE          	 				1
	#define	UI_KEYBOARD_ENABLE						1
	#define STUCK_KEY_PROCESS_ENABLE 	    		1
	#define STUCK_KEY_ENTERDEEP_TIME              	30
#endif

#define BATT_CHECK_ENABLE       					0   //enable or disable battery voltage detection
#define DEBUG_MODE									0   //DEBUG printf mode
#define CFG_BLE_24G_ADDR   							0x78000
#define CFG_DEVICE_MODE_ADDR   						0x79000
#define USE_EXTERNAL_CAP							0
#define GSENSOR_ENABLE  							0	//Gensor_Module
#define GSENSOR_DA217_ENABLE  						1	//Gensor_DA217_Module
#define PA_MODULE_ENABLE							1   //PA_Module
#define IR_MODULE_ENABLE							1   //IR_Module
#define OTA_BLE_ENABLE								0   //OTA_BLE
#define OTA_DFU_ENABLE								1   //OTA_DFU
#define CHARGE_ENABLE                               1	//Charge_Module
#define VOLTAGE_STOP_CH_EN          				1
#define TOUCH_ENABLE                                1	//Touch_Module
#define APP_RCU_FLASH_LOCK_ENABLE             		1 	//Enable Flash Lock
#define FACTORY_ENABLE								1	//Factory_Module
#define GHOST_KEY_FILTER_ENABLE                     1   
#define PCB_S1626_CHANGE							1	//1:PCB New
///////////////////////// System Clock  Configuration /////////////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ      					24000000

#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	#define SYS_CLK_TYPE  							SYS_CLK_16M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	#define SYS_CLK_TYPE  							SYS_CLK_24M_Crystal
#elif(CLOCK_SYS_CLOCK_HZ == 32000000)
	#define SYS_CLK_TYPE  							SYS_CLK_32M_Crystal
#elif(CLOCK_SYS_CLOCK_HZ == 48000000)
	#define SYS_CLK_TYPE  							SYS_CLK_48M_Crystal
#else
	#error "unsupported system clock !"
#endif

enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};


/**
 *  @brief  watchdog enable and timeout setting
 */
#define MODULE_WATCHDOG_ENABLE						1
#define WATCHDOG_INIT_TIMEOUT						500  //Unit:ms

#if 0
/**
 *  @brief  software uart enable and setting
 */
#define 	SOFT_UART_BAUD_RATE                     9600
#define 	SOFT_UART_TX_IO                   		GPIO_PD5
#define 	SOFT_UART_RX_IO                   		GPIO_PD6
#endif

#if (UI_KEYBOARD_ENABLE)   // if test pure power, kyeScan GPIO setting all disabled
	//---------------  KeyMatrix PA0/PD4/PF0/PF1 -----------------------------
	#define	MATRIX_ROW_PULL			PM_PIN_PULLDOWN_100K
	#define	MATRIX_COL_PULL			PM_PIN_PULLUP_10K

	#define	KB_LINE_HIGH_VALID		0   //dirve pin output 0 when keyscan, scanpin read 0 is valid
	//Normal keyboard map
	#define	KB_MAP_NORMAL		{	{1,		2,		3,		4,		5,		6,		7,		8},		\
									{9,	    10,		11,		12,		13,		14,		15,		16},	\
									{17,	18,		19,		20,		21,		22,		23,		24},	\
									{25,	26,		27,		28,		29,		30,		31,		32},	\
									{33,	34,		35,		36,		37,		38,		39,		40},	\
									{41,	42,		43,		44,		45,		46,		47,		48},	\
									{49,	50,		51,		52,		53,		54,		55,		56},	\
									{57,	58,		59,		60,		61,		62,		63,		64},	\
									{65,	66,		67,		68,		69,		70,		71},	}

	//////////////////// KEY CONFIG (EVK board) ///////////////////////////
	#define KB_DRIVE_PINS  			{GPIO_PB0, GPIO_PB1, GPIO_PB2, GPIO_PB3, GPIO_PC6, GPIO_PC7, GPIO_PF1, GPIO_PD4}
	#define KB_SCAN_PINS   			{GPIO_PD2, GPIO_PB5, GPIO_PB7, GPIO_PC0, GPIO_PC1, GPIO_PC3, GPIO_PD5, GPIO_PC5, GPIO_PD6}
	#define KB_DRIVE_NUM           	8
	#define KB_SCAN_NUM            	9
	//drive pin as gpio
	#define	PB0_FUNC				AS_GPIO
	#define	PB1_FUNC				AS_GPIO
	#define	PB2_FUNC				AS_GPIO
	#define	PB3_FUNC				AS_GPIO
	#define	PC6_FUNC				AS_GPIO
	#define	PC7_FUNC				AS_GPIO
	#define	PF1_FUNC				AS_GPIO
	#define	PD4_FUNC				AS_GPIO
	//drive pin need 100K pulldown
	#define	PULL_WAKEUP_SRC_PB0		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PB1		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PB2		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PB3		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PC6		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PC7		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PF1		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PD4		MATRIX_ROW_PULL
	//drive pin open input to read gpio wakeup level
	#define PB0_INPUT_ENABLE		1
	#define PB1_INPUT_ENABLE		1
	#define PB2_INPUT_ENABLE		1
	#define PB3_INPUT_ENABLE		1
	#define PC6_INPUT_ENABLE		1
	#define PC7_INPUT_ENABLE		1
	#define PF1_INPUT_ENABLE		1
	#define PD4_INPUT_ENABLE		1

	//scan pin as gpio
	#define	PD2_FUNC				AS_GPIO
	#define	PB5_FUNC				AS_GPIO
	#define	PB7_FUNC				AS_GPIO
	#define	PC0_FUNC				AS_GPIO
	#define	PC1_FUNC				AS_GPIO
	#define	PC3_FUNC				AS_GPIO
	#define	PD5_FUNC				AS_GPIO
	#define	PC5_FUNC				AS_GPIO
	#define	PD6_FUNC				AS_GPIO
	//scan  pin need 10K pullup
	#define	PULL_WAKEUP_SRC_PD2		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PB5		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PB7		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PC0		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PC1		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PC3		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PD5		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PC5		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PD6		MATRIX_COL_PULL
	//scan pin open input to read gpio level
	#define PD2_INPUT_ENABLE		1
	#define PB5_INPUT_ENABLE		1
	#define PB7_INPUT_ENABLE		1
	#define PC0_INPUT_ENABLE		1
	#define PC1_INPUT_ENABLE		1
	#define PC3_INPUT_ENABLE		1
	#define PD5_INPUT_ENABLE		1
	#define PC5_INPUT_ENABLE		1
	#define PD6_INPUT_ENABLE		1

	#define	KB_MAP_NUM				KB_MAP_NORMAL
	#define	KB_MAP_FN				KB_MAP_NORMAL
#endif


#if(UI_LED_ENABLE)
	/**
	 *  @brief  Definition gpio for led
	 */
	#define	GPIO_LED_1  			GPIO_PD3
	#define	GPIO_LED_2				GPIO_PA2
	#define LED_ON_LEVAL 			0 		//gpio output high voltage to turn on led

	#define PD3_FUNC				AS_GPIO
	#define PA2_FUNC				AS_GPIO

	#define	PD3_OUTPUT_ENABLE		0
	#define	PA2_OUTPUT_ENABLE		0

#if (TOUCH_ENABLE)
	#define PC2_FUNC				AS_GPIO
	#define	PULL_WAKEUP_SRC_PC2  	PM_PIN_PULLDOWN_100K
	#define PC2_OUTPUT_ENABLE       1

	#define PD7_FUNC				AS_GPIO
	#define	PULL_WAKEUP_SRC_PD7  	PM_PIN_PULLUP_1M
	#define PD7_OUTPUT_ENABLE       1

	#define PA0_FUNC				AS_GPIO
	#define	PULL_WAKEUP_SRC_PA0  	PM_PIN_PULLUP_10K
	#define PA0_INPUT_ENABLE        1
#endif 

#endif


/**
 *  @brief  Battery_check Configuration
 */
#if (BATT_CHECK_ENABLE)
	#define ADC_INPUT_PCHN						VBAT    //corresponding  ADC_InputPchTypeDef in adc.h

	#define VBAT_ALRAM_THRES_MV					2000   // 2000 mV low battery alarm

	/////////////////// DEEP SAVE FLG //////////////////////////////////
	#define USED_DEEP_ANA_REG                   DEEP_ANA_REG0 //u8,can save 8 bit info when deep
	#define	LOW_BATT_FLG					    BIT(0)

	//////////////////////////// FEATURE PM GPIO	(EVK board) /////////////////////////////////
	#define GPIO_WAKEUP_FEATURE					GPIO_PB6   //mcu wakeup module
	#define	PB6_FUNC							AS_GPIO
	#define PB6_INPUT_ENABLE					1
	#define	PB6_OUTPUT_ENABLE					0
	#define	PB6_DATA_OUT						0
	#define GPIO_WAKEUP_FEATURE_HIGH			gpio_setup_up_down_resistor(GPIO_WAKEUP_FEATURE, PM_PIN_PULLUP_10K);
	#define GPIO_WAKEUP_FEATURE_LOW				gpio_setup_up_down_resistor(GPIO_WAKEUP_FEATURE, PM_PIN_PULLDOWN_100K);
#endif

#if (CHARGE_ENABLE)
    #define GPIO_VBAT_DETECT                  	GPIO_PC4
    #define PC4_FUNC                          	AS_GPIO
    #define PC4_INPUT_ENABLE                 	0
    #define ADC_INPUT_PCHN                   	C4P    //corresponding  ADC_InputPchTypeDef in adc.h

#endif
#if VOLTAGE_STOP_CH_EN
	#define GPIO_NIC_DETECT                  	GPIO_PB6
    #define PB6_FUNC                          	AS_GPIO
    #define PB6_INPUT_ENABLE                 	0
    #define ADC_INPUT_PCHN_PB6                  B6P    //corresponding  ADC_InputPchTypeDef in adc.h

#endif
	/////////////////// DEEP SAVE FLG //////////////////////////////////
	#define USED_DEEP_ANA_REG                   DEEP_ANA_REG0 //u8,can save 8 bit info when deep
	#define	LOW_BATT_FLG					    BIT(0)
	#define POWER_ON_FLG                        BIT(5)

/**
 *  @brief  Definition for debug printf
 */
#if(DEBUG_MODE)
	#define PRINT_BAUD_RATE			115200	//1M baud rate,should Not bigger than 1Mb/s
	#define DEBUG_INFO_TX_PIN		GPIO_PA2
#endif




/////////////////// set default   ////////////////

#include "../common/default_config.h"
