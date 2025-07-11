/**
 * @defgroup xh_soc_api SoC Library API
 * @{
 * 
 * @brief APIs for the SoC library.
 * 
 */

#ifndef XH_SOC_H__
#define XH_SOC_H__

#include <stdint.h>
#include <stdbool.h>
#include "xh_svc.h"
#include "xh51.h"
#include "xh51_bitfields.h"
#include "xh_error_soc.h"

/**@addtogroup XH_SOC_DEFINES Defines
 * @{ */

/**@brief The number of the lowest SVC number reserved for the SoC library. */
#define SOC_SVC_BASE               (0x20)
#define SOC_SVC_BASE_NOT_AVAILABLE (0x2B)

/**@brief Guranteed time for application to process radio inactive notification. */
#define XH_RADIO_NOTIFICATION_INACTIVE_GUARANTEED_TIME_US  (62)

/**@brief The minimum allowed timeslot extension time. */
#define XH_RADIO_MINIMUM_TIMESLOT_LENGTH_EXTENSION_TIME_US (200)

#define SOC_ECB_KEY_LENGTH                (16)                       /**< ECB key length. */
#define SOC_ECB_CLEARTEXT_LENGTH          (16)                       /**< ECB cleartext length. */
#define SOC_ECB_CIPHERTEXT_LENGTH         (SOC_ECB_CLEARTEXT_LENGTH) /**< ECB ciphertext length. */

#define SD_EVT_IRQn                       (SWI2_IRQn)        /**< SoftDevice Event IRQ number. Used for both protocol events and SoC events. */
#define SD_EVT_IRQHandler                 (SWI2_IRQHandler)  /**< SoftDevice Event IRQ handler. Used for both protocol events and SoC events. */
#define RADIO_NOTIFICATION_IRQn           (SWI1_IRQn)        /**< The radio notification IRQ number. */
#define RADIO_NOTIFICATION_IRQHandler     (SWI1_IRQHandler)  /**< The radio notification IRQ handler. */

#define XH_RADIO_LENGTH_MIN_US           (100)               /**< The shortest allowed radio timeslot, in microseconds. */
#define XH_RADIO_LENGTH_MAX_US           (100000)            /**< The longest allowed radio timeslot, in microseconds. */

#define XH_RADIO_DISTANCE_MAX_US         (128000000UL - 1UL) /**< The longest timeslot distance, in microseconds, allowed for the distance parameter (see @ref xh_radio_request_normal_t) in the request. */

#define XH_RADIO_EARLIEST_TIMEOUT_MAX_US (128000000UL - 1UL) /**< The longest timeout, in microseconds, allowed when requesting the earliest possible timeslot. */

#define XH_RADIO_START_JITTER_US         (2)                 /**< The maximum jitter in @ref XH_RADIO_CALLBACK_SIGNAL_TYPE_START relative to the requested start time. */

/**@} */

/**@addtogroup XH_SOC_TYPES Types
 * @{ */

/**@brief The SVC numbers used by the SVC functions in the SoC library. */
enum XH_SOC_SVCS
{
  SD_PPI_CHANNEL_ENABLE_GET = SOC_SVC_BASE,
  SD_PPI_CHANNEL_ENABLE_SET,
  SD_PPI_CHANNEL_ENABLE_CLR,
  SD_PPI_CHANNEL_ASSIGN,
  SD_PPI_GROUP_TASK_ENABLE,
  SD_PPI_GROUP_TASK_DISABLE,
  SD_PPI_GROUP_ASSIGN,
  SD_PPI_GROUP_GET,
  SD_FLASH_PAGE_ERASE,
  SD_FLASH_WRITE,
  SD_FLASH_PROTECT,
  SD_MUTEX_NEW = SOC_SVC_BASE_NOT_AVAILABLE,
  SD_MUTEX_ACQUIRE,
  SD_MUTEX_RELEASE,
  SD_NVIC_ENABLEIRQ,
  SD_NVIC_DISABLEIRQ,
  SD_NVIC_GETPENDINGIRQ,
  SD_NVIC_SETPENDINGIRQ,
  SD_NVIC_CLEARPENDINGIRQ,
  SD_NVIC_SETPRIORITY,
  SD_NVIC_GETPRIORITY,
  SD_NVIC_SYSTEMRESET,
  SD_NVIC_CRITICAL_REGION_ENTER,
  SD_NVIC_CRITICAL_REGION_EXIT,
  SD_RAND_APPLICATION_POOL_CAPACITY,
  SD_RAND_APPLICATION_BYTES_AVAILABLE,
  SD_RAND_APPLICATION_GET_VECTOR,
  SD_POWER_MODE_SET,
  SD_POWER_SYSTEM_OFF,
  SD_POWER_RESET_REASON_GET,
  SD_POWER_RESET_REASON_CLR,
  SD_POWER_POF_ENABLE,
  SD_POWER_POF_THRESHOLD_SET,
  SD_POWER_RAMON_SET,
  SD_POWER_RAMON_CLR,
  SD_POWER_RAMON_GET,
  SD_POWER_GPREGRET_SET,
  SD_POWER_GPREGRET_CLR,
  SD_POWER_GPREGRET_GET,
  SD_POWER_DCDC_MODE_SET,
  SD_APP_EVT_WAIT,
  SD_CLOCK_HFCLK_REQUEST,
  SD_CLOCK_HFCLK_RELEASE,
  SD_CLOCK_HFCLK_IS_RUNNING,
  SD_RADIO_NOTIFICATION_CFG_SET,
  SD_ECB_BLOCK_ENCRYPT,
  SD_RADIO_SESSION_OPEN,
  SD_RADIO_SESSION_CLOSE,
  SD_RADIO_REQUEST,
  SD_EVT_GET,
  SD_TEMP_GET,
  SVC_SOC_LAST
};

