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

//u8 flag_mtu=0;
_attribute_data_retention_    u8    flag_dle=0;
_attribute_data_retention_    int   device_in_connection_state=0;

_attribute_data_retention_    u32   advertise_begin_tick;
//_attribute_data_retention_    bool  advertise_downgrade_flag=TRUE;


_attribute_data_retention_    u32   latest_user_event_tick;

_attribute_data_retention_    u32   mtuExchange_check_tick=0;
_attribute_data_retention_    u32   mtuExchange_check_timer=12000000;

_attribute_data_retention_    u32   ir_flash_erase_tick=0;

_attribute_data_retention_    u32   delay3s_to_sleep_tick=0;
_attribute_data_retention_    u32   adv_time_to_deep=0;

_attribute_data_retention_    u16   enterdeep_time = 0;
_attribute_data_retention_    u16   enterdeep_time_count = 0;
//_attribute_data_retention_    bool   service_change = FALSE;

_attribute_data_retention_    u8    device_timeout_state=0;
_attribute_data_retention_    u8    flag_schedule_ota=0;
_attribute_data_retention_    u16   count_schedule_ota=0;

_attribute_data_retention_    u8    app_slave_terminate=0;
_attribute_data_retention_    u8    app_is_set_mtusize=0;
//_attribute_data_retention_    u8    prepareWrite_b[300];

adv_type_t adv_type = ADV_TYPE_CONNECTABLE_UNDIRECTED;

//////////////////////////////////////////////////////////////////////////////
//     Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
const u8    tbl_advData_part2[] = {
     0x02, 0x01, 0x05,                          // BLE limited discoverable mode and BR/EDR not supported
     0x03, 0x19, 0x80, 0x01,                    // 384, Generic Remote Control, Generic category
     0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,        // incomplete list of service class UUIDs (0x1812, 0x180F)
     //0x03, 0x02, 0x12, 0x18,         // incomplete list of service class UUIDs (0x1812)

     //0x13, 0xFF,0x3D,0x03,0x00,xx,xx,xx,xx,xx,xx,0x00,0x00,0x00,0x00
     //'P','H','L','R','C'

};

const u8    tbl_advData_TPV[] = {
     0x02, 0x01, 0x05,                          // BLE limited discoverable mode and BR/EDR not supported
     0x03, 0x19, 0x80, 0x01,                    // 384, Generic Remote Control, Generic category
     0x03, 0x02, 0x12, 0x18,                    // incomplete list of service class UUIDs (0x1812-Human Interface Device)
     0x13, 0xFF, 0x3D, 0x03,                    // Manufacturer Specific
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x50, 0x48, 0x4C, 0x52, 0x43
     //0x03, 0x02, 0x12, 0x18,         // incomplete list of service class UUIDs (0x1812)

     //0x13, 0xFF,0x3D,0x03,0x00,xx,xx,xx,xx,xx,xx,0x00,0x00,0x00,0x00
     //'P','H','L','R','C'

};

const u8 tbl_scanRsp[] = {
    0x06,0x09,'P','H','L','R','C'
};

void app_set_advdata_rspdata(void)
{
    bls_ll_setAdvData( (u8 *)tbl_advData_TPV, sizeof(tbl_advData_TPV));

    bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

    printf("adv data=\r\n");
    for(u8 i=0;i<sizeof(tbl_advData_TPV);i++)
    {
       printf(" %x ",tbl_advData_TPV[i]);
    }

    printf("\r\nscan data=\r\n");
    for(u8 i=0;i<my_devNamelen+2;i++)
    {
       printf(" %x ",tbl_scanRsp[i]);
    }

}

void indirect_adv_end(u8 e, u8 *p, int n){
    //blc_smp_setBondingMode(Bondable_Mode);
    //printf("recover to bondable mode\n");
}


#define while_1_debug()    {while (1) {printf("h"); }}

extern bool  rc_reconn_normal;
void indirect_adv(u8 e, u8 *p, int n)
{
    printf("indirect_adv1\r\n");
    app_set_advdata_rspdata();
    rc_reconn_normal = FALSE; // fall back, when connected  set to TRUE

    u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
            ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type, 0, NULL,
            MY_APP_ADV_CHANNEL, ADV_FP_NONE);
    if (status != BLE_SUCCESS) { while_1_debug(); }  //debug: adv setting err
    set_app_adv_type(ADV_TYPE_CONNECTABLE_UNDIRECTED);
    //bls_ll_setAdvDuration(n*1000 , 1); //3s
    bls_ll_setAdvDuration(3*1000*1000 , 1); //fix 3s here, beacuse direct_adv time out call to here as well

    bls_app_registerEventCallback(BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &indirect_adv_end); // end recursive call
    if( !is_batt_low() ) {
        adv_time_to_deep = RECONN_ADV_ENTER_DEEP_TIME;
        bls_ll_setAdvEnable(1);
        printf("indirect_adv2\r\n");
        //TODO bug: 1 time ok key does not actually trigger indirect?
    }
}

