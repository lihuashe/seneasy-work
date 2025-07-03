/******************************************************************************
 * @file     app.c
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

#include "application/keyboard/keyboard.h"

#include "vendor/common/blt_led.h"
#include "vendor/common/blt_soft_timer.h"
#include "vendor/common/blt_common.h"

#include "app_ui.h"
#include "battery_check.h"
#include "rc_ir.h"
#include "app_audio.h"
#include "app_flash_write.h"
#include "application/audio/gl_audio.h"
#include "../ota_module/app_ota.h"
#include "app_ir.h"
#include "app_custom.h"
#include "app_config.h"
#include "app_flash_write.h"
#include "app_custom_lib.h"
#include "app_test.h"
#include "./app_find_me/app_fms.h"
#include "./key_module/key_handle.h"
#include "./key_module/key_driver.h"
#include "./led_module/led_driver.h"
#include "./profile_module/ota_profile.h"
#include "app_rcu.h"
#include "../led_module/backlight_handle.h"
#include "../led_module/aw21036_driver.h"
#include "../led_module/bct3236_driver.h"
#include "../profile_module/dis_profile.h"
#include "../led_module/ltr_sensor.h"
#include "../ir_module/ir_driver.h"
#include "../profile_module/seneasy_profile.h"

_attribute_data_retention_    own_addr_type_t   app_own_address_type = OWN_ADDRESS_PUBLIC;



//#define RX_FIFO_SIZE            96                    //-24//64
//#define RX_FIFO_SIZE           160                    //-24//64
#define RX_FIFO_SIZE             160                    //-24//64
#define RX_FIFO_NUM                8

//#define TX_FIFO_SIZE            88                    //-12//40
#if AUDIO_LTVV
#define TX_FIFO_SIZE             148//188
#else
#define TX_FIFO_SIZE             148                    //-12//40
#endif
#if !CLOSE_CODE_RAM
#define TX_FIFO_NUM               16
#else
#define TX_FIFO_NUM                8
#endif
//#define MTU_SIZE_SETTING        72
#define MTU_SIZE_SETTING         180
//#define MTU_SIZE_SETTING       210

#if 0
    MYFIFO_INIT(blt_rxfifo, RX_FIFO_SIZE, RX_FIFO_NUM);
#else
_attribute_data_retention_  u8              blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_    my_fifo_t    blt_rxfifo = {
                                                RX_FIFO_SIZE,
                                                RX_FIFO_NUM,
                                                0,
                                                0,
                                                blt_rxfifo_b,};
#endif


#if 0
    MYFIFO_INIT(blt_txfifo, TX_FIFO_SIZE, TX_FIFO_NUM);
#else
    _attribute_data_retention_  u8              blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
    _attribute_data_retention_    my_fifo_t    blt_txfifo = {
                                                    TX_FIFO_SIZE,
                                                    TX_FIFO_NUM,
                                                    0,
                                                    0,
                                                    blt_txfifo_b,};
#endif


_attribute_data_retention_  u8  flag_dle=0;
_attribute_data_retention_  u8  device_in_connection_state=0;
_attribute_data_retention_  u32 advertise_begin_tick;
_attribute_data_retention_  u8  advertise_downgrade_flag=0;
_attribute_data_retention_  u8  sendTerminate_before_enterDeep = 0;
_attribute_data_retention_  u32 latest_user_event_tick;
_attribute_data_retention_  u32 lowBattDet_tick   = 0;
_attribute_data_retention_  u32 mtuExchange_check_tick=0;
_attribute_data_retention_  u32 mtuExchange_check_timer=12000000;
_attribute_data_retention_  u32 ir_flash_erase_tick=0;
_attribute_data_retention_  u32 delay3s_to_sleep_tick=0;
_attribute_data_retention_  u16 enterdeep_time_count = 0;
_attribute_data_retention_  u16 enterdeep_time = 0;
_attribute_data_retention_  u16 service_change = 0;
_attribute_data_retention_  u8  device_timeout_state=0;
_attribute_data_retention_  u8  flag_schedule_ota=0;
_attribute_data_retention_  u16 count_schedule_ota=0;
_attribute_data_retention_  u8  app_slave_terminate=0;
_attribute_data_retention_  u8  app_is_set_mtusize=0;
#if !CLOSE_CODE_RAM
_attribute_data_retention_  u8  prepareWrite_b[300];
#endif
//_attribute_data_retention_  u8  wakeup_src=1;               //1:wakeup for pad  2:wake up for timer
//_attribute_data_retention_  u8  power_down_flag=1;          //1: the rcu previous state is power down 2: the rcu previous state is deep
//_attribute_data_retention_  u8  wakeup_key_send_flag=0;     //1: sending wake up packet 2:schedule ota flag
//_attribute_data_retention_  u8  wakeup_key_keyid = 0xff;         //Save the wake-up key keyid
_attribute_data_retention_  u8  wakeup_key_keycount=0;
//_attribute_data_retention_  u32 wakeup_packet_format=0x55;
//_attribute_data_retention_  u32 wakeup_tick=0;
_attribute_data_retention_  u8  detpowerkey=0;              //0:not det powerkey  1:det powerkey


_attribute_data_retention_ u8 app_rcu_tx_power = RF_POWER_P3p50dBm;
_attribute_data_retention_ u8 app_rcu_CouchMode = 0;



_attribute_data_retention_ u8 mac_public[6];

extern u8 is_pairing_mode;
extern u8 is_reconn_mode;
extern u8 ir_flash_erase_flag;

extern void app_pairing_led(void);
extern ble_sts_t    blc_att_requestMtuSizeExchange (u16 connHandle, u16 mtu_size);
extern void is_flash_info_full(void);
extern void battery_refresh(void);
extern void device_led_off(u8 led);
extern void app_connect_led(void);
extern void ir_flash_set_flag(u8 flag);
extern u8 init_ir_key_event_notify(void);
extern int bls_smp_param_deleteByIndex(u8 index);

#if (TL_SZ119_TVSSU)
    extern unsigned char tv_ssu_busy(void);
    extern void SIMPLE_SETUP_Handle(unsigned short SSU_handle);
    extern void simple_setup_key_register(const unsigned char SSU_key_table[36]);
    extern void SSU_flash_address_register(unsigned int address);
    extern void register_IR_TX_callback(unsigned char(*ssu_isuserIRCodeTxing)(void));
    extern void register_IR_PWM_frequency(unsigned int clock_cnt);
    extern void simple_setup_search_proc(unsigned char low_battery, unsigned char device_in_connection_state);  
    extern _attribute_data_retention_  u8  batt_status;
    u8 low_battery=0;
/*
                    SSU_KEY: SSU_KEY【0~37  =   DIGIT_0,    DIGIT_1,    DIGIT_2,    DIGIT_3,    DIGIT_4,    DIGIT_5, 
                                                DIGIT_6,    DIGIT_7,    DIGIT_8,    DIGIT_9,    PLAY,       PAUSE, 
                                                STOP,       FAST_FORWARD,REWIND,    RECORD,     RED,        GREEN, 
                                                YELLOW,     BLUE,       CURSOR_UP,  CURSOR_DN,  CURSOR_LT,  CURSOR_RT ,
                                                OK,         CHANNEL_UP, CHANNEL_DOWN,INFO,      GUIDE,      AUDIO_DESCRIPTION, 
                                                BACK,       HOME,       LIST,       VOL_UP,     VOL_DN,     MUTE,      SUBTITLE】
*/
    const unsigned char SSU_KEY_HIDDEN[37] = {  KEY_NUM_0,  KEY_NUM_1,  KEY_NUM_2,  KEY_NUM_3,  KEY_NUM_4,  KEY_NUM_5,  \
                                                KEY_NUM_6,  KEY_NUM_7,  KEY_NUM_8,  KEY_NUM_9,  KEY_PLAY,   KEY_PAUSE,  \
                                                0xFF,       KEY_FFWD,   KEY_REWIND, 0xFF,       0xFF,       0xFF,       \
                                                0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       0xFF ,      \
                                                0xFF,       KEY_CHN_UP, KEY_CHN_DN, 0xFF,       0xFF,       0xFF,       \
                                                KEY_BACK,   0xFF,       KEY_LIST,   KEY_VOL_UP, KEY_VOL_DN, KEY_MUTE,   0xFF};

    const unsigned char SSU_KEY_NORMAL_SUBTITLE[37] = { 0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       \
                                                        0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       \
                                                        0xFF,       0xFF,       0xFF,       0xFF,       KEY_RED,    KEY_GREEN,  \
                                                        KEY_YELLOW, KEY_BLUE,   KEY_UP,     KEY_DOWN,   KEY_LEFT,   KEY_RIGHT , \
                                                        KEY_CENTER, KEY_CHN_UP, KEY_CHN_DN, KEY_INFO,   KEY_GUIDE,  0xFF,       \
                                                        KEY_BACK,   0xFF,       KEY_LIST,   KEY_VOL_UP, KEY_VOL_DN, KEY_MUTE,   KEY_SUBTITLE};    
    const unsigned char SSU_KEY_NORMAL_CC[37] = {   0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       \
                                                        0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       \
                                                        0xFF,       0xFF,       0xFF,       0xFF,       KEY_RED,    KEY_GREEN,  \
                                                        KEY_YELLOW, KEY_BLUE,   KEY_UP,     KEY_DOWN,   KEY_LEFT,   KEY_RIGHT , \
                                                        KEY_CENTER, KEY_CHN_UP, KEY_CHN_DN, KEY_INFO,   KEY_GUIDE,  0xFF,       \
                                                        KEY_BACK,   0xFF,       KEY_LIST,   KEY_VOL_UP, KEY_VOL_DN, KEY_MUTE,   0xFF};   
    const unsigned char SSU_KEY_NORMAL_AD[37] = {   0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       \
                                                        0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       0xFF,       \
                                                        0xFF,       0xFF,       0xFF,       0xFF,       KEY_RED,    KEY_GREEN,  \
                                                        KEY_YELLOW, KEY_BLUE,   KEY_UP,     KEY_DOWN,   KEY_LEFT,   KEY_RIGHT , \
                                                        KEY_CENTER, KEY_CHN_UP, KEY_CHN_DN, KEY_INFO,   KEY_GUIDE,  KEY_SUBTITLE,\
                                                        KEY_BACK,   0xFF,       KEY_LIST,   KEY_VOL_UP, KEY_VOL_DN, KEY_MUTE,   0xFF};                   
