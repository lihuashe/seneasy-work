
/**
  @addtogroup BLE_COMMON
  @{
  @addtogroup  xh_error
  @{
    @ingroup BLE_COMMON
  @}

  @defgroup ble_err General error codes
  @{

  @brief General error code definitions for the BLE API.

  @ingroup BLE_COMMON
*/
#ifndef XH_BLE_ERR_H__
#define XH_BLE_ERR_H__

#include "xh_error.h"

/* @defgroup BLE_ERRORS Error Codes
 * @{ */
#define BLE_ERROR_NOT_ENABLED            (XH_ERROR_STK_BASE_NUM+0x001) /**< @ref xh_ble_enable has not been called. */
#define BLE_ERROR_INVALID_CONN_HANDLE    (XH_ERROR_STK_BASE_NUM+0x002) /**< Invalid connection handle. */
#define BLE_ERROR_INVALID_ATTR_HANDLE    (XH_ERROR_STK_BASE_NUM+0x003) /**< Invalid attribute handle. */
#define BLE_ERROR_NO_TX_BUFFERS          (XH_ERROR_STK_BASE_NUM+0x004) /**< Buffer capacity exceeded. */
#define BLE_ERROR_INVALID_ROLE           (XH_ERROR_STK_BASE_NUM+0x005) /**< Invalid role. */
/** @} */


/** @defgroup BLE_ERROR_SUBRANGES Module specific error code subranges
 *  @brief Assignment of subranges for module specific error codes.
 *  @note For specific error codes, see ble_<module>.h or ble_error_<module>.h.
 * @{ */
#define XH_L2CAP_ERR_BASE             (XH_ERROR_STK_BASE_NUM+0x100) /**< L2CAP specific errors. */
#define XH_GAP_ERR_BASE               (XH_ERROR_STK_BASE_NUM+0x200) /**< GAP specific errors. */
#define XH_GATTC_ERR_BASE             (XH_ERROR_STK_BASE_NUM+0x300) /**< GATT client specific errors. */
#define XH_GATTS_ERR_BASE             (XH_ERROR_STK_BASE_NUM+0x400) /**< GATT server specific errors. */
/** @} */

#endif


/** @} */
