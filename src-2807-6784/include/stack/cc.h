/*********************************************************************
 * @file cc.h
 * @brief 
 * @version 1.0
 * @date Mon 22 Dec 2014 03:13:07 PM CST
 * @author XIHAO
 *
 * @note 
 *
 *
 * About RF Quality Register:
 *
 * 1. WIN_EXT => For Window Search (Search Access Address):
 *     These bits will define how far the receive window is extended beyond the normal +/- 10 us. The
 * window is defined to be of width +/- 10 μs for WIN_EXT=0 and for a duration starting from -(10+WIN_EXT) us
 * to (10+WIN_EXT) us.
 *     Setting WIN_EXT to a non-zero value increases the likelihood of a successful Sync Word
 * correlation, and thereby improving the probability of a packet being received.
 *     When enabled by the State Machine, the Window generator will generate a 22 μs window around
 * the expected position of a first receive packet channel access code. This window will be used to enable
 * Tabasco DSP to check for the sync word in the receive bitstream.
 *     The position of the 1st window is set by the start of the receive slot which is controlled by software.
 * For subsequent receive windows the T_IFS counter is used to count from the last transmitted data bit as
 * specified in the Bluetooth LE specifications. This counter is controlled by the T_IFS_DEFAULT (§16.2.5 on
 * page 234) and the T_IFS_ABORT (§16.2.6 on page 235) registers.
 *     The channel access address receive window will be expanded to a width 21 + 2 * WIN_EXT.
 *     The extension of the search window is performed on BOTH sides of the window. Hence, if the
 * window is normally active from time instants 83 to 103, then if the WIN_EXT register is 20, the window is active
 * from (83-20 =) 63 to (103+20 =) 123.
 *
 * 2. DELAY_SRCH_WIN => Search Window Delay
 *     The contents of this register can be used to account for delays within the data stream path. The
 * receive search window will be delayed by the DELAY_SRCH_WIN[5:0].
 *     The DELAY_SRCH_WIN registers can be used to add a delay to the search window, which is
 * searching for the access address.
 *
 * 3. SYNC_ERROR => Access Address Tolerated Error Bit Number
 *     This 6-bit register contains the number of allowable symbol errors tolerated while searching for a sync
 * word correlation. Once the correlation sum exceeds the difference between the maximum sum and the error
 * sum, a candidate sync word is detected. This candidate will be qualified by a window flag from the link controller.
 *     In general some overhead is programmed to allow for some errors in reception of the sync word.
 * Typically 7 symbol errors is a reasonable value as this is midway between perfect correlation (0 errors) and
 * correlation of an incorrect sync word (14 errors).
 *
 * 4. T_IFS_DELAY => 150us TIFS Delay
 *     The contents of this register will be used to account for delays within the data stream path. The
 * transmit state machine will start to transmit when the T_IFS counter = 150 - TIFS_DELAY[5:0].
 *     The T_IFS_DELAY registers can be adjusted to achieve the 150us IFS specification.
 *
 */

#ifndef __CC_H__
#define __CC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "co.h"
//#include "ll_def.h"
//#include "controller_features.h"

/*********************************************************************
 * MACROS
 */

// LL supported features
#define LL_ENC_FEATURE                     0x01
#define LL_CON_PARAM_REQ_FEATURE           0x02
#define LL_EXT_REJ_IND_FEATURE             0x04
#define LL_SLV_INI_FEAT_REQ_FEATURE        0x08
#define LL_PING_FEATURE                    0x10
#define LL_DATA_PKT_LEN_EXT_FEATURE        0x20
#define LL_PRIVACY_FEATURE                 0x40
#define LL_EXT_SCAN_FILTER_POLICY_FEATURE  0x80

