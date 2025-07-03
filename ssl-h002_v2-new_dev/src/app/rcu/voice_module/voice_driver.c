/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     voice_driver.c
* @brief    voice module driver.
* @details
* @author
* @date     2020-03-05
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
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
#include "app_msg.h"
#include <app_task.h>
#include <app_section.h>
#include "rtl876x_uart.h"

#if SUPPORT_VOICE_FEATURE

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
/* LLI structure for I2S data */
static GDMA_LLIDef voice_driver_gdma_link_list[2];

/*============================================================================*
 *                              External Variables
 *============================================================================*/

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
T_VOICE_DRIVER_GLOBAL_DATA voice_driver_global_data;
T_VOICE_DRIVER_CODEC_PARAMS voice_driver_codec_params;

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void voice_driver_init_pad_and_pinmux(void);
static void voice_driver_init_i2s(void);
static void voice_driver_deinit_i2s(void);
static void voice_driver_init_codec(void);
static void voice_driver_deinit_codec(void);
static void voice_driver_init_rx_gdma(void);
static void voice_driver_deinit_rx_gdma(void);

void VOICE_GDMA_Channel_Handler(void) DATA_RAM_FUNCTION;
bool voice_driver_dlps_check(void) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief   initialization of pinmux settings and pad settings.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_init_pad_and_pinmux(void)
{
#if (VOICE_MIC_TYPE == AMIC_TYPE)
    Pad_Config(AMIC_MIC_N_PIN, PAD_SW_MODE, PAD_NOT_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(AMIC_MIC_P_PIN, PAD_SW_MODE, PAD_NOT_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
#elif (VOICE_MIC_TYPE == DMIC_TYPE)
    Pad_Config(DMIC_CLK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(DMIC_DATA_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);

    Pinmux_Deinit(DMIC_CLK_PIN);
    Pinmux_Deinit(DMIC_DATA_PIN);
    Pinmux_Config(DMIC_CLK_PIN, DMIC1_CLK);
    Pinmux_Config(DMIC_DATA_PIN, DMIC1_DAT);
#endif
#if SUPPORT_MIC_BIAS_OUTPUT
    Pad_Config(H_0, PAD_SW_MODE, PAD_NOT_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
#endif
}

/******************************************************************
 * @brief   Initialize I2S peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_init_i2s(void)
{
    RCC_PeriphClockCmd(APBPeriph_I2S0, APBPeriph_I2S0_CLOCK, ENABLE);

    I2S_InitTypeDef I2S_InitStruct;
    I2S_StructInit(&I2S_InitStruct);
    I2S_InitStruct.I2S_ClockSource      = I2S_CLK_40M;
    I2S_InitStruct.I2S_DataFormat       = I2S_Mode;
    I2S_InitStruct.I2S_DeviceMode       = I2S_DeviceMode_Master;
    I2S_InitStruct.I2S_RxWaterlevel     = 0x4;

    /* config I2S clock speed */
    /* BCLK = 40MHz*(I2S_BClockNi/I2S_BClockMi) = Sample Rate * 64BCLK/sample */
    if (voice_driver_codec_params.codec_sample_rate == SAMPLE_RATE_8KHz)
    {
        I2S_InitStruct.I2S_BClockMi         = 0x186A;
        I2S_InitStruct.I2S_BClockNi         = 0x50;
    }
    else if (voice_driver_codec_params.codec_sample_rate == SAMPLE_RATE_16KHz)
    {
        I2S_InitStruct.I2S_BClockMi         = 0x186A;
        I2S_InitStruct.I2S_BClockNi         = 0xA0;
    }
    else
    {
        APP_PRINT_WARN1("[voice_driver_init_i2s] invalid codec sample rate: %d",
                        voice_driver_codec_params.codec_sample_rate);
        I2S_InitStruct.I2S_BClockMi         = 0x186A;
        I2S_InitStruct.I2S_BClockNi         = 0xA0;
    }

    /* config I2S channel type */
    I2S_InitStruct.I2S_ChannelType      = I2S_Channel_Mono;

    I2S_Init(I2S0, &I2S_InitStruct);
    I2S_Cmd(I2S0, I2S_MODE_RX, ENABLE);
}

/******************************************************************
 * @brief   Deinitialize I2S peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_deinit_i2s(void)
{
    I2S_DeInit(I2S0);
}

/******************************************************************
 * @brief   Initialize codec peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_init_codec(void)
{
    /* switch power mode */
    SystemCall(3, 1);

    CODEC_AnalogCircuitInit();

    RCC_PeriphClockCmd(APBPeriph_CODEC, APBPeriph_CODEC_CLOCK, ENABLE);

    CODEC_InitTypeDef CODEC_InitStruct;
    CODEC_StructInit(&CODEC_InitStruct);

    /* Basic parameters section */
    CODEC_InitStruct.CODEC_SampleRate = voice_driver_codec_params.codec_sample_rate;
    CODEC_InitStruct.CODEC_DmicClock = voice_driver_codec_params.dmic_clock;
    CODEC_InitStruct.CODEC_I2SFormat = voice_driver_codec_params.codec_i2s_format;
    CODEC_InitStruct.CODEC_I2SDataWidth = voice_driver_codec_params.codec_i2s_data_width;
    CODEC_InitStruct.CODEC_I2SChSequence = voice_driver_codec_params.codec_i2s_ch_sequence;
    CODEC_InitStruct.CODEC_MicBIAS = voice_driver_codec_params.mic_bias_voltage;
    CODEC_InitStruct.CODEC_MicBstGain = voice_driver_codec_params.amic_bst_gain;
    CODEC_InitStruct.CODEC_MicBstMode = voice_driver_codec_params.amic_bst_mode;

    /* MIC channel 0 initialization parameters section */
    CODEC_InitStruct.CODEC_Ch0Mute = voice_driver_codec_params.codec_ch0_mute;
    CODEC_InitStruct.CODEC_Ch0MicType = voice_driver_codec_params.codec_ch0_mic_type;
    CODEC_InitStruct.CODEC_Ch0DmicDataLatch = voice_driver_codec_params.codec_ch0_dmic_data_latch;
    CODEC_InitStruct.CODEC_Ch0AdGain = voice_driver_codec_params.codec_ch0_ad_gain;
    CODEC_InitStruct.CODEC_Ch0BoostGain = voice_driver_codec_params.codec_ch0_boost_gain;
    CODEC_InitStruct.CODEC_Ch0ZeroDetTimeout = voice_driver_codec_params.codec_ch0_zero_det_timeout;

    CODEC_Init(CODEC, &CODEC_InitStruct);
}

/******************************************************************
 * @brief   Deinitialize codec peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_deinit_codec(void)
{
    CODEC_DeInit(CODEC);

    /* restore power mode */
    SystemCall(3, 0);
}

/******************************************************************
 * @brief   Initialize GDMA peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_init_rx_gdma(void)
{
    /* Enable GDMA clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = VOICE_GDMA_Channel_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize          = VOICE_GDMA_FRAME_SIZE / 4;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_16;
    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&(I2S0->RX_DR));
    GDMA_InitStruct.GDMA_SourceHandshake     = GDMA_Handshake_I2S0_RX;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)voice_driver_global_data.gdma_buffer.buf0;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC0_RX;
    GDMA_InitStruct.GDMA_Multi_Block_En      = 1;
    GDMA_InitStruct.GDMA_Multi_Block_Struct  = (uint32_t)voice_driver_gdma_link_list;
    GDMA_InitStruct.GDMA_Multi_Block_Mode    = LLI_TRANSFER;

    /* Initialize GDMA Link List Struct */
    for (uint8_t i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            voice_driver_gdma_link_list[i].DAR = (uint32_t)voice_driver_global_data.gdma_buffer.buf0;
            voice_driver_gdma_link_list[i].LLP = (uint32_t)&voice_driver_gdma_link_list[i +
                                                                                        1];  /* link to buffer 1 */
        }
        else
        {
            voice_driver_gdma_link_list[i].DAR = (uint32_t)voice_driver_global_data.gdma_buffer.buf1;
            voice_driver_gdma_link_list[i].LLP = (uint32_t)&voice_driver_gdma_link_list[i -
                                                                                        1];  /* link back to buffer 0 */
        }
        voice_driver_gdma_link_list[i].SAR = GDMA_InitStruct.GDMA_SourceAddr;

        /* Configure low 32 bit of CTL register */
        voice_driver_gdma_link_list[i].CTL_LOW = BIT(0)
                                                 | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                 | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                                 | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                 | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                                 | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                 | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                 | (GDMA_InitStruct.GDMA_DIR << 20)
                                                 | (GDMA_InitStruct.GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
        /* Configure high 32 bit of CTL register */
        voice_driver_gdma_link_list[i].CTL_HIGH = GDMA_InitStruct.GDMA_BufferSize;
    }

    GDMA_Init(VOICE_GDMA_Channel, &GDMA_InitStruct);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = VOICE_GDMA_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority  = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    GDMA_INTConfig(VOICE_GDMA_Channel_NUM, GDMA_INT_Block, ENABLE);
    GDMA_Cmd(VOICE_GDMA_Channel_NUM, ENABLE);
}

