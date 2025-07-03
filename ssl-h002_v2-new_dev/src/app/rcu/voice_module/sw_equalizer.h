/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     sw_equalizer.h
* @brief
* @details
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _SW_EQUALIZER_H_
#define _SW_EQUALIZER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include "app_section.h"

#if SUPPORT_SW_EQ

/*============================================================================*
 *                              Macros
 *============================================================================*/
#define SW_EQ_MAX_STAGE         5
#define EQ_CH_Cmd_ENABLE        1
#define EQ_CH_Cmd_DISABLE       0

/*============================================================================*
 *                              Types
 *============================================================================*/
typedef struct t_biquad
{
    int32_t a1;
    int32_t a2;
    int32_t b0;
    int32_t b1;
    int32_t b2;
    int16_t prev_input_1;
    int16_t prev_input_2;
    int16_t prev_output_1;
    int16_t prev_output_2;
} T_BIQUAD;

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
extern int32_t SW_EQ_PARAMS_TABLE[SW_EQ_MAX_STAGE][6];

/*============================================================================*
 *                         Functions
 *============================================================================*/
void bq_init(void) DATA_RAM_FUNCTION;
void bq_frame_process(int16_t *p_input_data, uint16_t size) DATA_RAM_FUNCTION;

#endif

#endif

/******************* (C) COPYRIGHT 2022 Realtek Semiconductor Corporation *****END OF FILE****/

