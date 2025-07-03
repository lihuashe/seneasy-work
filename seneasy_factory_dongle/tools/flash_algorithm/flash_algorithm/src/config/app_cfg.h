/**
 * @file     app_cfg.h
 * @version  V1.0
 * @date     2021/04/21
 * @history 
 * @note     flash_algorithm app configuration.
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR 
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE 
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */


#ifndef __APP_CFG_H__
#define __APP_CFG_H__


//=====================================================================================================================
// PERIPHERAL DEFINE
//=====================================================================================================================
//=============================================================================
// GPIO
//=============================================================================
#define GPIO_PORT_APP_DEBUG_TX             (GPIO_PORT_UART0_TX )
#define GPIO_PIN_APP_DEBUG_TX              (GPIO_PIN_UART0_TX )


//=============================================================================
// UART
//=============================================================================
#define UART_HANDLE_APP_DEBUG              (UART0)
#define UART_BPS_APP_DEBUG                 (921600)

//=============================================================================
// SYS_CTRL
//=============================================================================
#define SYS_CTRL_COM_REG_INDEX             (0)
#define SYS_CTRL_COM_REG_VAL               (0x1A2B3C4D)


//=====================================================================================================================
// DEBUG
//=====================================================================================================================
#define APP_DEBUG_ENABLED                  (0)

#if APP_DEBUG_ENABLED
    #define PRINTF(fmt, args...)           printf(fmt, ##args)
#else
    #define PRINTF(fmt, args...)
#endif


#endif /* __APP_CFG_H__ */


