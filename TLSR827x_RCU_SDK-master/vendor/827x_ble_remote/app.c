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
#include "app_ota.h"
#include "app_ir.h"
#include "app_config.h"
#include "app_flash_write.h"
#include "../../application/audio/gl_audio.h"

#include "app_att.h"
#include "gap.h"

#if (MP_TEST_MODE)
#include "app_test.h"
#endif

#define     BLE_DEVICE_ADDRESS_TYPE             BLE_DEVICE_ADDRESS_PUBLIC

#if (MCU_CORE_TYPE == MCU_CORE_8278)
#define     MY_RF_POWER_INDEX            RF_POWER_P3p50dBm
#else
#define     MY_RF_POWER_INDEX            RF_POWER_P3p01dBm
#endif

_attribute_data_retention_    own_addr_type_t   app_own_address_type = OWN_ADDRESS_PUBLIC;



#if 0
    MYFIFO_INIT(blt_rxfifo, RX_FIFO_SIZE, RX_FIFO_NUM);
#else
_attribute_data_retention_  u8  blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_  my_fifo_t   blt_rxfifo = {
                                                RX_FIFO_SIZE,
                                                RX_FIFO_NUM,
                                                0,
                                                0,
                                                blt_rxfifo_b,};
#endif


#if 0
    MYFIFO_INIT(blt_txfifo, TX_FIFO_SIZE, TX_FIFO_NUM);
#else
_attribute_data_retention_  u8  blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
_attribute_data_retention_  my_fifo_t   blt_txfifo = {
                                                    TX_FIFO_SIZE,
                                                    TX_FIFO_NUM,
                                                    0,
                                                    0,
                                                    blt_txfifo_b,};
#endif

u8  mac_public[6];


//extern u8 is_pairing_mode;
//extern u8 is_reconn_mode;
//extern ble_sts_t    blc_att_requestMtuSizeExchange (u16 connHandle, u16 mtu_size);
extern void is_flash_info_full(void);
extern void battery_refresh(void);
//extern void device_led_off(u8 led);

_attribute_data_retention_    u8    sendTerminate_before_enterDeep = 0;
_attribute_data_retention_    u32   lowBattDet_tick   = 0;

#if (APP_IR_ENABLE)
extern u8 ir_flash_erase_flag;
//extern void ir_flash_set_flag(u8 flag);
#endif

#if (UART_PRINT_ENABLE)
_attribute_data_retention_    u8 printf_uart_flag=0;
void app_set_printf_uart_flag(unsigned char data)
{
    printf_uart_flag = data;
}
#endif

void app_device_init(void)
{

}

void app_enter_deep_timeset(void)
{
    enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
}


#if (UART_PRINT_ENABLE)
void log_uart_init(void)
{
    gpio_setup_up_down_resistor(UART_TX_PB1, PM_PIN_PULLUP_10K);  //must, for stability and prevent from current leakage
    gpio_set_func(UART_TX_PB1,AS_UART); // set tx pin
    gpio_set_input_en(UART_TX_PB1, 1);  //experiment shows that tx_pin should open input en(confirmed by qiuwei)
    uart_init_baudrate(115200,24000000,PARITY_NONE, STOP_BIT_ONE); //baud rate: 115200
    uart_ndma_clear_tx_index();
    //uart_dma_enable(0, 0);
    //irq_disable_type(FLD_IRQ_DMA_EN);
    //dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 0);
    //uart_irq_enable(0,0);   //uart RX irq enable
    //uart_ndma_irq_triglevel(1,0);    //set the trig level. 1 indicate one byte will occur interrupt
}
#endif

_attribute_ram_code_ void    user_set_rf_power (u8 e, u8 *p, int n)
{
#if (MP_TEST_MODE)
    if (gpio_read(DUT_INPUT_PORT) && test_get_mode() == MODE_TEST)
        rf_set_power_level_index(RF_POWER_P7p37dBm);
    else
#endif
    rf_set_power_level_index (MY_RF_POWER_INDEX);
}


#define PEER_MAC_FLASH_ERASE_TIMEOUT 600000

