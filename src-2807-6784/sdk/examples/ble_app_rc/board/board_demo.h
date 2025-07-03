#ifndef __BOARD_DEMO_H
#define __BOARD_DEMO_H


/*******************************注意点***********************************/
//每次修改注意更新产测里面的项目编号和软件版本
//
/************************************************************************/
#define CONFIG_LOG_OUTPUT 1

#if (defined(CONFIG_LOG_OUTPUT))

#define REMOTE_UART_ENABLE				1
#define BOARD_STATUS_PRINTF_EN			0	//板子状态打印

#define APP_MAIN_LOG_LEVEL                                      3
#define APP_GAP_LOG_LEVEL										3
#define APP_KEY_LOG_LEVEL										3
#define APP_MODE_LOG_LEVEL                                      3
#define APP_LED_LOG_LEVEL                                       0
#define APP_BATT_LOG_LEVEL                                      3
#define APP_IR_LOG_LEVEL                                      	0
#define APP_ATV_LOG_LEVEL										3
#define APP_XIHAO_AUDIO_LOG_LEVEL								3
//#define APP_OTA_LOG_LEVEL										3
#define APP_XIHAO_FCTORY_LOG_LEVEL								0
#define APP_AUDIO_LOG_LEVEL										3
#define APP_MODULE_LOG_LEVEL                                    3
#endif

#define CONFIG_INTERNAL_DCDC_ENABLE				1
#define MODE_DEEP_SLEEP_NO_RESET_MCU			1
#define REMOTE_LED_ENABLE						1
#define REMOTE_IR_ENABLE						1
#define REMOTE_BATT_ENABLE						1
#define MTU_REQUEST_ENABLE						1
#define FACTORY_SIGNAL_ENABLE					1
#define IDENTITY_ADDRS_RESOLV_EN				1
#define XIHAO_FUN_ENABLE						1
#define APP_SEC_NVDS_SUPPORT 					1
#define SLEEP_FLASH_OFF_EN						1
#define REMOTE_64M_ENABLE						0
#define COMB_KEY_SEND_CODE_EN					1	//组合键发码
#define AUTO_PAIR_ENABLE						1

//功能模块使能
#define BACKLIGHT_ENABLE						1	//led_module
#define CHARGE_MODULE_ENABLE					1	//charge_module
#define SENSOR_3_AXIAL_ENABLE					1	//sensor_3_axial_module
#define SENSOR_OPTICAL_ENABLE					1	//sensor_optical_module

//gpio
#define GPIO_PA1                                21
#define GPIO_PA2                                22
#define GPIO_PA3                                7
#define GPIO_PA4                                2
#define GPIO_PA5                                3
#define GPIO_PA6                                10
#define GPIO_PA7                                8
#define GPIO_PA8                                11
#define GPIO_PA9                                12
#define GPIO_PA10                               9
#define GPIO_PA11                               13
#define GPIO_PA12                               14
#define GPIO_PA13                               15
#define GPIO_PA14                               4
#define GPIO_PA15                               5
#define GPIO_PA16                               6
#define GPIO_PA17                               16
#define GPIO_PA18                               17
#define GPIO_PA19                               0
#define GPIO_PA20                               1
#define GPIO_PA21                               18
#define GPIO_PA22                               19
#define GPIO_PA23                               20



/**debug引脚定义*/
#define PIN_UART1_TX                    GPIO_PA15
#define PIN_UART1_RX                    GPIO_PA16
#define DEBUG_UART                      XH_UART1
#define DEBUG_UART_BAUDRATE             921600


// <o> Default Ctune of 32M Xtal <0-31>
// <i> Value range: 0-31, unit: 0.75pF
#define XTAL32M_CTUNE_DEFAULT                                   24

//<o>Use the frequency offset calibration value measured
#define CONFIG_FREQ_CALIB_ENABLE                                0