/// LL supported states
//byte 0
#define LL_NON_CON_ADV_STATE               0x01
#define LL_DISC_ADV_STATE                  0x02
#define LL_CON_ADV_STATE                   0x04
#define LL_DIRECT_ADV_STATE                0x08
#define LL_PASS_SCAN_STATE                 0x10
#define LL_ACTIV_SCAN_STATE                0x20
#define LL_INIT_MASTER_STATE               0x40
#define LL_CON_SLAVE_STATE                 0x80
//byte 1
#define LL_NON_CON_ADV_PASS_SCAN_STATE     0x01
#define LL_DISC_ADV_PASS_SCAN_STATE        0x02
#define LL_CON_ADV_PASS_SCAN_STATE         0x04
#define LL_DIRECT_ADV_PASS_SCAN_STATE      0x08
#define LL_NON_CON_ADV_ACTIV_SCAN_STATE    0x10
#define LL_DISC_ADV_ACTIV_SCAN_STATE       0x20
#define LL_CON_ADV_ACTIV_SCAN_STATE        0x40
#define LL_DIRECT_ADV_ACTIV_SCAN_STATE     0x80
//byte 2
#define LL_NON_CON_ADV_INIT_STATE          0x01
#define LL_DISC_ADV_INIT_STATE             0x02
#define LL_NON_CON_ADV_MASTER_STATE        0x04
#define LL_DISC_ADV_MASTER_STATE           0x08
#define LL_NON_CON_ADV_SLAVE_STATE         0x10
#define LL_DISC_ADV_SLAVE_STATE            0x20
#define LL_PASS_SCAN_INIT_STATE            0x40
#define LL_ACTIV_SCAN_INIT_STATE           0x80
//byte 3
#define LL_PASS_SCAN_MASTER_STATE          0x01
#define LL_ACTIV_SCAN_MASTER_STATE         0x02
#define LL_PASS_SCAN_SLAVE_STATE           0x04
#define LL_ACTIV_SCAN_SLAVE_STATE          0x08
#define LL_INIT_MASTER_MASTER_STATE        0x10
#define LL_LOW_DUTY_CYCLE_DIR_ADV_STATE    0x20
#define LL_LOW_DUTY_CYCLE_DIR_ADV_PASS_SCAN_STATE  0x40
#define LL_LOW_DUTY_CYCLE_DIR_ADV_ACT_SCAN_STATE   0x80
//byte 4
#define LL_CNX_ADV_INIT_STATE              0x01
#define LL_HIGH_DUTY_CYCLE_INIT_STATE      0x02
#define LL_LOW_DUTY_CYCLE_INIT_STATE       0x04
#define LL_CNX_ADV_MASTER_STATE            0x08
#define LL_HIGH_DUTY_CYCLE_MASTER_STATE    0x10
#define LL_LOW_DUTY_CYCLE_MASTER_STATE     0x20
#define LL_CNX_ADV_SLAVE_STATE             0x40
#define LL_HIGH_DUTY_CYCLE_SLAVE_STATE     0x80
//byte 5
#define LL_LOW_DUTY_CYCLE_SLAVE_STATE      0x01
#define LL_INIT_SLAVE_STATE                0x02

