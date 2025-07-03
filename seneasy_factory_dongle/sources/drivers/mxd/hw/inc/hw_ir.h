/**********************************************************************************************************************
 * @file     hw_ir.h
 * @version  V1.1
 * @date     2022/1/19
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
 *********************************************************************************************************************/

#ifndef __HW_IR_H__
#define __HW_IR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */




/**
 * @brief  Set IR send signal path.
 * @param  pstIR: IR handle, should be IR0 / IR1 / IR2 / IR3.
 * @param  enPath: IR send signal path, @ref EN_IR_SEND_PATH_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ir_set_send_path(stTIMER_Handle_t* pstIR, EN_IR_SEND_PATH_T enPath);


/**
 * @brief  Set IR output current config compare value.
 * @param  pstIR: IR handle,Should be IR0 / IR1 / IR2 / IR3.
 * @param  u16Compare: Compare value.
 * @param  enPol: PWM polarity, @ref EN_PWM_POL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ir_set_pwm_current_compare_and_polarity(stTIMER_Handle_t* pstIR, uint16_t u16Compare,
                                                                   EN_PWM_POL_T enPol);


/**
 * @brief  Set IR output next period config compare value.
 * @param  pstIR: IR handle, Should be IR0 / IR1 / IR2 / IR3.
 * @param  u16Compare: Compare value.
 * @param  u16HiCnt: Compare high counter.
 * @param  u16LoCnt: Compare low counter.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ir_pwm_set_next_compare(stTIMER_Handle_t* pstIR, uint16_t u16Compare,
                                                   uint16_t u16LoCnt, uint16_t u16HiCnt);


/**
 * @brief  Enable IR function.
 * @param  pstIR: IR handle, should be IR0 / IR1 / IR2 / IR3.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ir_enable(stTIMER_Handle_t* pstIR);


/**
 * @brief  Disable IR function.
 * @param  pstIR: IR handle, should be IR0 / IR1 / IR2 / IR3.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ir_disable(stTIMER_Handle_t* pstIR);


/**
 * @brief  Set IR polarity.
 * @param  pstIR: IR handle, should be IR0 / IR1 / IR2 / IR3.
 * @param  enPol: IR polarity, @ref EN_IR_POL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ir_set_polarity(stTIMER_Handle_t* pstIR, EN_IR_POL_T enPol);


/**
 * @brief  Enable IR channel.
 * @param  pstIR: IR handle, Should be IR0 / IR1 / IR2 / IR3.
 * @param  enCh: IR channel @ref EN_IR_CH_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ir_enable_channel(stTIMER_Handle_t* pstIR, EN_IR_CH_T enCh);


/**
 * @brief  Disable IR channel.
 * @param  pstIR: IR handle, Should be IR0 / IR1 / IR2 / IR3.
 * @param  enCh: IR channel @ref EN_IR_CH_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ir_disable_channel(stTIMER_Handle_t* pstIR, EN_IR_CH_T enCh);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HW_IR_H__ */


