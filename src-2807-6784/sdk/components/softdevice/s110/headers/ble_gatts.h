
/**
  @addtogroup BLE_GATTS Generic Attribute Profile (GATT) Server
  @{
  @brief  Definitions and prototypes for the GATTS interface.
 */

#ifndef BLE_GATTS_H__
#define BLE_GATTS_H__

#include "ble_types.h"
#include "ble_ranges.h"
#include "ble_l2cap.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include "xh_svc.h"

/** @addtogroup BLE_GATTS_ENUMERATIONS Enumerations
 * @{ */

/**
 * @brief GATTS API SVC numbers.
 */
enum BLE_GATTS_SVCS
{
  SD_BLE_GATTS_SERVICE_ADD = BLE_GATTS_SVC_BASE, /**< Add a service. */
  SD_BLE_GATTS_INCLUDE_ADD,                      /**< Add an included service. */
  SD_BLE_GATTS_CHARACTERISTIC_ADD,               /**< Add a characteristic. */
  SD_BLE_GATTS_DESCRIPTOR_ADD,                   /**< Add a generic attribute. */
  SD_BLE_GATTS_VALUE_SET,                        /**< Set an attribute value. */
  SD_BLE_GATTS_VALUE_GET,                        /**< Get an attribute value. */
  SD_BLE_GATTS_HVX,                              /**< Handle Value Notification or Indication. */
  SD_BLE_GATTS_SERVICE_CHANGED,                  /**< Perform a Service Changed Indication to one or more peers. */
  SD_BLE_GATTS_RW_AUTHORIZE_REPLY,               /**< Reply to an authorization request for a read or write operation on one or more attributes. */
  SD_BLE_GATTS_SYS_ATTR_SET,                     /**< Set the persistent system attributes for a connection. */
  SD_BLE_GATTS_SYS_ATTR_GET,                     /**< Retrieve the persistent system attributes. */
};

/**
 * @brief GATT Server Event IDs.
 */
enum BLE_GATTS_EVTS
{
  BLE_GATTS_EVT_WRITE = BLE_GATTS_EVT_BASE,       /**< Write operation performed. @ref ble_gatts_evt_write_t */
  BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST,             /**< Read/Write Authorization request.@ref ble_gatts_evt_rw_authorize_request_t */
  BLE_GATTS_EVT_SYS_ATTR_MISSING,                 /**< A persistent system attribute access is pending, awaiting a xh_ble_gatts_sys_attr_set(). @ref ble_gatts_evt_sys_attr_missing_t */
  BLE_GATTS_EVT_HVC,                              /**< Handle Value Confirmation. @ref ble_gatts_evt_hvc_t */
  BLE_GATTS_EVT_SC_CONFIRM,                       /**< Service Changed Confirmation. No additional event structure applies. */
  BLE_GATTS_EVT_TIMEOUT                           /**< Timeout. @ref ble_gatts_evt_timeout_t */
};
/** @} */

/** @addtogroup BLE_GATTS_DEFINES Defines
 * @{ */

/** @defgroup BLE_ERRORS_GATTS SVC return values specific to GATTS
 * @{ */
#define BLE_ERROR_GATTS_INVALID_ATTR_TYPE   (XH_GATTS_ERR_BASE + 0x000) /**< Invalid attribute type. */
#define BLE_ERROR_GATTS_SYS_ATTR_MISSING    (XH_GATTS_ERR_BASE + 0x001) /**< System Attributes missing. */
/** @} */

/** @defgroup BLE_GATTS_ATTR_LENS_MAX Maximum attribute lengths
 * @{ */
#define BLE_GATTS_FIX_ATTR_LEN_MAX (510)  /**< Maximum length for fixed length Attribute Values. */
#define BLE_GATTS_VAR_ATTR_LEN_MAX (512)  /**< Maximum length for variable length Attribute Values. */
/** @} */

/** @defgroup BLE_GATTS_SRVC_TYPES GATT Server Service Types
 * @{ */
