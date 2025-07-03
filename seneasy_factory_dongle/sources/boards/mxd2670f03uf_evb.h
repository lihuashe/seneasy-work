/**
 * @file     mxd2670f03uf_evb.h
 * @version  V1.0
 * @date     2022/9/22
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2020, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __MXD2670F03UF_EVB__H__
#define __MXD2670F03UF_EVB__H__

// Key pin
#define GPIO_PORT_KEY                      (GPIOB)
#define GPIO_PIN_KEY                       (GPIO_PIN_3)  //P35, PB3

// UART Pins
// UART0
#define GPIO_PORT_UART0_TX                 (GPIOB)
#define GPIO_PIN_UART0_TX                  (GPIO_PIN_17) //P49, PB17
#define GPIO_PORT_UART0_RX                 (GPIOB)
#define GPIO_PIN_UART0_RX                  (GPIO_PIN_19) //P51, PB19

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


#endif /* __MXD2670F03UF_EVB__H__ */


