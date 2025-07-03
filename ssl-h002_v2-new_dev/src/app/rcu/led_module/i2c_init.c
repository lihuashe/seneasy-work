
#include "i2c_init.h"


void i2c_init(void)
{  
    // 配置 I2C SDA 引脚
	Pad_Config(I2C_SDA_PIN, 
               PAD_PINMUX_MODE,   // 使用引脚复用模式
               PAD_IS_PWRON,      // 启用引脚供电
               PAD_PULL_UP,       // 上拉配置
               PAD_OUT_ENABLE,    // 禁用输出
               PAD_OUT_HIGH);     // 设置输出高电平

	// 配置 I2C SCL 引脚
	Pad_Config(I2C_SCL_PIN, 
               PAD_PINMUX_MODE,   // 使用引脚复用模式
               PAD_IS_PWRON,      // 启用引脚供电
               PAD_PULL_UP,       // 上拉配置
               PAD_OUT_ENABLE,    // 禁用输出
               PAD_OUT_HIGH);     // 设置输出高电平

	//配置 I2C SDA 引脚
	Pinmux_Config(I2C_SDA_PIN, PINMUX_I2C_DAT); 

	// 配置 I2C SCL 引脚
	Pinmux_Config(I2C_SCL_PIN, PINMUX_I2C_CLK);

	//启用时钟
	RCC_PeriphClockCmd(APBP_I2C, APBP_I2C_CLOCK, ENABLE);

	//I2C结构体
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_StructInit(&I2C_InitStruct);
	I2C_InitStruct.I2C_ClockSpeed = 200000;
    I2C_InitStruct.I2C_SlaveAddress = DEVICE_ADDR;

	I2C_Init(DEV_I2Cx,&I2C_InitStruct);
	I2C_Cmd(DEV_I2Cx, ENABLE);
}




uint8_t i2c_read_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data)
{
	uint8_t status;

	if (os_mutex_take(i2c_mutex_handle, 5) == true)
	{
		if (NEED_SHARED_BUS)
		{
			I2C_SetSlaveAddress(DEV_I2Cx, slave_addr);
		}
		status = I2C_RepeatRead(DEV_I2Cx, &reg_addr, 1, data, 1);

		os_mutex_give(i2c_mutex_handle);
	}
	
	return status;
}


uint8_t i2c_write_byte(uint8_t slave_addr, uint8_t *data, uint8_t data_len)
{
	uint8_t status;

	if (os_mutex_take(i2c_mutex_handle, 5) == true)//200khz ,1ms写入85字节
	{
		if (NEED_SHARED_BUS)
		{
			I2C_SetSlaveAddress(DEV_I2Cx, slave_addr);
		}
		status = I2C_MasterWrite(DEV_I2Cx, data, data_len);

		os_mutex_give(i2c_mutex_handle);
	}

	return status;
}