#define BLE_GATTS_SRVC_TYPE_INVALID          0x00  /**< Invalid Service Type. */
#define BLE_GATTS_SRVC_TYPE_PRIMARY          0x01  /**< Primary Service. */
#define BLE_GATTS_SRVC_TYPE_SECONDARY        0x02  /**< Secondary Type. */
/** @} */


/** @defgroup BLE_GATTS_ATTR_TYPES GATT Server Attribute Types
 * @{ */
#define BLE_GATTS_ATTR_TYPE_INVALID         0x00  /**< Invalid Attribute Type. */
#define BLE_GATTS_ATTR_TYPE_PRIM_SRVC_DECL  0x01  /**< Primary Service Declaration. */
#define BLE_GATTS_ATTR_TYPE_SEC_SRVC_DECL   0x02  /**< Secondary Service Declaration. */
#define BLE_GATTS_ATTR_TYPE_INC_DECL        0x03  /**< Include Declaration. */
#define BLE_GATTS_ATTR_TYPE_CHAR_DECL       0x04  /**< Characteristic Declaration. */
#define BLE_GATTS_ATTR_TYPE_CHAR_VAL        0x05  /**< Characteristic Value. */
#define BLE_GATTS_ATTR_TYPE_DESC            0x06  /**< Descriptor. */
#define BLE_GATTS_ATTR_TYPE_OTHER           0x07  /**< Other, non-GATT specific type. */
/** @} */


/** @defgroup BLE_GATTS_OPS GATT Server Operations
 * @{ */
#define BLE_GATTS_OP_INVALID                0x00  /**< Invalid Operation. */
#define BLE_GATTS_OP_WRITE_REQ              0x01  /**< Write Request. */
#define BLE_GATTS_OP_WRITE_CMD              0x02  /**< Write Command. */
#define BLE_GATTS_OP_SIGN_WRITE_CMD         0x03  /**< Signed Write Command. */
#define BLE_GATTS_OP_PREP_WRITE_REQ         0x04  /**< Prepare Write Request. */
#define BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL  0x05  /**< Execute Write Request: Cancel all prepared writes. */
#define BLE_GATTS_OP_EXEC_WRITE_REQ_NOW     0x06  /**< Execute Write Request: Immediately execute all prepared writes. */
/** @} */

/** @defgroup BLE_GATTS_VLOCS GATT Value Locations
 * @{ */
#define BLE_GATTS_VLOC_INVALID       0x00  /**< Invalid Location. */
#define BLE_GATTS_VLOC_STACK         0x01  /**< Attribute Value is located in stack memory, no user memory is required. */
#define BLE_GATTS_VLOC_USER          0x02  /**< Attribute Value is located in user memory. This requires the user to maintain a valid buffer through the lifetime of the attribute, since the stack
                                                will read and write directly to the memory using the pointer provided in the APIs. There are no alignment requirements for the buffer. */
/** @} */

/** @defgroup BLE_GATTS_AUTHORIZE_TYPES GATT Server Authorization Types
 * @{ */
#define BLE_GATTS_AUTHORIZE_TYPE_INVALID    0x00  /**< Invalid Type. */
#define BLE_GATTS_AUTHORIZE_TYPE_READ       0x01  /**< Authorize a Read Operation. */
#define BLE_GATTS_AUTHORIZE_TYPE_WRITE      0x02  /**< Authorize a Write Request Operation. */
/** @} */

/** @defgroup BLE_GATTS_SYS_ATTR_FLAGS System Attribute Flags
 * @{ */
#define BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS (1 << 0)  /**< Restrict system attributes to system services only. */
#define BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS (1 << 1)  /**< Restrict system attributes to user services only. */
/** @} */


/** @} */

/** @addtogroup BLE_GATTS_STRUCTURES Structures
 * @{ */

/**
 * @brief BLE GATTS init options
 */
typedef struct
{
  uint8_t   service_changed:1;             /**< Include the Service Changed characteristic in the local attributes. */
} ble_gatts_enable_params_t;

