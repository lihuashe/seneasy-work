/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_codec.c
* @brief    This file provides all the CODEC firmware functions.
* @details
* @author   Yuan
* @date     2020-11-16
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_codec.h"
//#include "rtl876x_pinmux.h"
#include "platform_utils.h"

/**
  * @brief  Initialize the CODEC analog registers in AON area.
  * @param  None.
  * @retval None
  */
void CODEC_AnalogCircuitInit(void)
{
    uint16_t reg_value = 0;

    /* Enable ADC 1V1 power cut */
    reg_value = btaon_fast_read_safe(0x110);
    btaon_fast_write_safe(0x110, reg_value | 0x01);
}

/**
  * @brief  Deinitialize the CODEC analog registers in AON area.
  * @param  None.
  * @retval None
  */
void CODEC_AnalogCircuitDeInit(void)
{
    uint16_t reg_value = 0;

    /* Disable 1V1 power cut */
    reg_value = btaon_fast_read_safe(0x110);
    btaon_fast_write_safe(0x110, reg_value & (~0x01));
}

/**
  * @brief  Deinitializes the CODEC peripheral registers to their default reset values(turn off CODEC clock).
  * @param  CODECx: selected CODEC peripheral.
  * @retval None
  */
void CODEC_DeInit(CODEC_TypeDef *CODECx)
{
    /* Check the parameters */
    assert_param(IS_CODEC_PERIPH(CODECx));

    CODECx->ADC0_CTRL1 &= ~0x7F;

    platform_delay_ms(30);

    CODECx->ADC0_CTRL0 |= CODEC_ADC_CH0_AD_MUTE_MSK;

    CODECx->CLK_CR1 &= ~0x3FFF;

    CODECx->I2S_CTRL &= ~BIT0;

    CODECx->CR0 &= ~BIT0;

    if (CODEC->ADC0_CTRL0 & CODEC_AMIC_DMIC_CH0_SEL_MSK)
    {
        CODEC_ANA->ANA_CR0 &= CODEC_ADC_ANA_POW_CLR;
        CODEC_ANA->ANA_CR1 &= ~0x3000000;
        CODEC_ANA->ANA_CR1 &= ~BIT15;
        CODEC_ANA->ANA_CR1 &= ~BIT10;
        CODECx->ANA_CR1 &= ~BIT3;
        CODEC_ANA->ANA_CR0 &= CODEC_DAC_ADDACK_POW_CLR;
    }
    else
    {
        CODEC_ANA->ANA_CR1 &= ~BIT15;
        CODEC_ANA->ANA_CR1 &= ~BIT10;
        CODECx->ANA_CR1 &= ~BIT3;
        CODECx->CLK_CR2 &= ~BIT3;
    }

    RCC_PeriphClockCmd(APBPeriph_CODEC, APBPeriph_CODEC_CLOCK, DISABLE);
    CODEC_AnalogCircuitDeInit();
}

/**
  * @brief Initializes the CODEC peripheral according to the specified
  *   parameters in the CODEC_InitStruct
  * @param  CODECx: selected CODEC peripheral.
  * @param  CODEC_InitStruct: pointer to a CODEC_InitTypeDef structure that
  *   contains the configuration information for the specified CODEC peripheral
  * @retval None
  */
