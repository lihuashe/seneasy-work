/**
  @defgroup xh_mbr_api Master Boot Record API
  @{

  @brief APIs for updating SoftDevice and BootLoader

*/

/* Header guard */
#ifndef XH_MBR_H__
#define XH_MBR_H__

#include "xh_svc.h"
#include <stdint.h>


/** @addtogroup XH_MBR_DEFINES Defines
 * @{ */

/**@brief MBR SVC Base number. */
#define MBR_SVC_BASE 0x18   

/**@brief Page size in words. */
#define PAGE_SIZE_IN_WORDS 256
/** @} */

/** @addtogroup XH_MBR_ENUMS Enumerations
 * @{ */

/**@brief xh Master Boot Record API SVC numbers. */
enum XH_MBR_SVCS
{
  SD_MBR_COMMAND = MBR_SVC_BASE, /**< ::xh_mbr_command */
};

/**@brief Possible values for ::xh_mbr_command_t.command */
enum XH_MBR_COMMANDS
{
  SD_MBR_COMMAND_COPY_BL,               /**< Copy a new BootLoader. @see xh_mbr_command_copy_bl_t */
  SD_MBR_COMMAND_COPY_SD,               /**< Copy a new SoftDevice. @see ::xh_mbr_command_copy_xh_t*/
  SD_MBR_COMMAND_INIT_SD,               /**< Init forwarding interrupts to SD, and run reset function in SD*/
  SD_MBR_COMMAND_COMPARE,               /**< This command works like memcmp. @see ::xh_mbr_command_compare_t*/
  SD_MBR_COMMAND_VECTOR_TABLE_BASE_SET, /**< Start forwarding all exception to this address @see ::xh_mbr_command_vector_table_base_set_t*/
};

/** @} */

/** @addtogroup XH_MBR_TYPES Types
 * @{ */

/**@brief This command copies part of a new SoftDevice
 * The destination area is erased before copying.
 * If dst is in the middle of a flash page, that whole flash page will be erased.
 * If (dst+len) is in the middle of a flash page, that whole flash page will be erased.
 *
 * The user of this function is responsible for setting the PROTENSET registers.
 *
 * @retval ::XH_SUCCESS indicates that the contents of the memory blocks where copied correctly.
 * @retval ::XH_ERROR_INTERNAL indicates that the contents of the memory blocks where not verified correctly after copying.
 */
typedef struct
{
  uint32_t *src;  /**< Pointer to the source of data to be copied.*/
  uint32_t *dst;  /**< Pointer to the destination where the content is to be copied.*/
  uint32_t len;   /**< Number of 32 bit words to copy. Must be a multiple of PAGE_SIZE_IN_WORDS words.*/
} xh_mbr_command_copy_xh_t;


/**@brief This command works like memcmp, but takes the length in words.
 *
 * @retval ::XH_SUCCESS indicates that the contents of both memory blocks are equal.
 * @retval ::XH_ERROR_NULL indicates that the contents of the memory blocks are not equal.
 */
typedef struct
{
  uint32_t *ptr1; /**< Pointer to block of memory. */
  uint32_t *ptr2; /**< Pointer to block of memory. */
  uint32_t len;   /**< Number of 32 bit words to compare.*/
} xh_mbr_command_compare_t;


/**@brief This command copies a new BootLoader.
 *  With this command, destination of BootLoader is always the address written in XH_UICR->BOOTADDR.
 *
 *  Destination is erased by this function.
 *  If (destination+bl_len) is in the middle of a flash page, that whole flash page will be erased.
 *
 *  This function will use PROTENSET to protect the flash that is not intended to be written.
 *
 *  On Success, this function will not return. It will start the new BootLoader from reset-vector as normal.
 *
 * @retval ::XH_ERROR_INTERNAL indicates an internal error that should not happen.
 * @retval ::XH_ERROR_FORBIDDEN if XH_UICR->BOOTADDR is not set.
 * @retval ::XH_ERROR_INVALID_LENGTH if parameters attempts to read or write outside flash area.
 */
typedef struct
{
  uint32_t *bl_src;  /**< Pointer to the source of the Bootloader to be be copied.*/
  uint32_t bl_len;   /**< Number of 32 bit words to copy for BootLoader. */
} xh_mbr_command_copy_bl_t;

/**@brief Sets the base address of the interrupt vector table for interrupts forwarded from the MBR
 *
 * Once this function has been called, this address is where the MBR will start to forward interrupts to after a reset.
 *
 * To restore default forwarding this function should be called with @param address set to 0.
 * The MBR will then start forwarding to interrupts to the adress in NFR_UICR->BOOTADDR or to the SoftDevice if the BOOTADDR is not set.
 *
 * @retval ::XH_SUCCESS
 * @retval ::XH_ERROR_INTERNAL indicates an internal error that should not happen.
 * @retval ::XH_ERROR_INVALID_ADDR if parameter address is outside of the flash size.
 */
typedef struct
{
  uint32_t address; /**< The base address of the interrupt vector table for forwarded interrupts.*/
} xh_mbr_command_vector_table_base_set_t;

typedef struct
{
  uint32_t command;  /**< type of command to be issued see @ref XH_MBR_COMMANDS. */
  union
  {
    xh_mbr_command_copy_xh_t copy_sd;  /**< Parameters for copy SoftDevice.*/
    xh_mbr_command_copy_bl_t copy_bl;  /**< Parameters for copy BootLoader.*/
    xh_mbr_command_compare_t compare;  /**< Parameters for verify.*/
    xh_mbr_command_vector_table_base_set_t base_set; /**< Parameters for vector table base set.*/
  } params;
} xh_mbr_command_t;

/** @} */

/** @addtogroup XH_MBR_FUNCTIONS Functions
 * @{ */

/**@brief Issue Master Boot Record commands
 *
 * Commands used when updating a SoftDevice and bootloader.
 *
 * @param[in]  param Pointer to a struct describing the command.
 *
 *@note for retvals see ::xh_mbr_command_copy_xh_t ::xh_mbr_command_copy_bl_t ::xh_mbr_command_compare_t

*/
SVCALL(SD_MBR_COMMAND, uint32_t, xh_mbr_command(xh_mbr_command_t* param));

/** @} */
#endif // XH_MBR_H__

/**
  @}
*/
