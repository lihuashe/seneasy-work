/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     sw_equalizer.c
* @brief
* @details
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "sw_equalizer.h"
#include "trace.h"

#if SUPPORT_SW_EQ

/*============================================================================*
 *                                 Macros
 *============================================================================*/
#define EQ_FACTOR  15
#define EQ_ROUND   16384  /* The coefficients used for rounding 0.5 *(2 ^15) = 16384 */

/*============================================================================*
*                              Global Variables
*============================================================================*/
int32_t SW_EQ_PARAMS_TABLE[SW_EQ_MAX_STAGE][6] =
{
    /* CODEC_EQChCmd,   CODEC_EQCoefH0,  CODEC_EQCoefB1, CODEC_EQCoefB2, CODEC_EQCoefA1, CODEC_EQCoefA2 */
    {EQ_CH_Cmd_DISABLE,     0x00000000,  0x00000000,  0x00000000,  0x00000000,  0x00000000},
    {EQ_CH_Cmd_DISABLE,     0x00000000,  0x00000000,  0x00000000,  0x00000000,  0x00000000},
    {EQ_CH_Cmd_DISABLE,     0x00000000,  0x00000000,  0x00000000,  0x00000000,  0x00000000},
    {EQ_CH_Cmd_DISABLE,     0x00000000,  0x00000000,  0x00000000,  0x00000000,  0x00000000},
    {EQ_CH_Cmd_DISABLE,     0x00000000,  0x00000000,  0x00000000,  0x00000000,  0x00000000},
};

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
static T_BIQUAD bq[SW_EQ_MAX_STAGE];
static uint8_t sw_eq_stage;

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/******************************************************************
 * @brief   init biquad parameters.
 * @param   none
 * @return  none
 * @retval  void
 */
void bq_init(void)
{
    uint8_t i;
    sw_eq_stage = 0;
    for (i = 0; i < SW_EQ_MAX_STAGE && SW_EQ_PARAMS_TABLE[i][0] == EQ_CH_Cmd_ENABLE; i++)
    {
        sw_eq_stage = i + 1;
    }
    APP_PRINT_INFO1("[sw equalizer parameters init]sw_eq_stage=%d", sw_eq_stage);
    for (i = 0; i < sw_eq_stage; i++)
    {
        bq[i].b0 = SW_EQ_PARAMS_TABLE[i][1] ;
        bq[i].b1 = (SW_EQ_PARAMS_TABLE[i][2] * SW_EQ_PARAMS_TABLE[i][1] + EQ_ROUND) >> EQ_FACTOR;
        bq[i].b2 = (SW_EQ_PARAMS_TABLE[i][3] * SW_EQ_PARAMS_TABLE[i][1] + EQ_ROUND) >> EQ_FACTOR;
        bq[i].a1 = SW_EQ_PARAMS_TABLE[i][4] ;
        bq[i].a2 = SW_EQ_PARAMS_TABLE[i][5] ;
        bq[i].prev_input_1 = 0;
        bq[i].prev_input_2 = 0;
        bq[i].prev_output_1 = 0;
        bq[i].prev_output_2 = 0;
    }
}

/******************************************************************
 * @brief   a frame of voice data biquad process.
 * @param   p_input_data - point of input data
 * @param   size - input data size
 * @return  none
 * @retval  void
 */
void bq_frame_process(int16_t *p_input_data, uint16_t size)
{
    int16_t input_data;
    int16_t output_data;
    int32_t output;
    for (uint8_t stage = 0; stage < sw_eq_stage; stage++)
    {
        for (uint16_t i = 0; i < size; i++)
        {
            input_data = p_input_data[i];
            output = (bq[stage].b0 * input_data) +
                     (bq[stage].b1 * bq[stage].prev_input_1) +
                     (bq[stage].b2 * bq[stage].prev_input_2) +
                     (bq[stage].a1 * bq[stage].prev_output_1) +
                     (bq[stage].a2 * bq[stage].prev_output_2);
            output_data = (output + EQ_ROUND) >> EQ_FACTOR;
            bq[stage].prev_input_2 = bq[stage].prev_input_1;
            bq[stage].prev_input_1 = input_data;
            bq[stage].prev_output_2 = bq[stage].prev_output_1;
            bq[stage].prev_output_1 = output_data;
            p_input_data[i] = output_data;
        }
    }
}

#endif
/******************* (C) COPYRIGHT 2022 Realtek Semiconductor Corporation *****END OF FILE****/

