/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     ir_learn_driver.c
* @brief    This file provides ir learn driver.
* @details
* @author   barry_bian
* @date     2020-04-22
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include "trace.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_ir.h"
#include "app_task.h"
#include "ir_learn_driver.h"
#include "ir_send_driver.h"
#include "app_section.h"
#include "rtl876x_gdma.h"

#if (SUPPORT_IR_TX_FEATURE && SUPPORT_IR_LEARN_FEATURE)
/*============================================================================*
 *                          Local Variables
 *============================================================================*/
static T_IR_LEARN_STRUCT ir_learn_struct;
/* LLI structure for IR RX data */
static GDMA_LLIDef ir_learn_driver_gdma_link_list[IR_RX_GDMA_LINK_LIST_NUM];
/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void ir_learn_init_pad_and_pinmux(void);
static void ir_learn_init_driver(void);
static void ir_driver_rx_gdma_init(void);

static void ir_learn_get_rx_fifo_data(void) DATA_RAM_FUNCTION;
static void ir_learn_handler(void) DATA_RAM_FUNCTION;
#if IR_LEARN_DUTY_CYCLE_SUPPORT
T_IR_LEARN_RESULT ir_learn_decode_duty_cycle(T_IR_LEARN_PARA *p_ir_learn_para)
DATA_RAM_FUNCTION;
#endif
T_IR_LEARN_RESULT ir_learn_decode(T_IR_LEARN_PARA *p_ir_learn_para) DATA_RAM_FUNCTION;
bool ir_learn_check_dlps(void) DATA_RAM_FUNCTION;
void ir_learn_enter_dlps_config(void) DATA_RAM_FUNCTION;
void ir_learn_exit_dlps_config(void) DATA_RAM_FUNCTION;
void IR_RX_GDMA_Channel_Handler(void) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief   initialization of pinmux settings and pad settings.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_init_pad_and_pinmux(void)
{
    Pad_Config(IR_LEARN_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pinmux_Config(IR_LEARN_PIN, IRDA_RX);

    Pinmux_Deinit(IR_SEND_PIN);
    Pad_Config(IR_SEND_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
}

/******************************************************************
 * @brief   Initializes IR learn peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_init_driver(void)
{
    /* Enable IR clock */
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);

    /* Initialize IR */
    IR_InitTypeDef IR_InitStruct;
    IR_StructInit(&IR_InitStruct);
    IR_InitStruct.IR_Freq               = IR_LEARN_FREQ;
    /* Duty ratio = 1/IR_DutyCycle */
    IR_InitStruct.IR_DutyCycle          = 2;
    /* IR receiveing mode */
    IR_InitStruct.IR_Mode               = IR_MODE_RX;
    IR_InitStruct.IR_RxStartMode        = IR_RX_AUTO_MODE;
    /* Configure RX FIFO threshold level to trigger IR_INT_RF_LEVEL interrupt */
    IR_InitStruct.IR_RxFIFOThrLevel     = IR_RX_FIFO_THR_LEVEL;
    /* Discard the lastest received dta if RX FIFO is full */
    IR_InitStruct.IR_RxFIFOFullCtrl     = IR_RX_FIFO_FULL_DISCARD_NEWEST;
    /* Configure trigger type */
#if (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_RISING_EDGE)
    IR_InitStruct.IR_RxTriggerMode      = IR_RX_RISING_EDGE;
#elif (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_FALL_EDGE)
    IR_InitStruct.IR_RxTriggerMode      = IR_RX_FALL_EDGE;
#endif
    /* If high to low or low to high transition time <= 200ns,Filter out it. */
    IR_InitStruct.IR_RxFilterTime       = IR_RX_FILTER_TIME_200ns;
    /* Configure counting level */
#if (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_RISING_EDGE)
    IR_InitStruct.IR_RxCntThrType       = IR_RX_Count_Low_Level;
#elif (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_FALL_EDGE)
    IR_InitStruct.IR_RxCntThrType       = IR_RX_Count_High_Level;
#endif
    /* Configure RX counter threshold.You can use it to decide to stop receiving IR data */
    IR_InitStruct.IR_RxCntThr           = IR_LEARN_MAX_NO_WAVEFORM_TIME;
    IR_InitStruct.IR_RxDmaEn            = ENABLE;
    IR_InitStruct.IR_RxWaterLevel       = 0x4;

    IR_Init(&IR_InitStruct);

    IR_Cmd(IR_MODE_RX, DISABLE);

    IR_ClearRxFIFO();

    IR_INTConfig(IR_INT_RX_CNT_THR, ENABLE);
    IR_MaskINTConfig(IR_INT_RX_CNT_THR, DISABLE);

    /* Configure NVIC */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = IR_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* Modify IR interrupt handle */
    RamVectorTableUpdate(IR_VECTORn, ir_learn_handler);
}