#endif

#if (UART_PRINT_ENABLE)
_attribute_data_retention_    u8 printf_uart_flag=0;

void app_set_printf_uart_flag(unsigned char data)
{
    printf_uart_flag = data;
}
#endif

void app_rcu_set_TxPower(u8 Index){

    switch (Index) {
        case RF_POWER_P11p26dBm :
        case RF_POWER_P11p09dBm :
        case RF_POWER_P10p83dBm :
        case RF_POWER_P10p62dBm :
        case RF_POWER_P10p30dBm :
        case RF_POWER_P10p05dBm :
        case RF_POWER_P9p79dBm :
        case RF_POWER_P9p54dBm :
        case RF_POWER_P9p23dBm :
        case RF_POWER_P8p92dBm :
        case RF_POWER_P8p57dBm :
        case RF_POWER_P8p20dBm :
        case RF_POWER_P7p80dBm :
        case RF_POWER_P7p37dBm :
        case RF_POWER_P6p91dBm :
        case RF_POWER_P6p45dBm :
        case RF_POWER_P5p92dBm :
        case RF_POWER_P5p33dBm :
        case RF_POWER_P4p69dBm :
        case RF_POWER_P3p99dBm :
        case RF_POWER_P3p50dBm :
        case RF_POWER_P3p33dBm :
        case RF_POWER_P3p13dBm :
        case RF_POWER_P2p93dBm :
        case RF_POWER_P2p60dBm :
        case RF_POWER_P2p36dBm :
        case RF_POWER_P2p10dBm :
        case RF_POWER_P1p83dBm :
        case RF_POWER_P1p56dBm :
        case RF_POWER_P1p25dBm :
        case RF_POWER_P0p71dBm :
        case RF_POWER_P0p52dBm :
        case RF_POWER_N0p28dBm :
        case RF_POWER_N0p51dBm :
        case RF_POWER_N0p74dBm :
        case RF_POWER_N1p21dBm :
        case RF_POWER_N1p69dBm :
        case RF_POWER_N2p23dBm :
        case RF_POWER_N2p84dBm :
        case RF_POWER_N3p48dBm :
        case RF_POWER_N4p18dBm :
        case RF_POWER_N4p97dBm :
        case RF_POWER_N5p85dBm :
        case RF_POWER_N6p83dBm :
        case RF_POWER_N7p88dBm :
        case RF_POWER_N9p14dBm :
        case RF_POWER_N10p70dBm :
        case RF_POWER_N12p57dBm :
        case RF_POWER_N15p01dBm :
        case RF_POWER_N18p40dBm :
        case RF_POWER_N24p28dBm :
            app_rcu_tx_power = Index;
            break;
        default:
            app_rcu_tx_power = RF_POWER_P3p50dBm;
            break;
    }

#if 0
    /* set TX power index to hardware revision */
    extern u8 my_HWRevtrs [];
    my_HWRevtrs[5] = (app_rcu_tx_power&BIT(7))?'A':'B';
    my_HWRevtrs[6] = (app_rcu_tx_power&0x3F)/10 + '0';
    my_HWRevtrs[7] = (app_rcu_tx_power&0x3F)%10 + '0';

    printf("[NOTICE] Set TX power[%c%c%c]\n",my_HWRevtrs[5],my_HWRevtrs[6],my_HWRevtrs[7]);
#endif
}


void app_enter_deep_timeset(void)
{
    enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
}

void advertise_tick_retime(void)
{
    advertise_begin_tick = clock_time();
    advertise_downgrade_flag = 0;
    enterdeep_time_count = 0;
}

//////////////////////////////////////////////////////////////////////////////
//     Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
const u8    tbl_advData_g10_part2[] = {
     0x02, 0x01, 0x05,                          // BLE limited discoverable mode and BR/EDR not supported
     0x03, 0x19, 0x80, 0x01,                    // 384, Generic Remote Control, Generic category
     0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,        // incomplete list of service class UUIDs (0x1812, 0x180F)
};

void app_set_advdata_rspdata(void)
{
#if 0
    //printf("%s \n ", __FUNCTION__);
    u8 tbl_scanRsp[30];
    u8 tbl_advData[40];
    u8 len=0;

    tbl_scanRsp[0] = _custom_data.device_name_length+1;
    tbl_scanRsp[1] = 0x09;
    memcpy(&tbl_scanRsp[2],_custom_data.device_name,_custom_data.device_name_length);

    tbl_advData[0] = tbl_scanRsp[0];
    tbl_advData[1] = 0x09;
    memcpy(&tbl_advData[2],_custom_data.device_name,_custom_data.device_name_length);
    len = 2 + _custom_data.device_name_length;

    memcpy(&tbl_advData[len],tbl_advData_g10_part2,sizeof(tbl_advData_g10_part2));
    len += sizeof(tbl_advData_g10_part2);

    bls_ll_setAdvData( (u8 *)tbl_advData, len);
    bls_ll_setScanRspData( (u8 *)tbl_scanRsp, _custom_data.device_name_length+2);
#endif

    bls_ll_setAdvData( (u8 *)tbl_advData_TPV, sizeof(tbl_advData_TPV));
    bls_ll_setScanRspData( (u8 *)tbl_scanRsp_TPV, sizeof(tbl_scanRsp_TPV));

    //printf("adv data: ");
    //array_printf(tbl_advData,len);

    //printf("scan data: ");
    //array_printf(tbl_scanRsp, _custom_data.device_name_length+2);
}

void direct_adv(u8 e, u8 *p, int n)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("direct_adv\r\n");
    #endif
    is_reconn_mode = 0x55;
    bls_ll_setAdvEnable(1);  //must: set adv enable
}

#if (UART_PRINT_ENABLE)
void app_uart_tx(void)
{
    gpio_setup_up_down_resistor(UART_TX_PB1, PM_PIN_PULLUP_10K);  //must, for stability and prevent from current leakage
    gpio_set_func(UART_TX_PB1,AS_UART); // set tx pin
    gpio_set_input_en(UART_TX_PB1, 1);  //experiment shows that tx_pin should open input en(confirmed by qiuwei)
    uart_init_baudrate(1000000,16000000,PARITY_NONE, STOP_BIT_ONE); //baud rate: 115200
    uart_ndma_clear_tx_index();
    //uart_dma_enable(0, 0);
    //irq_disable_type(FLD_IRQ_DMA_EN);
    //dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 0);
    //uart_irq_enable(0,0);   //uart RX irq enable
    //uart_ndma_irq_triglevel(1,0);    //set the trig level. 1 indicate one byte will occur interrupt
}
#endif

void app_adv_interval_downgrade_direct(void)
{
    u8 status=0;
    smp_param_save_t  bondInfo;
    u8 rpa_flag=0;

    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    //direct adv
    if(bond_number)   //at least 1 bonding device exist
    {
        #if (!CLOSE_CODE_PRINTF)
       printf("app_adv_internal_downgrade_direct\r\n");
       #endif
       bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

       if (memcmp(bondInfo.peer_id_addr, bondInfo.peer_addr, 6) == 0)
       {
           rpa_flag = 0;
       }
       else
       {
        #if (!CLOSE_CODE_PRINTF)
           printf("en_slave_rpa: %d \n ", en_slave_rpa);
        #endif
           if(en_slave_rpa == 0)
               rpa_flag = 0;
           else
               rpa_flag = 1;
       }
       if(rpa_flag == 0)
       {
         status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_35MS,
                                  ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                                  bondInfo.peer_addr_type,  bondInfo.peer_addr,
                                  MY_APP_ADV_CHANNEL,
                                  ADV_FP_NONE);
       }
       else
       {
           app_own_address_type = OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC;

           status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_35MS,
              ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
              bondInfo.peer_id_adrType,  bondInfo.peer_id_addr,
                                                 MY_APP_ADV_CHANNEL,
                                                 ADV_FP_NONE);
       }
       if(status != BLE_SUCCESS) {      while(1); }  //debug: adv setting err
    }
}

