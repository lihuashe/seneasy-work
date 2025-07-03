/*********************************************************************
 * @file co_msg_param.h
 * @brief 
 * @version 1.0
 * @date Thu 06 Nov 2014 04:17:04 PM CST
 * @author XIHAO
 *
 * @note 
 */

#ifndef __CO_MSG_PARAM_H__
#define __CO_MSG_PARAM_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "features.h"

/*********************************************************************
 * MACROS
 */
#define MAX_ADV_DATA_LEN                    31

#define CO_GET_ACL_HANDLE(conhdl)           ((conhdl) & 0x0FFF)
#define CO_GET_ACL_PB_FLAG(conhdl)          ((acl_pb_type_t)(((conhdl) & 0x3000)>>12))
#define CO_SET_ACL_CONHDL(conhdl, hdl, pb)  do{conhdl = (hdl) | ((pb)<<12);}while(0)

/*********************************************************************
 * TYPEDEFS
 */

typedef enum
{
    ACL_PB_FIRST_NON_AUTO_FLUSH = 0x00,
    ACL_PB_CONTINUE             = 0x01,
    ACL_PB_FIRST_AUTO_FLUSH     = 0x02,
}acl_pb_type_t;

typedef uint8_t bd_addr_type_t;
typedef uint8_t bd_addr_t[6];
typedef uint8_t co_evt_mask_t[8];
typedef uint8_t co_chmap_t[5];
typedef uint8_t co_feats_t[8];
typedef uint8_t co_rand_nb_t[8];
typedef uint8_t co_ediv_t[2];
typedef uint8_t co_skd_t[8];
typedef uint8_t co_iv_t[4];
typedef uint8_t co_key_t[16];
typedef co_key_t co_ltk_t;
typedef uint8_t co_enc_data_t[16];
typedef uint8_t co_local_supp_cmds_t[64];
typedef uint8_t co_local_supp_stts_t[8];
typedef uint8_t co_dhkey_t[32];

typedef struct
{
    co_iv_t m;
    co_iv_t s;
}co_siv_t;

typedef struct
{
    co_skd_t m;
    co_skd_t s;
}co_sskd_t;

typedef struct
{
    uint8_t x[32];
    uint8_t y[32];
}co_p256_public_key_t;

/*****************************
 * HCI Commnad
 ******************************/

// MSG_CMD_LE_SET_EVENT_MASK
typedef struct
{
    co_evt_mask_t mask;
}msg_cmd_le_set_event_mask_t;

// MSG_CMD_LE_SET_RANDOM_ADDRESS
typedef struct
{
    bd_addr_t rand_addr;
}msg_cmd_le_set_random_address_t;

// MSG_CMD_LE_SET_ADV_PARAM
typedef struct
{
    uint16_t  intv_min;
    uint16_t  intv_max;
    uint8_t   type;
    uint8_t   own_addr_type;
    uint8_t   direct_addr_type;
    bd_addr_t direct_addr;
    uint8_t   chnl_map;
    uint8_t   filter_policy;
}msg_cmd_le_set_adv_param_t;

// MSG_CMD_LE_SET_ADV_DATA
typedef struct
{
    uint8_t len;
    uint8_t data[MAX_ADV_DATA_LEN];
}msg_cmd_le_set_adv_data_t;

/// MSG_CMD_LE_SET_SCAN_RESPONSE_DATA
typedef struct
{
    uint8_t len;
    uint8_t data[MAX_ADV_DATA_LEN];
}msg_cmd_le_set_scan_response_data_t;

// MSG_CMD_LE_SET_ADV_ENABLE
typedef struct
{
    uint8_t enable;
}msg_cmd_le_set_adv_enable_t;

// MSG_CMD_LE_SET_SCAN_PARAM
typedef struct
{
    uint8_t  type;
    uint16_t intv;
    uint16_t window;
    uint8_t  own_addr_type;
    uint8_t  filter_policy;
}msg_cmd_le_set_scan_param_t;

// MSG_CMD_LE_SET_SCAN_ENABLE
typedef struct
{
    uint8_t enable;
    uint8_t filter_duplicates_enable;
}msg_cmd_le_set_scan_enable_t;

// MSG_CMD_LE_CREATE_CONNECTION
typedef struct
{
    uint16_t  scan_intv;
    uint16_t  scan_window;
    uint8_t   init_filter_policy;
    uint8_t   peer_addr_type;
    bd_addr_t peer_addr;
    uint8_t   own_addr_type;
    uint16_t  intv_min;
    uint16_t  intv_max;
    uint16_t  latency;
    uint16_t  superv_to;
    uint16_t  ce_len_min;
    uint16_t  ce_len_max;
}msg_cmd_le_create_connection_t;

// MSG_CMD_LE_ADD_DEVICE_TO_WHITE_LIST
typedef struct
{
    uint8_t   dev_addr_type;
    bd_addr_t dev_addr;
}msg_cmd_le_add_device_to_white_list_t;

// MSG_CMD_LE_REMOVE_DEVICE_FROM_WHITE_LIST
typedef struct
{
    uint8_t   dev_addr_type;
    bd_addr_t dev_addr;
}msg_cmd_le_remove_device_from_white_list_t;