/******************************************************************
 * @brief   Initialize GDMA peripheral.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_driver_rx_gdma_init(void)
{
    /* Enable GDMA clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = IR_RX_GDMA_Channel_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize          = IR_RX_GDMA_FRAME_SIZE;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&IR->RX_FIFO);
    GDMA_InitStruct.GDMA_SourceHandshake     = GDMA_Handshake_IR_RX;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)
                                               ir_learn_struct.p_ir_learn_data->gdma_buf_data[0].buf;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC0_RX;
    GDMA_InitStruct.GDMA_Multi_Block_En      = 1;
    GDMA_InitStruct.GDMA_Multi_Block_Struct  = (uint32_t)ir_learn_driver_gdma_link_list;
    GDMA_InitStruct.GDMA_Multi_Block_Mode    = LLI_TRANSFER;

    /* Initialize GDMA Link List Struct */
    for (uint8_t i = 0; i < IR_RX_GDMA_LINK_LIST_NUM; i++)
    {
        if (i == (IR_RX_GDMA_LINK_LIST_NUM - 1))
        {
            ir_learn_driver_gdma_link_list[i].DAR = (uint32_t)
                                                    ir_learn_struct.p_ir_learn_data->gdma_buf_data[i].buf;
            ir_learn_driver_gdma_link_list[i].LLP = (uint32_t)
                                                    &ir_learn_driver_gdma_link_list[0];  /* link to buffer 0 */
        }
        else
        {
            ir_learn_driver_gdma_link_list[i].DAR = (uint32_t)
                                                    ir_learn_struct.p_ir_learn_data->gdma_buf_data[i].buf;
            ir_learn_driver_gdma_link_list[i].LLP = (uint32_t)&ir_learn_driver_gdma_link_list[i +
                                                    1];  /* link back to buffer i+1 */
        }
        ir_learn_driver_gdma_link_list[i].SAR = GDMA_InitStruct.GDMA_SourceAddr;

        /* Configure low 32 bit of CTL register */
        ir_learn_driver_gdma_link_list[i].CTL_LOW = BIT(0)
                                                    | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                    | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                                    | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                    | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                                    | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                    | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                    | (GDMA_InitStruct.GDMA_DIR << 20)
                                                    | (GDMA_InitStruct.GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
        /* Configure high 32 bit of CTL register */
        ir_learn_driver_gdma_link_list[i].CTL_HIGH = GDMA_InitStruct.GDMA_BufferSize;
    }

    GDMA_Init(IR_RX_GDMA_Channel, &GDMA_InitStruct);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = IR_RX_GDMA_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority  = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    GDMA_INTConfig(IR_RX_GDMA_Channel_NUM, GDMA_INT_Block, ENABLE);
    GDMA_Cmd(IR_RX_GDMA_Channel_NUM, ENABLE);
}

