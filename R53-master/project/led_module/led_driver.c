/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     led_driver.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "led_driver.h"
#include "../../sdk/vendor/common/blt_led.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
app_device_led_ctrl_t gAppDeviceLedCtrl[APP_DEVICE_LED_NUM];

const led_cfg_t led_cfg[20] = {
        {APP_LED_BLUE,      1,   1000,     0,      1,      0,  },    //0:   power-on, 1s on
        {APP_LED_BLUE,      1,   100,      0,      0,      1,  },    //1:   audio on, long on
        {APP_LED_BLUE,      0,   0,        100,    0,      1,  },    //2:   audio off, long off
        {APP_LED_BLUE,      1,   500,      500,    2,      0,  },    //3:   1Hz for 3 seconds
        {APP_LED_BLUE,      1,   100,      100,    1,      0,  },    //4:   5Hz for 3 seconds
        {APP_LED_BLUE,      1,   1000,     0,      3,      0,  },    //5:   3s on
        {APP_LED_BLUE,      1,   250,      250,    120,    0,  },    //6:   2Hz
        {APP_LED_BLUE,      1,   500,      500,    0,      0,  },    //7:   1Hz
        {APP_LED_BLUE,      1,   100,      100,    5,      0,  },    //8:   5Hz for 5 seconds
        {APP_LED_BLUE,      1,   50,       50,     4,      0,  },    //9:
        {APP_LED_BLUE,      1,   500,      500,    150,    0,  },    //10:  1Hz for 150 seconds
        {APP_LED_BLUE,      1,   0,        1000,   3,      1,  },    //11:
        {APP_LED_BLUE,      1,   100,      100,    1,      0,  },    //12:  5Hz for 3 seconds
        {APP_LED_BLUE,      1,   500,      500,    3,      0,  },    //13:  1Hz for 3 seconds
        {APP_LED_BLUE,      1,   100,      0,      0xFF,   0,  },    //14:  led long on
        {APP_LED_BLUE,      0,   0,        100,    0xFF,   0,  },    //15:  led long off
        {APP_LED_BLUE,      1,   250,      250,    4,      0,  },    //16:  2Hz pairfali batlow

};

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void device_led_config(u8 led, u8 onLevel, u32 gpio);

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  led driver init
 * @param  none
 * @return none
 * @retval void
 */
void led_driver_init(void)
{
    device_led_config(APP_LED_BLUE,  0, GPIO_LED);
}

/******************************************************************
 * @brief  device_led_config
 * @param  none
 * @return none
 * @retval void
 */
static void device_led_config(u8 led, u8 onLevel, u32 gpio)
{
    if(led >= APP_DEVICE_LED_COUNT) return;
    
    gAppDeviceLedCtrl[led].isEnable = false;
    gAppDeviceLedCtrl[led].isOn = false;
    gAppDeviceLedCtrl[led].onLevel = onLevel;
    gAppDeviceLedCtrl[led].gpio = gpio;
    gpio_write(gAppDeviceLedCtrl[led].gpio, !gAppDeviceLedCtrl[led].onLevel);
    gpio_set_func(gpio,  AS_GPIO);
    gpio_set_input_en(gpio, 0);
    gpio_set_output_en(gpio, 1);
}

/******************************************************************
 * @brief  user_i2c_gpio_set init
 * @param  none
 * @return none
 * @retval void
 */
void user_i2c_gpio_set(I2C_GPIO_GroupTypeDef i2c_pin_group)
{
	GPIO_PinTypeDef sda, scl;
	if(i2c_pin_group == I2C_GPIO_GROUP_C2C3){
		sda = GPIO_PC2;
		scl = GPIO_PC3;
	}
	else{ //ERR
		sda = 0;
		scl = 0;
	}
	 //When the pad is configured with mux input and a pull-up resistor is required, gpio_input_en needs to be placed before gpio_function_dis,
	//otherwise first set gpio_input_disable and then call the mux function interface,the mux pad will may misread the short low-level timing.confirmed by minghai.20210709.
	gpio_set_input_en(sda, 1);//enable sda input
	gpio_set_input_en(scl, 1);//enable scl input
	gpio_setup_up_down_resistor(sda, PM_PIN_PULLUP_10K);
	gpio_setup_up_down_resistor(scl, PM_PIN_PULLUP_10K);
	gpio_set_func(sda, AS_I2C);
	gpio_set_func(scl, AS_I2C);
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
