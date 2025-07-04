
/**
  @addtogroup BLE_API BLE SoftDevice Common
  @{
  @defgroup ble_api Events, type definitions and API calls
  @{

  @brief Module independent events, type definitions and API calls for the BLE SoftDevice.

 */

#ifndef BLE_H__
#define BLE_H__

#include "xh98xx_config.h"
#include "ble_ranges.h"
#include "ble_types.h"
#include "xh_svc.h"
#include "ble_gap.h"
#include "ble_l2cap.h"
#include "ble_gatt.h"
#include "ble_gattc.h"
#include "ble_gatts.h"

/** @addtogroup BLE_COMMON_ENUMERATIONS Enumerations
 * @{ */

/**
 * @brief Common API SVC numbers.
 */
enum BLE_COMMON_SVCS
{
  SD_BLE_ENABLE = BLE_SVC_BASE,         /**< Enable and initialize the BLE stack */
  SD_BLE_EVT_GET,                       /**< Get an event from the pending events queue. */
  SD_BLE_TX_BUFFER_COUNT_GET,           /**< Get the total number of available application transmission buffers from the BLE stack. */
  SD_BLE_UUID_VS_ADD,                   /**< Add a Vendor Specific UUID. */
  SD_BLE_UUID_DECODE,                   /**< Decode UUID bytes. */
  SD_BLE_UUID_ENCODE,                   /**< Encode UUID bytes. */
  SD_BLE_VERSION_GET,                   /**< Get the local version information (company id, Link Layer Version, Link Layer Subversion). */
  SD_BLE_USER_MEM_REPLY,                /**< User Memory Reply. */
  SD_BLE_OPT_SET,                       /**< Set a BLE option. */
  SD_BLE_OPT_GET,                       /**< Get a BLE option. */
};

/**
 * @brief BLE Module Independent Event IDs.
 */
enum BLE_COMMON_EVTS
{
  BLE_EVT_TX_COMPLETE  = BLE_EVT_BASE,  /**< Transmission Complete. @ref ble_evt_tx_complete_t */
  BLE_EVT_USER_MEM_REQUEST,             /**< User Memory request. @ref ble_evt_user_mem_request_t */
  BLE_EVT_USER_MEM_RELEASE              /**< User Memory release. @ref ble_evt_user_mem_release_t */
};

/**@brief Common Option IDs.
 * IDs that uniquely identify a common option.
 */
enum BLE_COMMON_OPTS
{
  BLE_COMMON_OPT_RADIO_CPU_MUTEX = BLE_OPT_BASE    /**< Radio CPU mutex option. @ref ble_common_opt_radio_cpu_mutex_t */
};
/** @} */

/** @addtogroup BLE_COMMON_DEFINES Defines
 * @{ */

/** @brief  Required pointer alignment for BLE Events.
*/
#define BLE_EVTS_PTR_ALIGNMENT    4

/** @defgroup BLE_USER_MEM_TYPES User Memory Types
 * @{ */
#define BLE_USER_MEM_TYPE_INVALID               0x00  /**< Invalid User Memory Types. */
#define BLE_USER_MEM_TYPE_GATTS_QUEUED_WRITES   0x01  /**< User Memory for GATTS queued writes. */
/** @} */

/** @brief  Maximum number of Vendor Specific UUIDs.
*/
#define BLE_UUID_VS_MAX_COUNT     10

/** @} */

/** @addtogroup BLE_COMMON_STRUCTURES Structures
 * @{ */

/**@brief User Memory Block. */
typedef struct
{
  uint8_t          *p_mem;      /**< Pointer to the start of the user memory block. */
  uint16_t          len;        /**< Length in bytes of the user memory block. */
} ble_user_mem_block_t;

/**
 * @brief Event structure for @ref BLE_EVT_TX_COMPLETE.
 */
typedef struct
{
  uint8_t count;                        /**< Number of packets transmitted. */
} ble_evt_tx_complete_t;

/**@brief Event structure for @ref BLE_EVT_USER_MEM_REQUEST. */
typedef struct
{
  uint8_t                     type;     /**< User memory type, see @ref BLE_USER_MEM_TYPES. */
} ble_evt_user_mem_request_t;