void CODEC_Init(CODEC_TypeDef *CODECx, CODEC_InitTypeDef *CODEC_InitStruct)
{
    /* Check the parameters */
    /* MIC initialization parameters for input */
    assert_param(IS_CODEC_PERIPH(CODECx));
    assert_param(IS_SAMPLE_RATE(CODEC_InitStruct->CODEC_SampleRate));
    assert_param(IS_CODEC_I2S_DATA_FORMAT(CODEC_InitStruct->CODEC_I2SFormat));
    assert_param(IS_CODEC_I2S_DATA_WIDTH(CODEC_InitStruct->CODEC_I2SDataWidth));
    assert_param(IS_MICBIAS_CONFIG(CODEC_InitStruct->CODEC_MicBIAS));
    assert_param(IS_MICBST_MODE(CODEC_InitStruct->CODEC_MicBstMode));
    assert_param(IS_MICBST_GAIN(CODEC_InitStruct->CODEC_MicBstGain));
    assert_param(IS_CODEC_DMIC_CLOCK(CODEC_InitStruct->CODEC_DmicClock));
    /* MIC channel 0 initialization parameters */
    assert_param(IS_CODEC_CH0_MIC_MUTE(CODEC_InitStruct->CODEC_Ch0Mute));
    assert_param(IS_CODEC_CH0_MIC_TYPE(CODEC_InitStruct->CODEC_Ch0MicType));
    assert_param(IS_DMIC_CH0_LATCH_EDGE(CODEC_InitStruct->CODEC_Ch0DmicDataLatch));
    assert_param(IS_AD_GAIN(CODEC_InitStruct->CODEC_Ch0AdGain));
    assert_param(IS_CH0_BOOST_GAIN(CODEC_InitStruct->CODEC_Ch0BoostGain));
    assert_param(IS_CH0_ADC_ZERO_DET_TIMEOUT(CODEC_InitStruct->CODEC_Ch0ZeroDetTimeout));

    uint16_t reg_value = 0;

    /* Enable ADC 1V8 LDO current limit */
    reg_value = btaon_fast_read_safe(0x110);
    btaon_fast_write_safe(0x110, reg_value | BIT3);

    /* Smaller LDO BW for LDO PSRR improvement */
    reg_value = btaon_fast_read_safe(0x112);
    btaon_fast_write_safe(0x112, reg_value & (~0x6000));

    /* Enable ADC 1V8 LDO */
    reg_value = btaon_fast_read_safe(0x110);
    btaon_fast_write_safe(0x110, reg_value | BIT2);

    /* Delay 15ms */
    platform_delay_us(15000);

    /* Power on Vref */
    CODEC_ANA->ANA_CR1 |= CODEC_MICBST_VREF_POW_MSK;

    /* MICBIAS voltage selection (default 1.8V) */
    CODEC_ANA->ANA_CR1 &= CODEC_MICBIAS_VSET_CLR;
//    CODEC_ANA->ANA_CR1 |= (3 << CODEC_MICBIAS_VSET_POS);
    CODEC_ANA->ANA_CR1 |= CODEC_InitStruct->CODEC_MicBIAS;

    /* Enable MICBIAS current limit & OCP */
    CODEC_ANA->ANA_CR2 |= CODEC_MICBIAS_LIMIT_MSK;

    /* Delay 1.5 ms */
    platform_delay_us(1500);

    /* Power on MICBIAS & chopper */
    CODEC_ANA->ANA_CR1 |= CODEC_MICBIAS_POW_MSK;
    CODEC_ANA->ANA_CR1 |= CODEC_MICBIAS_ENCHX_MSK;

    /* Enable ckx_micbias 312.5KHz */
    CODECx->ANA_CR1 |= CODEC_CKX_MICBIAS_EN_MSK;

    /* Delay 5 ms */
    platform_delay_ms(5);

    /* Disable MICBIAS current limit */
    CODEC_ANA->ANA_CR2 &= CODEC_MICBIAS_LIMIT_CLR;

    /* Disable ADC 1V8 LDO current limit */
    reg_value = btaon_fast_read_safe(0x110);
    btaon_fast_write_safe(0x110, reg_value & (~(BIT3)));

    /* Enable Vref RC bypass mode */
    reg_value = btaon_fast_read_safe(0x110);
    btaon_fast_write_safe(0x110, reg_value | (BIT5));

    /* Enable audio IP*/
    CODECx->CR0 |= CODEC_AUDIO_IP_EN_MSK;

    /* Sample Rate selection. */
    CODECx->CLK_CR3 &= CODEC_SAMPLE_RATE_CLR;
//    CODECx->CLK_CR3 |= 0x5;
    CODECx->CLK_CR3 |= CODEC_InitStruct->CODEC_SampleRate;

//    /* I2S rx data length select to 16bits */
//    CODECx->I2S_CTRL &= CODEC_I2S_RX_DATA_LEN_CLR;

//    /* I2S rx data format select to I2S */
//    CODECx->I2S_CTRL &= CODEC_I2S_RX_DATA_FORMAT_CLR;

//    /* Enable digital codec */
//    CODECx->I2S_CTRL |= CODEC_AUDIO_RST_N_MSK;

    /* Configure I2S parameters */
    CODECx->I2S_CTRL = CODEC_InitStruct->CODEC_I2SChSequence | \
                       CODEC_InitStruct->CODEC_I2SDataWidth  | \
                       CODEC_InitStruct->CODEC_I2SFormat     | \
                       CODEC_AUDIO_RST_N_MSK;

    if (CODEC_InitStruct->CODEC_Ch0MicType == CODEC_CH0_AMIC)
    {
        /* Enable AD/DA clock and ADC analog power */
        CODEC_ANA->ANA_CR0 = CODEC_DAC_ADDACK_POW_MSK   | \
                             CODEC_ADC_ANA_POW_MSK      | \
                             CODEC_DTSDM_CLK_EN_MSK;

        CODEC_ANA->ANA_CR1 &= ~0x3000000;
        CODEC_ANA->ANA_CR1 |= CODEC_MICBST_POW_MSK;

        CODEC_ANA->ANA_CR1 &= ~0x30000;
        CODEC_ANA->ANA_CR1 |= CODEC_InitStruct->CODEC_MicBstGain;

        CODEC_ANA->ANA_CR1 |= CODEC_MICBST_MUTE_MIC_MSK;

        if (CODEC_InitStruct->CODEC_MicBstMode == MICBST_Mode_Single)
        {
            CODEC_ANA->ANA_CR1 &= CODEC_MICBST_ENDFL_CLR;
            reg_value = btaon_fast_read_safe(0x110);
            btaon_fast_write_safe(0x110, reg_value | (BIT4));
        }
        else if (CODEC_InitStruct->CODEC_MicBstMode == MICBST_Mode_Differential)
        {
            CODEC_ANA->ANA_CR1 |= CODEC_MICBST_ENDFL_MSK;
            reg_value = btaon_fast_read_safe(0x110);
            btaon_fast_write_safe(0x110, reg_value & (~BIT4));
        }
        CODECx->ADC0_CTRL0 |= CODEC_AMIC_DMIC_CH0_SEL_MSK;
//        CODECx->ADC0_CTRL0 |= CODEC_ADC_CH0_AD_MUTE_MSK;
    }
    else if (CODEC_InitStruct->CODEC_Ch0MicType == CODEC_CH0_DMIC)
    {
        CODECx->CLK_CR2 = CODEC_DMIC1_CLK_EN_MSK | \
                          CODEC_InitStruct->CODEC_DmicClock;
        CODECx->ADC0_CTRL0 &= CODEC_AMIC_DMIC_CH0_SEL_CLR;
        CODECx->ADC0_CTRL0 &= CODEC_ADC0_DMIC_SRC_SEL_CLR;
        CODECx->ADC0_CTRL0 |= CODEC_InitStruct->CODEC_Ch0DmicDataLatch;
//        CODECx->ADC0_CTRL0 |= CODEC_ADC_CH0_AD_MUTE_MSK;
    }
    CODECx->ADC0_CTRL0 |= CODEC_ADC_CH0_AD_MUTE_MSK;

    /* AD LPF clock 10M(default 5M) */
    CODECx->CLK_CR3 &= CODEC_AD_LPF_CLK_SEL_CLR;

    /* ADC HPF FC sel */
    CODECx->ADC0_CTRL0 &= CODEC_ADC0_DCHPF_FC_SEL_CLR;

    /* Enable ADC0 HPF */
    CODECx->ADC0_CTRL0 |= CODEC_ADC0_DCHPF_EN_MSK;

    /* Configuer mic channel 0 parameters */
    CODECx->ADC0_CTRL0 &= CODEC_ADC_CH0_BOOST_GAIN_CLR;
    CODECx->ADC0_CTRL0 &= CODEC_ADC_CH0_ZDET_TOUT_CLR;
    CODECx->ADC0_CTRL0 |= CODEC_InitStruct->CODEC_Ch0BoostGain | \
                          CODEC_InitStruct->CODEC_Ch0ZeroDetTimeout;
    CODECx->ADC0_CTRL1 &= ~0x7F;
    CODECx->ADC0_CTRL1 |= CODEC_InitStruct->CODEC_Ch0AdGain;

    /* Enable codec clock control */
    CODECx->CLK_CR1 &= ~0x3FFF;
//    CODECx->CLK_CR1 = 0x1C40;

    /* Clock configuration */
    CODECx->CLK_CR1 = CODEC_AD_ANA_CLK_EN_MSK | \
                      CODEC_AD_FIFO_CLK_EN_MSK;
    if (CODEC_InitStruct->CODEC_Ch0Mute == CODEC_CH0_UNMUTE)
    {
        CODECx->CLK_CR1 |= CODEC_AD_FILTER_CH0_CLK_MSK | \
                           CODEC_AD_CH0_CLK_MSK;
        platform_delay_ms(60);
        CODECx->ADC0_CTRL0 &= CODEC_ADC_CH0_AD_MUTE_CLR;
    }
}

