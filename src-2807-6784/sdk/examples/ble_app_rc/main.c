/**
 * ************************************************************************
 * 
 * @file main.c
 * @author XiHao()
 * @brief 
 * 
 * ************************************************************************
 * @copyright Copyright 2021-2022 XiHao Inc. All Rights Reserved.
 * The demo provided is for reference only
 * ************************************************************************
 */
#include <stdint.h>
#include <string.h>
#include "peripheral.h"
#include "cc.h"
#include "radio.h"
#include "ble.h"
#include "cmsis_compiler.h"
#include "app_uart.h"
#include "key_scan.h"
#include "app_gap.h"
#include "app_keyboard.h"
#include "app_led.h"
#include "app_batt.h"
#include "board_config.h"
#include "ble_config.h"
#include "da228ec_driver.h"
#include "sensor_data_process.h"
#if (REMOTE_IR_ENABLE)
#include "app_ir.h"
#endif
#if (BACKLIGHT_ENABLE)
#include "modules/led_module/bct3236_driver.h"
#endif
#include "xh98xx_flash_protect.h"


#define APP_LOG_DOMAIN      "main"
#define APP_LOG_LEVEL       APP_MAIN_LOG_LEVEL
#include "co_debug.h"

#if (BOARD_STATUS_PRINTF_EN)
static co_timer_t simple_timer;
#endif
static co_timer_t s_power_on_timer;

	
static uint8_t att_db_buffer[1200];

BOARD_STATUS_T g_board_status;		//板子状态,记录当前模式连接电池电量等信息

const static char* reboot_reason_str[] = {
    "PMU_REBOOT_FROM_POWER_ON",
    "PMU_REBOOT_FROM_ULTRA_DEEP_SLEEP",
    "PMU_REBOOT_FROM_OTA_ISP",
    "PMU_REBOOT_FROM_USER",
    "PMU_REBOOT_FROM_WDT",
    "PMU_REBOOT_FROM_IFLASH_LOW_V"
};

/**
 * @brief		系统模式切换
 * @param[in]	sys_mode--系统模式 SYSTEM_MODE_E
 * @note		主要用于区分当前系统是否进特殊模式,关闭某些功能
 */
void system_mode_switch(unsigned char sys_mode)
{
	g_board_status.system_mode = sys_mode;
}

static void power_on_delay_task_handle(co_timer_t *timer, void *param)
{
    APP_LOG_INF_FUNC();
	
    APP_LOG_INF("kc:%d k0:%d  k1:%d  k2:%d\n",kb_event.cnt,kb_event.keycode[0],kb_event.keycode[1],kb_event.keycode[2]);
}




/**
 * @brief	外设恢复函数
 */
CO_SECTION_RAMTEXT
static void peripheral_restore(void)
{
#if REMOTE_64M_ENABLE
	pmu_xtal32m_x2_startup();
#endif
	
#if REMOTE_UART_ENABLE
  app_uart_init();
#endif
}


/**
 * @brief 功率管理事件处理函数
 * 
 * @param sleep_state 睡眠状态，包括进入睡眠、离开睡眠等
 * @param power_status 电源状态，目前未在函数中使用
 */
CO_SECTION_RAMTEXT
static void power_sleep_event_handler(co_power_sleep_state_t sleep_state, co_power_status_t power_status)
{
    switch(sleep_state)
    {
        case POWER_SLEEP_ENTRY: //进入睡眠状态的事件
			#if (SLEEP_FLASH_OFF_EN)
			if (g_board_status.rcu_linklayer_status == RCU_LINKLAYER_IDLE)
			{
				sfs_auto_disen_set(true);
			}
			#endif
            break;

        case POWER_SLEEP_LEAVE_TOP_HALF: //休眠离开的事件（无法立即控制 IO）用户应在此事件中重新初始化（或恢复）所有使用的外设。
//			//降低连接功耗
            peripheral_restore();
			
            break;

        case POWER_SLEEP_LEAVE_BOTTOM_HALF:  // 睡眠离开事件（可立即控制 IO）
            break;
    }
}

/**
 * @brief 获取当前的电源睡眠状态
 * 
 * 
 * @return co_power_status_t 返回当前的电源睡眠状态，类型为co_power_status_t枚举。
 */
co_power_status_t power_sleep_status_handler(void)
{
    co_power_status_t pmu_status;

    pmu_status = pmu_power_status();

#if (MODE_DEEP_SLEEP_NO_RESET_MCU)
	pmu_status = MIN(POWER_SLEEP,pmu_status);
#endif
	
    return pmu_status;
}

/**
 * @brief 获取随机数并填充到指定的字节数组中
 * 
 * 
 * @param data 目标字节数组的指针
 * @param nBytes 要填充的字节数
 */
static void get_random(uint8_t *data, uint8_t nBytes)
{

	uint32_t rand_data=XH_RANDOM->RANDOM;
	int rnd_data[3];
	rnd_data[0]=pmu_random_seed_fetch(rand_data);
    *data++ = (rnd_data[0] >> 24)&0xFF;
    *data++ = (rnd_data[0] >> 16)&0xFF;
    *data++ = (rnd_data[0] >>  8)&0xFF;
}

/**
 * @brief 读取非易失性存储器（NVDS）中的蓝牙设备 MAC 地址。
 *
 * 本函数尝试从 NVDS 中读取蓝牙设备的 MAC 地址。如果 NVDS 中已存储了 MAC 地址，
 * 则直接使用该地址；否则，生成一个随机的 MAC 地址并将其存储到 NVDS 中。
 *
 */
