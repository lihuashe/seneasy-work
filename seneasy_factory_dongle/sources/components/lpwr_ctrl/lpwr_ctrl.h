/**
 * @file     lpwr_ctrl.h
 * @version  V1.0
 * @date     2021/02/18
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __LPWR_CTRL_H__
#define __LPWR_CTRL_H__

/**
 * @brief  PMU LUT Configuration.
 */
#define LUT_INDEX_CP_SWD (0)
#define LUT_INDEX_MP_SWD (1)

#define LUT_CP_SWD_ACT   (LUT_ACT_PD_SYS_ON | LUT_ACT_CP_IRQ_EN)
#define LUT_MP_SWD_ACT   (LUT_ACT_PD_SYS_ON | LUT_ACT_MP_IRQ_EN)

/**
 * @brief  Low power control callback functions definition.
 */
typedef bool (*lpwr_ctrl_before_sleep_cb)(void);
typedef void (*lpwr_ctrl_after_wakeuup_cb)(void);

typedef enum
{
    LPWR_MODE_ACTIVE      = 0x00,
    LPWR_MODE_IDLE        = 0x01,
    LPWR_MODE_SLEEP       = 0x02, /* SRAM keep retention, system clock will power down. */
    LPWR_MODE_DEEPSLEEP   = 0x03, /* SRAM will power down, LCLK clock will keep working(shutdown with 32K). */
    LPWR_MODE_HIBERNATION = 0x04, /* SRAM will power down, LCLK clock will turn off(shutdown without 32K). */

} EN_LPWR_MODE_SEL_T;

#define IS_VALID_LPWR_MODE_SEL(SEL) ((SEL) <= LPWR_MODE_HIBERNATION)

/**
 * @brief  Get ble reamin sleep time.
 * @return Reamin sleep time, us.
 */
extern uint32_t lpwr_ctrl_get_ble_remain_sleep_time_us(void);

/**
 * @brief  Configure system low power mode
 * @param  enMode: System work mode, @ref EN_LPWR_MODE_SEL_T.
 * @return
 */
extern EN_ERR_STA_T lpwr_ctrl_set_low_power_mode(EN_LPWR_MODE_SEL_T enMode);

/**
 * @brief  Low Power Application Initialize
 *         Configure System Sleep Mode to PMU_SLEEP;
 *         Configure SWD Power Domain Controller(Look-Up Table)
 * @param  enMode: System work mode, @ref EN_LPWR_MODE_SEL_T.
 * @param  pfnBeforeSleepCb: Before sleep callback function.
 * @param  pfnAfterWakeupCb: After wakeup callback function.
 */
extern void lpwr_ctrl_init(EN_LPWR_MODE_SEL_T enMode, lpwr_ctrl_before_sleep_cb pfnBeforeSleepCb,
                           lpwr_ctrl_after_wakeuup_cb pfnAfterWakeupCb);

/**
 * @brief  System goto sleep function.
 */
extern void lpwr_ctrl_goto_sleep(void);

#endif /* __LPWR_CTRL_H__ */