// MSG_CMD_LE_SET_HOST_CHANNEL_CLASS
typedef struct
{
    co_chmap_t chmap;
}msg_cmd_le_set_host_channel_class_t;


/// MSG_CMD_LE_RECEIVER_TEST
typedef struct
{
    uint8_t rx_freq;
}msg_cmd_le_receiver_test_t;

/// MSG_CMD_LE_TRANSMITTER_TEST
typedef struct
{
    uint8_t tx_freq;
    uint8_t payload_len;
    uint8_t payload_type;
}msg_cmd_le_transmitter_test_t;

/// MSG_CMD_LE_ENCRYPT
typedef struct
{
    co_ltk_t      ltk;
    co_enc_data_t plain_data;
}msg_cmd_le_encrypt_t;

// MSG_CMD_SET_EVENT_MASK
typedef struct
{
    co_evt_mask_t mask;
}msg_cmd_set_event_mask_t;

// MSG_CMD_SET_EVENT_MASK_PAGE2
typedef struct
{
    co_evt_mask_t mask;
}msg_cmd_set_event_mask_page2_t;

// MSG_CMD_SET_CONTROLLER_TO_HOST_FLOW_CONTROL
typedef struct
{
    uint8_t flow_cntl;
}msg_cmd_set_controller_to_host_flow_control_t;

// MSG_CMD_HOST_BUFFER_SIZE
typedef struct
{
    uint16_t acl_pkt_len;
    uint8_t  sync_pkt_len;
    uint16_t nb_acl_pkts;
    uint16_t nb_sync_pkts;
}msg_cmd_host_buffer_size_t;

// MSG_CMD_HOST_NUMBER_OF_COMPLETED_PACKETS
typedef struct
{
    uint8_t  nb_of_hdl;
    uint16_t con_hdl[1];
    uint16_t nb_comp_pkt[1];
}msg_cmd_host_number_of_completed_packets_t;

// MSG_CMD_LE_CON_UPDATE
typedef struct
{
    uint16_t conhdl;
    uint16_t intv_min;
    uint16_t intv_max;
    uint16_t latency;
    uint16_t superv_to;
    uint16_t ce_len_min;
    uint16_t ce_len_max;
}msg_cmd_le_con_update_t;

// MSG_CMD_LE_START_ENCRYPTION
typedef struct
{
    uint16_t     conhdl;
    co_rand_nb_t rand;
    co_ediv_t    ediv;
    co_ltk_t     ltk;
}msg_cmd_le_start_encryption_t;

//MSG_CMD_LE_LTK_REQ_REPLY 
typedef struct
{
    uint16_t conhdl;
    co_ltk_t ltk;
}msg_cmd_le_ltk_req_reply_t;

// MSG_CMD_LE_LTK_REQ_NEGATIVE_REPLY
typedef struct
{
    uint16_t conhdl;
}msg_cmd_le_ltk_req_negative_reply_t;

// MSG_CMD_LE_READ_CHANNEL_MAP
typedef struct
{
    uint16_t conhdl;
}msg_cmd_le_read_channel_map_t;

// MSG_CMD_DISCONNECT
typedef struct
{
    uint16_t conhdl;
    uint8_t  reason;
}msg_cmd_disconnect_t;

// MSG_CMD_LE_READ_REMOTE_USED_FEATURES
typedef struct
{
    uint16_t conhdl;
}msg_cmd_le_read_remote_used_features_t;

/// MSG_CMD_READ_RSSI
typedef struct
{
    uint16_t conhdl;
}msg_cmd_read_rssi_t;

// MSG_CMD_READ_TRANSMIT_POWER_LEVEL
typedef struct
{
    uint16_t conhdl;
    uint8_t  type;
}msg_cmd_read_transmit_power_level_t;

// MSG_CMD_READ_AUTH_PAYLOAD_TIMEOUT
typedef struct
{
    uint16_t conhdl;
}msg_cmd_read_auth_payload_timeout_t;

// MSG_CMD_WRITE_AUTH_PAYLOAD_TIMEOUT
typedef struct
{
    uint16_t conhdl;
    uint16_t timeout;
}msg_cmd_write_auth_payload_timeout_t;

// MSG_CMD_READ_REMOTE_VER_INFO
typedef struct
{
    uint16_t conhdl;
}msg_cmd_read_remote_ver_info_t;

// MSG_CMD_LE_CON_PARAM_REQ_REPLY
typedef msg_cmd_le_con_update_t msg_cmd_le_con_param_req_reply_t;

// MSG_CMD_LE_CON_PARAM_REQ_NEGATIVE_REPLY
typedef struct
{
    uint16_t conhdl;
    uint8_t  reason;
}msg_cmd_le_con_param_req_negative_reply_t;

// MSG_CMD_LE_SET_DATA_LEN
typedef struct
{
    uint16_t conhdl;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
}msg_cmd_le_set_data_len_t;

// MSG_CMD_LE_READ_SUGG_DEF_DATA_LEN
// No param

// MSG_CMD_LE_WRITE_SUGG_DEF_DATA_LEN
typedef struct
{
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
}msg_cmd_le_write_sugg_def_data_len_t;

