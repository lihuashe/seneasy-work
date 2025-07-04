/******************************************************************************
 * @file     spi.h
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
#ifndef SPI_H
#define SPI_H

#include "bsp.h"
#include "gpio.h"
#include "compiler.h"
/**
 *  @brief  Define pin for SPI interface
 *          SDO  SDI  SCK  CN
 *          A2   A3   A4   D6
 *          B7   B6   D7   D2
 */

typedef enum {
    SPI_GPIO_SCL_A4=GPIO_PA4,
    SPI_GPIO_SCL_D7=GPIO_PD7,
}SPI_GPIO_SclkTypeDef;

typedef enum {
    SPI_GPIO_SDO_A2=GPIO_PA2,
    SPI_GPIO_SDO_B7=GPIO_PB7,
}SPI_GPIO_SdoTypeDef;

typedef enum {
    SPI_GPIO_SDI_A3=GPIO_PA3,
    SPI_GPIO_SDI_B6=GPIO_PB6,
}SPI_GPIO_SdiTypeDef;

typedef enum {
    SPI_GPIO_CS_D2=GPIO_PD2,
    SPI_GPIO_CS_D6=GPIO_PD6,
}SPI_GPIO_CsTypeDef;


/**
 *  @brief  Define the mode for SPI interface
 */
typedef enum {
    SPI_MODE0 = 0,
    SPI_MODE2,
    SPI_MODE1,
    SPI_MODE3,
} SPI_ModeTypeDef;

/**
 *  @brief  Define the clk for SPI interface(system clock = 24M)
 */
typedef enum {
    SPI_CLK_200K = 0x3c,
    SPI_CLK_250K = 0x2e,
    SPI_CLK_500K = 0x17,
    SPI_CLK_1M   = 0x0b,
    SPI_CLK_2M   = 0x05,
    SPI_CLK_4M   = 0x02,
} SPI_24M_ClkTypeDef;


/**
 * @brief This function reset SPI module.
 * @param[in] none
 * @return none
 */


static inline void reset_spi_module(void)
{
    reg_rst0 |= FLD_RST0_SPI;
    reg_rst0 &= (~FLD_RST0_SPI);
}
/**
 * @brief     This function selects a pin port for the SPI interface (master mode)
 * @param[in] PinGrp - the selected pin port
 * @return    none
 *  A3:5b7[0] set 1 as spi input,set 0 not as spi input ;5b7[4] set 1 as i2c input ,set 0 not as i2c input
 *    A4:5b7[1] set 1 as spi input,set 0 not as spi input ;5b7[5] set 1 as i2c input ,set 0 not as i3c input
 *    B6:5b7[2] set 1 as spi input,set 0 not as spi input ;5b7[6] set 1 as i2c input ,set 0 not as i4c input
 *    D7:5b7[3] set 1 as spi input,set 0 not as spi input ;5b7[7] set 1 as i2c input ,set 0 not as i5c input
 *    5b6[7:4]  set spi or i2c output,5b6[5:4] PA spi sel, 5b6[6] PB spi sel,5b6[7] PD spi_sel
 *    spi i2c mux sel:
 *    step1:close gpio func;
 *    step2:set 0x5a8-0x5af sel spii2c func;
 *    step3:set 5b6[7:4] to sel spi or i2c output;
 *    step4:set 5b7[7:0] to sel spi or i2c input;
 *    step5 if use pd[6]as spi slave csn,need to open pd[2] gpio func,because pd[2]have high priority than pd[6]
 */


extern void spi_master_gpio_set(SPI_GPIO_SclkTypeDef sclk_pin,SPI_GPIO_CsTypeDef cs_pin,SPI_GPIO_SdoTypeDef sdo_pin, SPI_GPIO_SdiTypeDef sdi_pin );

