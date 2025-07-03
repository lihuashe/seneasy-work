/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     aw21036_driver.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef AW21036_DRIVER_H_
#define AW21036_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define AW210XX_CHIPID				0x18
#define AW21036_DEVICE_ADDR			(0x34 << 1)  /* AD0——>GND */
#define AW21036_LED_NUM				36

#define AW_LED_ON                   1
#define AW_LED_OFF                  0

extern u8 LED_BR0_Data;
extern u8 LED_COL0_Data;
extern u8 Quick_WaitMs_Flag;
extern _attribute_data_retention_ u8 BackLight_Connect_Init;
extern _attribute_data_retention_ bool BR_TEST_CLOSE_FLAG;
extern _attribute_data_retention_ bool BR_TEST_LED_DUTY_FLAG;
/*============================================================================*
 *                          Functions
 *============================================================================*/
extern void aw21036_init(void);
extern void aw21036_Disable(void);
extern void aw21036_all_on(u8 set_br0, u8 set_col0);
extern void aw21036_all_off(void);
extern void aw21036_all_breath(void);
extern void aw21036_breath_off(void);
extern void aw21036_breath_one(u8 led_cnt, u8 led_flag, u8 col_val, u8 br_val);
extern void aw21036_onoff_one(u8 led_cnt, u8 led_flag, u8 col_val, u8 br_val);

extern void aw21036_set_brightness(u8 index, u8 brightness);
extern void aw21036_set_color(u8 index, u8 color);
extern void aw21036_update_brightness_to_display(void);

extern void aw21036_breath_quick(u8 led_flag);
extern void aw21036_init_TEST(void);
/************************************************************
*               led_timer_T0 --- Ramp rise time
*               led_timer_T1 --- Hold on   time
*               led_timer_T2 --- Ramp fall time
*               led_timer_T3 --- Hold off  time

        T0/T2       Time      |     T0/T2       Time            
        0x00        0.00s     |     0x80        2.1s
        0x10        0.13s     |     0x90        2.6s
        0x20        0.26s     |     0xA0        3.1s
        0x30        0.38s     |     0xB0        4.2s
        0x40        0.51s     |     0xC0        5.2s
        0x50        0.77s     |     0xD0        6.2s
        0x60        1.04s     |     0xE0        7.3s
        0x70        1.60s     |     0xF0        8.3s     
 ***********************************************************/
#define led_timer_T0            0x10
#define led_timer_T1            0x01
#define led_timer_T2            0x60
#define led_timer_T3            0x06
#define led_on_wait_ms          1//1130
#define led_off_wait_ms         1//2100

#define led_timer_key_T2        0x10
#define led_timer_key_T3        0x01
#define led_off_key_wait_ms     1//130

/************************************************************
*  LED_Backlight_Level
*  level: 0--6
 ***********************************************************/
#define BR_LUX_LEVEL_0          71      //30      //<30 Lux
#define BR_LUX_LEVEL_1          234     //100     //30 -100 Lux
#define BR_LUX_LEVEL_2          468     //200     //100-200 Lux
#define BR_LUX_LEVEL_3          704     //300     //200-300 Lux
#define BR_LUX_LEVEL_4          1204    //500     //300-500 Lux
#define BR_LUX_LEVEL_5          1679    //700     //500-700 Lux

/************************************************************
*  LED_COL_1~36
*  level: 0x00--0xFF  
 ***********************************************************/
#define LED_COL_ALL     0x7F            //50%   (ALL_LED)
#define LED_COL_TEMP_1  0x4C            //30%   (1、3、7、9、0、VOL+-、CHN+-)      
#define LED_COL_TEMP_2  0xCC            //80%   (2、5、6、8)