// MSG_CMD_LE_READ_LOCAL_P256_PUBLIC_KEY
// No param

// MSG_CMD_LE_GENERATE_DHKEY
typedef struct
{
    co_p256_public_key_t remote_p256_public_key;
}msg_cmd_le_generate_dhkey_t;

// MSG_CMD_LE_ADD_DEVICE_TO_RESOLVE_LIST
typedef struct
{
    uint8_t   peer_identity_addr_type;
    bd_addr_t peer_identity_addr;
    co_key_t  peer_irk;
    co_key_t  local_irk;
}msg_cmd_le_add_device_to_resolve_list_t;

// MSG_CMD_LE_REMOVE_DEVICE_FROM_RESOLVE_LIST
typedef struct
{
    uint8_t   peer_identity_addr_type;
    bd_addr_t peer_identity_addr;
}msg_cmd_le_remove_device_from_resolve_list_t;

// MSG_CMD_LE_CLEAR_RESOLVE_LIST
// No Param

// MSG_CMD_LE_READ_RESOLVE_LIST_SIZE
// No Param

// MSG_CMD_LE_READ_PEER_RESOLVE_ADDR
typedef struct
{
    uint8_t   peer_identity_addr_type;
    bd_addr_t peer_identity_addr;
}msg_cmd_le_read_peer_resolve_addr_t;

// MSG_CMD_LE_READ_LOCAL_RESOLVE_ADDR
typedef struct
{
    uint8_t   peer_identity_addr_type;
    bd_addr_t peer_identity_addr;
}msg_cmd_le_read_local_resolve_addr_t;

// MSG_CMD_LE_SET_ADDR_RESOLVE_ENABLE
typedef struct
{
    uint8_t enable;
}msg_cmd_le_set_addr_resolve_enable_t;

// MSG_CMD_LE_SET_RESOLVE_PRIVATE_ADDR_TIMEOUT
typedef struct
{
    uint8_t timeout;
}msg_cmd_le_set_resolve_private_addr_timeout_t;

// MSG_CMD_LE_READ_MAX_DATA_LEN
// No Param

/*****************************
 * HCI Events
 ******************************/

// MSG_CMD_READ_LOCAL_FEATURES 
typedef struct
{
    uint8_t status;
    co_feats_t feats;
}msg_evt_read_local_features_cmd_cmp_t;

// MSG_CMD_LE_READ_BUFFER_SIZE
typedef struct
{
    uint8_t  status;
    uint16_t hc_data_pk_len;
    uint8_t  hc_tot_nb_data_pkts;
}msg_evt_le_read_buffer_size_cmd_cmp_t;

// MSG_CMD_LE_READ_LOCAL_SUPP_FEATURES
typedef struct
{
    uint8_t status;
    co_feats_t feats;
}msg_evt_le_read_local_supp_features_cmd_cmp_t;

// MSG_CMD_LE_READ_ADV_CHANNEL_TX_POWER
typedef struct
{
    uint8_t status;
    uint8_t adv_tx_pw_lvl;
}msg_evt_le_read_adv_channel_tx_power_cmd_cmp_t;

// MSG_CMD_LE_READ_WHITE_LIST_SIZE
typedef struct
{
    uint8_t status;
    uint8_t wlst_size;
}msg_evt_le_read_white_list_size_cmd_cmp_t;

// MSG_CMD_LE_RAND
typedef struct
{
    uint8_t status;
    co_rand_nb_t  rand;
}msg_evt_le_rand_cmd_cmp_t;

// MSG_CMD_LE_ENCRYPT
typedef struct
{
    uint8_t       status;
    co_enc_data_t encrypted_data;
}msg_evt_le_encrypt_cmd_cmp_t;

// MSG_CMD_LE_READ_SUPPORTED_STATES
typedef struct
{
    uint8_t              status;
    co_local_supp_stts_t le_states;
}msg_evt_le_read_supported_states_cmd_cmp_t;

// MSG_CMD_LE_TEST_END
typedef struct
{
    uint8_t  status;
    uint16_t nb_packet_received;
}msg_evt_le_test_end_cmd_cmp_t;

// MSG_CMD_READ_BD_ADDR
typedef struct
{
    uint8_t   status;
    bd_addr_t local_addr;
}msg_evt_read_bd_addr_cmd_cmp_t;

// MSG_CMD_READ_LE_HOST_SUPPORT
typedef struct
{
    uint8_t status;
    uint8_t le_supp_host;
    uint8_t simultaneous_le_host;
}msg_evt_read_le_host_support_cmd_cmp_t;

// MSG_CMD_READ_LOCAL_VER_INFO
typedef struct
{
    uint8_t  status;
    uint8_t  hci_ver;
    uint16_t hci_rev;
    uint8_t  lmp_ver;
    uint16_t manuf_name;
    uint16_t lmp_subver;
}msg_evt_read_local_ver_info_cmd_cmp_t;

// MSG_CMD_READ_LOCAL_SUPP_COMMANDS
typedef struct
{
    uint8_t              status;
    co_local_supp_cmds_t local_cmds;
}msg_evt_read_local_supp_commands_cmd_cmp_t;