/******************************************************************
 * @brief   write ir rx fifo data to buf
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_get_rx_fifo_data(void)
{
    /*  Suspend GDMA_Channelx   */
    GDMA_SuspendCmd(IR_RX_GDMA_Channel, ENABLE);

    ir_learn_struct.p_ir_learn_data->remaining_fifo_data_len = IR_GetRxDataLen();
    ir_learn_struct.p_ir_learn_data->remaining_gdma_data_len = GDMA_GetTransferLen(
                                                                   IR_RX_GDMA_Channel) / sizeof(uint32_t);
    APP_PRINT_INFO2("[ir_learn_get_rx_fifo_data] gdma_remain_data_len = %d, ir_remain_data_len = %d",
                    ir_learn_struct.p_ir_learn_data->remaining_gdma_data_len,
                    ir_learn_struct.p_ir_learn_data->remaining_fifo_data_len);
    ir_learn_struct.p_ir_learn_data->in_buff_total_num++;
    IR_ReceiveBuf(ir_learn_struct.p_ir_learn_data->remaining_fifo_data,
                  ir_learn_struct.p_ir_learn_data->remaining_fifo_data_len);
    ir_learn_struct.p_ir_learn_data->ir_learn_stop_flag = 1;
}

/******************************************************************
 * @brief   IR interrupt handler function.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_handler(void)
{
    T_IO_MSG bee_io_msg;
    bee_io_msg.type = IO_MSG_TYPE_IR;

    /* Must read int status first */
    ITStatus int_status_rx_cnt_thr = IR_GetINTStatus(IR_INT_RX_CNT_THR);

    /* Mask IR all interrupt */
    IR_MaskINTConfig(IR_INT_RX_CNT_THR, ENABLE);

    /* Stop to receive IR data */
    if (int_status_rx_cnt_thr == SET)
    {
        APP_PRINT_INFO0("[ir_learn_handler] IR_INT_RX_CNT_THR");
        /* Read remaining data */
        ir_learn_get_rx_fifo_data();
        IR_ClearINTPendingBit(IR_INT_RX_CNT_THR_CLR);

        bee_io_msg.subtype = IO_MSG_TYPE_IR_LEARN_STOP;

        if (false == app_send_msg_to_apptask(&bee_io_msg))
        {
            APP_PRINT_ERROR0("[ir_learn_handler] Send IO_MSG_TYPE_IR message failed!");
        }
    }
    /* Unmask IR all interrupt */
    IR_MaskINTConfig(IR_INT_RX_CNT_THR, DISABLE);
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
#if IR_LEARN_DUTY_CYCLE_SUPPORT
/******************************************************************
 * @brief  learn a specific IR waveform duty cycle.
 * @param  p_ir_learn_para - point to IR packet struct.
 * @return the decoded status.
 * @retval T_IR_LEARN_RESULT
 */
T_IR_LEARN_RESULT ir_learn_decode_duty_cycle(T_IR_LEARN_PARA *p_ir_learn_para)
{
    uint16_t i = 0;
    uint32_t buf[2 * (IR_LEARN_DUTY_CYCLE_MAX_CYCLE_SIZE + 1)] = {0};
    uint32_t carrier_high_time = 0;
    uint32_t carrier_low_time = 0;
    uint8_t carrier_valid_count = 0;

    memcpy(buf,
           ir_learn_struct.p_ir_learn_data->gdma_buf_data[ir_learn_struct.p_ir_learn_data->out_buff_total_num %
                                                                                                              IR_RX_GDMA_LINK_LIST_NUM].buf, 2 * (IR_LEARN_DUTY_CYCLE_MAX_CYCLE_SIZE + 1) * sizeof(uint32_t));

    for (i = 1; i < IR_LEARN_DUTY_CYCLE_MAX_CYCLE_SIZE + 1; i++)
    {
        uint16_t j = 2 * i;
        if (((buf[j] & IR_DATA_MSK) < IR_LEARN_TIME_HIGHEST_VALUE) &&
            ((buf[j + 1] & IR_DATA_MSK) < IR_LEARN_TIME_HIGHEST_VALUE))
        {
            if (buf[j] & IR_CARRIER_DATA_TYPE)
            {
                carrier_high_time += buf[j] & IR_DATA_MSK;
                carrier_low_time += buf[j + 1] & IR_DATA_MSK;
            }
            else
            {
                carrier_high_time += buf[j + 1] & IR_DATA_MSK;
                carrier_low_time += buf[j] & IR_DATA_MSK;
            }
            carrier_valid_count++;
        }
    }

    if (carrier_high_time > 0 && carrier_low_time > 0)
    {
        p_ir_learn_para->duty_cycle = (float)(carrier_high_time + carrier_low_time) / carrier_high_time;
        p_ir_learn_para->carrier_time = (carrier_high_time + carrier_low_time) / carrier_valid_count;
        APP_PRINT_INFO3("[ir_learn_decode_duty_cycle]: duty = %d, carrier_time = %d, count = %d",
                        (uint8_t)(p_ir_learn_para->duty_cycle), p_ir_learn_para->carrier_time, carrier_valid_count);
        return IR_LEARN_OK;
    }
    return IR_LEARN_DUTY_CYCLE_ERR_NO_VALID_DATA;
}
#endif

