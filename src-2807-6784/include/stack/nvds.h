/**
 * @file nvds.h
 * @brief Non Volatile Data Storage (NVDS) system
 * @date 10 Dec 2020
 * @author wangyc
 *
 * @defgroup NVDS NVDS
 * @ingroup XH98xx
 * @brief NVDS system
 * @details 
 *
 * The NVDS system is used for storing information
 * which should be saved when chip loses power.
 *
 * <pre>
 * Features:
 *   • Two Sectors: one is NVDS sector, the othor one is BKUP sector
 *
 * </pre>
 *
 * @{
 *
 * @example example_nvds.c
 * This is an example of how to use the NVDS on storage.
 *
 */
#ifndef _NVDS_H_
#define _NVDS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h>           
#include <stdint.h>            
#include <stdio.h>
#include "nvds_tags.h"         
#include "peripheral.h"
/*
 * DEFINES
 ****************************************************************************************
 */
/// Maximum number of tags
#define NVDS_MAX_NUM_OF_TAGS        255

/// NVDS tag length type
typedef uint16_t nvds_tag_len_t;
/// NVDS address type
typedef uint32_t nvds_addr_len_t;
/// NVDS tag ID type
typedef enum NVDS_TAGS nvds_tag_id_t;

/*
 * ENUMERATION DEFINITIONS
 ****************************************************************************************
 */

/// Possible Returned Status
enum NVDS_STATUS
{
    /// NVDS status OK
    NVDS_OK,
    /// Generic NVDS status KO
    NVDS_FAIL,
    /// NVDS TAG unrecognized
    NVDS_TAG_NOT_DEFINED,
    /// No space for NVDS
    NVDS_NO_SPACE_AVAILABLE,
    /// Length violation
    NVDS_LENGTH_OUT_OF_RANGE,
    /// NVDS corrupted
    NVDS_CORRUPT,
    /// NVDS setup error
    NVDS_SETUP_ERROR,
    /// NVDS crc error
    NVDS_CRC_ERROR
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 * @brief NVDS initialize
 *
 * @param[in] flash_base  The NVDS's base address in flash
 *
 * @return errno
 **/
uint8_t nvds_init(nvds_addr_len_t flash_base);

/**
 * @brief Get the newest tag from NVDS
 *
 * @param[in] tag       tag index
 * @param[in] length    pdata buffer length, if it is null, ignore it.
 * @param[out] length'  real readed length, if length(in) > length(real),
 * @param[out] buf      A pointer to the buffer allocated by the caller to be filled with
 *                      the DATA part of the TAG
 *
 * @return errno
 **/
uint8_t nvds_get(nvds_tag_id_t tag, nvds_tag_len_t *length, void *buf);

/**
 * @brief Delete the specific tag from NVDS
 *
 * @param[in] tag  tag index
 *
 * @return errno
 **/
uint8_t nvds_del(nvds_tag_id_t tag);

/**
 * @brief Put tag to NVDS
 *
 * @param[in] tag       tag index
 * @param[in] length    data length, excluding tag node size, range in [1, 504].
 * @param[in] buf       Pointer to the buffer containing the DATA part of the TAG to add to
 *                      the NVDS
 *
 * @return errno
 **/
uint8_t nvds_put(nvds_tag_id_t tag, nvds_tag_len_t length, const void *buf);


/**
 * @brief Print debug information
 *
 * @param[in] printf_dump_func 
 *
 * @return None 
 **/
void nvds_dump(void *printf_dump_func);

/**
 * @brief Reset all sectors
 *
 * @return None
 **/
void nvds_reset(void);


/// @} NVDS
#ifdef __cplusplus
}
#endif

#endif // _NVDS_H_
