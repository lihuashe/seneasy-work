/**
 * @file cfg.h
 * @brief Configuration system
 * @date Mon 13 Apr 2015 10:46:17 AM CST
 * @author luwei
 *
 * @defgroup CONFIG Configuration
 * @ingroup XH98xx
 * @brief Configuration system
 * @details
 *
 * Just to be compatible with the old version of the config system
 *
 * @{
 *
 * @example example_configuration.c
 * This is an example of how to use the configuration on storage.
 *
 */

#ifndef __CFG_H__
#define __CFG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include "nvds.h"
/*********************************************************************
 * MACROS
 */

/// @cond

/// Configuration section define for system section
#define CFG_SECTION_SYS             1
/// Configuration section define for user section
#define CFG_SECTION_USR             2
/// Max tag length
#define CFG_TAG_LENGTH_MAX          NVDS_LEN_USER_MAX

// Configration tag define for section: CFG_SECTION_SYS
#define CFG_TAG_BDADDR              NVDS_TAG_BD_ADDRESS
#define CFG_TAG_BDNAME              NVDS_TAG_DEVICE_NAME
#define CFG_TAG_SCA                 NVDS_TAG_SCA
#define CFG_TAG_XTAL32M_CTUNE       NVDS_TAG_XTAL32M_CTUNE
#define CFG_TAG_XTAL32K_CTUNE       NVDS_TAG_XTAL32K_CTUNE
#define CFG_TAG_FREQ_OFFSET         NVDS_TAG_FREQ_OFFSET
#define CFG_TAG_DCDC_ENABLE         NVDS_TAG_DCDC_ENABLE
#define CFG_TAG_PRE_WAKEUP_TIME     NVDS_TAG_PRE_WAKEUP_TIME

// Configration tag length define for section: CFG_SECTION_SYS
#define CFG_TAG_BDADDR_LEN          NVDS_LEN_BD_ADDRESS
#define CFG_TAG_BDNAME_LEN          NVDS_LEN_DEVICE_NAME
#define CFG_TAG_SCA_LEN             NVDS_LEN_SCA

// Function macro, compatible with cfg interfaces
#define cfg_init(base_addr, length)             nvds_init(base_addr)
#define cfg_reset(section)                      nvds_reset()
#define cfg_put(section, tag, pdata, len)       nvds_put(((section)==CFG_SECTION_USR) ? ((tag)+NVDS_TAG_USER_BEGIN) : (tag), len,  pdata)
#define cfg_get(section, tag, pdata, plen)      nvds_get(((section)==CFG_SECTION_USR) ? ((tag)+NVDS_TAG_USER_BEGIN) : (tag), plen, pdata)
#define cfg_del(section, tag)                   nvds_del(((section)==CFG_SECTION_USR) ? ((tag)+NVDS_TAG_USER_BEGIN) : (tag))
#define cfg_dump(printf_dump_func)              nvds_dump(printf_dump_func)
/// @endcond

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERN VARIABLES
 */

/*********************************************************************
 * EXTERN FUNCTIONS
 */

#ifdef __cplusplus
}
#endif

#endif

/** @} */


