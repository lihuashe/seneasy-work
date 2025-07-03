
/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     factory_test.c
* @brief
* @details
* @author   lizefeng
* @date     2024-02-29
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "driver.h"
#include "factory_test.h"
#include "AAA_24G_rf_frame.h"
#include "AAA_app_config.h"
#include "AAA_project_config.h"
#include "rf_drv.h"
#include "types.h"
#include "pwm_handle.h"
#include "key_driver.h"
#include "ota_slaver.h"
#include "../drivers/flash.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

//*********************************代码补全区**************************************//
/**************************** 512 K Flash *****************************/
#if FACTORY_TEST_24G
#ifndef CFG_ADR_MAC_512K_FLASH
#define CFG_ADR_MAC_512K_FLASH 0x76000

#define D24G_PAIR_TIMER_OUT 350 // unit 1us
_attribute_data_retention_user u8 device_status = 0;
_attribute_data_retention_ u32 flash_sector_mac_address = CFG_ADR_MAC_512K_FLASH; // default flash is 512k
_attribute_data_retention_ u32 app_bath_factory_test_timer = 0;
_attribute_data_retention_user volatile unsigned int rf_state = 0;
_attribute_data_retention_ bool exit_factory_test = 0;
bool production_testing = 0;
u32 send_data_num = 1;

rf_packet_t rf_factory_buf =
    {
        14, // dma_len
        13, // rf_len
};
factory_data_t *p_factory_data = (factory_data_t *)&rf_factory_buf.dat[0];
rf_packet_t rf_factory_mac_buf =
    {
        16, // dma_len
        15, // rf_len
};

rf_packet_t rf_factory_soft_mac_buf =
    {
        18, // dma_len
        17, // rf_len
};

typedef enum
{
    RF_IDLE_STATUS = 0,
    RF_TX_START_STATUS = 1,
    RF_TX_END_STATUS = 2,
    RF_RX_START_STATUS = 3,
    RF_RX_END_STATUS = 4,
    RF_RX_TIMEOUT_STATUS = 5,

} RF_STATUS_USER;

_attribute_data_retention_user custom_cfg_t user_cfg;
_attribute_data_retention_user volatile int device_ack_received = 0;
extern _attribute_ram_code_sec_ void rf_set_tx_rx_setting();
extern _attribute_ram_code_sec_ void rf_set_channel_aaa(signed char chn, unsigned short set);
//*********************************代码补全区**************************************//
#define xstr(s) str(s)
#define str(s) #s
#define FW_VERSION_STR xstr(VERSION)
int strToInt(const char *str);

volatile u32 send_device_mac_time = 0;
bool send_device_mac_flag = false;

u32 factory_dongle_id = 0;
u8 factory_cmd = 0;
u8 factory_channel_mode = 0;
volatile u8 factory_channel = 3;
volatile u8 factory_channel_begin = 0;
u8 factory_signle_begin = 0;
u8 factory_exit_begin = 0;

static bool factory_exit_flag = false;
u8 *ptr_factory = 0;

factory_mac_data_t *p_factory_mac_data = (factory_mac_data_t *)&rf_factory_mac_buf.dat[0];
factory_soft_mac_data_t *p_factory_soft_mac_data = (factory_soft_mac_data_t *)&rf_factory_soft_mac_buf.dat[0];

extern _attribute_data_retention_user volatile unsigned int rf_rx_timeout_us;

_attribute_data_retention_user custom_cfg_t user_cfg;
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  app_rcu_factory_test
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_factory_test(void)
{
#if FACTORY_TEST_24G
    device_status = 4; // STATE_FACTORY;
    // rf_rx_timeout_us = D24G_PAIR_TIMER_OUT;
#endif
}

/******************************************************************
 * @brief  bath_rcu_enter_factory_test
 * @param  none
 * @return none
 * @retval void
 */
void bath_rcu_enter_factory_test(void)
{
#if FACTORY_TEST_24G
    printf("--->>> [work_debug] bath_rcu_enter_factory_test\n");
    app_bath_factory_test_timer = clock_time() | 1;
    for (int i = 0; i < 3; i++)
    {
        led_pwm_all_open();
        WaitMs(100);
        led_pwm_all_off();
        WaitMs(100);
    }
    device_channel = 15;
    app_rcu_factory_test();
#endif
}

