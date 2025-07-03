/**
 * @file otp.h
 * @brief 
 * @date Wed 14 Oct 2020 03:03:24 PM CST
 * @author XIHAO
 *
 * @defgroup 
 * @ingroup 
 * @brief 
 * @details 
 *
 * @{
 */

#ifndef __OTP_H__
#define __OTP_H__

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

// OPT info
#define OTP_INFO                    ((otp_info_t *)(XH_BUS_OTP_NONCACHABLE_BASE + XH_BUS_OTP_SIZE - sizeof(otp_info_t)))

// Jlink Lock
#define OTP_JLINK_LOCK_MAGIC        0x1234ABCD
#define OTP_APP_INVALID_MAGIC       0x444c5649  // IVLD

// Bonding option
#define OTP_IS_BONDING()            (XH_SYS->REV_ID & SYS_REV_BONDING_OTP_MASK)

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uint32_t app_invalid_magic;
    uint32_t jlink_lock_magic;
}otp_info_t;

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief  otp open
 **/
void otp_open(void);

/**
 * @brief  otp close
 **/
void otp_close(void);

/**
 * @brief  opt write (2.25v +)
 *
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void otp_write(uint32_t addr, const void *data, uint32_t length);

/**
 * @brief  otp xip set
 *
 * @param[in] xip  xip
 **/
void otp_xip_set(bool xip);

/**
 * @brief  otp lowpower enter
 *
 * @param[in] power_status  power status
 **/
void otp_lowpower_enter(co_power_status_t power_status);

/**
 * @brief  otp lowpower leave
 *
 * @param[in] power_status  power status
 **/
void otp_lowpower_leave(co_power_status_t power_status);

#ifdef __cplusplus
/*{*/ }
#endif

#endif

/** @} */

