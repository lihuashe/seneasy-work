/**
 * @file hci_receive.h
 * @brief hci transmit
 * @date Thu 30 Jul 2015 02:22:27 PM CST
 * @author XIHAO
 *
 * @addtogroup HCI
 *
 * @{
 */

#ifndef __HCI_RECEIVE_H__
#define __HCI_RECEIVE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */


/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */
/// @cond
/**
 * @brief hci_receive_init()
 *
 * @return 
 **/
void hci_receive_init(void);
/// @endcond

/**
 * @brief  hci receive set fifo pool
 *
 * @param[in] pool  pool
 * @param[in] pool_size  pool size
 **/
void hci_receive_fifo_pool_set(uint8_t *pool, uint32_t pool_size);

/**
 * @brief  hci receive set msg buffer
 *
 * @param[in] buffer  buffer
 * @param[in] buffer_size  buffer size
 **/
void hci_receive_msg_buffer_set(uint8_t *buffer, uint32_t buffer_size);

/**
 * @brief HCI receive data headler, Can be as callback with uart_init()
 *
 * @param[in] data  received one data
 *
 * @return None
 **/
void hci_receive_handler(uint8_t data);

/// @cond
/**
 * @brief hci_receive_is_busy()
 *
 * @return 
 **/
bool hci_receive_is_busy(void);
/// @endcond

#ifdef __cplusplus
}
#endif

#endif

/** @} */

