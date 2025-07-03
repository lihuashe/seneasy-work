/**
**********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     lp4081h_driver.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-20
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _LP4081H_DRIVER_H_
#define _LP4081H_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <board.h>

#define LP4081H_ADDR			            0x6B    //实际需要0x6B << 1  
#define LP4081H_ADDR_READ                   (LP4081H_ADDR << 1) | 1
#define NEED_SEND_LP4081H_ADDR              0
#define LP4081H_MOV_L_BIT(x,n)              (x << n)      


/*
RO (default: 0x00) 
状态寄存器0（地址00H）：
位	符号	        描述	                                                      读/写	 默认值
7	Trickle_TE	涓流充电定时器到期。	                                            r	    0
6	Term_TE	    快速充电定时器到期。	                                            r	    0
5	WTD_TE	    看门狗定时器到期。	                                                r	    0
4	BAT_UV	    电池欠压锁定（UVLO）。	                                            r	    0
3	SCP	        系统短路。	                                                       r	   0
2	CHG	        充电状态：00: 不充电, 01: 涓流充电, 10: 恒流/恒压, 11: 充电结束。	  r	      0
1	CHG	        与位2相同。	                                                       r	   0
0	PGD	        输入在欠压锁定（UVLO）和过压保护（OVP）之间。	                      r       0
*/
#define Lp4081h_Status_Register_0           0x00        

/*
RO (default: 0x03) 
状态寄存器1（地址01H）：
位	符号	描述	                                                                        读/写	默认值
2	NTC	    00: 无效, 001: <0°C, 010: 0-10°C, 011: 10-45°C, 100: >45°C, 101/110/111: 无效	r	    0
1	NTC	    与位2相同。                                                                     r	    1
0	NTC	    与位2相同。                                                                     r	    1
*/
#define Lp4081h_Status_Register_1           0x01        

/*
RW (default: 0x00)
中断屏蔽寄存器（地址：02H）                       
位	符号	 描述	                    读/写   默认值
7	保留		                        r/w	      0                                                            
6	mBAT_UV	电池欠压中断屏蔽             r/w	   0
5	mEOC	充电结束中断屏蔽             r/w	   0
4	mSCP	短路保护中断屏蔽             r/w	   0
3	mCHG	充电状态中断屏蔽             r/w	   0
2	mPGD	电源故障检测中断屏蔽	      r/w	   0
1	mTimer	定时器中断屏蔽	             r/w	   0
0	mNTC	负温度系数热敏电阻中断屏蔽	  r/w	    0
*/
#define Lp4081h_Interrupt_Mask_Register     0x02        

/*
RW (default: 0x3E)
控制寄存器0（地址：03H）
位	符号	    描述	                                                         读/写	默认值
7	保留		 -
6	保留		 -	                                                             -
5	ICC_EXT	    充电电流扩展：1表示所有相关电流乘以2.5，0表示默认设置	             r/w	 1
4	IDIS_LIM	放电电流限制：00: 440mA, 01: 1A, 10: 2A, 11: 3A                   r	      1
3	IDIS_LIM	放电电流限制：与位5相同	                                           r	   1
2	IIN_LIM	    输入电流限制：1表示1.5A，0表示650mA	                               r	   1               
1	DIS[1]	    BAT FET操作模式控制：00: 旁路, 01: 掉电8秒, 10: 自动, 11: 船模式	r/w	    1
0	DIS[0]	    BAT FET操作模式控制：与位2相同	                                   r/w     0
*/
#define Lp4081h_Control_Register0           0x03        

/*
RW (default: 0xE4)
控制寄存器1（地址：04H）
位	符号	     描述	                                                                                              读/写	 默认值
7	Iter[2]	    涓流和终止电流设置：000: 1mA, 001: 3mA, 010: 5mA, 011: 7mA, 100: 9mA, 101: 11mA, 110: 13mA, 111: 15mA	r/w	  111
6	Iter[1]	    涓流和终止电流设置：与位7相同	                                                                         r/w	-
5	Iter[0]	    涓流和终止电流设置：与位7相同	                                                                         r/w	-
4	BAT_CV[4]	电池CV电压设置：4.0V到4.5V的多个选项	                                                                 r/w    00100 (4.2V)
3	BAT_CV[3]	电池CV电压设置：与位4相同	                                                                             r/w	-
2	BAT_CV[2]	电池CV电压设置：与位4相同	                                                                             r/w	-
1	BAT_CV[1]	电池CV电压设置：与位4相同	                                                                             r/w	-
0	BAT_CV[0]	电池CV电压设置：与位4相同	                                                                             r/w    -
*/
#define Lp4081h_Control_Register1           0x04        