/**@brief Attribute metadata. */
typedef struct
{
  ble_gap_conn_sec_mode_t read_perm;       /**< Read permissions. */
  ble_gap_conn_sec_mode_t write_perm;      /**< Write permissions. */
  uint8_t                 vlen       :1;   /**< Variable length attribute. */
  uint8_t                 vloc       :2;   /**< Value location, see @ref BLE_GATTS_VLOCS.*/
  uint8_t                 rd_auth    :1;   /**< Read authorization and value will be requested from the application on every read operation. */
  uint8_t                 wr_auth    :1;   /**< Write authorization will be requested from the application on every Write Request operation (but not Write Command). */
} ble_gatts_attr_md_t;


/**@brief GATT Attribute. */
typedef struct
{
  ble_uuid_t          *p_uuid;          /**< Pointer to the attribute UUID. */
  ble_gatts_attr_md_t *p_attr_md;       /**< Pointer to the attribute metadata structure. */
  uint16_t             init_len;        /**< Initial attribute value length in bytes. */
  uint16_t             init_offs;       /**< Initial attribute value offset in bytes. If different from zero, the first init_offs bytes of the attribute value will be left uninitialized. */
  uint16_t             max_len;         /**< Maximum attribute value length in bytes, see @ref BLE_GATTS_ATTR_LENS_MAX for maximum values. */
  uint8_t*             p_value;         /**< Pointer to the attribute data. Please note that if the @ref BLE_GATTS_VLOC_USER value location is selected in the attribute metadata, this will have to point to a buffer
                                             that remains valid through the lifetime of the attribute. This excludes usage of automatic variables that may go out of scope or any other temporary location.
                                             The stack may access that memory directly without the application's knowledge. For writable characteristics, this value must not be a location in flash memory.*/
} ble_gatts_attr_t;

/**@brief GATT Attribute Value. */
typedef struct
{
  uint16_t  len;        /**< Length in bytes to be written or read. Length in bytes written or read after successful return.*/
  uint16_t  offset;     /**< Attribute value offset. */
  uint8_t   *p_value;   /**< Pointer to where value is stored or will be stored.
                             If value is stored in user memory, only the attribute length is updated when p_value == NULL.
                             Set to NULL when reading to obtain the complete length of the attribute value */
} ble_gatts_value_t;


/**@brief GATT Attribute Context. */
typedef struct
{
  ble_uuid_t           srvc_uuid;       /**< Service UUID. */
  ble_uuid_t           char_uuid;       /**< Characteristic UUID if applicable (BLE_UUID_TYPE_UNKNOWN otherwise). */
  ble_uuid_t           desc_uuid;       /**< Descriptor UUID if applicable (BLE_UUID_TYPE_UNKNOWN otherwise). */
  uint16_t             srvc_handle;     /**< Service Handle. */
  uint16_t             value_handle;    /**< Characteristic Value Handle if applicable (BLE_GATT_HANDLE_INVALID otherwise). */
  uint8_t              type;            /**< Attribute Type, see @ref BLE_GATTS_ATTR_TYPES. */
} ble_gatts_attr_context_t;


/**@brief GATT Characteristic Presentation Format. */
typedef struct
{
  uint8_t          format;      /**< Format of the value, see @ref BLE_GATT_CPF_FORMATS. */
  int8_t           exponent;    /**< Exponent for integer data types. */
  uint16_t         unit;        /**< Unit from Bluetooth Assigned Numbers. */
  uint8_t          name_space;  /**< Namespace from Bluetooth Assigned Numbers, see @ref BLE_GATT_CPF_NAMESPACES. */
  uint16_t         desc;        /**< Namespace description from Bluetooth Assigned Numbers, see @ref BLE_GATT_CPF_NAMESPACES. */
} ble_gatts_char_pf_t;