/******************************************************************
 * @brief  Bath_Factory_Test_Key
 * @param  bath_key
 * @return none
 * @retval void
 */

// 产测按键事件
void key_handle_factory_1_event()
{
#if RF_24G_FACTORY_ENABLE
    if (factory_test_key_values != 0xfb)
    {
        // printf("--->>> [work_debug] Bath_Factory_Test_Key: %d\n", factory_test_key_values);
        p_factory_data->head_0 = 0x55;
        p_factory_data->head_1 = 0xAA;
        p_factory_data->cmd_0 = 0x03;
        p_factory_data->cmd_1 = 0x10;
        p_factory_data->len = 0x06;
        p_factory_data->did = factory_dongle_id;
        p_factory_data->value_dat[0] = factory_test_key_values;
        p_factory_data->value_dat[1] = 0x00;
        ptr_factory = (u8 *)&rf_factory_buf;
        factory_cmd = 1;
        factory_test_key_values = 0xfb;
        // printf("--->>> [Factory_Test_24G_Data] ");
        // for (int i = 0; i < rf_factory_buf.rf_len; i++)
        //     printf(" %02X", rf_factory_buf.dat[i]);
        // printf("\r\n");
    }

#endif
}

void rf_rx_process_factory(rf_packet_t *p_rf_data)
{
#if FACTORY_TEST_24G
    // u8 *ptr = 0;
    factory_ack_data_t *factory_ack_dat_ptr = (factory_ack_data_t *)&p_rf_data->dat[0];
    static bool flash_change_flag = false;
    rf_set_tx_rx_off();
    rf_set_txmode();
    if ((factory_ack_dat_ptr->head_0 == 0xAA) && (factory_ack_dat_ptr->head_1 == 0x88))
    {
        p_factory_data->head_0 = 0x88;
        p_factory_data->head_1 = 0xAA;
        p_factory_data->cmd_0 = factory_ack_dat_ptr->cmd_0;
        p_factory_data->cmd_1 = factory_ack_dat_ptr->cmd_1;
        p_factory_data->did = factory_dongle_id;
        // 校验芯片Flash
        if ((factory_ack_dat_ptr->cmd_0 == 0x00) && (factory_ack_dat_ptr->cmd_1 == 0x05) && flash_change_flag == false)
        {
            static u32 erase_addr = 0xffffffff;
            flash_change_flag = true;
            u8 flash_check_flag = 0;
            static u32 read_old_buf, read_new_buf;
            static u32 check_flash_id = (PAIR_DEVICE_SAVE_ADDR - 0x400);

            flash_read_page(check_flash_id, 4, (u8 *)&read_old_buf);
            printf("--->>> [work_debug] Flash Read Old: 0x%02x\r\n", read_old_buf);

            read_old_buf = (0x0000ff00 & (DEVICE_SOFT_RSV_1 << 8));
            read_old_buf |= DEVICE_SOFT_RSV_2;
            printf("--->>> [work_debug] Flash Read Old: 0x%02x\r\n", read_old_buf);

            app_rcu_flash_unlock_init(); // UNLOCK
            flash_write_page(check_flash_id, 4, (u8 *)&read_old_buf);
            flash_read_page(check_flash_id, 4, (u8 *)&read_new_buf);
            printf("--->>> [work_debug] Flash Read New: 0x%02x\r\n", read_new_buf);

            if (read_old_buf == read_new_buf)
                flash_check_flag = 0;
            else
                flash_check_flag = 1;

            for (u8 i = 0; i < 4; i++)
                flash_write_page(check_flash_id, 4, (u8 *)&erase_addr);

            app_rcu_flash_lock_init(); // LOCK

            p_factory_data->len = 0x06;
            p_factory_data->value_dat[0] = 0x00;
            p_factory_data->value_dat[1] = flash_check_flag;
            ptr_factory = (u8 *)&rf_factory_buf;
            factory_cmd = 1;
            flash_change_flag = false;
        }
    }
    if ((factory_ack_dat_ptr->head_0 == 0xAA) && (factory_ack_dat_ptr->head_1 == 0x55))
    {
        p_factory_data->head_0 = 0x55;
        p_factory_data->head_1 = 0xAA;
        p_factory_data->cmd_0 = factory_ack_dat_ptr->cmd_0;
        p_factory_data->cmd_1 = factory_ack_dat_ptr->cmd_1;
        // 通知开始配对
        if ((factory_ack_dat_ptr->cmd_0 == 0x05) && (factory_ack_dat_ptr->cmd_1 == 0x20))
        {
            factory_dongle_id = factory_ack_dat_ptr->did;
            p_factory_data->len = 0x06;
            p_factory_data->did = factory_dongle_id;
            p_factory_data->value_dat[0] = DEVICE_SOFT_RSV_1;
            p_factory_data->value_dat[1] = DEVICE_SOFT_RSV_2;
            p_factory_data->software_dat[0] = DEVICE_PAIR_RSV_1;
            p_factory_data->software_dat[1] = DEVICE_PAIR_RSV_2;
            ptr_factory = (u8 *)&rf_factory_buf;
            factory_cmd = 1; // pairing
            printf("--->>> [Factory_Test] Start Pair !!!\r\n");
            printf("--->>> [Factory_Test_24G_Data] ");
            for (int i = 0; i < rf_factory_buf.rf_len; i++)
                printf(" %01x", rf_factory_buf.dat[i]);
            printf("\r\n");
        }
        else
        {
            if (factory_dongle_id == factory_ack_dat_ptr->did)
            {
                p_factory_data->did = factory_dongle_id;
                // 通知设定频道
                if ((factory_ack_dat_ptr->cmd_0 == 0x05) && (factory_ack_dat_ptr->cmd_1 == 0x30))
                {
                    factory_channel = factory_ack_dat_ptr->value_dat[0];
                    p_factory_data->len = 0x08;
                    p_factory_data->value_dat[0] = factory_channel;
                    p_factory_data->value_dat[1] = 0x00;
                    ptr_factory = (u8 *)&rf_factory_buf;
                    factory_cmd = 1;
                    // printf("--->>> [Factory_Test] Set Channel: %d !!!\r\n", factory_channel);
                    // printf("--->>> [Factory_Test_24G_Data] ");
                    // for (int i = 0; i < rf_factory_buf.rf_len; i++)
                    //     printf(" %01x", rf_factory_buf.dat[i]);
                    // printf("\r\n");
                    factory_channel_begin = 1;
                }
                // 读取软件版本
                if ((factory_ack_dat_ptr->cmd_0 == 0x02) && (factory_ack_dat_ptr->cmd_1 == 0x10))
                {
                    u8 fw_0[1] = {0}, fw_1[1] = {0};
                    fw_0[0] = FW_VERSION_STR[15];
                    fw_1[0] = FW_VERSION_STR[17];
                    u8 bath_rcu_fw_0 = fw_0[0] - 0x30;
                    u8 bath_rcu_fw_1 = fw_1[0] - 0x30;

                    p_factory_data->len = 0x06;
                    p_factory_data->value_dat[0] = bath_rcu_fw_0;
                    p_factory_data->value_dat[1] = bath_rcu_fw_1;
                    ptr_factory = (u8 *)&rf_factory_buf;
                    factory_cmd = 1;
                    // printf("--->>> [Factory_Test] Read FW version: V%d.%d !!!\r\n", bath_rcu_fw_0, bath_rcu_fw_1);
                    // printf("--->>> [Factory_Test_24G_Data] ");
                    // for (int i = 0; i < rf_factory_buf.rf_len; i++)
                    //     printf(" %01x", rf_factory_buf.dat[i]);
                    // printf("\r\n");
                }
                // 读取Mac地址
                if ((factory_ack_dat_ptr->cmd_0 == 0x02) && (factory_ack_dat_ptr->cmd_1 == 0x20))
                {
                    // u8 mac_random_static[6];
                    static u8 mac_public[6];
                    flash_read_page(CFG_MAC_ADDR, 4, (u8 *)mac_public);
                    p_factory_mac_data->head_0 = 0x55;
                    p_factory_mac_data->head_1 = 0xAA;
                    p_factory_mac_data->cmd_0 = factory_ack_dat_ptr->cmd_0;
                    p_factory_mac_data->cmd_1 = factory_ack_dat_ptr->cmd_1;
                    p_factory_mac_data->len = 0x0A;
                    p_factory_mac_data->did = factory_dongle_id;
                    p_factory_mac_data->mac_dat[0] = 0xFF;
                    p_factory_mac_data->mac_dat[1] = 0xFF;
                    p_factory_mac_data->mac_dat[2] = mac_public[3];
                    p_factory_mac_data->mac_dat[3] = mac_public[2];
                    p_factory_mac_data->mac_dat[4] = mac_public[1];
                    p_factory_mac_data->mac_dat[5] = mac_public[0];
                    ptr_factory = (u8 *)&rf_factory_mac_buf;
                    factory_cmd = 1;
                    // printf("--->>> [Factory_Test] Read Mac: [%02X %02X %02X %02X %02X %02X] !!!\r\n", mac_public[5], mac_public[4], mac_public[3], mac_public[2], mac_public[1], mac_public[0]);
                    // printf("--->>> [Factory_Test_24G_Data] ");
                    // for (int i = 0; i < rf_factory_mac_buf.rf_len; i++)
                    //     printf(" %02X", rf_factory_mac_buf.dat[i]);
                    // printf("\r\n");
                }
                // 通知进入单载波
                if ((factory_ack_dat_ptr->cmd_0 == 0x05) && (factory_ack_dat_ptr->cmd_1 == 0x10))
                {
                    p_factory_data->len = 0x06;
                    p_factory_data->value_dat[0] = 0x00;
                    p_factory_data->value_dat[1] = 0x00;
                    ptr_factory = (u8 *)&rf_factory_buf;
                    factory_cmd = 1;
                    // printf("--->>> [Factory_Test] Enter Single Tone !!!\r\n");
                    // printf("--->>> [Factory_Test_24G_Data] ");
                    // for (int i = 0; i < rf_factory_buf.rf_len; i++)
                    //     printf(" %01x", rf_factory_buf.dat[i]);
                    // printf("\r\n");
                    factory_signle_begin = 1;
                }

                // LED灯产测操作
                if ((factory_ack_dat_ptr->cmd_0 == 0x05) && (factory_ack_dat_ptr->cmd_1 == 0x40))
                {
                    p_factory_data->len = 0x06;
                    p_factory_data->value_dat[0] = factory_ack_dat_ptr->value_dat[0];
                    p_factory_data->value_dat[1] = factory_ack_dat_ptr->value_dat[1];
                    ptr_factory = (u8 *)&rf_factory_buf;
                    factory_cmd = 1;
                    if (factory_ack_dat_ptr->value_dat[1] == 0x01)
                    {
                        led_pwm_factory_control(factory_ack_dat_ptr->value_dat[0], 1);
                        // printf("--->>> [Factory_Test] Led : %01x open!!!\r\n", factory_ack_dat_ptr->value_dat[0]);
                    }
                    else if (factory_ack_dat_ptr->value_dat[1] == 0x00)
                    {
                        led_pwm_factory_control(factory_ack_dat_ptr->value_dat[0], 0);
                        // printf("--->>> [Factory_Test] Led : %01x off!!!\r\n", factory_ack_dat_ptr->value_dat[0]);
                    }
                    // printf("--->>> [Factory_Test_24G_Data] ");
                    // for (int i = 0; i < rf_factory_buf.rf_len; i++)
                    //     printf(" %01x", rf_factory_buf.dat[i]);
                    // printf("\r\n");
                }
                // 通知退出产测
                if ((factory_ack_dat_ptr->cmd_0 == 0x01) && (factory_ack_dat_ptr->cmd_1 == 0x20))
                {
                    p_factory_data->len = 0x06;
                    p_factory_data->value_dat[0] = 0x00;
                    p_factory_data->value_dat[1] = 0x00;
                    ptr_factory = (u8 *)&rf_factory_buf;
                    factory_cmd = 30;
                    factory_exit_begin = 1;
                    // printf("--->>> [Factory_Test] Exit Factory Test !!!\r\n");
                    // printf("--->>> [Factory_Test_24G_Data] ");
                    // for (int i = 0; i < rf_factory_buf.rf_len; i++)
                    //     printf(" %01x", rf_factory_buf.dat[i]);
                    // printf("\r\n");
                }
                // OTA 升级
                if ((factory_ack_dat_ptr->cmd_0 == 0x06) && (factory_ack_dat_ptr->cmd_1 == 0x01))
                {
                    p_factory_data->len = 0x06;
                    p_factory_data->value_dat[0] = 0x00;
                    p_factory_data->value_dat[1] = 0x00;
                    ptr_factory = (u8 *)&rf_factory_buf;
                    factory_cmd = 30;
                    ota_enable_begin = 1;
                    printf("OTA_FACTORY_UPDATE\r\n");
                }
            }
            else
                printf("--->>> [work_debug] Factory Dongle ID ERROR\r\n");
        }
    }
#endif
}

