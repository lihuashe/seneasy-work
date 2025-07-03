/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bath_rcu.h
* @brief
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _BATH_RCU_H_
#define _BATH_RCU_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "AAA_app_config.h"
#include "driver.h"
#include "AAA_project_config.h"
#include "utility.h"

/****************************代码补缺区*******************************/


typedef enum
{
	FN_PRESS_AAA = BIT(0),
	T_BIND_PRESS_AAA = BIT(1),
	MOUSE_KEY_PRESS_AAA = BIT(2),
	T_ESC_PRESS_AAA = BIT(3),
	T_Q_PRESS_AAA = BIT(4),
	T_W_PRESS_AAA = BIT(5),
	T_E_PRESS_AAA = BIT(6),
	T_R_PRESS_AAA = BIT(7),
	T_T_PRESS_AAA = BIT(8),
	T_Y_PRESS_AAA = BIT(9),
	T_U_PRESS_AAA = BIT(10),
	T_I_PRESS_AAA = BIT(11),
	T_O_PRESS_AAA = BIT(12),
	T_F1_PRESS_AAA = BIT(13),
	T_SPACE_PRESS_AAA = BIT(14),
	T_KB_1_PRESS_AAA = BIT(15),
	T_KB_2_PRESS_AAA = BIT(16),
	T_KB_3_PRESS_AAA = BIT(17),
	T_KB_4_PRESS_AAA = BIT(18),
	T_LCTRL_PRESS_AAA = BIT(19),
} SPECIAL_KEY_PRESS_FLAG_AAA;

#define MAX_BTN_CNT_AAA 6
typedef struct
{
	u8 cnt;
	u8 keycode[MAX_BTN_CNT_AAA];
	u8 press_cnt;
	u32 special_key_press_f;
} kb_data_t_aaa;

enum
{
	RF_1M_BLE_MODE = 0,
	RF_2M_2P4G_MODE = 1,
	USB_MODE = 2,
};
#define _attribute_data_retention_user _attribute_data_retention_

volatile extern u8 PAIR_STATUS;

extern my_fifo_t fifo_km;


_attribute_data_retention_user extern u8 device_channel;
/*============================================================================*
 *                        Export Global Variabless
 *============================================================================*/
/*起始码*/
#define PROTOCOL_HEAD 0x4C
/*功能执行码  Bit4-0*/
#define CMD_STOP 0x00		  // 01100 全自动
#define CMD_AUTO_STOP 0x0C		  // 01100 全自动
#define CMD_FLUSH_WATER_BIG 0x0E  // 01110 冲刷排水全排
#define CMD_FLUSH_WATER_SML 0x0F  // 01111 冲刷排水半排
#define CMD_SYSTEM_FUNCTION 0x1F  // 11111 系统(翻盖/翻圈/脚感/预湿润/氛围灯)
#define CMD_BUTTOCK_WASH 0x08	  // 01000 臀洗移动
#define CMD_WOMEN_WASH 0x0B		  // 01011 妇洗移动
#define CMD_WARM_WIND 0x04		  // 00100 烘干
#define CMD_AID_DEFECATE 0x1B	  // 11011 强洗(助便)移动
#define CMD_ADJUST_PARAMETER 0x19 // 11001 参数调节(水温/座温/风温/夜灯开关/自动冲刷)
#define CMD_ADJUST_WASH 0x18	  // 11000 冲洗调节(位置/水压)
#define CMD_OUT_STINK 0x12		  // 10010 除臭
#define CMD_FEEL_TEMPERATURE 0x02 // 00010 智能温控
#define CMD_CLEAN_NOZZLE 0x09	  // 01001 喷嘴清洗(自动保养)
#define CMD_CHILD_WASH 0x0D		  // 01101 童洗


/* 2.4G CMD */
#define CMD_24G_PAIR_CMD 0x55
#define CMD_24G_PAIR_ACK 0x88

