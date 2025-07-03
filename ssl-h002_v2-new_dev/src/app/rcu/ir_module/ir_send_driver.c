/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file         ir_send_app.c
* @brief        This file provides IR sending data handler by interrupt.
* @details      Application users can use the global functions to send IR data.
* @author       barry_bian
* @date         2020-02-26
* @version      v1.1
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "string.h"
#include "board.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_rtc.h"
#include "rtl876x_ir.h"
#include "trace.h"
#include "app_task.h"
#include "app_section.h"
#include "ir_send_driver.h"
#include "rtl876x_gdma.h"
#if SUPPORT_IR_LEARN_FEATURE
#include "ir_learn_driver.h"
#endif
#include "rtc_driver.h"

#if SUPPORT_IR_TX_FEATURE
/*============================================================================*
 *                          Local Variables
 *============================================================================*/
static T_IR_SEND_STRUCT ir_send_struct;

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void ir_send_driver_init(void);
static void ir_driver_tx_gdma_deinit(void);
static void ir_driver_tx_gdma_init(uint32_t source_addr, uint32_t buffer_size);

static void ir_send_int_handler(void) DATA_RAM_FUNCTION;
static void ir_send_rtc_handler(void) DATA_RAM_FUNCTION;

bool ir_send_check_dlps(void) DATA_RAM_FUNCTION;
void ir_send_enter_dlps_config(void) DATA_RAM_FUNCTION;
void ir_send_exit_dlps_config(void) DATA_RAM_FUNCTION;

/*============================================================================*
 *                          Local Functions
 *============================================================================*/

/******************************************************************
 * @brief   initialization of pinmux settings and pad settings.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_send_pad_init(void)
{
    Pad_Config(IR_SEND_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

#if SUPPORT_IR_LEARN_FEATURE
#if (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_RISING_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#elif (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_FALL_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
#endif
#endif
}

/******************************************************************
 * @brief   initialization of pinmux settings and pad settings.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_send_pinmux_init(void)
{
    Pinmux_Config(IR_SEND_PIN, IRDA_TX);

#if SUPPORT_IR_LEARN_FEATURE
    Pinmux_Deinit(IR_LEARN_PIN);
#endif
}

/******************************************************************
 * @brief   Initialize IR send peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_send_driver_init(void)
{
    /* Enable IR clock */
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);

    /* Initialize IR */
    IR_InitTypeDef IR_InitStruct;
    IR_StructInit(&IR_InitStruct);
    IR_InitStruct.IR_Freq           = ir_send_struct.p_ir_send_data->carrier_frequency_hz;
    IR_InitStruct.IR_DutyCycle      = ir_send_struct.p_ir_send_data->duty_cycle;
    IR_InitStruct.IR_Mode           = IR_MODE_TX;
    IR_InitStruct.IR_TxInverse      = IR_TX_DATA_NORMAL;
    IR_InitStruct.IR_TxFIFOThrLevel = IR_TX_FIFO_THR_LEVEL;
    IR_InitStruct.IR_TxDmaEn        = ENABLE;
    IR_InitStruct.IR_TxWaterLevel   = 28;
    IR_Init(&IR_InitStruct);

    /*must disable IR TX first*/
    IR_Cmd(IR_MODE_TX, DISABLE);

    /* Configure NVIC */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = IR_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* Modify IR interrupt handle */
    RamVectorTableUpdate(IR_VECTORn, ir_send_int_handler);
}

