/**
 * @file     sw_timer.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/11/1
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

#ifndef __SW_TIMER_H__
#define __SW_TIMER_H__

/**
 * @brief  Enumeration and struct definition.
 */
typedef enum
{
    SW_TIMER_MODE_ONCE,
    SW_TIMER_MODE_REPEAT

} EN_SW_TIMER_MODE_T;

#define IS_VALID_SW_TIMER_MODE(MODE) ((MODE) <= SW_TIMER_MODE_REPEAT)

typedef enum
{
    SW_TIMER_STATUS_IDLE    = 0,
    SW_TIMER_STATUS_READY   = 1,
    SW_TIMER_STATUS_RUNNING = 2,

} EN_SW_TIMER_STATUS_T;

typedef void (*sw_timer_timeout_handler_t)(void *pContext);

typedef struct stSwTimerNode
{
    uint32_t             u32CurrentTimeout;
    uint32_t             u32ChangedTimeout;
    uint32_t             u32Compare;
    uint32_t             u32Base;
    uint32_t             u32ChangedBase;
    bool                 bIsContextChanged;
    bool                 bIsStatusChanged;
    EN_SW_TIMER_STATUS_T enCurrentStatus;
    EN_SW_TIMER_STATUS_T enChangedStatus;
    EN_SW_TIMER_MODE_T   enMode;

    sw_timer_timeout_handler_t pfnHandler;

    void                 *pCurrentContext;
    void                 *pChangedContext;
    struct stSwTimerNode *pstNext;
    struct stSwTimerNode *pstLast;

} stSwTimerNode_t;

/**
 * @brief  Timer RTC interrupt handler.
 */
extern void timer_rtc_int_handlder(void);

/**
 * @brief  App timer initialize function, before use rtc timer, please call this function.
 * @return HW status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T sw_timer_init(void);

/**
 * @brief  Create a timer.
 * @param  pstTimer: If create the timer successed, this point will save the new timer id.
 * @param  enMode: Work mode, @ref EN_SW_TIMER_MODE_T.
 * @param  pfnHandler: Function to be executed when the timer expires.
 * @return HW status: @ref EN_ERR_STA_T
 */
extern EN_ERR_STA_T sw_timer_create(stSwTimerNode_t *pstTimer, EN_SW_TIMER_MODE_T enMode,
                                    sw_timer_timeout_handler_t pfnHandler);

/**
 * @brief  Start the timer.
 * @param  pstTimer: Point that the timer should be initialized.
 * @param  u32Time: timer out value unit ms, 0~131072000ms.
 * @param  pContext: point to the parameter when the timerout happened will send to the timerout handler.
 * @return HW status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T sw_timer_start(stSwTimerNode_t *pstTimer, uint32_t u32Time, void *pContext);

/**
 * @brief  Stop a specified timer.
 * @param  pstTimer: point to the timer will be stop.
 * @return HW status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T sw_timer_stop(stSwTimerNode_t *pstTimer);

/**
 * @brief  Stop one timer. after this, the timer can be restart.
 * @param  pstTimer: point to the timer will be stop.
 * @return HW status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T sw_timer_delete(stSwTimerNode_t *pstTimer);

#endif /* __SW_TIMER_H__ */
