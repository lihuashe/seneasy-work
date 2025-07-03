/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     gap_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef GAP_PROFILE_H_
#define GAP_PROFILE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;

extern const u16 serviceChangeUUID;
extern const u16 my_devNameUUID;
extern const u16 my_gapServiceUUID;
extern const u16 my_appearanceUIID;
extern const u16 my_periConnParamUUID;
extern const u16 my_appearance;
extern const u16 my_gattServiceUUID;

extern u16 serviceChangeVal[2];
extern u8 serviceChangeCCC[2];
extern const gap_periConnectParams_t my_periConnParameters;

extern const u8 my_devNameCharVal[5];
extern const u8 my_appearanceCharVal[5];
extern const u8 my_periConnParamCharVal[5];
extern const u8 my_serviceChangeCharVal[5];

#endif /* GAP_PROFILE_H_ */