void app_adv_interval_downgrade_indirect(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("app_adv_internal_downgrade_indirect\r\n");
    #endif
    bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_35MS,
                        ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
                        0,  NULL,
                        MY_APP_ADV_CHANNEL,
                        ADV_FP_NONE);
    if(is_pairing_mode == 0x55)
    {
        is_pairing_mode = 0;
        is_reconn_mode = 0;
        app_schedule_ota_enter_deep();
    }
}

typedef void (*app_adv_interval_downgradeCb_t)(void);

_attribute_data_retention_ app_adv_interval_downgradeCb_t     app_adv_interval_downgradeCb = NULL;
void app_adv_interval_downgrade_register(app_adv_interval_downgradeCb_t cb)
{
    app_adv_interval_downgradeCb = cb;
}

void app_set_adv_interval_downgrade_indirect(void)
{
    app_adv_interval_downgrade_register(app_adv_interval_downgrade_indirect);
}

void app_set_adv_interval_downgrade_direct(void)
{
    app_adv_interval_downgrade_register(app_adv_interval_downgrade_direct);
}

void app_adv_indirect(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("app_adv_indirect\r\n");
    #endif
    app_set_advdata_rspdata();

    bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
        ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
        0,    NULL,
        MY_APP_ADV_CHANNEL,
        ADV_FP_NONE);
    bls_ll_setAdvEnable(1);
    is_pairing_mode = 0x55;
    app_set_adv_interval_downgrade_indirect();
}

void app_adv_direct(void)
{
    u8 status=0;
    u8 rpa_flag=0;
    #if (!CLOSE_CODE_PRINTF)
    printf("app_adv_direct\r\n");
    #endif
    is_reconn_mode = 0x55;
    smp_param_save_t  bondInfo;
    bls_ll_setAdvEnable(0);  //adv enable
    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    if(bond_number)   //at least 1 bonding device exist
    {
       app_set_adv_interval_downgrade_direct();
       bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

       if (memcmp(bondInfo.peer_id_addr, bondInfo.peer_addr, 6) == 0)
       {
           rpa_flag = 0;
       }
       else
       {
           if(en_slave_rpa == 0)
               rpa_flag = 0;
           else
               rpa_flag = 1;
       }
       if(rpa_flag == 0)
       {
           #if (!CLOSE_CODE_PRINTF)
           printf("host not support rpa\r\n");
           #endif
           status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                                    ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                                    bondInfo.peer_addr_type,  bondInfo.peer_addr,
                                    MY_APP_ADV_CHANNEL,
                                    ADV_FP_NONE);
       }
       else
       {
            #if (!CLOSE_CODE_PRINTF)
           printf("host support rpa\r\n");
           #endif
           ll_resolvingList_reset();
           status = ll_resolvingList_add(bondInfo.peer_id_adrType, bondInfo.peer_id_addr, bondInfo.peer_irk, bondInfo.local_irk);
           #if (!CLOSE_CODE_PRINTF)
           printf("LL resolving list add status: 0x%x\n", status);
           #endif

           status = ll_resolvingList_setAddrResolutionEnable(1);
           #if (!CLOSE_CODE_PRINTF)
           printf("LL add resolution enable status: 0x%x\n", status);
           #endif
           app_own_address_type = OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC;

           status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                bondInfo.peer_id_adrType,  bondInfo.peer_id_addr,
                                                   MY_APP_ADV_CHANNEL,
                                                   ADV_FP_NONE);
       }
       if(status != BLE_SUCCESS) {      while(1); }  //debug: adv setting err
       bls_ll_setAdvEnable(1);  //adv enable
    }
    else
    {
        app_set_adv_interval_downgrade_indirect();
        bls_ll_setAdvEnable(1);  //adv enable
    }
}

u8 effective_rx_dle = 0;
void    task_datalengthexchange (u8 e, u8 *p, int n)
{
    flag_dle |= 1;
    //printf("task_datalengthexchange\n");
    ll_data_extension_t *data_p = (ll_data_extension_t *)p;
    effective_rx_dle = data_p->connEffectiveMaxRxOctets;
    //printf("connEffective:rx:%d __ tx: %d\n",data_p->connEffectiveMaxRxOctets,data_p->connEffectiveMaxTxOctets);
#if BLE_AUDIO_ENABLE
    set_audio_frame_size(data_p->connMaxTxOctets);
#if (RCU_CONFIG_TPV)
    if (key_not_released == 0)
        app_rcu_send_voice_ble();
#endif
#endif
}

void app_service_change(void)
{
    u8 service_data[4]={BATT_PS_H,0,0xff,0};
    if(service_change == 0x55)
    {
        //printf("app_service_change\r\n");
        bls_att_pushIndicateData(GenericAttribute_ServiceChanged_DP_H,service_data,sizeof(service_data));
        analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) & (~OTA_FLG));
        service_change = 0;
    }
}

void mtu_exchange_loop(void)
{
    if(mtuExchange_check_tick && clock_time_exceed(mtuExchange_check_tick, mtuExchange_check_timer ))
    {
        mtuExchange_check_tick = 0;
        app_schedule_ota_send_notify_to_continue_ota();
        if((flag_dle&0x10) == 0)
        {
            //printf("mtu_exchange_loop_mtu\r\n");
            blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, MTU_SIZE_SETTING);
        }
        if((flag_dle&0x10) == 0x10)
        {
            if(app_mtu_size < MTU_SIZE_SETTING)
            {
                //printf("mtu_exchange_loop_mtu\r\n");
                blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, MTU_SIZE_SETTING);
            }
        }
        if((flag_dle&0x01) == 0)
        {
            //printf("mtu_exchange_loop_dle\r\n");
            blc_ll_exchangeDataLength(LL_LENGTH_REQ,(TX_FIFO_SIZE-18));
            flag_dle |= 0x01;
        }
        if((flag_dle&0x80) == 0)
        {
            printf("mtu_exchange_loop_update conn parm\r\n");
            bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);
            flag_dle |= 0x80;
            if((backlight_normal_Flag == false) && (backlight_hidden_Flag == false))
                app_backlight_ic_close_timer = clock_time() | 1; 

#if (RCU_CONFIG_TPV)
            app_rcu_send_voice_ble();
#endif
        }
    }
}

int app_enable_adv_timeout(void)
{
    #if (!CLOSE_CODE_PRINTF)
     printf("app_enable_adv_timeout\r\n");
    #endif
     is_pairing_mode = 0x55;
     app_pairing_led();
     app_adv_indirect();
     enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
#if BLE_AUDIO_ENABLE
     if(google_voice_ctl){
         google_voice_ctl = 0;
     }
     if(ui_mic_enable){
        ui_enable_mic (0);
     }
#endif
     return -1;
}

ble_sts_t app_terminate(u8 reason)
{
    app_slave_terminate = 0x55;
    ble_sts_t status = bls_ll_terminateConnection(reason);
    return status;
}

int app_terminate_timeout(void)
{
     ble_sts_t status = app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN);
     printf("app_terminate_timeout status=%x\r\n",status);
     return -1;
}

u8 ll_reject_to_adv=0;
void ble_ll_reject(u8 e,u8 *p, int n) //*p is terminate reason
{
    #if (!CLOSE_CODE_PRINTF)
     printf("ble_ll_reject\r\n");
     #endif
     if(device_in_connection_state == 0)
     {
         blt_soft_timer_delete(app_terminate_timeout);
         //blt_soft_timer_add(app_enable_adv_timeout, 3000000);
         ll_reject_to_adv = TRUE;
         ble_sts_t status = app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN);
         printf("status=%x\r\n",status);
#if (BLE_AUDIO_ENABLE)
         if(google_voice_ctl & FLAG_GOOGLE_CAPS_RESP)
             google_voice_ctl &= ~FLAG_GOOGLE_CAPS_RESP;
#endif
     }
}

#if (GOOGLE_OTA)
int app_ota_terminate_timeoutcb(void)
{
    printf("app_ota_terminate_timeoutcb\r\n");
    app_ota_terminate(GOOGLE_OTA_USER_TERMINATE);

    return -1;
}
#else
int app_ota_terminate_timeoutcb(void){
    //do nothing
    return 0;
}
#endif

int app_terminate_to_direct_timeoutcb(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("app_terminate_to_direct_timeoutcb\r\n");
    #endif
    app_adv_direct();

    return -1;
}

void ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{

    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    printf("ble_remote_terminate  bond_number=%x,*p=%x\r\n",bond_number,*p);

    //if(app_repairing() == 0)
    {
        if(ota_is_working)
        {
            app_pairing_error();
            blt_soft_timer_add(app_ota_terminate_timeoutcb, 1000000);
            return;
        }
        if(*p == HCI_ERR_CONN_TIMEOUT){
            #if (!CLOSE_CODE_PRINTF)
            printf("HCI_ERR_CONN_TIMEOUT\r\n");
            #endif
            device_timeout_state=0x55;
            is_pairing_mode = 0;
            is_reconn_mode = 0;
            app_slave_terminate = 0;
            if((flag_schedule_ota == 0x55) || (bond_number == 0))
            {
                enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                bls_ll_setAdvEnable(0);
            }
            else
            {
                enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;
                app_adv_direct();

                #if (!CLOSE_CODE_PRINTF)
                printf("--->>> [work_debug] HCI_ERR_CONN_TIMEOUT == 0x08 idle to sleep\r\n");
                #endif
                WaitMs(1000);
                app_schedule_ota_enter_deep();
            }
        }
        else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13
            #if (!CLOSE_CODE_PRINTF)
             printf("remote user term\r\n");
            #endif
             //if(app_rcu_pairing_flag == 0x55)
             //   user_unpair_wait_terminate = true;
             //app_rcu_unpair_wait_terminate();
             device_timeout_state=0x55;
             if(is_pairing_mode)
                 is_pairing_mode = 0;
             if(is_reconn_mode)
                 is_reconn_mode = 0;
             bls_ll_setAdvEnable(0);
             if(app_slave_terminate == 0x55)
             {
                  enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                  app_slave_terminate = 0;
                  #if (!CLOSE_CODE_PRINTF)
                  printf("slave term\r\n");
                  #endif
                  if(repairing_flag == 0x55 || app_rcu_pairing_flag == 0x55)
                     blt_soft_timer_add(app_rcu_repairing, 1000000); // 4s or 1s?
                  else
                  {
                     if(key_clear_pair_flag == 0x01)
                        enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                     else
                        enterdeep_time = 5;//app_schedule_ota_enter_deep();
                  }
             }
             else
             {
                 if(device_in_connection_state == 0)
                 {
                     enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                     if(repairing_flag == 0x55 || app_rcu_pairing_flag == 0x55)
                        blt_soft_timer_add(app_rcu_repairing, 1000000); // 4s or 1s?
                 }
                 else
                 {
                     if(flag_schedule_ota == 0x55)
                     {
                         enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                         bls_ll_setAdvEnable(0);
                     }
                     else
                     {
                         bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
                         if(bond_number)
                         {
                              enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;
                         }
                         else
                             enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                         //blt_soft_timer_add(app_terminate_to_direct_timeoutcb, 600000);
                         #if (!CLOSE_CODE_PRINTF)
                         printf("--->>> [work_debug] HCI_ERR_REMOTE_USER_TERM_CONN == 0x13 idle to sleep\r\n");
                         #endif
                         //WaitMs(1000);
                        if(key_clear_pair_flag == 0x01)
                            enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                        else
                            enterdeep_time = 5;//app_schedule_ota_enter_deep();
                     }
                 }
             }
             
             if(ll_reject_to_adv){
                 repairing_flag = 0x55;
                 blt_soft_timer_add(app_rcu_repairing, 1000000); // 4s or 1s?
                 ll_reject_to_adv = FALSE;
             }
        }
        else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){
              is_pairing_mode = 0;
              is_reconn_mode = 0;
              app_pairing_error();
              enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
              bls_ll_setAdvEnable(0);
              #if (!CLOSE_CODE_PRINTF)
              printf("MIC_FAILURE\r\n");
              #endif
              bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
              if(bond_number)
              {
                  enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;
              }
              else
                  enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
              app_adv_direct();
        }
#if BLE_AUDIO_ENABLE
        google_voice_ctl = 0;
#endif
    }
#if APP_IR_OVER_BLE
    ir_suppress_reinit();
#endif
    device_in_connection_state = 0;
    mtuExchange_check_tick = 0;
    flag_dle = 0;
#if (FIND_ME_ENABLE)
    if(app_buzzer_is_buzy()){
        app_buzzer_stop();
    }
#endif

#if (BLE_REMOTE_PM_ENABLE)
     //user has push terminate pkt to ble TX buffer before deepsleep
    if(sendTerminate_before_enterDeep == 1){
        sendTerminate_before_enterDeep = 2;
    }
#endif

#if (BLE_AUDIO_ENABLE)
    if(ui_mic_enable){
        ui_enable_mic (0);
    }
#endif

    advertise_tick_retime();
}


_attribute_ram_code_ void user_set_rf_power (u8 e, u8 *p, int n)
{
#if (MP_TEST_MODE)
    if (gpio_read(DUT_INPUT_PORT) && test_get_mode() == MODE_TEST)
        rf_set_power_level_index(RF_POWER_P7p37dBm);
    else
#endif
    rf_set_power_level_index (app_rcu_tx_power);
}


#define PEER_MAC_FLASH_ERASE_TIMEOUT 600000

int app_peer_mac_flash_erase_timeoutcb(void)
{
    //printf("app_peer_mac_flash_erase_timeoutcb\r\n");

    if(bls_ll_requestConnBrxEventDisable() > 120)
    {
        //printf("app_peer_mac_flash_erase \r\n");
        bls_ll_disableConnBrxEvent();
        flash_erase_sector(MAC_DATA_SECT_ADDR);
        bls_ll_restoreConnBrxEvent();
        //printf("app_peer_mac_flash_erase succ\r\n");
        write_peer_mac_info(peer_mac);
        return -1;
    }

    return 0;
}

void app_peer_mac_flash_erase(void)
{
    blt_soft_timer_add(app_peer_mac_flash_erase_timeoutcb, PEER_MAC_FLASH_ERASE_TIMEOUT);
}

void app_save_peer_mac_info(u8 *mac)
{
    //printf("peer_mac =\r\n");
    //for(u8 i=0;i<6;i++)
    //    printf(" %x",mac[i]);
    if(memcmp(mac,peer_mac,6))
    {
        #if (!CLOSE_CODE_PRINTF)
            printf("new mac\r\n");
        #endif
            memcpy(peer_mac,mac,6);
#if APP_IR_OVER_BLE
            ir_table_init();
            write_ir_key_event_notify(0);
            ir_flash_erase_flag = 0x55;
#endif
            flag_ccc_data = 0;
#if BLE_AUDIO_ENABLE
            atv_char_ctl_ccc = 0;
            atv_char_rx_ccc = 0;
#endif
            write_ccc_info(&flag_ccc_data);
            if(is_peer_mac_flash_info_full() == 0)
                write_peer_mac_info(peer_mac);
            else
                app_peer_mac_flash_erase();
    }
}

void app_cmp_mac(void)
{
    smp_param_save_t  bondInfo;

    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();
    if(bond_number)
    {
        //printf("app_cmp_mac\r\n");
        bls_smp_param_loadByIndex(0, &bondInfo);
        app_save_peer_mac_info(bondInfo.peer_addr);
        printf("\r\npeer_addr=%x %x %x %x %x %x\r\n",bondInfo.peer_addr[0],bondInfo.peer_addr[1],bondInfo.peer_addr[2],bondInfo.peer_addr[3],bondInfo.peer_addr[4],bondInfo.peer_addr[5]);
        printf("\r\npeer_id_addr=%x %x %x %x %x %x\r\n",bondInfo.peer_id_addr[0],bondInfo.peer_id_addr[1],bondInfo.peer_id_addr[2],bondInfo.peer_id_addr[3],bondInfo.peer_id_addr[4],bondInfo.peer_id_addr[5]);
    }
}

int app_host_event_callback (u32 h, u8 *para, int n)
{

    u8 event = h & 0xFF;

    switch(event)
    {
        case GAP_EVT_SMP_PARING_BEAGIN:
        {
            printf("----- Pairing begin -----\n");
        }
        break;

        case GAP_EVT_SMP_PARING_SUCCESS:
        {
            gap_smp_paringSuccessEvt_t* p = (gap_smp_paringSuccessEvt_t*)para;
            printf("Pairing success:bond flg %s\n", p->bonding ?"true":"false");

            if(p->bonding_result){
                //app_connect_led();
                app_cmp_mac();
                //printf("save smp key succ\n");
            }
            else{
                printf("save smp key failed\n");
            }

        }
        break;

        case GAP_EVT_SMP_PARING_FAIL:
        {
            gap_smp_paringFailEvt_t* p = (gap_smp_paringFailEvt_t*)para;
            printf("----- Pairing failed:rsn:0x%x -----\n", p->reason);
            app_pairing_error();
        }
        break;

        case GAP_EVT_SMP_CONN_ENCRYPTION_DONE:
        {
            gap_smp_connEncDoneEvt_t* p = (gap_smp_connEncDoneEvt_t*)para;

            printf("----- Connection encryption done -----\n");

            blt_soft_timer_delete(app_terminate_timeout);

            if(p->re_connect == SMP_STANDARD_PAIR){  //first paring
                printf("first pairing\r\n");
                led_driver_init();
                app_connect_led();
                app_rcu_pairing_flag = 0x00;
                app_is_set_mtusize = 0;
                mtuExchange_check_timer = 12000000;
                #if APP_IR_OVER_BLE
                if(ir_flash_erase_flag == 0x55)
                {
                    ir_flash_set_flag(0);
                    ir_flash_erase_flag = 0;
                }
                #endif
            }
            else if(p->re_connect == SMP_FAST_CONNECT){  //auto connect
                if(app_mtu_size < MTU_SIZE_SETTING)
                    blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, MTU_SIZE_SETTING);
                //app_connect_led();//取消回连指示
                #if (!CLOSE_CODE_PRINTF)
                printf("--->>> [work_debug] auto REconnect!!!!!!!!");
                #endif
                mtuExchange_check_timer = 250000;//4000000;
                app_is_set_mtusize = 1;
                printf("auto connect \r\n");
                //ltr_sensor_init();
            }

            device_in_connection_state = 1;
            is_pairing_mode = 0;
            is_reconn_mode = 0;
//            wakeup_key_send_flag = 0;
            app_woble_clear_para();
            mtuExchange_check_tick = clock_time() | 1;
            #if BLE_AUDIO_ENABLE
            notify_get_rsp_tick = clock_time()|1;
            #endif
            wakeup_key_keycount = 0;
            write_wakeup_keyindex_info(wakeup_key_keycount);

            battery_refresh();
            blt_soft_timer_add(app_cachekey_send_timer,30000);
        }
        break;

        case GAP_EVT_ATT_EXCHANGE_MTU:
        {
           // flag_mtu = 1;
            gap_gatt_mtuSizeExchangeEvt_t *pEvt = (gap_gatt_mtuSizeExchangeEvt_t *)para;
            printf("MTU Peer MTU(%d)/Effect ATT MTU(%d).\n", pEvt->peer_MTU, pEvt->effective_MTU);
            app_mtu_size = U16_LO(pEvt->effective_MTU);
            flag_dle |= 0x10;
            /*
            if(!flag_dle){
                blc_ll_exchangeDataLength(LL_LENGTH_REQ,pEvt->effective_MTU);
            }
            */
        }
        break;


        default:
        break;
    }

    return 0;
}

