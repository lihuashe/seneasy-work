/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file         ir_learn_handle.c
* @brief        This file provides application layer application code of using IR learin driver.
* @details
* @author       barry_bian
* @date         2020-04-22
* @version      v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include "ftl.h"
#include "swtimer.h"
#include "os_timer.h"
#include "app_msg.h"
#include "led_driver.h"
#include "ir_learn_handle.h"
#include "app_section.h"

#if (SUPPORT_IR_TX_FEATURE && SUPPORT_IR_LEARN_FEATURE)

/*============================================================================*
 *                          Local Variables
 *============================================================================*/
static T_IR_LEARN_KEY_STATE ir_learn_key_state = IR_LEARN_KEY_RELEASE;
static T_IR_LEARN_PARA ir_learn_data;
static TimerHandle_t ir_learn_timer = NULL;
static T_IR_LEARN_STORAGE_INFO ir_learn_storage_info;
static T_IR_LEARN_KEY_INFO ir_learn_table[IR_LEARN_MAX_KEY_NUM] =
{
    {0, VK_TV_POWER},
    {1, VK_TV_SIGNAL},
    {2, VK_POWER},
};

/*============================================================================*
 *                         Functions Declaration
 *============================================================================*/
static uint32_t ir_learn_get_key_offset(T_KEY_INDEX_DEF ir_learn_key_index);
static bool ir_learn_set_storaged_data(T_IR_LEARN_STORAGE_INFO *p_ir_learn_storage_info,
                                       T_IR_LEARN_PARA *p_ir_learn_data);
static bool ir_learn_storage_wave_data(T_IR_LEARN_STORAGE_INFO *p_ir_learn_storage_info);

static void ir_learn_timer_callback(TimerHandle_t pxTimer) DATA_RAM_FUNCTION;
void ir_learn_msg_proc(uint16_t msg_sub_type) DATA_RAM_FUNCTION;

/*============================================================================*
 *                         Local Functions
 *============================================================================*/
/******************************************************************
 * @brief   ir learn timer callback.
 * @param   pxTimer
 * @return  none
 * @retval  void
 */
void ir_learn_timer_callback(TimerHandle_t pxTimer)
{
    APP_PRINT_INFO0("[IR] ir learn time out");
    ir_learn_deinit();
}

/******************************************************************
 * @brief   get ir learn key offset.
 * @param   ir_learn_key_index
 * @return  ir learn key offset
 * @retval  uint32_t
 */
uint32_t ir_learn_get_key_offset(T_KEY_INDEX_DEF ir_learn_key_index)
{
    for (uint8_t i = 0; i < IR_LEARN_MAX_KEY_NUM; i++)
    {
        if (ir_learn_table[i].key_index == ir_learn_key_index)
        {
            return ir_learn_table[i].offset;
        }
    }
    return INVALID_OFFSET;
}

/******************************************************************
 * @brief   set ir learn storaged data.
 * @param   p_ir_learn_storage_info
 * @param   p_ir_learn_data
 * @return  result
 * @retval  true or false
 */
bool ir_learn_set_storaged_data(T_IR_LEARN_STORAGE_INFO *p_ir_learn_storage_info,
                                T_IR_LEARN_PARA *p_ir_learn_data)
{
    APP_PRINT_INFO1("[IR] ir learn carrier frequency is %d Hz",
                    (uint32_t)p_ir_learn_data->carrier_freq);
    APP_PRINT_INFO1("[IR] ir learn duty cycle is %d percent",
                    (uint32_t)(100 / p_ir_learn_data->duty_cycle));
    APP_PRINT_INFO1("[IR] ir learn wave data len is %d", p_ir_learn_data->ir_learn_buf_index);

    if (p_ir_learn_data->carrier_freq < 5000 || p_ir_learn_data->carrier_freq > 2000000 ||
        p_ir_learn_data->ir_learn_buf_index == 0 ||
        p_ir_learn_data->ir_learn_buf_index > IR_LEARN_WAVE_MAX_LEN ||
        p_ir_learn_data->duty_cycle <= 0)
    {
        return false;
    }

    p_ir_learn_storage_info->frequency = p_ir_learn_data->carrier_freq;
    p_ir_learn_storage_info->duty_cycle = p_ir_learn_data->duty_cycle;
    p_ir_learn_storage_info->buf_len = p_ir_learn_data->ir_learn_buf_index;
    memcpy(p_ir_learn_storage_info->ir_learn_storage_buf, p_ir_learn_data->ir_learn_buffer,
           IR_LEARN_WAVE_MAX_LEN * sizeof(uint32_t));
    p_ir_learn_storage_info->valid_flag = VALID_FLAG;
    return true;
}