void rf_tx_process_factory(void)
{
#if FACTORY_TEST_24G
    u8 Send_24G_Data_Flag = 0;
    set_pair_access_code(0x39517695);
    rf_set_power_level_index(RF_POWER_N18p40dBm);
    static bool set_device_mac = false;

    // // 增加组合键发送MAC地址
    if (send_device_mac_flag == true)
    {
        if (set_device_mac == false)
        {
            static u8 mac_public[6];
            static u8 fw_0[1] = {0}, fw_1[1] = {0};
            fw_0[0] = FW_VERSION_STR[15];
            fw_1[0] = FW_VERSION_STR[17];
            u8 bath_rcu_fw_0 = fw_0[0] - 0x30;
            u8 bath_rcu_fw_1 = fw_1[0] - 0x30;

            flash_read_page(CFG_MAC_ADDR, 4, (u8 *)mac_public);

            p_factory_soft_mac_data->head_0 = 0x55;
            p_factory_soft_mac_data->head_1 = 0xAA;
            p_factory_soft_mac_data->cmd_0 = 0x03;
            p_factory_soft_mac_data->cmd_1 = 0x11;
            p_factory_soft_mac_data->len = 0x0C;
            // software id
            p_factory_soft_mac_data->software_id[0] = DEVICE_SOFT_RSV_1;
            p_factory_soft_mac_data->software_id[1] = DEVICE_SOFT_RSV_2;
            p_factory_soft_mac_data->software_id[2] = DEVICE_PAIR_RSV_1;
            p_factory_soft_mac_data->software_id[3] = DEVICE_PAIR_RSV_2;
            // software version
            p_factory_soft_mac_data->software_version[0] = bath_rcu_fw_0;
            p_factory_soft_mac_data->software_version[1] = bath_rcu_fw_1;
            // mac data
            p_factory_soft_mac_data->mac_dat[0] = 0xFF;
            p_factory_soft_mac_data->mac_dat[1] = 0xFF;
            p_factory_soft_mac_data->mac_dat[2] = mac_public[3];
            p_factory_soft_mac_data->mac_dat[3] = mac_public[2];
            p_factory_soft_mac_data->mac_dat[4] = mac_public[1];
            p_factory_soft_mac_data->mac_dat[5] = mac_public[0];
            ptr_factory = (u8 *)&rf_factory_soft_mac_buf;
            set_device_mac = true;
            printf("--->>> set_device_mac_successful\r\n");
        }
        if (send_data_num)
        {
            rf_state = RF_TX_START_STATUS;
            device_ack_received = 0;
            PIN_DEBUG_UI_TIME_LEVEL(1);
            rf_set_tx_pipe(0);
            rf_set_tx_rx_off();
            rf_start_stx((void *)ptr_factory, clock_time());
            reg_rf_irq_status = 0xffff;
            send_data_num++;
            if (send_data_num >= 96)
            {
                if (factory_channel == 0x03)
                    factory_channel = 0x07;
                else if (factory_channel == 0x07)
                    factory_channel = 0x0b;
                else if (factory_channel == 0x0b)
                    factory_channel = 0x0f;
                else if (factory_channel == 0x0f)
                    factory_channel = 0x03;

                send_data_num = 1;
                rf_set_tx_rx_off();
                rf_set_tx_rx_setting();
                PIN_DEBUG_RF_TX_LEVEL(1);
                // rf_set_channel(rf_chn[host_channel], 0);
                rf_set_channel_aaa(factory_channel, RF_CHN_TABLE);
            }
        }
    }

    if (factory_cmd > 0)
    {
        if (factory_cmd >= 1 && factory_cmd <= 30)
        {
            factory_cmd++;
        }
        else if (factory_cmd >= 30 && factory_cmd <= 60)
        {
            factory_cmd++;
        }
        else
        {
            if (factory_channel_begin == 1)
                factory_channel_begin = 2;

            if (factory_signle_begin == 1)
            {
                factory_signle_begin = 0;
                bath_rcu_enter_single_tone();
            }
            if (factory_exit_begin == 1)
            {
                factory_exit_begin = 0;
                factory_exit_flag = true;
            }
            if (ota_enable_begin == 1)
            {
                ota_enable_begin = 2;
                factory_channel_mode = 2;
                factory_channel_begin = 1;
                OTA_fACTORY_ENABLE_EVENT();
            }

            factory_cmd = 0;
        }
        Send_24G_Data_Flag = 0x01;
    }
    else
        Send_24G_Data_Flag = 0x00;

    if (Send_24G_Data_Flag == 0x01)
    {
        rf_state = RF_TX_START_STATUS;
        device_ack_received = 0;
        PIN_DEBUG_UI_TIME_LEVEL(1);
        rf_set_tx_pipe(0);
        rf_set_tx_rx_off();
        rf_start_stx((void *)ptr_factory, clock_time());
        reg_rf_irq_status = 0xffff;
        WaitMs(5);
        Send_24G_Data_Flag = 0x00;
    }
    else
    {
        rf_set_tx_rx_off();
        rf_set_tx_rx_setting();
        PIN_DEBUG_RF_TX_LEVEL(1);
        // rf_set_channel(rf_chn[host_channel], 0);
        rf_set_channel_aaa(factory_channel, RF_CHN_TABLE);
        rf_set_rxmode();
        rf_state = RF_RX_START_STATUS;
        // my_rf_start_srx(clock_time(), rf_rx_timeout_us);
    }

#if DEVICE_WACK_ENABLE
    if (factory_exit_flag == true && Send_24G_Data_Flag == 0x00)
    {
        production_testing = 0; // 退出产测标志
        factory_channel_mode = 0;
        exit_factory_test = 1;
        led_pwm_all_off();
        WaitMs(10);
        dev_now_wake_function();
    }
#endif
#endif
}

