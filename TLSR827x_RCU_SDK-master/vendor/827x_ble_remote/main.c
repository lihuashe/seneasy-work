/******************************************************************************
 * @file     main.c
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

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "vendor/common/blt_fw_sign.h"
#include "vendor/common/flash_fw_check.h"
#include "../../vendor/common/user_config.h"
#include "battery_check.h"
#include "app_ui.h"


extern void app_init_normal();
extern void app_init_deepRetn();
extern void app_loop (void);

extern void rc_ir_irq_prc(void);

/**
 * @brief 中断处理
 * 
 * @return _attribute_ram_code_ 
 */
_attribute_ram_code_ void irq_handler(void)
{
#if (REMOTE_IR_ENABLE)
    rc_ir_irq_prc();
#endif

    irq_blt_sdk_handler ();

#if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE)
    extern void irq_phyTest_handler(void);
    irq_phyTest_handler();
#endif

#if (PTM)
    app_uart_irq_proc(); // currentlly used for ptm only
#endif
}

/**
 * @brief 唤醒快速按键扫描
 * 
 */
void device_wakeup_keyscan(void)
{
#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
    //if deepsleep wakeup is wakeup by GPIO(key press), we must quickly scan this
    //press, hold this data to the cache, when connection established OK, send to maste
    // #define KEY_OK 31 //forgive me to make it dependent with app_ui.c
    //deepsleep_wakeup_fast_keyscan
    if(analog_read(USED_DEEP_ANA_REG) & POWER_ON_FLG){
        key_wakeup_fast_scan();
        analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) & (~CONN_DEEP_FLG));
    }
#endif
}

/**
 * @brief 欠压保护
 * Note: battery check must do before any flash write/erase operation, cause flash write/erase
 *       under a low or unstable power supply will lead to error flash operation
 *       Some module initialization may involve flash write/erase, include: OTA initialization,
 *          SMP initialization, ..
 *          So these initialization must be done after  battery check
 * @return  
 */
_attribute_ram_code_ void device_uvlo(void)
{
#if (BATT_CHECK_ENABLE)  //battery check must do before OTA relative operation
    #if(VBAT_LEAKAGE_PROTECT_EN)
    do{
        // 读取当前电压
        app_battery_power_check(VBAT_DEEP_THRES_MV,VBAT_SUSPEND_THRES_MV);
        // 必要时清除看门狗
        #if (MODULE_WATCHDOG_ENABLE)
        wd_clear(); //clear watch dog
        #endif
        // 判断是否为低电状态
        u8 analog_deep = analog_read(USED_DEEP_ANA_REG);
        if(analog_deep & LOW_BATT_SUSPEND_FLG){
            sleep_us(100000);
        }
    }while(analog_read(USED_DEEP_ANA_REG) & LOW_BATT_SUSPEND_FLG);
    #else
    if(analog_read(USED_DEEP_ANA_REG) & LOW_BATT_FLG){
        app_battery_power_check(VBAT_DEEP_THRES_MV + 200);
    }
    else{
        app_battery_power_check(VBAT_DEEP_THRES_MV);
    }
    #endif
#endif
}

/**
 * @brief 主函数入口:
 * 进入条件
 * 1.芯片硬件复位(上电/看门狗复位/异常复位等);
 * 2.deep睡眠唤醒
 * 
 * @return  
 */
_attribute_ram_code_ int main (void)    //must run in ramcode
{
    blc_pm_select_internal_32k_crystal();

#if(MCU_CORE_TYPE == MCU_CORE_8258)
    cpu_wakeup_init();
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
    cpu_wakeup_init(LDO_MODE,EXTERNAL_XTAL_24M);
#endif
    // 判定当前复位源
    int is_deep_wakeup = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp
    gpio_init(!is_deep_wakeup );  //analog resistance will keep available in deepSleep mode, so no need initialize again

    // PHY初始化
    rf_drv_init(RF_MODE_BLE_1M);
    // blt_extended_reject_enable(0);

    // 时钟初始化
#if (CLOCK_SYS_CLOCK_HZ == 16000000)
    clock_init(SYS_CLK_16M_Crystal);
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
    clock_init(SYS_CLK_24M_Crystal);
#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
    clock_init(SYS_CLK_32M_Crystal);
#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
    clock_init(SYS_CLK_48M_Crystal);
#endif

    // LOG接口初始化
#if (UART_PRINT_ENABLE)
    extern void log_uart_init(void);
    log_uart_init();
#endif

    // 快速按键扫描
    device_wakeup_keyscan();

    // 冷启动:上电,看门狗复位
    if (!is_deep_wakeup) {//read flash size
        // Undervoltage-Lockout
        device_uvlo(); //battery check must do before flash code
        blc_readFlashSize_autoConfigCustomFlashSector();
        extern u8 app_wakeup_src(void);
        //1:wakeup for pad  2:wake up for timer
        printf("wakeup_src=%x\r\n", app_wakeup_src()); 
        #if FIRMWARE_CHECK_ENABLE
            //Execution time is in ms.such as:48k fw,16M crystal clock,need about 290ms.
            if(flash_fw_check(0xffffffff)){ //return 0, flash fw crc check ok. return 1, flash fw crc check fail
                while(1);                    //Users can process according to the actual application.
            }
        #endif
        #if FIRMWARES_SIGNATURE_ENABLE
            blt_firmware_signature_check();
        #endif
        app_init_normal(); // call to app_ui_init_normal()
    }
    // 深度睡眠唤醒启动
    else {
        printf("."); // a . means a wakeup from deep
        app_init_deepRetn ();
    }

    //load customized freq_offset cap value
    blc_app_loadCustomizedParameters();  

    irq_enable();

    while (1) {
#if (MODULE_WATCHDOG_ENABLE)
        wd_clear(); //clear watch dog
#endif
        app_loop (); // key_change_proc(), process_1_key()
    }
}