/******************************************************************
 * @brief   save ir learn data to FTL section.
 * @param   ir_learn_storage_info
 * @return  result
 * @retval  true or false
 */
bool ir_learn_storage_wave_data(T_IR_LEARN_STORAGE_INFO *p_ir_learn_storage_info)
{
    if (0 == ftl_save(p_ir_learn_storage_info,
                      IR_WAVE_DATA_BASE_ADDR + p_ir_learn_storage_info->offset * sizeof(T_IR_LEARN_STORAGE_INFO),
                      sizeof(T_IR_LEARN_STORAGE_INFO)))
    {
        return true;
    }
    else
    {
        APP_PRINT_INFO0("[IR] ERR: IR learn storage wave data fail.");
        return false;
    }
}

/*============================================================================*
 *                         Global Functions
 *============================================================================*/
/******************************************************************
 * @brief   get ir learn wave data.
 * @param   ir_send_key_index
 * @param   p_ir_send_para
 * @return  get data successfully or not
 * @retval  true or false
 */
bool ir_get_learned_wave_data(T_KEY_INDEX_DEF ir_send_key_index, T_IR_SEND_PARA *p_ir_send_para)
{
    T_IR_LEARN_STORAGE_INFO ir_learn_ftl_dada;

    uint8_t offset = ir_learn_get_key_offset(ir_send_key_index);

    if (offset != INVALID_OFFSET)
    {
        APP_PRINT_INFO2("[IR] IR key index: %d, offset: %d", ir_send_key_index, offset);
        APP_PRINT_INFO1("[IR] sizeof(T_IR_LEARN_STORAGE_INFO): %d", sizeof(T_IR_LEARN_STORAGE_INFO));
        APP_PRINT_INFO1("[IR] ir learn data save adress: 0x%x",
                        IR_WAVE_DATA_BASE_ADDR + offset * sizeof(T_IR_LEARN_STORAGE_INFO));
        if (0 == ftl_load(&ir_learn_ftl_dada,
                          IR_WAVE_DATA_BASE_ADDR + offset * sizeof(T_IR_LEARN_STORAGE_INFO), sizeof(T_IR_LEARN_STORAGE_INFO)))
        {
            APP_PRINT_INFO1("[IR] get ftl wave data, valid flag is 0x%x", ir_learn_ftl_dada.valid_flag);
            APP_PRINT_INFO1("[IR] get ftl wave data, offest is %d", ir_learn_ftl_dada.offset);
            APP_PRINT_INFO1("[IR] get ftl wave data, carrier frequency is %d Hz",
                            (uint32_t)ir_learn_ftl_dada.frequency);
            APP_PRINT_INFO1("[IR] get ftl wave data, duty cycle is %d percent",
                            (uint32_t)(100 / ir_learn_ftl_dada.duty_cycle));
            APP_PRINT_INFO1("[IR] get ftl wave data, data len is %d", ir_learn_ftl_dada.buf_len);

            if (ir_learn_ftl_dada.valid_flag == VALID_FLAG &&
                ir_learn_ftl_dada.offset == offset &&
                ir_learn_ftl_dada.key_index == ir_send_key_index &&
                ir_learn_ftl_dada.duty_cycle > 0 &&
                ir_learn_ftl_dada.buf_len > 0 && ir_learn_ftl_dada.buf_len <= IR_LEARN_WAVE_MAX_LEN &&
                ir_learn_ftl_dada.frequency >= 5000 && ir_learn_ftl_dada.frequency <= 2000000)
            {
                p_ir_send_para->carrier_frequency_hz = ir_learn_ftl_dada.frequency;
                p_ir_send_para->duty_cycle = ir_learn_ftl_dada.duty_cycle;
                p_ir_send_para->send_buf_len = ir_learn_ftl_dada.buf_len;
                p_ir_send_para->repeat_buf_len = 0;

                memcpy(p_ir_send_para->ir_send_buf, &ir_learn_ftl_dada.ir_learn_storage_buf,
                       IR_LEARN_WAVE_MAX_LEN * sizeof(uint32_t));

                APP_PRINT_INFO0("[IR] get IR learn wave data success");
                return true;
            }
        }
    }
    APP_PRINT_INFO0("[IR] ERR: get IR learn wave data err");
    return false;
}