/******************************************************************
 * @brief  Convert IR learned waveform data to actual IR data which can be sent.
 * @param  p_ir_learn_para - point to IR packet struct.
 * @return none
 * @retval void
 */
void ir_learn_convert_data(T_IR_LEARN_PARA *p_ir_learn_para)
{
    uint16_t i = 0;
    if ((p_ir_learn_para->carrier_freq == 0) || (p_ir_learn_para->duty_cycle <= 0))
    {
        return ;
    }
    for (i = 0; i < p_ir_learn_para->ir_learn_buf_index; i++)
    {

        if (p_ir_learn_para->ir_learn_buffer[i] & IR_CARRIER_DATA_TYPE)
        {
            p_ir_learn_para->ir_learn_buffer[i] = (p_ir_learn_para->ir_learn_buffer[i] & IR_DATA_MSK) *
                                                  p_ir_learn_para->carrier_freq / IR_LEARN_FREQ;
            p_ir_learn_para->ir_learn_buffer[i] |= IR_CARRIER_DATA_TYPE;
        }
        else
        {
            p_ir_learn_para->ir_learn_buffer[i] = (p_ir_learn_para->ir_learn_buffer[i] & IR_DATA_MSK) *
                                                  p_ir_learn_para->carrier_freq / IR_LEARN_FREQ;
        }

        /*ir learn buffer data must minus 1 for writing to register*/
        if ((p_ir_learn_para->ir_learn_buffer[i] & IR_DATA_MSK) > 0)
        {
            p_ir_learn_para->ir_learn_buffer[i] -= 1;
        }
    }
}

/******************************************************************
 * @brief  learn a specific IR waveform freqency.
 * @param  p_ir_learn_para - point to IR packet struct.
 * @return the learning status.
 * @retval T_IR_LEARN_RESULT
 */
T_IR_LEARN_RESULT ir_learn_freq(T_IR_LEARN_PARA *p_ir_learn_para)
{
    uint16_t i = 0;
    uint16_t j = 0;
    float freq_sum = 0;
#if FILTER_IR_LEARN_FREQ
    float max_freq = 0;
    float min_freq = 2000000;
    float temp = 0;
#endif
    for (i = 0, j = 0; (i <= p_ir_learn_para->ir_learn_buf_index) &&
         (j <= p_ir_learn_para->carrier_info_idx); i++)
    {
        if (p_ir_learn_para->ir_learn_buffer[i] & IR_CARRIER_DATA_TYPE)
        {
            temp = (p_ir_learn_para->carrier_info_buf[j++] * (IR_LEARN_FREQ * 0.5)) / \
                   (p_ir_learn_para->ir_learn_buffer[i] & IR_DATA_MSK);

            freq_sum += temp;

#if FILTER_IR_LEARN_FREQ
            if (max_freq < temp)
            {
                max_freq = temp;
            }

            if (min_freq > temp)
            {
                min_freq = temp;
            }
#endif
        }
    }
#if FILTER_IR_LEARN_FREQ

    if (j > 2)
    {
        freq_sum -= (max_freq + min_freq);
        j -= 2;
    }
#endif
    if (j)
    {
        p_ir_learn_para->carrier_freq = freq_sum / j;
    }

    return IR_LEARN_OK;
}

/******************************************************************
 * @brief  learn a specific IR waveform.
 * @param  p_ir_learn_para - point to IR packet struct.
 * @return the decoded status.
 * @retval T_IR_LEARN_RESULT
 */
