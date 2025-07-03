/***********************************************************************************************************************
 * @file     mxd2670f03uf_dongle_evb.h
 * @version  V1.0
 * @date     2022/9/22
 * @history
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2020, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/
 
#ifndef MXD2670F03UF_DONGLE_EVB__H
#define MXD2670F03UF_DONGLE_EVB__H


// UART Pins
// UART0
#define GPIO_PORT_UART0_TX                 ( GPIOA )
#define GPIO_PIN_UART0_TX                  ( GPIO_PIN_30 ) //P30, PA30
//#define GPIO_PORT_UART0_RX                 ( GPIOB )
//#define GPIO_PIN_UART0_RX                  ( GPIO_PIN_19 ) //P51, PB19

// UART1
#define GPIO_PORT_UART1_TX                 ( GPIOB )
#define GPIO_PIN_UART1_TX                  ( GPIO_PIN_10 ) //P42, PB10
#define GPIO_PORT_UART1_RX                 ( GPIOA )
#define GPIO_PIN_UART1_RX                  ( GPIO_PIN_30 ) //P30, PA30

// UART2
#define GPIO_PORT_UART2_TX                 ( GPIOB )
#define GPIO_PIN_UART2_TX                  ( GPIO_PIN_10 ) //P42, PB10
//#define GPIO_PORT_UART2_RX                 ( GPIOA )
//#define GPIO_PIN_UART2_RX                  ( GPIO_PIN_22 ) //P22, PA22


#endif /* MXD2670F03UF_DONGLE_EVB__H */


