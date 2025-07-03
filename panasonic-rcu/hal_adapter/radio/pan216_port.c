/*******************************************************************************
 * @note Copyright (C) 2020 Shanghai Panchip Microelectronics Co., Ltd. All rights reserved.
 *
 * @file pan216_port.c
 * @brief
 *
 * @history - V2.0, 2020-12-30
 *******************************************************************************/
#include "pan216_port.h"
#include "py32f0xx_hal_spi.h"
#include "py32f0xx_ll_gpio.h"
#include "pan216_ext.h"
#include "stdbool.h"
#include "string.h"
#include "time_utils.h"
#include "my_log.h"

/*============================================================================*
 *                              Local Variables
 *============================================================================*/

static SPI_HandleTypeDef hspi;
const PAN216_Fun_t Pan216_Funs = {

	.spi_writebyte = spi_4line_writebyte,
	.spi_write_Nbyte = spi_4line_write_Nbyte,
	.spi_readbyte = spi_4line_readbyte,
	.spi_read_Nbyte = spi_4line_read_Nbyte,
	.spi_cs_high = spi_cs_set_high,
	.spi_cs_low = spi_cs_set_low,

	.delayms = HAL_Delay,
	.delayus = delay_us,
};
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief 初始化spi
 * @param
 */
void SPI_Init(void)
{
	// spi配置
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
	HAL_GPIO_Init(SCK_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MOSI_PIN | MISO_PIN;
	HAL_GPIO_Init(MOSI_PORT, &GPIO_InitStruct);

	hspi.Instance = SPI1;
	hspi.Init.Mode = SPI_MODE_MASTER;
	hspi.Init.Direction = SPI_DIRECTION_2LINES;
	hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi.Init.CLKPolarity = SPI_POLARITY_LOW; // CPOL=0
	hspi.Init.CLKPhase = SPI_PHASE_1EDGE;	  // CPHA=0
	hspi.Init.NSS = SPI_NSS_SOFT;
	hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	HAL_SPI_Init(&hspi);
	HAL_Delay(2);
}

// GPIO初始化
void RF_GPIO_Init(void)
{
	// CSN/CE配置为输出
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = CSN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(CSN_PORT, &GPIO_InitStruct);

	// IRQ配置为输入
	GPIO_InitStruct.Pin = IRQ_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(IRQ_PORT, &GPIO_InitStruct);
	/* Enable EXTI interrupt */
	// HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	/* Configure interrupt priority */
	// HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);

	// 初始状态
	SPI_CS_HIGH;
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/**
 * @brief 初始化总线跟pan216模块
 * @param
 */
void RF_Bus_With_PAN216_Init(void)
{
#if __DEBUG__ >= __DEBUG_INFO__
	my_log(">I%d[%s] \n", __LINE__, __FUNCTION__);
#endif
	SPI_Init();
	RF_GPIO_Init();
	PAN216_Init();

	PAN216_ConfigIT(RF_IT_TX_IRQ);
	PAN216_ClearIRQFlags(0xFF);
	PAN216_PrepareTRxWithAck(PAN216_TX, false);
}

void RF_SendPacket(uint8_t *playload, uint8_t data_len)
{
	uint8_t flag = 0;
	uint8_t ret = PAN216_SendPacket(playload, data_len);
	while (!IRQ_DETECTED)
		;
	while (1)
	{
		flag = PAN216_GetIRQFlags();
		if (flag & RF_IT_TX_IRQ)
		{
			// my_log("RF_IT_TX_IRQ\r\n");
			PAN216_ClearIRQFlags(RF_IT_TX_IRQ);
			break;
		}
	}
}

uint8_t RF_ReadPacket(uint8_t *rx_data, uint8_t data_len)
{
	uint8_t flag = 0;
	uint8_t ret = 0;

	while (!IRQ_DETECTED)
		;
	flag = PAN216_GetIRQFlags();

	if (flag & RF_IT_RX_IRQ)
	{
		ret = PAN216_ReadFifo(TRX_FIFO, rx_data, data_len);
		PAN216_ClearIRQFlags(RF_IT_RX_IRQ);
		my_log("Rx:\r\n");
	}
	else if (flag == RF_IT_CRC_ERR_IRQ)
	{
		PAN216_ClearIRQFlags(flag);
		my_log(">> RF_IT_CRC_ERR_IRQ%d\r\n", flag);
	}

	return ret;
}

/**
 * @brief Set SPI CS high, use STM32 GPIOA4 as SPI CS pin
 * @param[in] none
 * @return none
 */
void spi_cs_set_high(void)
{
	SPI_CS_HIGH;
}

/**
 * @brief Set SPI CS low, use STM32 GPIOA4 as SPI CS pin
 * @param[in] none
 * @return none
 */
void spi_cs_set_low(void)
{
	SPI_CS_LOW;
}

void spi_4line_writebyte(uint8_t value)
{
	HAL_SPI_Transmit(&hspi, &value, 1, 100);
}

void spi_4line_write_Nbyte(uint8_t reg, uint8_t *values, uint8_t length)
{

	// 准备发送缓冲区，第一个字节是寄存器地址加上写标志位（0x20）
	uint8_t tx_buf[60];
	tx_buf[0] = reg;

	// 将要写入的数据复制到发送缓冲区
	for (uint8_t i = 0; i < length; i++)
	{
		tx_buf[i + 1] = values[i];
	}

	HAL_SPI_Transmit(&hspi, tx_buf, length + 1, 100);
}

uint8_t spi_4line_readbyte(void)
{
	uint8_t value;
	HAL_SPI_Receive(&hspi, &value, 1, 100);
	return value;
}

uint8_t spi_4line_read_Nbyte(uint8_t reg, uint8_t *data, uint8_t data_len)
{
	uint8_t value;
	HAL_SPI_Transmit(&hspi, &reg, 1, 50);
	HAL_SPI_Receive(&hspi, data, data_len, 100);

	return value;
}
