/**
  @defgroup xh_sdm_api SoftDevice Manager API
  @{
     
  @brief APIs for SoftDevice management.
 
*/

/* Header guard */
#ifndef XH_SDM_H__
#define XH_SDM_H__

#include "xh_svc.h"
#include "XH51.h"
#include "xh_error_sdm.h"

/** @addtogroup XH_SDM_DEFINES Defines
 * @{ */

/** @brief SoftDevice Manager SVC Base number. */
#define SDM_SVC_BASE 0x10   

/** @} */

/** @brief Defines the SoftDevice Information Structure location (address) as an offset from 
the start of the softdevice (without MBR)*/
#define SOFTDEVICE_INFO_STRUCT_OFFSET (0x2000)

/** @brief Defines the usual size reserverd for the MBR when a softdevice is written to flash. 
This is the offset where the first byte of the softdevice hex file is written.*/
#define MBR_SIZE (0x1000)

/** @brief Defines the absolute Softdevice information structure location (address)*/
#define SOFTDEVICE_INFO_STRUCT_ADDRESS (SOFTDEVICE_INFO_STRUCT_OFFSET + MBR_SIZE)

/** @brief Defines the offset for Softdevice size value relative to Softdevice base address*/
#define SD_SIZE_OFFSET (SOFTDEVICE_INFO_STRUCT_OFFSET + 0x08)

/** @brief Defines the offset for FWID value relative to Softdevice base address*/
#define SD_FWID_OFFSET (SOFTDEVICE_INFO_STRUCT_OFFSET + 0x0C)

/** @brief Defines a macro for retreiving the actual Softdevice size value from a given base address
           use @ref MBR_SIZE when Softdevice is installed just above the MBR (the usual case)*/
#define SD_SIZE_GET(baseaddr) (*((uint32_t *) ((baseaddr) + SD_SIZE_OFFSET)))

/** @brief Defines a macro for retreiving the actual FWID value from a given base address
           use @ref MBR_SIZE when Softdevice is installed just above the MBR (the usual case)*/
#define SD_FWID_GET(baseaddr) ((*((uint32_t *) ((baseaddr) + SD_FWID_OFFSET))) & 0xFFFF)


/** @addtogroup XH_SDM_ENUMS Enumerations
 * @{ */

/**@brief xh SoftDevice Manager API SVC numbers. */
enum XH_SD_SVCS
{
  SD_SOFTDEVICE_ENABLE = SDM_SVC_BASE, /**< ::xh_softdevice_enable */
  SD_SOFTDEVICE_DISABLE,               /**< ::xh_softdevice_disable */
  SD_SOFTDEVICE_IS_ENABLED,            /**< ::xh_softdevice_is_enabled */
  SD_SOFTDEVICE_VECTOR_TABLE_BASE_SET, /**< ::xh_softdevice_vector_table_base_set */
  SVC_SDM_LAST                         /**< Placeholder for last SDM SVC */
};

/**@brief Possible lfclk oscillator sources. */
enum XH_CLOCK_LFCLKSRCS
{
  XH_CLOCK_LFCLKSRC_SYNTH_250_PPM,                       /**< LFCLK Synthesized from HFCLK.                    */
  XH_CLOCK_LFCLKSRC_XTAL_500_PPM,                        /**< LFCLK crystal oscillator 500 PPM accuracy.       */
  XH_CLOCK_LFCLKSRC_XTAL_250_PPM,                        /**< LFCLK crystal oscillator 250 PPM accuracy.       */
  XH_CLOCK_LFCLKSRC_XTAL_150_PPM,                        /**< LFCLK crystal oscillator 150 PPM accuracy.       */
  XH_CLOCK_LFCLKSRC_XTAL_100_PPM,                        /**< LFCLK crystal oscillator 100 PPM accuracy.       */
  XH_CLOCK_LFCLKSRC_XTAL_75_PPM,                         /**< LFCLK crystal oscillator 75 PPM accuracy.        */
  XH_CLOCK_LFCLKSRC_XTAL_50_PPM,                         /**< LFCLK crystal oscillator 50 PPM accuracy.        */
  XH_CLOCK_LFCLKSRC_XTAL_30_PPM,                         /**< LFCLK crystal oscillator 30 PPM accuracy.        */
  XH_CLOCK_LFCLKSRC_XTAL_20_PPM,                         /**< LFCLK crystal oscillator 20 PPM accuracy.        */
  XH_CLOCK_LFCLKSRC_RC_250_PPM_250MS_CALIBRATION,        /**< LFCLK RC oscillator, 250ms  calibration interval.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_500MS_CALIBRATION,        /**< LFCLK RC oscillator, 500ms  calibration interval.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_1000MS_CALIBRATION,       /**< LFCLK RC oscillator, 1000ms calibration interval.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_2000MS_CALIBRATION,       /**< LFCLK RC oscillator, 2000ms calibration interval.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_4000MS_CALIBRATION,       /**< LFCLK RC oscillator, 4000ms calibration interval.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION,       /**< LFCLK RC oscillator, 8000ms calibration interval.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_TEMP_1000MS_CALIBRATION,  /**< LFCLK RC oscillator. Temperature checked every 1000ms, if changed above a threshold, a calibration is done.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_TEMP_2000MS_CALIBRATION,  /**< LFCLK RC oscillator. Temperature checked every 2000ms, if changed above a threshold, a calibration is done.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_TEMP_4000MS_CALIBRATION,  /**< LFCLK RC oscillator. Temperature checked every 4000ms, if changed above a threshold, a calibration is done.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_TEMP_8000MS_CALIBRATION,  /**< LFCLK RC oscillator. Temperature checked every 8000ms, if changed above a threshold, a calibration is done.*/
  XH_CLOCK_LFCLKSRC_RC_250_PPM_TEMP_16000MS_CALIBRATION, /**< LFCLK RC oscillator. Temperature checked every 16000ms, if changed above a threshold, a calibration is done.*/
};

