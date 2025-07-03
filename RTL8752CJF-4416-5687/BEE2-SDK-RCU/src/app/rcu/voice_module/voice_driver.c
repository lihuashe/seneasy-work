#include <string.h>
#include "board.h"
#include <trace.h>
#include "voice_driver.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "rtl876x_codec.h"
#include "rtl876x_i2s.h"
#include "swtimer.h"
#include "os_timer.h"
#include "app_msg.h"
#include <app_task.h>
#include <app_section.h>

#if RCU_VOICE_EN

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
#if SUPPORT_CODEC_EQ_CONFIG_FEATURE
static CODEC_EQTypeDef *const CODEC_EQ_TABLE[CODEC_EQ_MAX_NUM] =
{CODEC_EQ1, CODEC_EQ2, CODEC_EQ3, CODEC_EQ4, CODEC_EQ5};

static const uint32_t CODEC_EQ_DEFAULT_PARAMS[CODEC_EQ_MAX_NUM][6] =
{
    /* CODEC_EQChCmd,   CODEC_EQCoefH0, CODEC_EQCoefB1, CODEC_EQCoefB2, CODEC_EQCoefA1, CODEC_EQCoefA2 */
    {EQ_CH_Cmd_DISABLE, 0x00,           0x00,           0x00,           0x00,           0x00},  /* CODEC_EQ1 */
    {EQ_CH_Cmd_DISABLE, 0x00,           0x00,           0x00,           0x00,           0x00},  /* CODEC_EQ2 */
    {EQ_CH_Cmd_DISABLE, 0x00,           0x00,           0x00,           0x00,           0x00},  /* CODEC_EQ3 */
    {EQ_CH_Cmd_DISABLE, 0x00,           0x00,           0x00,           0x00,           0x00},  /* CODEC_EQ4 */
    {EQ_CH_Cmd_DISABLE, 0x00,           0x00,           0x00,           0x00,           0x00},  /* CODEC_EQ5 */
};
#endif