/*
RW (default: 0XF0)
控制寄存器2（地址：05H）
位	符号	描述	                              读/写    默认值
7	EN_CHG	1表示启用充电，0表示禁用充电	        r/w	      1
6	VTRI	涓流充电阈值设置：1表示3.0V，0表示2.8V	r/w	      1
5	ICC[5]	恒定电流设置：25mA到1175mA的多个选项	r/w	      110000 (800mA)
4	ICC[4]	恒定电流设置：与位5相同	                r/w	      -
3	ICC[3]	恒定电流设置：与位5相同	                r/w	      -
2	ICC[2]	恒定电流设置：与位5相同	                r/w	      -
1	ICC[1]	恒定电流设置：与位5相同	                r/w	      -
0	ICC[0]	恒定电流设置：与位5相同	                r/w	      -
*/
#define Lp4081h_Control_Register2           0x05       


/*
RW (default: 0x2B)
控制寄存器3（地址：06H）
位	符号	        描述	                                                                                               读/写	默认值
7	WTD	写入WTD=1   将重置看门狗定时器。SoC需要在看门狗定时器计数结束前将此位写入1。	                                         r/w	  0
6	EN_WTD	        1表示启用看门狗功能，0表示禁用看门狗功能。	                                                               r/w	    0
5	EN_NTC	        1表示启用NTC电路，0表示禁用NTC电路。	                                                                  r/w	   1
4	EN_Tri_Timer	1表示启用0.75小时的涓流充电安全定时器，0表示禁用0.75小时的涓流充电安全定时器。                                r/w	   0
3	EN_Term	        1表示在EOC后停止充电，0表示在EOC后继续充电，直到3.75小时的安全定时器到期。	                                 r/w	  1
2	DPM[2]	        VSYS DPM阈值：000: 3.0V, 001: 3.2V, 010: 3.4V, 011: 3.6V, 100: 3.8V, 101: 4.0V, 110: 4.2V, 111: 4.4V    r/w	     011
1	DPM[1]	        与DPM[2]相同。                                                                                      	r/w	     -
0	DPM[0]	        与DPM[2]相同。                                                                                      	r/w	     -
 */
#define Lp4081h_Control_Register3           0x06        


typedef enum
{
    LP4081H_TRICKLE_TE  = LP4081H_MOV_L_BIT(1,7),
    LP4081H_TERM_TE	    = LP4081H_MOV_L_BIT(1,6),
    LP4081H_WTD_TE	    = LP4081H_MOV_L_BIT(1,5),  
    LP4081H_BAT_UV	    = LP4081H_MOV_L_BIT(1,4),	  
    LP4081H_SCP		    = LP4081H_MOV_L_BIT(1,3),      
    LP4081H_CHG_DONE    = LP4081H_MOV_L_BIT(1,2) + LP4081H_MOV_L_BIT(1,1),  
    LP4081H_CONST_CHG	= LP4081H_MOV_L_BIT(1,2),  
    LP4081H_TRICKLE_CHG = LP4081H_MOV_L_BIT(1,1),   
    LP4081H_NO_CHG	    = 0,   
    LP4081H_PGD	  	    = 1    

}ENUM_LP4081H_BAT_STATUS_DEF;

typedef enum 
{
    LP4081H_INT_RESERVED = LP4081H_MOV_L_BIT(1,7),
    LP4081H_INT_BAT_UV  = LP4081H_MOV_L_BIT(1,6),
    LP4081H_INT_EOC     = LP4081H_MOV_L_BIT(1,5),
    LP4081H_INT_SCP     = LP4081H_MOV_L_BIT(1,4),
    LP4081H_INT_CHG     = LP4081H_MOV_L_BIT(1,3),
    LP4081H_INT_PGD     = LP4081H_MOV_L_BIT(1,2),
    LP4081H_INT_TIMER   = LP4081H_MOV_L_BIT(1,1),
    LP4081H_INT_NTC     = 1

}ENUM_LP4081H_INTTERUPT_MASK_DEF;