/**@brief Possible values of a ::xh_mutex_t. */
enum XH_MUTEX_VALUES
{
  XH_MUTEX_FREE,
  XH_MUTEX_TAKEN
};

/**@brief Possible values of ::xh_app_irq_priority_t. */
enum XH_APP_PRIORITIES
{
  XH_APP_PRIORITY_HIGH = 1,
  XH_APP_PRIORITY_LOW = 3
};

/**@brief Possible values of ::xh_power_mode_t. */
enum XH_POWER_MODES
{
  XH_POWER_MODE_CONSTLAT,  /**< Constant latency mode. See power management in the reference manual. */
  XH_POWER_MODE_LOWPWR     /**< Low power mode. See power management in the reference manual. */
};


/**@brief Possible values of ::xh_power_failure_threshold_t */
enum XH_POWER_THRESHOLDS
{
  XH_POWER_THRESHOLD_V21,  /**< 2.1 Volts power failure threshold. */
  XH_POWER_THRESHOLD_V23,  /**< 2.3 Volts power failure threshold. */
  XH_POWER_THRESHOLD_V25,  /**< 2.5 Volts power failure threshold. */ 
  XH_POWER_THRESHOLD_V27   /**< 2.7 Volts power failure threshold. */
};


/**@brief Possible values of ::xh_power_dcdc_mode_t. */
enum XH_POWER_DCDC_MODES
{
  XH_POWER_DCDC_DISABLE,          /**< The DCDC is disabled. */
  XH_POWER_DCDC_ENABLE            /**< The DCDC is enabled.  */
};

/**@brief Possible values of ::xh_radio_notification_distance_t. */
enum XH_RADIO_NOTIFICATION_DISTANCES
{
  XH_RADIO_NOTIFICATION_DISTANCE_NONE = 0, /**< The event does not have a notification. */
  XH_RADIO_NOTIFICATION_DISTANCE_800US,    /**< The distance from the active notification to start of radio activity. */
  XH_RADIO_NOTIFICATION_DISTANCE_1740US,   /**< The distance from the active notification to start of radio activity. */
  XH_RADIO_NOTIFICATION_DISTANCE_2680US,   /**< The distance from the active notification to start of radio activity. */
  XH_RADIO_NOTIFICATION_DISTANCE_3620US,   /**< The distance from the active notification to start of radio activity. */
  XH_RADIO_NOTIFICATION_DISTANCE_4560US,   /**< The distance from the active notification to start of radio activity. */
  XH_RADIO_NOTIFICATION_DISTANCE_5500US    /**< The distance from the active notification to start of radio activity. */
};


/**@brief Possible values of ::xh_radio_notification_type_t. */
enum XH_RADIO_NOTIFICATION_TYPES
{
  XH_RADIO_NOTIFICATION_TYPE_NONE = 0,        /**< The event does not have a radio notification signal. */
  XH_RADIO_NOTIFICATION_TYPE_INT_ON_ACTIVE,   /**< Using interrupt for notification when the radio will be enabled. */
  XH_RADIO_NOTIFICATION_TYPE_INT_ON_INACTIVE, /**< Using interrupt for notification when the radio has been disabled. */
  XH_RADIO_NOTIFICATION_TYPE_INT_ON_BOTH,     /**< Using interrupt for notification both when the radio will be enabled and disabled. */
};

/**@brief SoC Events. */
enum XH_SOC_EVTS
{
  XH_EVT_HFCLKSTARTED,                         /**< Event indicating that the HFCLK has started. */
  XH_EVT_POWER_FAILURE_WARNING,                /**< Event indicating that a power failure warning has occurred. */
  XH_EVT_FLASH_OPERATION_SUCCESS,              /**< Event indicating that the ongoing flash operation has completed successfully. */
  XH_EVT_FLASH_OPERATION_ERROR,                /**< Event indicating that the ongoing flash operation has timed out with an error. */
  XH_EVT_RADIO_BLOCKED,                        /**< Event indicating that a radio timeslot was blocked. */
  XH_EVT_RADIO_CANCELED,                       /**< Event indicating that a radio timeslot was canceled by SoftDevice. */
  XH_EVT_RADIO_SIGNAL_CALLBACK_INVALID_RETURN, /**< Event indicating that a radio signal callback handler return was invalid. */
  XH_EVT_RADIO_SESSION_IDLE,                   /**< Event indicating that a radio session is idle. */
  XH_EVT_RADIO_SESSION_CLOSED,                 /**< Event indicating that a radio session is closed. */
  XH_EVT_NUMBER_OF_EVTS
};

/**@} */

/**@addtogroup XH_SOC_TYPES Types
 * @{ */

/**@brief Represents a mutex for use with the xh_mutex functions.
 * @note Accessing the value directly is not safe, use the mutex functions!
 */
typedef volatile uint8_t xh_mutex_t;

/**@brief The interrupt priorities available to the application while the softdevice is active. */
typedef uint8_t xh_app_irq_priority_t;

/**@brief Represents a power mode, used in power mode functions */
typedef uint8_t xh_power_mode_t;

/**@brief Represents a power failure threshold value. */
typedef uint8_t xh_power_failure_threshold_t;

/**@brief Represents a DCDC mode value. */
typedef uint32_t xh_power_dcdc_mode_t;

/**@brief Radio notification distances. */
typedef uint8_t xh_radio_notification_distance_t;

/**@brief Radio notification types. */
typedef uint8_t xh_radio_notification_type_t;