#define IR_STUCK_KEY				(1<<0)
#define BLE_STUCK_KEY				(1<<1)
#define STUCK_KEY_PROCESS_ENABLE	(IR_STUCK_KEY|BLE_STUCK_KEY)   //卡键 红外+蓝牙

#define LED1_LED_FLAG						(1<<0)
#define LED2_LED_FLAG						(1<<1)
#define LED3_LED_FLAG						(1<<2)

#define BLT_APP_LED_ENABLE					(LED1_LED_FLAG)


/////////////////////////语音控制部分///////////////////////////////

#define XIHAO_STB_ENABLE			1
#define ATV_AUDIO_ENABLE			2


#if (XIHAO_STB_ENABLE || ATV_AUDIO_ENABLE)
#define BLE_AUDIO_ENABLE			1

#define I2S_BLOCK_SIZE        256//2048//2048//1024//256
#define I2S_DMA_BUFFER_SIZE   1024//8192//4096//1024
#define I2S_BLOCK_NUM         (sizeof(i2s_dma_buffer)/I2S_BLOCK_SIZE)
#define PCM_FRAME_SIZE        256
#endif

#if (!defined(BLE_AUDIO_ENABLE))
#define BLE_AUDIO_ENABLE			0
#endif

#define AUDIO_NONE_TYPE				0
#define GOOGLE_AUDIO_TYPE     		1
#define OTHER_AUDIO_TYPE			2

#define AUDIO_ENCODE_MSBC			1
#define AUDIO_ENCODE_SBC			2
#define AUDIO_ENCODE_IMA_ADPCM		3


#if (BLE_AUDIO_ENABLE)

#if (ATV_AUDIO_ENABLE)
#define BLE_APP_AUDIO_TYPE     				(GOOGLE_AUDIO_TYPE )// | OTHER_AUDIO_TYPE// 控制语言是否存在，主要是谷歌服务
#define AUDIO_ENCODE_TYPE					AUDIO_ENCODE_IMA_ADPCM | AUDIO_ENCODE_SBC // 未用到
#define VOICE_PCM_FRAME_SIZE				512 //480
#define ENABLE_GOOGLE_VOICE_1P0				1
#endif


//使用2的n次方
#define AUDIO_PCM_LEN           2048			
#define AUDIO_ENCODE_LEN        1024

#if (!defined(BLE_APP_AUDIO_TYPE))
#define BLE_APP_AUDIO_TYPE			AUDIO_NONE_TYPE	
#endif
#endif


/////////////////////语音控制部分/////////////////////////////////////

#define BATT_DETECT_MIN_LEVEL				2000
#define BATT_DETECT_MAX_LEVEL				3000
#define LOW_BATT_PROMPT_LEVEL				2300

#define LOW_BATT_POWER_OFF_LEVEL			2000
#define LOW_BATT_PRODECT_ENABLE				1
#define LOW_BATT_IR_PRODECT_LEVEL			2000	
#define LOW_BATT_BLE_PRODECT_LEVEL			2000


#if (BLT_APP_LED_ENABLE)

#define LED1_LED				0
#define LED2_LED				1
#define LED3_LED				2
#define LED_MAX_CONT			1

#define LED1_PORT				GPIO_PA18
#define LED1_ON_LEVAL			0

#define LED2_PORT				GPIO_PA22
#define LED2_ON_LEVAL			1

#define LED3_PORT				GPIO_PA23
#define LED3_ON_LEVAL			1

#define LED_DC_EN_PORT			GPIO_PA2

enum{
	LED_IR_PRESS=0, 			//红外led按下
	LED_BLE_PRESS,				//蓝牙leda按下
	LED_VOICE_PRESS,
	LED_VOICE_RELEASE,
	LED_PRESS_RELEASE,  		//按键释放
	LED_BLE_PRESS_RELEASE,

	LED_WAIT_PAIR_HOLD,
	