/**
  * @brief  Fills each CODEC_InitStruct member with its default value.
  * @param  CODEC_InitStruct: pointer to an CODEC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void CODEC_StructInit(CODEC_InitTypeDef *CODEC_InitStruct)
{
    /* Basic parameters section */
    CODEC_InitStruct->CODEC_SampleRate          = SAMPLE_RATE_16KHz;
    CODEC_InitStruct->CODEC_I2SFormat           = CODEC_I2S_DataFormat_I2S;
    CODEC_InitStruct->CODEC_I2SDataWidth        = CODEC_I2S_DataWidth_16Bits;
    CODEC_InitStruct->CODEC_I2SChSequence       = CODEC_I2S_CH_L_L;
    CODEC_InitStruct->CODEC_MicBIAS             = MICBIAS_VOLTAGE_1_8;
    CODEC_InitStruct->CODEC_MicBstMode          = MICBST_Mode_Single;
    CODEC_InitStruct->CODEC_MicBstGain          = MICBST_Gain_0dB;
    CODEC_InitStruct->CODEC_DmicClock           = DMIC_Clock_2500KHz;
    /* MIC channel 0 initialization parameters section */
    CODEC_InitStruct->CODEC_Ch0MicType          = CODEC_CH0_AMIC;
    CODEC_InitStruct->CODEC_Ch0BoostGain        = Ch0_Boost_Gain_0dB;
    CODEC_InitStruct->CODEC_Ch0Mute             = CODEC_CH0_UNMUTE;
    CODEC_InitStruct->CODEC_Ch0ZeroDetTimeout   = Ch0_ADC_Zero_DetTimeout_1024_32_Sample;
    CODEC_InitStruct->CODEC_Ch0DmicDataLatch    = DMIC_Ch0_Rising_Latch;
    CODEC_InitStruct->CODEC_Ch0AdGain           = 0x2F;
}

/**
  * @brief  Enable or disable mic_bias output.
  * @param  CODECx: selected CODEC peripheral.
  * @param  NewState: new state of MICBIAS.
  *   This parameter can be: ENABLE or DISABLE.
  * @return none.
  */
void CODEC_MICBIASCmd(CODEC_TypeDef *CODECx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_CODEC_PERIPH(CODECx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        if (!(CODEC_ANA->ANA_CR1 & CODEC_MICBIAS_POW_MSK))
        {
            /* Analog initialization in AON register */
            CODEC_AnalogCircuitInit();

            /* MICBIAS power on */
            CODEC_ANA->ANA_CR1 |= CODEC_MICBIAS_ENCHX_MSK       | \
                                  CODEC_MICBST_VREF_POW_MSK     | \
                                  CODEC_MICBIAS_POW_MSK;

            /* Clock enable */
            CODECx->ANA_CR1 = CODEC_CKX_MICBIAS_EN_MSK;
        }
    }
    else
    {
        CODEC_ANA->ANA_CR1 &= CODEC_MICBIAS_POW_CLR;
    }
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