/**@brief The Radio signal callback types. */
enum XH_RADIO_CALLBACK_SIGNAL_TYPE
{
  XH_RADIO_CALLBACK_SIGNAL_TYPE_START,             /**< This signal indicates the start of the radio timeslot. */
  XH_RADIO_CALLBACK_SIGNAL_TYPE_TIMER0,            /**< This signal indicates the XH_TIMER0 interrupt. */
  XH_RADIO_CALLBACK_SIGNAL_TYPE_RADIO,             /**< This signal indicates the XH_RADIO interrupt. */
  XH_RADIO_CALLBACK_SIGNAL_TYPE_EXTEND_FAILED,     /**< This signal indicates extend action failed. */
  XH_RADIO_CALLBACK_SIGNAL_TYPE_EXTEND_SUCCEEDED   /**< This signal indicates extend action succeeded. */
};

/**@brief The actions requested by the signal callback.
 *
 *  This code gives the SOC instructions about what action to take when the signal callback has
 *  returned.
 */
enum XH_RADIO_SIGNAL_CALLBACK_ACTION
{
  XH_RADIO_SIGNAL_CALLBACK_ACTION_NONE,            /**< Return without action. */
  XH_RADIO_SIGNAL_CALLBACK_ACTION_EXTEND,          /**< Request an extension of the current timeslot (maximum execution time for this action is when the extension succeeded). */
  XH_RADIO_SIGNAL_CALLBACK_ACTION_END,             /**< End the current radio timeslot. */
  XH_RADIO_SIGNAL_CALLBACK_ACTION_REQUEST_AND_END  /**< Request a new radio timeslot and end the current timeslot. */
};

/**@brief Radio timeslot high frequency clock source configuration. */
enum XH_RADIO_HFCLK_CFG
{
  XH_RADIO_HFCLK_CFG_DEFAULT,                      /**< Use the currently selected oscillator as HF clock source during the timeslot (i.e. the source is not specified). */
  XH_RADIO_HFCLK_CFG_FORCE_XTAL,                   /**< Force external crystal to be used as HF clock source during whole the timeslot. */
};

/**@brief Radio timeslot priorities. */
enum XH_RADIO_PRIORITY
{
  XH_RADIO_PRIORITY_HIGH,                          /**< High (equal priority as the normal connection priority of the SoftDevice stack(s)). */
  XH_RADIO_PRIORITY_NORMAL,                        /**< Normal (equal priority as the priority of secondary activites of the SoftDevice stack(s)). */
};

/**@brief Radio timeslot request type. */
enum XH_RADIO_REQUEST_TYPE
{
  XH_RADIO_REQ_TYPE_EARLIEST,                      /**< Request timeslot as early as possible. This should always be used for the first request in a session. */
  XH_RADIO_REQ_TYPE_NORMAL                         /**< Normal timeslot request. */
};

/**@brief Parameters for a request for a timeslot as early as possible. */
typedef struct
{
  uint8_t       hfclk;                              /**< High frequency clock source, see @ref XH_RADIO_HFCLK_CFG. */
  uint8_t       priority;                           /**< The radio timeslot priority, see @ref XH_RADIO_PRIORITY. */
  uint32_t      length_us;                          /**< The radio timeslot length (in the range 100 to 100,000] microseconds). */
  uint32_t      timeout_us;                         /**< Longest acceptable delay until the start of the requested timeslot (up to @ref XH_RADIO_EARLIEST_TIMEOUT_MAX_US microseconds). */
} xh_radio_request_earliest_t;

/**@brief Parameters for a normal radio request. */
typedef struct
{
  uint8_t       hfclk;                              /**< High frequency clock source, see @ref XH_RADIO_HFCLK_CFG. */
  uint8_t       priority;                           /**< The radio timeslot priority, see @ref XH_RADIO_PRIORITY. */
  uint32_t      distance_us;                        /**< Distance from the start of the previous radio timeslot (up to @ref XH_RADIO_DISTANCE_MAX_US microseconds). */
  uint32_t      length_us;                          /**< The radio timeslot length (in the range [100..100,000] microseconds). */
} xh_radio_request_normal_t;

/**@brief Radio request parameters. */
typedef struct
{
  uint8_t                         request_type;     /**< Type of request, see @ref XH_RADIO_REQUEST_TYPE. */
  union
  {
    xh_radio_request_earliest_t  earliest;         /**< Parameters for a request for a timeslot as early as possible. */
    xh_radio_request_normal_t    normal;           /**< Parameters for a normal radio request. */
  } params;
} xh_radio_request_t;

/**@brief Return parameters of the radio timeslot signal callback. */
typedef struct
{
  uint8_t               callback_action;            /**< The action requested by the application when returning from the signal callback, see @ref XH_RADIO_SIGNAL_CALLBACK_ACTION. */
  union
  {
    struct
    {
      xh_radio_request_t * p_next;                 /**< The request parameters for the next radio timeslot. */
    } request;                                      /**< Additional parameters for return_code @ref XH_RADIO_SIGNAL_CALLBACK_ACTION_REQUEST_AND_END. */
    struct
    {
      uint32_t              length_us;              /**< Requested extension of the timeslot duration (microseconds) (for minimum time see @ref XH_RADIO_MINIMUM_TIMESLOT_LENGTH_EXTENSION_TIME_US). */
    } extend;                                       /**< Additional parameters for return_code @ref XH_RADIO_SIGNAL_CALLBACK_ACTION_EXTEND. */
  } params;
} xh_radio_signal_callback_return_param_t;

/**@brief The radio signal callback type.
 *
 * @note In case of invalid return parameters, the radio timeslot will automatically end
 *       immediately after returning from the signal callback and the
 *       @ref XH_EVT_RADIO_SIGNAL_CALLBACK_INVALID_RETURN event will be sent.
 * @note The returned struct pointer must remain valid after the signal callback
 *       function returns. For instance, this means that it must not point to a stack variable.
 *
 * @param[in] signal_type Type of signal, see @ref XH_RADIO_CALLBACK_SIGNAL_TYPE.
 *
 * @return Pointer to structure containing action requested by the application.
 */
typedef xh_radio_signal_callback_return_param_t * (*xh_radio_signal_callback_t) (uint8_t signal_type);