T_IR_LEARN_RESULT ir_learn_decode(T_IR_LEARN_PARA *p_ir_learn_para)
{
    uint32_t time_interval_buf[IR_RX_GDMA_FRAME_SIZE + IR_RX_FIFO_SIZE] = {0};
    uint32_t decode_data_len = 0;//*4byte

    if (ir_learn_struct.ir_learn_result != IR_LEARN_OK)
    {
        APP_PRINT_INFO1("[IR] ir_learn_decode ir_learn_result = %d", ir_learn_struct.ir_learn_result);
        return ir_learn_struct.ir_learn_result;
    }

#if IR_LEARN_DUTY_CYCLE_SUPPORT
    if (p_ir_learn_para->duty_cycle <= 0)
    {
        ir_learn_struct.ir_learn_result = ir_learn_decode_duty_cycle(p_ir_learn_para);

        if (ir_learn_struct.ir_learn_result != IR_LEARN_OK)
        {
            return ir_learn_struct.ir_learn_result;
        }
    }
#endif

    if (ir_learn_struct.p_ir_learn_data->ir_learn_stop_flag)
    {
        decode_data_len = ir_learn_struct.p_ir_learn_data->remaining_gdma_data_len +
                          ir_learn_struct.p_ir_learn_data->remaining_fifo_data_len;
        memcpy(time_interval_buf,
               ir_learn_struct.p_ir_learn_data->gdma_buf_data[ir_learn_struct.p_ir_learn_data->out_buff_total_num %
                                                                                                                  IR_RX_GDMA_LINK_LIST_NUM].buf,
               ir_learn_struct.p_ir_learn_data->remaining_gdma_data_len * sizeof(uint32_t));
        memcpy(time_interval_buf + ir_learn_struct.p_ir_learn_data->remaining_gdma_data_len,
               ir_learn_struct.p_ir_learn_data->remaining_fifo_data,
               ir_learn_struct.p_ir_learn_data->remaining_fifo_data_len * sizeof(uint32_t));
    }
    else
    {
        decode_data_len = IR_RX_GDMA_FRAME_SIZE;
        memcpy(time_interval_buf,
               ir_learn_struct.p_ir_learn_data->gdma_buf_data[ir_learn_struct.p_ir_learn_data->out_buff_total_num %
                                                                                                                  IR_RX_GDMA_LINK_LIST_NUM].buf, decode_data_len * sizeof(uint32_t));
    }
    ir_learn_struct.p_ir_learn_data->gdma_buf_data[(ir_learn_struct.p_ir_learn_data->out_buff_total_num++)
                                                   % IR_RX_GDMA_LINK_LIST_NUM].buf_full_flag = false;

    APP_PRINT_INFO1("[IR] out_buff_total_num = %d",
                    ir_learn_struct.p_ir_learn_data->out_buff_total_num);

    for (uint32_t i = 0; i < decode_data_len; i++)
    {
        /* Extract data */
        time_interval_buf[i] &= IR_DATA_MSK;

        /* Check the maximum number of learning data */
        if (p_ir_learn_para->ir_learn_buf_index >= IR_LEARN_WAVE_MAX_LEN)
        {
            ir_learn_struct.ir_learn_result = IR_LEARN_EXCEED_SIZE;
            break;
        }

        /* Record total time of carrier wave */
        if (time_interval_buf[i] < IR_LEARN_TIME_HIGHEST_VALUE)
        {
            /* Record carrier waveform time */
            p_ir_learn_para->ir_learn_buffer[p_ir_learn_para->ir_learn_buf_index] += time_interval_buf[i];

            /* Record carrier number */
            p_ir_learn_para->carrier_info_buf[p_ir_learn_para->carrier_info_idx]++;

            /* Record data type */
            p_ir_learn_para->is_carrier = true;
#if IR_LEARN_DUTY_CYCLE_SUPPORT
            p_ir_learn_para->last_handle_data = time_interval_buf[i];
#endif
        }
        else
        {
            if (p_ir_learn_para->is_carrier == true)
            {
                /* Record carrier number */
                p_ir_learn_para->carrier_info_buf[p_ir_learn_para->carrier_info_idx]++;

#if IR_LEARN_DUTY_CYCLE_SUPPORT
                if (p_ir_learn_para->duty_cycle)
                {
                    if (p_ir_learn_para->last_handle_data < IR_LEARN_TIME_HIGHEST_VALUE)
                    {
                        p_ir_learn_para->ir_learn_buffer[p_ir_learn_para->ir_learn_buf_index] +=
                            p_ir_learn_para->carrier_time -
                            p_ir_learn_para->last_handle_data;
                    }
                }
                else
                {
                    APP_PRINT_INFO0("[IR] Warning: IR carrier data compensation handle error!");
                    ir_learn_struct.ir_learn_result =  IR_LEARN_CARRIRE_DATA_HANDLE_ERR;
                }
#endif
                APP_PRINT_INFO2("[IR] high power time, index[%d]: %d", p_ir_learn_para->ir_learn_buf_index,
                                p_ir_learn_para->ir_learn_buffer[p_ir_learn_para->ir_learn_buf_index]);
                /* Store acitve carriar data, set IR_TX_DATA_TYPE*/
                p_ir_learn_para->ir_learn_buffer[p_ir_learn_para->ir_learn_buf_index++] |= IR_CARRIER_DATA_TYPE;

                /* Check the maximum number of learning data */
                if (p_ir_learn_para->ir_learn_buf_index >= IR_LEARN_WAVE_MAX_LEN)
                {
                    ir_learn_struct.ir_learn_result = IR_LEARN_EXCEED_SIZE;
                    break;
                }

#if IR_LEARN_DUTY_CYCLE_SUPPORT
                if (p_ir_learn_para->duty_cycle)
                {
                    /* Store value of low waveform */
                    p_ir_learn_para->ir_learn_buffer[p_ir_learn_para->ir_learn_buf_index++] = time_interval_buf[i] +
                                                                                              p_ir_learn_para->last_handle_data - \
                                                                                              p_ir_learn_para->carrier_time;
                }
                else
                {
                    /* Store value of low waveform */
                    p_ir_learn_para->ir_learn_buffer[p_ir_learn_para->ir_learn_buf_index++] = time_interval_buf[i];
                    APP_PRINT_INFO0("[IR] Warning: IR no carrier data compensation handle error!");
                    ir_learn_struct.ir_learn_result = IR_LEARN_NO_CARRIRE_DATA_HANDLE_ERR;
                }
#else
                /* Store inactive carrier data*/
                p_ir_learn_para->ir_learn_buffer[p_ir_learn_para->ir_learn_buf_index++] = time_interval;
#endif
                APP_PRINT_INFO2("[IR] low power time, index[%d]: %d", p_ir_learn_para->ir_learn_buf_index - 1,
                                p_ir_learn_para->ir_learn_buffer[p_ir_learn_para->ir_learn_buf_index - 1]);

                /* Record new carrier waveform information */
                p_ir_learn_para->carrier_info_idx++;
                /* Record data type */
                p_ir_learn_para->is_carrier = false;
            }
            else
            {
                APP_PRINT_INFO0("[IR] Warning: exceed maximum stop signal value!");
                ir_learn_struct.ir_learn_result = IR_LEARN_WAVEFORM_ERR;
                break;
            }
        }

        /* Check the maximum number of learning data */
        if (p_ir_learn_para->ir_learn_buf_index >= IR_LEARN_WAVE_MAX_LEN)
        {
            ir_learn_struct.ir_learn_result = IR_LEARN_EXCEED_SIZE;
            break;
        }

        /* Check IR end signal */
        if (time_interval_buf[i] >= IR_LEARN_STOP_TIME)
        {
            ir_learn_struct.ir_learn_result = IR_LEARN_EXIT;
            break;
        }
    }

    return ir_learn_struct.ir_learn_result;
}

