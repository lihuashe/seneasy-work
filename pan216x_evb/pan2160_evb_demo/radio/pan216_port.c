/*******************************************************************************
 * @note Copyright (C) 2020 Shanghai Panchip Microelectronics Co., Ltd. All rights reserved.
 *
 * @file pan216_port.c
 * @brief
 *
 * @history - V2.0, 2020-12-30
 *******************************************************************************/

#pragma O0 // disable optimization, otherwise the spi/iic time sequence will be inaccurate

#include "PanSeries.h"
#include "pan_gpio.h"
#include "pan216_port.h"

#if (INTERFACE_SPEED == INTERFACE_SPEED_2M)
#define DELAY() ITF_DELAY_2M()
#elif (INTERFACE_SPEED == INTERFACE_SPEED_1M)
#define DELAY() ITF_DELAY_1M()
#elif (INTERFACE_SPEED == INTERFACE_SPEED_700K)
#define DELAY() ITF_DELAY_700K()
#elif (INTERFACE_SPEED == INTERFACE_SPEED_400K)
#define DELAY() ITF_DELAY_400K()
#elif (INTERFACE_SPEED == INTERFACE_SPEED_200K)
#define DELAY() ITF_DELAY_200K()
#endif

PAN216_Fun_t Pan216_Funs = {
#if INTERFACE_MODE == USE_SPI_3LINE
    .spi_writebyte = spi_3line_writebyte,
    .spi_readbyte = spi_3line_readbyte,
    .spi_cs_high = spi_cs_set_high,
    .spi_cs_low = spi_cs_set_low,
#elif INTERFACE_MODE == USE_I2C
    .i2c_write_buf = sw_i2c_write_buf,
    .i2c_read_buf = sw_i2c_read_buf,
    .i2c_reset = sw_i2c_reset,
#endif
#if INTERFACE_MODE == USE_SPI_4LINE
    .spi_writebyte = spi_4line_writebyte,
    .spi_readbyte = spi_4line_readbyte,
    .spi_cs_high = spi_cs_set_high,
    .spi_cs_low = spi_cs_set_low,
#endif
    .delayms = ITF_DELAY_MS,
    .delayus = ITF_DELAY_US,
};

void RF_BusInit(void)
{
#if INTERFACE_MODE == USE_SPI_3LINE
    ITF_3SPIINIT();
#elif INTERFACE_MODE == USE_I2C
    ITF_IICINIT();
#endif
#if INTERFACE_MODE == USE_SPI_4LINE
    ITF_4SPIINIT();
#endif
}

#if INTERFACE_MODE != USE_I2C
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
#endif

#if INTERFACE_MODE == USE_SPI_3LINE
void spi_3line_writebyte(uint8_t value)
{
    unsigned char i;
    
    SPI_DATA_OUTPUT;
    
    for (i = 0; i < 8; i++)
    {
        SPI_SCK_LOW;
        DELAY();
        if (value & 0x80)
        {
            SPI_DATA_HIGH;
        }
        else
        {
            SPI_DATA_LOW;
        }

        value <<= 1;
        DELAY();
        SPI_SCK_HIGH;
        DELAY();
    }
 
    SPI_DATA_INPUT;
    SPI_SCK_LOW;
}

uint8_t spi_3line_readbyte(void)
{
    u8 i;
    u8 value = 0;

    for (i = 0; i < 8; i++)
    {
        SPI_SCK_LOW;
        value <<= 1;
        DELAY();
        SPI_SCK_HIGH;
        DELAY();
        if (SPI_DATA_STATUS)
        {
            value |= 0x01;
        }
    }
    SPI_SCK_LOW;

    return value;
}
#endif

#if INTERFACE_MODE == USE_SPI_4LINE
void spi_4line_writebyte(uint8_t value)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        SPI_SCK_LOW;
        DELAY();
        if (value & 0x80)
        {
            SPI_DATA_HIGH;
        }
        else
        {
            SPI_DATA_LOW;
        }

        value <<= 1;
        DELAY();
        SPI_SCK_HIGH;
        DELAY();
    }

    SPI_SCK_LOW;
}

uint8_t spi_4line_readbyte(void)
{
    u8 i;
    u8 value = 0;

    for (i = 0; i < 8; i++)
    {
        SPI_SCK_LOW;
        value <<= 1;
        DELAY();
        SPI_SCK_HIGH;
        DELAY();
        if (SPI_DATA_STATUS)
        {
            value |= 0x01;
        }
    }
    SPI_SCK_LOW;

    return value;
}
#endif

#if INTERFACE_MODE == USE_I2C

#define ACK_TIMEOUT_DEF     10000
#define DEVICE_ADDR         0x71

/**
 * @brief I2C start
 * @param[in] none
 * @return I2C bus state
 */
I2C_Bus_State_t sw_i2c_start(void)
{
    DELAY();
    I2C_SDA_OUTPUT;
    I2C_SDA_HIGH;
    I2C_SCK_HIGH;
    DELAY();
    I2C_SDA_LOW;
    DELAY();
    I2C_SCK_LOW;
    DELAY();

    return SI2C_READY;
}

/**
 * @brief I2C stop
 * @param[in] none
 * @return none
 */
void sw_i2c_stop(void)
{
    DELAY();
    I2C_SDA_OUTPUT;
    I2C_SDA_LOW;
    DELAY();
    I2C_SCK_HIGH;
    DELAY();
    I2C_SDA_HIGH;
    DELAY();
    I2C_SDA_INPUT;
}

