/**
 * @file gpio.h
 * @brief gpio driver
 * @date Wed 31 May 2017 07:15:14 PM CST
 * @author XIHAO
 *
 * @defgroup EFUSE EFUSE
 * @ingroup PERIPHERAL
 * @brief EFUSE driver
 * @details EFUSE dirver
 *
 * The XH98xx has up to 256bits EFUSE.
 *
 * @{
 */

#ifndef __EFUSE_H__
#define __EFUSE_H__

#ifdef __cplusplus
extern "C"
{ /*}*/
#endif

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief  efuse open
 **/
void efuse_open(void);

/**
 * @brief  efuse close
 **/
void efuse_close(void);

/**
 * @brief  opt write (2.25v +)
 *
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void efuse_write(uint32_t addr, const void *data, uint32_t length);

/**
 * @brief  opt read
 *
 * @param[in] addr  address. Must be 4bytes aign
 * @param[in] data  data
 * @param[in] length  length. Must be 4bytes aign
 **/
void efuse_read(uint32_t addr, void *data, uint32_t length);

#ifdef __cplusplus
/*{*/ }
#endif

#endif

/** @} */