// MSG_CMD_READ_BUFFER_SIZE
typedef struct
{
    uint8_t  status;
    uint16_t hc_data_pk_len;
    uint8_t  hc_sync_pk_len;
    uint16_t hc_tot_nb_data_pkts;
    uint16_t hc_tot_nb_sync_pkts;
}msg_evt_read_buffer_size_cmd_cmp_t;

// MSG_CMD_LE_READ_CHANNEL_MAP
typedef struct
{
    uint8_t    status;
    uint16_t   conhdl;
    co_chmap_t chmap;
}msg_evt_le_read_channel_map_cmd_cmp_t;

// MSG_CMD_READ_RSSI
typedef struct
{
    uint8_t  status;
    uint16_t conhdl;
    uint8_t  rssi;
}msg_evt_read_rssi_cmd_cmp_t;

// MSG_CMD_READ_TRANSMIT_POWER_LEVEL
typedef struct
{
    uint8_t  status;
    uint16_t conhdl;
    uint8_t  tx_pow_lvl;
}msg_evt_read_transmit_power_level_cmd_cmp_t;

// MSG_CMD_READ_AUTH_PAYLOAD_TIMEOUT
typedef struct
{
    uint8_t  status;
    uint16_t conhdl;
    uint16_t timeout;
}msg_evt_read_auth_payload_timeout_cmd_cmp_t;

// MSG_EVT_CMP_LE_SET_DATA_LEN
typedef struct
{
    uint8_t  status;
    uint16_t conhdl;
}msg_evt_le_set_data_len_cmd_cmp_t;

// MSG_EVT_CMP_LE_READ_SUGG_DEF_DATA_LEN
typedef struct
{
    uint8_t  status;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
}msg_evt_le_read_sugg_def_data_len_cmd_cmp_t;

// MSG_EVT_CMP_LE_WRITE_SUGG_DEF_DATA_LEN
typedef struct
{
    uint8_t status;
}msg_evt_le_write_sugg_def_data_len_cmd_cmp_t;

// MSG_EVT_CMP_LE_ADD_DEVICE_TO_RESOLVE_LIST
typedef struct
{
    uint8_t status;
}msg_evt_le_add_device_to_resolve_list_cmd_cmp_t;

// MSG_EVT_CMP_LE_REMOVE_DEVICE_FROM_RESOLVE_LIST
typedef struct
{
    uint8_t status;
}msg_evt_le_remove_device_from_resolve_list_cmd_cmp_t;

// MSG_EVT_CMP_LE_CLEAR_RESOLVE_LIST
typedef struct
{
    uint8_t status;
}msg_evt_le_clear_resolve_list_cmd_cmp_t;

// MSG_EVT_CMP_LE_READ_RESOLVE_LIST_SIZE
typedef struct
{
    uint8_t status;
    uint8_t size;
}msg_evt_le_read_resolve_list_size_cmd_cmp_t;

// MSG_EVT_CMP_LE_READ_PEER_RESOLVE_ADDR
typedef struct
{
    uint8_t   status;
    bd_addr_t address;
}msg_evt_le_read_peer_resolve_addr_cmd_cmp_t;

// MSG_EVT_CMP_LE_READ_LOCAL_RESOLVE_ADDR
typedef struct
{
    uint8_t   status;
    bd_addr_t address;
}msg_evt_le_read_local_resolve_addr_cmd_cmp_t;

// MSG_EVT_CMP_LE_SET_ADDR_RESOLVE_ENABLE
typedef struct
{
    uint8_t status;
}msg_evt_le_set_addr_resolve_enable_cmd_cmp_t;

// MSG_EVT_CMP_LE_SET_RESOLVE_PRIVATE_ADDR_TIMEOUT
typedef struct
{
    uint8_t status;
}msg_evt_le_set_resolve_private_addr_timeout_cmd_cmp_t;

// MSG_EVT_CMP_LE_READ_MAX_DATA_LEN
typedef struct
{
    uint8_t  status;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
}msg_evt_le_read_max_data_len_cmd_cmp_t;

#ifdef CONFIG_TCI

// MSG_CMD_TT_READ_LINK_INFO
typedef struct
{
    uint16_t conhdl;
}msg_cmd_tt_read_link_info_t;

// MSG_CMD_TT_MOVE_MASTER_ANCHOR
typedef struct
{
    int16_t offset;
}msg_cmd_tt_move_master_anchor_t;


// MSG_CMD_TT_FIX_SLAVE_WIN_EXT
typedef struct
{
    uint16_t win_ext;
}msg_cmd_tt_fix_slave_win_ext_t;

// MSG_CMD_TT_FIX_MASTER_CON_PARAMS
typedef struct
{
    uint16_t winsize;   // Unit:1.25ms
    uint16_t winoffset; // Unit:1.25ms
    uint16_t txoffset;  // Unit:1.25ms, relation to winoffset
}msg_cmd_tt_fix_master_con_params_t;

// MSG_CMD_TT_SET_ADV_PARAM_EX
typedef struct
{
    uint8_t rsv;
}msg_cmd_tt_set_adv_param_ex_t;