/******************************************************************
 * @brief   Check ir learn if working.
 * @param   none
 * @return  if ir is workging for learning
 * @retval  true or false
 */
bool ir_learn_is_working(void)
{
    if (ir_learn_struct.ir_learn_state == IR_LEARN_IDLE)
    {
        return false;
    }
    return true;
}

/******************************************************************
 * @brief   Check if ir learn allow enter dlps.
 * @param   none
 * @return  ir learn allow enter dlps or not
 * @retval  true or false
 */
bool ir_learn_check_dlps(void)
{
    /*ir learn not in work, allow dlps*/
    if (ir_learn_struct.ir_learn_state == IR_LEARN_WORKING)
    {
        return false;
    }
    return true;
}

/******************************************************************
 * @brief    ir learn enter DLPS config
 * @param    none
 * @return   none
 * @retval   void
 */
void ir_learn_enter_dlps_config(void)
{
#if (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_RISING_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#elif (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_FALL_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
#endif
}

/******************************************************************
 * @brief    ir learn exit DLPS config
 * @param    none
 * @return   none
 * @retval   void
 */
void ir_learn_exit_dlps_config(void)
{
#if (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_RISING_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
#elif (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_FALL_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
#endif
}

/******************************************************************
 * @brief   GDMA interrupt handler function.
 * @param   none
 * @return  none
 * @retval  void
 */