/**@brief AES ECB data structure */
typedef struct
{
  uint8_t key[SOC_ECB_KEY_LENGTH];                  /**< Encryption key. */
  uint8_t cleartext[SOC_ECB_CLEARTEXT_LENGTH];      /**< Clear Text data. */
  uint8_t ciphertext[SOC_ECB_CIPHERTEXT_LENGTH];    /**< Cipher Text data. */
} xh_ecb_hal_data_t;

/**@} */

/**@addtogroup XH_SOC_FUNCTIONS Functions
 * @{ */

/**@brief Initialize a mutex.
 *
 * @param[in] p_mutex Pointer to the mutex to initialize.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_MUTEX_NEW, uint32_t, xh_mutex_new(xh_mutex_t * p_mutex));

/**@brief Attempt to acquire a mutex.
 *
 * @param[in] p_mutex Pointer to the mutex to acquire.
 *
 * @retval ::XH_SUCCESS The mutex was successfully acquired.
 * @retval ::XH_ERROR_SOC_MUTEX_ALREADY_TAKEN The mutex could not be acquired.
 */
SVCALL(SD_MUTEX_ACQUIRE, uint32_t, xh_mutex_acquire(xh_mutex_t * p_mutex));

/**@brief Release a mutex.
 *
 * @param[in] p_mutex Pointer to the mutex to release.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_MUTEX_RELEASE, uint32_t, xh_mutex_release(xh_mutex_t * p_mutex));

/**@brief Enable External Interrupt.
 * @note Corresponds to NVIC_EnableIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in] IRQn See the NVIC_EnableIRQ documentation in CMSIS.
 *
 * @retval ::XH_SUCCESS The interrupt was enabled.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE The interrupt is not available for the application.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED The interrupt has a priority not available for the application.
 */
SVCALL(SD_NVIC_ENABLEIRQ, uint32_t, xh_nvic_EnableIRQ(IRQn_Type IRQn));

/**@brief  Disable External Interrupt.
 * @note Corresponds to NVIC_DisableIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in] IRQn See the NVIC_DisableIRQ documentation in CMSIS.
 *
 * @retval ::XH_SUCCESS The interrupt was disabled.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE The interrupt is not available for the application.
 */
SVCALL(SD_NVIC_DISABLEIRQ, uint32_t, xh_nvic_DisableIRQ(IRQn_Type IRQn));

/**@brief  Get Pending Interrupt.
 * @note Corresponds to NVIC_GetPendingIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in]   IRQn          See the NVIC_GetPendingIRQ documentation in CMSIS.
 * @param[out]  p_pending_irq Return value from NVIC_GetPendingIRQ.
 *
 * @retval ::XH_SUCCESS The interrupt is available for the application.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE IRQn is not available for the application.
 */
SVCALL(SD_NVIC_GETPENDINGIRQ, uint32_t, xh_nvic_GetPendingIRQ(IRQn_Type IRQn, uint32_t * p_pending_irq));

/**@brief  Set Pending Interrupt.
 * @note Corresponds to NVIC_SetPendingIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in] IRQn See the NVIC_SetPendingIRQ documentation in CMSIS.
 *
 * @retval ::XH_SUCCESS The interrupt is set pending.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE IRQn is not available for the application.
 */
SVCALL(SD_NVIC_SETPENDINGIRQ, uint32_t, xh_nvic_SetPendingIRQ(IRQn_Type IRQn));

/**@brief  Clear Pending Interrupt.
 * @note Corresponds to NVIC_ClearPendingIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in] IRQn See the NVIC_ClearPendingIRQ documentation in CMSIS.
 *
 * @retval ::XH_SUCCESS The interrupt pending flag is cleared.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE IRQn is not available for the application.
 */
SVCALL(SD_NVIC_CLEARPENDINGIRQ, uint32_t, xh_nvic_ClearPendingIRQ(IRQn_Type IRQn));

/**@brief Set Interrupt Priority.
 * @note Corresponds to NVIC_SetPriority in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 * @pre Priority is valid and not reserved by the stack.
 *
 * @param[in] IRQn      See the NVIC_SetPriority documentation in CMSIS.
 * @param[in] priority  A valid IRQ priority for use by the application.
 *
 * @retval ::XH_SUCCESS The interrupt and priority level is available for the application.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE IRQn is not available for the application.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED The interrupt priority is not available for the application.
 */
SVCALL(SD_NVIC_SETPRIORITY, uint32_t, xh_nvic_SetPriority(IRQn_Type IRQn, xh_app_irq_priority_t priority));

/**@brief Get Interrupt Priority.
 * @note Corresponds to NVIC_GetPriority in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in]  IRQn         See the NVIC_GetPriority documentation in CMSIS.
 * @param[out] p_priority   Return value from NVIC_GetPriority.
 *
 * @retval ::XH_SUCCESS The interrupt priority is returned in p_priority.
 * @retval ::XH_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE - IRQn is not available for the application.
 */
SVCALL(SD_NVIC_GETPRIORITY, uint32_t, xh_nvic_GetPriority(IRQn_Type IRQn, xh_app_irq_priority_t * p_priority));

/**@brief System Reset.
 * @note Corresponds to NVIC_SystemReset in CMSIS.
 *
 * @retval ::XH_ERROR_SOC_NVIC_SHOULD_NOT_RETURN
 */
SVCALL(SD_NVIC_SYSTEMRESET, uint32_t, xh_nvic_SystemReset(void));