#define LED_COL_1       LED_COL_TEMP_1  //NUM_1
#define LED_COL_2       LED_COL_TEMP_2  //NUM_2
#define LED_COL_3       LED_COL_TEMP_1  //NUM_3
#define LED_COL_4       LED_COL_ALL     //NUM_4
#define LED_COL_5       LED_COL_TEMP_2  //NUM_5
#define LED_COL_6       LED_COL_TEMP_2  //NUM_6
#define LED_COL_7       LED_COL_TEMP_1  //NUM_7
#define LED_COL_8       LED_COL_TEMP_2  //NUM_8
#define LED_COL_9       LED_COL_TEMP_1  //NUM_9
#define LED_COL_10      LED_COL_TEMP_1  //NUM_10
#define LED_COL_11      LED_COL_ALL     //REWIND
#define LED_COL_12      LED_COL_ALL     //PAUSE
#define LED_COL_13      LED_COL_ALL     //PLAY
#define LED_COL_14      LED_COL_ALL     //FFWD
#define LED_COL_15      LED_COL_ALL     
#define LED_COL_16      LED_COL_ALL     //123
#define LED_COL_17      LED_COL_ALL     //SOURCE
#define LED_COL_18      LED_COL_ALL     //BACK
#define LED_COL_19      LED_COL_ALL     //OTT_1
#define LED_COL_20      LED_COL_ALL     //VOICE
#define LED_COL_21      LED_COL_ALL     //HOME
#define LED_COL_22      LED_COL_ALL     //MENU
#define LED_COL_23      LED_COL_ALL     //POWER
#define LED_COL_24      LED_COL_ALL     //OTT_3
#define LED_COL_25      LED_COL_TEMP_1  //VOL_L
#define LED_COL_26      LED_COL_ALL     //OTT_2
#define LED_COL_27      LED_COL_TEMP_1  //VOL_R
#define LED_COL_28      LED_COL_ALL     //OTT_5
#define LED_COL_29      LED_COL_TEMP_1  //CHN_L
#define LED_COL_30      LED_COL_ALL     //OTT_4
#define LED_COL_31      LED_COL_TEMP_1  //CHN_R
#define LED_COL_32      LED_COL_ALL     //OTT_6
#define LED_COL_33      LED_COL_ALL     //AMBILIGHT
#define LED_COL_34      LED_COL_ALL     
#define LED_COL_35      LED_COL_ALL     
#define LED_COL_36      LED_COL_ALL
/************************************************************
*  LED_BR_1~36
*  level: 0x00--0xFF  
 ***********************************************************/
/*
#define LED_BR_ALL      0x0c
#define LED_BR_1        0x0c
#define LED_BR_2        0x0c
#define LED_BR_3        0x0c
#define LED_BR_4        0x0c
#define LED_BR_5        0x0c
#define LED_BR_6        0x0c
#define LED_BR_7        0x0c
#define LED_BR_8        0x0c
#define LED_BR_9        0x0c
#define LED_BR_10       0x0c
#define LED_BR_11       0x0c
#define LED_BR_12       0x0c
#define LED_BR_13       0x0c
#define LED_BR_14       0x0c
#define LED_BR_15       0x0c
#define LED_BR_16       0x0c
#define LED_BR_17       0x0c
#define LED_BR_18       0x0c
#define LED_BR_19       0x0c
#define LED_BR_20       0x0c
#define LED_BR_21       0x0c
#define LED_BR_22       0x0c
#define LED_BR_23       0x0c
#define LED_BR_24       0x0c
#define LED_BR_25       0x0c
#define LED_BR_26       0x0c
#define LED_BR_27       0x0c
#define LED_BR_28       0x0c
#define LED_BR_29       0x0c
#define LED_BR_30       0x0c
#define LED_BR_31       0x0c
#define LED_BR_32       0x0c
#define LED_BR_33       0x0c
#define LED_BR_34       0x0c
#define LED_BR_35       0x0c
#define LED_BR_36       0x0c
*/

#define LED_BR_ALL      0xFF
#define LED_BR_1        0xFF
#define LED_BR_2        0xFF
#define LED_BR_3        0xFF
#define LED_BR_4        0xFF
#define LED_BR_5        0xFF
#define LED_BR_6        0xFF
#define LED_BR_7        0xFF
#define LED_BR_8        0xFF
#define LED_BR_9        0xFF
#define LED_BR_10       0xFF
#define LED_BR_11       0xFF
#define LED_BR_12       0xFF
#define LED_BR_13       0xFF
#define LED_BR_14       0xFF
#define LED_BR_15       0xFF
#define LED_BR_16       0xFF
#define LED_BR_17       0xFF
#define LED_BR_18       0xFF
#define LED_BR_19       0xFF
#define LED_BR_20       0xFF
#define LED_BR_21       0xFF
#define LED_BR_22       0xFF
#define LED_BR_23       0xFF
#define LED_BR_24       0xFF
#define LED_BR_25       0xFF
#define LED_BR_26       0xFF
#define LED_BR_27       0xFF
#define LED_BR_28       0xFF
#define LED_BR_29       0xFF
#define LED_BR_30       0xFF
#define LED_BR_31       0xFF
#define LED_BR_32       0xFF
#define LED_BR_33       0xFF
#define LED_BR_34       0xFF
#define LED_BR_35       0xFF
#define LED_BR_36       0xFF

/************************************************************
*  LED_KEY  
 ***********************************************************/
