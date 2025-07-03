/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     atv_profile.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "atv_profile.h"
#include "stack/ble/ble.h"
/*============================================================================*
 *                             Global Variables
 *============================================================================*/
u8 atv_char_tx_data[20] = {0};
u8 atv_char_rx_data[1]  = {0};
u8 atv_char_ctl_data[1] = {0};

#if AUDIO_LTVV
#define AUDIO_LTVV_SERVICE_UUID     0xdf,0xbb,0x48,0x4f,0xa8,0x4a,0x9f,0x97,0x99,0x48,0xa7,0x40,0xaa,0x97,0x0d,0xa7
#define AUDIO_LTVV_TX_CHAR_UUID     0x54,0x80,0x37,0x51,0x80,0xe3,0xf6,0xbf,0x09,0x4b,0x6c,0xc0,0xb4,0x78,0xf5,0x7a
#define AUDIO_LTVV_RX_CHAR_UUID     0x93,0xe7,0x98,0xed,0x5f,0x93,0x50,0xa5,0xb8,0x43,0x32,0x11,0xf7,0x67,0x6c,0xd9
#define AUDIO_LTVV_CTL_CHAR_UUID    0xcb,0xa4,0x54,0x47,0x29,0x9b,0x40,0x9b,0xf4,0x4b,0x83,0x4c,0x54,0x30,0xec,0x24

const u8 ATVV_Server_UUID[16]   = {AUDIO_LTVV_SERVICE_UUID};
const u8 ATVV_Char_UUID_TX[16]  = {AUDIO_LTVV_TX_CHAR_UUID};
const u8 ATVV_Char_UUID_RX[16]  = {AUDIO_LTVV_RX_CHAR_UUID};
const u8 ATVV_Char_UUID_CTL[16] = {AUDIO_LTVV_CTL_CHAR_UUID};
#else
const u8 ATVV_Server_UUID[16]   = AUDIO_GOOGLE_SERVICE_UUID;
const u8 ATVV_Char_UUID_TX[16]  = {AUDIO_GOOGL_TX_CHAR_UUID};
const u8 ATVV_Char_UUID_RX[16]  = {AUDIO_GOOGL_RX_CHAR_UUID};
const u8 ATVV_Char_UUID_CTL[16] = {AUDIO_GOOGL_CTL_CHAR_UUID};
#endif
/*============================================================================*
 *                            ATV Attribute Values
 *============================================================================*/
const u8 my_MicCharTx[20] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_NOTIFY,
    U16_LO(AUDIO_GOOGLE_TX_DP_H), U16_HI(AUDIO_GOOGLE_TX_DP_H),
    AUDIO_GOOGL_TX_CHAR_UUID,
};
const u8 my_MicCharRx[20] = {
    CHAR_PROP_READ | CHAR_PROP_NOTIFY,
    U16_LO(AUDIO_GOOGLE_RX_DP_H), U16_HI(AUDIO_GOOGLE_RX_DP_H),
    AUDIO_GOOGL_RX_CHAR_UUID,
};
const u8 my_MicCharCtl[20] = {
    CHAR_PROP_READ | CHAR_PROP_NOTIFY,
    U16_LO(AUDIO_GOOGLE_CTL_DP_H), U16_HI(AUDIO_GOOGLE_CTL_DP_H),
    AUDIO_GOOGL_CTL_CHAR_UUID,
};

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