/**@brief Enters critical region.
 *
 * @post Application interrupts will be disabled.
 * @sa xh_nvic_critical_region_exit
 *
 * @param[out]  p_is_nested_critical_region  1: If in a nested critical region.
 *                                           0: Otherwise.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_NVIC_CRITICAL_REGION_ENTER, uint32_t, xh_nvic_critical_region_enter(uint8_t * p_is_nested_critical_region));

/**@brief Exit critical region.
 *
 * @pre Application has entered a critical region using ::xh_nvic_critical_region_enter.
 * @post If not in a nested critical region, the application interrupts will restored to the state before ::xh_nvic_critical_region_enter was called. 
 *
 * @param[in] is_nested_critical_region If this is set to 1, the critical region won't be exited. @sa xh_nvic_critical_region_enter.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_NVIC_CRITICAL_REGION_EXIT, uint32_t, xh_nvic_critical_region_exit(uint8_t is_nested_critical_region));

/**@brief Query the capacity of the application random pool.
 *
 * @param[out] p_pool_capacity The capacity of the pool.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_RAND_APPLICATION_POOL_CAPACITY, uint32_t, xh_rand_application_pool_capacity_get(uint8_t * p_pool_capacity));

/**@brief Get number of random bytes available to the application.
 *
 * @param[out] p_bytes_available The number of bytes currently available in the pool.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_RAND_APPLICATION_BYTES_AVAILABLE, uint32_t, xh_rand_application_bytes_available_get(uint8_t * p_bytes_available));

/**@brief Get random bytes from the application pool.
 *
 * @param[out]  p_buff  Pointer to unit8_t buffer for storing the bytes.
 * @param[in]   length  Number of bytes to take from pool and place in p_buff.
 *
 * @retval ::XH_SUCCESS The requested bytes were written to p_buff.
 * @retval ::XH_ERROR_SOC_RAND_NOT_ENOUGH_VALUES No bytes were written to the buffer, because there were not enough bytes available.
*/
SVCALL(SD_RAND_APPLICATION_GET_VECTOR, uint32_t, xh_rand_application_vector_get(uint8_t * p_buff, uint8_t length));

/**@brief Gets the reset reason register. 
 *
 * @param[out]  p_reset_reason  Contents of the XH_POWER->RESETREAS register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_RESET_REASON_GET, uint32_t, xh_power_reset_reason_get(uint32_t * p_reset_reason));

/**@brief Clears the bits of the reset reason register. 
 *
 * @param[in] reset_reason_clr_msk Contains the bits to clear from the reset reason register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_RESET_REASON_CLR, uint32_t, xh_power_reset_reason_clr(uint32_t reset_reason_clr_msk));

/**@brief Sets the power mode when in CPU sleep.
 *
 * @param[in] power_mode The power mode to use when in CPU sleep. @sa xh_app_evt_wait
 *
 * @retval ::XH_SUCCESS The power mode was set.
 * @retval ::XH_ERROR_SOC_POWER_MODE_UNKNOWN The power mode was unknown.
 */
SVCALL(SD_POWER_MODE_SET, uint32_t, xh_power_mode_set(xh_power_mode_t power_mode));

/**@brief Puts the chip in System OFF mode. 
 *
 * @retval ::XH_ERROR_SOC_POWER_OFF_SHOULD_NOT_RETURN
 */
SVCALL(SD_POWER_SYSTEM_OFF, uint32_t, xh_power_system_off(void));

/**@brief Enables or disables the power-fail comparator.
 *
 * Enabling this will give a softdevice event (XH_EVT_POWER_FAILURE_WARNING) when the power failure warning occurs.
 * The event can be retrieved with xh_evt_get();
 *
 * @param[in] pof_enable    True if the power-fail comparator should be enabled, false if it should be disabled.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_POF_ENABLE, uint32_t, xh_power_pof_enable(uint8_t pof_enable));

/**@brief Sets the power-fail threshold value.
 *
 * @param[in] threshold The power-fail threshold value to use.
 *
 * @retval ::XH_SUCCESS The power failure threshold was set.
 * @retval ::XH_ERROR_SOC_POWER_POF_THRESHOLD_UNKNOWN The power failure threshold is unknown.
 */
SVCALL(SD_POWER_POF_THRESHOLD_SET, uint32_t, xh_power_pof_threshold_set(xh_power_failure_threshold_t threshold));

/**@brief Sets bits in the XH_POWER->RAMON register.
 *
 * @param[in] ramon Contains the bits needed to be set in the XH_POWER->RAMON register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_RAMON_SET, uint32_t, xh_power_ramon_set(uint32_t ramon));

/**@brief Clears bits in the XH_POWER->RAMON register.
 *
 * @param ramon Contains the bits needed to be cleared in the XH_POWER->RAMON register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_RAMON_CLR, uint32_t, xh_power_ramon_clr(uint32_t ramon));

/**@brief Get contents of XH_POWER->RAMON register, indicates power status of ram blocks.
 *
 * @param[out] p_ramon Content of XH_POWER->RAMON register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_RAMON_GET, uint32_t, xh_power_ramon_get(uint32_t * p_ramon));

/**@brief Set bits in the XH_POWER->GPREGRET register.
 *
 * @param[in] gpregret_msk Bits to be set in the GPREGRET register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_GPREGRET_SET, uint32_t, xh_power_gpregret_set(uint32_t gpregret_msk));

/**@brief Clear bits in the XH_POWER->GPREGRET register.
 *
 * @param[in] gpregret_msk Bits to be clear in the GPREGRET register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_GPREGRET_CLR, uint32_t, xh_power_gpregret_clr(uint32_t gpregret_msk));

/**@brief Get contents of the XH_POWER->GPREGRET register.
 *
 * @param[out] p_gpregret Contents of the GPREGRET register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_POWER_GPREGRET_GET, uint32_t, xh_power_gpregret_get(uint32_t *p_gpregret));

/**@brief Sets the DCDC mode.
 *
 * This function is to enable or disable the DCDC periperhal.
 *
 * @param[in] dcdc_mode The mode of the DCDC.
 *
 * @retval ::XH_SUCCESS
 * @retval ::XH_ERROR_INVALID_PARAM The DCDC mode is invalid.
 */