/// LL supported commands
//byte0
#define LL_DISC_CMD                0x20
//byte2
#define LL_RD_REM_VERS_CMD         0x80
//byte5
#define LL_SET_EVT_MSK_CMD         0x40
#define LL_RESET_CMD               0x80
//byte10
#define LL_HL_BUF_SIZE_CMD         0x40
#define LL_RD_TX_PWR_CMD           0x04
#define LL_HL_NB_CMP_PKT_CMD       0x80
//byte14
#define LL_RD_LOC_VERS_CMD         0x08
#define LL_RD_LOC_SUP_FEAT_CMD     0x20
#define LL_RD_BUF_SIZE_CMD         0x80
//byte15
#define LL_RD_BD_ADDR_CMD          0x02
#define LL_RD_RSSI_CMD             0x20
//byte25
#define LL_LE_SET_EVT_MSK_CMD      0x01
#define LL_LE_RD_BUF_SIZE_CMD      0x02
#define LL_LE_RD_LOC_SUP_FEAT_CMD  0x04
#define LL_LE_SET_RAND_ADDR_CMD    0x10
#define LL_LE_SET_ADV_PARAM_CM     0x20
#define LL_LE_RD_ADV_TX_PWR_CMD    0x40
#define LL_LE_SET_ADV_DATA_CMD     0x80
//byte26
#define LL_LE_SET_SC_RSP_DATA_CMD  0x01
#define LL_LE_SET_ADV_EN_CMD       0x02
#define LL_LE_SET_SC_PARAM_CMD     0x04
#define LL_LE_SET_SC_EN_CMD        0x08
#define LL_LE_CREAT_CNX_CMD        0x10
#define LL_LE_CREAT_CNX_CNL_CMD    0x20
#define LL_LE_RD_WL_SIZE_CMD       0x40
#define LL_LE_CLEAR_WL_CMD         0x80
//byte27
#define LL_LE_ADD_DEV_WL_CMD       0x01
#define LL_LE_REM_DEV_WL_CMD       0x02
#define LL_LE_CNX_UPDATE_CMD       0x04
#define LL_LE_SET_HL_CH_CLASS_CMD  0x08
#define LL_LE_RD_CH_MAP_CMD        0x10
#define LL_LE_RD_REM_USED_FEAT_CMD 0x20
#define LL_LE_ENCRYPT_CMD          0x40
#define LL_LE_RAND_CMD             0x80
//byte28
#define LL_LE_START_ENC_CMD        0x01
#define LL_LE_LTK_REQ_RPLY_CMD     0x02
#define LL_LE_LTK_REQ_NEG_RPLY_CMD 0x04
#define LL_LE_RD_SUPP_STATES_CMD   0x08
#define LL_LE_RX_TEST_CMD          0x10
#define LL_LE_TX_TEST_CMD          0x20
#define LL_LE_STOP_TEST_CMD        0x40
//byte32
#define LL_RD_AUTH_PALY_TO_CMD                 0x10
#define LL_WR_AUTH_PALY_TO_CMD                 0x20
//byte33
#define LL_LE_REM_CNX_PARAM_REQ_RPLY_CMD       0x10
#define LL_LE_REM_CNX_PARAM_REQ_NEG_RPLY_CMD   0x20

/// Features byte 0
#ifdef CONFIG_LL_ENC_FEATURE
#define __LL_ENC_FEATURE LL_ENC_FEATURE
#else
#define __LL_ENC_FEATURE 0
#endif
#ifdef CONFIG_LL_CON_PARAM_REQ_FEATURE
#define __LL_CON_PARAM_REQ_FEATURE LL_CON_PARAM_REQ_FEATURE
#else
#define __LL_CON_PARAM_REQ_FEATURE 0
#endif
#ifdef CONFIG_LL_EXT_REJ_IND_FEATURE
#define __LL_EXT_REJ_IND_FEATURE LL_EXT_REJ_IND_FEATURE
#else
#define __LL_EXT_REJ_IND_FEATURE 0
#endif
#ifdef CONFIG_LL_SLV_INI_FEAT_REQ_FEATURE
#define __LL_SLV_INI_FEAT_REQ_FEATURE LL_SLV_INI_FEAT_REQ_FEATURE
#else
#define __LL_SLV_INI_FEAT_REQ_FEATURE 0
#endif
#ifdef CONFIG_LL_PING_FEATURE
#define __LL_PING_FEATURE LL_PING_FEATURE
#else
#define __LL_PING_FEATURE 0
#endif
#ifdef CONFIG_LL_DATA_PKT_LEN_EXT_FEATURE
#define __LL_DATA_PKT_LEN_EXT_FEATURE LL_DATA_PKT_LEN_EXT_FEATURE
#else
#define __LL_DATA_PKT_LEN_EXT_FEATURE 0
#endif
#ifdef CONFIG_LL_PRIVACY_FEATURE
#define __LL_PRIVACY_FEATURE LL_PRIVACY_FEATURE
#else
#define __LL_PRIVACY_FEATURE 0
#endif
#define __LL_EXT_SCAN_FILTER_POLICY_FEATURE 0