/******************************************************************
 * @brief   Deinitialize GDMA peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_deinit_rx_gdma(void)
{
    GDMA_Cmd(VOICE_GDMA_Channel_NUM, DISABLE);
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/******************************************************************
 * @brief   Initialize voice driver global data.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_init_data(void)
{
    APP_PRINT_INFO0("[voice_driver_init_data] init data");
    memset(&voice_driver_global_data, 0, sizeof(voice_driver_global_data));
    voice_driver_global_data.is_allowed_to_enter_dlps = true;
}

/******************************************************************
 * @brief   Initialize codec parameters.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_init_codec_params(void)
{
    APP_PRINT_INFO0("[voice_driver_init_codec_params] init codec parameters");

    memset(&voice_driver_codec_params, 0, sizeof(voice_driver_codec_params));

    /* Basic parameters section */
    voice_driver_codec_params.codec_i2s_format = CODEC_I2S_DataFormat_I2S;
    voice_driver_codec_params.codec_i2s_data_width = CODEC_I2S_DataWidth_16Bits;
    voice_driver_codec_params.dmic_clock = DMIC_Clock_2500KHz;
#if (CODEC_SAMPLE_RATE_SEL == CODEC_SAMPLE_RATE_8KHz)
    voice_driver_codec_params.codec_sample_rate = SAMPLE_RATE_8KHz;