int app_peer_mac_flash_erase_timeoutcb(void)
{
    printf("app_peer_mac_flash_erase_timeoutcb\r\n");

    if(bls_ll_requestConnBrxEventDisable() > 120)
    {
        printf("app_peer_mac_flash_erase \r\n");
        bls_ll_disableConnBrxEvent();
        flash_erase_sector(MAC_DATA_SECT_ADDR);
        bls_ll_restoreConnBrxEvent();
        printf("app_peer_mac_flash_erase succ\r\n");
        write_peer_mac_info(peer_mac);
        return -1;
    }

    return 0;
}

void app_peer_mac_flash_erase(void)
{
    blt_soft_timer_add(app_peer_mac_flash_erase_timeoutcb, PEER_MAC_FLASH_ERASE_TIMEOUT);
}

#if (APP_IR_ENABLE)
void app_save_peer_mac_info(u8 *mac)
{

    if((peer_mac[0] == 0) && (peer_mac[1] == 0))
    {
        memcpy(peer_mac,mac,6);
        write_peer_mac_info(peer_mac);
    }
    else
    {
        printf("peer_mac =\r\n");
        for(u8 i=0;i<6;i++)
            printf(" %x",mac[i]);
        if(memcmp(mac,peer_mac,6))
        {
            printf("new mac\r\n");
            memcpy(peer_mac,mac,6);
            ir_table_init();
            write_ir_key_event_notify(0);
            flag_ccc_data = 0;
            atv_char_ctl_ccc = 0;
            atv_char_rx_ccc = 0;
            ir_flash_erase_flag = 0x55;
            write_ccc_info(&flag_ccc_data);
            if(is_peer_mac_flash_info_full() == 0)
               write_peer_mac_info(peer_mac);
            else
                  app_peer_mac_flash_erase();
        }
    }
}

void app_cmp_mac(void)
{
    smp_param_save_t  bondInfo;

    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();
    if(bond_number)
    {
        printf("app_cmp_mac\r\n");
        bls_smp_param_loadByIndex(0, &bondInfo);
        app_save_peer_mac_info(bondInfo.peer_addr);
    }
}
#endif

//----------------------------------------------------------------------------
void task_before_deep(){
    // not used pin ouput disable , input enable and pull down, to avoid current leakage,
    //...

    //or UNUSED_GPIO_PULLDOWN_ENABLE
}

void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
    if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_16M_SYS_TIMER_CLK_1MS){  //suspend time > 30ms.add gpio wakeup
        bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio pad wakeup suspend/deepsleep
    }
}


void blt_pm_proc(void)
{
#if(BLE_REMOTE_PM_ENABLE)

#if (PTM)
    extern bool ptm_enabled;
#endif

#if (UART_PRINT_ENABLE)
    // 串口正在打印，不进入低功耗模式
    if(printf_uart_flag) {
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
        return;
    }
#endif

    // 卡键处理(按键按住并长时间未释放)
#if (STUCK_KEY_PROCESS_ENABLE)
    extern u32 stuckkey_keypresstimer;
    if(key_not_released && stuckkey_keypresstimer && clock_time_exceed(stuckkey_keypresstimer, STUCK_KEY_ENTERDEEP_TIME*1000000))
    {
        printf("stuck key\r\n");
        stuckkey_keypresstimer = 0;
        extern u32 drive_pins[];
        u32 pin[] = KB_DRIVE_PINS;
        for(int i=0; i<(sizeof(pin)/sizeof(*pin));i++) {
            extern u8 stuckKeyPress[];
            if(stuckKeyPress[i]) {
                printf("stuck key i=%x\r\n",i);
                cpu_set_gpio_wakeup(drive_pins[i], KB_LINE_HIGH_VALID, 1);//set H active , then if key release , wakeup from deep
            }
            else {
                cpu_set_gpio_wakeup(drive_pins[i], !KB_LINE_HIGH_VALID, 0);
            }
        }
        app_release_hid_key();
        printf("stuckkey to deep\r\n");        
        task_before_deep();
        cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K,PM_WAKEUP_PAD,0);
        printf("stuckkey to deep fail\r\n");
    }