void task_connect (u8 e, u8 *p, int n)
{
    printf("connect\r\n");
    app_mtu_size = 23;
    bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(1000);
    enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;

    latest_user_event_tick = clock_time();
    //blt_soft_timer_add(app_terminate_timeout, 10000000);//2000000
//    bls_ota_set_random(p+12);
#if FIND_ME_ENABLE
    app_fms_delete_adv_task();
#endif
}

void task_conn_update_req (u8 e, u8 *p, int n)
{
    //printf("task_conn_update_req\r\n");
}

void task_conn_update_done (u8 e, u8 *p, int n)
{
    //printf("task_conn_update_done\n");
}

int app_conn_param_update_response(u8 id, u16  result)
{
    if(result == CONN_PARAM_UPDATE_ACCEPT){
       // printf("CONN_PARAM_UPDATE_ACCEPT\n");
    }
    else if(result == CONN_PARAM_UPDATE_REJECT){
        //printf("CONN_PARAM_UPDATE_REJECT\n");
    }

    return 0;
}

#if (AUDIO_TRANS_USE_2M_PHY_ENABLE)
int app_host_event_callback (u32 h, u8 *para, int n)
{
    u8 event = h & 0xFF;
    if(event==GAP_EVT_ATT_EXCHANGE_MTU)
    {
        blc_ll_setPhy(BLS_CONN_HANDLE, PHY_TRX_PREFER, PHY_PREFER_2M,      PHY_PREFER_2M,    CODED_PHY_PREFER_NONE);
    }
    return 0;
}
void app_phy_update_complete_event(u8 e,u8 *p, int n)
{
}
#endif

extern u32 drive_pins[];
void blt_pm_proc(void)
{
    /* 221116 by ken */
    app_ui_check_delay_en_pm();

#if(BLE_REMOTE_PM_ENABLE)

    u8 time=0;

#if (UART_PRINT_ENABLE)
    if(printf_uart_flag)
    {
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
        return;
    }
#endif

#if (STUCK_KEY_PROCESS_ENABLE)
    extern u32 stuckkey_keypresstimer;
    if(key_not_released && stuckkey_keypresstimer && clock_time_exceed(stuckkey_keypresstimer, STUCK_KEY_ENTERDEEP_TIME*1000000))
    {
         printf("stuck key\r\n");

        app_backlight_ic_close_timer = 0;
        backlight_task = 0;
        aw21036_all_off();
        ALS_Disable();
        aw21036_Disable();     

         if(1 == app_rcu_CouchMode){
             app_key_release_action();
         }else{
             stuckkey_keypresstimer = 0;
             u8 len = app_custom_get_drive_len();
             {
                for(int i=0; i<len;i++)
                {
                     extern u8 stuckKeyPress[];
                     if(stuckKeyPress[i])
                     {
                        #if (!CLOSE_CODE_PRINTF)
                         printf("stuck key i=%x\r\n",i);
                         #endif
                         cpu_set_gpio_wakeup(drive_pins[i], KB_LINE_HIGH_VALID, 1);
                     }
                     else
                     {
                        cpu_set_gpio_wakeup(drive_pins[i], KB_LINE_HIGH_VALID, 0);
                     }
                }
                app_release_hid_key();
                #if (!CLOSE_CODE_PRINTF)
                printf("deep\r\n");
                #endif
                cpu_sleep_wakeup(DEEPSLEEP_MODE,PM_WAKEUP_PAD,0);
                #if (!CLOSE_CODE_PRINTF)
                printf("into deep fail\r\n");
                #endif
             }
         }
    }
#endif

#if (TL_SZ119_TVSSU)   
    if(tv_ssu_busy() == 1) 
    { 
        bls_pm_setManualLatency(0); 
    } 
    else if(tv_ssu_busy() == 2) 
    { 
        bls_pm_setSuspendMask (SUSPEND_DISABLE);  
        return; 
    }
#endif

    if(device_led_busy())
    {
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
        return;
    }

#if FIND_ME_ENABLE
    if(app_buzzer_is_buzy())
    {
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
        return;
    }
#endif

#if APP_IR_OVER_BLE
    if(ir_flash_busy())
    {
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
        return;
    }
#endif

    if((ota_is_working) && (device_in_connection_state))
    {
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
    }
#if BLE_AUDIO_ENABLE
    if((ui_mic_enable || google_voice_ctl) && (device_in_connection_state))
    {
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
    }
#endif

    #if(REMOTE_IR_ENABLE)
        else if( ir_send_ctrl.is_sending){
            //printf(" ir_send_ctrl.is_sending\r\n");
            bls_pm_setSuspendMask(SUSPEND_DISABLE);
        }
    #endif

    #if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE)
        else if( blc_phy_isPhyTestEnable() )
        {
            bls_pm_setSuspendMask(SUSPEND_DISABLE);  //phy test can not enter suspend
        }
    #endif

#if (MP_TEST_MODE)
    else if (test_get_mode() == MODE_TEST)
    {
        if (test_get_state() == TEST_STAT_LED)
            cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_PAD, 0);
        else if (test_get_state() == TEST_STAT_CURRENT)
            cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
        else bls_pm_setSuspendMask(SUSPEND_DISABLE);
    }