/*============================================================================*
 *                              External Variables
 *============================================================================*/

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
T_VOICE_DRIVER_GLOBAL_DATA voice_driver_global_data;
T_VOICE_DRIVER_CODEC_PARAMS voice_driver_codec_params;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
static void voice_driver_init_pad_and_pinmux(void)
{
#if (VOICE_MIC_TYPE == AMIC_TYPE)
    Pad_Config(H_0, PAD_SW_MODE, PAD_NOT_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(AMIC_PIN1, PAD_SW_MODE, PAD_NOT_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(AMIC_PIN2, PAD_SW_MODE, PAD_NOT_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
#elif (VOICE_MIC_TYPE == DMIC_TYPE)
    Pad_Config(DMIC_MSBC_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(DMIC_MSBC_DAT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pinmux_Deinit(DMIC_MSBC_CLK);
    Pinmux_Deinit(DMIC_MSBC_DAT);
    Pinmux_Config(DMIC_MSBC_CLK, DMIC1_CLK);
    Pinmux_Config(DMIC_MSBC_DAT, DMIC1_DAT);
#endif
}

/**
  * @brief  Initialize I2S peripheral.
  * @param   No parameter.
  * @return  void
  */
static void voice_driver_init_i2s(void)
{
    RCC_PeriphClockCmd(APBPeriph_I2S0, APBPeriph_I2S0_CLOCK, ENABLE);

    I2S_InitTypeDef I2S_InitStruct;
    I2S_StructInit(&I2S_InitStruct);
    I2S_InitStruct.I2S_ClockSource      = I2S_CLK_40M;
    if (voice_driver_codec_params.codec_sample_rate == SAMPLE_RATE_8KHz)
    {
        I2S_InitStruct.I2S_BClockMi         = 0x271;/* <!BCLK = 8K */
        I2S_InitStruct.I2S_BClockNi         = 0x08;
    }
    else if (voice_driver_codec_params.codec_sample_rate == SAMPLE_RATE_16KHz)
    {
        I2S_InitStruct.I2S_BClockMi         = 0x271;/* <!BCLK = 16K */
        I2S_InitStruct.I2S_BClockNi         = 0x10;
    }
    I2S_InitStruct.I2S_DeviceMode       = I2S_DeviceMode_Master;
    I2S_InitStruct.I2S_ChannelType      = I2S_Channel_Mono;
    I2S_InitStruct.I2S_DataFormat       = I2S_Mode;
    I2S_InitStruct.I2S_RxWaterlevel       = 0x4;
    I2S_InitStruct.I2S_TxWaterlevel       = 0x8;
    I2S_Init(I2S0, &I2S_InitStruct);

    I2S_Cmd(I2S0, I2S_MODE_TX | I2S_MODE_RX, ENABLE);
}

/**
  * @brief  Deinitialize I2S peripheral.
  * @param   No parameter.
  * @return  void
  */
static void voice_driver_deinit_i2s(void)
{
    I2S_DeInit(I2S0);
}

/**
  * @brief  Initialize codec peripheral.
  * @param   No parameter.
  * @return  void
  */
static void voice_driver_init_codec(void)
{
    RCC_PeriphClockCmd(APBPeriph_CODEC, APBPeriph_CODEC_CLOCK, ENABLE);

    CODEC_InitTypeDef CODEC_InitStruct;
    CODEC_StructInit(&CODEC_InitStruct);

    /* set codec common parameters */
    CODEC_InitStruct.CODEC_SampleRate = voice_driver_codec_params.codec_sample_rate;
    CODEC_InitStruct.CODEC_MicType = voice_driver_codec_params.mic_type;

    if (voice_driver_codec_params.mic_type == CODEC_AMIC)
    {
        CODEC_InitStruct.CODEC_MicBIAS = voice_driver_codec_params.amic_bias_voltage;
        CODEC_InitStruct.CODEC_MICBstMode = voice_driver_codec_params.amic_input_mode;
        CODEC_InitStruct.CODEC_MICBstGain = voice_driver_codec_params.amic_analog_gain;
        CODEC_InitStruct.CODEC_AdGain = voice_driver_codec_params.amic_digital_gain;
    }
    else if (voice_driver_codec_params.mic_type == CODEC_DMIC)
    {
        CODEC_InitStruct.CODEC_DmicClock = voice_driver_codec_params.dmic_clock;
        CODEC_InitStruct.CODEC_DmicDataLatch = voice_driver_codec_params.dmic_data_latch;
        CODEC_InitStruct.CODEC_BoostGain = voice_driver_codec_params.dmic_boost_gain;
    }

    CODEC_InitStruct.CODEC_I2SFormat        = CODEC_I2S_DataFormat_I2S;
    CODEC_InitStruct.CODEC_I2SDataWidth     = CODEC_I2S_DataWidth_16Bits;
    CODEC_Init(CODEC, &CODEC_InitStruct);

#if SUPPORT_CODEC_EQ_CONFIG_FEATURE
    /* init codec EQ parameters */
    for (uint8_t eq_index = 0; eq_index < CODEC_EQ_MAX_NUM; eq_index++)
    {
        CODEC_EQInitTypeDef codec_eq_params;
        CODEC_EQStructInit(&codec_eq_params);
        if (voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQChCmd == EQ_CH_Cmd_ENABLE)
        {
            codec_eq_params.CODEC_EQCoefH0 = voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefH0;
            codec_eq_params.CODEC_EQCoefB1 = voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefB1;
            codec_eq_params.CODEC_EQCoefB2 = voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefB2;
            codec_eq_params.CODEC_EQCoefA1 = voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefA1;
            codec_eq_params.CODEC_EQCoefA2 = voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefA2;
        }
        CODEC_EQInit(CODEC_EQ_TABLE[eq_index], &codec_eq_params);
    }
#endif
}

/**
  * @brief  Deinitialize codec peripheral.
  * @param   No parameter.
  * @return  void
  */
static void voice_driver_deinit_codec(void)
{
    CODEC_DeInit(CODEC);
}

/**
  * @brief  Initialize GDMA peripheral.
  * @param   No parameter.
  * @return  void
  */
static void voice_driver_init_rx_gdma(void)
{
    /* Enable GDMA clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_Channel_RX_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize          = VOICE_GDMA_FRAME_SIZE / 4;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_16;
    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&(I2S0->RX_DR));
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)voice_driver_global_data.gdma_buffer.buf0;
    GDMA_InitStruct.GDMA_SourceHandshake     = GDMA_Handshake_SPORT0_RX;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC_RX;
    GDMA_Init(GDMA_Channel_RX, &GDMA_InitStruct);
    GDMA_INTConfig(GDMA_Channel_RX_NUM, GDMA_INT_Transfer, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GDMA0_Channel_RX_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    GDMA_Cmd(GDMA_Channel_RX_NUM, ENABLE);
}

/**
  * @brief  Deinitialize GDMA peripheral.
  * @param   No parameter.
  * @return  void
  */
static void voice_driver_deinit_rx_gdma(void)
{
    GDMA_Cmd(GDMA_Channel_RX_NUM, DISABLE);
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/**
* @brief  Initialize voice driver global data
*/
void voice_driver_init_data(void)
{
    APP_PRINT_INFO0("[voice_driver_init_data] init data");
    memset(&voice_driver_global_data, 0, sizeof(voice_driver_global_data));
    voice_driver_global_data.is_allowed_to_enter_dlps = true;
}

/**
* @brief  Initialize codec parameters
*/
void voice_driver_init_codec_params(void)
{
    APP_PRINT_INFO0("[voice_driver_init_codec_params] init codec parameters");

    memset(&voice_driver_codec_params, 0, sizeof(voice_driver_codec_params));

    /* init codec common parameters */
#if (CODEC_SAMPLE_RATE_SEL == CODEC_SAMPLE_RATE_16K)
    voice_driver_codec_params.codec_sample_rate = SAMPLE_RATE_16KHz;
#elif (CODEC_SAMPLE_RATE_SEL == CODEC_SAMPLE_RATE_8K)
    voice_driver_codec_params.codec_sample_rate = SAMPLE_RATE_8KHz;
#endif

#if (VOICE_MIC_TYPE == AMIC_TYPE)
    /* init amic paramstes */
    voice_driver_codec_params.mic_type = CODEC_AMIC;
#if (AMIC_INPUT_TYPE_SEL == AMIC_INPUT_TYPE_DIFF)
    voice_driver_codec_params.amic_input_mode = MICBST_Mode_Differential;
#elif (AMIC_INPUT_TYPE_SEL == AMIC_INPUT_TYPE_SINGLE)
    voice_driver_codec_params.amic_input_mode = MICBST_Mode_Single;
#endif
    voice_driver_codec_params.amic_bias_voltage = MICBIAS_VOLTAGE_1_8;
    voice_driver_codec_params.amic_analog_gain = MICBST_Gain_30dB;
    voice_driver_codec_params.amic_digital_gain =
        0x2F;  /* 0x2F - 0dB, step 0.375dB, range -17.625dB ~ 30dB */

#elif (VOICE_MIC_TYPE == DMIC_TYPE)
    /* init dmic paramstes */
    voice_driver_codec_params.mic_type = CODEC_DMIC;
    voice_driver_codec_params.dmic_clock = DMIC_Clock_2MHz;
    voice_driver_codec_params.dmic_data_latch = DMIC_Rising_Latch;
    voice_driver_codec_params.dmic_boost_gain = Boost_Gain_0dB;
#endif

#if SUPPORT_CODEC_EQ_CONFIG_FEATURE
    /* init codec EQ parameters */
    for (uint8_t eq_index = 0; eq_index < CODEC_EQ_MAX_NUM; eq_index++)
    {
        voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQChCmd  =
            CODEC_EQ_DEFAULT_PARAMS[eq_index][0];
        voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefH0 =
            CODEC_EQ_DEFAULT_PARAMS[eq_index][1];
        voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefB1 =
            CODEC_EQ_DEFAULT_PARAMS[eq_index][2];
        voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefB2 =
            CODEC_EQ_DEFAULT_PARAMS[eq_index][3];
        voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefA1 =
            CODEC_EQ_DEFAULT_PARAMS[eq_index][4];
        voice_driver_codec_params.codec_eq_params[eq_index].CODEC_EQCoefA2 =
            CODEC_EQ_DEFAULT_PARAMS[eq_index][5];
    }
#endif
}

/**
* @brief  Initialize voice driver.
* @param   No parameter.
* @return  void
*/
void voice_driver_init(void)
{
    voice_driver_init_data();
    voice_driver_global_data.is_allowed_to_enter_dlps = false;
    voice_driver_global_data.is_voice_driver_working = true;

    voice_driver_init_pad_and_pinmux();
    voice_driver_init_i2s();
    voice_driver_init_codec();
    voice_driver_init_rx_gdma();
}

/**
* @brief  Deinitialize voice driver.
* @param   No parameter.
* @return  void
*/
void voice_driver_deinit(void)
{
    voice_driver_global_data.is_allowed_to_enter_dlps = true;
    voice_driver_global_data.is_voice_driver_working = false;

    voice_driver_deinit_i2s();
    voice_driver_deinit_codec();
    voice_driver_deinit_rx_gdma();
}

/**
* @brief GDMA interrupt handler function.
* @param   No parameter.
* @return  void
*/
void GDMA0_Channel_RX_Handler(void) DATA_RAM_FUNCTION;
void GDMA0_Channel_RX_Handler(void)
{
    GDMA_ClearINTPendingBit(GDMA_Channel_RX_NUM, GDMA_INT_Transfer);
    if (voice_driver_global_data.is_voice_driver_working == true)
    {
        T_IO_MSG gdma_msg;

        APP_PRINT_INFO0("[GDMA0_Channel_RX_Handler] GDMA interrupt!");
        gdma_msg.type = IO_MSG_TYPE_GDMA;
        gdma_msg.subtype = 0;
        if (voice_driver_global_data.current_bibuff_index == 0)
        {
            gdma_msg.u.buf = (void *)voice_driver_global_data.gdma_buffer.buf0;
        }
        else
        {
            gdma_msg.u.buf = (void *)voice_driver_global_data.gdma_buffer.buf1;
        }
        app_send_msg_to_apptask(&gdma_msg);
        GDMA_SetSourceAddress(GDMA_Channel_RX, (uint32_t)(&(I2S0->RX_DR)));
        if (voice_driver_global_data.current_bibuff_index == 0)
        {
            voice_driver_global_data.current_bibuff_index = 1;
            GDMA_SetDestinationAddress(GDMA_Channel_RX, (uint32_t)voice_driver_global_data.gdma_buffer.buf1);
        }
        else
        {
            voice_driver_global_data.current_bibuff_index = 0;
            GDMA_SetDestinationAddress(GDMA_Channel_RX, (uint32_t)voice_driver_global_data.gdma_buffer.buf0);
        }
        GDMA_SetBufferSize(GDMA_Channel_RX, VOICE_GDMA_FRAME_SIZE / 4);
        GDMA_Cmd(GDMA_Channel_RX_NUM, ENABLE);
    }
}

#endif