#endif

    // LED正在工作，不进入睡眠状态
    if(device_led_busy()) {
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
        return;
    }

#if (APP_IR_ENABLE)
    if(ir_flash_busy())
    {
        //printf("flash busy\n");
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
        return;
    }
#endif

    // 语音或者OTA模块正在工作
    if((ui_mic_enable || google_voice_ctl || ota_is_working)) {
        //printf("pm mic or voice ctl or ota ");
        bls_pm_setSuspendMask (SUSPEND_DISABLE);
    }

    // 红外正在发码,不进入睡眠状态
#if(REMOTE_IR_ENABLE)
    else if (ir_send_ctrl.is_sending) {
        //printf("pm ir sending ");
        bls_pm_setSuspendMask(SUSPEND_DISABLE);
    }
#endif

#if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE)
    else if( blc_phy_isPhyTestEnable() )
    {
        bls_pm_setSuspendMask(SUSPEND_DISABLE); //phy test can not enter suspend
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

#if (PTM)
    else if (ptm_enabled)
    {
        bls_pm_setSuspendMask(SUSPEND_DISABLE);
    }

#endif
    else
    {
        #if (PM_DEEPSLEEP_RETENTION_ENABLE)
        bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
        #else
        bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
        #endif

        // 有活动时,进入suspend功耗模式(广播和连接)
        int user_task_flg = ota_is_working || scan_pin_need || key_not_released || ui_mic_enable;
        if(user_task_flg){
            //printf("pm user task\n");
            bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

            #if (LONG_PRESS_KEY_POWER_OPTIMIZE)
            extern int key_matrix_same_as_last_cnt;
            if(!ota_is_working && key_matrix_same_as_last_cnt > 5){  //key matrix stable can optize
                bls_pm_setManualLatency(2);
            } 
            else{
                bls_pm_setManualLatency(0);  //latency off: 0
            }
            #else
            bls_pm_setManualLatency(0);
            #endif
        }

        // 进入睡眠时主动断开连接
        if(sendTerminate_before_enterDeep == 1){ //sending Terminate and wait for ack before enter deepsleep
            if (user_task_flg) {  //detect key Press again,  can not enter deep now
                sendTerminate_before_enterDeep = 0;
                if(!is_batt_low()) { 
                    bls_ll_setAdvEnable(1); 
                }
            }
        }
        else if(sendTerminate_before_enterDeep == 2){  //Terminate OK
            analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) | CONN_DEEP_FLG);
            printf("deep conn idle or user terminate\n");
            sendTerminate_before_enterDeep = 0;
            task_before_deep();
            // cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
            cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD, 0);
        }

        // 没有待处理的蓝牙事件
        if(!blc_ll_isControllerEventPending()) {  //no controller event pending
            //set adv to deep as 60 or 3s (ADV_IDLE_ENTER_DEEP_TIME = 60 or 3s)
            if ((blc_ll_getCurrentState() == BLS_LINK_STATE_ADV) && clock_time_exceed(advertise_begin_tick, adv_time_to_deep * 1000000)) {
                printf("deep adv idle\n");
                task_before_deep();
                // cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
                cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD, 0);
            }

            // connect time out to deep
            if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && !user_task_flg && 
                clock_time_exceed(latest_user_event_tick, CONN_IDLE_ENTER_DEEP_TIME * 1000000) )
            {
                bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
                bls_ll_setAdvEnable(0);   //disable adv
                sendTerminate_before_enterDeep = 1;
                printf("no action to terminate connection\n");
            }


            // idle to deep
            if ( (blc_ll_getCurrentState() != BLS_LINK_STATE_CONN) && (blc_ll_getCurrentState() != BLS_LINK_STATE_ADV) &&  !user_task_flg ) {
                if (clock_time_exceed(latest_user_event_tick, IR_IDLE_ENTER_DEEP_TIME * 1000000)) { //when pairing, need to add up at least 4s
                    printf("device idle to deep\n");
                    #if (GOOGLE_OTA)
                    app_schedule_ota_enter_deep();
                    #else
                    task_before_deep();
                    // cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
                    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD, 0);
                    #endif
                }
            }
        }
    }