void direct_adv(u8 e, u8 *p, int n) // used for R52
{
    printf("direct_adv1\n");
    bls_ll_setAdvEnable(0);  //adv enable
    //is_reconn_mode = 0x55; // remove later,for led only
    app_set_advdata_rspdata();
    smp_param_save_t  bondInfo;
    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    if(bond_number)   //at least 1 bonding device exist
    {
       //app_set_adv_interval_downgrade_direct(); //when adv time out call direct 30~35MS
       bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

       u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                                        ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                                        bondInfo.peer_addr_type,  bondInfo.peer_addr,
                                        MY_APP_ADV_CHANNEL,
                                        ADV_FP_NONE);
       if(status != BLE_SUCCESS) { while_1_debug(); }  //debug: adv setting err
       set_app_adv_type(ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY);

       bls_ll_setAdvDuration(n, 1);
       //bls_ll_setAdvDuration(1*1000*1000, 1);
       //bls_ll_setAdvDuration(50*1000, 1);

       bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &indirect_adv); // set mode to fall back in indirect_adv
       if( !is_batt_low() ) {
           printf("direct_adv2\n");
           adv_time_to_deep = RECONN_ADV_ENTER_DEEP_TIME;
           bls_ll_setAdvEnable(1);
       }
    }
}

void set_app_adv_type(adv_type_t type){
    adv_type = type; // for debug printf only
}

void advertise_tick_retime(void)
{
    advertise_begin_tick = clock_time();
   //advertise_downgrade_flag = TRUE; // we dont need downgrade adv
    enterdeep_time_count = 0;
}


void app_adv_interval_downgrade_direct(void)
{
    smp_param_save_t  bondInfo;
    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    //direct adv
    if(bond_number)   //at least 1 bonding device exist
    {
       printf("app_adv_internal_downgrade_direct\r\n");
       bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

       u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_35MS,
                                        ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                                        bondInfo.peer_addr_type,  bondInfo.peer_addr,
                                        MY_APP_ADV_CHANNEL,
                                        ADV_FP_NONE);
       if(status != BLE_SUCCESS) { while_1_debug(); }  //debug: adv setting err
       set_app_adv_type(ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY);

    }
}

void app_adv_interval_downgrade_indirect(void)
{
    printf("app_adv_internal_downgrade_indirect\r\n");
    bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_35MS,
                        ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
                        0,  NULL,
                        MY_APP_ADV_CHANNEL,
                        ADV_FP_NONE);
    set_app_adv_type(ADV_TYPE_CONNECTABLE_UNDIRECTED);

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
    printf("app_adv_indirec\n");
    app_set_advdata_rspdata();

    bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
        ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
        0,    NULL,
        MY_APP_ADV_CHANNEL,
        ADV_FP_NONE);
    set_app_adv_type(ADV_TYPE_CONNECTABLE_UNDIRECTED);

    if( !is_batt_low() ) {
        adv_time_to_deep = PAIR_ADV_ENTER_DEEP_TIME;
        bls_ll_setAdvEnable(1);
        printf("app_adv_indirec start adv\n");

        //app_set_adv_interval_downgrade_indirect(); // when adv time out call indirect
    }
}



void app_adv_direct(void)
{
    printf("app_adv_direct\r\n");
    //is_reconn_mode = 0x55;
    smp_param_save_t  bondInfo;
    bls_ll_setAdvEnable(0);  //adv enable
    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    if(bond_number)   //at least 1 bonding device exist
    {
       app_set_adv_interval_downgrade_direct(); //when adv time out call direct 30~35MS
       bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

       u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                                        ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type,
                                        bondInfo.peer_addr_type,  bondInfo.peer_addr,
                                        MY_APP_ADV_CHANNEL,
                                        ADV_FP_NONE);
       if(status != BLE_SUCCESS) { while_1_debug(); }  //debug: adv setting err
       set_app_adv_type(ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY);

    }
    else
    {
        app_set_adv_interval_downgrade_indirect(); //when adv time out call indirect
    }
    if( !is_batt_low() ) { bls_ll_setAdvEnable(1); }

}