/**
 * @brief     This function selects a pin port for the SPI interface (slave mode)
 * @param[in] PinGrp - the selected pin port
 * @return    none
 *  A3:5b7[0] set 1 as spi input,set 0 not as spi input ;5b7[4] set 1 as i2c input ,set 0 not as i2c input
 *    A4:5b7[1] set 1 as spi input,set 0 not as spi input ;5b7[5] set 1 as i2c input ,set 0 not as i3c input
 *    B6:5b7[2] set 1 as spi input,set 0 not as spi input ;5b7[6] set 1 as i2c input ,set 0 not as i4c input
 *    D7:5b7[3] set 1 as spi input,set 0 not as spi input ;5b7[7] set 1 as i2c input ,set 0 not as i5c input
 *    5b6[7:4]  set spi or i2c output,5b6[5:4] PA spi sel, 5b6[6] PB spi sel,5b6[7] PD spi_sel
 *    spi i2c mux sel:
 *    step1:close gpio func;
 *    step2:set 0x5a8-0x5af sel spii2c func;
 *    step3:set 5b6[7:4] to sel spi or i2c output;
 *    step4:set 5b7[7:0] to sel spi or i2c input;
 *    step5 if use pd[6]as spi slave csn,need to open pd[2] gpio func,because pd[2]have high priority than pd[6]
 */



extern void spi_slave_gpio_set(SPI_GPIO_SclkTypeDef sclk_pin,SPI_GPIO_CsTypeDef cs_pin,SPI_GPIO_SdoTypeDef sdo_pin, SPI_GPIO_SdiTypeDef sdi_pin);


/**
 * @brief     This function selects a GPIO pin as CS of SPI function.
 * @param[in] CSPin - the selected CS pin
 * @return    none
 */



extern void spi_masterCSpin_select(GPIO_PinTypeDef CSPin);

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] DivClock - the division factor for SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] Mode - the selected working mode of SPI module
 *            Telink spi supports four standard working modes
 *            register  0x0b set working mode
 *            bit0:CPOL-Clock Polarity  ; bit1:CPHA-Clock Phase
 *            MODE0: CPOL = 0 , CPHA =0;
 *            MODE1: CPOL = 0 , CPHA =1;
 *            MODE2: CPOL = 1 , CPHA =0;
 *            MODE3: CPOL = 1 , CPHA =1;
 * @return    none
 */
extern void spi_master_init(unsigned char DivClock, SPI_ModeTypeDef Mode);
/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] DivClock - the division factor for SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] Mode - the selected working mode of SPI module (slave mode)
 * @return    none
 */
extern void spi_slave_init(unsigned char DivClock, SPI_ModeTypeDef Mode);
/**
 * @brief      This function serves to write a bulk of data to the SPI slave
 *             device specified by the CS pin
 * @param[in]  Cmd - pointer to the command bytes needed written into the
 *             slave device first before the writing operation of actual data
 * @param[in]  CmdLen - length in byte of the command bytes
 * @param[in]  Data - pointer to the data need to write
 * @param[in]  DataLen - length in byte of the data need to write
 * @param[in]  CSPin - the CS pin specifing the slave device
 * @return     none
 */


extern void spi_write(unsigned char *Cmd, int CmdLen, unsigned char *Data, int DataLen, GPIO_PinTypeDef CSPin);

/**
 * @brief      This function serves to read a bulk of data from the SPI slave
 *             device specified by the CS pin
 * @param[in]  Cmd - pointer to the command bytes needed written into the
 *             slave device first before the reading operation of actual data
 * @param[in]  CmdLen - length in byte of the command bytes
 * @param[out] Data - pointer to the buffer that will cache the reading out data
 * @param[in]  DataLen - length in byte of the data need to read
 * @param[in]  CSPin - the CS pin specifing the slave device
 * @return     none
 */


extern void spi_read(unsigned char *Cmd, int CmdLen, unsigned char *Data, int DataLen, GPIO_PinTypeDef CSPin);

/**
 * @brief         This function use to set the share mode of SPI.
 * @param[in]     none
 * @return         none
 */
extern void spi_set_share_mode_en(void);

#endif