#endif
    else
    {

        #if (PM_DEEPSLEEP_RETENTION_ENABLE)
            bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
        #else
            bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
        #endif

        int user_task_flg = ota_is_working || scan_pin_need || key_not_released || backlight_task;

        if(user_task_flg){
              // printf("ota_is_working scan_pin_need=%x google_voice_ctl=%x key_not_released=%x,device_led_busy()=%x\r\n",ui_mic_enable,scan_pin_need,key_not_released,device_led_busy());
            bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

            #if (LONG_PRESS_KEY_POWER_OPTIMIZE)
                extern int key_matrix_same_as_last_cnt;
                if(!ota_is_working && key_matrix_same_as_last_cnt > 5){  //key matrix stable can optize
                    bls_pm_setManualLatency(0);//2
                }
                else{
                    bls_pm_setManualLatency(0);  //latency off: 0
                }
            #else
                bls_pm_setManualLatency(0);
            #endif
        }

        #if 0
        /* The button has been released and g_delay_en_pm is not 0 */
        if((0 == key_not_released) && g_delay_en_pm){
            /* 221116 by ken. A short latency is required during the validity period of the delay PM.
             * In order to quickly receive the mic open command of GOOGLE voice */
            bls_pm_setManualLatency(5);
        }
        #endif

    #if 1 //deepsleep
        if(sendTerminate_before_enterDeep == 1){ //sending Terminate and wait for ack before enter deepsleep
            if(user_task_flg){  //detect key Press again,  can not enter deep now
                sendTerminate_before_enterDeep = 0;
                bls_ll_setAdvEnable(1);   //enable adv again
            }
        }
        else if(sendTerminate_before_enterDeep == 2){  //Terminate OK
            analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) | CONN_DEEP_FLG);
            cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
        }

        if(  !blc_ll_isControllerEventPending() ){  //no controller event pending
            //adv 30s interval downgraded
            if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && !sendTerminate_before_enterDeep && \
                clock_time_exceed(advertise_begin_tick , ADV_INTERNAL_SWITCH_TIME * 1000000))
            {
                //interval downgraded
                if(advertise_downgrade_flag == 0)
                {
                    advertise_downgrade_flag = 0xaa;
                    if(app_adv_interval_downgradeCb)
                        app_adv_interval_downgradeCb();
                }
            }
            //adv 1min, deepsleep
            if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && !sendTerminate_before_enterDeep && \
                clock_time_exceed(advertise_begin_tick , ADV_IDLE_ENTER_DEEP_TIME * 1000000))
            {
                enterdeep_time_count++;
                printf("enterdeep_time_count=%x\r\n",enterdeep_time_count);

                bls_ll_setAdvEnable(0);
                delay3s_to_sleep_tick = clock_time()|1;
                device_led_off(APP_LED_BLUE);
                advertise_begin_tick = clock_time();
            }
            if(blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && delay3s_to_sleep_tick && clock_time_exceed(delay3s_to_sleep_tick,3000*1000))
            {
                #if (!CLOSE_CODE_PRINTF)
                 printf("adv timeout\r\n");
                 #endif
                 app_schedule_ota_enter_deep();
            }
            if((is_pairing_mode == 0) && (is_reconn_mode == 0) && (device_in_connection_state == 0) &&  !user_task_flg)//  && (0 == s_p_app_fms_flag_adv[0]))
            {
                 if(enterdeep_time == ADV_RECONN_ENTER_DEEP_COUNT)
                        time = ADV_IDLE_ENTER_DEEP_TIME;
                 else
                        time = ADV_IDLE_ENTER_DEEP_COUNT;
                 if(clock_time_exceed(advertise_begin_tick ,  time* 1000000))
                 {
                    #if (!CLOSE_CODE_PRINTF)
                     printf("idle to sleep\r\n");
                     #endif
                     app_schedule_ota_enter_deep();
                 }
            }

            if ( (blc_ll_getCurrentState() != BLS_LINK_STATE_CONN) && (blc_ll_getCurrentState() != BLS_LINK_STATE_ADV) &&  !user_task_flg )
            {
                if(enterdeep_time == ADV_RECONN_ENTER_DEEP_COUNT)
                        time = ADV_IDLE_ENTER_DEEP_TIME;
                else
                        time = ADV_IDLE_ENTER_DEEP_COUNT;
#if (TEST_SW_LED)                        
                if(led_test_sw_flag)
                    time = 10;
#endif
                if(clock_time_exceed(advertise_begin_tick ,  time* 1000000))
                {
                    app_schedule_ota_enter_deep();
                }
             }
            // connect time out to deep
            if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && !user_task_flg && \
                clock_time_exceed(latest_user_event_tick, CONN_IDLE_ENTER_DEEP_TIME * 1000000) )
            {
                app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN);
                bls_ll_setAdvEnable(0);
                sendTerminate_before_enterDeep = 0;
                #if (!CLOSE_CODE_PRINTF)
                printf("--->>> [work_debug] no action to terminate connection\n");
                #endif
            }
        }
    #endif
    }

#endif  //END of  BLE_REMOTE_PM_ENABLE
}



void ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
    if( ((blc_ll_getCurrentState() == BLS_LINK_STATE_ADV)  || (blc_ll_getCurrentState() == BLS_LINK_STATE_CONN)) && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_16M_SYS_TIMER_CLK_1MS){  //suspend time > 30ms.add gpio wakeup
        bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio pad wakeup suspend/deepsleep
    }
}

#if (!CLOSE_CODE_RAM)
_attribute_ram_code_
#endif
void user_init_battery_power_check(void)
{
    /*****************************************************************************************
     Note: battery check must do before any flash write/erase operation, cause flash write/erase
           under a low or unstable power supply will lead to error flash operation

           Some module initialization may involve flash write/erase, include: OTA initialization,
                SMP initialization, ..
                So these initialization must be done after  battery check
    *****************************************************************************************/
    #if (BATT_CHECK_ENABLE)  //battery check must do before OTA relative operation
        #if(VBAT_LEAKAGE_PROTECT_EN)
        do{
            u8 analog_deep = analog_read(USED_DEEP_ANA_REG);
            #if 0
            u16 vbat_deep_thres = VBAT_DEEP_THRES_MV;
            u16 vbat_suspend_thres = VBAT_SUSPEND_THRES_MV;
            if(analog_deep & LOW_BATT_FLG){
                if(analog_deep & LOW_BATT_SUSPEND_FLG){//<1.8v
                    vbat_deep_thres += 200;
                    vbat_suspend_thres += 100;
                }
                else{//1.8--2.0v
                    vbat_deep_thres += 200;
                }
            }
            app_battery_power_check(vbat_deep_thres,vbat_suspend_thres);
            #endif
            app_battery_power_check(VBAT_DEEP_THRES_MV,VBAT_SUSPEND_THRES_MV);
            #if (MODULE_WATCHDOG_ENABLE)
            wd_clear(); //clear watch dog
            #endif
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

void app_delay_set(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("app_delay_set\r\n");
    #endif
    #if(BLE_AUDIO_ENABLE)
    google_get_rsp_delay();
    #endif
    if((flag_dle & 0x80) == 0)
    {
        mtuExchange_check_tick = clock_time() | 1;
        mtuExchange_check_timer = 500000;
    }
}

void app_set_mtusize(void)
{
    if(app_is_set_mtusize == 0)
    {
        app_is_set_mtusize = 1;
        printf("app_mtu_size=%x\r\n",app_mtu_size);
        if(app_mtu_size < MTU_SIZE_SETTING)
           blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, MTU_SIZE_SETTING);
    }
}

void app_before_write_or_erase_register(app_before_write_or_erase_cb_t p_event_handle)
{
    app_before_write_or_erase_cb = p_event_handle;
}

void app_before_write_or_erase(void)
{
    if(flash_type == FLASH_ETOX_ZB)
    {
        if(app_get_bat_vol() < 2100)
        {
            u8 analog_used_deep_reg = analog_read(USED_DEEP_ANA_REG);
            analog_write(USED_DEEP_ANA_REG,  analog_used_deep_reg | (LOW_BATT_FLG & (~LOW_BATT_SUSPEND_FLG)));  //mark
            cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
        }
    }
}

u8 is_mic_enable(void)
{
    return ui_mic_enable;
}

#if (TL_SZ119_TVSSU) 
unsigned char ssu_isuserIRCodeTxing(void)
{
    if (ir_send_ctrl.is_sending)
    {
        return 1;
    }else
    {
        return 0;
    } 
}
#endif

void user_init_normal(void)
{
    //random number generator must be initiated here( in the beginning of user_init_nromal)
    //when deepSleep retention wakeUp, no need initialize again
    random_generator_init();  //this is must

////////////////// BLE stack initialization ////////////////////////////////////

    u8  mac_random_static[6];
    //for 512K Flash, flash_sector_mac_address equals to 0x76000
    //for 1M   Flash, flash_sector_mac_address equals to 0xFF000
    blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] mac_public = [%02X %02X %02X %02X %02X %02X]\r\n",mac_public[5],mac_public[4], mac_public[3], mac_public[2], mac_public[1], mac_public[0]);
    #endif
    #if(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_PUBLIC)
        app_own_address_type = OWN_ADDRESS_PUBLIC;
    #elif(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_RANDOM_STATIC)
        app_own_address_type = OWN_ADDRESS_RANDOM;
        blc_ll_setRandomAddr(mac_random_static);
    #endif
    blc_ll_resolvListInit();

    ////// Controller Initialization  //////////
    blc_ll_initBasicMCU();                      //mandatory
    blc_ll_initStandby_module(mac_public);                //mandatory
    blc_ll_initAdvertising_module(mac_public);     //adv module:          mandatory for BLE slave,
    blc_ll_initConnection_module();                //connection module  mandatory for BLE slave/master
    blc_ll_initSlaveRole_module();                //slave module:      mandatory for BLE slave,
    blc_ll_initPowerManagement_module();        //pm module:           optional
    blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);

    #if(AUDIO_TRANS_USE_2M_PHY_ENABLE)
        blc_ll_init2MPhyCodedPhy_feature();            //enable 2M/coded PHY
    #endif

    ////// Host Initialization  //////////
    blc_gap_peripheral_init();    //gap initialization
    extern void my_att_init ();
    my_att_init (); //gatt initialization
    blc_l2cap_register_handler (blc_l2cap_packet_receive);      //l2cap initialization
#if !CLOSE_CODE_RAM
    extern void   blc_att_setPrepareWriteBuffer(u8 *p, u16 len);
    blc_att_setPrepareWriteBuffer (prepareWrite_b, sizeof(prepareWrite_b));
#endif
    blc_att_setRxMtuSize(MTU_SIZE_SETTING);

    //Smp Initialization may involve flash write/erase(when one sector stores too much information,
    //   is about to exceed the sector threshold, this sector must be erased, and all useful information
    //   should re_stored) , so it must be done after battery check
    #if (BLE_REMOTE_SECURITY_ENABLE)
        blc_smp_peripheral_init();

        //Hid device on android7.0/7.1 or later version
        // New paring: send security_request immediately after connection complete
        // reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
        //blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection )
        //blc_smp_configSecurityRequestSending(SecReq_NOT_SEND, SecReq_NOT_SEND, 1000);
        blc_smp_configSecurityRequestSending(SecReq_PEND_SEND, SecReq_PEND_SEND, 5000);
    #else
        blc_smp_setSecurityLevel(No_Security);
    #endif

    #if (PCB_VER == PCB_SRC_3215)
        blc_ll_initChannelSelectionAlgorithm_2_feature();
    #endif