/**@brief GATT Characteristic metadata. */
typedef struct
{
  ble_gatt_char_props_t       char_props;               /**< Characteristic Properties. */
  ble_gatt_char_ext_props_t   char_ext_props;           /**< Characteristic Extended Properties. */
  uint8_t                    *p_char_user_desc;         /**< Pointer to a UTF-8 encoded string (non-NULL terminated), NULL if the descriptor is not required. */
  uint16_t                    char_user_desc_max_size;  /**< The maximum size in bytes of the user description descriptor. */
  uint16_t                    char_user_desc_size;      /**< The size of the user description, must be smaller or equal to char_user_desc_max_size. */
  ble_gatts_char_pf_t*        p_char_pf;                /**< Pointer to a presentation format structure or NULL if the CPF descriptor is not required. */
  ble_gatts_attr_md_t*        p_user_desc_md;           /**< Attribute metadata for the User Description descriptor, or NULL for default values. */
  ble_gatts_attr_md_t*        p_cccd_md;                /**< Attribute metadata for the Client Characteristic Configuration Descriptor, or NULL for default values. */
  ble_gatts_attr_md_t*        p_sccd_md;                /**< Attribute metadata for the Server Characteristic Configuration Descriptor, or NULL for default values. */
} ble_gatts_char_md_t;


/**@brief GATT Characteristic Definition Handles. */
typedef struct
{
  uint16_t          value_handle;       /**< Handle to the characteristic value. */
  uint16_t          user_desc_handle;   /**< Handle to the User Description descriptor, or @ref BLE_GATT_HANDLE_INVALID if not present. */
  uint16_t          cccd_handle;        /**< Handle to the Client Characteristic Configuration Descriptor, or @ref BLE_GATT_HANDLE_INVALID if not present. */
  uint16_t          sccd_handle;        /**< Handle to the Server Characteristic Configuration Descriptor, or @ref BLE_GATT_HANDLE_INVALID if not present. */
} ble_gatts_char_handles_t;


/**@brief GATT HVx parameters. */
typedef struct
{
  uint16_t          handle;             /**< Characteristic Value Handle. */
  uint8_t           type;               /**< Indication or Notification, see @ref BLE_GATT_HVX_TYPES. */
  uint16_t          offset;             /**< Offset within the attribute value. */
  uint16_t         *p_len;              /**< Length in bytes to be written, length in bytes written after successful return. */
  uint8_t          *p_data;             /**< Actual data content, use NULL to use the current attribute value. */
} ble_gatts_hvx_params_t;

/**@brief GATT Read Authorization parameters. */
typedef struct
{
  uint16_t          gatt_status;        /**< GATT status code for the operation, see @ref BLE_GATT_STATUS_CODES. */
  uint8_t           update : 1;         /**< If set, data supplied in p_data will be used in the ATT response. */
  uint16_t          offset;             /**< Offset of the attribute value being updated. */
  uint16_t          len;                /**< Length in bytes of the value in p_data pointer, see @ref BLE_GATTS_ATTR_LENS_MAX. */
  uint8_t          *p_data;             /**< Pointer to new value used to update the attribute value. */
} ble_gatts_read_authorize_params_t;

/**@brief GATT Write Authorization parameters. */
typedef struct
{
  uint16_t          gatt_status;        /**< GATT status code for the operation, see @ref BLE_GATT_STATUS_CODES. */
} ble_gatts_write_authorize_params_t;

/**@brief GATT Read or Write Authorize Reply parameters. */
typedef struct
{
  uint8_t                               type;   /**< Type of authorize operation, see @ref BLE_GATTS_AUTHORIZE_TYPES. */
  union {
    ble_gatts_read_authorize_params_t   read;   /**< Read authorization parameters. */
    ble_gatts_write_authorize_params_t  write;  /**< Write authorization parameters. */
  } params;                                     /**< Reply Parameters. */
} ble_gatts_rw_authorize_reply_params_t;



/**@brief Event structure for @ref BLE_GATTS_EVT_WRITE. */
typedef struct
{
  uint16_t                    handle;             /**< Attribute Handle. */
  uint8_t                     op;                 /**< Type of write operation, see @ref BLE_GATTS_OPS. */
  ble_gatts_attr_context_t    context;            /**< Attribute Context. */
  uint16_t                    offset;             /**< Offset for the write operation. */
  uint16_t                    len;                /**< Length of the received data. */
  uint8_t                     *data;            /**< Received data, variable length. */
} ble_gatts_evt_write_t;

