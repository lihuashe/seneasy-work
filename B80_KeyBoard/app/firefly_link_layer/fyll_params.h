#pragma once

#include "tl_tpll.h"

/*-----------------------------------------------------------------------------
  地址和频道配置
-----------------------------------------------------------------------------*/

#define FYLL_BASE_ADDRESS_0           0x39, 0x51, 0x76, 0x95
#define FYLL_BASE_ADDRESS_1           0xc2, 0xc2, 0xc2, 0xc2
#define FYLL_BASE_ADDRESS_PREFIX      0x71, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6

#define FYLL_DEFAULT_CHANNEL_TAB      {3, 23, 40, 61, 75}  // [RF channels]
#define FYLL_DEFAULT_CHANNEL_TAB_SIZE 1               // [bytes]

// Static parameters  
#define FYLL_MAX_CHANNEL_TAB_SIZE 16                     // [# channels]              
#define FYLL_MAX_PAYLOAD_LENGTH   32                     // [bytes]

/*-----------------------------------------------------------------------------
  跳频及同步参数控制
-----------------------------------------------------------------------------*/

#define FYLL_DEFAULT_PARAM_TX_TIMEOUT 400
#define FYLL_DEFAULT_PARAM_TX_ATTEMPTS_PR_CHANNEL_WHEN_SYNC_ON 2
#define FYLL_DEFAULT_PARAM_TX_ATTEMPTS_PR_CHANNEL_WHEN_SYNC_OFF (FYLL_DEFAULT_CHANNEL_TAB_SIZE * 7)

#define FYLL_DEFAULT_PARAM_HOST_MODE 0
#define FYLL_DEFAULT_PARAM_RX_PIPES 0x3f
#define FYLL_DEFAULT_PARAM_RX_TIMEOUT 0                 // [# FYLL_DEFAULT_PARAM_RX_PERIODs]
#define FYLL_DEFAULT_PARAM_RX_PERIOD 1100               // [us]

#define FYLL_DEFAULT_PARAM_RX_PERIOD_MODIFIER \
    (TRF_TX_WAIT_US + TRF_TX_SETTLE_US + TRF_RX_WAIT_US + TRF_RX_SETTLE_US)           // [us]
#define FYLL_DEFAULT_PARAM_MAX_SYNC_PERIOD  250         // [# FYLL_DEFAULT_PARAM_RX_PERIODs]

#define FYLL_DEFAULT_PARAM_COLLISION_CHANNEL_SWITCH_LIMIT 50 // [# Channel switches]

/*
Number of bytes pr packet exclusive the payload.
That is: 2 preamble + 2 CRC + 5 address + ~2 byte packet control
*/
#define FYLL_CONST_BYTES_PR_PACKET (2 + 2 + 5 + 2) 

               
/*-----------------------------------------------------------------------------
  无线速率和发射强度
-----------------------------------------------------------------------------*/               

#define FYLL_DEFAULT_PARAM_OUTPUT_POWER TRF_TPLL_POWER_P3p95dBm

/** 是否启动外部RF前端 */
#define RF_FEM_ENABLE 0

#if RF_FEM_ENABLE
#define GPIO_RF_FEM_PA   GPIO_PA6
// #define GPIO_RF_FEM_LNA  GPIO_PD0
#endif

#define FYLL_DATARATE_1MBPS       0
#define FYLL_DATARATE_2MBPS       1
#define FYLL_DATARATE_500KBPS     2
#define FYLL_DATARATE_250KBPS     3

/** 定义无线传输速率 */
#define FYLL_DATARATE FYLL_DATARATE_2MBPS

#if(FYLL_DATARATE == FYLL_DATARATE_2MBPS)
  #define FYLL_US_PR_BYTE 4
  #define FYLL_HAL_DATARATE TRF_TPLL_BITRATE_2MBPS
  #define FYLL_HOST_CE_LOW_IDLE_DELAY 10 // Host cative -> Idle delay. 9 * 50us = 500 us.
  #if(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH > 15)
    #define FYLL_AUTO_RETR_DELAY 500       
  #else
    #define FYLL_AUTO_RETR_DELAY 250       
  #endif  
#endif

#if(FYLL_DATARATE == FYLL_DATARATE_1MBPS)
  #define FYLL_US_PR_BYTE 8
  #define FYLL_HAL_DATARATE TRF_TPLL_BITRATE_1MBPS
  #define FYLL_HOST_CE_LOW_IDLE_DELAY 14 // Host cative -> Idle delay. 13 * 50us = 700 us.
  #if(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH > 5)
    #define FYLL_AUTO_RETR_DELAY 500       
  #else
    #define FYLL_AUTO_RETR_DELAY 250       
  #endif   
#endif

#if(FYLL_DATARATE == FYLL_DATARATE_500KBPS)
  #define FYLL_US_PR_BYTE 16
  #define FYLL_HAL_DATARATE TRF_TPLL_BITRATE_500KBPS
  #define FYLL_HOST_CE_LOW_IDLE_DELAY 37 // Host cative -> Idle delay. 36 * 50us = 1850 us.
  
  #if(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH == 0)
    #define FYLL_AUTO_RETR_DELAY 500       
  #elif(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH < 8)
    #define FYLL_AUTO_RETR_DELAY 750       
  #elif(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH < 16)
    #define FYLL_AUTO_RETR_DELAY 1000       
  #elif(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH < 24)
    #define FYLL_AUTO_RETR_DELAY 1250       
  #else
    #define FYLL_AUTO_RETR_DELAY 1500       
  #endif
#endif   

#if(FYLL_DATARATE == FYLL_DATARATE_250KBPS)
  #define FYLL_US_PR_BYTE 32
  #define FYLL_HAL_DATARATE TRF_TPLL_BITRATE_250KBPS
  #define FYLL_HOST_CE_LOW_IDLE_DELAY 37 // Host cative -> Idle delay. 36 * 50us = 1850 us.
  
  #if(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH == 0)
    #define FYLL_AUTO_RETR_DELAY 500       
  #elif(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH < 8)
    #define FYLL_AUTO_RETR_DELAY 750       
  #elif(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH < 16)
    #define FYLL_AUTO_RETR_DELAY 1000       
  #elif(FYLL_INTERNAL_ACK_PAYLOAD_LENGTH < 24)
    #define FYLL_AUTO_RETR_DELAY 1250       
  #else
    #define FYLL_AUTO_RETR_DELAY 1500       
  #endif
#endif   

/*-----------------------------------------------------------------------------
  配对
-----------------------------------------------------------------------------*/   

/** Definition of output power to be used when sending a pairing request. */ 
#define FYLL_PAIRING_TX_POWER TRF_TPLL_POWER_N0p22dBm

#define FYLL_DEFAULT_AES_KEYS {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}, \
                               {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32}}

/*-----------------------------------------------------------------------------
  角色定义(Device or Host)
-----------------------------------------------------------------------------*/  
#define FYLL_DEVICE     0
#define FYLL_HOST       1

// #define FYLL_ROLE   FYLL_HOST

#ifndef FYLL_ROLE   
#define FYLL_ROLE   FYLL_DEVICE
#endif


#define FYLL_DEFAULT_DEVICE_MODE  4

