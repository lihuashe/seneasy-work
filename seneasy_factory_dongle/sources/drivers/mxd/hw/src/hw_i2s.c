/**
 * @file     hw_i2s.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/6/10
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "type_def.h"
#include "utility.h"

/**********************************************************************************************************************
 *                                             I2S Interrupt Cfg
 *********************************************************************************************************************/

/**
 * @brief  Get indicated i2s interrupt flag(status).
 * @param  pu32Flag: Indicate which interrupt flag will be read.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_get_interrupt_flag(uint32_t *pu32Flag)
{
    if (NULL == pu32Flag)
    {
        return ERR_PARA_ERR;
    }

    *pu32Flag = I2S->I2S_INT_FLAG & I2S_INT_MASK;

    return ERR_STA_OK;
}

/**
 * @brief  Clear indicated I2S interrupt flag(status).
 * @param  u32Flag: Indicate which flag will be cleared
 *                  bit-1 means clear
 *                  bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_clear_interrupt_flag(uint32_t u32Flag)
{
    if (u32Flag > I2S_INT_MASK)
    {
        return ERR_PARA_ERR;
    }

    I2S->I2S_INT_CLR = u32Flag & I2S_INT_MASK;

    return ERR_STA_OK;
}

/**
 * @brief  Enable indicated i2s interrupt.
 * @param  u32IntEn: Indicate which interrupt will be enable,
 *                   bit-1 means enable
 *                   bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_enable_interrupt(uint32_t u32IntEn)
{
    if (u32IntEn > I2S_INT_MASK)
    {
        return ERR_PARA_ERR;
    }

    I2S->I2S_INT_CLR = u32IntEn;
    I2S->I2S_INT_EN |= (u32IntEn & I2S_INT_MASK);

    return ERR_STA_OK;
}

/**
 * @brief  disable indicated i2s interrupt.
 * @param  u32IntDis: Indicate which interrupt will be disable,
 *                    bit-1 means disable
 *                    bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_disable_interrupt(uint32_t u32IntDis)
{
    if (u32IntDis > I2S_INT_MASK)
    {
        return ERR_PARA_ERR;
    }

    I2S->I2S_INT_EN &= ~(u32IntDis & I2S_INT_MASK);
    I2S->I2S_INT_CLR = u32IntDis;

    return ERR_STA_OK;
}

/**********************************************************************************************************************
 *                                             I2S Module Cfg
 *********************************************************************************************************************/

/**
 * @brief  I2S module deinit.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_deinit(void)
{
    I2S->I2S_INT_EN  = 0x00000000;
    I2S->I2S_INT_CLR = I2S_INT_MASK;
    I2S->I2S_CFG2    = I2S_CFG2_DEFAULT;
    I2S->I2S_CFG     = I2S_CFG_DEFAULT;

    return ERR_STA_OK;
}

/**
 * @brief  Configure I2S.
 * @param  u32Cfg: I2C configuation.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_init(uint32_t u32Cfg)
{
    I2S->I2S_CFG = u32Cfg & I2S_CFG_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Set i2s work mode.
 * @param  enWorkmode: Config work mode, @ref EN_I2S_WORK_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_set_work_mode(EN_I2S_WORK_MODE_T enWorkmode)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_I2S_WORK_MODE(enWorkmode))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = I2S->I2S_CFG;
    u32Cfg &= ~(I2S_WORK_MODE_MSK << I2S_WORK_MODE_SHIFT);
    u32Cfg |= ((enWorkmode & I2S_WORK_MODE_MSK) << I2S_WORK_MODE_SHIFT);
    I2S->I2S_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Set i2s wclk width which determine the sampling rate. fWCLK = fBCLK / wclkWidth / 2.
 * @param  enWclkWidth: wclk width or data Frame length.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_set_wclk_freq(uint8_t enWclkWidth)
{
    uint32_t u32Cfg = 0;

    u32Cfg = I2S->I2S_CFG2;
    u32Cfg &= ~(I2S_WCLK_WIDTH_MSK << I2S_WCLK_WIDTH_SHIFT);
    u32Cfg |= ((enWclkWidth & I2S_WCLK_WIDTH_MSK) << I2S_WCLK_WIDTH_SHIFT);
    I2S->I2S_CFG2 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Enable i2s tx mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_enable_tx(void)
{
    I2S->I2S_CFG2 |= (ENABLE & I2S_TX_ENABLE_MSK) << I2S_TX_ENABLE_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable i2s tx mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_disable_tx(void)
{
    I2S->I2S_CFG2 &= ~((ENABLE & I2S_TX_ENABLE_MSK) << I2S_TX_ENABLE_SHIFT);

    return ERR_STA_OK;
}

/**********************************************************************************************************************
 *                                             I2S Module Trx Data
 *********************************************************************************************************************/

