#include "ota_slaver.h"
#include "driver.h"
#include "factory_test.h"
#include "AAA_24G_rf_frame.h"
#include "AAA_app_config.h"
#include "AAA_project_config.h"
#include "types.h"
#include "pwm_handle.h"
#include "key_driver.h"

// 变量声明
volatile u8 ita_retransmissions = 0;
volatile u16 ota_led_pwm_up_Hz = 1000;
volatile u16 address_offset = 0;
volatile u32 address_offset_sum = 0;
volatile u32 address_offset_turn = 0;
volatile u32 ota_send_neef = 0;
volatile u32 ota_send_sum = 0;
volatile u32 Heartbeat_clock = 0;

// OTA时间变量声明
volatile u32 ota_start_time = 0;

bool read_firw_flag = true;
bool ota_slaver_start = false;
bool ota_led_pwm_turn_flag = false;
bool OTA_FAIL_FLAG = false;
bool OTA_REQUEST_SUCCESS_FLAG = false;

u32 FIRMWARE_END_FLAG = 0xffff0000;
u32 OTA_FLASH_FWIR_FLAG = 0x00;
u32 OTA_SUCCESS_NUM = 0x00;
u32 OTA_CURRENT_VERSION = 0x00;
u32 OTA_MAC_ADDRESS = 0xffffffff;
u8 OTA_WRITE_NUM = 0x00;
u8 OTA_ERASE_NUM = 0x00;

// 管道
u8 ota_host_channel = 0;
bool ota_channel_flag = false;
// 结构体声明
SEND_OTA_DATE Send_ota_date;
ACK_OTA_DATE *ota_ack_date;
OTA_WAKE_STATE Ota_wake_state = OTA_WAKE_STATE_IDLE;

u8 DRIVER_ADDRESS_CLEAN = 0x00;

