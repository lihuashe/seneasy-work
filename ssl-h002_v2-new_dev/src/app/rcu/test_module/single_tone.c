/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      single_tone.c
* @brief     single tone test implementation.
* @details   none.
* @author    Chenjie Jin
* @date      2020-03-03
* @version   v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *                                  Header Files
 *============================================================================*/
#include "board.h"
#include "single_tone.h"
#include "trace.h"
#include "os_mem.h"
#include "os_task.h"
#include "os_sched.h"
#include "os_timer.h"
#include "rtl876x_wdg.h"
#include <app_section.h>

#if MP_TEST_MODE_SUPPORT_SINGLE_TONE_TEST

#endif
/*============================================================================*
 *                                  Macros
 *============================================================================*/
/* open EXIT_SINGLE_TONE_TEST_WHEN_TIMEOUT to enable the function of exiting Single Tone Test when Timeout */
#define EXIT_SINGLE_TONE_TEST_WHEN_TIMEOUT  1

#if EXIT_SINGLE_TONE_TEST_WHEN_TIMEOUT
#define EXIT_SINGLE_TONE_TIME    (2*60*1000)    /* 2min */
#endif

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
typedef void *TimerHandle_t;
#if EXIT_SINGLE_TONE_TEST_WHEN_TIMEOUT
static TimerHandle_t single_tone_exit_timer = NULL;
#endif
static bool single_tone_is_sent_start_cmd = false;
static bool single_tone_is_sent_restart_cmd = false;
static void *single_tone_task_handle;

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void single_tone_reset(uint8_t channel_num);
static void single_tone_start(uint8_t channel_num);
static bool single_tone_handle_hci_cb(T_SINGLE_TONE_EVT evt, bool status, uint8_t *p_buf,
                                      uint32_t len);
static void single_tone_task(void *p_param);
#if EXIT_SINGLE_TONE_TEST_WHEN_TIMEOUT
static void single_tone_exit_timeout_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
#endif

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief   reset singletone.
 * @param   channel_num - channel of singletone.
 * @return  none
 * @retval  void
 */
void single_tone_reset(uint8_t channel_num)
{
    T_SINGLE_TONE_VEND_CMD_PARAMS *p_vend_cmd_params = os_mem_alloc(RAM_TYPE_DATA_ON,
                                                                    sizeof(T_SINGLE_TONE_VEND_CMD_PARAMS));

    if (p_vend_cmd_params)
    {
        APP_PRINT_INFO0("Single Tone Reset Command!");

        p_vend_cmd_params->pkt_type = 1;
        p_vend_cmd_params->opcode = 0xfc78;
        p_vend_cmd_params->length = 4;
        p_vend_cmd_params->start = 0;
        p_vend_cmd_params->channle = channel_num;
        p_vend_cmd_params->tx_power = 6;
        p_vend_cmd_params->is_le = 1;

        hci_if_write((uint8_t *)p_vend_cmd_params, sizeof(T_SINGLE_TONE_VEND_CMD_PARAMS));

        single_tone_is_sent_restart_cmd = true;
    }
}

/******************************************************************
 * @brief   start singletone.
 * @param   channel_num - channel of singletone.
 * @return  none
 * @retval  void
 */
void single_tone_start(uint8_t channel_num)
{
    APP_PRINT_INFO0("Single Tone Start!");

    T_SINGLE_TONE_VEND_CMD_PARAMS *p_vend_cmd_params = os_mem_alloc(RAM_TYPE_DATA_ON,
                                                                    sizeof(T_SINGLE_TONE_VEND_CMD_PARAMS));

    if (p_vend_cmd_params)
    {
        p_vend_cmd_params->pkt_type = 1;
        p_vend_cmd_params->opcode = 0xfc78;
        p_vend_cmd_params->length = 4;
        p_vend_cmd_params->start = 1;
        p_vend_cmd_params->channle = 0x80 | channel_num;
        /** note:
          * tx_power config:
          * 0x00/(-20dBm), 0x60/0dBm, 0x90/3dBm,
          * 0xA0/4dBm, 0xD0/7.5dBm(only for rtl876x)
          */
        p_vend_cmd_params->tx_power = 0x60;
        p_vend_cmd_params->is_le = 1;

        hci_if_write((uint8_t *)p_vend_cmd_params, sizeof(T_SINGLE_TONE_VEND_CMD_PARAMS));

        single_tone_is_sent_start_cmd = true;
    }
}

/******************************************************************
 * @brief   HCI callback of single tone handler.
 * @param   evt - single tone event
 * @param   status
 * @param   p_buf - point to hci buf
 * @param   len - buf length
 * @return  result
 * @retval  true or false
 */
bool single_tone_handle_hci_cb(T_SINGLE_TONE_EVT evt, bool status, uint8_t *p_buf,
                               uint32_t len)
{
    bool result = true;
    uint8_t channel_num = 20; /* 2.442G */

    APP_PRINT_INFO1("[single_tone_handle_hci_cb] evt is %d", evt);

    switch (evt)
    {
    case SINGLE_TONE_EVT_OPENED:
        if (!single_tone_is_sent_restart_cmd)
        {
            single_tone_reset(channel_num);
        }
        break;

    case SINGLE_TONE_EVT_CLOSED:
        break;

    case SINGLE_TONE_EVT_DATA_IND:
        hci_if_confirm(p_buf);
        if (!single_tone_is_sent_start_cmd)
        {
            single_tone_start(channel_num);
        }
        break;

    case SINGLE_TONE_EVT_DATA_XMIT:
        os_mem_free(p_buf);
        break;

    case SINGLE_TONE_EVT_ERROR:
        break;

    default:
        break;
    }
    return (result);
}

/******************************************************************
 * @brief   single tone task
 * @param   p_param
 * @return  none
 * @retval  void
 */
void single_tone_task(void *p_param)
{
    os_delay(100);
    hci_if_open(single_tone_handle_hci_cb);

    while (1)
    {
        os_delay(1000);
    }
}

#if EXIT_SINGLE_TONE_TEST_WHEN_TIMEOUT
/******************************************************************
 * @brief   single tone exit timer callback
 * @param   p_timer
 * @return  none
 * @retval  void
 */
void single_tone_exit_timeout_cb(TimerHandle_t p_timer)
{
    WDG_SystemReset(RESET_ALL_EXCEPT_AON, SINGLE_TONE_TIMEOUT_RESET);
    while(hci_if_close() == 1);
    os_task_delete(single_tone_task_handle);
}
#endif

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/******************************************************************
 * @brief   single tone module init
 * @param   none
 * @return  none
 * @retval  void
 */
void single_tone_init(void)
{
    APP_PRINT_INFO0("Single Tone Init");

    if (true == os_timer_create(&single_tone_exit_timer, "single_tone_exit_timer",  1,
                                EXIT_SINGLE_TONE_TIME, false, single_tone_exit_timeout_cb))
    {
        os_timer_start(&single_tone_exit_timer);
    }

    os_task_create(&single_tone_task_handle, "single_tone", single_tone_task, 0, 512, 1);
}



void set_single_tone_init(uint16_t single_timeout)
{
    APP_PRINT_INFO0("Single Tone Init");
    if (single_tone_exit_timer == NULL)
    {
        if (true == os_timer_create(&single_tone_exit_timer, "single_tone_exit_timer",  1,
                                single_timeout, false, single_tone_exit_timeout_cb))
        {
            os_timer_start(&single_tone_exit_timer);
        }

        os_task_create(&single_tone_task_handle, "single_tone", single_tone_task, 0, 512, 1);
    }
          
}


/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/