/** @} */

/** @addtogroup XH_SDM_TYPES Types
 * @{ */

/**@brief Type representing lfclk oscillator source. */
typedef uint32_t xh_clock_lfclksrc_t;


/**@brief SoftDevice Assertion Handler type.
 *
 * When an unexpected error occurs within the SoftDevice it will call the SoftDevice assertion handler callback.
 * The protocol stack will be in an undefined state when this happens and the only way to recover will be to
 * perform a reset, using e.g. CMSIS NVIC_SystemReset().
 *
 * @note This callback is executed in HardFault context, thus SVC functions cannot be called from the SoftDevice assert callback.
 *       
 * @param[in] pc The program counter of the failed assert.
 * @param[in] line_number Line number where the assert failed.
 * @param[in] file_name File name where the assert failed.
 */
typedef void (*softdevice_assertion_handler_t)(uint32_t pc, uint16_t line_number, const uint8_t * p_file_name);

/** @} */

/** @addtogroup XH_SDM_FUNCTIONS Functions
 * @{ */

/**@brief Enables the SoftDevice and by extension the protocol stack.
 *
 * Idempotent function to enable the SoftDevice.
 *
 * @note Some care must be taken if a low frequency clock source is already running when calling this function:
 *       If the LF clock has a different source then the one currently running, it will be stopped. Then, the new
 *       clock source will be started.
 *
 * @note This function has no effect when returning with an error.
 *
 * @post If return code is ::XH_SUCCESS 
 *       - SoC library and protocol stack APIs are made available.
 *       - A portion of RAM will be unavailable (see relevant SDS documentation).
 *       - Some peripherals will be unavailable or available only through the SoC API (see relevant SDS documentation).
 *       - Interrupts will not arrive from protected peripherals or interrupts.
 *       - xh_nvic_ functions must be used instead of CMSIS NVIC_ functions for reliable usage of the softdevice.
 *       - Interrupt latency may be affected by the SoftDevice  (see relevant SDS documentation).
 *       - Chosen low frequency clock source will be running.
 *
 * @param clock_source Low frequency clock source and accuracy. (Note: In the case of XTAL source, the PPM accuracy of the chosen clock source must be greater than or equal to the actual characteristics of your XTAL clock).
 * @param assertion_handler Callback for SoftDevice assertions.
 *
 * @retval ::XH_SUCCESS
 * @retval ::XH_ERROR_INVALID_STATE SoftDevice is already enabled, and the clock source and assertion handler cannot be updated.
 * @retval ::XH_ERROR_SDM_INCORRECT_INTERRUPT_CONFIGURATION SoftDeviceinterrupt is already enabled, or an enabled interrupt has an illegal priority level.
 * @retval ::XH_ERROR_SDM_LFCLK_SOURCE_UNKNOWN Unknown low frequency clock source selected.
 */
SVCALL(SD_SOFTDEVICE_ENABLE, uint32_t, xh_softdevice_enable(xh_clock_lfclksrc_t clock_source, softdevice_assertion_handler_t assertion_handler));

/**@brief Disables the SoftDevice and by extension the protocol stack.
 * 
 * Idempotent function to disable the SoftDevice.
 *
 * @post SoC library and protocol stack APIs are made unavailable.
 * @post All interrupts that was protected by the SoftDevice will be disabled and initialized to priority 0 (highest).
 * @post All peripherals used by the SoftDevice will be reset to default values.
 * @post All of RAM become available.
 * @post All interrupts are forwarded to the application.
 * @post LFCLK source chosen in ::xh_softdevice_enable will be left running.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_SOFTDEVICE_DISABLE, uint32_t, xh_softdevice_disable(void));

/**@brief Check if the SoftDevice is enabled.
 *
 * @param[out]  p_softdevice_enabled If the SoftDevice is enabled: 1 else 0.
 *
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_SOFTDEVICE_IS_ENABLED, uint32_t, xh_softdevice_is_enabled(uint8_t * p_softdevice_enabled));

/**@brief Sets the base address of the interrupt vector table for interrupts forwarded from the SoftDevice
 * 
 * This function is only intended to be called when a bootloader is enabled.
 *
 * @param[in] address The base address of the interrupt vector table for forwarded interrupts.
 
 * @retval ::XH_SUCCESS
 */
SVCALL(SD_SOFTDEVICE_VECTOR_TABLE_BASE_SET, uint32_t, xh_softdevice_vector_table_base_set(uint32_t address)); 

/** @} */

#endif // XH_SDM_H__

/**
  @}
*/
