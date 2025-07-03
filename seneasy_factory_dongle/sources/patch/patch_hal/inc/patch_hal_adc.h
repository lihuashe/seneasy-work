/**
 * @file     patch_hal_adc.h
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

#ifndef __PATCH_HAL_ADC_H__
#define __PATCH_HAL_ADC_H__

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
extern EN_ERR_STA_T patch_hal_adc_init(stADCInitHandle_t *pstADCInitHandle, uint16_t u16SampNum);

/**
 * @brief  Transform adc sample data to voltage.
 * @param  pstADCInitHandle: ADC init handle, @ref stADCInitHandle_t.
 * @param  enCh: ADC Channel, @ref EN_ADC_CH_T.
 * @param  u16ADCSampData: ADC sample data.
 * @param  pu16Volt: ADC voltage, unit is mV.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T patch_hal_adc_trans_voltage(stADCInitHandle_t *pstADCInitHandle, EN_ADC_CH_T enCh, uint16_t u16ADCSampData,
                                                uint16_t *pu16Volt);

/**
 * @brief  Get adc value for one channel.
 * @param  pstADCInitHandle: ADC init handle, @ref stADCInitHandle_t.
 * @param  enCh: ADC channel.
 * @param  pu16Volt: Pointer to adc sample voltage, unit is mV.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T patch_hal_adc_get_voltage(stADCInitHandle_t *pstADCInitHandle, EN_ADC_CH_T enCh, uint16_t *pu16Volt);

#endif /* __PATCH_HAL_ADC_H__ */
