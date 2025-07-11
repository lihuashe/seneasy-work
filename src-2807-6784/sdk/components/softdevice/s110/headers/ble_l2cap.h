
/**
  @addtogroup BLE_L2CAP Logical Link Control and Adaptation Protocol (L2CAP)
  @{
  @brief Definitions and prototypes for the L2CAP interface.
 */

#ifndef BLE_L2CAP_H__
#define BLE_L2CAP_H__ 

#include "ble_types.h"
#include "ble_ranges.h"
#include "ble_err.h"
#include "xh_svc.h"

/**@addtogroup BLE_L2CAP_ENUMERATIONS Enumerations
 * @{ */

/**@brief L2CAP API SVC numbers. */
enum BLE_L2CAP_SVCS 
{
  SD_BLE_L2CAP_CID_REGISTER = BLE_L2CAP_SVC_BASE,  /**< Register a CID. */
  SD_BLE_L2CAP_CID_UNREGISTER,                     /**< Unregister a CID. */
  SD_BLE_L2CAP_TX                                  /**< Transmit a packet. */
};

/**@brief L2CAP Event IDs. */
enum BLE_L2CAP_EVTS 
{
  BLE_L2CAP_EVT_RX  = BLE_L2CAP_EVT_BASE          /**< L2CAP packet received. */
};

/** @} */

/**@addtogroup BLE_L2CAP_DEFINES Defines
 * @{ */

/**@defgroup BLE_ERRORS_L2CAP SVC return values specific to L2CAP
 * @{ */
#define BLE_ERROR_L2CAP_CID_IN_USE            (XH_L2CAP_ERR_BASE + 0x000)  /**< CID already in use. */
/** @} */

/**@brief Default L2CAP MTU. */
#define BLE_L2CAP_MTU_DEF           (23)    

/**@brief Invalid Channel Identifier. */
#define BLE_L2CAP_CID_INVALID       (0x0000) 

/**@brief Dynamic Channel Identifier base. */
#define BLE_L2CAP_CID_DYN_BASE      (0x0040) 

/**@brief Maximum amount of dynamic CIDs. */
#define BLE_L2CAP_CID_DYN_MAX       (8) 

/** @} */

/**@addtogroup BLE_L2CAP_STRUCTURES Structures
 * @{ */

/**@brief Packet header format for L2CAP transmission. */
typedef struct
{
  uint16_t   len;                                 /**< Length of valid info in data member. */
  uint16_t   cid;                                 /**< Channel ID on which packet is transmitted. */
} ble_l2cap_header_t;


/**@brief L2CAP Received packet event report. */
typedef struct
{
  ble_l2cap_header_t header;                      /**< L2CAP packet header. */
  uint8_t    data[1];                             /**< Packet data, variable length. */
} ble_l2cap_evt_rx_t;


/**@brief L2CAP event callback event structure. */
typedef struct
{
  uint16_t conn_handle;                           /**< Connection Handle on which event occured. */
  union
  {
    ble_l2cap_evt_rx_t rx;                        /**< RX Event parameters. */
  } params;                                       /**< Event Parameters. */
} ble_l2cap_evt_t;

/** @} */

/**@addtogroup BLE_L2CAP_FUNCTIONS Functions
 * @{ */

/**@brief Register a CID with L2CAP.
 *
 * @details This registers a higher protocol layer with the L2CAP multiplexer, and is requried prior to all operations on the CID.
 *          
 * @param[in] cid L2CAP CID.
 *
 * @retval ::XH_SUCCESS Successfully registered a CID with the L2CAP layer.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied, CID must be above @ref BLE_L2CAP_CID_DYN_BASE.
 * @retval ::BLE_ERROR_L2CAP_CID_IN_USE L2CAP CID already in use.
 * @retval ::XH_ERROR_NO_MEM Not enough memory to complete operation.
 */
SVCALL(SD_BLE_L2CAP_CID_REGISTER, uint32_t, xh_ble_l2cap_cid_register(uint16_t cid));

/**@brief Unregister a CID with L2CAP.
 *
 * @details This unregisters a previously registerd higher protocol layer with the L2CAP multiplexer.
 *          
 * @param[in] cid L2CAP CID.
 *
 * @retval ::XH_SUCCESS Successfully unregistered the CID.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied.
 * @retval ::XH_ERROR_NOT_FOUND CID not previously registered.
 */
SVCALL(SD_BLE_L2CAP_CID_UNREGISTER, uint32_t, xh_ble_l2cap_cid_unregister(uint16_t cid));

/**@brief Transmit an L2CAP packet.
 *
 * @note    It is important to note that a call to this function will <b>consume an application buffer</b>, and will therefore 
 *          generate a @ref BLE_EVT_TX_COMPLETE event when the packet has been transmitted. 
 *          Please see the documentation of @ref xh_ble_tx_buffer_count_get for more details.
 *
 * @param[in] conn_handle Connection Handle.
 * @param[in] p_header    Pointer to a packet header containing length and CID.
 * @param[in] p_data      Pointer to the data to be transmitted.
 *
 * @retval ::XH_SUCCESS Successfully queued an L2CAP packet for transmission.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied, CIDs must be registered beforehand with @ref xh_ble_l2cap_cid_register.
 * @retval ::XH_ERROR_NOT_FOUND CID not found.
 * @retval ::XH_ERROR_NO_MEM Not enough memory to complete operation.
 * @retval ::BLE_ERROR_NO_TX_BUFFERS Not enough application buffers available.
 * @retval ::XH_ERROR_DATA_SIZE Invalid data size(s) supplied, see @ref BLE_L2CAP_MTU_DEF.
 */
SVCALL(SD_BLE_L2CAP_TX, uint32_t, xh_ble_l2cap_tx(uint16_t conn_handle, ble_l2cap_header_t const *p_header, uint8_t const *p_data));

/** @} */

#endif // BLE_L2CAP_H__

/**
  @}
*/