#define LL_FEATURES_BYTE0  __LL_ENC_FEATURE

#if CONFIG_BLE_VERSION >= BLE_VERSION_4_1
#undef LL_FEATURES_BYTE0
/// Features byte 0
#define LL_FEATURES_BYTE0  ( __LL_ENC_FEATURE \
                           | __LL_CON_PARAM_REQ_FEATURE \
                           | __LL_EXT_REJ_IND_FEATURE \
                           | __LL_SLV_INI_FEAT_REQ_FEATURE \
                           | __LL_PING_FEATURE)
#endif

#if CONFIG_BLE_VERSION >= BLE_VERSION_4_2
#undef LL_FEATURES_BYTE0
/// Features byte 0
#define LL_FEATURES_BYTE0  ( __LL_ENC_FEATURE \
                           | __LL_CON_PARAM_REQ_FEATURE \
                           | __LL_EXT_REJ_IND_FEATURE \
                           | __LL_SLV_INI_FEAT_REQ_FEATURE \
                           | __LL_PING_FEATURE \
                           | __LL_DATA_PKT_LEN_EXT_FEATURE \
                           | __LL_PRIVACY_FEATURE \
                           | __LL_EXT_SCAN_FILTER_POLICY_FEATURE)
#endif

/// Features byte 1
#define LL_FEATURES_BYTE1  0x00
/// Features byte 2
#define LL_FEATURES_BYTE2  0x00
/// Features byte 3
#define LL_FEATURES_BYTE3  0x00
/// Features byte 4
#define LL_FEATURES_BYTE4  0x00
/// Features byte 5
#define LL_FEATURES_BYTE5  0x00
/// Features byte 6
#define LL_FEATURES_BYTE6  0x00
/// Features byte 7
#define LL_FEATURES_BYTE7  0x00

#if CONFIG_BLE_VERSION >= BLE_VERSION_4_1
/// States byte 0
#define LL_STATES_BYTE0    ( LL_NON_CON_ADV_STATE | LL_DISC_ADV_STATE\
                            | LL_CON_ADV_STATE | LL_DIRECT_ADV_STATE\
                            | LL_PASS_SCAN_STATE | LL_ACTIV_SCAN_STATE\
                            | LL_INIT_MASTER_STATE | LL_CON_SLAVE_STATE)
/// States byte 1
#define LL_STATES_BYTE1    0x00
/// States byte 2
#define LL_STATES_BYTE2    ( LL_NON_CON_ADV_INIT_STATE   | LL_DISC_ADV_INIT_STATE   \
                           | LL_NON_CON_ADV_MASTER_STATE | LL_DISC_ADV_MASTER_STATE \
                           | LL_NON_CON_ADV_SLAVE_STATE  | LL_DISC_ADV_SLAVE_STATE  \
                           | LL_PASS_SCAN_INIT_STATE     | LL_ACTIV_SCAN_INIT_STATE)
/// States byte 3
#define LL_STATES_BYTE3    ( LL_PASS_SCAN_MASTER_STATE | LL_ACTIV_SCAN_MASTER_STATE\
                            | LL_PASS_SCAN_SLAVE_STATE | LL_ACTIV_SCAN_SLAVE_STATE\
                            | LL_INIT_MASTER_MASTER_STATE |LL_LOW_DUTY_CYCLE_DIR_ADV_STATE\
                            | LL_LOW_DUTY_CYCLE_DIR_ADV_PASS_SCAN_STATE |LL_LOW_DUTY_CYCLE_DIR_ADV_ACT_SCAN_STATE)
