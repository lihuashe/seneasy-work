/******************************************************************************
 * @file     i2c.h
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

#pragma once

#ifndef I2C_H
#define I2C_H
#include "gpio.h"


#define I2C_SLAVE_DEVICE_NO_START_EN                           0

/**
 *  @brief  select pin as SDA and SCL of i2c
 */
typedef enum{
    I2C_GPIO_SDA_A3        = GPIO_PA3,
    I2C_GPIO_SDA_B6        = GPIO_PB6,
    I2C_GPIO_SDA_C0        = GPIO_PC0,
    I2C_GPIO_SDA_C2        = GPIO_PC2,
}I2C_GPIO_SdaTypeDef;

typedef enum{
    I2C_GPIO_SCL_A4        = GPIO_PA4,
    I2C_GPIO_SCL_D7        = GPIO_PD7,
    I2C_GPIO_SCL_C1        = GPIO_PC1,
    I2C_GPIO_SCL_C3        = GPIO_PC3,
}I2C_GPIO_SclTypeDef;


/**
 *  @brief  select i2c slave mode: DMA and MAPPING
 *
 *   dma mode:
 *              all sram(64k area form 0x40000~0x4ffff, address 3 byte) could be used for reading or writing data buffer,
 *             i2c master just need send sram address(3 byte) on i2c bus in i2c reading or writing, reading/writing
 *             sram address could be anywhere and could be different.
 *               i2c master timing is:
 *                  1.master write:  start + device_id(W) + address2+ address1 + address0 + data(1,2...n byte)  +  stop
 *                  2.master read:   start + device_id(W) + address2+ address1 + address0 + start + device_id(R) + data(1,2...n byte) + stop
 *
 *   mapping mode:
 *              i2c slave data buffer sram could be configured by function: void i2c_slave_mapping_mode_data_buffer_config(unsigned char * pMapBuf),
 *              and it is also configured in function: void i2c_slave_init(unsigned char device_ID,I2C_SlaveMode mode,unsigned char * pMapBuf)
 *               reading/writing data buffer configured by pMapBuf addressed in core_e1/e2/e3,
 *               for i2c master: writing data buffer is pMapBuf,and reading data buffer is (pMapBuf+64) (this offset 64 is managed by MCU hardware, user can not change it)
 *
 *              i2c master no need send any address when reading or writing data, which means i2c master timing is:
 *                  1.master write:  start + device_id(W) + data(1,2...n byte)  +  stop
 *                  2.master read:   start + device_id(W) + start + device_id(R) + data(1,2...n byte) + stop
 *
 *
 */
typedef enum {
    I2C_SLAVE_DMA = 0,
    I2C_SLAVE_MAP,
}I2C_SlaveMode;


/**
 * @brief This function reset I2C module.
 * @param[in] none
 * @return none
 */
static inline void reset_i2c_module(void)
{
    reg_rst0 |= FLD_RST0_I2C;
    reg_rst0 &= (~FLD_RST0_I2C);
}

/**
 * @brief This function serves to set id of I2C module.
 * @param[in] SlaveID - this id is fixed id for slave device.For master device, this id is set to access different slave devices.
 * @return none
 */
static inline void i2c_set_id(unsigned char SlaveID)
{
    reg_i2c_id      = SlaveID; //slave address
}

/**
 * @brief This function servers to config i2c data buffer in slave mode.
 * @param[in] pMapBuf - to store the buffer data.
 * @return none
 */
static inline void i2c_slave_mapping_mode_data_buffer_config(unsigned char * pMapBuf)
{
     reg_i2c_slave_map_addrl  = (unsigned char)(((unsigned int)pMapBuf & 0xff)); //
     reg_i2c_slave_map_addrm = (unsigned char)(((unsigned int)pMapBuf>>8)&0xff);
     reg_i2c_slave_map_addrh = 0x04;
}
/**
 * @brief      This function serves to select a pin port for I2C interface.
 * @param[in]  i2c_pin_group - the pin port selected as I2C interface pin port.
 * @return     none
 */
void i2c_gpio_set(I2C_GPIO_SdaTypeDef sda_pin,I2C_GPIO_SclTypeDef scl_pin);

/**
 * @brief      This function serves to set the id of slave device and the speed of I2C interface
 *             note: the param ID contain the bit of writing or reading.
 *             eg:the parameter 0x5C. the reading will be 0x5D and writing 0x5C.
 * @param[in]  SlaveID - the id of slave device.it contains write or read bit,the lsb is write or read bit.
 *                       ID|0x01 indicate read. ID&0xfe indicate write.
 * @param[in]  DivClock - the division factor of I2C clock,
 *             I2C clock = System clock / (4*DivClock);if the datasheet you look at is 2*,pls modify it.
 * @return     none
 */
void i2c_master_init(unsigned char SlaveID, unsigned char DivClock);


/**
 *  @brief      This function serves to set the ID and mode of slave device.
 *  @param[in]  device_ID - it contains write or read bit,the lsb is write or read bit.
 *              ID|0x01 indicate read. ID&0xfe indicate write.
 *  @param[in]  mode - set slave mode. slave has two modes, one is DMA mode, the other is MAPPING mode.
 *  @param[in]  pMapBuf - if slave mode is MAPPING, set the first address of buffer master write or read slave.
 *  @return     none
 */
void i2c_slave_init(unsigned char device_ID,I2C_SlaveMode mode,unsigned char * pMapBuf);


/**
 * @brief      This function serves to write one byte to the slave device at the specified address
 * @param[in]  Addr - i2c slave address where the one byte data will be written
 * @param[in]  AddrLen - length in byte of the address, which makes this function is
 *             compatible for slave device with both one-byte address and two-byte address
 * @param[in]  Data - the one byte data will be written via I2C interface
 * @return     none
 */
void i2c_write_byte(unsigned int Addr, unsigned int AddrLen, unsigned char Data);

/**
 * @brief      This function serves to read one byte from the slave device at the specified address
 * @param[in]  Addr - i2c slave address where the one byte data will be read
 * @param[in]  AddrLen - length in byte of the address, which makes this function is
 *             compatible for slave device with both one-byte address and two-byte address
 * @return     the one byte data read from the slave device via I2C interface
 */
unsigned char i2c_read_byte(unsigned int Addr, unsigned int AddrLen);

/**
 *  @brief      This function serves to write a packet of data to the specified address of slave device
 *  @param[in]  Addr - the register that master write data to slave in. support one byte and two bytes. i.e param2 AddrLen may be 1 or 2.
 *  @param[in]  AddrLen - the length of register. enum 0 or 1 or 2 or 3. based on the spec of i2c slave.
 *  @param[in]  dataBuf - the first SRAM buffer address to write data to slave in.
 *  @param[in]  dataLen - the length of data master write to slave.
 *  @return     none
 */
void i2c_write_series(unsigned int Addr, unsigned int AddrLen, unsigned char * dataBuf, int dataLen);
/**
 * @brief      This function serves to read a packet of data from the specified address of slave device
 * @param[in]  Addr - the register master read data from slave in. support one byte and two bytes.
 * @param[in]  AddrLen - the length of register. enum 0 or 1 or 2 or 3 based on the spec of i2c slave.
 * @param[in]  dataBuf - the first address of SRAM buffer master store data in.
 * @param[in]  dataLen - the length of data master read from slave.
 * @return     none.
 */
void i2c_read_series(unsigned int Addr, unsigned int AddrLen, unsigned char * dataBuf, int dataLen);

#endif