/******************************************************************
 * @brief   Initialize GDMA peripheral for IR TX.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_driver_tx_gdma_init(uint32_t source_addr, uint32_t buffer_size)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef GDMA_InitStruct;

    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum             = IR_TX_GDMA_Channel_num;
    GDMA_InitStruct.GDMA_DIR                    = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc              = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc         = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize         = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize    = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize            = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize       = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationAddr        = (uint32_t)(&IR->TX_FIFO);
    GDMA_InitStruct.GDMA_DestHandshake          = GDMA_Handshake_IR_TX;
    //IR FIFO empty interruption doesn't mean the end of IR transmission.
    //The buffer_size plus one to ensure that the last data is sent intact.
    GDMA_InitStruct.GDMA_BufferSize             = buffer_size + 1;
    GDMA_InitStruct.GDMA_SourceAddr             = source_addr;
    GDMA_Init(IR_TX_GDMA_CHANNEL, &GDMA_InitStruct);
}

/******************************************************************
 * @brief   Deinitialize GDMA peripheral for IR TX.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_driver_tx_gdma_deinit(void)
{
    GDMA_Cmd(IR_TX_GDMA_Channel_num, DISABLE);
}

/******************************************************************
 * @brief   IR interrupt handler function for IR sending.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_send_int_handler(void)
{
    /* Must Read INT Status first */
    ITStatus int_status = IR_GetINTStatus(IR_INT_TF_EMPTY);

    /* Mask IR interrupt */
    IR_MaskINTConfig(IR_INT_TF_EMPTY, ENABLE);

    if (int_status == SET)
    {
        APP_PRINT_INFO0("[IR] IR_INT_TF_EMPTY");

        IR_ClearINTPendingBit(IR_INT_TF_EMPTY_CLR);

        if (ir_send_struct.ir_send_state == IR_SEND_CAMMAND)
        {
            ir_send_struct.ir_send_state = IR_SEND_CAMMAND_COMPLETE;
        }
        else if (ir_send_struct.ir_send_state == IR_SEND_REPEAT_CODE)
        {
            ir_send_struct.ir_send_state = IR_SEND_REPEAT_CODE_COMPLETE;
        }
        T_IO_MSG bee_io_msg;
        bee_io_msg.type = IO_MSG_TYPE_IR;
        bee_io_msg.subtype = IO_MSG_TYPE_IR_SEND_COMPLETE;
        if (false == app_send_msg_to_apptask(&bee_io_msg))
        {
            APP_PRINT_ERROR0("[ir_send_int_handler] Send IO_MSG_TYPE_IR message failed!");
        }
    }
}

/******************************************************************
 * @brief   RTC interrupt handle function for IR sending repeat code.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_send_rtc_handler(void)
{
    APP_PRINT_INFO0("[IR] IR Repeat Code RTC Handler.");

    T_IO_MSG bee_io_msg;
    bee_io_msg.type = IO_MSG_TYPE_IR;
    bee_io_msg.subtype = IO_MSG_TYPE_IR_START_SEND_REPEAT_CODE;
    if (false == app_send_msg_to_apptask(&bee_io_msg))
    {
        APP_PRINT_ERROR0("[ir_send_rtc_handler] Send IO_MSG_TYPE_IR message failed!");
    }
}

/*============================================================================*
 *                          Global Functions
 *============================================================================*/

/******************************************************************
 * @brief    convert time and frequency to carrier cycle.
 * @param    time - time of waveform.
 * @param    carrier_cycle - cycle of carrier.
 * @return   vaule of data whose unit is cycle of carrier.
 */
uint32_t ir_send_convert_to_carrier_cycle(uint32_t time, uint32_t freq)
{
    return ((time & PULSE_HIGH) | ((time & IR_DATA_MSK) * freq / 1000000));
}

/******************************************************************
 * @brief   check if ir send is working.
 * @param   none
 * @return  if ir send is working
 * @retval  true or false
 */
bool ir_send_is_working(void)
{
    if (ir_send_struct.ir_send_state == IR_SEND_IDLE)
    {
        return false;
    }
    return true;
}

/******************************************************************
 * @brief   check ir send if allow enter dlps.
 * @param   none
 * @return  if allow enter dlps
 * @retval  true or false
 */
bool ir_send_check_dlps(void)
{
    /*ir send in idle or send complete*/
    if (ir_send_struct.ir_send_state == IR_SEND_IDLE)
    {
        return true;
    }

    /*ir send not in idle and not send completed*/
    return false;
}

/******************************************************************
 * @brief    ir send enter DLPS config
 * @param    none
 * @return   none
 * @retval   void
 */