// MSG_CMD_TT_SET_SCAN_PARAM_EX
typedef struct
{
    uint8_t   chmap;
    uint8_t   backoff_enable;
    uint8_t   scana_retarget;
    uint8_t   scana_type;
    bd_addr_t scana;
    uint8_t   adva_retarget;
    uint8_t   adva_type;
    bd_addr_t adva;
}msg_cmd_tt_set_scan_param_ex_t;

// MSG_CMD_TT_SET_INIT_PARAM_EX
typedef struct
{
    uint8_t   chmap;
    uint8_t   inita_retarget;
    uint8_t   inita_type;
    bd_addr_t inita;
    uint8_t   adva_retarget;
    uint8_t   adva_type;
    bd_addr_t adva;
}msg_cmd_tt_set_init_param_ex_t;


// MSG_EVT_CMP_TT_READ_LINK_INFO
typedef struct
{
    uint8_t  status;
    uint8_t  alive;
    uint16_t conhdl;
    uint32_t lost_pkts;
    uint32_t crc_err_pkts;
    uint32_t retrans_pkts;
    uint16_t slv_intraslot;
    uint16_t slv_win;
    uint16_t count;
    uint16_t rx_delay;
}msg_evt_tt_read_link_info_cmd_cmp_t;

#endif

// MSG_EVT_COMMAND_COMPLETE Parametter
typedef union
{
    msg_evt_le_rand_cmd_cmp_t                      le_rand;
    msg_evt_le_encrypt_cmd_cmp_t                   le_encrypt;
    msg_evt_le_test_end_cmd_cmp_t                  le_test_end;
    msg_evt_le_read_buffer_size_cmd_cmp_t          le_read_buf_size;
    msg_evt_le_read_local_supp_features_cmd_cmp_t  le_read_local_supp_feat;
    msg_evt_le_read_adv_channel_tx_power_cmd_cmp_t le_read_adv_chnl_tx_power;
    msg_evt_le_read_white_list_size_cmd_cmp_t      le_read_white_list_size;
    msg_evt_le_read_supported_states_cmd_cmp_t     le_read_supp_states;
    msg_evt_le_read_channel_map_cmd_cmp_t          le_read_chnl_map;
    msg_evt_read_rssi_cmd_cmp_t                    read_rssi;
    msg_evt_read_transmit_power_level_cmd_cmp_t    read_trans_power_level;
    msg_evt_read_bd_addr_cmd_cmp_t                 read_bd_addr;
    msg_evt_read_local_ver_info_cmd_cmp_t          read_local_ver_info;
    msg_evt_read_local_supp_commands_cmd_cmp_t     read_local_supp_commands;
    msg_evt_read_local_features_cmd_cmp_t          read_local_features;
    msg_evt_read_buffer_size_cmd_cmp_t             read_buffer_size;
    msg_evt_read_le_host_support_cmd_cmp_t         read_le_host_supp;
#if CONFIG_BLE_VERSION >= BLE_VERSION_4_1
    msg_evt_read_auth_payload_timeout_cmd_cmp_t    read_auth_payload_timeout;
#endif
#if CONFIG_BLE_VERSION >= BLE_VERSION_4_2
    msg_evt_le_read_sugg_def_data_len_cmd_cmp_t    read_sugg_def_data_len;
    msg_evt_le_read_max_data_len_cmd_cmp_t         read_max_data_len;
    msg_evt_le_read_resolve_list_size_cmd_cmp_t    read_resolve_list_size;
    msg_evt_le_read_peer_resolve_addr_cmd_cmp_t    read_peer_resolve_addr;
    msg_evt_le_read_local_resolve_addr_cmd_cmp_t   read_local_resolve_addr;
#endif
#ifdef CONFIG_TCI
    msg_evt_tt_read_link_info_cmd_cmp_t            tt_read_link_info;
#endif

}msg_evt_cmd_cmp_param_t;

// MSG_EVT_COMMAND_COMPLETE
typedef struct
{
    uint8_t  num_cmd_pkt;
    uint16_t opcode;
    msg_evt_cmd_cmp_param_t param;
}msg_evt_command_complete_t;

// MSG_EVT_COMMAND_COMPLETE
typedef struct
{
    uint8_t  num_cmd_pkt;
    uint16_t opcode;
    uint8_t  status;
}msg_evt_command_complete_basic_t;

// MSG_EVT_COMMAND_COMPLETE
typedef struct
{
    uint8_t  num_cmd_pkt;
    uint16_t opcode;
    uint8_t  status;
    uint16_t conhdl;
}msg_evt_command_complete_basic_conhdl_t;

// MSG_EVT_COMMAND_STATUS
typedef struct
{
    uint8_t  status;
    uint8_t  num_cmd_pkt;
    uint16_t opcode;
}msg_evt_command_status_t;

// MSG_EVT_HARDWARE_ERROR
typedef struct
{
    uint8_t err_code;
}msg_evt_hardware_error_t;

// MSG_EVT_DISCONNECTION_COMPLETE
typedef struct
{
    uint8_t  status;
    uint16_t conhdl;
    uint8_t  reason;
}msg_evt_disconnection_complete_t;

