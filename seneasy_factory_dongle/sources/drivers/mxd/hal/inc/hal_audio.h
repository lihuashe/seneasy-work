/**********************************************************************************************************************
 * @file     hal_audio.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/01/23
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
 * Copyright (c) 2016~2020, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 
**********************************************************************************************************************/
#ifndef __HAL_AUDIO_H__
#define __HAL_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef enum
{
    AUDIO_PDM_SAMP_EDGE_RISE = 0x00,  /* _|- */
    AUDIO_PDM_SAMP_EDGE_FALL = 0x01,  /* -|_*/
    AUDIO_PDM_SAMP_EDGE_BOTH = 0x02,  /* _|--|_ */

} EN_AUDIO_PDM_SAMP_EDGE_T;

#define IS_AUDIO_PDM_SAMP_EDGE_VALID(_EDGE_)    (((_EDGE_) == AUDIO_PDM_SAMP_EDGE_FALL) || \
                                                 ((_EDGE_) == AUDIO_PDM_SAMP_EDGE_RISE) || \
                                                 ((_EDGE_) == AUDIO_PDM_SAMP_EDGE_BOTH))




/**
 * @brief  software dcr handler.
 * @param  p32PdmDcrBuf: status value.
 * @param  p32PdmDcrValue: dcr value.
 * @param  p16buf: audio data buffer, and the target data will save the same position.
 * @param  u16len: buf length.
 * @return status: @ref EN_ERR_STA_T.
 */
extern void rom_hal_audio_software_dcr_handler(int32_t *p32PdmDcrBuf, int32_t *p32PdmDcrValue,
                                               int16_t *p16buf, uint16_t u16len);


/**
 * @brief  audio software CIC algorithm, this function will convert audio data to 16K x 16bit PCM data.
 * @param  pu32DataIn: audio data buffer.
 * @param  pu16DataOut: after convert the resault save buffer.
 * @param  u16start: first data pos.
 * @param  u32len: min len is 256 / 2 = 128 WORD.
 * @return valid output data length.
 */
extern uint16_t rom_hal_audio_sw_cic_handler(uint32_t *pu32DataIn, uint16_t *pu16DataOut, uint16_t u16start, uint32_t u32len);


/**
 * @brief  udio adc mode deinit.
 * @param  pstAUDIO: Audio handle, Should be AUDIO0/AUDIO1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_audio_adc_deinit(stAUDIO_Handle_t *pstAUDIO);


/**
 * @brief  audio adc mode init.
 * @param  pstAUDIO: Audio handle, Should be AUDIO0/AUDIO1.
 * @param  enAudioSampleRate :range: 8k - 32K  @ref EN_AUDIO_SAMPLE_RATE_T , SLPF is enabled by default.
 * @note   if slpf on: 8K <= Sample rate <= 32K.
           if slpf off: 32K<= Sample rate <= 64K.
 * @param  enAudioInputMode: single or diff @ref EN_AUDIO_ADC_INPUT_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_audio_adc_init(stAUDIO_Handle_t *pstAUDIO, EN_AUDIO_SAMPLE_RATE_T enAudioSampleRate,
                                           EN_AUDIO_ADC_INPUT_MODE_T enAudioInputMode);


/**
 * @brief  audio pdm mode init.
 * @param  enAudioSampleRate :range: 8k - 32K  @ref EN_AUDIO_SAMPLE_RATE_T , SLPF is enabled by default.
 * @note   if slpf on: 8K <= Sample rate <= 32K.
           if slpf off: 32K<= Sample rate <= 64K.
 * @param  enPdmClockRate: 2Mhz or 1Mhz @ref EN_PDM_CLOCK_T.
 * @param  enPdmSampleEdge: rise or fall or both, pdm0 only support rise sample, pdm1 only support rise sample.
                            @ref EN_AUDIO_PDM_SAMP_EDGE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_audio_pdm_init(EN_AUDIO_SAMPLE_RATE_T enAudioSampleRate, EN_PDM_CLOCK_T enPdmClockRate,
                                           EN_AUDIO_PDM_SAMP_EDGE_T enPdmSampleEdge);


/**
 * @brief  Enable audio pdm.
 * @param  enPdmSampleEdge: rise or fall or both, pdm0 only support rise sample, pdm1 only support rise sample.
                            @ref EN_AUDIO_PDM_SAMP_EDGE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_audio_pdm_enable(EN_AUDIO_PDM_SAMP_EDGE_T enPdmSampleEdge);


/**
 * @brief  Disable audio pdm.
 * @param  enPdmSampleEdge: rise or fall or both, pdm0 only support rise sample, pdm1 only support rise sample.
                            @ref EN_AUDIO_PDM_SAMP_EDGE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_audio_pdm_disable(EN_AUDIO_PDM_SAMP_EDGE_T enPdmSampleEdge);


#ifdef __cplusplus
}
#endif

#endif /* __HAL_AUDIO_H__ */

