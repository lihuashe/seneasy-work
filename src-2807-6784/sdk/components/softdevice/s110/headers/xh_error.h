 /**
  @defgroup xh_error SoftDevice Global Error Codes
  @{
   
  @brief Global Error definitions
*/

/* Header guard */
#ifndef XH_ERROR_H__
#define XH_ERROR_H__

/** @defgroup XH_ERRORS_BASE Error Codes Base number definitions
 * @{ */
#define XH_ERROR_BASE_NUM      (0x0)       ///< Global error base
#define XH_ERROR_SDM_BASE_NUM  (0x1000)    ///< SDM error base
#define XH_ERROR_SOC_BASE_NUM  (0x2000)    ///< SoC error base
#define XH_ERROR_STK_BASE_NUM  (0x3000)    ///< STK error base
/** @} */

#define XH_SUCCESS                           (XH_ERROR_BASE_NUM + 0)  ///< Successful command
#define XH_ERROR_SVC_HANDLER_MISSING         (XH_ERROR_BASE_NUM + 1)  ///< SVC handler is missing
#define XH_ERROR_SOFTDEVICE_NOT_ENABLED      (XH_ERROR_BASE_NUM + 2)  ///< SoftDevice has not been enabled
#define XH_ERROR_INTERNAL                    (XH_ERROR_BASE_NUM + 3)  ///< Internal Error
#define XH_ERROR_NO_MEM                      (XH_ERROR_BASE_NUM + 4)  ///< No Memory for operation
#define XH_ERROR_NOT_FOUND                   (XH_ERROR_BASE_NUM + 5)  ///< Not found
#define XH_ERROR_NOT_SUPPORTED               (XH_ERROR_BASE_NUM + 6)  ///< Not supported
#define XH_ERROR_INVALID_PARAM               (XH_ERROR_BASE_NUM + 7)  ///< Invalid Parameter
#define XH_ERROR_INVALID_STATE               (XH_ERROR_BASE_NUM + 8)  ///< Invalid state, operation disallowed in this state
#define XH_ERROR_INVALID_LENGTH              (XH_ERROR_BASE_NUM + 9)  ///< Invalid Length
#define XH_ERROR_INVALID_FLAGS               (XH_ERROR_BASE_NUM + 10) ///< Invalid Flags
#define XH_ERROR_INVALID_DATA                (XH_ERROR_BASE_NUM + 11) ///< Invalid Data
#define XH_ERROR_DATA_SIZE                   (XH_ERROR_BASE_NUM + 12) ///< Data size exceeds limit
#define XH_ERROR_TIMEOUT                     (XH_ERROR_BASE_NUM + 13) ///< Operation timed out
#define XH_ERROR_NULL                        (XH_ERROR_BASE_NUM + 14) ///< Null Pointer
#define XH_ERROR_FORBIDDEN                   (XH_ERROR_BASE_NUM + 15) ///< Forbidden Operation
#define XH_ERROR_INVALID_ADDR                (XH_ERROR_BASE_NUM + 16) ///< Bad Memory Address
#define XH_ERROR_BUSY                        (XH_ERROR_BASE_NUM + 17) ///< Busy

#endif // XH_ERROR_H__

/**
  @}
*/
