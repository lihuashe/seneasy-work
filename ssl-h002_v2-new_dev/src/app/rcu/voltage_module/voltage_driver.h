#ifndef _VOLTAGE_DRIVER_H_
#define _VOLTAGE_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <app_section.h>
#include "swtimer.h"
#include <rtl876x_gpio.h>
#include "app_function.h"

/*============================================================================*
*                               定义区（本系列）
*============================================================================*/
#define IO_MSG_SUBTYPE_VOLTAGE      0xFE

#define VOLTAGE_PWM_TIMER_NUM       TIM3
#define VOLTAGE_INT_Handler         Timer3_Handler
#define VOLTAGE_TIMER_IRQ           Timer3_IRQn
#define LV_PWM_TIMER_NUM            TIM2

#define HV_HIV_PWM_P                DWGPIO
#define HV_LIV_PWM_N                DWGPIO
#define LV_HIV_PWM_P                PWM2_P
#define LV_LIV_PWM_N                PWM2_N

#define HV_HIV_Pin                  P2_6
#define HV_LIV_Pin                  P2_7
#define LV_HIV_Pin                  P2_4
#define LV_LIV_Pin                  P2_5

#define  HV_POWER_Pin               P5_1
#define LV_POWER_Pin                P5_2

#define DC24V_OR_DC14V_POWER_Pin    H_0

#define OC1_Pin                     P4_1
#define OC2_Pin                     P4_0
#define OC3_Pin                     P0_6
#define OC4_Pin                     P0_5

#define HV_POWER_OPEN()       GPIO_SetBits(GPIO_GetPin(HV_POWER_Pin))
#define HV_POWER_OFF()        GPIO_ResetBits(GPIO_GetPin(HV_POWER_Pin))

#define LV_POWER_OPEN()       GPIO_SetBits(GPIO_GetPin(LV_POWER_Pin))
#define LV_POWER_OFF()        GPIO_ResetBits(GPIO_GetPin(LV_POWER_Pin))

#define OC1_Pin_OPEN()        GPIO_SetBits(GPIO_GetPin(OC1_Pin))
#define OC1_Pin_OFF()         GPIO_ResetBits(GPIO_GetPin(OC1_Pin))

#define OC2_Pin_OPEN()        GPIO_SetBits(GPIO_GetPin(OC2_Pin))
#define OC2_Pin_OFF()         GPIO_ResetBits(GPIO_GetPin(OC2_Pin))

#define OC3_Pin_OPEN()        GPIO_SetBits(GPIO_GetPin(OC3_Pin))
#define OC3_Pin_OFF()         GPIO_ResetBits(GPIO_GetPin(OC3_Pin))

#define OC4_Pin_OPEN()        GPIO_SetBits(GPIO_GetPin(OC4_Pin))
#define OC4_Pin_OFF()         GPIO_ResetBits(GPIO_GetPin(OC4_Pin))

/*============================================================================*
*                        local must define
*============================================================================*/

#define HV_LEVEL_KEEP_TIME          12        //实测硬件自身开关时间很快，只需1us，实际时间1+set_time
#define LEVEL_DEAD_TIME             (0+6)   //至少要保持比较长时间的死区时间
#define CHANGE_DELAY_US             1000      //脉冲间隔时间
#define VOLTAGE_TIMER_MS_COUNT      1000 * 4 * 10 //这是设置ms周期倍率，40mhz，1us计40次
#define VOLTAGE_DELAY_US(delay_us)  platform_delay_us(delay_us)

#define SET_VOLTAGE_Pin(pinx, state)  ((state) ? GPIO_SetBits(GPIO_GetPin(pinx)) : GPIO_ResetBits(GPIO_GetPin(pinx)))


typedef struct
{
    uint8_t oc1;//on or off
    uint8_t oc2;
    uint8_t oc3;
    uint8_t oc4;
    uint8_t beat_ratio;//写0为不执行，节拍比例

}Oc_Open_Off_Define; 


typedef struct
{
    uint8_t mode;//模式
    uint8_t gear;//档位
    Oc_Open_Off_Define oc_ctrl[2];//保存OC1-4

}Oc_Open_Off_Cycles; 

typedef enum
{
    HV_MODE    = 0x01,
    LV_MODE,
    HV_AND_LV_MODE

}ENUM_VOLTAGE_HV_LV_MODE_DEF;

typedef enum
{
    HV_TEST_MODE = 0x10,
    LV_24V_TEST_MODE = 0x20,
    LV_14V_TEST_MODE = 0x30,

}ENUM_TEST_LV_MODE_DEF;

typedef struct
{
    uint32_t cycles_time;
    uint8_t  cycles_count;
}Cycles_Time_Count; 

typedef struct
{
    uint8_t                     mode;//模式
    uint8_t                     gear;//档位
    uint8_t                     hv_lv_mode;//高低压模式
    uint8_t                     hv_pulse_count;//单个周期内脉冲数
    Cycles_Time_Count           hv_loop_cycles[2];//高压周期
    uint32_t                    lv_high_level;//高电平占空比%
    Cycles_Time_Count           lv_loop_cycles[2];//保存了中压和低压时间

}Voltage_Map; 