#endif
}

_attribute_data_retention_ u8 power_on_from=POWER_ON_NORMAL;          //1: the rcu previous state is power down 2: the rcu previous state is power down

typedef struct{
    u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
    u8    type;               //RA(1)_TA(1)_RFU(2)_TYPE(4)
    u8    rf_len;             //LEN(6)_RFU(2)
    u8    advA[6];            //address
    u8    data[31];           //0-31 byte
}rf_packet_adv_ind_module_t;

void app_tx_power_wakeup(u8 keyid)
{
    rf_packet_adv_ind_module_t tx_power_wakeup;
    smp_param_save_t  bondInfo;
    u8 buffer[40];
    u8 index=0,i;
    u8 pos_index[3]={0};

    printf("app_tx_power_wakeup\r\n");
    rf_set_ble_access_code_adv();
    rf_set_ble_crc_adv();
    bls_smp_param_loadByIndex(0, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

    flash_read_page(0x78000+wakeup_key_pos, 40, buffer);

    //google format
    {
        tx_power_wakeup.rf_len = 19;
        tx_power_wakeup.dma_len = tx_power_wakeup.rf_len + 2;
        tx_power_wakeup.type = ADV_TYPE_CONNECTABLE_UNDIRECTED;

        memcpy(&(tx_power_wakeup.advA[0]),mac_public,6);
        tx_power_wakeup.data[0] = 0x0c;
        tx_power_wakeup.data[1] = 0x16;
        tx_power_wakeup.data[2] = 0x36;
        tx_power_wakeup.data[3] = 0xfd;
        tx_power_wakeup.data[4] = 0x01;
        if(keyid == 0xff)
        {
            tx_power_wakeup.data[5] = 0x02;  //schedule ota wakeup packet
        }
        else
        {
            // tx_power_wakeup.data[5] = app_custom_wakeupkey_packet_index(keyid);
            tx_power_wakeup.data[5] = 0;
        }
        if(read_wakeup_keyindex_info(&index) == 0)
        {
            if(keyid == 0xff)
            {
                if(index)
                  tx_power_wakeup.data[6] = index-1;
                else
                  tx_power_wakeup.data[6] = 0;
            }
            else
                tx_power_wakeup.data[6] = index;
        }
        else
            tx_power_wakeup.data[6] = 0;

        memcpy(&(tx_power_wakeup.data[7]), bondInfo.peer_addr, 6);
    }

    for(u8 i=0;i<3;i++)
    {
       rf_set_ble_channel(37+i);

       rf_start_stx (&tx_power_wakeup, 100);

       while(rf_tx_finish()!=0x01);

       rf_tx_finish_clear_flag();
    }

    //custom format
    if(buffer[0] != 0xff)
    {
        tx_power_wakeup.rf_len = buffer[0]+6;
        tx_power_wakeup.dma_len = tx_power_wakeup.rf_len + 2;
        tx_power_wakeup.type = 0;
        memcpy(&(tx_power_wakeup.advA[0]),mac_public,6);
        for(i=0;i<buffer[0];i++)
            tx_power_wakeup.data[i] = buffer[4+i];
        printf("raw packet =\r\n");
        for(i=0;i<buffer[0];i++)
            printf(" %x",tx_power_wakeup.data[i]);
        for(i=0;i<3;i++)
        {
            if(buffer[i])
            {
                pos_index[i] = buffer[i+1];
            }
            else
                pos_index[i] = 0xff;
        }
        printf("\r\npos_index[0]=%x,pos_index[1]=%x,pos_index[2]=%x\r\n",pos_index[0],pos_index[1],pos_index[2]);

        for(i=0;i<buffer[0];i++)
        {
            if(i == pos_index[0])
            {
                if(keyid == 0xff)
                    tx_power_wakeup.data[i] = 2;
                else
                    // tx_power_wakeup.data[i] = app_custom_wakeupkey_packet_index(keyid);
                    tx_power_wakeup.data[i] = 0;
            }
            else if(i == pos_index[1])
            {
                 if(read_wakeup_keyindex_info(&index) == 0)
                 {
                    if(keyid == 0xff)
                    {
                        if(index)
                            tx_power_wakeup.data[i] = index-1;
                        else
                            tx_power_wakeup.data[i] = 0;
                    }
                    else
                        tx_power_wakeup.data[i] = index;
                 }
                 else
                     tx_power_wakeup.data[i] = 0;
                 printf("tx_power_wakeup.data[i]=%x\r\n",tx_power_wakeup.data[i]);
            }
            else if(i == pos_index[2])
            {
                 memcpy(&(tx_power_wakeup.data[i]), bondInfo.peer_addr, 6);
                 i += 5;
            }
        }

        printf("wakeup packet =\r\n");
        for(i=0;i<buffer[0];i++)
            printf(" %x",tx_power_wakeup.data[i]);
        for(u8 i=0;i<3;i++)
        {
           rf_set_ble_channel(37+i);

           rf_start_stx (&tx_power_wakeup, 100);

           while(rf_tx_finish()!=0x01);

           rf_tx_finish_clear_flag();
        }

    }

    if(keyid != 0xff)
    {
        index++;
        write_wakeup_keyindex_info(index);
    }
}

void app_init_normal(void)
{
    //random number generator must be initiated here( in the beginning of user_init_nromal)
    //when deepSleep retention wakeUp, no need initialize again
    random_generator_init();  //this is must

////////////////// BLE stack initialization ////////////////////////////////////
    //u8 *pREMOTE_R52 = (u8 *)("AHLRC"); //APHRC
    u8 *pREMOTE_R52 = (u8 *)("PHLRC"); //APHRC for linux ota test
    my_devNamelen = 5;//strlen(pREMOTE_R52)-1;
    memcpy(my_devName,pREMOTE_R52,my_devNamelen);
    //app_custom_ui_layout(0);
    //app_custom_param_init(device_type); // key table init
    u8  mac_random_static[6];
    //for 512K Flash, flash_sector_mac_address equals to 0x76000
    //for 1M   Flash, flash_sector_mac_address equals to 0xFF000
    blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);
    printf("mac_public=[%02X %02X %02X %02X %02X %02X]\r\n",mac_public[5],mac_public[4], mac_public[3], mac_public[2], mac_public[1], mac_public[0]);
    #if(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_PUBLIC)
        app_own_address_type = OWN_ADDRESS_PUBLIC;
    #elif(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_RANDOM_STATIC)
        app_own_address_type = OWN_ADDRESS_RANDOM;
        blc_ll_setRandomAddr(mac_random_static);
    #endif

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

    //after patch new liblt_8278.a, blc_att_prepareWriteBufferCfg() is not available.
    //if want to use goolge IR , need to set as prepareWrite_b 300
    //default as 256
    //extern ble_sts_t blc_att_prepareWriteBufferCfg (u8 *pBuf, u16 bufSize);
    //u8 status = blc_att_prepareWriteBufferCfg (prepareWrite_b, sizeof(prepareWrite_b));
    //if(status != BLE_SUCCESS) { while_1_debug(); }  //debug: l2cap buffer setting err

    blc_att_setRxMtuSize(MTU_SIZE_SETTING);

    //Smp Initialization may involve flash write/erase(when one sector stores too much information,
    //   is about to exceed the sector threshold, this sector must be erased, and all useful information
    //   should re_stored) , so it must be done after battery check
    #if (BLE_REMOTE_SECURITY_ENABLE)
        blc_smp_peripheral_init();

        //Hid device on android7.0/7.1 or later version
        // New paring: send security_request immediately after connection complete
        // reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
        blc_smp_configSecurityRequestSending(SecReq_PEND_SEND, SecReq_PEND_SEND, 8000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection )
        //blc_smp_configSecurityRequestSending(SecReq_NOT_SEND, SecReq_NOT_SEND, 1000);
    #else
        blc_smp_setSecurityLevel(No_Security);
    #endif

///////////////////// USER application initialization ///////////////////
    app_set_advdata_rspdata();

    if(analog_read(USED_DEEP_ANA_REG) & POWER_ON_FLG) //already powered on once
    {
        //debug only, deep wake up should go here..
        //printf("power on deep\r\n");
        printf("------------------ power on deep -----------------------------\r\n");
        power_on_from = POWER_ON_DEEP;
//        if (kb_scan_key(KB_NUMLOCK_STATUS_POWERON, 1) && kb_event.cnt) {
//            //memcpy(&kb_event_cache, &kb_event, sizeof(kb_event));
//            printf("kb_event.cnt %d, kb_event.keycode %d\n", kb_event.cnt,kb_event.keycode[0]);
//            if ((kb_event.cnt == 1) && (kb_event.keycode[0] == KEY_OK)) {
//                printf("deep back ok key pressed\n");
//            }
//        }

    }
    else
    {
        #define KEY_BACK 26 //forgive me to make it dependent with app_ui.c
        #define KEY_VOL_DOWN    38

        power_on_from = POWER_ON_NORMAL;
        printf("------------------ power on Cold start -----------------------------\r\n");
        if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON, 1) && kb_event.cnt){
             memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
             printf("kb_event.cnt %d, kb_event.keycode %d\n",kb_event.cnt, kb_event.keycode[0]);
             //power_on_pressed_key = kb_event.keycode[0];
             if( (kb_event.cnt == 1) && (kb_event.keycode[0] == KEY_BACK) ){
                 extern u32 ftick_power_on_key_pressed;
                 ftick_power_on_key_pressed = clock_time()|TRUE;
                 printf("power on back key pressed\n");
             }

             if (kb_event.cnt == 1 && kb_event.keycode[0] == KEY_VOL_DOWN) {
                extern u32 ftick_vol_up_pressed;
                ftick_vol_up_pressed = clock_time() | TRUE;
                printf("power on VOL- key pressed\n");
                app_adv_indirect(); // 启动广播

             }
             // process in post
        }

    }
    u8 data = analog_read(DEEP_ANA_REG2);
    if(data == 0x0f)
        analog_write(DEEP_ANA_REG2, 0);
    app_schedule_ota_wakeup_host();

    //is_pairing_mode = 0;
    device_in_connection_state = 0;
    //is_reconn_mode = 0;

    ////////////////// config adv packet /////////////////////

    blc_smp_param_setBondingDeviceMaxNumber(1);
