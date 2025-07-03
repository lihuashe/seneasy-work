/**
 * @file hci_transmit.h
 * @brief hci transmit
 * @date Thu 30 Jul 2015 02:22:27 PM CST
 * @author XIHAO
 *
 * @defgroup HCI HCI
 * @ingroup BLE
 * @brief Host Controller Interface (HCI, ACI)
 * @details 
 *
 * @{
 */

#ifndef __HCI_TRANSMIT_H__
#define __HCI_TRANSMIT_H__

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

/**
 * @brief HCI transmit callback event
 *
 * @param[in] pdata  transmit data
 * @param[out] pdata  transmited data
 * @param[in] len  transmit data length
 * @param[out] len  transmit data left length
 * @param[in] cmp_cb  complete callabck
 *
 * @return None
 **/
typedef void (*hci_transmit_callback_t)(uint8_t **pdata, unsigned *len, void (*cmp_cb)(void));

/**
 * @brief Wait HCI transmited done event
 *
 * @return None
 **/
typedef void (*hci_transmit_wait_done_callback_t)(void);

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/// @cond
/**
 * @brief hci_transmit_init()
 *
 * @return 
 **/
void hci_transmit_init(void);

/**
 * @brief hci_transmit_msg()
 *
 * @param[in] msg_param  
 *
 * @return 
 **/
void hci_transmit_msg(const void *msg_param);

/**
 * @brief hci_transmit_is_busy()
 *
 * @return 
 **/
bool hci_transmit_is_busy(void);
/// @endcond

/**
 * @brief hci_transmit_flush()
 *
 * @return 
 **/
void hci_transmit_flush(void);

/**
 * @brief Register HCI transmit callback
 *
 * @param[in] transmit_cb  transmit callback
 * @param[in] transmit_wait_done_cb  transmit complete callback
 *
 * @return None
 **/
void hci_transmit_register(hci_transmit_callback_t           transmit_cb,
                           hci_transmit_wait_done_callback_t transmit_wait_done_cb);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