/// States byte 4
#define LL_STATES_BYTE4    ( LL_CNX_ADV_INIT_STATE | LL_HIGH_DUTY_CYCLE_INIT_STATE\
                            | LL_LOW_DUTY_CYCLE_INIT_STATE | LL_CNX_ADV_MASTER_STATE\
                            | LL_HIGH_DUTY_CYCLE_MASTER_STATE |LL_LOW_DUTY_CYCLE_MASTER_STATE\
                            | LL_CNX_ADV_SLAVE_STATE |LL_HIGH_DUTY_CYCLE_SLAVE_STATE)
/// States byte 5
#define LL_STATES_BYTE5    ( LL_LOW_DUTY_CYCLE_SLAVE_STATE | LL_INIT_SLAVE_STATE)
/// States byte 6
#define LL_STATES_BYTE6    0x0
/// States byte 7
#define LL_STATES_BYTE7    0x0
#else
/// States byte 0
#define LL_STATES_BYTE0    ( LL_NON_CON_ADV_STATE | LL_DISC_ADV_STATE\
                            | LL_CON_ADV_STATE | LL_DIRECT_ADV_STATE\
                            | LL_PASS_SCAN_STATE | LL_ACTIV_SCAN_STATE\
                            | LL_INIT_MASTER_STATE | LL_CON_SLAVE_STATE)
/// States byte 1
#define LL_STATES_BYTE1    0x00
/// States byte 2
#define LL_STATES_BYTE2    ( LL_NON_CON_ADV_MASTER_STATE | LL_DISC_ADV_MASTER_STATE\
                            | LL_NON_CON_ADV_SLAVE_STATE | LL_DISC_ADV_SLAVE_STATE)
/// States byte 3
#define LL_STATES_BYTE3    ( LL_PASS_SCAN_MASTER_STATE | LL_ACTIV_SCAN_MASTER_STATE\
                            | LL_PASS_SCAN_SLAVE_STATE | LL_ACTIV_SCAN_SLAVE_STATE\
                            | LL_INIT_MASTER_MASTER_STATE)
/// States byte 4
#define LL_STATES_BYTE4    0x0
/// States byte 5
#define LL_STATES_BYTE5    0x0
/// States byte 6
#define LL_STATES_BYTE6    0x0
/// States byte 7
#define LL_STATES_BYTE7    0x0
#endif

/// LL supported commands
//byte1
#define LL_CMDS_BYTE0      LL_DISC_CMD
//byte2
#define LL_CMDS_BYTE2      LL_RD_REM_VERS_CMD
//byte5
#define LL_CMDS_BYTE5      (LL_SET_EVT_MSK_CMD | LL_RESET_CMD)
//byte10
#define LL_CMDS_BYTE10     LL_HL_NB_CMP_PKT_CMD | LL_RD_TX_PWR_CMD
//byte14
#define LL_CMDS_BYTE14     (LL_RD_LOC_VERS_CMD | LL_RD_LOC_SUP_FEAT_CMD\
                            |LL_RD_BUF_SIZE_CMD)
//byte15
#define LL_CMDS_BYTE15     (LL_RD_BD_ADDR_CMD | LL_RD_RSSI_CMD)
//byte25
#define LL_CMDS_BYTE25     (LL_LE_SET_EVT_MSK_CMD | LL_LE_RD_BUF_SIZE_CMD\
                            |LL_LE_RD_LOC_SUP_FEAT_CMD | LL_LE_SET_RAND_ADDR_CMD\
                            |LL_LE_SET_ADV_PARAM_CM | LL_LE_RD_ADV_TX_PWR_CMD\
                            |LL_LE_SET_ADV_DATA_CMD)
//byte26
#define LL_CMDS_BYTE26     (LL_LE_SET_SC_RSP_DATA_CMD | LL_LE_SET_ADV_EN_CMD\
                            |LL_LE_SET_SC_PARAM_CMD | LL_LE_SET_SC_EN_CMD\
                            |LL_LE_CREAT_CNX_CMD | LL_LE_CREAT_CNX_CNL_CMD\
                            |LL_LE_RD_WL_SIZE_CMD | LL_LE_CLEAR_WL_CMD)