static void read_nvds_device_mac(void)
{
	ble_gap_addr_t gap_addr;
	
#if (APP_SEC_NVDS_SUPPORT)
    nvds_tag_len_t length = BLE_GAP_ADDR_LEN;
    if (nvds_get(NVDS_TAG_BD_ADDRESS, &length, &gap_addr.addr) == NVDS_OK)
    {
		gap_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;
    }
    else
#endif
    {
        uint8_t loc_addr[6] = LOCAL_DEFAULT_ADDR;
		get_random(loc_addr, 6);     
		gap_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;
        memcpy(&gap_addr.addr,&loc_addr,BLE_GAP_ADDR_LEN);
        #if (APP_SEC_NVDS_SUPPORT)
        nvds_put(NVDS_TAG_BD_ADDRESS, BLE_GAP_ADDR_LEN, &gap_addr.addr);
        #endif
    }
    APP_LOG_INF("bd_addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",gap_addr.addr[0],gap_addr.addr[1],gap_addr.addr[2],gap_addr.addr[3],gap_addr.addr[4],gap_addr.addr[5]);
    xh_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &gap_addr); 
}


/**
 * @brief 初始化硬件资源
 * 
 */
static void hardware_init(void)
{
	pmu_pin_pa6_reset_mode_enable(false);
	// default all is pull up
    pmu_pin_mode_set(0x7FFFF, PMU_PIN_MODE_PU);

	gpio_open();

#if REMOTE_UART_ENABLE
    app_uart_init();
#endif
	
#if (REMOTE_IR_ENABLE)
	app_ir_init();
#endif
	
  app_key_init(key_task_func);

#if (BLT_APP_LED_ENABLE)
  led_module_init();
#endif
	
#if (REMOTE_BATT_ENABLE)
	app_batt_init();
#endif

#if (BACKLIGHT_ENABLE)
	bct3236_init();
#endif

#if (SENSOR_3_AXIAL_ENABLE)
    da228ec_init();
#endif

    co_power_register_user_status(power_sleep_status_handler);
    co_power_register_sleep_event(power_sleep_event_handler);
}


static void ble_stack_config(void)
{
    // disable WDT
    wdt_enable(0);
	
#if CONFIG_INTERNAL_DCDC_ENABLE
    pmu_dcdc_enable(true);
#endif
    // Select 32k clock for stack
    cc_sca_set(CC_SCA_500PPM);

    // Enable sleep, SWD will be closed.
    co_power_sleep_enable(true);

    rf_tx_power_set(RF_TX_POWER_NORMAL);
	
    pmu_xtal32m_change_param(XTAL32M_CTUNE_DEFAULT);
  
}

/**
 * @brief 调整dvdd电压
 *
 */
void ajust_dvdd(void)
{
     int8_t dig_ldo = REGR(&XH_PMU->ANA_REG, MASK_POS(PMU_ANA_LDO_V0P9_DIG_VBAT));
     calib_repair_env.delta_vdd_1p2 = calib_repair_env.delta_vdd_1p2+2;

     // DVDD
     REGW(&XH_PMU->ANA_REG, MASK_2REG(PMU_ANA_LDO_V0P9_DIG_VBAT,calib_repair_value_select(dig_ldo+5, 0,15), PMU_ANA_CTRL_LDO_DIG_UPDATE,1));
     // LDO
     REGW(&XH_PMU->ANA_REG, MASK_1REG(PMU_ANA_LDO_V1P2_A_VBAT,calib_repair_value_select(CALIB_REPAIR_LDO_V_NORMAL_T+calib_repair_env.delta_vdd_1p2,0,CALIB_REPAIR_LDO_V_MAX)));	
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

int main_loop(void)
{

    while(1)
    {
        uint8_t chip_id;
        APP_LOG_INF("reset result %d\r\n", 888);
        sensor_event_handler();
        co_delay_ms(500);
    }
}




int main(void)
{
    int xh_rf_status = 0;
	unsigned char reset_status = 0;
	
#if REMOTE_64M_ENABLE
	pmu_xtal32m_x2_startup();
#endif
	
	ajust_dvdd();
	
	system_mode_switch(SYS_NORMAL_MODE);

    hardware_init();
    
	ble_stack_config();
    ble_xihao_config();
    xh_ble_enable(NULL);
    read_nvds_device_mac();        
	softdevice_ble_evt_handler_set(app_ble_gap_evt);
	ble_set_db_buffer(att_db_buffer, sizeof(att_db_buffer));

	app_flash_protect_on();
	
	reset_status = pmu_reboot_reason();
	
	if (!xh_rf_status)
	{
		/*开机延时任务处理*/
		if (reset_status == PMU_REBOOT_FROM_POWER_ON)
		{
			co_timer_set(&s_power_on_timer, 100, TIMER_ONE_SHOT, power_on_delay_task_handle, NULL);
		}
		app_gap_init(reset_status);
	}
	
	uint32_t uartClk = cpm_get_clock(CPM_UART1_CLK);
    uint32_t topClk = cpm_get_clock(CPM_TOP_CLK);
    uint32_t cpuClk = cpm_get_clock(CPM_CPU_CLK);
    uint32_t clkIrtx = cpm_get_clock(CPM_IRTX_CLK);

	APP_LOG_INF("topClk =%d, cpuClk=%d %d, irtxCLK=%d\n",topClk,cpuClk,uartClk,clkIrtx);
    APP_LOG_INF("reset result %s\r\n", reboot_reason_str[reset_status]);
    APP_LOG_INF("att_db_free_size: %d bytes\r\n", ble_get_db_buffer_free_size());
	
    main_loop(); // Enter main loop.

    return 0;
}