#elif (CODEC_SAMPLE_RATE_SEL == CODEC_SAMPLE_RATE_16KHz)
    voice_driver_codec_params.codec_sample_rate = SAMPLE_RATE_16KHz;
#endif

#if SUPPORT_MIC_BIAS_OUTPUT
    voice_driver_codec_params.is_mic_bias_output_en = true;
    voice_driver_codec_params.mic_bias_voltage = MICBIAS_VOLTAGE_1_8;
#endif

    voice_driver_codec_params.codec_ch0_mute = CODEC_CH0_UNMUTE;
    voice_driver_codec_params.codec_i2s_ch_sequence = CODEC_I2S_CH_L_L;

#if (VOICE_MIC_TYPE == AMIC_TYPE)
    voice_driver_codec_params.codec_ch0_mic_type = CODEC_CH0_AMIC;
    voice_driver_codec_params.amic_bst_gain = MICBST_Gain_30dB;
    voice_driver_codec_params.codec_ch0_ad_gain =
        0x2F;  /* 0x2F - 0dB, step 0.375dB, range -17.625dB ~ 30dB */
    voice_driver_codec_params.codec_ch0_boost_gain = Ch0_Boost_Gain_0dB;
#if (AMIC_INPUT_TYPE_SEL == AMIC_INPUT_TYPE_DIFF)
    voice_driver_codec_params.amic_bst_mode = MICBST_Mode_Differential;
#elif (AMIC_INPUT_TYPE_SEL == AMIC_INPUT_TYPE_SINGLE)
    voice_driver_codec_params.amic_bst_mode = MICBST_Mode_Single;
#endif

#elif (VOICE_MIC_TYPE == DMIC_TYPE)
    voice_driver_codec_params.codec_ch0_mic_type = CODEC_CH0_DMIC;
    voice_driver_codec_params.codec_ch0_boost_gain = Ch0_Boost_Gain_24dB;
    voice_driver_codec_params.codec_ch0_ad_gain =
        0x2F;  /* 0x2F - 0dB, step 0.375dB, range -17.625dB ~ 30dB */
    voice_driver_codec_params.codec_ch0_zero_det_timeout = Ch0_ADC_Zero_DetTimeout_1024_32_Sample;
#if (DMIC0_DATA_LATCH_TYPE == DMIC_DATA_LATCH_RISING_EDGE)
    voice_driver_codec_params.codec_ch0_dmic_data_latch = DMIC_Ch0_Rising_Latch;
#elif (DMIC0_DATA_LATCH_TYPE == DMIC_DATA_LATCH_FALLING_EDGE)
    voice_driver_codec_params.codec_ch0_dmic_data_latch = DMIC_Ch0_Falling_Latch;
#endif
#endif
}

/******************************************************************
 * @brief   Initialize voice driver.
 * @param   none
 * @return  none
 * @retval  void
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

/******************************************************************
 * @brief   Deinitialize voice driver.
 * @param   none
 * @return  none
 * @retval  void
 */
void voice_driver_deinit(void)
{
    voice_driver_global_data.is_allowed_to_enter_dlps = true;
    voice_driver_global_data.is_voice_driver_working = false;

    voice_driver_deinit_i2s();
    voice_driver_deinit_codec();
    voice_driver_deinit_rx_gdma();
}

/******************************************************************
 * @brief    voice driver check DLPS callback
 * @param    none
 * @return   bool
 * @retval   true or false
 */
bool voice_driver_dlps_check(void)
{
    return voice_driver_global_data.is_allowed_to_enter_dlps;
}

/******************************************************************
 * @brief   GDMA interrupt handler function.
 * @param   none
 * @return  none
 * @retval  void
 */