//byte27
#define LL_CMDS_BYTE27     (LL_LE_ADD_DEV_WL_CMD | LL_LE_REM_DEV_WL_CMD\
                            |LL_LE_CNX_UPDATE_CMD | LL_LE_SET_HL_CH_CLASS_CMD\
                            |LL_LE_RD_CH_MAP_CMD | LL_LE_RD_REM_USED_FEAT_CMD\
                            |LL_LE_ENCRYPT_CMD | LL_LE_RAND_CMD)
//byte28
#define LL_CMDS_BYTE28     (LL_LE_START_ENC_CMD | LL_LE_LTK_REQ_RPLY_CMD\
                            |LL_LE_LTK_REQ_NEG_RPLY_CMD | LL_LE_RD_SUPP_STATES_CMD\
                            |LL_LE_RX_TEST_CMD | LL_LE_TX_TEST_CMD\
                            |LL_LE_STOP_TEST_CMD)

#if CONFIG_BLE_VERSION >= BLE_VERSION_4_2
//byte32
#define LL_CMDS_BYTE32     (LL_RD_AUTH_PALY_TO_CMD | LL_WR_AUTH_PALY_TO_CMD)
//byte33
#define LL_CMDS_BYTE33     0xF0
//byte34
#define LL_CMDS_BYTE34     0xFF
//byte35
#define LL_CMDS_BYTE35     0x0F
#elif CONFIG_BLE_VERSION >= BLE_VERSION_4_1
//byte32
#define LL_CMDS_BYTE32     (LL_RD_AUTH_PALY_TO_CMD | LL_WR_AUTH_PALY_TO_CMD)
//byte33
#define LL_CMDS_BYTE33     (LL_LE_REM_CNX_PARAM_REQ_RPLY_CMD | LL_LE_REM_CNX_PARAM_REQ_NEG_RPLY_CMD)
//byte34
#define LL_CMDS_BYTE34     0
//byte35
#define LL_CMDS_BYTE35     0
#else
//byte32
#define LL_CMDS_BYTE32     0
//byte33
#define LL_CMDS_BYTE33     0
//byte34
#define LL_CMDS_BYTE34     0
//byte35
#define LL_CMDS_BYTE35     0
#endif



/*********************************************************************
 * TYPEDEFS
 */

typedef enum
{
    CC_SCA_500PPM,
    CC_SCA_250PPM,
    CC_SCA_150PPM,
    CC_SCA_100PPM,
    CC_SCA_75PPM,
    CC_SCA_50PPM,
    CC_SCA_30PPM,
    CC_SCA_20PPM,
    CC_SCA_700PPM,   // Only valid in slave
    CC_SCA_1000PPM,  // Only valid in slave
    CC_SCA_2000PPM,  // Only valid in slave
    CC_SCA_3000PPM,  // Only valid in slave
    CC_SCA_4000PPM,  // Only valid in slave
    CC_SCA_5000PPM,  // Only valid in slave
    CC_SCA_7000PPM,  // Only valid in slave
    CC_SCA_10000PPM, // Only valid in slave
    CC_SCA_20000PPM, // Only valid in slave
    CC_SCA_NUM,
}cc_sca_t;