SVCALL(SD_POWER_DCDC_MODE_SET, uint32_t, xh_power_dcdc_mode_set(xh_power_dcdc_mode_t dcdc_mode));

/**@brief Request the high frequency crystal oscillator.
 *
 * Will start the high frequency crystal oscillator, the startup time of the crystal varies
 * and the ::xh_clock_hfclk_is_running function can be polled to check if it has started.
 *
 * @see xh_clock_hfclk_is_running
 * @see xh_clock_hfclk_release
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_CLOCK_HFCLK_REQUEST, uint32_t, xh_clock_hfclk_request(void));

/**@brief Releases the high frequency crystal oscillator.
 *
 * Will stop the high frequency crystal oscillator, this happens immediately.
 *
 * @see xh_clock_hfclk_is_running
 * @see xh_clock_hfclk_request
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_CLOCK_HFCLK_RELEASE, uint32_t, xh_clock_hfclk_release(void));

/**@brief Checks if the high frequency crystal oscillator is running.
 *
 * @see xh_clock_hfclk_request
 * @see xh_clock_hfclk_release
 *
 * @param[out] p_is_running 1 if the external crystal oscillator is running, 0 if not.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_CLOCK_HFCLK_IS_RUNNING, uint32_t, xh_clock_hfclk_is_running(uint32_t * p_is_running));

/**@brief Waits for an application event.
 * 
 * An application event is either an application interrupt or a pended interrupt when the
 * interrupt is disabled. When the interrupt is enabled it will be taken immediately since
 * this function will wait in thread mode, then the execution will return in the application's
 * main thread. When an interrupt is disabled and gets pended it will return to the application's 
 * thread main. The application must ensure that the pended flag is cleared using 
 * ::xh_nvic_ClearPendingIRQ in order to sleep using this function. This is only necessary for
 * disabled interrupts, as the interrupt handler will clear the pending flag automatically for
 * enabled interrupts.
 *
 * In order to wake up from disabled interrupts, the SEVONPEND flag has to be set in the Cortex-M0
 * System Control Register (SCR). @sa CMSIS_SCB
 *
 * @note If an application interrupt has happened since the last time xh_app_evt_wait was
 *       called this function will return immediately and not go to sleep. This is to avoid race
 *       conditions that can occur when a flag is updated in the interrupt handler and processed
 *       in the main loop.
 *
 * @post An application interrupt has happened or a interrupt pending flag is set.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_APP_EVT_WAIT, uint32_t, xh_app_evt_wait(void));

/**@brief Get PPI channel enable register contents.
 *
 * @param[out] p_channel_enable The contents of the PPI CHEN register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_PPI_CHANNEL_ENABLE_GET, uint32_t, xh_ppi_channel_enable_get(uint32_t * p_channel_enable));

/**@brief Set PPI channel enable register.
 *
 * @param[in] channel_enable_set_msk Mask containing the bits to set in the PPI CHEN register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_PPI_CHANNEL_ENABLE_SET, uint32_t, xh_ppi_channel_enable_set(uint32_t channel_enable_set_msk));

/**@brief Clear PPI channel enable register.
 *
 * @param[in] channel_enable_clr_msk Mask containing the bits to clear in the PPI CHEN register.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_PPI_CHANNEL_ENABLE_CLR, uint32_t, xh_ppi_channel_enable_clr(uint32_t channel_enable_clr_msk));

/**@brief Assign endpoints to a PPI channel.
 *
 * @param[in] channel_num Number of the PPI channel to assign.
 * @param[in] evt_endpoint Event endpoint of the PPI channel.
 * @param[in] task_endpoint Task endpoint of the PPI channel.
 *
 * @retval ::XH_ERROR_SOC_PPI_INVALID_CHANNEL The channel number is invalid.
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_PPI_CHANNEL_ASSIGN, uint32_t, xh_ppi_channel_assign(uint8_t channel_num, const volatile void * evt_endpoint, const volatile void * task_endpoint));

/**@brief Task to enable a channel group.
 *
 * @param[in] group_num Number of the channel group.
 *
 * @retval ::XH_ERROR_SOC_PPI_INVALID_GROUP The group number is invalid
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_PPI_GROUP_TASK_ENABLE, uint32_t, xh_ppi_group_task_enable(uint8_t group_num));

/**@brief Task to disable a channel group.
 *
 * @param[in] group_num Number of the PPI group.
 *
 * @retval ::XH_ERROR_SOC_PPI_INVALID_GROUP The group number is invalid.
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_PPI_GROUP_TASK_DISABLE, uint32_t, xh_ppi_group_task_disable(uint8_t group_num));

/**@brief Assign PPI channels to a channel group.
 *
 * @param[in] group_num Number of the channel group.
 * @param[in] channel_msk Mask of the channels to assign to the group.
 *
 * @retval ::XH_ERROR_SOC_PPI_INVALID_GROUP The group number is invalid.
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_PPI_GROUP_ASSIGN, uint32_t, xh_ppi_group_assign(uint8_t group_num, uint32_t channel_msk));

/**@brief Gets the PPI channels of a channel group.
 *
 * @param[in]   group_num Number of the channel group.
 * @param[out]  p_channel_msk Mask of the channels assigned to the group.
 *
 * @retval ::XH_ERROR_SOC_PPI_INVALID_GROUP The group number is invalid.
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_PPI_GROUP_GET, uint32_t, xh_ppi_group_get(uint8_t group_num, uint32_t * p_channel_msk));

/**@brief Configures the Radio Notification signal.
 *
 * @note
 *      - The notification signal latency depends on the interrupt priority settings of SWI used
 *        for notification signal.
 *      - To ensure that the radio notification signal behaves in a consistent way, always 
 *        configure radio notifications when there is no protocol stack or other SoftDevice 
 *        activity in progress. It is recommended that the radio notification signal is 
 *        configured directly after the SoftDevice has been enabled.
 *      - In the period between the ACTIVE signal and the start of the Radio Event, the SoftDevice
 *        will interrupt the application to do Radio Event preparation.
 *      - Using the Radio Notification feature may limit the bandwidth, as the SoftDevice may have
 *        to shorten the connection events to have time for the Radio Notification signals.
 *
 * @param[in]  type      Type of notification signal.
 *                       @ref XH_RADIO_NOTIFICATION_TYPE_NONE shall be used to turn off radio
 *                       notification. Using @ref XH_RADIO_NOTIFICATION_DISTANCE_NONE is
 *                       recommended (but not required) to be used with
 *                       @ref XH_RADIO_NOTIFICATION_TYPE_NONE.
 *
 * @param[in]  distance  Distance between the notification signal and start of radio activity.
 *                       This parameter is ignored when @ref XH_RADIO_NOTIFICATION_TYPE_NONE or 
 *                       @ref XH_RADIO_NOTIFICATION_TYPE_INT_ON_INACTIVE is used.
 *
 * @retval ::XH_ERROR_INVALID_PARAM The group number is invalid.
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_RADIO_NOTIFICATION_CFG_SET, uint32_t, xh_radio_notification_cfg_set(xh_radio_notification_type_t type, xh_radio_notification_distance_t distance));

/**@brief Encrypts a block according to the specified parameters.
 *
 * 128-bit AES encryption.
 *
 * @param[in, out] p_ecb_data Pointer to the ECB parameters' struct (two input
 *                            parameters and one output parameter).
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_ECB_BLOCK_ENCRYPT, uint32_t, xh_ecb_block_encrypt(xh_ecb_hal_data_t * p_ecb_data));

/**@brief Gets any pending events generated by the SoC API.
 *
 * The application should keep calling this function to get events, until ::XH_ERROR_NOT_FOUND is returned.
 *
 * @param[out] p_evt_id Set to one of the values in @ref XH_SOC_EVTS, if any events are pending.
 *
 * @retval ::XH_SUCCESS An event was pending. The event id is written in the p_evt_id parameter.
 * @retval ::XH_ERROR_NOT_FOUND No pending events. 
 */