#if (MP_TEST_MODE)
    if (test_get_mode() != MODE_TEST)
#endif
    {
        //bls_smp_eraseAllParingInformation();

        u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
        smp_param_save_t  bondInfo;
        if(bond_number)   //at least 1 bonding device exist
        {
           bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )
        }
        printf("init bond_number %d\n",bond_number);


        if(bond_number)   //set direct adv
        {
            //app_set_adv_interval_downgrade_direct();
            printf("peer_addr=%x %x %x %x %x %x\r\n",bondInfo.peer_addr[0],bondInfo.peer_addr[1],bondInfo.peer_addr[2],bondInfo.peer_addr[3],bondInfo.peer_addr[4],bondInfo.peer_addr[5]);
//            if((analog_read(USED_DEEP_ANA_REG) & OTA_FLG) == OTA_FLG) // if deep from OTA, call a service change(no more used)
//            {
//                printf("ana read = ota succ\r\n");
//                service_change = TRUE;
//            }
            enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;
            //set direct adv
            u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                                            ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                                            bondInfo.peer_addr_type,  bondInfo.peer_addr,
                                            MY_APP_ADV_CHANNEL,
                                            ADV_FP_NONE);
            if(status != BLE_SUCCESS) { 
                while (1) {
                    printf("adv setting failed");    //debug: adv setting err
                }
            }
            //it is recommended that direct adv only last for several seconds, then switch to indirect adv
            set_app_adv_type(ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY);


            //recursively call to direct adv
            bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &indirect_adv);

            bls_ll_setAdvDuration(2000000, 1); // 2s
            #if (EQ_TEST)
                bls_ll_setAdvEnable(1);  printf("power on direct adv\r\n");
            #endif
            //adv_time_to_deep = RECONN_ADV_ENTER_DEEP_TIME;


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
            if(status != BLE_SUCCESS) { 
                while(1) {
                    printf("adv setting failed");
                }
            }  //debug: adv setting err
            set_app_adv_type(ADV_TYPE_CONNECTABLE_UNDIRECTED);

            #if (EQ_TEST)
                bls_ll_setAdvEnable(1);  printf("power on indirect adv\r\n");
            #endif
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
        if(status != BLE_SUCCESS) { while_1_debug(); }  //debug: adv setting err
        set_app_adv_type(ADV_TYPE_CONNECTABLE_UNDIRECTED);

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