typedef enum
{
    LP4081H_BYPASS_MODE = 0,
    LP4081H_POWER_DOWN_8_SECONDS_MODE = 1,
    LP4081H_AUTO_MODE  = LP4081H_MOV_L_BIT(1,1),
    LP4081H_SHIP_MODE  = LP4081H_MOV_L_BIT(1,1) + 1,

}ENUM_LP4081H_BAT_FET_MODE_DEF;

typedef enum
{
    LP4081H_RESET_WTD       = LP4081H_MOV_L_BIT(1,7),
    LP4081H_EN_WTD          = LP4081H_MOV_L_BIT(1,6),
    LP4081H_EN_NTC          = LP4081H_MOV_L_BIT(1,5),
    LP4081H_EN_TRI_TIMER    = LP4081H_MOV_L_BIT(1,4),
    LP4081H_EN_EOC_CONTINUE = LP4081H_MOV_L_BIT(1,3)

}ENUM_LP4081H_BAT_MANAGEMENT_DEF;


typedef enum
{
    LP4081H_VsysDpm_3_0V = 0,
    LP4081H_VsysDpm_3_2V = 1,
    LP4081H_VsysDpm_3_4V = 2,
    LP4081H_VsysDpm_3_6V = 3,
    LP4081H_VsysDpm_3_8V = 4,
    LP4081H_VsysDpm_4_0V = 5,
    LP4081H_VsysDpm_4_2V = 6,
    LP4081H_VsysDpm_4_4V = 7,

}ENUM_LP4081H_BAT_VSYSDPM_DEF;


typedef enum
{
    LP4081H_DisCurLimit_440mA  = 0,
    LP4081H_DisCurLimit_1000mA = 1,
    LP4081H_DisCurLimit_2000mA = 2,
    LP4081H_DisCurLimit_3000mA = 3,

}ENUM_LP4081H_DISCUR_LIMIT_DEF;


typedef enum
{
    LP4081H_CurLimit_650mA  = 0,
    LP4081H_CurLimit_1500mA = 1,

}ENUM_LP4081H_CUR_LIMIT_DEF;

typedef enum {
    LP4081H_Iter_1mA = 0,   // 000
    LP4081H_Iter_3mA = 1,   // 001
    LP4081H_Iter_5mA = 2,   // 010
    LP4081H_Iter_7mA = 3,   // 011
    LP4081H_Iter_9mA = 4,   // 100
    LP4081H_Iter_11mA = 5,  // 101
    LP4081H_Iter_13mA = 6,  // 110
    LP4081H_Iter_15mA = 7   // 111
}ENUM_LP4081H_ITER_DEF;

typedef enum
{
    LP4081H_TriVolLimit_2_8V = 0,
    LP4081H_TriVolLimit_3_0V = 1,

}ENUM_LP4081H_TRI_VOL_LIMIT_DEF;