/**@brief Event structure for @ref BLE_EVT_USER_MEM_RELEASE. */
typedef struct
{
  uint8_t                     type;       /**< User memory type, see @ref BLE_USER_MEM_TYPES. */
  ble_user_mem_block_t        mem_block;  /**< User memory block */
} ble_evt_user_mem_release_t;


/**@brief Event structure for events not associated with a specific function module. */
typedef struct
{
  uint16_t conn_handle;                 /**< Connection Handle on which this event occurred. */
  union
  {
    ble_evt_tx_complete_t           tx_complete;        /**< Transmission Complete. */
    ble_evt_user_mem_request_t      user_mem_request;   /**< User Memory Request Event Parameters. */
    ble_evt_user_mem_release_t      user_mem_release;   /**< User Memory Release Event Parameters. */
  } params; /**< params. */
} ble_common_evt_t;

/**@brief BLE Event header. */
typedef struct
{
  uint16_t evt_id;                      /**< Value from a BLE_<module>_EVT series. */
  uint16_t evt_len;                     /**< Length in octets excluding this header. */
} ble_evt_hdr_t;

/**@brief Common BLE Event type, wrapping the module specific event reports. */
typedef struct
{
  ble_evt_hdr_t header;                 /**< Event header. */
  union
  {
    ble_common_evt_t  common_evt;         /**< Common Event, evt_id in BLE_EVT_* series. */
    ble_gap_evt_t     gap_evt;            /**< GAP originated event, evt_id in BLE_GAP_EVT_* series. */
    ble_l2cap_evt_t   l2cap_evt;          /**< L2CAP originated event, evt_id in BLE_L2CAP_EVT* series. */
    ble_gattc_evt_t   gattc_evt;          /**< GATT client originated event, evt_id in BLE_GATTC_EVT* series. */
    ble_gatts_evt_t   gatts_evt;          /**< GATT server originated event, evt_id in BLE_GATTS_EVT* series. */
  } evt; /**< evt. */
} ble_evt_t;


/**
 * @brief Version Information.
 */
typedef struct
{
  uint8_t   version_number;             /**< Link Layer Version number for BT 4.1 spec is 7 (https://www.bluetooth.org/en-us/specification/assigned-numbers/link-layer). */
  uint16_t  company_id;                 /**< Company ID */
  uint16_t  subversion_number;          /**< Link Layer Sub Version number, corresponds to the SoftDevice Config ID or Firmware ID (FWID). */
} ble_version_t;

/**@brief Mutual exclusion of radio activity and CPU execution.
 *
 *        This option configures the application's access to the CPU when the radio is active. The
 *        application can configure itself to be blocked from using the CPU while the radio is
 *        active. By default, the application will be able to share CPU time with the SoftDevice
 *        during radio activity. This parameter structure is used together with @ref xh_ble_opt_set
 *        to configure the @ref BLE_COMMON_OPT_RADIO_CPU_MUTEX option.
 *
 * @note  Note that the application should use this option to configure the SoftDevice to block the
 *        CPU during radio activity (i.e enable mutual exclusion) when running the SoftDevice on
 *        hardware affected by PAN #44 "CCM may exceed real time requirements"and PAN #45 "AAR may
 *        exceed real time requirements".
 *
 * @note  Note that when acting as a scanner, the mutex is only enabled for radio TX activity.
 *
 * @note  @ref xh_ble_opt_get is not supported for this option.
 *
 */
typedef struct
{
  uint8_t enable : 1;                          /**< Enable mutual exclusion of radio activity and the CPU execution. */
} ble_common_opt_radio_cpu_mutex_t;

/**@brief Option structure for common options. */
typedef union
{
  ble_common_opt_radio_cpu_mutex_t  radio_cpu_mutex;        /**< Parameters for the option for the mutual exclusion of radio activity and CPU execution. */
} ble_common_opt_t;

/**@brief Common BLE Option type, wrapping the module specific options. */
typedef union
{
  ble_common_opt_t  common_opt;         /**< Common option, opt_id in BLE_COMMON_OPT_* series. */
  ble_gap_opt_t     gap_opt;            /**< GAP option, opt_id in BLE_GAP_OPT_* series. */
} ble_opt_t;

/**
 * @brief BLE GATTS init options
 */
