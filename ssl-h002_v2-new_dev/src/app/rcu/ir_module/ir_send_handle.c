/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file         ir_send_handle.c
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
#include "board.h"
#include "trace.h"
#include "string.h"
#include "rtl876x_ir.h"
#include "app_msg.h"
#include "ir_protocol_nec.h"
#include "ir_send_driver.h"
#include "ir_send_handle.h"
#include "os_timer.h"
#include "key_handle.h"
#include "battery_driver.h"
#if SUPPORT_IR_LEARN_FEATURE
#include "ir_learn_handle.h"
#endif

#if SUPPORT_IR_TX_FEATURE

/*============================================================================*
 *                          Local Variables
 *============================================================================*/
static T_IR_SEND_KEY_STATE ir_send_key_state = IR_SEND_KEY_RELEASE;
extern TimerHandle_t ir_power_key_send_timer;
static int ir_send_power_num = 0;
bool ir_power_key_send_flag = 0;    //发送红外power键键值标志位
static T_IR_SEND_PARA ir_send_parameters;

/*============================================================================*
 *                          Local Functions
 *============================================================================*/
/******************************************************************
* @brief   encode ir send command data
* @param   T_IR_PROTOCOL protocol
* @param   uint8_t ir_key_command
* @param   T_IR_SEND_PARA *p_ir_send_parameters
* @return  result
* @retval  T_IRDA_RET
*/
static T_IRDA_RET ir_send_command_encode(T_IR_PROTOCOL ir_protocol, uint8_t ir_key_command,
                                         T_IR_SEND_PARA *p_ir_send_parameters)
{
    T_IRDA_RET ret = IRDA_SUCCEED;

    switch (ir_protocol)
    {
    case NEC_PROTOCOL:
        ret = ir_protocol_nec_command_encode(ir_key_command, p_ir_send_parameters);
        break;
    default:
        break;
    }

    return ret;
}

/******************************************************************
* @brief   encode ir send repeat code data
* @param   T_IR_PROTOCOL protocol
* @param   uint8_t ir_key_command
* @param   T_IR_SEND_PARA *p_ir_send_parameters
* @return  result
* @retval  T_IRDA_RET
*/
static T_IRDA_RET ir_send_repeat_code_encode(T_IR_PROTOCOL ir_protocol, uint8_t ir_key_command,
                                             T_IR_SEND_PARA *p_ir_send_parameters)
{
    T_IRDA_RET ret = IRDA_SUCCEED;

    switch (ir_protocol)
    {
    case NEC_PROTOCOL:
        ret = ir_protocol_nec_repeat_code_encode(p_ir_send_parameters);
        break;
    default:
        break;
    }

    return ret;
}

/*============================================================================*
 *                          Global Functions
 *============================================================================*/
/******************************************************************
 * @brief   IR app send message process.
 * @param   msg_sub_type - message subtype
 * @return  none
 * @retval  void
 */
void ir_send_msg_proc(uint16_t msg_sub_type)
{
    APP_PRINT_INFO2("[ir_send_msg_proc] msg_sub_type is %d, ir_send_get_current_state is %d",
                    msg_sub_type, ir_send_get_current_state());

    if (msg_sub_type == IO_MSG_TYPE_IR_START_SEND_REPEAT_CODE)
    {
        if (ir_send_get_current_state() == IR_SEND_CAMMAND_COMPLETE ||
            ir_send_get_current_state() == IR_SEND_REPEAT_CODE_COMPLETE)
        {
            if (false == ir_send_repeat_code_start())
            {
                ir_send_exit();
            }
        }
        else
        {
            ir_send_exit();
        }
    }
    else if (msg_sub_type == IO_MSG_TYPE_IR_SEND_COMPLETE)
    {
        if (ir_send_key_state == IR_SEND_KEY_RELEASE)
        {
            ir_send_exit();
        }
    }
}

/******************************************************************
 * @brief   Application code for IR send key press handle.
 * @param   T_KEY_INDEX_DEF ir_send_key_index
 * @return  none
 * @retval  void
 */
void ir_send_key_press_handle(T_KEY_INDEX_DEF ir_send_key_index)
{
    APP_PRINT_INFO0("[IR] ir send key press.");
    ir_send_key_state = IR_SEND_KEY_PRESS;

#if SUPPORT_IR_LEARN_FEATURE
    if (ir_learn_is_working())
    {
        APP_PRINT_INFO0("[IR] IR learn is working, can't start ir send.");
        return;
    }
#endif
    memset(&ir_send_parameters, 0, sizeof(T_IR_SEND_PARA));

#if SUPPORT_IR_LEARN_FEATURE
    if (false == ir_get_learned_wave_data(ir_send_key_index, &ir_send_parameters))
    {
        uint8_t ir_code = key_handle_get_ir_key_code_by_index(ir_send_key_index);
        ir_send_command_encode(NEC_PROTOCOL, ir_code, &ir_send_parameters);
        ir_send_repeat_code_encode(NEC_PROTOCOL, ir_code, &ir_send_parameters);
    }
#else
    uint8_t ir_code = key_handle_get_ir_key_code_by_index(ir_send_key_index);
    ir_send_command_encode(NEC_PROTOCOL, ir_code, &ir_send_parameters);
    if (ir_send_key_index == VK_POWER)
    {
        ir_power_key_send_flag = 1;
        if (ir_send_power_num == 0)
        {
            os_timer_start(&ir_power_key_send_timer);
        }
    }
    else if (ir_send_key_index == VK_PAIR)  //发送红外配对按键时不发重复码
    {
    }
    else
    {
        ir_send_repeat_code_encode(NEC_PROTOCOL, ir_code, &ir_send_parameters);
    }
#endif
    if (true == ir_send_module_init(&ir_send_parameters))
    {
        if (ir_send_power_num != 0 || ir_send_key_index != VK_POWER)
        {
            ir_send_command_start();
        }
    }
}

/******************************************************************
 * @brief   Application code for IR send key release handle.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_send_key_release_handle(void)
{
    APP_PRINT_INFO0("[IR] ir send key release.");
    ir_send_key_state = IR_SEND_KEY_RELEASE;

#if SUPPORT_IR_LEARN_FEATURE
    if (ir_learn_is_working())
    {
        return;
    }
#endif
    if (ir_send_is_working() == false)
    {
        return;
    }

    if (ir_send_get_current_state() != IR_SEND_CAMMAND &&
        ir_send_get_current_state() != IR_SEND_REPEAT_CODE)
    {
        ir_send_exit();
    }
}

void ir_power_key_send_timer_cb(TimerHandle_t p_timer)
{
    if (ir_send_power_num <= 5)
    {
        ir_send_key_press_handle(VK_POWER);
        ir_send_power_num ++;
    }
    else
    {
        os_timer_stop(&ir_power_key_send_timer);
        if (BAT_MODE_LOW_POWER != bat_get_current_mode())
        {
            ir_power_key_send_flag = 0;
        }
        ir_send_power_num = 0;
    }
}

#endif /*end Micro @SUPPORT_IR_TX_FEATURE*/


/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