/******************************************************************
 * @brief   Application code for IR learn data process.
 * @param   msg_sub_type - IR sub type msg.
 * @return  none
 * @retval  void
 */
void ir_learn_msg_proc(uint16_t msg_sub_type)
{
    T_IR_LEARN_RESULT status = IR_LEARN_OK;

    if (msg_sub_type == IO_MSG_TYPE_IR_LEARN_DATA)
    {
        /* Decode IR waveform data and duty cycle*/
        status = ir_learn_decode(&ir_learn_data);

        if (status != IR_LEARN_OK)
        {
            APP_PRINT_ERROR1("[IR] ERR: IR learn err code is %d.", status);
            ir_learn_stop();
        }

        APP_PRINT_INFO0("[IR] IR learn data process ok.");
    }
    else if (msg_sub_type == IO_MSG_TYPE_IR_LEARN_STOP)
    {
        /* pick up the last ir data */
        status = ir_learn_decode(&ir_learn_data);
        if (status == IR_LEARN_EXIT)
        {
            /* decode IR carrier freqency */
            ir_learn_freq(&ir_learn_data);
            /* convert data*/
            ir_learn_convert_data(&ir_learn_data);
            /* set ir learn data to storage*/
            if (true == ir_learn_set_storaged_data(&ir_learn_storage_info, &ir_learn_data))
            {
                /* storage ir learn valid wave */
                if (true == ir_learn_storage_wave_data(&ir_learn_storage_info))
                {
                    APP_PRINT_INFO0("[IR] IR learn wave data is storaged successfully.");
                }
            }
        }
        else
        {
            APP_PRINT_ERROR1("[IR] ERR: IR learn err code is %d.", status);
        }

#if SUPPORT_LED_INDICATION_FEATURE
        LED_BLINK(LED_1, LED_TYPE_BLINK_IR_LEARN_SUCCESS, 5);
#endif
        ir_learn_stop();
    }
}

/******************************************************************
 * @brief   Application code for IR learn init.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_init(void)
{
    APP_PRINT_INFO0("[IR] ir learn init.");
    if (ir_learn_timer == NULL)
    {
        /*ir_learn_timer is used for ir learn*/
        if (false == os_timer_create(&ir_learn_timer, "ir_learn_timer",  1, \
                                     IR_LEARN_TIMEOUT, false, ir_learn_timer_callback))
        {
            APP_PRINT_ERROR0("[IR] ir_learn_timer creat failed!");
        }
        else
        {
            APP_PRINT_ERROR0("[IR] ir_learn_timer creat success!");
            os_timer_start(&ir_learn_timer);
        }
    }
    else
    {
        os_timer_restart(&ir_learn_timer, IR_LEARN_TIMEOUT);
    }

    ir_learn_module_init(&ir_learn_data);

#if SUPPORT_LED_INDICATION_FEATURE
    LED_BLINK(LED_1, LED_TYPE_BLINK_IR_LEARN_MODE, 0);
#endif
}