void IR_RX_GDMA_Channel_Handler(void)
{
    GDMA_ClearINTPendingBit(IR_RX_GDMA_Channel_NUM, GDMA_INT_Block);

    T_IO_MSG gdma_msg;

    APP_PRINT_INFO0("[IR_RX_GDMA_Channel_Handler] GDMA interrupt!");
    gdma_msg.type = IO_MSG_TYPE_IR;
    gdma_msg.subtype = IO_MSG_TYPE_IR_LEARN_DATA;
    if (false ==
        ir_learn_struct.p_ir_learn_data->gdma_buf_data[(ir_learn_struct.p_ir_learn_data->in_buff_total_num)
                                                       % IR_RX_GDMA_LINK_LIST_NUM].buf_full_flag)
    {
        gdma_msg.u.buf = (void *)
                         ir_learn_struct.p_ir_learn_data->gdma_buf_data[ir_learn_struct.p_ir_learn_data->in_buff_total_num %
                                                                                                                           IR_RX_GDMA_LINK_LIST_NUM].buf;
        ir_learn_struct.p_ir_learn_data->gdma_buf_data[(ir_learn_struct.p_ir_learn_data->in_buff_total_num++)
                                                       % IR_RX_GDMA_LINK_LIST_NUM].buf_full_flag = true;
    }
    else
    {
        ir_learn_struct.ir_learn_result = IR_LEARN_BUFFER_OVERFLOW;
    }
    APP_PRINT_INFO1("[IR_RX_GDMA_Channel_Handler] in_buff_total_num = %d",
                    ir_learn_struct.p_ir_learn_data->in_buff_total_num);

    if (false == app_send_msg_to_apptask(&gdma_msg))
    {
        APP_PRINT_WARN0("[IR_RX_GDMA_Channel_Handler] Send IO_MSG_TYPE_IR failed!");
    }
}

/******************************************************************
 * @brief   get ir learn current state
 * @param   none
 * @return  ir learn current state
 * @retval  T_IR_LEARN_RESULT
 */
T_IR_LEARN_STATE ir_learn_get_current_state(void)
{
    return ir_learn_struct.ir_learn_state;
}

/******************************************************************
 * @brief   Initializes IR learn data.
 * @param   p_ir_learn_para
 * @return  none
 * @retval  void
 */
void ir_learn_data_init(T_IR_LEARN_PARA *p_ir_learn_para)
{
    memset(p_ir_learn_para, 0, sizeof(T_IR_LEARN_PARA));

    for (uint8_t i = 0; i < IR_RX_GDMA_LINK_LIST_NUM; i++)
    {

        p_ir_learn_para->gdma_buf_data[i].buf = os_mem_alloc(RAM_TYPE_DATA_ON,
                                                             IR_RX_GDMA_FRAME_SIZE * sizeof(uint32_t));
        if (p_ir_learn_para->gdma_buf_data[i].buf != NULL)
        {
            memset((uint8_t *)p_ir_learn_para->gdma_buf_data[i].buf, 0,
                   IR_RX_GDMA_FRAME_SIZE * sizeof(uint32_t));
        }
        else
        {
            os_mem_free(p_ir_learn_para->gdma_buf_data[i].buf);
            p_ir_learn_para->gdma_buf_data[i].buf = NULL;
        }
    }
#if !IR_LEARN_DUTY_CYCLE_SUPPORT
    ir_learn_struct.p_ir_learn_data->duty_cycle = IR_DUTY_CYCLE;
#endif
}