/**@brief Event substructure for authorized read requests, see @ref ble_gatts_evt_rw_authorize_request_t. */
typedef struct
{
  uint16_t                    handle;             /**< Attribute Handle. */
  ble_gatts_attr_context_t    context;            /**< Attribute Context. */
  uint16_t                    offset;             /**< Offset for the read operation. */
} ble_gatts_evt_read_t;

/**@brief Event structure for @ref BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST. */
typedef struct
{
  uint8_t                     type;             /**< Type of authorize operation, see @ref BLE_GATTS_AUTHORIZE_TYPES. */
  union {
    ble_gatts_evt_read_t      read;             /**< Attribute Read Parameters. */
    ble_gatts_evt_write_t     write;            /**< Attribute Write Parameters. */
  } request;                                    /**< Request Parameters. */
} ble_gatts_evt_rw_authorize_request_t;

/**@brief Event structure for @ref BLE_GATTS_EVT_SYS_ATTR_MISSING. */
typedef struct
{
  uint8_t hint;                                 /**< Hint (currently unused). */
} ble_gatts_evt_sys_attr_missing_t;


/**@brief Event structure for @ref BLE_GATTS_EVT_HVC. */
typedef struct
{
  uint16_t          handle;                       /**< Attribute Handle. */
} ble_gatts_evt_hvc_t;

/**@brief Event structure for @ref BLE_GATTS_EVT_TIMEOUT. */
typedef struct
{
  uint8_t          src;                       /**< Timeout source, see @ref BLE_GATT_TIMEOUT_SOURCES. */
} ble_gatts_evt_timeout_t;


/**@brief GATT Server event callback event structure. */
typedef struct
{
  uint16_t conn_handle;                                       /**< Connection Handle on which the event occurred. */
  union
  {
    ble_gatts_evt_write_t                 write;              /**< Write Event Parameters. */
    ble_gatts_evt_rw_authorize_request_t  authorize_request;  /**< Read or Write Authorize Request Parameters. */
    ble_gatts_evt_sys_attr_missing_t      sys_attr_missing;   /**< System attributes missing. */
    ble_gatts_evt_hvc_t                   hvc;                /**< Handle Value Confirmation Event Parameters. */
    ble_gatts_evt_timeout_t               timeout;            /**< Timeout Event. */
  } params;                                                   /**< Event Parameters. */
} ble_gatts_evt_t;

extern const uint8_t ATT_SERV_UUID[2];       /**< Primary Service Declaration */
extern const uint8_t ATT_SEC_SERV_UUID[2];   /**< Secondary Service Declaration */
extern const uint8_t ATT_INCLUDE_UUID[2];    /**< Included Service Declaration */
extern const uint8_t ATT_CHAR_UUID[2];       /**< Characteristic Declaration */
extern const uint8_t ATT_CCCD_UUID[2];       /**< Client Characteristic Configuration Descriptor */
extern const uint8_t ATT_CUDD_UUID[2];       /**< Characteristic User Description Descriptor */
extern const uint8_t ATT_RRD_UUID[2];        /**< Report reference */

#define ATT_PROP_READ          (1<<1)  /**< attribute property of read */
#define ATT_PROP_WRITE_CMD     (1<<2)  /**< attribute property of write command */
#define ATT_PROP_WRITE         (1<<3)  /**< attribute property of write request */
#define ATT_PROP_NTF           (1<<4)  /**< attribute property of notification */
#define ATT_PROP_IND           (1<<5)  /**< attribute property of indication */

/**
 * @brief BLE GATTS database attribute
 */
typedef struct {
    const uint8_t* uuid;            /**< UUID of this attribute, must be global variable. */
    uint8_t* value;                 /**< Value of this attribute, must be global variable. */
    uint16_t value_len;             /**< Length of value */
    uint8_t properties;             /**< Properties of this attribute */
    uint8_t uuid_128bit    : 1;     /**< True if the uuid is 128 bit else 16 bit */
    uint8_t perm_read      : 1;     /**< True if bond is required when read this attribute */
    uint8_t perm_write     : 1;     /**< True if bond is required when write this attribute */
    uint16_t value_max_len;         /**< Maximum length of the value can be written */
} att_db_t;