	LED_FAC_PAIR_ENTER, 		//配对开始
	LED_FAC_PAIR_EXIT, 			//配对退出
	LED_WAIT_PAIR, 				//配对开始
	LED_FAC_WAIT_PAIR, 			//配对开始
	LED_PAIR_SUCCESS, 			//配对成功
	LED_PAIR_FAIL, 				//配对失败
	LED_UNBOND_DISCON_LED2, 	//解绑
	LED_UNBOND_DISCON_LED1,

	LED_LOW_PRESSURE, 			//低压提示
	LED_LOW_PRESSURE_RELEASE, 	//低压提示
	LED_MOUSE_ENABLE,
	LED_MOUSE_DISABLE,
	
	LED_CLEAR_BOND_SUCCESS,
	
	LED_OTA_START,
	LED_OTA_EXIT,

	LED_CLEAR_SMART_INFO_HINT,

};


#define LED_MAP_ARR 	{\
{250,  		250,  	0x06,    			0x20,	LED_IR_PRESS,				LED1_LED},\
{500,  		10,  	1,   				0x20,	LED_BLE_PRESS,				LED1_LED},\
{30000,  	10, 	0x01,  				0x23,	LED_VOICE_PRESS,			LED1_LED},\
{0, 	   	100,	LED_SINGLE_STATE,  	0x24,	LED_VOICE_RELEASE,			LED1_LED},\
{0,	   		10, 	0x01,  				0x22,	LED_PRESS_RELEASE,			LED1_LED},\
{0,			10,		0x01,				0x22,	LED_BLE_PRESS_RELEASE,		LED1_LED},\
{250,		10,		0x01,				0x30,	LED_WAIT_PAIR_HOLD,			LED1_LED},\
{100, 		100, 	0xFFFFFF,			0x40,	LED_WAIT_PAIR,				LED1_LED},\
{3000,      10,   	1,					0x41,	LED_PAIR_SUCCESS,			LED1_LED},\
{0,  		10,		LED_SINGLE_STATE,	0x41,	LED_PAIR_FAIL,				LED1_LED},\
{250, 	   	250, 	3,					0x41,	LED_UNBOND_DISCON_LED2,		LED1_LED},\
{250, 	   	250, 	3,					0x41,	LED_UNBOND_DISCON_LED1,		LED1_LED},\
{500, 	   	500, 	3,					0x23,	LED_LOW_PRESSURE,			LED1_LED},\
{500, 	   	500, 	3,					0x23,	LED_LOW_PRESSURE_RELEASE,	LED1_LED},\
{300,		300,	0x03,				0x41,	LED_CLEAR_BOND_SUCCESS,		LED1_LED},\
{1000,		1000,	0xFFFF,				0x44,	LED_OTA_START,				LED1_LED},\
{0,			10,		0x01,				0x45,	LED_OTA_EXIT,				LED1_LED},\
{500,		500,	3,					0x41,	LED_CLEAR_SMART_INFO_HINT,	LED1_LED},\
}
#endif


#define UPDATE_CONNECT_PARAM_TIMER		4000	//多少ms后更新连接参数
//通用配对广播配置
#define BLE_DEVICE_NAME                 'A','P','H','R','C'//'X','H','_','R','T'
#define BLE_DEVICE_NAME_LEN				(5+1)

#define KEY_SCAN_GND_EN					0

#define KEY_ROW_NUM             		6	//DD
#define KEY_COL_NUM             		6	//DD

#define PIN_KEY_ROW_0 					GPIO_PA8
#define PIN_KEY_ROW_1 					GPIO_PA9
#define PIN_KEY_ROW_2 					GPIO_PA11
#define PIN_KEY_ROW_3 					GPIO_PA12
#define PIN_KEY_ROW_4 					GPIO_PA13
#define PIN_KEY_ROW_5 					GPIO_PA17
#define PIN_KEY_ROW_6 					0xFF
#define PIN_KEY_ROW_7 					0xFF	