/******************************************************************
 * @brief   IR learn module init.
 * @param   p_ir_learn_para
 * @return  none
 * @retval  void
 */
void ir_learn_module_init(T_IR_LEARN_PARA *p_ir_learn_para)
{
    ir_learn_init_pad_and_pinmux();
    ir_learn_struct.p_ir_learn_data = p_ir_learn_para;
    ir_learn_struct.ir_learn_state = IR_LEARN_READY;
}

/******************************************************************
 * @brief   start ir learning
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_start(void)
{
    if (ir_learn_struct.ir_learn_state != IR_LEARN_WORKING)
    {
        APP_PRINT_INFO0("[IR] ir learn mode start.");
        ir_learn_data_init(ir_learn_struct.p_ir_learn_data);
        ir_learn_struct.ir_learn_state = IR_LEARN_WORKING;
        ir_learn_struct.ir_learn_result = IR_LEARN_OK;
        ir_learn_init_driver();
        ir_driver_rx_gdma_init();
        IR_Cmd(IR_MODE_RX, ENABLE);
    }
    else
    {
        APP_PRINT_INFO0("[IR] ir learn mode already started.");
    }
}

/******************************************************************
 * @brief   stop ir learning
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_stop(void)
{
    if (ir_learn_struct.ir_learn_state == IR_LEARN_WORKING)
    {
        APP_PRINT_INFO0("[IR] ir learn mode stop.");
        ir_learn_struct.ir_learn_state = IR_LEARN_READY;
        GDMA_Cmd(IR_RX_GDMA_Channel_NUM, DISABLE);
        IR_Cmd(IR_MODE_RX, DISABLE);
        for (uint8_t i = 0; i < IR_RX_GDMA_LINK_LIST_NUM; i++)
        {
            if (ir_learn_struct.p_ir_learn_data->gdma_buf_data[i].buf != NULL)
            {
                APP_PRINT_INFO0("[IR] ir_learn_stop: mem_free_buf");
                os_mem_free(ir_learn_struct.p_ir_learn_data->gdma_buf_data[i].buf);
                ir_learn_struct.p_ir_learn_data->gdma_buf_data[i].buf = NULL;
            }
        }
    }
    else
    {
        APP_PRINT_INFO0("[IR] ir learn mode is not working, can not stop.");
    }
}

/******************************************************************
 * @brief   exit ir learning
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_exit(void)
{
    if (ir_learn_struct.ir_learn_state != IR_LEARN_IDLE)
    {
        APP_PRINT_INFO0("[IR] ir learn mode exit.");
        ir_learn_struct.ir_learn_state = IR_LEARN_IDLE;
        IR_Cmd(IR_MODE_RX, DISABLE);
        ir_send_pad_init();
        ir_send_pinmux_init();
        for (uint8_t i = 0; i < IR_RX_GDMA_LINK_LIST_NUM; i++)
        {
            if (ir_learn_struct.p_ir_learn_data->gdma_buf_data[i].buf != NULL)
            {
                APP_PRINT_INFO0("[IR] ir_learn_exit: mem_free_buf");
                os_mem_free(ir_learn_struct.p_ir_learn_data->gdma_buf_data[i].buf);
                ir_learn_struct.p_ir_learn_data->gdma_buf_data[i].buf = NULL;
            }
        }
    }
    else
    {
        APP_PRINT_INFO0("[IR] ir learn mode already exited.");
    }
}
#endif /*end Micro @SUPPORT_IR_TX_FEATURE && SUPPORT_IR_LEARN_FEATURE*/

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