// MSG_EVT_NUMBER_OF_COMPLETED_PACKETS
typedef struct
{
    uint8_t  nb_of_hdl;
    uint16_t conhdl[1];
    uint16_t nb_comp_pkt[1];
}msg_evt_number_of_completed_packets_t;

// MSG_EVT_DATA_BUFFER_OVERFLOW
typedef struct
{
    uint8_t link_type;
}msg_evt_data_buffer_overflow_t;

// MSG_EVT_ENCRYPTION_CHANGE
typedef struct
{
    uint8_t  status;
    uint16_t conhdl;
    uint8_t  enc_stat;
}msg_evt_encryption_change_t;

// MSG_EVT_ENCRYPTION_KEY_REFRESH_COMPLETE
typedef struct
{
    uint8_t  status;
    uint16_t conhdl;
}msg_evt_encryption_key_refresh_complete_t;

// MSG_EVT_AUTH_PAYLOAD_TIMEOUT_EXPIRED
typedef struct
{
    uint16_t conhdl;
}msg_evt_auth_payload_timeout_expired_t;

// MSG_EVT_READ_REMOTE_VER_INFO_COMPLETE
typedef struct
{
    uint8_t  status;
    uint16_t conhdl;
    uint8_t  vers;
    uint16_t compid;
    uint16_t subvers;
}msg_evt_read_remote_ver_info_complete_t;

// MSG_EVT_META_LE_CONNECTION_COMPLETE
typedef struct
{
    uint8_t   subevent_code;
    uint8_t   status;
    uint16_t  conhdl;
    uint8_t   role;
    uint8_t   peer_addr_type;
    bd_addr_t peer_addr;
    uint16_t  con_interval;
    uint16_t  con_latency;
    uint16_t  superv_to;
    uint8_t   clk_accuracy;
}msg_evt_meta_le_connection_complete_t;

typedef struct
{
    uint8_t   evt_type;
    uint8_t   adv_addr_type;
    bd_addr_t adv_addr;
    uint8_t   data_len;
    uint8_t   data[MAX_ADV_DATA_LEN];
    uint8_t   rssi;
}adv_report_t;

// MSG_EVT_META_LE_ADV_REPORT
typedef struct
{
    uint8_t      subevent_code;
    uint8_t      nb_reports;
    adv_report_t adv_rep[1];
}msg_evt_meta_le_adv_report_t;

// MSG_EVT_META_LE_CON_UPDATE_COMPLETE
typedef struct
{
    uint8_t  subevent_code;
    uint8_t  status;
    uint16_t conhdl;
    uint16_t con_interval;
    uint16_t con_latency;
    uint16_t superv_to;
}msg_evt_meta_le_con_update_complete_t;

// MSG_EVT_META_LE_READ_REMOTE_USED_FEATURES
typedef struct
{
    uint8_t  subevent_code;
    uint8_t  status;
    uint16_t conhdl;
    co_feats_t feats;
}msg_evt_meta_le_read_remote_used_features_t;

// MSG_EVT_META_LE_LTK_REQ
typedef struct
{
    uint8_t      subevent_code;
    uint16_t     conhdl;
    co_rand_nb_t rand;
    co_ediv_t    ediv;
}msg_evt_meta_le_ltk_req_t;

// MSG_EVT_META_LE_REMOTE_CON_PARAM_REQ
typedef struct
{
    uint8_t  subevent_code;
    uint16_t conhdl;
    uint16_t intv_min;
    uint16_t intv_max;
    uint16_t latency;
    uint16_t superv_to;
}msg_evt_meta_le_remote_con_param_req_t;

// MSG_EVT_META_LE_DATA_LENGTH_CHANGE
typedef struct
{
    uint8_t  subevent_code;
    uint16_t conhdl;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
}msg_evt_meta_le_data_length_change_t;

// MSG_EVT_META_LE_READ_LOCAL_P256_PUBLIC_KEY
typedef struct
{
    uint8_t subevent_code;
    uint8_t status;
    co_p256_public_key_t key;
}msg_evt_meta_le_read_local_p256_public_key_t;

// MSG_EVT_META_LE_GENERATE_DHKEY
typedef struct
{
    uint8_t    subevent_code;
    uint8_t    status;
    co_dhkey_t key;
}msg_evt_meta_le_generate_dhkey_t;

// MSG_EVT_META_LE_ENHANCED_CONNECTION_COMPLETE
typedef struct
{
    uint8_t   subevent_code;
    uint8_t   status;
    uint16_t  conhdl;
    uint8_t   role;
    uint8_t   peer_addr_type;
    bd_addr_t peer_addr;
    bd_addr_t local_resolve_private_addr;
    bd_addr_t peer_resolve_private_addr;
    uint16_t  con_interval;
    uint16_t  con_latency;
    uint16_t  superv_to;
    uint8_t   clk_accuracy;
}msg_evt_meta_le_enhanced_connection_complete_t;

typedef struct
{
    uint8_t   evt_type;
    uint8_t   addr_type;
    bd_addr_t addr;
    uint8_t   direct_addr_type;
    bd_addr_t direct_addr;
    uint8_t   rssi;
}direct_adv_report_t;