#if (GOOGLE_OTA)//(BLE_REMOTE_OTA_ENABLE)
    ////////////////// OTA relative ////////////////////////
    bls_ota_clearNewFwDataArea(); //must
    bls_ota_init_handle(OTA_CMD_OUT_DP_H_GL);
#endif
#if (TELINK_OTA)
    bls_ota_clearNewFwDataArea(); //must
    bls_ota_registerStartCmdCb(app_enter_ota_mode);
    //bls_ota_registerResultIndicateCb(app_debug_ota_result);  //debug
#endif

    app_ui_init_normal();

    advertise_tick_retime();

    //user data init
    init_ccc_value();
    printf("init_ccc_value data:0x%x\n",flag_ccc_data);
    is_flash_info_full();
    init_peer_mac();
#if (APP_IR_ENABLE)
    app_ir_programming_end_register(app_delay_set);
    ir_param_init(); //init ir which read from TV, do macro disable
    u8 ccc = init_ir_key_event_notify();
    if(ccc != 0xff)
        ir_init_key_event_notify(ccc);
#endif
}


_attribute_ram_code_ void app_init_deepRetn(void)
{
#if (PM_DEEPSLEEP_RETENTION_ENABLE)
    blc_ll_initBasicMCU();   //mandatory
#if (MP_TEST_MODE)
    if (gpio_read(DUT_INPUT_PORT) && test_get_mode() == MODE_TEST)
        rf_set_power_level_index(RF_POWER_P7p37dBm);
    else
#endif
    rf_set_power_level_index (MY_RF_POWER_INDEX);

    blc_ll_recoverDeepRetention();
    blc_att_setRxMtuSize(MTU_SIZE_SETTING);

    DBG_CHN0_HIGH;    //debug

    irq_enable();

    app_ui_init_deepRetn();
#endif
}

