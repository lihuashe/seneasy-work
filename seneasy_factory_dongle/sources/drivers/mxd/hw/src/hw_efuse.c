/**********************************************************************************************************************
 * @file     hw_efuse.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/1/14
 * @history
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "type_def.h"
#include "err_def.h"
#include "utility.h"

#include "hw_efuse.h"


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define TPD_TIME_US                        ( 1 )
#define TRPW_SET_FREQ                      ( 32000000 ) // 32M
#define TPI_TIME_US                        ( 2 )
#define TPP_TIME_US                        ( 5 )

#define PROGRAM_TIMEOUT_US                 ( 5000 )
#define PROGRAM_TIMEOUT_CHK_STEP_US        ( 10 )

#define GET_CLK_COUNT(freq, us)            ( (freq) * (us) / 1000000U )




/**
 * @brief  eFuse init.
 * @param  u32Hz: Config eFuse clock, set when APB clk changed, (default 16000000).
 * @return status @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_efuse_init(uint32_t u32Hz)
{
    uint32_t u32Trpw;
    uint32_t u32TpdCount;
    uint32_t u32TppCount;
    uint32_t u32TpiCount;

    if (0 == u32Hz)
    {
        return ERR_PARA_ERR;
    }

    u32TpdCount = GET_CLK_COUNT(u32Hz, TPD_TIME_US);
    if (u32TpdCount > EFUSE_TPD_TIME_MSK)
    {
        return ERR_STA_ERROR;
    }

    u32Trpw = (u32Hz >= (TRPW_SET_FREQ)) ? 1 : 0;

    u32TpiCount = GET_CLK_COUNT(u32Hz, TPI_TIME_US);
    if (u32TpiCount > EFUSE_TPI_TIME_MSK)
    {
        return ERR_STA_ERROR;
    }

    u32TppCount = GET_CLK_COUNT(u32Hz, TPP_TIME_US);
    if (u32TppCount > EFUSE_TPP_TIME_MSK)
    {
        return ERR_STA_ERROR;
    }

    EFUSE->EFUSE_CFG = ((u32TpdCount << EFUSE_TPD_TIME_SHIFT) \
                      | (u32Trpw << EFUSE_TRPW_TIME_SHIFT)    \
                      | (u32TpiCount << EFUSE_TPI_TIME_SHIFT) \
                      | (u32TppCount << EFUSE_TPP_TIME_SHIFT));

    return ERR_STA_OK;
}

/**
 * @brief  eFuse read bytes.
 * @param  u8Addr: Indicate which addr will read, @ref EN_EFUSE_ADDR_T.
 * @param  pu8Data: Indicate which data will read.
 * @param  u8Len: Length of pu8Data.
 * @return status @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_efuse_read_bytes(uint8_t u8Addr, uint8_t *pu8Data, uint8_t u8Len)
{
    uint32_t i = 0;

    if ((NULL == pu8Data) || (0 == u8Len) || (u8Addr + u8Len > (EFUSE_SIZE_BYTES)))
    {
        return ERR_PARA_ERR;
    }

    for (i = 0; i < u8Len; i++)
    {
        /* Low 8bit of a word is used */
        pu8Data[i] = EFUSE_ARRAY[u8Addr + i] & 0xFF;
    }

    /* Direct read no need check status */

    return ERR_STA_OK;
}

/**
 * @brief  EFUSE write bytes to efuse with indirect mode.
 * @param  u8Addr: Indicate which addr will write, @ref EN_EFUSE_ADDR_T.
 * @param  pu8Data: Indicate which data will write.
 * @param  u8Len: Length of pu8Data.
 * @return status @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_efuse_write_bytes(uint8_t u8Addr, uint8_t *pu8Data, uint8_t u8Len)
{
    uint32_t u32Reg = 0;
    uint32_t i = 0;
    uint32_t u32Timeout = 0;

    if ((NULL == pu8Data) || (0 == u8Len) || (u8Addr + u8Len > EFUSE_SIZE_BYTES))
    {
        return ERR_PARA_ERR;
    }

    /* Turn on 2.5v voltage */
    EFUSE->EFUSE_POWER = EFUSE_POWER_EN_MSK << EFUSE_POWER_EN_SHIFT;

    for (i = 0; i < u8Len; i++)
    {
        /* Set addr */
        EFUSE->EFUSE_ADDR &= ~(EFUSE_ADDR_MSK << EFUSE_ADDR_SHIFT);
        EFUSE->EFUSE_ADDR |= ((u8Addr + i) & EFUSE_ADDR_MSK) << EFUSE_ADDR_SHIFT;

        /* Set data */
        EFUSE->EFUSE_DATA &= ~(EFUSE_DATA_MSK << EFUSE_DATA_SHIFT);
        EFUSE->EFUSE_DATA |= (pu8Data[i] & EFUSE_DATA_MSK) << EFUSE_DATA_SHIFT;

        /* Clear flag(optional,because trig program will automatically clear flag) */
        EFUSE->EFUSE_CMD = EFUSE_CMD_CLR_MSK << EFUSE_CMD_CLR_SHIFT;

        /* Trig program */
        EFUSE->EFUSE_CMD = EFUSE_CMD_PROGM_MSK << EFUSE_CMD_PROGM_SHIFT;

        /* Wait for programing done. */
        u32Timeout = 0;
        while (0 == (EFUSE->EFUSE_FLAG & (EFUSE_FLAG_END_MSK << EFUSE_FLAG_END_SHIFT)))
        {
            rom_delay_us(PROGRAM_TIMEOUT_CHK_STEP_US);

            u32Timeout += PROGRAM_TIMEOUT_CHK_STEP_US;

            if (u32Timeout >= PROGRAM_TIMEOUT_US)
            {
                /* Turn off 2.5v voltage */
                EFUSE->EFUSE_POWER &= ~(EFUSE_POWER_EN_MSK << EFUSE_POWER_EN_SHIFT);

                return ERR_STA_ERROR;
            }
        }
    }

    /* Clear flag (optional) */
    EFUSE->EFUSE_CMD = EFUSE_CMD_CLR_MSK << EFUSE_CMD_CLR_SHIFT;

    /* Turn off 2.5v voltage */
    EFUSE->EFUSE_POWER &= ~(EFUSE_POWER_EN_MSK << EFUSE_POWER_EN_SHIFT);

    return ERR_STA_OK;
}