#define PIN_KEY_COL_0 					GPIO_PA7
#define PIN_KEY_COL_1 					GPIO_PA6
#define PIN_KEY_COL_2 					GPIO_PA21
#define PIN_KEY_COL_3 					GPIO_PA22
#define PIN_KEY_COL_4 					GPIO_PA23
#define PIN_KEY_COL_5 					GPIO_PA1
#define PIN_KEY_COL_6 					0xFF
#define PIN_KEY_COL_7 					0xFF   


#define     KB_MAP_NORMAL  {\
	{1,		2,		3,		4,		5,		6,}, \
	{7,		8,		9,		10,		11,		12,}, \
	{13,	14,		15,		16,		17,		18,}, \
	{19,	20,		21,		22,		23,		24,}, \
	{25,	26,		27,		28,		29,		30,}, \
	{31,	32,		33,		34,		35,		36,}, \
}

#define PIN_IR_CONTROL					9

#define VK_NC							0xFF
#define KEY_TYPE_NONE					0
#define KEY_TYPE_IR						(BIT_MASK(0))
#define KEY_TYPE_BLE					(BIT_MASK(1))
#define KEY_TYPE_IR_BLE					(KEY_TYPE_IR | KEY_TYPE_BLE)


#define	DEFAULT_NEC_USER_CODE			0x807F
				/* IR      BLE key type        ble     WOBLE   */
#define KB_MAP_CODE		{\
{0,			KEY_TYPE_NONE,         	  		VK_NC,  	        VK_NC,        VK_NC,  },\
{1,			KEY_TYPE_IR_BLE,       		    0x20,  	     		0x07,  	      0x0080, },\
{2,			KEY_TYPE_IR_BLE,       			0x27,  	    	    0x0C,  	      0x0224, },\
{3,			KEY_TYPE_IR_BLE,       		    0x89,  	     		0x0C,  	      0x00ea, },\
{4,    		KEY_TYPE_IR_BLE,    	   		0x5C,  	        	0x0C,         0x00e2, },\
{5,			KEY_TYPE_IR_BLE,       			0x07,  		   		0x07,         0x0024, },\
{6,			KEY_TYPE_IR_BLE,      	 		0x08,      			0x07,         0x0025, },\
{7,    		KEY_TYPE_IR_BLE,    	   		0x50,  	        	0x0C,         0x000E, },\
{8,			KEY_TYPE_IR_BLE,       			0x03,  		    	0x07,         0x0020, },\
{9,			KEY_TYPE_IR_BLE,       		    0x04,  	     		0x07,  	      0x0021, },\
{10,		KEY_TYPE_IR_BLE,       		    0x2A,  	     		0x07,  	      0x0083, },\
{11,    	KEY_TYPE_IR_BLE,    	   		0x2B,  	        	0x07,         0x0081, },\
{12,    	KEY_TYPE_IR_BLE,    	   		0x05,  	        	0x07,         0x0022, },\
{13,		KEY_TYPE_IR_BLE,    	   		0x70,  	        	0x07,         0x003A, },\
{14,		KEY_TYPE_IR_BLE,       	   	 	0x02,  		    	0x07,         0x001f, },\
{15,		KEY_TYPE_IR_BLE,       		    0x01,  	        	0x07,  	      0x001e, },\
{16,		KEY_TYPE_IR_BLE,      	 		0x73, 	            0x0C,         0x0223, },\
{17,		KEY_TYPE_IR_BLE,     	  		0x1E,  	        	0x0C,         0x00D5, },\
{18,		KEY_TYPE_IR_BLE,       		    0x06,  	        	0x07,  	      0x0023, },\
{19,		KEY_TYPE_IR_BLE,       		    0x11,  	     		0x0C,  	      0x008D, },\
{20,		KEY_TYPE_IR_BLE,       	   	 	0x81,  		    	0x0C,         0x30,   },\
{21,		KEY_TYPE_IR,       	   	 		0x0C,  		    	VK_NC,        VK_NC,  },\
{22,		KEY_TYPE_IR_BLE,    	   		0x65,  	        	0x0C,         0x020A, },\
{23,		KEY_TYPE_IR_BLE,      	 		0x22,  		    	0x0C,         0x0009, },\
{24,		KEY_TYPE_IR_BLE,       		    0x09,  	        	0x07,  	      0x0026, },\
{25,		KEY_TYPE_IR_BLE,       		    0x59,  	        	0x0C,  	      0x000A, },\
{26,		KEY_TYPE_IR_BLE,     	  		0x26,  	       	  	0x07,  	      0x004B, },\
{27,		KEY_TYPE_IR_BLE,       	   	 	0x40,  		    	0x0C,         0x0043, },\
{28,		KEY_TYPE_IR_BLE,       			0x25,  		 		0x07,         0x004E, },\
{29,		KEY_TYPE_IR_BLE,    	   		0x51,  		    	0x0C,  	      0x0089, },\
{30,		KEY_TYPE_IR_BLE,      	 		0x7A,  	     	    0x07,         0x003C, },\
{31,		KEY_TYPE_IR_BLE,       			0x2E,  		    	0x0C,         0x000B, },\
{32,		KEY_TYPE_IR_BLE,       			0x38,  		    	0x0C,         0x0042, },\
{33,		KEY_TYPE_IR_BLE,       	   	 	0x39,  		    	0x0C,         0x0045, },\
{34,		KEY_TYPE_IR_BLE,       		  	0x13,  	        	0x0C,  	      0x0041, },\
{35,		KEY_TYPE_IR_BLE,     	  		0x77,  		    	0x07,  	      0x003D, },\
{36,		KEY_TYPE_IR_BLE,     	  		0x75,  		    	0x07,  	      0x003B  },\
{37,		KEY_TYPE_IR_BLE,     	  		0x5A,  		    	0x0C,  	      0x0221, },\
{38,		KEY_TYPE_IR_BLE,     	  		0x5D,  		    	0x0C,  	      0x002A, },\
{39,		KEY_TYPE_IR_BLE,     	  		0x37,  		    	0x0C,  	      0x0044, },\
{40,    	KEY_TYPE_IR_BLE,    	   		0x87,  	        	0x0C,         0x00e9, },\
{41,    	KEY_TYPE_IR,    	   		 	0xED,  	        	VK_NC,        VK_NC,  },\
{42,		KEY_TYPE_IR_BLE,       			0x00,  		  		0x07,         0x0027, },\
}