SVCALL(SD_EVT_GET, uint32_t, xh_evt_get(uint32_t * p_evt_id));

/**@brief Get the temperature measured on the chip
 * 
 * This function will block until the temperature measurement is done.
 * It takes around 50us from call to return.
 *
 * @note Pan #28 in PAN-028 v 1.6 "Negative measured values are not represented correctly" is corrected by this function.
 *
 * @param[out] p_temp Result of temperature measurement. Die temperature in 0.25 degrees celsius.
 *
 * @retval ::XH_SUCCESS A temperature measurement was done, and the temperature was written to temp
 */
SVCALL(SD_TEMP_GET, uint32_t, xh_temp_get(int32_t * p_temp));

/**@brief Flash Write
*
* Commands to write a buffer to flash
*
* If the SoftDevice is enabled:
*  This call initiates the flash access command, and its completion will be communicated to the
*  application with exactly one of the following events:
*      - @ref XH_EVT_FLASH_OPERATION_SUCCESS - The command was successfully completed.
*      - @ref XH_EVT_FLASH_OPERATION_ERROR   - The command could not be started.
*
* If the SoftDevice is not enabled no event will be generated, and this call will return @ref XH_SUCCESS when the 
 * write has been completed
*
* @note
*      - This call takes control over the radio and the CPU during flash erase and write to make sure that
*        they will not interfere with the flash access. This means that all interrupts will be blocked
*        for a predictable time (depending on the NVMC specification in XH51 Series Reference Manual
*        and the command parameters).
*
*
* @param[in]  p_dst Pointer to start of flash location to be written.
* @param[in]  p_src Pointer to buffer with data to be written.
* @param[in]  size  Number of 32-bit words to write. Maximum size is 256 32bit words.
*
* @retval ::XH_ERROR_INVALID_ADDR   Tried to write to a non existing flash address, or p_dst or p_src was unaligned.
* @retval ::XH_ERROR_BUSY           The previous command has not yet completed.
* @retval ::XH_ERROR_INVALID_LENGTH Size was 0, or more than 256 words.
* @retval ::XH_ERROR_FORBIDDEN      Tried to write to or read from protected location.
* @retval ::XH_SUCCESS              The command was accepted.
*/
SVCALL(SD_FLASH_WRITE, uint32_t, xh_flash_write(uint32_t * const p_dst, uint32_t const * const p_src, uint32_t size));


/**@brief Flash Erase page
*
* Commands to erase a flash page
* If the SoftDevice is enabled:
*  This call initiates the flash access command, and its completion will be communicated to the
*  application with exactly one of the following events:
*      - @ref XH_EVT_FLASH_OPERATION_SUCCESS - The command was successfully completed.
*      - @ref XH_EVT_FLASH_OPERATION_ERROR   - The command could not be started.
*
* If the SoftDevice is not enabled no event will be generated, and this call will return @ref XH_SUCCESS when the 
* erase has been completed
*
* @note
*      - This call takes control over the radio and the CPU during flash erase and write to make sure that
*        they will not interfere with the flash access. This means that all interrupts will be blocked
*        for a predictable time (depending on the NVMC specification in XH51 Series Reference Manual
*        and the command parameters).
*
*
* @param[in]  page_number Pagenumber of the page to erase
* @retval ::XH_ERROR_INTERNAL      If a new session could not be opened due to an internal error.
* @retval ::XH_ERROR_INVALID_ADDR  Tried to erase to a non existing flash page.
* @retval ::XH_ERROR_BUSY          The previous command has not yet completed.
* @retval ::XH_ERROR_FORBIDDEN     Tried to erase a protected page.
* @retval ::XH_SUCCESS             The command was accepted.
*/
SVCALL(SD_FLASH_PAGE_ERASE, uint32_t, xh_flash_page_erase(uint32_t page_number));