/******************************************************************
 * @brief  bath_rcu_enter_single_tone
 * @param  none
 * @return none
 * @retval void
 */
void bath_rcu_enter_single_tone(void)
{
    printf("--->>> [work_debug] bath_rcu_enter_single_tone\n");
    led_pwm_all_open();
    rf_emi_single_tone(BIT(7) | 63, 42);
    WaitMs(2000);

    rf_emi_stop();

    printf("--->>> [work_debug] bath_rcu_enter_single_tone OVER!!!\n");
    rf_drv_private_2m_init();
    // d24_user_init();
    irq_enable();
    rf_set_power_level_index(RF_POWER_P11p46dBm);

    app_rcu_factory_test();
    set_pair_access_code(0x39517695);
    rf_set_tx_rx_off();
    rf_set_tx_rx_setting();
    PIN_DEBUG_RF_TX_LEVEL(1);
    rf_set_channel_aaa(factory_channel, RF_CHN_TABLE);
    rf_set_rxmode();
    WaitMs(100);
    led_pwm_all_off();
}

/******************************************************************
 * @brief  strToInt
 * @param  none
 * @return none
 * @retval void
 */
int strToInt(const char *str)
{
    int result = 0;
    int sign = 1;
    int i = 0;
    if (str[0] == '+' || str[0] == '-')
    {
        if (str[0] == '-')
        {
            sign = -1;
        }
        i++;
    }
    while (str[i] != '\0')
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            result = result * 10 + (str[i] - '0');
        }
        else
        {
            break;
        }
        i++;
    }
    return result * sign;
}

void factory_test_check(void)
{
    if (production_testing)
    {
        dev_wake_time_tick = 0; // 关闭3秒睡眠
        if (tick_factory_test_check && clock_time_exceed(tick_factory_test_check, 120 * 1000 * 1000))
        {
            tick_factory_test_check = 0;
            production_testing = 0;
            factory_channel_mode = 0;
            exit_factory_test = 1;
            led_pwm_all_off();
            printf("exit_production_testing\r\n");
#if DEVICE_WACK_ENABLE
            WaitMs(100);
            dev_now_wake_function();
#endif
        }
    }
}

/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/
#endif
#endif
