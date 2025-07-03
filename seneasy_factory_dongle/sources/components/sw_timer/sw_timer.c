/**
 * @file     sw_timer.c
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_crg.h"
#include "hw_stim.h"
#include "hw_sys_ctrl.h"

#include "app_cfg.h"
#include "type_def.h"
#include "utility.h"

#include "sw_timer.h"

/**
 * @brief  Macro definition.
 */
#undef PRINTF
#define PRINTF(fmt, args...)

#if defined(__MAIN_PROCESSOR)
    #define STIM_IRQ                  (STIM0_IRQ3)
    #define STIM_CH                   (STIM_CH3)
    #define STIM_HANDLE               (STIM0)
    #define STIM_INT_MSK              (STIM_CH3_INT_MATCH)
    #define STIM_CLK_GATE             (CRG_STIM0_CLK_GATE)
    #define STIM_SYS_CTRL             (SYS_CTRL_MP)
    #define SW_TIMER_STIM_IRQ_HANDLER STIM0_IRQ3_Handler
#else
    #define STIM_IRQ                  (STIM1_IRQ3)
    #define STIM_CH                   (STIM_CH3)
    #define STIM_HANDLE               (STIM1)
    #define STIM_INT_MSK              (STIM_CH3_INT_MATCH)
    #define STIM_CLK_GATE             (CRG_STIM1_CLK_GATE)
    #define STIM_SYS_CTRL             (SYS_CTRL_CP)
    #define SW_TIMER_STIM_IRQ_HANDLER STIM1_IRQ3_Handler
#endif

/**
 * @brief  Local variables.
 */
static stSwTimerNode_t *m_stTimerHead;
static stSwTimerNode_t *m_stTimerTail;

/**
 * @brief  Timer compare.
 * @param  pstTimer: Timer node.
 * @param  u32BaseCnt.
 * @param  u32Ms.
 */
static void timer_compare(stSwTimerNode_t *pstTimer, uint32_t u32BaseCnt, uint32_t u32Ms)
{
    uint32_t u32Compare;
    u32Compare = u32BaseCnt + (TIME_MS_TO_32K_CNT(u32Ms));

    pstTimer->u32Base = u32BaseCnt;

    if (u32Compare > STIM_CNT_MAX)
    {
        u32Compare = u32Compare - STIM_CNT_MAX - 1;
    }

    pstTimer->u32Compare = u32Compare;
}

/**
 * @brief  Trigger STIM interrupt once.
 */
static void timer_trig_schedule(void)
{
    NVIC_SetPendingIRQ(STIM_IRQ);
}

/**
 * @brief  Timer configuration.
 * @param  pstTimer.
 */
static void timer_config(stSwTimerNode_t *pstTimer)
{
    bool     bIsContextChanged;
    bool     bIsStatusChanged;
    uint32_t u32Primask;
    uint32_t u32Temp;

    bIsContextChanged = pstTimer->bIsContextChanged;
    bIsStatusChanged  = pstTimer->bIsStatusChanged;

    if (bIsContextChanged)
    {
        u32Primask = __get_PRIMASK();
        __disable_irq();

        pstTimer->bIsContextChanged = false;
        pstTimer->u32CurrentTimeout = pstTimer->u32ChangedTimeout;
        pstTimer->pCurrentContext   = pstTimer->pChangedContext;

        __set_PRIMASK(u32Primask);
    }

    if (bIsStatusChanged)
    {
        u32Primask = __get_PRIMASK();
        __disable_irq();

        pstTimer->bIsStatusChanged = false;
        pstTimer->enCurrentStatus  = pstTimer->enChangedStatus;

        __set_PRIMASK(u32Primask);
    }

    if (SW_TIMER_STATUS_IDLE == pstTimer->enCurrentStatus)
    {
        /* delete */
        u32Primask = __get_PRIMASK();
        __disable_irq();

        if (NULL != pstTimer->pstLast)
        {
            pstTimer->pstLast->pstNext = pstTimer->pstNext;
        }

        if (m_stTimerHead == pstTimer)
        {
            m_stTimerHead = pstTimer->pstNext;
        }

        if (m_stTimerTail == pstTimer)
        {
            m_stTimerTail = pstTimer->pstLast;
        }

        __set_PRIMASK(u32Primask);

        pstTimer->pstLast = NULL;
        pstTimer->pstNext = NULL;

        return;
    }

    if (SW_TIMER_STATUS_READY == pstTimer->enCurrentStatus)
    {
        return; /* stop */
    }

    if (SW_TIMER_STATUS_RUNNING != pstTimer->enCurrentStatus)
    {
        return; /* unkown status, error */
    }

    /* running */
    if (bIsStatusChanged)
    {
        u32Temp = pstTimer->u32ChangedBase;
        timer_compare(pstTimer, u32Temp, pstTimer->u32CurrentTimeout);
        return;
    }

    if (SW_TIMER_MODE_REPEAT == pstTimer->enMode)
    {
        u32Temp = 0;
        rom_hw_stim_get_count(STIM_HANDLE, &u32Temp);
        timer_compare(pstTimer, u32Temp, pstTimer->u32CurrentTimeout);
    }
    else
    {
        pstTimer->enCurrentStatus = SW_TIMER_STATUS_READY;
    }
}