/******************************************************************
 * @brief   Application code for IR learn deinit.
 * @param   none
 * @return  none
 * @retval  void
 */
void ir_learn_deinit(void)
{
#if SUPPORT_LED_INDICATION_FEATURE
    LED_BLINK_EXIT(LED_1, LED_TYPE_BLINK_IR_LEARN_MODE);
    LED_BLINK_EXIT(LED_1, LED_TYPE_BLINK_IR_LEARN_WAITING);
#endif

    ir_learn_exit();
}

/******************************************************************
 * @brief   Application code for IR learn key press handle.
 * @param   ir_learn_key_index
 * @return  result
 * @retval  ture or false
 */
bool ir_learn_press_handle(T_KEY_INDEX_DEF ir_learn_key_index)
{
    if (ir_learn_get_current_state() != IR_LEARN_READY)
    {
        APP_PRINT_INFO0("[IR] Err: not in ir learn ready state.");
        return false;
    }

    if (ir_learn_key_state == IR_LEARN_KEY_PRESS)
    {
        APP_PRINT_INFO0("[IR] Err: ir learn key is not release.");
        return false;
    }

    if (true == os_timer_restart(&ir_learn_timer, IR_LEARN_TIMEOUT))
    {
        APP_PRINT_INFO0("[IR] ir_learn_timer restart success!");
    }
    else
    {
        APP_PRINT_ERROR0("[IR] ir_learn_timer restart failed!");
        return false;
    }

    memset(&ir_learn_storage_info, 0, sizeof(T_IR_LEARN_STORAGE_INFO));
    ir_learn_storage_info.key_index = ir_learn_key_index;
    ir_learn_storage_info.offset = ir_learn_get_key_offset(ir_learn_key_index);
    if (ir_learn_storage_info.offset == INVALID_OFFSET)
    {
        APP_PRINT_INFO0("[IR] Err: this key is not in ir_learn_table.");
        return false;
    }
    else
    {
        APP_PRINT_INFO1("[IR] ir key offset is %d.", ir_learn_storage_info.offset);
    }

    APP_PRINT_INFO0("[IR] ir learn key press.");
#if SUPPORT_LED_INDICATION_FEATURE
    LED_BLINK(LED_1, LED_TYPE_BLINK_IR_LEARN_WAITING, 0);
#endif

    ir_learn_key_state = IR_LEARN_KEY_PRESS;
    ir_learn_start();
    return true;
}

/******************************************************************
 * @brief   Application code for IR learn key release handle.
 * @param   none
 * @return  result
 * @retval  ture or false
 */
bool ir_learn_key_release_handle(void)
{
#if SUPPORT_LED_INDICATION_FEATURE
    LED_BLINK_EXIT(LED_1, LED_TYPE_BLINK_IR_LEARN_WAITING);
#endif

    if (ir_learn_key_state == IR_LEARN_KEY_RELEASE)
    {
        APP_PRINT_INFO0("[IR] ir learn key is not pressed.");
        return false;
    }
    APP_PRINT_INFO0("[IR] ir learn key release.");
    ir_learn_key_state = IR_LEARN_KEY_RELEASE;
    ir_learn_stop();

    return true;
}

/******************************************************************
 * @brief   Application code for IR learn key release handle.
 * @param   none
 * @return  result
 * @retval  ture or false
 */
bool ir_learn_reset_ftl_learning_data(void)
{
    bool result = true;
    uint32_t reset_value = 0;

    for (uint8_t index = 0; index < IR_LEARN_MAX_KEY_NUM; index++)
    {
        if (0 != ftl_save(&reset_value, IR_WAVE_DATA_BASE_ADDR + index * sizeof(T_IR_LEARN_STORAGE_INFO),
                          sizeof(reset_value)))
        {
            result = false;
        }
    }

    APP_PRINT_INFO1("[ir_learn_reset_ftl_learning_data] result %d", result);

    return result;
}

#endif /*end Micro SUPPORT_IR_LEARN_FEATURE */


/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