///////////////////// USER application initialization ///////////////////
    app_set_advdata_rspdata();
    detpowerkey = 0;

    if(APP_WAKEUP_SRC_POWER_ON == app_ui_get_wakeup_src()){
        //printf("power on for power down\r\n");
        key_handle_deep_pressed();
#if 0
        if(kb_detbackkey())
        {
            printf("press and hold back key\r\n");
            for(u8 i=0;i<8;i++)
            {
                if(i<4)
                    flash_erase_sector(IR_DATA_SECT_0_ADDR+i*0x1000);
                else
                    flash_erase_sector(IR_DATA_SECT_0_ADDR-0x4000+(i-4)*0x1000);
            }
            u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
            if(bond_number)
                 bls_smp_param_deleteByIndex(0);
            bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
            flash_erase_sector(MAC_DATA_SECT_ADDR);
            flash_erase_sector(IR_KEY_EVENT_NOTIFY_SECT_ADDR);
            flash_erase_sector(CCC_DATA_AREA);
//            u8 data=0;
//            flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR, 1, &data);
//            if(data != 0xff)
//                flash_erase_sector(APP_DATA_UPDATE_OVER_OTA_ADDR);

            /* Preserve NEC IR data during Factory rest in v2.10 */
//            flash_read_page(APP_NEC_IR_CODE_TABLE, 1, &data);
//            if(data != 0xff)
//                flash_erase_sector(APP_NEC_IR_CODE_TABLE);
            analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) & (~POWER_ON_FLG));
            u16 my_KeyReleaseCnt = 0;
            while(kb_detbackkey() && my_KeyReleaseCnt<400){
                my_KeyReleaseCnt++;
                WaitMs(10);
            }
            start_reboot();
        }
        if(kb_detpowerkey())
        {
           detpowerkey = 1;
           printf("press and hold power key\r\n");
        }
#endif
    }
    u8 data = analog_read(DEEP_ANA_REG2);
    if(data == 0x0f)
        analog_write(DEEP_ANA_REG2, 0);
    app_schedule_ota_wakeup_host();

    is_pairing_mode = 0;
    device_in_connection_state = 0;
    is_reconn_mode = 0;

    ////////////////// config adv packet /////////////////////

    blc_smp_param_setBondingDeviceMaxNumber(1);
#if (MP_TEST_MODE)
    if (test_get_mode() != MODE_TEST)
#endif
    {
        u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
        if(bond_number)   //at least 1 bonding device exist
        {
            u8 my_sch_ota_index = analog_read(DEEP_ANA_REG2);
            if((APP_WAKEUP_SRC_DEEP_TIMER == app_ui_get_wakeup_src()) && (!my_sch_ota_index)){
                /* This wake-up is for the periodic wake-up of the FMS
                 * When the wake-up source is Timer, currently there are only scheduled OTA and periodic wake-up
                 * If it is scheduled OTA, my_sch_ota_index is not 0.*/
#if FIND_ME_ENABLE
                app_fms_try_connect();
#endif

            }else{
                if(APP_WAKEUP_SRC_POWER_ON == app_ui_get_wakeup_src())
                    user_direct_adv(MY_DIRECT_ADV_TMIE);
#if 0
                smp_param_save_t  bondInfo;
                bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )
                ll_resolvingList_reset();
                ll_resolvingList_add(bondInfo.peer_id_adrType,bondInfo.peer_id_addr,bondInfo.peer_irk,bondInfo.local_irk);
                ll_whiteList_reset();
                app_set_adv_interval_downgrade_direct();
                //printf("\r\npeer_addr=%x %x %x %x %x %x\r\n",bondInfo.peer_addr[0],bondInfo.peer_addr[1],bondInfo.peer_addr[2],bondInfo.peer_addr[3],bondInfo.peer_addr[4],bondInfo.peer_addr[5]);
                //printf("\r\npeer_id_addr=%x %x %x %x %x %x\r\n",bondInfo.peer_id_addr[0],bondInfo.peer_id_addr[1],bondInfo.peer_id_addr[2],bondInfo.peer_id_addr[3],bondInfo.peer_id_addr[4],bondInfo.peer_id_addr[5]);

                enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;
                if((analog_read(USED_DEEP_ANA_REG) & OTA_FLG) == OTA_FLG)
                {
                    printf("ana read = ota succ\r\n");
                    service_change = 0x55;
                }
                u8 status=0,rpa_flag=0;
                if (memcmp(bondInfo.peer_id_addr, bondInfo.peer_addr, 6) == 0)
                {
                   rpa_flag = 0;
                }
                else
                {
                   if(en_slave_rpa == 0)
                       rpa_flag = 0;
                   else
                       rpa_flag = 1;
                }
                if(rpa_flag == 0)
                {
                    printf("host not support rpa\r\n");
                    ll_whiteList_add(bondInfo.peer_addr_type,bondInfo.peer_addr);
                    status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                                                ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                                                bondInfo.peer_addr_type,  bondInfo.peer_addr,
                                                MY_APP_ADV_CHANNEL,
                                                ADV_FP_NONE);
                }
                else
                {
                    printf("host support rpa\r\n");
                    app_own_address_type = OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC;
                    //ll_resolvingList_setResolvablePrivateAddrTimer(60);
                    status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                            ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                            bondInfo.peer_id_adrType,  bondInfo.peer_id_addr,
                                                               MY_APP_ADV_CHANNEL,
                                                               ADV_FP_NONE);
                }

                if(status != BLE_SUCCESS) {      while(1); }  //debug: adv setting err
                //it is recommended that direct adv only last for several seconds, then switch to indirect adv
                bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE, 1);
                enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                //bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &direct_adv);
                is_reconn_mode = 0x55;
                bls_ll_setAdvEnable(1);  //adv enable
                printf("direct adv\r\n");
#endif
            }
        }
        else   //set indirect adv
        {
            app_set_adv_interval_downgrade_indirect();
            enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
            u8 status = bls_ll_setAdvParam(  MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                                             ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
                                             0,  NULL,
                                             MY_APP_ADV_CHANNEL,
                                             ADV_FP_NONE);
            if(status != BLE_SUCCESS) {      while(1); }  //debug: adv setting err
            #if (!CLOSE_CODE_PRINTF)
            printf("inderict adv\r\n");
            #endif
            // TODO TEST
            //app_adv_indirect();

        }
    }


#if (MP_TEST_MODE)
    else
    {
        printf("test mode\r\n");

        app_set_adv_interval_downgrade_indirect();
        enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
        u8 status = bls_ll_setAdvParam(  MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                                         ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
                                         0,  NULL,
                                         MY_APP_ADV_CHANNEL,
                                         ADV_FP_NONE);
        if(status != BLE_SUCCESS) {      while(1); }  //debug: adv setting err
        printf("inderict adv\r\n");
    }
#endif

    //set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend
    user_set_rf_power(0, 0, 0);
    bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power);

    blc_gap_registerHostEventHandler( app_host_event_callback );
    blc_gap_setEventMask( GAP_EVT_MASK_SMP_PARING_BEAGIN             |  \
                          GAP_EVT_MASK_SMP_PARING_SUCCESS           |  \
                          GAP_EVT_MASK_SMP_PARING_FAIL                |  \
                          GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE     |  \
                          GAP_EVT_MASK_ATT_EXCHANGE_MTU
                          );

    //ble event call back
    bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
    bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);

    bls_app_registerEventCallback (BLT_EV_FLAG_LL_REJECT_IND, &ble_ll_reject);

    bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_REQ, &task_conn_update_req);
    bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_UPDATE, &task_conn_update_done);

    bls_app_registerEventCallback (BLT_EV_FLAG_DATA_LENGTH_EXCHANGE, &task_datalengthexchange);
    blc_l2cap_registerConnUpdateRspCb(app_conn_param_update_response);

    #if(AUDIO_TRANS_USE_2M_PHY_ENABLE)
        blc_gap_setEventMask(GAP_EVT_MASK_ATT_EXCHANGE_MTU);
        blc_gap_registerHostEventHandler( app_host_event_callback );
        bls_app_registerEventCallback (BLT_EV_FLAG_PHY_UPDATE, &app_phy_update_complete_event);
    #endif

    ///////////////////// Power Management initialization///////////////////
#if(BLE_REMOTE_PM_ENABLE)
    blc_ll_initPowerManagement_module();

    #if (PM_DEEPSLEEP_RETENTION_ENABLE)
        bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
        blc_pm_setDeepsleepRetentionThreshold(50, 30);
        #if (__PROJECT_8278_BLE_REMOTE__)
            blc_pm_setDeepsleepRetentionEarlyWakeupTiming(480);
        #else
            blc_pm_setDeepsleepRetentionEarlyWakeupTiming(400);
        #endif
    #else
        bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
    #endif

    bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
    bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif

#if    (TL_AUDIO_MODE & TL_AUDIO_MASK_SBC_MODE | TL_AUDIO_MASK_MSBC_MODE)
    blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);
#else
#endif

#if (BLE_REMOTE_OTA_ENABLE)
    ////////////////// OTA relative ////////////////////////
    bls_ota_clearNewFwDataArea(); //must