// 产测_OTA使能标志位
volatile u8  ota_enable_begin = 0;
u8 address[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
// OTA 主循环
void OTA_LOOP(void)
{
    OTA_SLAVER_START();
    Channel_modulation();
}

// OTA 呼吸灯
void OTA_BREACH_LED_SHOW(void)
{
    static u32 tick;
    if (clock_time_exceed(tick, 5 * 100 * 1000))
    {
        tick = clock_time();
        if (ota_led_pwm_turn_flag == true)
        {
            led_pwm_all_open();
            ota_led_pwm_turn_flag = false;
        }
        else
        {
            led_pwm_all_off();
            ota_led_pwm_turn_flag = true;
        }
    }
}

// OTA 调频处理
void Channel_modulation(void)
{
    check_rf_fast_setting_time();

    if (ota_channel_flag == true)
    {
        if (host_channel == 0)
            host_channel = 14;
        else if (host_channel == 14)
            host_channel = 0;
        rf_set_tx_rx_off();
        rf_set_tx_rx_setting();
        rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
        rf_set_rxmode();
        ota_channel_flag = false;
        printf("host_channel:%d\r\n", host_channel);
    }
}

// OTA 初始化
void OTA_INIT(void)
{
    OTA_FIRMWARE_ADDRESS_CORREAT();
    OTA_FIRMWARE_ERASE_SECTOR();
    set_pair_access_code(0x39517695);
    Send_ota_date.OTA_DMA_LEN = OTA_SEND_DMA_LEN;
    Send_ota_date.OTA_RF_LEN = OTA_SEND_RF_LEN;
    Send_ota_date.SOFTWARE_ID = OTA_SOFTWARE_ID;
    rf_set_power_level_index(129);
    host_channel = 0;
    rf_set_tx_rx_off();
    rf_set_tx_rx_setting();
    rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
    rf_set_rxmode();
}

// OTA版本地址、MAC地址确定
void OTA_FIRMWARE_ADDRESS_CORREAT(void)
{

    // flash_read_page(OTA_FIRMWARE_FLASH_ADDRESS, sizeof(OTA_FLASH_FWIR_FLAG), (u8 *)&OTA_FLASH_FWIR_FLAG);
    // if (OTA_FLASH_FWIR_FLAG == U32_MAX)
    // {
    //     flash_write_page(OTA_FIRMWARE_FLASH_ADDRESS, sizeof(OTA_SUCCESS_NUM), (u8 *)&OTA_SUCCESS_NUM);
    // }
    // else
    // {
    //     OTA_SUCCESS_NUM = OTA_FLASH_FWIR_FLAG;
    // }

    // OTA_CURRENT_VERSION = OTA_SUCCESS_NUM;
    // OTA_ERASE_NUM = OTA_CURRENT_VERSION % 2;
    // OTA_WRITE_NUM = (OTA_CURRENT_VERSION + 1) % 2;

    // printf("--->OTA_CURRENT_VERSION %4x\r\n", OTA_FLASH_FWIR_FLAG);
    // printf("--->OTA_MAC_FLASH_ADDRESS %4x\r\n", OTA_MAC_FLASH_ADDRESS);
    // printf("--->OTA_ERASE_NUM %d\r\n", OTA_ERASE_NUM);
    // printf("--->OTA_WRITE_NUM %d\r\n", OTA_WRITE_NUM);

    static u8 ota_update_date_0 = 0xff;
    static u8 ota_update_date_1 = 0xff;
    flash_read_page(OTA_MAC_FLASH_ADDRESS, sizeof(OTA_MAC_ADDRESS), (u8 *)&OTA_MAC_ADDRESS);
    flash_read_page(OTA_FLASH_START_ADD_FIRM_0 + DRIVE_ADDR_OFFSER, sizeof(ota_update_date_0), (u8 *)&ota_update_date_0);
    flash_read_page(OTA_FLASH_START_ADD_FIRM + DRIVE_ADDR_OFFSER, sizeof(ota_update_date_1), (u8 *)&ota_update_date_1);
    if (ota_update_date_0 == 0x4b)
    {
        OTA_ERASE_NUM = 0x01;
        OTA_WRITE_NUM = 0x01;
    }
    else if (ota_update_date_1 == 0x4b)
    {
        OTA_ERASE_NUM = 0x00;
        OTA_WRITE_NUM = 0x00;
    }
}

_attribute_ram_code_sec_ void OTA_UPDATE_IRP_HANDLE(u8 *raw_pkt, rf_packet_t *p_rf_rx_data)
{
#if OTA_UPDATE_ENABLE
#if OTA_EMI_RSSI_ENABLE
    unsigned int ota_emi_rssibuf = 0;
    ota_emi_rssibuf = (read_reg8(0x449));
    if (ota_emi_rssibuf < 55)
    {
        // printf("RX: emi_rssibuf low== %d\r\n", emi_rssibuf);
        return;
    }
#endif
    if (RF_TPLL_PACKET_CRC_OK(raw_pkt) && RF_TPLL_PACKET_LENGTH_OK(raw_pkt))
    {
        PIN_DEBUG_RF_RX_CRC_OK_TOGGLE;
        ota_ack_date = (ACK_OTA_DATE *)&p_rf_rx_data->dat[0];
        if (ota_ack_date->OTA_MCK_ADDR == OTA_MAC_ADDRESS && ota_ack_date->OTA_LEN_CHECK == address_offset)
        {
            rf_set_tx_rx_off();
            WIRTE_OTA_FIRMWARE_DATE(ota_ack_date);
            ++address_offset;
            Heartbeat_clock = 0;
            ota_send_neef = 1;
            ota_enable_begin = 0;
            printf("ack_address_offset: %d\r\n", ota_ack_date->OTA_LEN_CHECK);
        }
        else if (ota_ack_date->OTA_MCK_ADDR == OTA_SOFTWARE_ID && ota_ack_date->OTA_LEN_CHECK == 0x00 && OTA_REQUEST_SUCCESS_FLAG == false && ota_slaver_start == true)
        {
            rf_set_tx_rx_off();
            Ota_wake_state = OTA_WAKE_STATE_SEND_DATA;
            Send_ota_date.SOFTWARE_ID = OTA_SOFTWARE_ID;
            Send_ota_date.OTA_MAC_ADDR = OTA_MAC_ADDRESS;
            ota_ack_date->OTA_MCK_ADDR = 0;
            WIRTE_OTA_FIRMWARE_DATE(ota_ack_date);
            OTA_REQUEST_SUCCESS_FLAG = true;
            ++address_offset;
            Heartbeat_clock = 0;
            ota_send_neef = 1;
            printf("ack_address_offset: %d\r\n", ota_ack_date->OTA_LEN_CHECK);
        }
        else
        {
        }
    }
#endif
}

// OTA 数据接收失败
_attribute_ram_code_sec_ void OTA_ACK_FAIL_EVENT(void)
{
    rf_set_tx_rx_off();
    Ota_wake_state = OTA_WAKE_STATE_IDLE;
    for (int i = 0; i < OTA_ERASE_LEN; i++)
        flash_erase_sector(OTA_FLASH_START_ADD_FIRM * OTA_WRITE_NUM + i * OTA_ERASE_BYTE_SIZE);
    printf("OTA_UPDATE_ERROR\r\n");
    led_pwm_status_open(16);
    irq_disable();
    WaitMs(1000);
    start_reboot();
    while (1)
        ;
}

// OTA 数据接收成功
_attribute_ram_code_sec_ void OTA_FIRMWARE_DATE_ACK_END(void)
{
    rf_set_tx_rx_off();
    static u8 OTA_SUCCESS_FLAG = 0x4B & 0xff;
    static u8 FIRW_EREASE = 0x00;
    Ota_wake_state = OTA_WAKE_STATE_IDLE;
    read_firw_flag = true;
    OTA_SUCCESS_NUM = OTA_SUCCESS_NUM + 1;
    if (OTA_WRITE_NUM == 0x01)
        FIRW_EREASE = 0x00;
    else if (OTA_WRITE_NUM == 0x00)
        FIRW_EREASE = 0x01;

    printf("--->OTA_SUCCESS_FLAG: %1x", OTA_SUCCESS_FLAG);
    flash_write_page(OTA_FIRMWARE_FLASH_ADDRESS, sizeof(OTA_SUCCESS_NUM), (u8 *)&OTA_SUCCESS_NUM);
    flash_write_page(OTA_FLASH_START_ADD_FIRM * OTA_WRITE_NUM + DRIVE_ADDR_OFFSER, sizeof(OTA_SUCCESS_FLAG), (u8 *)&OTA_SUCCESS_FLAG);
    flash_write_page(OTA_FLASH_START_ADD_FIRM * FIRW_EREASE + DRIVE_ADDR_OFFSER, sizeof(DRIVER_ADDRESS_CLEAN), (u8 *)&DRIVER_ADDRESS_CLEAN);
    printf("---->FIRMWARE_SUCCESS_FLAG:\r\n");
    irq_disable();
    WaitMs(1000);
    start_reboot();
    while (1)
        ;
}

// OAT 成功固件抹除
void OTA_FIRMWARE_ERASE_SECTOR(void)
{
#if OTA_UPDATE_ENABLE
    static u8 Ota_erase_start_address = 0xff;
    static u8 ota_erase_head_address = 0x00;
    flash_read_page(OTA_FLASH_START_ADD_FIRM * OTA_WRITE_NUM + DEVICE_DRIVER_ADDRESS, sizeof(Ota_erase_start_address), (u8 *)&Ota_erase_start_address);
    flash_read_page(OTA_FLASH_START_ADD_FIRM * OTA_WRITE_NUM, sizeof(ota_erase_head_address), (u8 *)&ota_erase_head_address);
    // printf("---> OTA_FIRMWARE_ERASE_SECTOR %d\r\n", Ota_erase_start_address);

    if (Ota_erase_start_address != 0x4b && ota_erase_head_address != 0xff)
    {
        for (int i = 0; i < OTA_ERASE_LEN; i++)
            flash_erase_sector(OTA_FLASH_START_ADD_FIRM * OTA_WRITE_NUM + i * OTA_ERASE_BYTE_SIZE);
    }

#endif
}

// 写入固件数据
_attribute_ram_code_sec_ void WIRTE_OTA_FIRMWARE_DATE(ACK_OTA_DATE *ota_ack_date)
{
    if (address_offset != 0)
    {
        flash_write_page(OTA_FLASH_START_ADD_FIRM * OTA_WRITE_NUM + address_offset * READ_FLASH_SIZE, READ_FLASH_SIZE, (u8 *)&ota_ack_date->OTA_DATA_PACKET);
    }
    if (address_offset == 0)
    {
        ota_ack_date->OTA_DATA_PACKET[8] = 0xff;
        flash_write_page(OTA_FLASH_START_ADD_FIRM * OTA_WRITE_NUM + address_offset * READ_FLASH_SIZE, READ_FLASH_SIZE, (u8 *)&ota_ack_date->OTA_DATA_PACKET);
    }
    else if (address_offset == 1)
    {
        FIRMWARE_END_FLAG |= (ota_ack_date->OTA_DATA_PACKET[9] << 8);
        FIRMWARE_END_FLAG |= ota_ack_date->OTA_DATA_PACKET[8];
        FIRMWARE_END_FLAG &= 0x0000ffff;
        FIRMWARE_END_FLAG = FIRMWARE_END_FLAG / READ_FLASH_SIZE;
        FIRMWARE_END_FLAG = FIRMWARE_END_FLAG + 1;
        printf("FIRMWARE_END_FLAG :%d\r\n", FIRMWARE_END_FLAG);
    }
}

// OTA 射频发送
_attribute_ram_code_sec_ void OTA_RF_TX(void)
{
    if (ota_send_neef == 1 || ota_send_neef % OTA_SEND_DMA_LEN == OTA_SEND_DMA_LEN + 1)
        rf_set_tx_rx_off();
    rf_set_tx_pipe(0);
    rf_start_stx((void *)&Send_ota_date.OTA_DMA_LEN, clock_time());
    reg_rf_irq_status = 0xffff;
}

_attribute_ram_code_sec_ void OTA_SLAVER_START(void)
{
    if (ota_slaver_start == true)
    {
        if (Ota_wake_state == OTA_WAKE_STATE_SEND_DATA)
        {
            OTA_BREACH_LED_SHOW();
            Send_ota_date.OTA_LEN_CHECK = address_offset - 1;
            if (ota_send_neef)
            {
                OTA_RF_TX();
                ota_send_neef++;
                if (ota_send_neef > 15 * OTA_SEND_DMA_LEN)
                {
                    ota_send_neef = 0;
                    Heartbeat_clock = clock_time();
                }
            }

            if (Heartbeat_clock && clock_time_exceed(Heartbeat_clock, 1 * 10 * 1000))
            {
                Heartbeat_clock = 0;
                // printf("clock_time_exceed(Heartbeat_clock,100000)\r\n");
                if (address_offset != 0)
                    ota_send_neef = 1;
            }

            if (address_offset != 0)
            {
                if (address_offset_turn == 0)
                {
                    address_offset_turn = address_offset;
                }
                if (address_offset_turn == address_offset)
                {
                    address_offset_sum++;
                }
                else
                {
                    printf("\r\n");
                    ita_retransmissions = 0;
                    address_offset_turn = 0;
                    address_offset_sum = 0;
                }
                if (address_offset_sum >= OTA_FREQ_INTERVAL && ota_channel_flag == false)
                {
                    printf("address_offset_sum:%d\r\n", address_offset_sum);
                    address_offset_turn = 0;
                    address_offset_sum = 0;
                    ota_channel_flag = true;
                    ita_retransmissions++;
                }
                if (ita_retransmissions >= OTA_ACK_RETRANMISSION)
                {
                    OTA_ACK_FAIL_EVENT();
                }
            }
        }
        else if (Ota_wake_state != OTA_WAKE_STATE_SEND_DATA && ota_start_time && clock_time_exceed(ota_start_time, 10 * 1000 * 1000))
        {
            ota_start_time = 0;
            led_pwm_status_open(16);
            irq_disable();
            WaitMs(1000);
            start_reboot();
            while (1)
                ;
        }

        if (address_offset > FIRMWARE_END_FLAG && Ota_wake_state == OTA_WAKE_STATE_SEND_DATA)
        {
            OTA_FIRMWARE_DATE_ACK_END();
        }
    }
}

// ----------------------------------------------------------------//
//                   OTA_SLAVER使用说明                             //
// ----------------------------------------------------------------//

// 1、添加相应头文件               #include "ota_slaver.h"
// 2、添加初始化函数               OTA_INIT();
// 3、添加函数到中断函数 	       OTA_UPDATE_IRP_HANDLE(raw_pkt, p_rf_rx_data);
// 4、添加OTA主循环               OTA_LOOP(); （注意：与main函数的main_loop以标志位ota_slaver_start选择主循环）
// 5、填按键自定义OTA驱动事件      powered_on_press_1s_S3_S10_enter_ota_update_event()

// 按键驱动ota事件添加（添加到按键功能事件）
/*
void powered_on_press_1s_S3_S10_enter_ota_update_event()
{
    if (ota_slaver_start == false)
    {
        sleep_ms(30);
        if ((kb_code.kb_now_code[0] == KB_CODE_S3 && kb_code.kb_now_code[1] == KB_CODE_S10) ||
            (kb_code.kb_now_code[0] == KB_CODE_S10 && kb_code.kb_now_code[1] == KB_CODE_S3))
        {
            printf("--->>> led_pwm_init_all_led();\r\n");
            led_pwm_init_all_led();
            ota_start_time = clock_time();
            dev_wake_time_tick = 0; // 关闭3秒睡眠
            ota_slaver_start = true;
            led_pwm_all_open();
            printf("--->>>  into_ota_wait\r\n");
            key_cur_type = NO_ACTION;
        }
    }
}
*/



// 产测驱动OTA事件
void OTA_fACTORY_ENABLE_EVENT()
{
    led_pwm_init_all_led();
    ota_start_time = clock_time();
    dev_wake_time_tick = 0; // 关闭3秒睡眠
    led_pwm_all_open();
    WaitMs(100);
    rf_set_power_level_index(129);
    ota_slaver_start = true;
    host_channel = 0;
    rf_set_tx_rx_off();
    rf_set_tx_rx_setting();
    rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
    rf_set_rxmode();
    printf("--->>>  into_ota_wait\r\n");
}