void VOICE_GDMA_Channel_Handler(void)
{
    GDMA_ClearINTPendingBit(VOICE_GDMA_Channel_NUM, GDMA_INT_Block);
    if (voice_driver_global_data.is_voice_driver_working == true)
    {
        T_IO_MSG gdma_msg;

        APP_PRINT_INFO0("[VOICE_GDMA_Channel_Handler] GDMA interrupt!");
        gdma_msg.type = IO_MSG_TYPE_GDMA;
        gdma_msg.subtype = 0;
        if (voice_driver_global_data.current_bibuff_index == 0)
        {
            voice_driver_global_data.current_bibuff_index = 1;
            gdma_msg.u.buf = (void *)voice_driver_global_data.gdma_buffer.buf0;
        }
        else
        {
            voice_driver_global_data.current_bibuff_index = 0;
            gdma_msg.u.buf = (void *)voice_driver_global_data.gdma_buffer.buf1;
        }
        if (false == app_send_msg_to_apptask(&gdma_msg))
        {
            APP_PRINT_WARN0("[VOICE_GDMA_Channel_Handler] Send IO_MSG_TYPE_GDMA failed!");
        }

    }
}

#if SUPPORT_UART_DUMP_FEATURE
/******************************************************************
 * @brief   Initialize UART driver.
 */
void voice_driver_uart_init(void)
{
    Pad_Config(VOICE_UART_TEST_TX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(VOICE_UART_TEST_TX, UART0_TX);

    RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);

    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);
    /* change to 3M baudrate */
    UART_InitStruct.UART_Div            = 1;
    UART_InitStruct.UART_Ovsr           = 8;
    UART_InitStruct.UART_OvsrAdj        = 0x492;
    UART_InitStruct.UART_TxWaterLevel   = 12;
    UART_InitStruct.UART_TxDmaEn        = ENABLE;
    UART_InitStruct.UART_DmaEn          = UART_DMA_ENABLE;
    UART_Init(UART0, &UART_InitStruct);
}

/******************************************************************
 * @brief   Initialize GDMA driver.
 */
void voice_uart_dump_init_rx_gdma(void)
{
    /* Enable GDMA clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = VOICE_GDMA_Channel_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize          = VOICE_GDMA_FRAME_SIZE / 4;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_16;
    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&(I2S0->RX_DR));
    GDMA_InitStruct.GDMA_SourceHandshake     = GDMA_Handshake_I2S0_RX;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(UART0->RB_THR));
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_UART0_TX;
    GDMA_InitStruct.GDMA_Multi_Block_En      = 1;
    GDMA_InitStruct.GDMA_Multi_Block_Struct  = (uint32_t)voice_driver_gdma_link_list;
    GDMA_InitStruct.GDMA_Multi_Block_Mode    = LLI_TRANSFER;

    /* Initialize GDMA Link List Struct */
    for (uint8_t i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            voice_driver_gdma_link_list[i].LLP = (uint32_t)&voice_driver_gdma_link_list[i +
                                                                                        1];  /* link to buffer 1 */
        }
        else
        {
            voice_driver_gdma_link_list[i].LLP = (uint32_t)&voice_driver_gdma_link_list[i -
                                                                                        1];  /* link back to buffer 0 */
        }
        voice_driver_gdma_link_list[i].DAR = GDMA_InitStruct.GDMA_DestinationAddr;
        voice_driver_gdma_link_list[i].SAR = GDMA_InitStruct.GDMA_SourceAddr;

        /* Configure low 32 bit of CTL register */
        voice_driver_gdma_link_list[i].CTL_LOW = BIT(0)
                                                 | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                 | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                                 | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                 | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                                 | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                 | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                 | (GDMA_InitStruct.GDMA_DIR << 20)
                                                 | (GDMA_InitStruct.GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
        /* Configure high 32 bit of CTL register */
        voice_driver_gdma_link_list[i].CTL_HIGH = GDMA_InitStruct.GDMA_BufferSize;
    }

    GDMA_Init(VOICE_GDMA_Channel, &GDMA_InitStruct);
    GDMA_Cmd(VOICE_GDMA_Channel_NUM, ENABLE);
}

/******************************************************************
 * @brief   Start voice recording.
 */
void voice_uart_dump_start_recording(void)
{
    voice_driver_init_data();
    voice_driver_global_data.is_allowed_to_enter_dlps = false;
    voice_driver_global_data.is_voice_driver_working = true;

    voice_driver_init_pad_and_pinmux();
    voice_driver_init_i2s();
    voice_driver_init_codec();
    voice_uart_dump_init_rx_gdma();
}

/******************************************************************
 * @brief   Start UART dump voice data.
 */
void voice_uart_dump_start(void)
{
    voice_driver_uart_init();
    voice_uart_dump_start_recording();
}

#endif

#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