static const Voltage_Map voltage_map[] = {
    //模式     档位 高低压模式 脉冲数  高压周期            占空比%  低压周期  
    {SCALP_MODE, 1, HV_MODE,   1,  {{160,  1},{0,   0}}, 0, {{0,  0},{0,  0}}},
    {SCALP_MODE, 2, HV_MODE,   3,  {{160,  1},{0,   0}}, 0, {{0,  0},{0,  0}}},
    {SCALP_MODE, 3, HV_MODE,   5,  {{130,  1},{0,   0}}, 0, {{0,  0},{0,  0}}},
    {SCALP_MODE, 4, HV_MODE,   8,  {{105,  1},{0,   0}}, 0, {{0,  0},{0,  0}}},
    {SCALP_MODE, 5, HV_MODE,   10, {{105,  1},{0,   0}}, 0, {{0,  0},{0,  0}}},

    {FACE_MODE, 1, HV_MODE,    1,   {{800,  1},{0,   0}}, 0, {{0,  0},{0,  0}}},
    {FACE_MODE, 2, HV_MODE,    3,   {{800,  1},{0,   0}}, 0, {{0,  0},{0,  0}}},
    {FACE_MODE, 3, HV_MODE,    5,   {{440,  1},{770, 1}}, 0, {{0,  0},{0,  0}}},
    {FACE_MODE, 4, HV_MODE,    8,   {{240,  1},{350, 1}}, 0, {{0,  0},{0,  0}}},
    {FACE_MODE, 5, HV_MODE,    10,  {{70,   1},{350, 1}}, 0, {{0,  0},{0,  0}}},
    
    {FACE_MESO_MODE, 1, HV_AND_LV_MODE, 2,  {{800,  4},{0,   0}}, 50, {{1800,  1},{20, 60}}},
    {FACE_MESO_MODE, 2, HV_AND_LV_MODE, 2,  {{520,  6},{0,   0}}, 50, {{1800,  1},{20, 60}}},
    {FACE_MESO_MODE, 3, HV_AND_LV_MODE, 2,  {{320, 10},{0,   0}}, 50, {{1800,  1},{20, 60}}},
    {FACE_MESO_MODE, 4, HV_AND_LV_MODE, 2,  {{230, 14},{0,   0}}, 50, {{1800,  1},{20, 60}}},
    {FACE_MESO_MODE, 5, HV_AND_LV_MODE, 2,  {{160, 18},{0,   0}}, 50, {{1800,  1},{20, 60}}},
    
    {SCALP_MESO_MODE, 1, HV_AND_LV_MODE, 4,   {{175, 36},{0,   0}}, 50, {{180,  1},{5, 60}}},
    {SCALP_MESO_MODE, 2, HV_AND_LV_MODE, 5,   {{175, 36},{0,   0}}, 50, {{180,  1},{5, 60}}},
    {SCALP_MESO_MODE, 3, HV_AND_LV_MODE, 6,   {{175, 36},{0,   0}}, 50, {{180,  1},{5, 60}}},
    {SCALP_MESO_MODE, 4, HV_AND_LV_MODE, 8,   {{175, 36},{0,   0}}, 50, {{180,  1},{5, 60}}},
    {SCALP_MESO_MODE, 5, HV_AND_LV_MODE, 10,  {{175, 36},{0,   0}}, 50, {{180,  1},{5, 60}}},

    {HV_TEST_MODE,     5, HV_MODE, 10,  {{70,1},{0,   0}},    50, {{0,  0},{0,  0}}},//用于生产测试1800v
    {LV_24V_TEST_MODE, 5, LV_MODE, 0,   {{0, 0},{0,   0}},    50, {{10 * 1000,  1},{0, 0}}},//用于生产测试24v
    {LV_14V_TEST_MODE, 5, LV_MODE, 0,   {{0, 0},{0,   0}},    50, {{0,  0},{10 * 1000, 1}}},//用于生产测试14v
};

static const Oc_Open_Off_Cycles oc1_4_map[] = {
    //模式     档位   oc1-4    beat_ratio
    {SCALP_MESO_MODE, 1, {{1,0,0,1, 1}, {0,1,1,0, 1}}},
    {SCALP_MESO_MODE, 2, {{0,1,0,1, 1}, {1,0,1,0, 1}}},
    {SCALP_MESO_MODE, 3, {{1,1,0,0, 1}, {0,0,1,1, 1}}},
    {SCALP_MESO_MODE, 4, {{1,1,0,1, 1}, {1,0,1,1, 1}}},
    {SCALP_MESO_MODE, 5, {{1,1,1,0, 1}, {0,1,1,1, 1}}},

};

static Voltage_Map now_voltage;

/*============================================================================*
 *                          Functions
 *============================================================================*/

void voltage_gpio_init(void);

void voltage_init(void);
void open_voltage_1800v(void);
void open_voltage_24v(void);
void hv_io_off(void);
void lv_pwm_off(void);
void out_voltage_off(void);
void oc1_oc4_open(void);
void oc1_oc4_off(void);
void VOLTAGE_INT_Handler(void) DATA_RAM_FUNCTION;
void voltage_hv_pulse_do(bool *flag, uint8_t hv_pulse_count, uint32_t hv_pulse_space) DATA_RAM_FUNCTION;
void voltage_mode_gear_change(uint8_t mode, uint8_t gear);
bool timer_change_set_cycles(void *loop_cycles, uint8_t cycles_len, uint8_t *arr_on, uint8_t *loop_count, uint8_t hv_pulse_count) DATA_RAM_FUNCTION;
void voltage_mode_gear_do(void) DATA_RAM_FUNCTION;
#endif  

/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/
