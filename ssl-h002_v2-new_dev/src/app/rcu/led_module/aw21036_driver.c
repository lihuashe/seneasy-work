/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     aw21036_driver.c
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
#include "aw21036_driver.h"
#include "rtl876x_i2c.h"
#include "rtl876x.h"
#include "system_rtl876x.h"
#include "rtl876x_pinmux.h"
#include "app_msg.h"
#include "app_task.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
u8 LED_BR0_Data  = 255;
u8 LED_COL0_Data = 255;
u8 Quick_WaitMs_Flag = 0;
u8 BackLight_Connect_Init = 0;
bool BR_TEST_CLOSE_FLAG = false;
bool BR_TEST_LED_DUTY_FLAG = false;

/*============================================================================*
 *                              Extern Functions
 *============================================================================*/

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
void aw21036_i2c_init(void)
{
	
}



#if (LED_AW21036_ENABLE)


static void aw21036_set_global_current(u8 current)
{
	
}
#endif
void aw21036_set_brightness(u8 index, u8 brightness)
{
	
}
void aw21036_set_color(u8 index, u8 color)
{
	
}
void aw21036_update_brightness_to_display(void)
{
	
}

void aw210xx_chip_hwen(void)
{
	
	Pad_Config(AW21036_CHIP_LEN_EN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	GPIO_SetBits(AW21036_CHIP_LEN_EN);
	/* delay 2ms at least */
	os_delay(2);
}

static void aw210xx_soft_rst(void)
{
	
	PRINT("aw210xx_soft_rst\n");
	/* delay 2ms at least */
	os_delay(2);
};

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/******************************************************************
 * @brief  aw21036 init
 * @param  none
 * @return none
 * @retval void
 */
void aw21036_init(void)
{
#if (LED_AW21036_ENABLE)
	
	i2c_init();//I2C初始化
	aw210xx_chip_hwen();//硬件复位
	aw210xx_soft_rst();//软件复位

    /* 1. enable chip and enable auto power-saving 使能芯片--active模式 */
    //0x01:63kHz  0x11:32kHz  0x21:4kHz  0x31:2kHz  0x41:1kHz  0x51:500Hz  0X61:244Hz  0x71:122Hz
    //i2c_write_byte(REG_GCR, 1, 0x01);
    /* 2. check id */
    //unsigned char chip_id = i2c_read_byte(REG_RESET, 1);
    //printf("--->>> AW21036_Sensor Chip_id  == 0x%02x\r\n",chip_id);
	/* 3. config global current 设置全局电流 */
    aw21036_set_global_current(0xff);

	Quick_WaitMs_Flag = 0;

#endif
}

/******************************************************************
 * @brief  aw21036_Disable
 * @param  none
 * @return none
 * @retval void
 */
void aw21036_Disable(void)
{
	//aw21036_all_off();
    //i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    //i2c_write_byte(REG_GCR, 1, 0x00);
    //  gpio_write(GPIO_PD7,1);
    //gpio_write(GPIO_PC4,0);

}



void aw21036_init_TEST(void)
{

	PRINT("--->>> aw21036_init_TEST\n"); 
	//PRINT("--->>> aw210xx_chip_hwen\n");
	i2c_init();//I2C初始化
	aw210xx_soft_rst();//软件复位


	/* 1. enable chip and enable auto power-saving 使能芯片--active模式 */
	aw210xx_chip_hwen();//硬件复位
    //0x01:63kHz  0x11:32kHz  0x21:4kHz  0x31:2kHz  0x41:1kHz  0x51:500Hz  0X61:244Hz  0x71:122Hz
    //i2c_write_byte(REG_GCR, 1, 0x01);
    /* 2. check id */
    //unsigned char chip_id = i2c_read_byte(REG_RESET, 1);
   // PRINT("--->>> AW21036_Sensor Chip_id  == 0x%02x\n",chip_id);
	/* 3. config global current 设置全局电流 */
    aw21036_set_global_current(0xff);
	//if(chip_id != 0x18)

}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