/**
 * @brief  Timer RTC interrupt handler.
 */
void SW_TIMER_STIM_IRQ_HANDLER(void)
{
    uint16_t u16IntFlag;
    bool     bNextHwCompareSet;
    bool     bTimerReachedFlg;

    uint32_t u32Compare;
    uint32_t u32Base;
    uint32_t u32CurrentHwCompare = 0;
    uint32_t u32NextHwCompare;
    uint32_t u32IntTimeCnt;

    stSwTimerNode_t *pstTimer;
    stSwTimerNode_t *pstNextTimer;

    bNextHwCompareSet = false;

    rom_hw_stim_get_interrupt_flag(STIM_HANDLE, &u16IntFlag);
    PRINTF("STIM int: %08X\n", u16IntFlag);

    /* Must clear stim interrupt firstly */
    if (u16IntFlag & STIM_INT_MSK)
    {
        rom_hw_stim_clear_interrupt_flag(STIM_HANDLE, (u16IntFlag & STIM_INT_MSK));
        rom_delay_us(100);
        timer_trig_schedule();
    }

    rom_hw_stim_get_compare(STIM_HANDLE, STIM_CH, &u32CurrentHwCompare);
    PRINTF("Current Set Comp: %d\n", u32CurrentHwCompare);

    pstTimer = m_stTimerHead;
    while (NULL != pstTimer)
    {
        /* Get pstNext pstTimer before config(timer delete) */
        pstNextTimer = pstTimer->pstNext;

        if (pstTimer->bIsContextChanged || pstTimer->bIsStatusChanged)
        {
            timer_config(pstTimer);
            timer_trig_schedule();

            pstTimer = pstNextTimer;
            continue;
        }

        if (SW_TIMER_STATUS_RUNNING != pstTimer->enCurrentStatus)
        {
            pstTimer = pstNextTimer;
            continue;
        }

        /* Get int time */
        bTimerReachedFlg = false;
        // rom_hw_stim_get_compare ( STIM_HANDLE, STIM_CH, &u32IntTimeCnt );
        rom_hw_stim_get_count(STIM_HANDLE, &u32IntTimeCnt);

        /* compare overlap */
        u32Compare = pstTimer->u32Compare;
        u32Base    = pstTimer->u32Base;

        if (u32Compare < u32Base)
        {
            if (u32IntTimeCnt >= u32Compare && u32IntTimeCnt < u32Base)
            {
                bTimerReachedFlg = true;
                // PRINTF("bTimerReachedFlg 1\r\n");
            }
        }
        else
        {
            if (u32IntTimeCnt < u32Base || u32IntTimeCnt >= u32Compare)
            {
                bTimerReachedFlg = true;
                // PRINTF("bTimerReachedFlg 2\r\n");
            }
        }

        if (bTimerReachedFlg)
        {
            /* callback */
            if (NULL != pstTimer->pfnHandler)
            {
                pstTimer->pfnHandler(pstTimer->pCurrentContext);
            }

            timer_config(pstTimer);
            timer_trig_schedule();
        }

        pstTimer = pstNextTimer;
    }

    /* Find latest shedule time */
    for (pstTimer = m_stTimerHead; NULL != pstTimer; pstTimer = pstTimer->pstNext)
    {
        /* only schedule running timer */
        if (SW_TIMER_STATUS_RUNNING != pstTimer->enCurrentStatus)
        {
            continue;
        }

        u32Compare = pstTimer->u32Compare;
        u32Base    = pstTimer->u32Base;

        /* Find latest shedule time */
        if (!bNextHwCompareSet)
        {
            bNextHwCompareSet = true;
            u32NextHwCompare  = u32Compare;
            continue;
        }

        if (u32NextHwCompare < u32CurrentHwCompare)
        {
            if (u32Compare < u32Base)
            {
                if (u32Compare < u32NextHwCompare)
                {
                    u32NextHwCompare = u32Compare;
                }
            }
            else
            {
                u32NextHwCompare = u32Compare;
            }
        }
        else
        {
            if (u32Compare < u32Base)
            {
                /* Do nothing */
            }
            else
            {
                if (u32Compare < u32NextHwCompare)
                {
                    u32NextHwCompare = u32Compare;
                }
            }
        }
    }

    /* Schedule pstNext timer */
    if ((bNextHwCompareSet) && (u32NextHwCompare != u32CurrentHwCompare))
    {
        rom_hw_stim_set_compare(STIM_HANDLE, STIM_CH, u32NextHwCompare);
        PRINTF("Next Compare: %d.\n", u32NextHwCompare);

        timer_trig_schedule();
    }
}

/**
 * @brief  App timer initialize function, before use rtc timer, please call this function.
 */
