/******************************************************************************
 * @file     ll_pm.h
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/
/*
 * ll_pm.h
 *
 *  Created on: 2017-3-7
 *      Author: Administrator
 */

#ifndef LL_PM_H_
#define LL_PM_H_

#ifndef     BLS_USER_TIMER_WAKEUP_ENABLE
#define        BLS_USER_TIMER_WAKEUP_ENABLE    1
#endif


////////////////// Power Management ///////////////////////
#define            SUSPEND_DISABLE                0
#define            SUSPEND_ADV                    BIT(0)
#define            SUSPEND_CONN                   BIT(1)
#define            DEEPSLEEP_RETENTION_ADV        BIT(2)
#define            DEEPSLEEP_RETENTION_CONN       BIT(3)
#define            MCU_STALL                      BIT(6)





typedef struct {
    u8      suspend_mask;
    u8      wakeup_src;
    u8      conn_no_suspend;
    u8      timer_wakeup;

    u8      latency_off;
    u8      no_latency;
    u16     latency_en;

    u16     sys_latency;
    u16     user_latency;
    u16     valid_latency;
    u16     latency_use;

    u32     deepRet_advThresTick;
    u32     deepRet_connThresTick;
    u32     deepRet_earlyWakeupTick;

    u8      deepRt_en;
    u8      deepRet_type;
    u8      appWakeup_en;
    u8      appWakeup_flg;

    u8      appWakeup_loop_noLatency;
    u8      timing_miss;
    u8      timing_synced;
    u8      rsvd;

    u32     appWakeup_tick;

    u32     current_wakeup_tick; //The system wake-up tick of the actual transfer of the cpu_sleep_wakeup function.

    u16     pm_border_flag;
    u16     rsvd1;

}st_ll_pm_t;

extern _attribute_aligned_(4) st_ll_pm_t  bltPm;


typedef     void (*ll_module_pm_callback_t)(void);


typedef     void (*pm_appWakeupLowPower_callback_t)(int);




/******************************* User Interface  ************************************/
void         blc_ll_initPowerManagement_module(void);


void         bls_pm_setSuspendMask (u8 mask);
u8           bls_pm_getSuspendMask (void);
void         bls_pm_setWakeupSource(u8 source);
u32          bls_pm_getSystemWakeupTick(void);

void         bls_pm_setManualLatency(u16 latency); //manual set latency to save power

void         bls_pm_setAppWakeupLowPower(u32 wakeup_tick, u8 enable);
void         bls_pm_registerAppWakeupLowPowerCb(pm_appWakeupLowPower_callback_t cb);

void         blc_pm_setDeepsleepRetentionThreshold(u32 adv_thres_ms, u32 conn_thres_ms);
void         blc_pm_setDeepsleepRetentionEarlyWakeupTiming(u32 earlyWakeup_us);
void         blc_pm_setDeepsleepRetentionType(SleepMode_TypeDef sleep_type);

/************************* Stack Interface, user can not use!!! ***************************/




#endif /* LL_PM_H_ */