void ir_send_enter_dlps_config(void)
{
    Pad_Config(IR_SEND_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
}

/******************************************************************
 * @brief    ir send exit DLPS config
 * @param    none
 * @return   none
 * @retval   void
 */
void ir_send_exit_dlps_config(void)
{
#if SUPPORT_IR_LEARN_FEATURE
    if (ir_learn_is_working() == false)
    {
        Pad_Config(IR_SEND_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
#else
    Pad_Config(IR_SEND_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#endif
}

/******************************************************************
 * @brief   get ir send current state
 * @param   none
 * @return  ir send current state
 * @retval  T_IR_SEND_STATE
 */
T_IR_SEND_STATE ir_send_get_current_state(void)
{
    return ir_send_struct.ir_send_state;
}

/******************************************************************
 * @brief   Initialize IR send module
 * @param   key_value - pressed key value
 * @return  result of initialization
 * @retval  true or false
 */
bool ir_send_module_init(T_IR_SEND_PARA *p_ir_send_para)
{
    if (p_ir_send_para == NULL)
    {
        APP_PRINT_INFO0("[IR] ir send module init fail, p_ir_send_para is NULL.");
        return false;
    }
    ir_send_struct.p_ir_send_data = p_ir_send_para;
    if (ir_send_struct.p_ir_send_data->carrier_frequency_hz < 5000 ||
        ir_send_struct.p_ir_send_data->carrier_frequency_hz > 2000000)
    {
        APP_PRINT_INFO0("[IR] ir send module init fail, carrier frequecy <5KHz or >2MHz.");
        return false;
    }
    if (ir_send_struct.p_ir_send_data->duty_cycle <= 0)
    {
        APP_PRINT_INFO0("[IR] ir send module init fail, duty_cycle is invalid.");
        return false;
    }
    ir_send_pad_init();
    ir_send_pinmux_init();
    ir_send_driver_init();

    APP_PRINT_INFO0("[IR] ir send module init success.");
    return true;
}

/******************************************************************
 * @brief   start function of sending IR command.
 * @param   none
 * @return  start successfully or not
 * @retval  true or false
 */
bool ir_send_command_start(void)
{
    if (ir_send_struct.p_ir_send_data->carrier_frequency_hz < 5000 ||
        ir_send_struct.p_ir_send_data->carrier_frequency_hz > 2000000)
    {
        APP_PRINT_INFO0("[IR] ir send command start fail, carrier frequecy <5KHz or >2MHz.");
        return false;
    }
    APP_PRINT_INFO0("[IR] ir send start command.");

    /*init gdma*/
    ir_driver_tx_gdma_init((uint32_t)ir_send_struct.p_ir_send_data->ir_send_buf,
                           ir_send_struct.p_ir_send_data->send_buf_len);
    /*
      1. enable IR TX
      2. enable gdma
      3. enable IR_INT_TF_EMPTY
    */
    IR_Cmd(IR_MODE_TX, ENABLE);
    GDMA_Cmd(IR_TX_GDMA_Channel_num, ENABLE);
    IR_MaskINTConfig(IR_INT_TF_EMPTY, DISABLE);
    IR_INTConfig(IR_INT_TF_EMPTY, ENABLE);

    /*enable RTC*/
    rtc_driver_set_comp_0_params(ir_send_struct.p_ir_send_data->command_time_ms, ir_send_rtc_handler);

    ir_send_struct.ir_send_state = IR_SEND_CAMMAND;

    return true;
}

/******************************************************************
 * @brief   start function of sending IR repeat code.
 * @param   none
 * @return  start successfully or not
 * @retval  true or false
 */
bool ir_send_repeat_code_start(void)
{
    if (ir_send_struct.p_ir_send_data->carrier_frequency_hz < 5000 ||
        ir_send_struct.p_ir_send_data->carrier_frequency_hz > 2000000)
    {
        APP_PRINT_INFO0("[IR] ERR: ir send carrier frequency <5KHz or >2MHz.");
        return false;
    }

    if (ir_send_struct.p_ir_send_data->repeat_buf_len == 0)
    {
        APP_PRINT_INFO0("[IR] ERR: ir send repeat buffer length is 0.");
        return false;
    }

    APP_PRINT_INFO0("[IR] ir send start repeat code.");
    /*init gdma*/
    ir_driver_tx_gdma_init((uint32_t)ir_send_struct.p_ir_send_data->ir_repeat_code_buf,
                           ir_send_struct.p_ir_send_data->repeat_buf_len);
    /*
      1. enable IR TX
      2. enable gdma
      3. enable IR_INT_TF_EMPTY
    */
    IR_Cmd(IR_MODE_TX, ENABLE);
    GDMA_Cmd(IR_TX_GDMA_Channel_num, ENABLE);
    IR_MaskINTConfig(IR_INT_TF_EMPTY, DISABLE);
    IR_INTConfig(IR_INT_TF_EMPTY, ENABLE);

    /*enable RTC*/
    rtc_driver_set_comp_0_params(ir_send_struct.p_ir_send_data->repeat_code_time_ms,
                                 ir_send_rtc_handler);

    ir_send_struct.ir_send_state = IR_SEND_REPEAT_CODE;

    return true;
}

/******************************************************************
 * @brief   exit IR sending.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_send_exit(void)
{
    APP_PRINT_INFO0("[IR] ir send exit.");

    rtc_driver_clear_comp_0();

    ir_driver_tx_gdma_deinit();

    IR_DeInit();
    IR_INTConfig(IR_INT_TF_EMPTY, DISABLE);

    ir_send_struct.ir_send_state = IR_SEND_IDLE;
}

#endif /*end Micro @SUPPORT_IR_TX_FEATURE*/


/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