/**
 * @brief I2C send ACK
 * @param[in] none
 * @return none
 */
void sw_i2c_send_ack(void)
{
    I2C_SDA_OUTPUT;
    I2C_SDA_LOW;
    DELAY();
    I2C_SCK_HIGH;
    DELAY();
    I2C_SCK_LOW;
    DELAY();
    I2C_SDA_HIGH;
}

/**
 * @brief I2C send NACK
 * @param[in] none
 * @return none
 */
void sw_i2c_send_nack(void)
{
    I2C_SDA_OUTPUT;

    I2C_SDA_HIGH;
    DELAY();
    I2C_SCK_HIGH;
    DELAY();
    I2C_SCK_LOW;
    DELAY();
}

/**
 * @brief I2C write byte
 * @param[in] data Byte to write
 * @return I2C bus state
 */
I2C_Bus_State_t sw_i2c_write_byte(uint8_t data)
{
    int i;
    uint32_t m;

    DELAY();

    I2C_SDA_OUTPUT;
    I2C_SCK_LOW;
    DELAY();
    I2C_SDA_LOW;

    for (i = 0; i < 8; i++)
    {
        DELAY();
        if (data & 0x80)
        {
            I2C_SDA_HIGH;
        }
        else
        {
            I2C_SDA_LOW;
        }
        data <<= 1;
        DELAY();
        I2C_SCK_HIGH;
        DELAY();
        I2C_SCK_LOW;
    }
    I2C_SDA_INPUT;

    DELAY();
    I2C_SCK_HIGH;
    DELAY();

    m = ACK_TIMEOUT_DEF;
    DELAY();

    do
    {
        if (!I2C_SDA_STATUS)
        {
            I2C_SCK_LOW;
            return SI2C_ACK;
        }
        DELAY();
    } while (--m > 0);

    I2C_SDA_OUTPUT;
    DELAY();
    I2C_SCK_LOW;

    return SI2C_NACK;
}

/**
 * @brief I2C read byte
 * @param[in] none
 * @return Data read
 */
uint8_t sw_i2c_read_byte(void)
{
    int i;
    uint8_t data = 0;

    DELAY();
    I2C_SDA_INPUT;
    DELAY();
    I2C_SCK_LOW;
    DELAY();

    for (i = 0; i < 8; i++)
    {
        I2C_SCK_HIGH;
        DELAY();
        data <<= 1;
        if (I2C_SDA_STATUS)
        {
            data |= 0x01;
        }
        I2C_SCK_LOW;
        DELAY();
    }
    return data;
}

/**
 * @brief I2C write buffer
 * @param[in] addr Register address
 * @param[in] buf Buffer to write
 * @param[in] length Buffer length
 * @return I2C bus state
 */
I2C_Bus_State_t sw_i2c_write_buf(uint8_t addr, uint8_t *buf, uint16_t length)
{
    uint8_t i;
    I2C_Bus_State_t status;

    status = sw_i2c_start();
    if (status != SI2C_READY)
    {
        goto end;
    }

    status = sw_i2c_write_byte(DEVICE_ADDR << 1);
    if (status != SI2C_ACK)
    {
        goto end;
    }

    status = sw_i2c_write_byte(addr << 1);
    if (status != SI2C_ACK)
    {
        goto end;
    }

    for (i = 0; i < length; i++)
    {
        status = sw_i2c_write_byte(buf[i]);
        if (status != SI2C_ACK)
        {
            goto end;
        }
    }
    status = SI2C_READY;
end:

    sw_i2c_stop();
    return status;
}

/**
 * @brief I2C read buffer
 * @param[in] addr Register address
 * @param[out] buf Buffer to read into
 * @param[in] length Buffer length
 * @return I2C bus state
 */
I2C_Bus_State_t sw_i2c_read_buf(uint8_t addr, uint8_t *buf, uint16_t length)
{
    uint8_t i;
    I2C_Bus_State_t status;

    status = sw_i2c_start();
    if (status != SI2C_READY)
    {
        goto end;
    }

    status = sw_i2c_write_byte((DEVICE_ADDR << 1));
    if (status != SI2C_ACK)
    {
        goto end;
    }

    status = sw_i2c_write_byte((addr << 1) | 0x01);
    if (status != SI2C_ACK)
    {
        goto end;
    }
    
    /*Repeat Start*/
    status = sw_i2c_start();
    if (status != SI2C_READY)
    {
        goto end;
    }

    status = sw_i2c_write_byte((DEVICE_ADDR << 1) | 0x01);
    if (status != SI2C_ACK)
    {
        goto end;
    }

    for (i = 0; i < length; i++)
    {
        buf[i] = sw_i2c_read_byte();
        if (i != length - 1)
        {
            sw_i2c_send_ack();
        }
        else
        {
            sw_i2c_send_nack();
        }
    }
    status = SI2C_READY;
end:

    sw_i2c_stop();

    return status;
}

void sw_i2c_reset(void)
{
    uint8_t m = 10;
    uint8_t i = 0;
    while(m--)
    {
        //上一次通I2c通信可能还未结束，且I2c总线上设备拉低的能力比拉高的能力更强
        for(i =0;i < 8;i++)
        {
            I2C_SCK_HIGH;
            DELAY();
            I2C_SCK_LOW;
            DELAY();
        }
        sw_i2c_stop();
        if (I2C_SDA_STATUS)
        {
            break;
        }
    }
}

#endif
