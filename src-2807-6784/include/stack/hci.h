/*********************************************************************
 * @file hci.h
 * @brief 
 * @version 1.0
 * @date Tue 18 Nov 2014 05:06:20 PM CST
 * @author XIHAO
 *
 * @note 
 */

#ifndef __HCI_H__
#define __HCI_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "co.h"
#include "hci_pack.h"
#include "hci_unpack.h"
#include "hci_receive.h"
#include "hci_transmit.h"

/*********************************************************************
 * MACROS
 */
#define HCI_CMD_PKT_NUM  CONFIG_LE_CMD_PKT_NUM
#define HCI_MAX_PKT_LEN  280

#define HCI_CMD_PACKET   0x01
#define HCI_ACL_PACKET   0x02
#define HCI_EVENT_PACKET 0x04

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uint8_t num_cmd_pkt;
}hci_env_t;

/*********************************************************************
 * EXTERN VARIABLES
 */
extern hci_env_t hci_env;

/*********************************************************************
 * EXTERN FUNCTIONS
 */
/**
 * @brief hci_init()
 *
 * @return 
 **/
void hci_init(void);

/**
 * @brief hci_virtual_alloc_send()
 *
 * @param[in] pdata  
 * @param[in] length  
 *
 * @return 
 **/
void hci_virtual_alloc_send(const void *pdata, uint16_t length);

/**
 * @brief hci_align_msg_alloc_param()
 *
 * @param[in] param  
 * @param[in] pkt_type  
 *
 * @return 
 **/
void hci_align_msg_alloc_param(void **param, uint8_t pkt_type);

#ifdef __cplusplus
}
#endif

#endif

