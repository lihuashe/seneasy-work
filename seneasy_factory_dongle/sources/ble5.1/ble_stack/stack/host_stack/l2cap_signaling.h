#ifndef _L2CAP_SIGNALING_
#define _L2CAP_SIGNALING_

#define L2CAP_CMD_REJECT                    0x01
#define L2CAP_CMD_BLE_DISCNNT_REQ            0x6
#define L2CAP_CMD_BLE_DISCNNT_RSP            0x7
#define L2CAP_CMD_BLE_UPDATE_REQ            0x12
#define L2CAP_CMD_BLE_UPDATE_RSP            0x13
#define L2CAP_CMD_Credit_Based_Connection_REQ 0x14
#define L2CAP_CMD_Credit_Based_Connection_RSP 0x15
#define L2CAP_CMD_Flow_Control_Credit  0x16

#define L2CAP_BLE_UPDATE_RSP_ACCEPT            0x0000
#define L2CAP_BLE_UPDATE_RSP_REJECT            0x0001
 
extern void rom_l2cap_signaling_handler(uint16_t conn_handle, uint8_t *packet, uint16_t size);
extern int rom_l2cap_conn_para_update_req(uint16_t conn_handle, uint16_t min_intvl_25ms ,uint16_t max_intvl_25ms ,uint16_t slave_latency ,uint16_t timeout_10ms );

extern uint8_t * rom_l2cap_get_avail_data_buffer(uint16_t u16BufLen);
//extern void l2cap_tx_buffer_commit(uint8_t idx, uint16_t CID, uint8_t *p_data, uint16_t data_size, uint8_t need_feedback);
extern uint8_t l2cap_get_avail_data_buffer_num(void);


#endif