/**
 * @brief BLE GATTS database service
 */
typedef struct {
    const att_db_t* att_db;         /**< Attribute list of the service */
    uint16_t att_num;               /**< Number of attribute of the att_db */
} gatt_serv_db_t;

/** @} */

/** @addtogroup BLE_GATTS_FUNCTIONS Functions
 * @{ */

/**@brief Add a service declaration to the Attribute Table.
 *
 * @param[in] type      Toggles between primary and secondary services, see @ref BLE_GATTS_SRVC_TYPES.
 * @param[in] p_uuid    Pointer to service UUID.
 * @param[out] p_handle Pointer to a 16-bit word where the assigned handle will be stored.
 *
 * @note Secondary Services are only relevant in the context of the entity that references them, it is therefore forbidden to
 *       add a secondary service declaration that is not referenced by another service later in the Attribute Table.
 *
 * @retval ::XH_SUCCESS Successfully added a service declaration.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied, Vendor Specific UUIDs need to be present in the table.
 * @retval ::XH_ERROR_FORBIDDEN Forbidden value supplied, certain UUIDs are reserved for the stack.
 * @retval ::XH_ERROR_NO_MEM Not enough memory to complete operation.
 */
uint32_t xh_ble_gatts_service_add(uint8_t type, ble_uuid_t const *p_uuid, uint16_t *p_handle);


/**@brief Add an include declaration to the Attribute Table.
 *
 * @note It is currently only possible to add an include declaration to the last added service (i.e. only sequential population is supported at this time).
 *
 * @note The included service must already be present in the Attribute Table prior to this call.
 *
 * @param[in] service_handle    Handle of the service where the included service is to be placed, if @ref BLE_GATT_HANDLE_INVALID is used, it will be placed sequentially.
 * @param[in] inc_srvc_handle   Handle of the included service.
 * @param[out] p_include_handle Pointer to a 16-bit word where the assigned handle will be stored.
 *
 * @retval ::XH_SUCCESS Successfully added an include declaration.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied, handle values need to match previously added services.
 * @retval ::XH_ERROR_INVALID_STATE Invalid state to perform operation.
 * @retval ::XH_ERROR_FORBIDDEN Forbidden value supplied, self inclusions are not allowed.
 * @retval ::XH_ERROR_NO_MEM Not enough memory to complete operation.
 * @retval ::XH_ERROR_NOT_FOUND Attribute not found.
 */
uint32_t xh_ble_gatts_include_add(uint16_t service_handle, uint16_t inc_srvc_handle, uint16_t *p_include_handle);


/**@brief Add a characteristic declaration, a characteristic value declaration and optional characteristic descriptor declarations to the Attribute Table.
 *
 * @note It is currently only possible to add a characteristic to the last added service (i.e. only sequential population is supported at this time).
 *
 * @note Several restrictions apply to the parameters, such as matching permissions between the user description descriptor and the writeable auxiliaries bits,
 *       readable (no security) and writeable (selectable) CCCDs and SCCDs and valid presentation format values.
 *
 * @note If no metadata is provided for the optional descriptors, their permissions will be derived from the characteristic permissions.
 *
 * @param[in] service_handle    Handle of the service where the characteristic is to be placed, if @ref BLE_GATT_HANDLE_INVALID is used, it will be placed sequentially.
 * @param[in] p_char_md         Characteristic metadata.
 * @param[in] p_attr_char_value Pointer to the attribute structure corresponding to the characteristic value.
 * @param[out] p_handles        Pointer to the structure where the assigned handles will be stored.
 *
 * @retval ::XH_SUCCESS Successfully added a characteristic.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied, service handle, Vendor Specific UUIDs, lengths, and permissions need to adhere to the constraints.
 * @retval ::XH_ERROR_INVALID_STATE Invalid state to perform operation, a service context is required.
 * @retval ::XH_ERROR_FORBIDDEN Forbidden value supplied, certain UUIDs are reserved for the stack.
 * @retval ::XH_ERROR_NO_MEM Not enough memory to complete operation.
 * @retval ::XH_ERROR_DATA_SIZE Invalid data size(s) supplied, attribute lengths are restricted by @ref BLE_GATTS_ATTR_LENS_MAX.
 */