typedef struct
{
  ble_gap_enable_params_t    gap_enable_params;   /**< GAP init options @ref ble_gap_enable_params_t */
  ble_gatts_enable_params_t  gatts_enable_params; /**< GATTS init options @ref ble_gatts_enable_params_t. */
  uint8_t work_mode; /**< Deprecated. */
} ble_enable_params_t;

/** @} */

/** @addtogroup BLE_COMMON_FUNCTIONS Functions
 * @{ */

/**@brief Enable the BLE stack
 *
 * @param[in] p_ble_enable_params Pointer to ble_enable_params_t
 * @note This parameter is not used, should be set NULL.
 *
 * @details This call initializes the BLE stack, no other BLE related function can be called before this one.
 *
 * @return ::XH_SUCCESS BLE the BLE stack has been initialized successfully
 */
uint32_t xh_ble_enable(ble_enable_params_t * p_ble_enable_params);

/**@brief Get an event from the pending events queue.
 *
 * @param[out] p_dest Pointer to buffer to be filled in with an event, or NULL to retrieve the event length. This buffer <b>must be 4-byte aligned in memory</b>.
 * @param[in, out] p_len Pointer the length of the buffer, on return it is filled with the event length.
 *
 * @details This call allows the application to pull a BLE event from the BLE stack. The application is signalled that an event is 
 * available from the BLE stack by the triggering of the SD_EVT_IRQn interrupt.
 * The application is free to choose whether to call this function from thread mode (main context) or directly from the Interrupt Service Routine
 * that maps to SD_EVT_IRQn. In any case however, and because the BLE stack runs at a higher priority than the application, this function should be called
 * in a loop (until @ref XH_ERROR_NOT_FOUND is returned) every time SD_EVT_IRQn is raised to ensure that all available events are pulled from the BLE stack. 
 * Failure to do so could potentially leave events in the internal queue without the application being aware of this fact.
 * Sizing the p_dest buffer is equally important, since the application needs to provide all the memory necessary for the event to be copied into
 * application memory. If the buffer provided is not large enough to fit the entire contents of the event, @ref XH_ERROR_DATA_SIZE will be returned
 * and the application can then call again with a larger buffer size.
 * Please note that because of the variable length nature of some events, sizeof(ble_evt_t) will not always be large enough to fit certain events, 
 * and so it is the application's responsibility to provide an amount of memory large enough so that the relevant event is copied in full.
 * The application may "peek" the event length by providing p_dest as a NULL pointer and inspecting the value of *p_len upon return.
 *
 * @note The pointer supplied must be aligned to the extend defined by @ref BLE_EVTS_PTR_ALIGNMENT
 *
 * @retval ::XH_SUCCESS Event pulled and stored into the supplied buffer.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid or not sufficiently aligned pointer supplied.
 * @retval ::XH_ERROR_NOT_FOUND No events ready to be pulled.
 * @retval ::XH_ERROR_DATA_SIZE Event ready but could not fit into the supplied buffer.
 */
uint32_t xh_ble_evt_get(uint8_t *p_dest, uint16_t *p_len);


/**@brief Get the total number of available application transmission buffers per link in the BLE stack.
 *
 * @details This call allows the application to obtain the total number of
 *          transmission buffers available per link for application data. Please note that
 *          this does not give the number of free buffers, but rather the total amount of them.
 *          The application has two options to handle its own application transmission buffers:
 *          - Use a simple arithmetic calculation: at boot time the application should use this function
 *          to find out the total amount of buffers available to it and store it in a variable.
 *          Every time a packet that consumes an application buffer is sent using any of the 
 *          exposed functions in this BLE API, the application should decrement that variable.
 *          Conversely, whenever a @ref BLE_EVT_TX_COMPLETE event is received by the application
 *          it should retrieve the count field in such event and add that number to the same
 *          variable storing the number of available packets.
 *          This mechanism allows the application to be aware at any time of the number of
 *          application packets available in the BLE stack's internal buffers, and therefore
 *          it can know with certainty whether it is possible to send more data or it has to
 *          wait for a @ref BLE_EVT_TX_COMPLETE event before it proceeds.
 *          - Choose to simply not keep track of available buffers at all, and instead handle the 
 *          @ref BLE_ERROR_NO_TX_BUFFERS error by queueing the packet to be transmitted and 
 *          try again as soon as a @ref BLE_EVT_TX_COMPLETE event arrives.
 *
 *          The API functions that <b>may</b> consume an application buffer depending on 
 *          the parameters supplied to them can be found below:
 *
 *          - @ref xh_ble_gattc_write (write without response only)
 *          - @ref xh_ble_gatts_hvx (notifications only)
 *
 * @param[out] p_count Pointer to a uint8_t which will contain the number of application transmission buffers upon
 *                     successful return.
 *
 * @retval ::XH_SUCCESS Number of application transmission buffers retrieved successfully.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 */
