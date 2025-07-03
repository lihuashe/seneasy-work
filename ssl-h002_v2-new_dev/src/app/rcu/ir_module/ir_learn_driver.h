/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ir_learn_driver.h
* @brief
* @details
* @author    elliot chen
* @date      2020-02-26
* @version   v1.1
* *********************************************************************************************************
*/

#ifndef __IR_LEARN_DRIVER_H
#define __IR_LEARN_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "rtl876x.h"
#include "ir_send_driver.h"
#include "loop_queue.h"
#include "rtl876x_tim.h"
#include "rtl876x_ir.h"
/* Defines -------------------------------------------------------------------*/

#define  IR_LEARN_WAVE_MAX_LEN              IR_SEND_WAVE_MAX_LEN

/**
  * @brief  Board configuration by user
  */
#define IR_RX_FIFO_THR_LEVEL                20
#define IR_LEARN_FREQ                       40000000 /* 40MHz*/

/**
  * @brief  No carrier waveform maximum time is 6ms
  */
#define IR_LEARN_MAX_NO_WAVEFORM_TIME       ((uint32_t)6*(IR_LEARN_FREQ/1000))
#define IR_LEARN_STOP_TIME                  (IR_LEARN_MAX_NO_WAVEFORM_TIME*0.95)

/* Filter threshold value. If time interval< 400us(5KHz), treat it as a part of a carrier time */
/* IR learn carrier freqency between 5KHz and 2MHz */
#define IR_LEARN_TIME_HIGHEST_VALUE         (400*(IR_LEARN_FREQ/1000000))

/* Carrier waveform data type select */
#define IR_CARRIER_DATA_TYPE                ((uint32_t)0x80000000UL)

/**
  * @brief  Enable filter IR freqency or not
  */
#define FILTER_IR_LEARN_FREQ                1
/**
  * @brief  Enable IR duty cycle learning or not
  */
#define IR_LEARN_DUTY_CYCLE_SUPPORT         1
#define IR_LEARN_DUTY_CYCLE_MAX_CYCLE_SIZE  6


#define IR_LEARN_LOOP_QUEUE_MAX_SIZE        (1024 * sizeof(uint32_t))

/* IR RX GDMA definitions */
#define IR_RX_GDMA_Channel                  GDMA_Channel1
#define IR_RX_GDMA_Channel_NUM              1
#define IR_RX_GDMA_Channel_IRQn             GDMA0_Channel1_IRQn
#define IR_RX_GDMA_Channel_Handler          GDMA0_Channel1_Handler

#define IR_RX_GDMA_LINK_LIST_NUM            2
#define IR_RX_GDMA_FRAME_SIZE               256

/** @defgroup IR_LEARN_Exported_Types IR Learn Exported Types
    * @{
    */
typedef enum
{
    IR_LEARN_OK,                            /**< IR learn ok: learning */
    IR_LEARN_EXIT,                          /**< IR learn exit: complete IR learn */
    IR_LEARN_WAVEFORM_ERR,                  /**< IR learn waveform error */
    IR_LEARN_CARRIRE_DATA_HANDLE_ERR,       /**< IR learn carrier data compensation error */
    IR_LEARN_NO_CARRIRE_DATA_HANDLE_ERR,    /**< IR learn carrier data compensation error */
    IR_LEARN_EXCEED_SIZE,                   /**< IR learn exceed maximum size */
    IR_LEARN_DUTY_CYCLE_ENERR_NO_SAMPLE,    /**< IR learn duty cycle error: samples were not collceted */
    IR_LEARN_DUTY_CYCLE_ERR_NO_VALID_DATA,  /**< IR learn duty cycle error: have no valid data */
    IR_LEARN_BUFFER_OVERFLOW,
} T_IR_LEARN_RESULT;

typedef enum
{
    IR_LEARN_IDLE = 0,
    IR_LEARN_READY,
    IR_LEARN_WORKING,
} T_IR_LEARN_STATE;

/**
  * @brief  IR learn data structure
  */
typedef struct
{
    uint32_t *buf;
    bool buf_full_flag;
} T_GDMA_BUF_DATA_TYPE __attribute__((aligned(4)));

typedef struct
{
    uint32_t             ir_learn_buffer[IR_LEARN_WAVE_MAX_LEN];
    uint16_t             ir_learn_buf_index;
    uint16_t             carrier_info_buf[IR_LEARN_WAVE_MAX_LEN / 2 + 2];
    uint16_t             carrier_info_idx;
    uint8_t              is_carrier;
    float                carrier_freq;
    float                duty_cycle;
#if IR_LEARN_DUTY_CYCLE_SUPPORT
    uint32_t             last_handle_data;
    uint32_t             carrier_time;
#endif
    uint32_t             in_buff_total_num;
    uint32_t             out_buff_total_num;
    uint16_t             remaining_gdma_data_len;
    uint16_t             remaining_fifo_data_len;
    uint8_t              ir_learn_stop_flag;
    uint32_t             remaining_fifo_data[IR_RX_FIFO_SIZE];
    T_GDMA_BUF_DATA_TYPE gdma_buf_data[IR_RX_GDMA_LINK_LIST_NUM];
} T_IR_LEARN_PARA  __attribute__((aligned(4)));

typedef struct
{
    T_IR_LEARN_STATE    ir_learn_state;
    T_IR_LEARN_RESULT   ir_learn_result;
    T_IR_LEARN_PARA     *p_ir_learn_data;
} T_IR_LEARN_STRUCT;
/** End of IR_LEARN_Exported_Types
    * @}
    */

#if IR_LEARN_DUTY_CYCLE_SUPPORT
T_IR_LEARN_RESULT ir_learn_decode_duty_cycle(T_IR_LEARN_PARA *p_ir_learn_para);
#endif
void ir_learn_convert_data(T_IR_LEARN_PARA *p_ir_learn_para);
T_IR_LEARN_RESULT ir_learn_freq(T_IR_LEARN_PARA *p_ir_learn_para);
T_IR_LEARN_RESULT ir_learn_decode(T_IR_LEARN_PARA *p_ir_learn_para);
bool ir_learn_is_working(void);
bool ir_learn_check_dlps(void);
void ir_learn_enter_dlps_config(void);
void ir_learn_exit_dlps_config(void);
T_IR_LEARN_STATE ir_learn_get_current_state(void);
void ir_learn_module_init(T_IR_LEARN_PARA *p_ir_learn_para);
void ir_learn_start(void);
void ir_learn_stop(void);
void ir_learn_exit(void);

#ifdef __cplusplus
}
#endif

#endif /*__IR_LEARN_H*/


/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

