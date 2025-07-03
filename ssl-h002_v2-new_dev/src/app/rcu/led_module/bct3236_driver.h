/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bct3236_driver.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-18
* @version  v1.0
*********************************************************************************************************
*/

#ifndef BCT3236_DRIVER_H_
#define BCT3236_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <swtimer.h>
#include <stdio.h>
#include <rtl876x.h>
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "i2c_init.h"
#include "common_macros.h"


/*============================================================================*
 *                        宏定义
 *============================================================================*/
#define LED_BCT3236_ENABLE          1

#define BCT3236_ADDR			            0x3C  /* AD——>GND */
#define NEED_SEND_BCT3236_ADDR              0

#define LED_EN                              P0_1
#define LED_EN_OPEN()                       GPIO_SetBits(GPIO_GetPin(LED_EN))
#define LED_EN_OFF()                        GPIO_ResetBits(GPIO_GetPin(LED_EN))

#define Bct3236_Software_Mode_Register      0x00
#define Bct3236_Brightness_Update_Register  0x25
#define Bct3236_Led_Control_Register        0x26
#define Bct3236_Global_Control_Register     0x4A
#define Bct3236_Reset_Register              0x4F

/*============================================================================*
 *                        Struct Define
 *============================================================================*/

typedef struct
{
    uint8_t     led_index;
    uint8_t     light;
    uint8_t     iout;
    
}Bct3236_Led_Config_t;


/*============================================================================*
 *                          Functions
 *============================================================================*/
extern void bct3236_init(void);
extern void bct3236_set_pinx(char *pin_x, uint8_t brightness, uint8_t led_ctrl)  DATA_RAM_FUNCTION;
extern void bct3236_set_led_group(uint8_t *pin_arr, uint8_t pin_arr_len, uint8_t brightness, uint8_t led_ctrl)  DATA_RAM_FUNCTION;
extern void bct3236_led_all_on(void);
extern void bct3236_led_all_off(void);
void bct3236_write_reg_and_data(uint8_t reg_addr, uint8_t data);

#endif /* BCT3236_DRIVER_H_ */