/**@brief Flash Protection set
 *
 * Commands to set the flash protection registers PROTENSETx
 *
 * @note To read the values in PROTENSETx you can read them directly. They are only write-protected.
 *
 * @param[in]  protenset0 Value to be written to PROTENSET0.
 * @param[in]  protenset1 Value to be written to PROTENSET1.
 *
 * @retval ::XH_ERROR_FORBIDDEN Tried to protect the SoftDevice.
 * @retval ::XH_SUCCESS Values successfully written to PROTENSETx.
 */
SVCALL(SD_FLASH_PROTECT, uint32_t, xh_flash_protect(uint32_t protenset0, uint32_t protenset1));

/**@brief Opens a session for radio requests.
 *
 * @note Only one session can be open at a time.
 * @note p_radio_signal_callback(@ref XH_RADIO_CALLBACK_SIGNAL_TYPE_START) will be called when the radio timeslot
 *       starts. From this point the XH_RADIO and XH_TIMER0 peripherals can be freely accessed
 *       by the application.
 * @note p_radio_signal_callback(@ref XH_RADIO_CALLBACK_SIGNAL_TYPE_TIMER0) is called whenever the XH_TIMER0
 *       interrupt occurs.
 * @note p_radio_signal_callback(@ref XH_RADIO_CALLBACK_SIGNAL_TYPE_RADIO) is called whenever the XH_RADIO
 *       interrupt occurs.
 * @note p_radio_signal_callback() will be called at ARM interrupt priority level 0. This
 *       implies that none of the xh_* API calls can be used from p_radio_signal_callback().
 *
 * @param[in] p_radio_signal_callback The signal callback.
 *
 * @retval ::XH_ERROR_INVALID_ADDR p_radio_signal_callback is an invalid function pointer.
 * @retval ::XH_ERROR_BUSY If session cannot be opened.
 * @retval ::XH_ERROR_INTERNAL If a new session could not be opened due to an internal error.
 * @retval ::XH_SUCCESS Otherwise.
 */
 SVCALL(SD_RADIO_SESSION_OPEN, uint32_t, xh_radio_session_open(xh_radio_signal_callback_t p_radio_signal_callback));

/**@brief Closes a session for radio requests.
 *
 * @note Any current radio timeslot will be finished before the session is closed.
 * @note If a radio timeslot is scheduled when the session is closed, it will be canceled.
 * @note The application cannot consider the session closed until the @ref XH_EVT_RADIO_SESSION_CLOSED
 *       event is received.
 *
 * @retval ::XH_ERROR_FORBIDDEN If session not opened.
 * @retval ::XH_ERROR_BUSY If session is currently being closed.
 * @retval ::XH_SUCCESS Otherwise.
 */
 SVCALL(SD_RADIO_SESSION_CLOSE, uint32_t, xh_radio_session_close(void));

/**@brief Requests a radio timeslot.
 *
 * @note The request type is determined by p_request->request_type, and can be one of @ref XH_RADIO_REQ_TYPE_EARLIEST
 *       and @ref XH_RADIO_REQ_TYPE_NORMAL. The first request in a session must always be of type @ref
 *       XH_RADIO_REQ_TYPE_EARLIEST.
 * @note For a normal request (@ref XH_RADIO_REQ_TYPE_NORMAL), the start time of a radio timeslot is specified by
 *       p_request->distance_us and is given relative to the start of the previous timeslot. 
 * @note A too small p_request->distance_us will lead to a @ref XH_EVT_RADIO_BLOCKED event.
 * @note Timeslots scheduled too close will lead to a @ref XH_EVT_RADIO_BLOCKED event.
 * @note See the SoftDevice Specification for more on radio timeslot scheduling, distances and lengths.
 * @note If an opportunity for the first radio timeslot is not found before 100ms after the call to this
 *       function, it is not scheduled, and instead a @ref XH_EVT_RADIO_BLOCKED event is sent.
 *       The application may then try to schedule the first radio timeslot again.
 * @note Successful requests will result in xh_radio_signal_callback_t(@ref XH_RADIO_CALLBACK_SIGNAL_TYPE_START).
 *       Unsuccessful requests will result in a @ref XH_EVT_RADIO_BLOCKED event, see @ref XH_SOC_EVTS.
 * @note The jitter in the start time of the radio timeslots is +/- @ref XH_RADIO_START_JITTER_US us.
 * @note The xh_radio_signal_callback_t(@ref XH_RADIO_CALLBACK_SIGNAL_TYPE_START) call has a latency relative to the
 *       specified radio timeslot start, but this does not affect the actual start time of the timeslot.
 * @note XH_TIMER0 is reset at the start of the radio timeslot, and is clocked at 1MHz from the high frequency
 *       (16 MHz) clock source. If p_request->hfclk_force_xtal is true, the high frequency clock is 
 *       guaranteed to be clocked from the external crystal.
 * @note The SoftDevice will neither access the XH_RADIO peripheral nor the XH_TIMER0 peripheral
 *       during the radio timeslot.
 *
 * @param[in] p_request Pointer to the request parameters.
 *
 * @retval ::XH_ERROR_FORBIDDEN If session not opened or the session is not IDLE.
 * @retval ::XH_ERROR_INVALID_ADDR If the p_request pointer is invalid.
 * @retval ::XH_ERROR_INVALID_PARAM If the parameters of p_request are not valid.
 * @retval ::XH_SUCCESS Otherwise.
 */
 SVCALL(SD_RADIO_REQUEST, uint32_t, xh_radio_request(xh_radio_request_t * p_request ));

/**@} */

#endif // XH_SOC_H__

/**@} */
