/**
 * @file     patch_hal_adc.c
 * @version  V1.0
 * @date     2023/8/19
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_efuse.h"
#include "hw_adc.h"
#include "hal_adc.h"

#include "utility.h"

/**
 * @brief  DEFINE
 */
#define ADC_STABLE_TIME_CYCLE    (640)
#define CHECK_ADC_STABLE_TIME_US (80)
#define ADC_MAX_TIMEOUT_US       (10 * 1000)

/**
 * @brief  ADC tune value
 */
static stADCTuneValue_t m_stADCTuneValue;

/**
 * @brief  ADC init function.
 * @param  pstADCInitHandle: ADC init handle, @ref stADCInitHandle_t.
 *         .pstADCHandle: ADC handle, should be ADC0 / ADC1;
 *         .u8Resolution: ADC resolution, should be 10Bit / 12Bit;
 *         .enCh: ADC sample channel(s), @ref EN_ADC_CH_T;
 *         .enSampMode: ADC sample mode, @ref EN_ADC_SAMP_MODE_SEL_T;
 *         .enSampSrc: ADC sample source, @ref EN_ADC_SAMP_SRC_SEL_T;
 *         .enPwmSampSrc: @ref EN_ADC_PWM_SAMP_SRC_SEL_T;
 * @param  u16SampNum: Each adc channel sample number when sample mode is ADC_SAMP_BY_AUTO_TRIG,
 *                     can not set to 0 when sample mode is others.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T patch_hal_adc_init(stADCInitHandle_t *pstADCInitHandle, uint16_t u16SampNum)
{
    EN_ERR_STA_T enRet = 0;

    // 1. Read ADC tune value.
    rom_hw_efuse_read_bytes(EFUSE_ADC0_VIO_LO_TUNE_ADDR, (uint8_t *)&m_stADCTuneValue, sizeof(stADCTuneValue_t));

    // 2. ADC init.
    enRet = rom_hal_adc_init(pstADCInitHandle, u16SampNum);
    ERR_RETURN_IF_ERROR(enRet);

    // 3. Set ADC stable time.
    pstADCInitHandle->pstADCHandle->ADC_STABLE_TIME = ADC_STABLE_TIME_CYCLE;

    return enRet;
}

/**
 * @brief  Get adc value for one channel.
 * @param  pstADCInitHandle: ADC init handle, @ref stADCInitHandle_t.
 * @param  enCh: ADC channel.
 * @output pu16Value: Pointer to adc sample data.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T patch_hal_adc_get_data(stADCInitHandle_t *pstADCInitHandle, EN_ADC_CH_T enCh, uint16_t *pu16Value)
{
    uint32_t u32IntFlag     = 0;
    uint32_t u32Timeout     = 0;
    uint16_t u16ADCVal      = 0;
    uint16_t u16AdcRawValue = 0;

    if ((!(IS_VALID_ADC_HANDLE(pstADCInitHandle->pstADCHandle))) || (!(IS_VALID_ADC_CH(enCh))) || (NULL == pu16Value)
        || (!(pstADCInitHandle->enCh & enCh)))
    {
        return ERR_PARA_ERR;
    }

    // 1.Check ADC is ready or not.
    u32Timeout = 0;
    do
    {
        rom_hw_adc_get_interrupt_flag(pstADCInitHandle->pstADCHandle, &u32IntFlag);
        if (u32IntFlag & ADC_CH_READY)
        {
            break;
        }

        rom_delay_us(1);

        if (++u32Timeout >= CHECK_ADC_STABLE_TIME_US)
        {
            return ERR_STA_ERROR;
        }
    } while (1);

    rom_hw_adc_clear_interrupt_flag(pstADCInitHandle->pstADCHandle, ADC_INT_MASK);

    // 2. Trigger ADC start to sample.
    if (ADC_SAMP_BY_SW_TRIG == pstADCInitHandle->enSampMode)
    {
        rom_hw_adc_start_sample(pstADCInitHandle->pstADCHandle);
    }

    // 3. Get ADC data.
    u32Timeout = 0;
    while (u32Timeout <= ADC_MAX_TIMEOUT_US)
    {
        rom_hw_adc_get_interrupt_flag(pstADCInitHandle->pstADCHandle, &u32IntFlag);

        if (u32IntFlag & (ADC_READ_FIFO_UNDER_FLOW | ADC_BUSY_BY_AUDIO))
        {
            rom_hw_adc_clear_interrupt_flag(pstADCInitHandle->pstADCHandle,
                                            ADC_READ_FIFO_UNDER_FLOW | ADC_BUSY_BY_AUDIO);
            *pu16Value = 0;
            return ERR_STA_ERROR;
        }
        else
        {
            if (u32IntFlag & enCh)
            {
                rom_hw_adc_get_adc_rawdata(pstADCInitHandle->pstADCHandle, &u16AdcRawValue);
                rom_delay_cycles(1);
                rom_hal_adc_fifo_reg_to_lsb(pstADCInitHandle, u16AdcRawValue, &u16ADCVal);
                *pu16Value = u16ADCVal;

                return ERR_STA_OK;
            }

            rom_delay_us(1);
            u32Timeout++;
        }
    }

    *pu16Value = 0;

    return ERR_STA_TIMEOUT;
}

/**
 * @brief  Transform adc sample data to voltage.
 * @param  pstADCInitHandle: ADC init handle, @ref stADCInitHandle_t.
 * @param  enCh: ADC Channel, @ref EN_ADC_CH_T.
 * @param  u16ADCSampData: ADC sample data.
 * @param  pu16Volt: ADC voltage, unit is mV.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T patch_hal_adc_trans_voltage(stADCInitHandle_t *pstADCInitHandle, EN_ADC_CH_T enCh, uint16_t u16ADCSampData,
                                         uint16_t *pu16Volt)
{
    uint8_t u8Div   = 0;
    float   voltage = 0;

    if (NULL == pu16Volt)
    {
        return ERR_PARA_ERR;
    }

    if (m_stADCTuneValue.u16ADC0IOLoVolt == 0)
    {
        return ERR_PARA_ERR;
    }

    switch (pstADCInitHandle->u8Resolution)
    {
        case ADC_RESOLUTION_10BIT:
            u8Div = 2;
            break;
        case ADC_RESOLUTION_12BIT:
            u8Div = 0;
            break;
        default:
            break;
    }

    if ((ADC0 == pstADCInitHandle->pstADCHandle) && (ADC_CH_VDDR == enCh))
    {
        voltage = ADC_VDDR_DELTA_VAL * (u16ADCSampData - (m_stADCTuneValue.u16ADC0VddrHiVolt >> u8Div));
        voltage = voltage / ((m_stADCTuneValue.u16ADC0VddrHiVolt - m_stADCTuneValue.u16ADC0VddrLoVolt) >> u8Div);
        voltage = voltage + ADC_VDDR_STANDARD_3500MV_VAL;
    }
    else if ((ADC0 == pstADCInitHandle->pstADCHandle) && (ADC_CH_IO_MSK & enCh))
    {
        voltage = ADC_IO_DELTA_VAL * (u16ADCSampData - (m_stADCTuneValue.u16ADC0IOHiVolt >> u8Div));
        voltage = voltage / ((m_stADCTuneValue.u16ADC0IOHiVolt - m_stADCTuneValue.u16ADC0IOLoVolt) >> u8Div);
        voltage = voltage + ADC_IO_STANDARD_900MV_VAL;
    }
    else if ((ADC1 == pstADCInitHandle->pstADCHandle) && (ADC_CH_IO_MSK & enCh))
    {
        voltage = ADC_IO_DELTA_VAL * (u16ADCSampData - (m_stADCTuneValue.u16ADC1IOHiVolt >> u8Div));
        voltage = voltage / ((m_stADCTuneValue.u16ADC1IOHiVolt - m_stADCTuneValue.u16ADC1IOLoVolt) >> u8Div);
        voltage = voltage + ADC_IO_STANDARD_900MV_VAL;
    }
    else
    {
        return ERR_PARA_ERR;
    }

    *pu16Volt = (uint16_t)voltage;

    return ERR_STA_OK;
}

/**
 * @brief  Get adc value for one channel.
 * @param  pstADCInitHandle: ADC init handle, @ref stADCInitHandle_t.
 * @param  enCh: ADC channel.
 * @param  pu16Volt: Pointer to adc sample voltage, unit is mV.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T patch_hal_adc_get_voltage(stADCInitHandle_t *pstADCInitHandle, EN_ADC_CH_T enCh, uint16_t *pu16Volt)
{
    uint16_t     u16ADCVal = 0;
    EN_ERR_STA_T enRet     = 0;

    enRet = patch_hal_adc_get_data(pstADCInitHandle, enCh, &u16ADCVal);
    if (ERR_STA_OK == enRet)
    {
        enRet = patch_hal_adc_trans_voltage(pstADCInitHandle, enCh, u16ADCVal, pu16Volt);
    }

    return enRet;
}