#if (GOOGLE_OTA)
    bls_ota_init_handle(OTA_CMD_OUT_DP_H);
#endif
#if (TELINK_OTA)
    blc_ota_registerOtaStartCmdCb(app_enter_ota_mode_1);
    blc_ota_registerOtaResultIndicationCb(app_debug_ota_result_1);
#endif

#endif
#if APP_RCU_FLASH_LOCK_ENABLE
    if(APP_WAKEUP_SRC_DEEP_PAD != app_ui_get_wakeup_src()){
        app_rcu_flash_lock_init();
    }
#endif

    app_ui_init_normal();
    advertise_tick_retime();

#if (BLE_AUDIO_ENABLE)
    //user data init
    init_ccc_value();
    //printf("flag_ccc_data:0x%x\n",flag_ccc_data);
#endif
    is_flash_info_full();
    init_peer_mac();
#if APP_IR_OVER_BLE
    app_ir_programming_end_register(app_delay_set);
    ir_param_init();
    u8 ccc = init_ir_key_event_notify();
    if(ccc != 0xff)
        ir_init_key_event_notify(ccc);
#endif
    init_wakeup_keyindex();

#if TPV_RCU_PROJECT_SEL
    read_flash_rcu_fw();
#endif
#if (FUNCTION_CALIBRATION_BL)
    read_flash_rcu_test_bl();
#endif
    //if(APP_WAKEUP_SRC_POWER_ON == app_ui_get_wakeup_src())
    {
        extern void init_device_info (void);
        init_device_info();
    }
    app_before_write_or_erase_register(app_before_write_or_erase);

    // TODO test WOBLE code
//    app_woboe_set_para(0,wakeup_key_keycount);
#if FIND_ME_ENABLE
    app_fms_init();
#endif

#if (TL_SZ119_TVSSU)
    SIMPLE_SETUP_Handle(SIMPLESETUP_CMD_OUT_DP_H);
    #if (PCB_VER == PCB_SRC_3215)
        if (strcmp((char*)rcu_project_name,"EU01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
        else if (strcmp((char*)rcu_project_name,"RU01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
        else if (strcmp((char*)rcu_project_name,"LM01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_CC);
    #endif
    #if (PCB_VER == PCB_SRC_3009)
        if (strcmp((char*)rcu_project_name,"EU01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
        else if (strcmp((char*)rcu_project_name,"AP01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_AD);
        else if (strcmp((char*)rcu_project_name,"LM01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_CC);
    #endif
    SSU_flash_address_register (APP_TL_SZ119_TVSSU_ADDR); 
    register_IR_TX_callback(ssu_isuserIRCodeTxing);
    register_IR_PWM_frequency(CLOCK_SYS_CLOCK_HZ / 1000000);
    ir_hw_initialed = 1;
    ir_driver_init_philips();
#endif 
}


_attribute_ram_code_ void user_init_deepRetn(void)
{
#if (PM_DEEPSLEEP_RETENTION_ENABLE)

    #if (TL_SZ119_TVSSU)
    #if (PCB_VER == PCB_SRC_3215)
        if (strcmp((char*)rcu_project_name,"EU01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
        else if (strcmp((char*)rcu_project_name,"RU01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
        else if (strcmp((char*)rcu_project_name,"LM01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_CC);
    #endif
    #if (PCB_VER == PCB_SRC_3009)
        if (strcmp((char*)rcu_project_name,"EU01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
        else if (strcmp((char*)rcu_project_name,"AP01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_AD);
        else if (strcmp((char*)rcu_project_name,"LM01") == 0)
            simple_setup_key_register (SSU_KEY_NORMAL_CC);
    #endif
    ir_hw_initialed = 1;
    ir_driver_init_philips();
    #endif 
    blc_ll_initBasicMCU();   //mandatory
#if (MP_TEST_MODE)
    if (gpio_read(DUT_INPUT_PORT) && test_get_mode() == MODE_TEST)
        rf_set_power_level_index(RF_POWER_P7p37dBm);
    else
#endif
    rf_set_power_level_index (app_rcu_tx_power);

    blc_ll_recoverDeepRetention();

    irq_enable();

    app_ui_init_deepRetn();
#endif
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////

#if 0
_attribute_data_retention_ u32 test_toggle_tick=0;
_attribute_data_retention_ u8 test_toggle_level=0;

void test_toggle(void)
{
    if(clock_time_exceed(test_toggle_tick, 30000))
    {
        test_toggle_tick = clock_time() | 1;
        if(test_toggle_level)
        {
            device_led_on(0);
            test_toggle_level = 0;
        }
        else
        {
            device_led_off(0);
            test_toggle_level = 1;
        }

    }
}
#endif

void app_send_wakeup_resume(void)
{
   u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
   if(bond_number)
       app_adv_direct();
   else
       app_adv_indirect();
}

void app_start_send_wakeup(rf_packet_adv_ind_module_t advdata)
{
   // u8 tbl_scanRsp[30];
    u8 tbl_advData[40];


    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    smp_param_save_t  bondInfo;
    if(bond_number)   //at least 1 bonding device exist
    {
        bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )
        if (memcmp(bondInfo.peer_id_addr, bondInfo.peer_addr, 6) == 0)
        {
            ll_whiteList_reset();
            ll_whiteList_add(bondInfo.peer_addr_type,bondInfo.peer_addr);
        }
    }
    #if (!CLOSE_CODE_PRINTF)
    printf("app_start_send_wakeup\r\n");
    #endif
    bls_ll_setAdvEnable(0);
    memcpy(tbl_advData,advdata.data,advdata.rf_len);
    for(u8 i=0;i<25;i++)
        printf(" %02X ",tbl_advData[i]);
    bls_ll_setAdvData( (u8 *)tbl_advData, advdata.rf_len);

    bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS,
        ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
        0,    NULL,
        MY_APP_ADV_CHANNEL,
        ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);
    bls_ll_setAdvEnable(1);

}

void app_send_wakeup_loop(void)
{
    /* Return 3 to indicate that ADV needs to be resume */
    if(3 == app_woble_send_task()){
        bls_ll_setAdvEnable(0);
        advertise_begin_tick = clock_time() | 1;
        #if (!CLOSE_CODE_PRINTF)
        printf("app_send_wakeup_loop second part\r\n");
        #endif
//        wakeup_key_send_flag = 0;
        app_send_wakeup_resume();
        device_led_off(APP_LED_BLUE);
    }
}


void main_loop (void)
{
    //test_toggle();
#if (BLT_TEST_SOFT_TIMER_ENABLE)
    blt_soft_timer_process(MAINLOOP_ENTRY);
#endif
    ////////////////////////////////////// BLE entry /////////////////////////////////
    blt_sdk_main_loop();
    mtu_exchange_loop();
#if APP_IR_OVER_BLE
    ir_flash_erase_loop();
    ir_flash_save_loop();
    ir_nec_ir_table_erase_loop();
#endif
#if APP_DATA_UPDATE_OVER_BLE
    app_custom_data_update_sector_erase_loop();
#endif
    //app_send_wakeup_loop();
    ////////////////////////////////////// UI entry /////////////////////////////////
    #if (BLE_AUDIO_ENABLE)
        app_audio_task();
    #endif
#if (!TEST_BLBL)
    app_backlight_task();
    app_rcu_timer_task();
#endif
    app_ota_proc();

#if FIND_ME_ENABLE
//    app_custom_find_me_buzzer_on();
    app_buzzer_task();
#endif

    #if (BATT_CHECK_ENABLE)
        // 如果红外有发码,延迟一个周期后再检测电量
        if (ir_send_ctrl.is_sending) {
            lowBattDet_tick = clock_time();
        }
        if(battery_get_detect_enable() && clock_time_exceed(lowBattDet_tick, 300000) ){
            lowBattDet_tick = clock_time();
            #if(VBAT_LEAKAGE_PROTECT_EN)

            u8 analog_deep = analog_read(USED_DEEP_ANA_REG);
            u16 vbat_deep_thres = VBAT_DEEP_THRES_MV;
            u16 vbat_suspend_thres = VBAT_SUSPEND_THRES_MV;
            if(analog_deep & LOW_BATT_FLG){
                if(analog_deep & LOW_BATT_SUSPEND_FLG){//<1.8v
                    vbat_deep_thres += 200;
                    vbat_suspend_thres += 100;
                }
                else{//1.8--2.0v
                    vbat_deep_thres += 200;
                }
            }
            if(!backlight_task)
            app_battery_power_check(vbat_deep_thres,vbat_suspend_thres);

            #else
                app_battery_power_check(VBAT_DEEP_THRES_MV);  //2000 mV low battery
            #endif
        }
    #endif
    proc_keyboard (0,0, 0);

#if (BLT_APP_LED_ENABLE)
    device_led_process();
#endif
#if (TL_SZ119_TVSSU)
    //if (batt_status ==0)
    if ((batt_status == BATT_LOW) || (batt_status == BATT_POOR))
    {
        low_battery =1;
    }else 
    {
        low_battery =0;
    }
    simple_setup_search_proc (low_battery, device_in_connection_state); 
#endif

    blt_pm_proc();
}