uint32_t xh_ble_tx_buffer_count_get(uint8_t *p_count);


/**@brief Add a Vendor Specific UUID.
 *
 * @details This call enables the application to add a vendor specific UUID to the BLE stack's table,
 *          for later use all other modules and APIs. This then allows the application to use the shorter,
 *          24-bit @ref ble_uuid_t format when dealing with both 16-bit and 128-bit UUIDs without having to
 *          check for lengths and having split code paths. The way that this is accomplished is by extending the 
 *          grouping mechanism that the Bluetooth SIG standard base UUID uses for all other 128-bit UUIDs. The 
 *          type field in the @ref ble_uuid_t structure is an index (relative to @ref BLE_UUID_TYPE_VENDOR_BEGIN) 
 *          to the table populated by multiple calls to this function, and the uuid field in the same structure 
 *          contains the 2 bytes at indices 12 and 13. The number of possible 128-bit UUIDs available to the 
 *          application is therefore the number of Vendor Specific UUIDs added with the help of this function times 65536, 
 *          although restricted to modifying bytes 12 and 13 for each of the entries in the supplied array.
 *
 * @note Bytes 12 and 13 of the provided UUID will not be used internally, since those are always replaced by 
 * the 16-bit uuid field in @ref ble_uuid_t.
 *
 *
 * @param[in]  p_vs_uuid    Pointer to a 16-octet (128-bit) little endian Vendor Specific UUID disregarding
 *                          bytes 12 and 13.
 * @param[out] p_uuid_type  Pointer to a uint8_t where the type field in @ref ble_uuid_t corresponding to this UUID will be stored.
 *
 * @retval ::XH_SUCCESS Successfully added the Vendor Specific UUID.
 * @retval ::XH_ERROR_INVALID_ADDR If p_vs_uuid or p_uuid_type is NULL or invalid.
 * @retval ::XH_ERROR_NO_MEM If there are no more free slots for VS UUIDs.
 * @retval ::XH_ERROR_FORBIDDEN If p_vs_uuid has already been added to the VS UUID table.
 */
uint32_t xh_ble_uuid_vs_add(ble_uuid128_t const *p_vs_uuid, uint8_t *p_uuid_type);


/** @brief Decode little endian raw UUID bytes (16-bit or 128-bit) into a 24 bit @ref ble_uuid_t structure.
 * 
 * @details The raw UUID bytes excluding bytes 12 and 13 (i.e. bytes 0-11 and 14-15) of p_uuid_le are compared 
 * to the corresponding ones in each entry of the table of vendor specific UUIDs populated with @ref xh_ble_uuid_vs_add 
 * to look for a match. If there is such a match, bytes 12 and 13 are returned as p_uuid->uuid and the index 
 * relative to @ref BLE_UUID_TYPE_VENDOR_BEGIN as p_uuid->type. 
 *
 * @note If the UUID length supplied is 2, then the type set by this call will always be @ref BLE_UUID_TYPE_BLE.
 *
 * @param[in]      uuid_le_len Length in bytes of the buffer pointed to by p_uuid_le (must be 2 or 16 bytes).
 * @param[in]      p_uuid_le   Pointer pointing to little endian raw UUID bytes.
 * @param[out]  p_uuid      Pointer to a @ref ble_uuid_t structure to be filled in.
 *
 * @retval ::XH_SUCCESS Successfully decoded into the @ref ble_uuid_t structure.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_LENGTH Invalid UUID length.
 * @retval ::XH_ERROR_NOT_FOUND For a 128-bit UUID, no match in the populated table of UUIDs.
 */                                                 
uint32_t xh_ble_uuid_decode(uint8_t uuid_le_len, uint8_t const *p_uuid_le, ble_uuid_t *p_uuid);