enum {
	REMOTE_HOLD_IDLE=0,			 /**空闲*/
	REMOTE_PAIR_HOLD,			 /**配对模式hold*/
	REMOTE_CLEAR_LEARN_INFO_HOLD,/**学习模式恢复*/
	REMOTE_CLEAR_BOND_INFO_HOLD, /**清除配对信息*/
	REMOTE_MAX_HOLD,             /**最大hold 用于分隔*/
};


/**
 * @brief RCU特殊模式
 */
enum {
	REMOTE_IDLE=0,				/**空闲*/
	
	REMOTE_PAIR_START,			/**开始配对	1*/	
	REMOTE_CON_DIS_WAIT,		/**等待断开	2*/
	REMOTE_PAIR_WAIT,			/**等待连接	3*/
	REMOTE_PAIR_FAIL,			/*配对失败	4*/
	REMOTE_PAIR_OK,				/*配对成功	5*/
	REMOTE_PAIR_END,			/**结束配对	6*/
    
  REMOTE_TASK_FINISH,           /*remote 任务结束*/
};

#define RECONNECTING_MAX_COUNT			10

#define RECONNECTION_ADV_TIMEOUT 		60
#define RECONNECTION_ADV_DIR_TIMEOUT   	60

//按键映射
#define KEY_POWER       19
#define KEY_AMBILIGHT   20
#define KEY_SOURCE      26
#define KEY_VOICE       27
#define KEY_MENU	    28
#define KEY_GUIDE		1
#define KEY_UP          2
#define KEY_INFO        3
#define KEY_LEFT        6
#define KEY_OK          7
#define KEY_RIGHT       8
#define KEY_OPTIONS     11
#define KEY_DOWN        12
#define KEY_SUBTITLE    13
#define KEY_BACK		16
#define KEY_TV			17
#define KEY_HOME		18
#define KEY_VOL_UP      21
#define KEY_VOL_DN      29
#define KEY_MUTE		24
#define KEY_123			22
#define KEY_CHN_UP      23
#define KEY_CHN_DN      25
#define KEY_LIST        30
#define KEY_RED         31
#define KEY_GREEN       32 
#define KEY_YELLOW      33
#define KEY_BLUE        34 
#define KEY_OTT_1       4
#define KEY_OTT_2       5
#define KEY_OTT_3       9
#define KEY_OTT_4       10
#define KEY_OTT_5       14
#define KEY_OTT_6       15