/**
 * @brief  i2s read rx fifo
 * @param  pu32RxData:pointer the data，read from i2s rx fifo,on rx fifo not empty
 * @param  u16Size:number of data sample to be revice
 * @param  u32TimeoutUs:Maximum time to receive u16Sizedata,step 1us
 * @note   The minimum u32TimeoutUs is 200us. i2s freq min = 8Khz(T = 128us) , Send Data max size = 32,so min timeout
 * time is 128us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_read_rx_fifo(uint32_t *pu32RxData, uint16_t u16Size, uint32_t u32TimeoutUs)
{
    uint32_t  u32Timout    = 0;
    uint32_t  u32I2sStatus = 0;
    uint16_t  u16I2sRxSize = u16Size;
    uint32_t *pu32I2sRxDataBuf;
    uint8_t   u8I2sRxfifoFull = 0;

    if ((pu32RxData == NULL) || (u16I2sRxSize == 0))
    {
        return ERR_PARA_ERR;
    }

    pu32I2sRxDataBuf = pu32RxData;

    /* 1.Clear not empty & full & underflow & overflow */
    I2S->I2S_INT_CLR = I2S_INT_MASK;

    /* 2.Start Revice I2s Data */
    while (u16I2sRxSize > 0U)
    {
        u32I2sStatus = I2S->I2S_INT_FLAG;

        /*3.1 Check i2s status: not empty or fifo full */
        if (u32I2sStatus & (I2S_RX_FIFO_NOT_EMPTY | I2S_RX_FIFO_FULL))
        {
            /* 4. If rx full, read multiple fifo times */
            if ((u32I2sStatus)&I2S_RX_FIFO_FULL)
            {
                u8I2sRxfifoFull = (u16I2sRxSize > I2S_RX_FIFO_DEPTH) ? I2S_RX_FIFO_DEPTH : u16I2sRxSize;

                while (u8I2sRxfifoFull > 0U)
                {
                    *pu32I2sRxDataBuf = (uint32_t)(I2S->I2S_RX_FIFO);
                    pu32I2sRxDataBuf++;
                    u8I2sRxfifoFull--;
                    u16I2sRxSize--;
                }
            }
            else
            {
                *pu32I2sRxDataBuf = (uint32_t)(I2S->I2S_RX_FIFO);
                pu32I2sRxDataBuf++;
                u16I2sRxSize--;
            }

            /* 5.clear i2s Status */
            I2S->I2S_INT_CLR = u32I2sStatus;
        }
        else
        {
            u32TimeoutUs++;
            rom_delay_us(1);
        }

        /*3.2 Check i2s status :over flow or underflow */
        u32I2sStatus = I2S->I2S_INT_FLAG;
        if (u32I2sStatus & (I2S_RX_FIFO_OVER_FLOW | I2S_RX_FIFO_UNDER_FLOW))
        {
            I2S->I2S_INT_CLR = I2S_RX_FIFO_OVER_FLOW | I2S_RX_FIFO_UNDER_FLOW;
            return ERR_STA_ERROR;
        }

        /* 3.3 Check timeout  */
        if (u32TimeoutUs <= u32Timout)
        {
            return ERR_STA_TIMEOUT;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  i2s send tx fifo
 * @param  pu32TxData:pointer the data，send to  i2s tx fifo
 * @param  u16Size:number of data sample to be revice
 * @param  u32TimeoutUs:Maximum time to Sendu16Sizedata,step 1us
 * @note   The minimum u32TimeoutUs is 200us. i2s freq min = 8Khz(T = 128us) , Send Data max size = 32,so min timeout
 * time is 128us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_i2s_send_tx_fifo(uint32_t *pu32TxData, uint16_t u16Size, uint32_t u32TimeoutUs)
{
    uint16_t  u16I2sTxSize = u16Size;
    uint32_t *pu32TxDataBuf;
    uint8_t   u8TxEnStatus            = 0;
    uint32_t  u32TxSendStatus         = 0;
    uint32_t  u32Timout               = 0;
    uint32_t  u32TxShiftdoneTimeoutUs = 200; // min 200US

    if ((pu32TxData == NULL) || (u16Size == 0))
    {
        return ERR_PARA_ERR;
    }

    pu32TxDataBuf = pu32TxData;

    /* 2.Clear i2s int status */
    I2S->I2S_INT_CLR = I2S_TX_FIFO_FULL | I2S_TX_FIFO_OVER_FLOW;

    /* 2.Send i2s tx data */
    while (u16I2sTxSize > 0U)
    {
        u32TxSendStatus = I2S->I2S_INT_FLAG;

        /* 3. check tx fifo empty  */
        if (u32TxSendStatus & I2S_TX_FIFO_EMPTY)
        {
            I2S->I2S_INT_CLR = I2S_TX_FIFO_EMPTY;
            I2S->I2S_TX_FIFO = (*pu32TxDataBuf);

            pu32TxDataBuf++;
            u16I2sTxSize--;

            while (!(I2S->I2S_INT_FLAG & I2S_TX_DONE))
            {
                u32TxShiftdoneTimeoutUs--;
                u32Timout++;
                rom_delay_us(1);

                if ((u32TxShiftdoneTimeoutUs == 0) || (u32Timout >= u32TimeoutUs))
                {
                    return ERR_STA_TIMEOUT;
                }
            }

            u32TxShiftdoneTimeoutUs = 200;
            I2S->I2S_INT_CLR        = I2S_TX_DONE;
        }
        else
        {
            u32Timout++;
        }

        if (u32Timout >= u32TimeoutUs)
        {
            return ERR_STA_TIMEOUT;
        }

        rom_delay_us(1);
    }

    return ERR_STA_OK;
}