typedef enum {
    /* 25mA 到 450mA，步长 12.5mA */
    LP4081H_ConstCur_25mA   = 0,
    LP4081H_ConstCur_37_5mA = 1,
    LP4081H_ConstCur_50mA   = 2,
    LP4081H_ConstCur_62_5mA = 3,
    LP4081H_ConstCur_75mA   = 4,
    LP4081H_ConstCur_87_5mA = 5,
    LP4081H_ConstCur_100mA  = 6,
    LP4081H_ConstCur_112_5mA = 7,
    LP4081H_ConstCur_125mA  = 8,
    LP4081H_ConstCur_137_5mA = 9,
    LP4081H_ConstCur_150mA  = 10,
    LP4081H_ConstCur_162_5mA = 11,
    LP4081H_ConstCur_175mA  = 12,
    LP4081H_ConstCur_187_5mA = 13,
    LP4081H_ConstCur_200mA  = 14,
    LP4081H_ConstCur_212_5mA = 15,
    LP4081H_ConstCur_225mA  = 16,
    LP4081H_ConstCur_237_5mA = 17,
    LP4081H_ConstCur_250mA  = 18,
    LP4081H_ConstCur_262_5mA = 19,
    LP4081H_ConstCur_275mA  = 20,
    LP4081H_ConstCur_287_5mA = 21,
    LP4081H_ConstCur_300mA  = 22,
    LP4081H_ConstCur_312_5mA = 23,
    LP4081H_ConstCur_325mA  = 24,
    LP4081H_ConstCur_337_5mA = 25,
    LP4081H_ConstCur_350mA  = 26,
    LP4081H_ConstCur_362_5mA = 27,
    LP4081H_ConstCur_375mA  = 28,
    LP4081H_ConstCur_387_5mA = 29,
    LP4081H_ConstCur_400mA  = 30,
    LP4081H_ConstCur_412_5mA = 31,
    LP4081H_ConstCur_425mA  = 32,
    LP4081H_ConstCur_437_5mA = 33,
    LP4081H_ConstCur_450mA  = 34,

    /* 450mA 到 1175mA，步长 25mA */
    LP4081H_ConstCur_475mA  = 35,
    LP4081H_ConstCur_500mA  = 36,
    LP4081H_ConstCur_525mA  = 37,
    LP4081H_ConstCur_550mA  = 38,
    LP4081H_ConstCur_575mA  = 39,
    LP4081H_ConstCur_600mA  = 40,
    LP4081H_ConstCur_625mA  = 41,
    LP4081H_ConstCur_650mA  = 42,
    LP4081H_ConstCur_675mA  = 43,
    LP4081H_ConstCur_700mA  = 44,
    LP4081H_ConstCur_725mA  = 45,
    LP4081H_ConstCur_750mA  = 46,
    LP4081H_ConstCur_775mA  = 47,
    LP4081H_ConstCur_800mA  = 48,
    LP4081H_ConstCur_825mA  = 49,
    LP4081H_ConstCur_850mA  = 50,
    LP4081H_ConstCur_875mA  = 51,
    LP4081H_ConstCur_900mA  = 52,
    LP4081H_ConstCur_925mA  = 53,
    LP4081H_ConstCur_950mA  = 54,
    LP4081H_ConstCur_975mA  = 55,
    LP4081H_ConstCur_1000mA = 56,
    LP4081H_ConstCur_1025mA = 57,
    LP4081H_ConstCur_1050mA = 58,
    LP4081H_ConstCur_1075mA = 59,
    LP4081H_ConstCur_1100mA = 60,
    LP4081H_ConstCur_1125mA = 61,
    LP4081H_ConstCur_1150mA = 62,
    LP4081H_ConstCur_1175mA = 63,

} ENUM_LP4081H_CONST_CHG_CUR_DEF;


typedef enum {
    LP4081H_BAT_CV_4_0V  = 0,  // 00000
    LP4081H_BAT_CV_4_05V = 1, // 00001
    LP4081H_BAT_CV_4_1V  = 2,  // 00010
    LP4081H_BAT_CV_4_15V = 3, // 00011
    LP4081H_BAT_CV_4_2V  = 4,  // 00100
    LP4081H_BAT_CV_4_25V = 5, // 00101
    LP4081H_BAT_CV_4_3V  = 6,  // 00110
    LP4081H_BAT_CV_4_35V = 7, // 00111
    LP4081H_BAT_CV_4_4V  = 8,  // 01000
    LP4081H_BAT_CV_4_45V = 9, // 01001
    LP4081H_BAT_CV_4_5V  = 10, // 01010
    // 继续补充其他电压值...
} ENUM_LP4081H_BAT_CV_DEF;


typedef struct
{
    bool        DoubleCurMode;
    uint8_t     ChgTriVolLimit;
    uint8_t     BatVsysDpm;
    uint8_t     ChgTrickle;
    uint8_t     BatFetMode;
    uint8_t     BatManagement;
    uint8_t     IntMaskSet;
    uint8_t     DischargeCurLimit;
    uint8_t     ChgCurLimit;
    uint8_t     ConstChgVol;
    uint8_t     ConstChgCur;
    
}LP4081H_Config_t;

extern void lp4081h_init(void);

extern uint8_t lp4081h_get_bat_status(void);

extern int lp4081h_get_bat_temperature(void);

extern void lp4081h_set_interrupt_mask(uint8_t interrupt_status);

extern void lp4081h_set_charging_current(uint8_t double_current, char *out_current, char *in_current, uint8_t bat_fet_mode);

extern void lp4081h_set_charging_voltage(char *voltage, uint8_t trickle);

extern void lp4081h_set_const_chg(uint8_t en_chg, uint8_t tri_voltage, char *const_chg);

extern void lp4081h_set_wtd_ntc_vsysdpm(uint8_t status, char *vsysdpm);

extern void lp4081h_chg_system_set(uint8_t de_chg, char *out_c, char *in_c, 
                                char *chg_v, char *chg_const, char *trickle);

extern void lp4081h_default_config(void);
extern void lp4081h_start_chg(void);

extern void lp4081h_stop_chg(void);

#ifdef __cplusplus
}
#endif

#endif


/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/