// MSG_EVT_META_LE_DIRECT_ADV_REPORT
typedef struct
{
    uint8_t subevent_code;
    uint8_t nb_reports;
    direct_adv_report_t adv_rep[1];
}msg_evt_meta_le_direct_adv_report_t;


/*****************************
 * HCI ACL
 ******************************/
typedef struct
{
    uint16_t conhdl;
    uint16_t length;
    uint8_t  pdata[1];
}msg_acl_t;


/*****************************
 * BB CHANNEL
 ******************************/
typedef struct
{
    uint8_t pdu_type;
    uint8_t rx_add;
    uint8_t tx_add;
    int8_t  rssi;
    uint8_t length;
    uint8_t rlist_entry;
    int16_t freq_offset;
}msg_bb_adv_channel_header_t;

typedef struct
{
    msg_bb_adv_channel_header_t header;
    uint8_t  pdata[4];
}msg_bb_adv_channel_t;

typedef struct
{
    uint8_t  llid;
    uint8_t  length;
    void     *link;
    //uint8_t  padding[3];
}msg_bb_data_channel_header_t;

typedef struct
{
    msg_bb_data_channel_header_t header;
    // Align
    uint8_t  pdata[4];
}msg_bb_data_channel_t;

/*****************************
 * BB DATA CHANNEL CONTROL PDU
 ******************************/

typedef struct
{
    uint8_t opcode;
}bb_ll_base_t;

/// LL_CON_UPDATE_REQ
typedef struct
{
    uint8_t  opcode;
    uint8_t  winsize;
    uint16_t winoffset;
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
    uint16_t instant;
}bb_ll_con_update_req_t;

/// LL_CHANNEL_MAP_REQ
typedef struct
{
    uint8_t  opcode;
    co_chmap_t chmap;
    uint16_t instant;
}bb_ll_channel_map_req_t;

/// LL_TERMINATE_IND
typedef struct
{
    uint8_t opcode;
    uint8_t err_code;
}bb_ll_terminate_ind_t;

/// LL_UNKNOWN_RSP
typedef struct
{
    uint8_t opcode;
    uint8_t unk_type;
}bb_ll_unknown_rsp_t;

/// LL_ENC_REQ
typedef struct
{
    uint8_t      opcode;
    co_rand_nb_t rand;
    co_ediv_t    ediv;
    co_skd_t    skdm;
    co_iv_t     ivm;
}bb_ll_enc_req_t;

/// LL_ENC_RSP
typedef struct
{
    uint8_t   opcode;
    co_skd_t skds;
    co_iv_t  ivs;
}bb_ll_enc_rsp_t;

/// LL_FEATURE_REQ
typedef struct
{
    uint8_t    opcode;
    co_feats_t feats;
}bb_ll_feature_req_t;

/// LL_FEATURE_RSP
typedef struct
{
    uint8_t    opcode;
    co_feats_t feats;
}bb_ll_feature_rsp_t;

/// LL_VERSION_IND
typedef struct
{
    uint8_t  opcode;
    uint8_t  vers;
    uint16_t compid;
    uint16_t subvers;
}bb_ll_version_ind_t;

/// LL_REJECT_IND
typedef struct
{
    uint8_t opcode;
    uint8_t err_code;
}bb_ll_reject_ind_t;

/// LL_SLAVE_FEATURE_REQ
typedef struct
{
    uint8_t    opcode;
    co_feats_t feats;
}bb_ll_slave_feature_req_t;

/// LL_CON_PARAM_REQ
typedef struct
{
    uint8_t opcode;
    uint16_t intv_min;
    uint16_t intv_max;
    uint16_t latency;
    uint16_t superv_to;
    uint8_t  preferred_periodicity;
    uint16_t reference_con_event_count;
    uint16_t offset[6];
}bb_ll_con_param_req_t;

/// LL_CON_PARAM_RSP
typedef bb_ll_con_param_req_t bb_ll_con_param_rsp_t;

// LL_REJECT_IND_EXT
typedef struct
{
    uint8_t opcode;
    uint8_t reject_opcode;
    uint8_t err_code;
}bb_ll_reject_ind_ext_t;

typedef struct
{
    uint8_t opcode;
}bb_ll_start_enc_req_t;

typedef struct
{
    uint8_t opcode;
}bb_ll_start_enc_rsp_t;

typedef struct
{
    uint8_t opcode;
}bb_ll_pause_enc_req_t;

typedef struct
{
    uint8_t opcode;
}bb_ll_pause_enc_rsp_t;

typedef struct
{
    uint8_t opcode;
}bb_ll_ping_req_t;

typedef struct
{
    uint8_t opcode;
}bb_ll_ping_rsp_t;

typedef struct
{
    uint8_t  opcode;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
}bb_ll_length_req_t;

typedef struct
{
    uint8_t  opcode;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
}bb_ll_length_rsp_t;

/// LL_CON_UPDATE_REQ
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_con_update_req_t payload;
}msg_bb_ll_con_update_req_t;

/// LL_CHANNEL_MAP_REQ
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_channel_map_req_t payload;
}msg_bb_ll_channel_map_req_t;

/// LL_TERMINATE_IND
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_terminate_ind_t payload;
}msg_bb_ll_terminate_ind_t;