#define CMD_24G_AUTO_STOP 0x11	   // 自动/停止
#define CMD_24G_FLUSH_WATER_B 0x12 // 大冲
#define CMD_24G_FLUSH_WATER_S 0x13 // 小冲
#define CMD_24G_FLIP_COVER 0x05	   // 翻盖
#define CMD_24G_FLIP_CIRCLE 0x08   // 翻圈
#define CMD_24G_BUTTOCK_WASH 0x0D  // 臀洗
#define CMD_24G_WOMEN_WASH 0x09	   // 妇洗
#define CMD_24G_WARM_WIND 0x01	   // 暖风
#define CMD_24G_AID_DEFECATE 0x04  // 助便
#define CMD_24G_SEAT_TEMP_UP 0x0C  // 座温-Up
#define CMD_24G_SEAT_TEMP_DN 0x10  // 座温-Dn
#define CMD_24G_WATER_TEMP_UP 0x07 // 水温-Up
#define CMD_24G_WATER_TEMP_DN 0x0F // 水温-Dn
#define CMD_24G_WATER_PRES_UP 0x0B // 水压-Up
#define CMD_24G_WATER_PRES_DN 0x03 // 水压-Dn
#define CMD_24G_SEAT_POSI_UP 0x0A  // 位置-Up
#define CMD_24G_SEAT_POSI_DN 0x02  // 位置-Dn
#define CMD_24G_WIND_TEMP_UP 0x06  // 风温-Up
#define CMD_24G_WIND_TEMP_DN 0x0E  // 风温-Dn

// 档位默认值
extern _attribute_data_retention_ u8 set_pos_water_temperature;
extern _attribute_data_retention_ u8 set_pos_seat_temperature;
extern _attribute_data_retention_ u8 set_pos_water_pressure;
extern _attribute_data_retention_ u8 set_pos_seat_position;
extern _attribute_data_retention_ u8 set_pos_wind_temperature;
extern _attribute_data_retention_ u8 set_pos_water_pressure_FX;
extern _attribute_data_retention_ u8 set_pos_seat_position_FX;

extern _attribute_data_retention_ u8 set_fun_foot_feel;
extern _attribute_data_retention_ u8 set_fun_night_light;
extern _attribute_data_retention_ u8 set_fun_pre_humid;
extern _attribute_data_retention_ u8 set_fun_atmosphere_light;
extern _attribute_data_retention_ u8 set_fun_filter_element;


extern _attribute_data_retention_user u8 bath_send_need_f;
extern _attribute_data_retention_ u8 tick_key_women_wash_fx;
extern _attribute_data_retention_ bool DEVICE_PAIRED_STATUS;

/*============================================================================*
 *                          Functions
 *============================================================================*/
void led_to_power_on(void);
void led_position_seat_temp(void);
void led_position_water_temp(void);
void led_position_water_pres(void);
void led_position_wind_temp(void);
extern void Send_Bath_24G_Data(u8 bath_cmd, u8 sys_1, u8 sys_2,u8 key_vaule);
void Send_24G_Pair_Success(void);

void pressed_auto_stop_key(void);
void pressed_flush_water_B_key(void);
void pressed_flush_water_B_key_release(void);
void pressed_flush_water_S_key(void);
void pressed_flush_water_S_key_release(void);
void pressed_flip_cover_key(void);
void pressed_flip_cover_key_release(void);
void pressed_flip_circle_key(void);
void pressed_flip_circle_key_release(void);
void pressed_buttock_wash_key(void);
void pressed_women_wash_key(void);
void pressed_women_wash_key_release(void);
void pressed_warm_wind_key(void);
void pressed_aid_defecate_key(void);
void pressed_seat_temp_up_key(void);
void pressed_seat_temp_dn_key(void);
void pressed_seat_temp_dn_key_release(void);
void pressed_water_temp_up_key(void);
void pressed_water_temp_dn_key(void);
void pressed_water_temp_dn_key_release(void);
void pressed_water_pres_up_key(void);
void pressed_water_pres_dn_key(void);
void pressed_water_pres_dn_key_release(void);
void pressed_seat_posi_up_key(void);
void pressed_seat_posi_dn_key(void);
void pressed_seat_posi_dn_key_release(void);
void pressed_wind_temp_up_key(void);
void pressed_wind_temp_dn_key(void);
void pressed_wind_temp_dn_key_release(void);
void pressed_filter_element_reset(void);
void pressed_filter_element_stop(void);
void Bath_Rcu_key0_Handle(u8 key0);
void Bath_Rcu_key2_Handle(u8 key0, u8 key1);
void Send_Pair_24G_Data();

#endif /* BATH_RCU_H_ */