EN_ERR_STA_T sw_timer_init(void)
{
    /* Enable stim clock gate first. */
    rom_hw_crg_enable_clk_gate(STIM_CLK_GATE);

    rom_hw_stim_set_prescale(STIM_HANDLE, 0);

    NVIC_ClearPendingIRQ(STIM_IRQ);
    NVIC_SetPriority(STIM_IRQ, 3);
    NVIC_EnableIRQ(STIM_IRQ);

    rom_hw_sys_ctrl_enable_peri_int(STIM_SYS_CTRL, STIM_IRQ);
    rom_hw_stim_enable_wakeup(STIM_HANDLE, STIM_CH3_INT_MATCH_WAKEUP);
    rom_hw_stim_enable_interrupt(STIM_HANDLE, STIM_INT_MSK);
    rom_hw_stim_disable_tick_overflow_interrupt(STIM_HANDLE, 0xFF);

    rom_hw_stim_start(STIM_HANDLE);

    m_stTimerHead = NULL;
    m_stTimerTail = NULL;

    return ERR_STA_OK;
}

/**
 * @brief  Create a timer.
 * @param  pstTimer: If create the timer successed, this point will save the new timer id.
 * @param  enMode: Work mode, @ref EN_SW_TIMER_MODE_T.
 * @param  pfnHandler: Function to be executed when the timer expires.
 * @return HW status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T sw_timer_create(stSwTimerNode_t *pstTimer, EN_SW_TIMER_MODE_T enMode,
                             sw_timer_timeout_handler_t pfnHandler)
{
    uint32_t u32Primask;

    if ((NULL == pstTimer) || (!IS_VALID_SW_TIMER_MODE(enMode)))
    {
        return ERR_PARA_ERR;
    }

    u32Primask = __get_PRIMASK();
    __disable_irq();

    if (SW_TIMER_STATUS_IDLE != pstTimer->enCurrentStatus)
    {
        __set_PRIMASK(u32Primask);
        return ERR_PARA_ERR;
    }

    pstTimer->enCurrentStatus = SW_TIMER_STATUS_READY;
    pstTimer->enMode          = enMode;
    pstTimer->pfnHandler      = pfnHandler;
    pstTimer->pstNext         = NULL;

    if (NULL == m_stTimerHead)
    {
        pstTimer->pstLast = NULL;
        m_stTimerHead     = pstTimer;
    }

    if (NULL != m_stTimerTail)
    {
        pstTimer->pstLast      = m_stTimerTail;
        m_stTimerTail->pstNext = pstTimer;
    }

    m_stTimerTail = pstTimer;

    __set_PRIMASK(u32Primask);

    return ERR_STA_OK;
}

/**
 * @brief  Start the timer.
 * @param  pstTimer: Point that the timer should be initialized.
 * @param  u32Time: timer out value unit ms, 0~131072000ms.
 * @param  pContext: point to the parameter when the timerout happened will send to the timerout handler.
 * @return HW status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T sw_timer_start(stSwTimerNode_t *pstTimer, uint32_t u32Time, void *pContext)
{
    if ((NULL == pstTimer) || (SW_TIMER_STATUS_IDLE == pstTimer->enCurrentStatus))
    {
        return ERR_PARA_ERR;
    }

    pstTimer->u32ChangedTimeout = u32Time;
    pstTimer->pChangedContext   = pContext;

    rom_hw_stim_get_count(STIM_HANDLE, &pstTimer->u32ChangedBase);

    pstTimer->enChangedStatus   = SW_TIMER_STATUS_RUNNING;
    pstTimer->bIsContextChanged = true;
    pstTimer->bIsStatusChanged  = true;

    timer_trig_schedule();

    return ERR_STA_OK;
}

/**
 * @brief  Stop a specified timer.
 * @param  pstTimer: point to the timer will be stop.
 * @return HW status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T sw_timer_stop(stSwTimerNode_t *pstTimer)
{
    if ((NULL == pstTimer) || (SW_TIMER_STATUS_IDLE == pstTimer->enCurrentStatus))
    {
        return ERR_PARA_ERR;
    }

    pstTimer->enChangedStatus  = SW_TIMER_STATUS_READY;
    pstTimer->bIsStatusChanged = true;

    timer_trig_schedule();

    return ERR_STA_OK;
}

/**
 * @brief  Stop one timer. after this, the timer can be restart.
 * @param  pstTimer: point to the timer will be stop.
 * @return HW status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T sw_timer_delete(stSwTimerNode_t *pstTimer)
{
    uint32_t u32Primask;

    if ((NULL == pstTimer) || (SW_TIMER_STATUS_IDLE == pstTimer->enCurrentStatus))
    {
        return ERR_PARA_ERR;
    }

    pstTimer->enChangedStatus  = SW_TIMER_STATUS_IDLE;
    pstTimer->bIsStatusChanged = true;

    timer_trig_schedule();

    return ERR_STA_OK;
}