typedef struct
{
    bd_addr_t public_bdaddr;

    uint8_t  sca;

    uint8_t  max_links_num;

    uint32_t random_seed;

    uint32_t min_sleep_space;

    uint8_t earlier_wakeup_time;

#ifdef CONFIG_HCI_CMD_READ_TRANSMIT_POWER_LEVEL
    int8_t tx_power_max;
    int8_t tx_power;
    int8_t rx_gain;
#endif

#ifdef CONFIG_HCI_CMD_READ_LOCAL_SUPP_COMMANDS
    const uint8_t *supp_cmds;
#endif
#ifdef CONFIG_HCI_CMD_LE_READ_SUPPORTED_STATES
    const uint8_t *supp_states;
#endif
    const uint8_t *supp_features;

    uint8_t  version;
    uint16_t company_id;
    uint16_t sub_version;
    uint8_t  hci_version;
    uint16_t hci_reversion;

    uint16_t acl_pkt_q_len;
    uint8_t  acl_pkt_q_num;

#if (CONFIG_BLE_VERSION >= BLE_VERSION_4_2) && defined(CONFIG_LL_DATA_PKT_LEN_EXT_FEATURE) && defined(CONFIG_LL_DATA_PKT_LEN_EXT_LOCAL_DYNAMIC)
    // Valid ranges for data PDU length management parameters (core_v42 P2628)
    // +-----------------------------+-----------------------+-----------------------+
    // |                             | Parameters with names | Parameters with names |
    // | LE Data Packet Length       | ending in "Octets"    | ending in "Time"      |
    // | Extension feature supported +-----------+-----------+-----------+-----------+
    // |                             |  Minimum  |  Maximum  |  Minimum  |  Maximum  |
    // +-----------------------------+-----------+-----------+-----------+-----------+
    // |             No              |  27       |  27       |  328      |  328      |
    // |             Yes             |  27       |  251      |  328      |  2120     |
    // +-----------------------------+-----------+-----------+-----------+-----------+
    // 7.8.46 LE Read Maximum Data Length Command
    ll_pdu_length_t supported_pdu_length;
#endif

    uint16_t link_ce;
}cc_t;

typedef void (*cc_callback_t)(cc_t *cc);

/*********************************************************************
 * EXTERN VARIABLES
 */
extern const cc_t * const cc;

/*********************************************************************
 * EXTERN FUNCTIONS
 */
/**
 * @brief cc_init()
 *
 * @return 
 **/
void cc_init(cc_callback_t cb);

/**
 * @brief cc_reset()
 *
 * @return 
 **/
void cc_reset(void);

/**
 * @brief Set public address
 *
 * @param[in] bdaddr  public address
 *
 * @return None
 *
 * @note When called before stack init, the address is default value. the config value may overwrite it.
 *       When called after stack init, the address is final value.
 **/
void cc_public_address_set(const bd_addr_t bdaddr);

/**
 * @brief Get public address get
 *
 * @param[out] bdaddr  public address
 *
 * @return None
 **/
void cc_public_address_get(bd_addr_t bdaddr);

/**
 * @brief Is public address valid
 *
 * @return valid ?
 **/
bool cc_public_address_is_valid(void);

/**
 * @brief Set SCA (Sleep Slock Accuracy)
 *
 * @param[in] sca  SCA
 *
 * @return None
 *
 * @note When called before stack init, the SCA is default value. the config value may overwrite it.
 *       When called after stack init, the SCA is final value.
 **/
void cc_sca_set(cc_sca_t sca);

/**
 * @brief set max links number
 *
 * @param[in] num  max links number
 *
 * @return None
 *
 * @note Must be called before stack init
 **/
void cc_max_links_num_set(uint8_t num);

/**
 * @brief Set ACL packet queue
 *
 * @param[in] num  ACL packet number
 * @param[in] length  each ACL packet length
 *
 * @return None
 *
 * @note Must be called before stack init
 **/
void cc_acl_pkt_q_set(uint16_t num, uint16_t length);

/**
 * @brief Set early wakeup time
 *
 * @param[in] time  unit:312.5us
 *
 * @return None
 *
 * @note some xtal24m fast-setup is slow, use this function to let wakeup earlier.
 **/
void cc_pre_wakeup_time_set(uint8_t time);

/**
 * @brief cc validate the configuration
 *
 * @return None
 **/
void cc_configuration_validate(void);

#ifdef __cplusplus
}
#endif

#endif

