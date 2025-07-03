/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     led_driver.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef LED_DRIVER_H_
#define LED_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define APP_LED_BLUE          0
#define APP_LED_WHITE         1
#define APP_DEVICE_LED_NUM    2

enum{
    LED_POWER_ON = 0,
    LED_AUDIO_ON,           //1
    LED_AUDIO_OFF,          //2
    LED_SHINE_SLOW,         //3
    LED_SHINE_FAST,         //4
    LED_SHINE_SUCCESS,      //5
    LED_SHINE_PAIRING,      //6
    LED_SHINE_PTT,          //7
    LED_SHINE_LOWBAT,       //8
    LED_PAIR_ERROR,         //9
    LED_OTA_START,          //10
    LED_OTA_END,            //11
    LED_KEYPRESS,           //12
    LED_SHINE_ERASE_PAIR,   //13
    LED_LONG_ON,            //14
    LED_LONG_OFF,           //15
    LED_PAIR_LOWBAT         //16
};
extern const led_cfg_t led_cfg[20];

/**
 *  @brief  select pin as SDA and SCL of i2c
 */
typedef enum {
	I2C_GPIO_GROUP_A3A4,
	I2C_GPIO_GROUP_B6D7,
	I2C_GPIO_GROUP_C0C1,
	I2C_GPIO_GROUP_C2C3,
}I2C_GPIO_GroupTypeDef;

/*============================================================================*
 *                          Functions
 *============================================================================*/
extern void led_driver_init(void);
extern void user_i2c_gpio_set(I2C_GPIO_GroupTypeDef i2c_pin_group);

#endif /* LED_DRIVER_H_ */