#define LED_POWER           23
#define LED_AMBILIGHT       33
#if (PCB_LED_VER == PCB_LED_INIT)
#define LED_SOURCE          17
#elif (PCB_LED_VER == PCB_LED_POINT)
#define LED_SOURCE          34
#endif
#define LED_VOICE           20
#define LED_MENU            22
#define LED_NUM_1           1
#define LED_NUM_2           2
#define LED_NUM_3           3
#define LED_NUM_4           4
#define LED_NUM_5           5
#define LED_NUM_6           6
#define LED_NUM_7           7
#define LED_NUM_8           8
#define LED_NUM_9           9
#define LED_BACK            18
#define LED_NUM_0           10
#define LED_HOME            21
#define LED_123             16
#define LED_REWIND          11
#define LED_PAUSE           12
#define LED_PLAY            13
#define LED_FFWD            14
#define LED_OTT_1           19
#define LED_OTT_2           26
#define LED_OTT_3           24
#define LED_OTT_4           30
#define LED_OTT_5           28
#define LED_OTT_6           32
#define LED_VOL_L           25
#define LED_VOL_R           27
#define LED_CHN_L           29
#define LED_CHN_R           31
/************************************************************
 *
 * Register List
 *
 ***********************************************************/
#define REG_GCR					0x00
#define REG_BR0					0x01
#define REG_BR1					0x02
#define REG_BR2					0x03
#define REG_BR3					0x04
#define REG_BR4					0x05
#define REG_BR5					0x06
#define REG_BR6					0x07
#define REG_BR7					0x08
#define REG_BR8					0x09
#define REG_BR9					0x0A
#define REG_BR10				0x0B
#define REG_BR11				0x0C
#define REG_BR12				0x0D
#define REG_BR13				0x0E
#define REG_BR14				0x0F
#define REG_BR15				0x10
#define REG_BR16				0x11
#define REG_BR17				0x12
#define REG_BR18				0x13
#define REG_BR19				0x14
#define REG_BR20				0x15
#define REG_BR21				0x16
#define REG_BR22				0x17
#define REG_BR23				0x18
#define REG_BR24				0x19
#define REG_BR25				0x1A
#define REG_BR26				0x1B
#define REG_BR27				0x1C
#define REG_BR28				0x1D
#define REG_BR29				0x1E
#define REG_BR30				0x1F
#define REG_BR31				0x20
#define REG_BR32				0x21
#define REG_BR33				0x22
#define REG_BR34				0x23
#define REG_BR35				0x24
#define REG_UPDATE				0x49
#define REG_COL0				0x4a
#define REG_COL1				0x4b
#define REG_COL2				0x4c
#define REG_COL3				0x4d
#define REG_COL4				0x4e
#define REG_COL5				0x4f
#define REG_COL6				0x50
#define REG_COL7				0x51
#define REG_COL8				0x52
#define REG_COL9				0x53
#define REG_COL10				0x54
#define REG_COL11				0x55
#define REG_COL12				0x56
#define REG_COL13				0x57
#define REG_COL14				0x58
#define REG_COL15				0x59
#define REG_COL16				0x5a
#define REG_COL17				0x5b
#define REG_COL18				0x5c
#define REG_COL19				0x5d
#define REG_COL20				0x5e
#define REG_COL21				0x5f
#define REG_COL22				0x60
#define REG_COL23				0x61
#define REG_COL24				0x62
#define REG_COL25				0x63
#define REG_COL26				0x64
#define REG_COL27				0x65
#define REG_COL28				0x66
#define REG_COL29				0x67
#define REG_COL30				0x68
#define REG_COL31				0x69
#define REG_COL32				0x6a
#define REG_COL33				0x6b
#define REG_COL34				0x6c
#define REG_COL35				0x6d
#define REG_GCCR				0x6e
#define REG_PHCR				0x70
#define REG_OSDCR				0x71
#define REG_OSST0				0x72
#define REG_OSST1				0x73
#define REG_OSST2				0x74
#define REG_0SST3				0x75
#define REG_OSST4				0x76
#define REG_OTCR				0x77
#define REG_SSCR				0x78
#define REG_UVCR				0x79
#define REG_GCR2				0x7A
#define REG_GCR4				0x7C
#define REG_VER					0x7E
#define REG_RESET				0x7F
#define REG_WBR					0x90
#define REG_WBG					0x91
#define REG_WBB					0x92
#define REG_PATCFG				0xA0
#define REG_PATGO				0xA1
#define REG_PATCT0				0xA2
#define REG_PATCT1				0xA3
#define REG_PATCT2				0xA4
#define REG_PATCT3				0xA5
#define REG_FADEH				0xA6
#define REG_FADEL				0xA7
#define REG_GCOLR				0xA8
#define REG_GCOLG				0xA9
#define REG_GCOLB				0xAA
#define REG_GCFG0				0xAB
#define REG_GCFG1				0xAC

#endif /* AW21036_DRIVER_H_ */