uint32_t xh_ble_gatts_characteristic_add(uint16_t service_handle, ble_gatts_char_md_t const *p_char_md, ble_gatts_attr_t const *p_attr_char_value, ble_gatts_char_handles_t *p_handles);


/**@brief Add a descriptor to the Attribute Table.
 *
 * @note It is currently only possible to add a descriptor to the last added characteristic (i.e. only sequential population is supported at this time).
 *
 * @param[in] char_handle   Handle of the characteristic where the descriptor is to be placed, if @ref BLE_GATT_HANDLE_INVALID is used, it will be placed sequentially.
 * @param[in] p_attr        Pointer to the attribute structure.
 * @param[out] p_handle     Pointer to a 16-bit word where the assigned handle will be stored.
 *
 * @retval ::XH_SUCCESS Successfully added a descriptor.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied, characteristic handle, Vendor Specific UUIDs, lengths, and permissions need to adhere to the constraints.
 * @retval ::XH_ERROR_INVALID_STATE Invalid state to perform operation, a characteristic context is required.
 * @retval ::XH_ERROR_FORBIDDEN Forbidden value supplied, certain UUIDs are reserved for the stack.
 * @retval ::XH_ERROR_NO_MEM Not enough memory to complete operation.
 * @retval ::XH_ERROR_DATA_SIZE Invalid data size(s) supplied, attribute lengths are restricted by @ref BLE_GATTS_ATTR_LENS_MAX.
 */
uint32_t xh_ble_gatts_descriptor_add(uint16_t char_handle, ble_gatts_attr_t const *p_attr, uint16_t *p_handle);

/**@brief Set the value of a given attribute.
 *
 * @param[in] conn_handle  Connection handle. If the value does not belong to a system attribute then @ref BLE_CONN_HANDLE_INVALID can be used.
 * @param[in] handle       Attribute handle.
 * @param[in,out] p_value  Attribute value information.
 *
 * @note Values other than system attributes can be set at any time, regardless of wheter any active connections exist.
 *
 * @retval ::XH_SUCCESS Successfully set the value of the attribute.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied.
 * @retval ::XH_ERROR_NOT_FOUND Attribute not found.
 * @retval ::XH_ERROR_FORBIDDEN Forbidden handle supplied, certain attributes are not modifiable by the application.
 * @retval ::XH_ERROR_DATA_SIZE Invalid data size(s) supplied, attribute lengths are restricted by @ref BLE_GATTS_ATTR_LENS_MAX.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid connection handle supplied.
 * @retval ::BLE_ERROR_GATTS_INVALID_ATTR_TYPE @ref BLE_CONN_HANDLE_INVALID supplied on a system attribute.
 */
uint32_t xh_ble_gatts_value_set(uint16_t conn_handle, uint16_t handle, ble_gatts_value_t *p_value);

/**@brief Get the value of a given attribute.
 *
 * @param[in] conn_handle  Connection handle. If the value does not belong to a system attribute then @ref BLE_CONN_HANDLE_INVALID can be used.
 * @param[in] handle       Attribute handle.
 * @param[in,out] p_value  Attribute value information.
 *
 * @note                 If the attribute value is longer than the size of the supplied buffer,
 *                       p_len will return the total attribute value length (excluding offset),
 *                       and not the number of bytes actually returned in p_data.
 *                       The application may use this information to allocate a suitable buffer size.
 *
 * @note                 When retrieving system attribute values with this function, the connection handle
 *                       may refer to an already disconnected connection. Refer to the documentation of
 *                       @ref xh_ble_gatts_sys_attr_get for further information.
 *
 * @retval ::XH_SUCCESS Successfully retrieved the value of the attribute.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_NOT_FOUND Attribute not found.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid connection handle supplied.
 * @retval ::BLE_ERROR_GATTS_INVALID_ATTR_TYPE @ref BLE_CONN_HANDLE_INVALID supplied on a system attribute.
 */
