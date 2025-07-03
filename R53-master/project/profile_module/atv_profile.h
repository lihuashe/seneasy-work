/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     atv_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef ATV_PROFILE_H_
#define ATV_PROFILE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
#include "application/audio/gl_audio.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern u8 atv_char_tx_data[20];
extern u8 atv_char_rx_data[1];
extern u8 atv_char_ctl_data[1];

extern const u8 ATVV_Server_UUID[16];
extern const u8 ATVV_Char_UUID_TX[16];
extern const u8 ATVV_Char_UUID_RX[16];
extern const u8 ATVV_Char_UUID_CTL[16];

extern const u8 my_MicCharTx[20];
extern const u8 my_MicCharRx[20];
extern const u8 my_MicCharCtl[20];

#endif /* ATV_PROFILE_H_ */
