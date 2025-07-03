/**
 * @file     mxd2670f03uc_evb.h
 * @version  V1.0
 * @date     2023/9/1
 * @history
 * @note
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

#ifndef __MXD2670F03UC_EVB__H__
#define __MXD2670F03UC_EVB__H__

// UART Pins
// UART0
#define GPIO_PORT_UART0_TX                 (GPIOB)
#define GPIO_PIN_UART0_TX                  (GPIO_PIN_17) //P49, PB17
#define GPIO_PORT_UART0_RX                 (GPIOB)
#define GPIO_PIN_UART0_RX                  (GPIO_PIN_1)  //P33, PB1

// UART1
#define GPIO_PORT_UART1_TX                 (GPIOA)
#define GPIO_PIN_UART1_TX                  (GPIO_PIN_31) //P31, PA31
#define GPIO_PORT_UART1_RX                 (GPIOA)
#define GPIO_PIN_UART1_RX                  (GPIO_PIN_30) //P30, PA30

// UART2
#define GPIO_PORT_UART2_TX                 (GPIOA)
#define GPIO_PIN_UART2_TX                  (GPIO_PIN_29) //P29, PA29
#define GPIO_PORT_UART2_RX                 (GPIOA)
#define GPIO_PIN_UART2_RX                  (GPIO_PIN_22) //P22, PA22

// Flash Pins
#define GPIO_PORT_FLASH_CLK                (GPIOA)
#define GPIO_PIN_FLASH_CLK                 (GPIO_PIN_23) //P23, PA23
#define GPIO_PORT_FLASH_CS                 (GPIOA)
#define GPIO_PIN_FLASH_CS                  (GPIO_PIN_24) //P24, PA24
#define GPIO_PORT_FLASH_SI                 (GPIOA)
#define GPIO_PIN_FLASH_SI                  (GPIO_PIN_25) //P25, PA25
#define GPIO_PORT_FLASH_SO                 (GPIOA)
#define GPIO_PIN_FLASH_SO                  (GPIO_PIN_26) //P26, PA26
#define GPIO_PORT_FLASH_WP                 (GPIOA)
#define GPIO_PIN_FLASH_WP                  (GPIO_PIN_27) //P27, PA27
#define GPIO_PORT_FLASH_HLD                (GPIOA)
#define GPIO_PIN_FLASH_HLD                 (GPIO_PIN_28) //P28, PA28


#endif /* __MXD2670F03UC_EVB__H__ */


