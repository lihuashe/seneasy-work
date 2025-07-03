#ifndef __PAN216_PORT_H_
#define __PAN216_PORT_H_

#include <stdbool.h>
#include <stdint.h>
#include "PanSeries.h"
#include "pan_gpio.h"
#include "base_types.h"
#include "bsp.h"


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
#define INTERFACE_SPEED      INTERFACE_SPEED_1M
#define XTAL_FREQ            XTAL_FREQ_32M
#define SPI_CHECK_WRITE      1

/* Porting functions. Implement these porting functions based on the MCU platform */
#define ITF_DELAY_200K       BSP_DELAY_200K
#define ITF_DELAY_400K       BSP_DELAY_400K
#define ITF_DELAY_700K       BSP_DELAY_700K
#define ITF_DELAY_1M         BSP_DELAY_1M
#define ITF_DELAY_2M         BSP_DELAY_2M

#define ITF_DELAY_MS         BSP_DelayMs
#define ITF_DELAY_US         BSP_DelayUs

#define ITF_4SPIINIT         BSP_4LineSPIInit
#define ITF_3SPIINIT         BSP_3LineSPIInit
#define ITF_IICINIT          BSP_IICInit

#if INTERFACE_MODE == USE_SPI_3LINE
#define SPI_CS_HIGH     CS_PIN = 1
#define SPI_CS_LOW      CS_PIN = 0
#define SPI_DATA_OUTPUT GPIO_SetModeByPin(P4_3, GPIO_MODE_OUTPUT)
#define SPI_DATA_INPUT  GPIO_SetModeByPin(P4_3, GPIO_MODE_INPUT)
#define SPI_SCK_LOW     SCK_PIN = 0
#define SPI_SCK_HIGH    SCK_PIN = 1
#define SPI_DATA_HIGH   SDA_PIN = 1
#define SPI_DATA_LOW    SDA_PIN = 0
#define SPI_DATA_STATUS SDA_PIN
#define IRQ_DETECTED    (!SPI_DATA_STATUS)
#elif INTERFACE_MODE == USE_I2C
#define I2C_SDA_OUTPUT GPIO_SetModeByPin(P4_3, GPIO_MODE_OUTPUT)
#define I2C_SDA_INPUT  GPIO_SetModeByPin(P4_3, GPIO_MODE_INPUT)
#define I2C_SCK_LOW    SCK_PIN = 0
#define I2C_SCK_HIGH   SCK_PIN = 1
#define I2C_SDA_LOW    SDA_PIN = 0
#define I2C_SDA_HIGH   SDA_PIN = 1
#define I2C_SDA_STATUS SDA_PIN
#define IRQ_DETECTED   (!I2C_SDA_STATUS)
#endif
#if INTERFACE_MODE == USE_SPI_4LINE
#define SPI_CS_HIGH     CS_PIN = 1
#define SPI_CS_LOW      CS_PIN = 0
#define SPI_SCK_LOW     SCK_PIN = 0
#define SPI_SCK_HIGH    SCK_PIN = 1
#define SPI_DATA_LOW    MOSI_PIN = 0
#define SPI_DATA_HIGH   MOSI_PIN = 1
#define SPI_DATA_STATUS MISO_PIN
#define IRQ_DETECTED    (!IRQ_PIN)
#endif
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
#if INTERFACE_MODE == USE_I2C
    I2C_Bus_State_t (*i2c_write_buf)(uint8_t addr, uint8_t *buf, uint16_t length);
    I2C_Bus_State_t (*i2c_read_buf)(uint8_t addr, uint8_t *buf, uint16_t length);
    void (*i2c_reset)(void);
#else
    void (*spi_writebyte)(uint8_t value);
    uint8_t (*spi_readbyte)(void);
    void (*spi_cs_high)(void);
    void (*spi_cs_low)(void);
#endif

    void (*delayus)(const uint32_t time);
    void (*delayms)(const uint32_t time);
} PAN216_Fun_t;

extern PAN216_Fun_t Pan216_Funs;

void RF_BusInit(void);

#if INTERFACE_MODE == USE_SPI_3LINE
void spi_cs_set_high(void);
void spi_cs_set_low(void);
void spi_3line_writebyte(uint8_t value);
uint8_t spi_3line_readbyte(void);
#elif INTERFACE_MODE == USE_I2C
I2C_Bus_State_t sw_i2c_start(void);
void sw_i2c_stop(void);
void sw_i2c_send_ack(void);
void sw_i2c_send_nack(void);
I2C_Bus_State_t sw_i2c_write_byte(uint8_t data);
uint8_t sw_i2c_read_byte(void);
I2C_Bus_State_t sw_i2c_write_buf(uint8_t addr, uint8_t *buf, uint16_t length);
I2C_Bus_State_t sw_i2c_read_buf(uint8_t addr, uint8_t *buf, uint16_t length);
void sw_i2c_reset(void);
#endif
#if INTERFACE_MODE == USE_SPI_4LINE
void spi_cs_set_high(void);
void spi_cs_set_low(void);
void spi_4line_writebyte(uint8_t value);
uint8_t spi_4line_readbyte(void);
#endif

#endif