/// LL_UNKNOWN_RSP
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_unknown_rsp_t payload;
}msg_bb_ll_unknown_rsp_t;

/// LL_ENC_REQ
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_enc_req_t payload;
}msg_bb_ll_enc_req_t;

/// LL_ENC_RSP
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_enc_rsp_t payload;
}msg_bb_ll_enc_rsp_t;

/// LL_FEATURE_REQ
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_feature_req_t payload;
}msg_bb_ll_feature_req_t;

/// LL_FEATURE_RSP
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_feature_rsp_t payload;
}msg_bb_ll_feature_rsp_t;

/// LL_VERSION_IND
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_version_ind_t payload;
}msg_bb_ll_version_ind_t;

/// LL_REJECT_IND
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_reject_ind_t payload;
}msg_bb_ll_reject_ind_t;

/// LL_SLAVE_FEATURE_REQ
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_slave_feature_req_t payload;
}msg_bb_ll_slave_feature_req_t;

/// LL_CON_PARAM_REQ
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_con_param_req_t payload;
}msg_bb_ll_con_param_req_t;

/// LL_CON_PARAM_RSP
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_con_param_rsp_t payload;
}msg_bb_ll_con_param_rsp_t;

// LL_REJECT_IND_EXT
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_reject_ind_ext_t payload;
}msg_bb_ll_reject_ind_ext_t;

typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_start_enc_req_t payload;
}msg_bb_ll_start_enc_req_t;

typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_start_enc_rsp_t payload;
}msg_bb_ll_start_enc_rsp_t;

typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_pause_enc_req_t payload;
}msg_bb_ll_pause_enc_req_t;

typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_pause_enc_rsp_t payload;
}msg_bb_ll_pause_enc_rsp_t;

typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_ping_req_t payload;
}msg_bb_ll_ping_req_t;

typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_ping_rsp_t payload;
}msg_bb_ll_ping_rsp_t;

// MSG_BB_CTRL_LL_LENGTH_REQ
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_length_req_t payload;
}msg_bb_ll_length_req_t;

// MSG_BB_CTRL_LL_LENGTH_RSP
typedef struct
{
    msg_bb_data_channel_header_t header;
    bb_ll_length_rsp_t payload;
}msg_bb_ll_length_rsp_t;

/*****************************
 * BB ADV CHANNEL PDU
 ******************************/

// ADV_IND payload
typedef struct
{
    bd_addr_t adva;
    uint8_t   data[1];
}__bb_adv_ind_t;

typedef struct
{
    bd_addr_t adva;
    uint8_t   data[MAX_ADV_DATA_LEN];
    uint8_t   data_len;
}bb_adv_ind_t;

// ADV_NONCONN_IND payload
typedef bb_adv_ind_t bb_adv_nonconn_ind_t;

// ADV_SCAN_IND payload
typedef bb_adv_ind_t bb_adv_scan_ind_t;

// SCAN_RSP payload
typedef bb_adv_ind_t bb_scan_rsp_t;

// ADV_DIRECT_IND payload
typedef struct
{
    bd_addr_t adva;
    bd_addr_t inita;
}bb_adv_direct_ind_t;

// SCAN_REQ payload
typedef struct
{
    bd_addr_t scana;
    bd_addr_t adva;
}bb_scan_req_t;

// CONNECT_REQ payload
typedef struct
{
    bd_addr_t inita;
    bd_addr_t adva;
    uint8_t   aa[4];
    uint8_t   crcinit[3];
    uint8_t   winsize;
    uint16_t  winoffset;
    uint16_t  interval;
    uint16_t  latency;
    uint16_t  timeout;
    co_chmap_t chmap;
    uint8_t   hop_sca;
}bb_connect_req_t;

// ADV_IND
typedef struct
{
    msg_bb_adv_channel_header_t header;
    __bb_adv_ind_t payload;
}msg_bb_adv_ind_t;

// ADV_NONCONN_IND
typedef msg_bb_adv_ind_t msg_bb_adv_nonconn_ind_t;

// ADV_SCAN_IND
typedef msg_bb_adv_ind_t msg_bb_adv_scan_ind_t;

// SCAN_RSP
typedef msg_bb_adv_ind_t msg_bb_scan_rsp_t;

// ADV_DIRECT_IND
typedef struct
{
    msg_bb_adv_channel_header_t header;
    bb_adv_direct_ind_t payload;
}msg_bb_adv_direct_ind_t;

// SCAN_REQ
typedef struct
{
    msg_bb_adv_channel_header_t header;
    bb_scan_req_t payload;
}msg_bb_scan_req_t;

// CONNECT_REQ
typedef struct
{
    msg_bb_adv_channel_header_t header;
    bb_connect_req_t payload;
}msg_bb_connect_req_t;

// misc
typedef struct
{
    unsigned len;
    uint8_t  data[1];
}msg_debug_receive_t;

// misc
typedef struct
{
    unsigned len;
    uint8_t  data[1];
}msg_vhci_t;

/*********************************************************************
 * EXTERN FUNCTIONS
 */



#ifdef __cplusplus
}
#endif

#endif