uint32_t xh_ble_gatts_value_get(uint16_t conn_handle, uint16_t handle, ble_gatts_value_t *p_value);

/**@brief Notify or Indicate an attribute value.
 *
 * @details This function checks for the relevant Client Characteristic Configuration descriptor value to verify that the relevant operation
 *          (notification or indication) has been enabled by the client. It is also able to update the attribute value before issuing the PDU, so that
 *          the application can atomically perform a value update and a server initiated transaction with a single API call.
 *          If the application chooses to indicate an attribute value, a @ref BLE_GATTS_EVT_HVC event will be issued as soon as the confirmation arrives from
 *          the peer.
 *
 * @note    The local attribute value may be updated even if an outgoing packet is not sent to the peer due to an error during execution.
 *          When receiveing the error codes @ref XH_ERROR_INVALID_STATE, @ref XH_ERROR_BUSY, @ref BLE_ERROR_GATTS_SYS_ATTR_MISSING and
 *          @ref BLE_ERROR_NO_TX_BUFFERS the Attribute Table has been updated.
 *          The caller can check whether the value has been updated by looking at the contents of *(p_hvx_params->p_len).
 *
 * @note    It is important to note that a notification will <b>consume an application buffer</b>, and will therefore
 *          generate a @ref BLE_EVT_TX_COMPLETE event when the packet has been transmitted. An indication on the other hand will use the
 *          standard server internal buffer and thus will only generate a @ref BLE_GATTS_EVT_HVC event as soon as the confirmation
 *          has been received from the peer. Please see the documentation of @ref xh_ble_tx_buffer_count_get for more details.
 *
 * @param[in] conn_handle  Connection handle.
 * @param[in] p_hvx_params Pointer to an HVx parameters structure. If the p_data member contains a non-NULL pointer the attribute value will be updated with
 *                         the contents pointed by it before sending the notification or indication.
 *
 * @retval ::XH_SUCCESS Successfully queued a notification or indication for transmission, and optionally updated the attribute value.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::XH_ERROR_INVALID_STATE Invalid Connection State or notifications and/or indications not enabled in the CCCD.
 * @retval ::XH_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::XH_ERROR_INVALID_PARAM Invalid parameter(s) supplied.
 * @retval ::BLE_ERROR_INVALID_ATTR_HANDLE Invalid attribute handle(s) supplied. Only attributes added directly by the application are available to notify and indicate.
 * @retval ::BLE_ERROR_GATTS_INVALID_ATTR_TYPE Invalid attribute type(s) supplied, only characteristic values may be notified and indicated.
 * @retval ::XH_ERROR_NOT_FOUND Attribute not found.
 * @retval ::XH_ERROR_DATA_SIZE Invalid data size(s) supplied.
 * @retval ::XH_ERROR_BUSY Procedure already in progress.
 * @retval ::BLE_ERROR_NO_TX_BUFFERS There are no available buffers to send the data, applies only to notifications.
 */
uint32_t xh_ble_gatts_hvx(uint16_t conn_handle, ble_gatts_hvx_params_t const *p_hvx_params);

/**@brief deprecated.
 */
uint32_t xh_ble_gatts_service_changed(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);

/**@brief deprecated.
 */
uint32_t xh_ble_gatts_rw_authorize_reply(uint16_t conn_handle, ble_gatts_rw_authorize_reply_params_t const *p_rw_authorize_reply_params);


/**@brief deprecated.
 */
uint32_t xh_ble_gatts_sys_attr_set(uint16_t conn_handle, uint8_t const *p_sys_attr_data, uint16_t len, uint32_t flags);


/**@brief deprecated.
 */
uint32_t xh_ble_gatts_sys_attr_get(uint16_t conn_handle, uint8_t *p_sys_attr_data, uint16_t *p_len, uint32_t flags);

/**@brief Init database by const serv_db.
 */
uint16_t gatt_database_add(const gatt_serv_db_t* serv_db, uint16_t* handle);
/** @} */

#endif // BLE_GATTS_H__

/**
  @}
*/