u8 effective_rx_dle = 0;
void    task_datalengthexchange (u8 e, u8 *p, int n)
{
    flag_dle |= 1;
    printf("task_datalengthexchange\n");
    ll_data_extension_t *data_p = (ll_data_extension_t *)p;
    effective_rx_dle = data_p->connEffectiveMaxRxOctets;
    printf("connEffective:rx:%d __ tx: %d\n",data_p->connEffectiveMaxRxOctets,data_p->connEffectiveMaxTxOctets);

    set_audio_frame_size(data_p->connMaxTxOctets);
    //set_audio_frame_size(23);
}

//void app_service_change(void)
//{
//    u8 service_data[4]={BATT_PS_H,0,0xff,0};
//    if(service_change)
//    {
//        printf("app_service_change\r\n");
//        bls_att_pushIndicateData(GenericAttribute_ServiceChanged_DP_H,service_data,sizeof(service_data));
//        analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) & (~OTA_FLG));
//        service_change = FALSE;
//    }
//}

void mtu_exchange_loop(void)
{
    if(mtuExchange_check_tick && clock_time_exceed(mtuExchange_check_tick, mtuExchange_check_timer ))
    {
        mtuExchange_check_tick = 0;
        app_schedule_ota_send_notify_to_continue_ota();
        if((flag_dle&0x10) == 0)
        {
            printf("1 MTU_SIZE %d\r\n", MTU_SIZE_SETTING);
            blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, MTU_SIZE_SETTING);
        }
        if((flag_dle&0x10) == 0x10)
        {
            if(app_mtu_size < MTU_SIZE_SETTING)
            {
                printf("2 MTU_SIZE %d\r\n", MTU_SIZE_SETTING);
                blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, MTU_SIZE_SETTING);
            }
        }
        if((flag_dle&0x01) == 0)
        {
            printf("blc_ll_exchangeDataLength\r\n");
            blc_ll_exchangeDataLength(LL_LENGTH_REQ,(TX_FIFO_SIZE-18));
            flag_dle |= 0x01;
        }
        if((flag_dle&0x80) == 0)
        {
            printf("bls_l2cap_requestConnParamUpdate 8, 8, 99, 400\r\n");
            bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);
            flag_dle |= 0x80;

            extern u32 ftick_voice_reconn_send_code;
            if (ftick_voice_reconn_send_code) {
                ftick_voice_reconn_send_code = 0;
                send_voice_ble();
            }

            extern u32 ftick_ok_reconn_send_code;
            if (ftick_ok_reconn_send_code) {
                ftick_ok_reconn_send_code = 0;
                send_ok_ble();
            }


        }
        //app_service_change();
    }
}

int app_enable_adv_timeout(void)
{
     printf("ble_ll_reject time out to indirect\r\n");
     //is_pairing_mode = 0x55;
     app_adv_indirect(); // for ll_reject_ind
     enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
     if(google_voice_ctl){
         google_voice_ctl = 0;
         get_cap_received = false;
     }
     if(ui_mic_enable){
        ui_enable_mic (0);
     }
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
    latest_user_event_tick = clock_time();

    printf("ble_ll_reject\r\n");
    if (device_in_connection_state == 0) {
        blt_soft_timer_delete(app_terminate_timeout);
        //blt_soft_timer_add(app_enable_adv_timeout, 1000000);
        ll_reject_to_adv = TRUE;
        ble_sts_t status = app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN);
        printf("status=%x\r\n", status);
        if (google_voice_ctl & FLAG_GOOGLE_CAPS_RESP)
            google_voice_ctl &= ~FLAG_GOOGLE_CAPS_RESP;
    }
}

#if (GOOGLE_OTA)
int app_ota_terminate_timeoutcb(void)
{
    printf("app_ota_terminate_timeoutcb\r\n");
    app_ota_terminate(OTA_USER_TERMINATE);
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
    printf("app_terminate_to_direct_timeoutcb\r\n");
    app_adv_direct();

    return -1;
}

void app_ui_unpair_user_wait_terminate(){
    extern u8 user_unpair_wait_terminate;
    extern u8 user_pair_wait_terminate;
    extern u32 ftick_ir_release_n_indirect_adv;

    if(user_pair_wait_terminate){
        printf("erase pair info33333333333333\n");
        bls_smp_eraseAllParingInformation();
        user_pair_wait_terminate = FALSE;
        ftick_ir_release_n_indirect_adv = clock_time()|TRUE; // TODO: test only 1 ir frame out, may time out and send 2
    }

    if(user_unpair_wait_terminate){
        printf("erase pair info 44444444444444444\n");
        bls_smp_eraseAllParingInformation();
        user_unpair_wait_terminate = FALSE;
    }

    //TODO : if again indirect adv will trigger host to reconnect
}



void ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{

    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    printf("ble_remote_terminate  bond_number=%x,*p=%x\r\n",bond_number,*p);

    app_led_disconnect();

    latest_user_event_tick = clock_time();


    //if(app_repairing() == 0)// don't need to repair
    {
        if(ota_is_working)
        {
            app_pairing_error();
            blt_soft_timer_add(app_ota_terminate_timeoutcb, 1000000);
            return;
        }
        if(*p == HCI_ERR_CONN_TIMEOUT){
            printf("HCI_ERR_CONN_TIMEOUT\r\n");
            device_timeout_state=0x55;
            //is_pairing_mode = 0;
            //is_reconn_mode = 0;
            if((flag_schedule_ota == 0x55) || (bond_number == 0))
            {
                enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                bls_ll_setAdvEnable(0);
            }
            else
            {
                enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;
                //app_adv_direct();
            }
        }
        else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

             printf("remote user terminate\r\n");
             app_ui_unpair_user_wait_terminate();   //handle user press key to unpair

             device_timeout_state=0x55;             //indicate OTA tiem out to deep

             //if(is_pairing_mode)    is_pairing_mode = 0;
             //if(is_reconn_mode)     is_reconn_mode = 0;

             bls_ll_setAdvEnable(0);
             if(app_slave_terminate == 0x55) // 0x55 means a slave terminate
             {
                  enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
                  app_slave_terminate = 0;
                  printf("slave terminate\r\n");
             }
             else
             {
                 if(device_in_connection_state == 0)
                 {
                     enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
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

                         //when user terminate , no more adv to reconn
                         //blt_soft_timer_add(app_terminate_to_direct_timeoutcb, 600000);
                     }
                 }
             }

             if(ll_reject_to_adv){
                 //app_enable_adv_timeout();
                 latest_user_event_tick = clock_time();

                 blt_soft_timer_add(app_enable_adv_timeout, 1000000); // 4s or 1s?

                 ll_reject_to_adv = FALSE;
             }

        }
        else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){
              //is_pairing_mode = 0;
              //is_reconn_mode = 0;
              app_pairing_error();
              enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
              bls_ll_setAdvEnable(0);
              printf("HCI_ERR_CONN_TERM_MIC_FAILURE\r\n");
              bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
              if(bond_number)
              {
                  enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;
              }
              else
                  enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
              app_adv_direct();
        }
        google_voice_ctl = 0;
        get_cap_received = 0;
    }

    device_in_connection_state = 0;
    mtuExchange_check_tick = 0;
    flag_dle = 0;

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
void     app_phy_update_complete_event(u8 e,u8 *p, int n)
{
}
#endif


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
                app_led_connect();
                #if (APP_IR_ENABLE)
                    app_cmp_mac();
                    printf("save smp key succ\n");
                #endif
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

            printf("GAP_EVT_SMP_CONN_ENCRYPTION_DONE \n");

            blt_soft_timer_delete(app_terminate_timeout);

            if(p->re_connect == SMP_STANDARD_PAIR){  //first paring
                printf("first pairing\r\n");

                app_is_set_mtusize = 0;
                //mtuExchange_check_timer = 12000000;// if receive IR from host
                mtuExchange_check_timer = 3000000;   // R52 don't need so long, make it quick for 1st time RC power on voice search

#if (APP_IR_ENABLE)
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
                // app_led_connect();
                mtuExchange_check_timer = 250000; //check later if vocie search start slow
                //mtuExchange_check_timer = 3000000; //check later if vocie search start slow
                app_is_set_mtusize = 1;
                printf("auto connect \n");
            }

            device_in_connection_state = 1;
            //is_pairing_mode = 0;
            //is_reconn_mode = 0;

            mtuExchange_check_tick = clock_time() | 1;
            notify_get_rsp_tick = clock_time()|1;



            battery_refresh();

            blt_soft_timer_add(app_cachekey_send_timer,30000);//TODO: remove this?

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

void    task_connect (u8 e, u8 *p, int n)
{
    printf("task_connect\n");
    app_mtu_size = 23;
    bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(1000); //1000
    enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;

    latest_user_event_tick = clock_time();
    #if (GOOGLE_OTA)
    blt_soft_timer_add(app_terminate_timeout, 2000000);//for google OTA?
    bls_ota_set_random(p+12);
    #endif
    rc_reconn_normal = TRUE; // normal mode

}

void    task_conn_update_req (u8 e, u8 *p, int n)
{
    printf("task_conn_update_req\r\n");
}

void    task_conn_update_done (u8 e, u8 *p, int n)
{
    printf("task_conn_update_done\n");
    latest_user_event_tick = clock_time();

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


void app_delay_set(void)
{
    //printf("app_delay_set\r\n");
    google_get_rsp_delay();
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

u8 is_mic_enable(void)
{
    return ui_mic_enable;
}