u32 ftick_loop;
u8 tick_loop;
void app_loop (void)
{
    if(clock_time_exceed(ftick_loop, 1*1000*1000)){
        tick_loop ++;
        ftick_loop = clock_time();
    }

#if (BLT_TEST_SOFT_TIMER_ENABLE)
    blt_soft_timer_process(MAINLOOP_ENTRY);
#endif
    ////////////////////////////////////// BLE entry /////////////////////////////////
    blt_sdk_main_loop();

    mtu_exchange_loop();
#if (APP_IR_ENABLE)
    ir_flash_erase_loop();
    ir_flash_save_loop();
#endif
    ////////////////////////////////////// UI entry /////////////////////////////////
#if (BLE_AUDIO_ENABLE)
    app_audio_task();
#endif
#if (GOOGLE_OTA)
    app_ota_proc();
#endif

#if (BATT_CHECK_ENABLE)
    // 如果红外有发码,延迟一个周期后再检测电量
    if (ir_send_ctrl.is_sending) {
        lowBattDet_tick = clock_time();
    }
    if(battery_get_detect_enable() && clock_time_exceed(lowBattDet_tick, 300000)){
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
        app_battery_power_check(vbat_deep_thres,vbat_suspend_thres);

        #else
            app_battery_power_check(VBAT_DEEP_THRES_MV);  //2000 mV low battery
        #endif
    }
#endif

    proc_keyboard (0,0, 0);
    ui_key_post_process();
    ui_post_process();

#if (BLT_APP_LED_ENABLE)
    app_led_proc2();
    device_led_process();
#endif

    blt_pm_proc();
}