/** @brief Encode a @ref ble_uuid_t structure into little endian raw UUID bytes (16-bit or 128-bit).
 *
 * @note The pointer to the destination buffer p_uuid_le may be NULL, in which case only the validity and size of p_uuid is computed.
 *
 * @param[in]      p_uuid        Pointer to a @ref ble_uuid_t structure that will be encoded into bytes.
 * @param[out]     p_uuid_le_len Pointer to a uint8_t that will be filled with the encoded length (2 or 16 bytes).
 * @param[out]     p_uuid_le     Pointer to a buffer where the little endian raw UUID bytes (2 or 16) will be stored.
 *
 * @retval ::XH_SUCCESS Successfully encoded into the buffer.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid UUID type.
 */
uint32_t xh_ble_uuid_encode(ble_uuid_t const *p_uuid, uint8_t *p_uuid_le_len, uint8_t *p_uuid_le);


/**@brief Get Version Information.
 *
 * @details This call allows the application to get the BLE stack version information.
 *
 * @param[out] p_version Pointer to a ble_version_t structure to be filled in.
 *
 * @retval ::XH_SUCCESS  Version information stored successfully.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_BUSY The BLE stack is busy (typically doing a locally-initiated disconnection procedure).
 */
uint32_t xh_ble_version_get(ble_version_t *p_version);


/**@brief Provide a user memory block.
 *
 * @note This call can only be used as a response to a @ref BLE_EVT_USER_MEM_REQUEST event issued to the application.
 *
 * @param[in] conn_handle Connection handle.
 * @param[in,out] p_block Pointer to a user memory block structure.
 *
 * @retval ::XH_SUCCESS Successfully queued a response to the peer.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::XH_ERROR_INVALID_STATE Invalid Connection state or no execute write request pending.
 * @retval ::XH_ERROR_BUSY The BLE stack is busy. Retry at later time.
 */
uint32_t xh_ble_user_mem_reply(uint16_t conn_handle, ble_user_mem_block_t const *p_block);

/**@brief Set a BLE option.
 *
 * @details This call allows the application to set the value of an option.
 *
 * @param[in] opt_id Option ID.
 * @param[in] p_opt Pointer to a ble_opt_t structure containing the option value.
 *
 * @retval ::XH_SUCCESS  Option set successfully.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied, check parameter limits and constraints.
 * @retval ::XH_ERROR_INVALID_STATE Unable to set the parameter at this time.
 * @retval ::XH_ERROR_BUSY The BLE stack is busy or the previous procedure has not completed.
 */
uint32_t xh_ble_opt_set(uint32_t opt_id, ble_opt_t const *p_opt);


/**@brief Get a BLE option.
 *
 * @details This call allows the application to retrieve the value of an option.
 *
 * @param[in] opt_id Option ID.
 * @param[out] p_opt Pointer to a ble_opt_t structure to be filled in.
 *
 * @retval ::XH_SUCCESS  Option retrieved successfully.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied, check parameter limits and constraints.
 * @retval ::XH_ERROR_INVALID_STATE Unable to retrieve the parameter at this time.
 * @retval ::XH_ERROR_BUSY The BLE stack is busy or the previous procedure has not completed.
 * @retval ::XH_ERROR_NOT_SUPPORTED This option is not supported.
 *
 */
uint32_t xh_ble_opt_get(uint32_t opt_id, ble_opt_t *p_opt);

/** @} */

/**@brief Application stack event handler type. */
typedef void (*ble_evt_handler_t) (ble_evt_t * p_ble_evt);

/**@brief     Function for registering for BLE events.
 *
 * @details   The application should use this function to register for receiving BLE events from
 *            the SoftDevice. If the application does not call this function, then any BLE event
 *            that may be generated by the SoftDevice will NOT be fetched. Once the application has
 *            registered for the events, it is not possible to cancel the registration.
 *            However, it is possible to register a different function for handling the events at
 *            any point of time.
 *
 * @param[in] ble_evt_handler Function to be called for each received BLE event.
 *
 * @retval    XH_SUCCESS     Successful registration.
 * @retval    XH_ERROR_NULL  Null pointer provided as input.
 */

uint32_t softdevice_ble_evt_handler_set(ble_evt_handler_t ble_evt_handler);


#endif /* BLE_H__ */

/**
  @}
  @}
*/
