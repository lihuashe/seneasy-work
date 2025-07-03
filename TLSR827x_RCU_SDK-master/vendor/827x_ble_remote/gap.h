#ifndef __GAP_H
#define __GAP_H


#define     ADV_INTERNAL_SWITCH_TIME            30  //30s
#define     MY_APP_ADV_CHANNEL                  BLT_ENABLE_ADV_ALL
#define     MY_ADV_INTERVAL_MIN                 ADV_INTERVAL_20MS
#define     MY_ADV_INTERVAL_MAX                 ADV_INTERVAL_25MS

//#define   ADV_IDLE_ENTER_DEEP_TIME            60  //spec 60s
#define     PAIR_ADV_ENTER_DEEP_TIME            60  //spec 60s
#define     RECONN_ADV_ENTER_DEEP_TIME          3   //spec 60s

#define     ADV_RECONN_ENTER_DEEP_COUNT         1
#define     ADV_IDLE_ENTER_DEEP_COUNT           5
#define     CONN_IDLE_ENTER_DEEP_TIME           60   //60  //6s for test , spec 60s,
#define     IR_IDLE_ENTER_DEEP_TIME             5   //120s for development, spec not defined but better set to 2s

//#define MTU_SIZE_SETTING          72
#define MTU_SIZE_SETTING            180
//#define MTU_SIZE_SETTING          210

//#define RX_FIFO_SIZE              96                    //-24//64
//#define RX_FIFO_SIZE              160                    //-24//64
#define RX_FIFO_SIZE                160                    //-24//64
#define RX_FIFO_NUM                 8

//#define TX_FIFO_SIZE              88                    //-12//40
#define TX_FIFO_SIZE                148                    //-12//40
#define TX_FIFO_NUM                 8

void app_set_advdata_rspdata(void);

void app_adv_indirect(void);

void indirect_adv(u8 e, u8 *p, int n);
void app_set_adv_interval_downgrade_indirect(void);
void advertise_tick_retime(void);
void set_app_adv_type(adv_type_t type);

int app_host_event_callback (u32 h, u8 *para, int n);

void task_connect (u8 e, u8 *p, int n);
void ble_remote_terminate(u8 e,u8 *p, int n); //*p is terminate reason
void ble_ll_reject(u8 e,u8 *p, int n); //*p is terminate reason
void task_conn_update_req (u8 e, u8 *p, int n);
void task_conn_update_done (u8 e, u8 *p, int n);
void    task_datalengthexchange (u8 e, u8 *p, int n);
int app_conn_param_update_response(u8 id, u16  result);

void mtu_exchange_loop(void);

// export variables
extern u32  adv_time_to_deep;
extern u32  advertise_begin_tick;
extern u32  latest_user_event_tick;
extern int  device_in_connection_state;
extern u16   enterdeep_time;

// import variables:app.c
extern u8    sendTerminate_before_enterDeep;
extern own_addr_type_t app_own_address_type;
#endif