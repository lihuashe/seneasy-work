/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     lp4981b_driver.h
* @brief    
* @details
* @author   chenrongrong
* @date     2024-05-31
* @version  v1.0
*********************************************************************************************************
*/

#ifndef LP4081B_DRIVER_H_
#define LP4081B_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../tl_common.h"

#if(CHARGE_ENABLE)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
/* LP4081B address */
#define LP40801B_DEVICE_ADDR	(0x6B << 1)

#define LP4081_REG_NUM      7

/************************************************************
 *
 * Register List
 *
 ***********************************************************/
#define LP4081_REG_STUTAS0          0x00
#define LP4081_REG_STUTAS1          0x01
#define LP4081_REG_MSK              0x02
#define LP4081_REG_CTRL0            0x03
#define LP4081_REG_CTRL1            0x04
#define LP4081_REG_CTRL2            0x05
#define LP4081_REG_CTRL3            0x06

typedef union {
    u8 byte;
    struct {
        u8 pgd     : 1;   /* =1 Vuvlo_bus < Vbus < Vovp_bus */
        u8 chg     : 2;   /* 00: Not in charging mode/ 01: Trickle/ 10: CC|CV/ 11: EOC */
        u8 scp     : 1;   /* =1 SYS is short circuit to ground */
        u8 bat_uv  : 1;   /* =1 Battery discharge UVLO */
        u8 wtd_te  : 1;   /* =1 Watchdog timer expiration */
        u8 term_te : 1;   /* =1 Fast charge timer expiration */
        u8 tri_te  : 1;   /* =1 Trickle charge timer expiration */
    } fields;
} LP4081_STATUS_TypeDef;

typedef union {
    u8 byte;
    struct {
        u8 reserved0  : 1;
        u8 mtimer     : 1;
        u8 mpgd       : 1;
        u8 mchg       : 1;
        u8 mscp       : 1;
        u8 meoc       : 1; 
        u8 mbat_uv    : 1; 
    } fields;
} LP4081_MSK_TypeDef;

typedef union {
    u8 byte;
    struct {
        u8 dis        : 2;   /* BAT_FET operation mode control 00:BAT_FET bypass mode/ 01:system reset mode/ 10:auto mode/ 11:ship mode */
        u8 iin_lim    : 1;   /* input current limit 1:1.6A/ 0:700mA */
        u8 idsi_lim   : 2;   /* discharge current limit 00:400mA/ 01:300mA/ 10:200mA/ 11:100mA */    
        u8 icc_ext    : 1;
    } fields;
} LP4081_CTRL_0_TypeDef;

typedef union {
    u8 byte;
    struct {
        u8 bat_cv     : 5;
        u8 iterm      : 3;
    } fields;
} LP4081_CTRL_1_TypeDef;

typedef union {
    u8 byte;
    struct {
        u8 icc        : 6;
        u8 vtri       : 1;   /* Trickle charge threshold set 1:3.0V/ 0:2.8V */
        u8 en_chg     : 1;   /* 1:Charge enable/ 0:Charge disable */
    } fields;
} LP4081_CTRL_2_TypeDef;

typedef union {
    u8 byte;
    struct {
        u8 dpm              : 3;
        u8 en_term          : 1;
        u8 en_tri_timer     : 1;
        u8 reserved0        : 1;
        u8 en_wdt           : 1;
        u8 ship             : 1;
    } fields;
} LP4081_CTRL_3_TypeDef;

typedef struct {
        LP4081_STATUS_TypeDef   status;     /* 00H read only */
        u8                      reserved;   /* 01H read only, 001:<0°/ 010:0-10°/ 011:10-45°/ 100:>45° */
        LP4081_MSK_TypeDef      intr_msk;   /* 02H read and write */     
        LP4081_CTRL_0_TypeDef   ctrl_0;     /* 03H read and write */  
        LP4081_CTRL_1_TypeDef   ctrl_1;     /* 04H read and write */  
        LP4081_CTRL_2_TypeDef   ctrl_2;     /* 05H read and write */  
        LP4081_CTRL_3_TypeDef   ctrl_3;     /* 06H read and write */  
} LP4081_REG_Typedef;

extern bool app_rcu_charge_flag;
extern bool app_rcu_fully_charged;
extern u8 user_led_blink_flag;
extern u8 user_led_on_flag;
extern _attribute_data_retention_ u32 app_rcu_charge_check_timer;
extern _attribute_data_retention_ u32 app_rcu_charge_out_timer;
extern _attribute_data_retention_ u8 app_rcu_charge_out_flag;
/*============================================================================*
 *                          Functions
 *============================================================================*/
/******************************************************************
 * @brief  lp4081_init
 * @param  none
 * @return none
 * @retval void
 */
void lp4081_init(void);

/******************************************************************
 * @brief  enter_ship_mode
 * @param  none
 * @return none
 * @retval void
 */
void enter_ship_mode(void);

/******************************************************************
 * @brief  read_temperature
 * @param  none
 * @return  0: < 0°     get invalid value return -1
 *          1：0-10°
 *          2: 10-45°
 *          3：> 45°
 * @retval int
 */
int read_temperature(void);

/******************************************************************
 * @brief  state_of_charge
 * @param  none
 * @return 0:Not in charging mode, 1:Trickle, 2:CC/CV, 3:EOC
 * @retval int
 */
u8 state_of_charge(void);

void irqb_gpio_init(void);

void irqb_scan_func(void);
void app_check_charge_batt(void);
u16 get_battery_voltage(void);
void adc_vbat_detect_init(bool chose);

#if(VOLTAGE_STOP_CH_EN)
#define CHARGE_STOP_MIN_TMP         1250//1180
#define CHARGE_STOP_MAX_TMP         2000//2210

#define CHARGE_STOP_INIT            \
{                                   \
    .dat_filter = 0,                \
    .batt_nic_mv = 0,               \
    .tick = 0,                      \
    .tick_flag = false              \
}

typedef struct
{
    u8   dat_filter;
    u16  batt_nic_mv;
    u32  tick;
    bool tick_flag;
}charge_stop_t;

void adc_time_multplex_read(void);
void voltage_start_charge_handle(void);
#endif
#endif
#endif /* LED_DRIVER_H_ */