#define KEY_NUM_1       1
#define KEY_NUM_2       2
#define KEY_NUM_3       3
#define KEY_NUM_4       6
#define KEY_NUM_5       7
#define KEY_NUM_6       8
#define KEY_NUM_7       11
#define KEY_NUM_8       12
#define KEY_NUM_9       13
#define KEY_NUM_0       17
#define KEY_REWIND      31
#define KEY_PAUSE       32
#define KEY_PLAY        33
#define KEY_FFWD        34

#define PAIR_KEY0_INDEX					KEY_BACK//BACK
#define PAIR_KEY1_INDEX					KEY_HOME//HOME

#define CLEAR_BOND_INFO_KEY1			0xFF
#define CLEAR_BOND_INFO_KEY2			0xFF
#define CLEAR_BOND_INFO_HOLD_TIMEOUT	5*1000
#define PAIR_ADV_TIMEOUT				60
#define PAIR_HOLD_TIMEOUT				3000

#define PAIR_WAIT_CON_DIS_TIMEOUT		2*1000

#define POWER_ADV_TIMEOUT				20
#define NETFLIX_ADV_TIMEOUT				30
#define GOOGLE_ADV_TIMEOUT				10

#define VOICE_AUDIO_KEY_INDEX   		KEY_VOICE
#define APP_AUDIO_GOOGLE_TIMEOUT		20*1000
#define OK_KEY_INDEX					KEY_OK

#define SINGLE_CARRIER_KEY1				3//vol-
#define SINGLE_CARRIER_KEY2				3//vol-

#define POWER_KEY_INDEX					KEY_POWER	//box power
#define NETFLIX_KEY_INDEX				7
#define	STUCK_KEY_TIMEOUT     			30000
#define HOME_KEY_INDEX					KEY_HOME

//产测组合键
#define PAIR_ENTER_KEY			34//ok
#define FACTORY_TEST_KEY0		20	//POWER
#define FACTORY_TEST_KEY1		32	//上下左右
#define FACTORY_TEST_KEY2		39
#define FACTORY_TEST_KEY3		33
#define FACTORY_TEST_KEY4		27
#define FACTORY_TEST_KEY5		40
#define FACTORY_TEST_KEY6		3
#define FACTORY_TEST_KEY7		0xff
#define FACTORY_TEST_KEY8		0xff

#define FACTORY_RECOVERY_KEY1		0xFF	
#define FACTORY_RECOVERY_KEY2		0xFF	
#define FACTORY_TEST_HOLD_TIMEOUT	2*1000
#define FACTORY_TEST_ADV_TIMEOUT	15*1000

#define VERSION_STR		"1.1.1.250604" //1:主；1:次;  0:release;1~9:debug
#define VERSION_LEN		 12

//OTA地址
#define OTA_APP_ADDRESS			0x00018000
#define OTA_APP_FLASH_SIZE		0x00013000


#endif //__BOARD_DEMO_H
