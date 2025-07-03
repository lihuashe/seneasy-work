#ifndef __PAN216_PORT_H_
#define __PAN216_PORT_H_

#include <stdbool.h>
#include <stdint.h>
#include "types.h"
#include "py32f0xx_hal.h"
#include "my_log.h"


/* Do not modify these definitions */
#define USE_I2C              1
#define USE_SPI_3LINE        2
#define USE_SPI_4LINE        3

/* Do not modify these definitions */
#define INTERFACE_SPEED_200K 1
#define INTERFACE_SPEED_400K 2
#define INTERFACE_SPEED_700K 3
#define INTERFACE_SPEED_1M   4
#define INTERFACE_SPEED_2M   5

/* Do not modify these definitions */
#define XTAL_FREQ_16M        1
#define XTAL_FREQ_32M        2

/* Interface mode. Select one of the following */
#define EASY_RF              0
#define INTERFACE_MODE       USE_SPI_4LINE
#define INTERFACE_SPEED      INTERFACE_SPEED_2M
#define XTAL_FREQ            XTAL_FREQ_32M
#define SPI_CHECK_WRITE      0


#define CSN_PORT  GPIOA
#define CSN_PIN   GPIO_PIN_4

#define SCK_PORT  GPIOA
#define SCK_PIN   GPIO_PIN_5

#define MOSI_PORT GPIOA
#define MOSI_PIN  GPIO_PIN_7

#define MISO_PORT GPIOA
#define MISO_PIN  GPIO_PIN_6

#define IRQ_PORT  GPIOB
#define IRQ_PIN   GPIO_PIN_6

#define SPI_CS_HIGH       HAL_GPIO_WritePin(CSN_PORT, CSN_PIN, GPIO_PIN_SET)
#define SPI_CS_LOW        HAL_GPIO_WritePin(CSN_PORT, CSN_PIN, GPIO_PIN_RESET)

#define SPI_SCK_HIGH      HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET)
#define SPI_SCK_LOW       HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET)

#define SPI_DATA_HIGH     HAL_GPIO_WritePin(MOSI_PORT, MOSI_PIN, GPIO_PIN_SET)
#define SPI_DATA_LOW      HAL_GPIO_WritePin(MOSI_PORT, MOSI_PIN, GPIO_PIN_RESET)

#define SPI_DATA_STATUS   HAL_GPIO_ReadPin(MISO_PORT, MISO_PIN)

#define IRQ_DETECTED      !HAL_GPIO_ReadPin(IRQ_PORT, IRQ_PIN)

/* Porting functions end */

typedef enum
{
    SI2C_READY = 0,
    SI2C_BUS_BUSY = 1,
    SI2C_BUS_ERROR = 2,
    SI2C_NACK = 3,
    SI2C_ACK = 4,
} I2C_Bus_State_t;

typedef struct
{
    void (*spi_writebyte)(uint8_t value);
    void (*spi_write_Nbyte)(uint8_t reg, uint8_t *values, uint8_t length);
    uint8_t (*spi_readbyte)(void);
    uint8_t (*spi_read_Nbyte)(uint8_t reg, uint8_t *data, uint8_t data_len);
    void (*spi_cs_high)(void);
    void (*spi_cs_low)(void);
    void (*delayus)(const uint32_t time);
    void (*delayms)(const uint32_t time);
} PAN216_Fun_t;


extern const PAN216_Fun_t Pan216_Funs;

void RF_Bus_With_PAN216_Init(void);
void RF_SendPacket(uint8_t *playload, uint8_t data_len);
uint8_t RF_ReadPacket(uint8_t *rx_data, uint8_t data_len);

void spi_cs_set_high(void);
void spi_cs_set_low(void);
void spi_4line_writebyte(uint8_t value);
void spi_4line_write_Nbyte(uint8_t reg, uint8_t *values, uint8_t length);
uint8_t spi_4line_readbyte(void);
uint8_t spi_4line_read_Nbyte(uint8_t reg, uint8_t *data, uint8_t data_len);


#endif
