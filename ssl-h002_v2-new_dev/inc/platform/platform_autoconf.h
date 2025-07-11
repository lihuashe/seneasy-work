/**
************************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     platform_autoconf.h
* @brief    Platform configuration
* @author
* @date     2018-04-16
* @version  v0.1
*************************************************************************************************************
*/
#ifndef _PLATFORM_AUTOCONF_H_
#define _PLATFORM_AUTOCONF_H_


/** @defgroup  BUILD_CONFIGURATION    Build Configuration
    * @brief Platform build configuration
    * @{
    */

//#define RELEASE_VERSION

#ifdef RELEASE_VERSION
#define CHECK_STACK_OVERFLOW_ENABLE            0
#define CHECK_LOG_BUFFER_BEFORE_DLPS_ENABLE    0
#define PLATFORM_ASSERT_ENABLE                 0
#define CONFIG_LOG_FUNCTION_ENABLE             0
#define RUN_APP_IN_HCIMODE_ENABLE              0
#define SUPPORT_FTL_IN_APP                     0
/*disable it if need reduce code size*/
#define ENABLE_FULL_FEATURED_DIRECT_LOG        1
/**
    * RETARGET_PRINTF_METHOD different value
    * 0: redefine printf empty function
    * 1: redefine printf and using except log uart to print log
    * others: redefine printf just using log uart to print log
*/
#define RETARGET_PRINTF_METHOD      0
#else
#define CHECK_STACK_OVERFLOW_ENABLE            1
#define CHECK_LOG_BUFFER_BEFORE_DLPS_ENABLE    1
#define PLATFORM_ASSERT_ENABLE                 1
#define CONFIG_LOG_FUNCTION_ENABLE             1
#define RUN_APP_IN_HCIMODE_ENABLE              0
#define SUPPORT_FTL_IN_APP                     0
/*disable it if need reduce code size*/
#define ENABLE_FULL_FEATURED_DIRECT_LOG        1

/**
    * RETARGET_PRINTF_METHOD different value
    * 0: redefine printf empty function
    * 1: redefine printf and using except log uart to print log
    * others: redefine printf just using log uart to print log
*/
#define RETARGET_PRINTF_METHOD      0
#endif


/** End of BUILD_CONFIGURATION
  * @}
  */

#endif /* End of _PLATFORM_AUTOCONF_H_ */
