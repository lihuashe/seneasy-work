/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     hid_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef HID_PROFILE_H_
#define HID_PROFILE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern const u16 extReportRefUUID;
extern const u16 reportRefUUID;
extern const u16 my_hidServiceUUID;
extern const u16 hidServiceUUID;
extern const u16 hidProtocolModeUUID;
extern const u16 hidReportUUID;
extern const u16 hidReportMapUUID;
extern const u16 hidbootKeyInReportUUID;
extern const u16 hidbootKeyOutReportUUID;
extern const u16 hidbootMouseInReportUUID;
extern const u16 hidinformationUUID;
extern const u16 hidCtrlPointUUID;
extern const u16 hidIncludeUUID;
extern const u8 protocolMode;

extern u8 reportKeyIn[8];
extern u8 reportKeyInCCC[2];
extern const u8 reportRefKeyIn[2];

extern u8 reportKeyOut[1];
extern const u8 reportRefKeyOut[2];

extern u8 reportConsumerControlIn[2];
extern u8 reportConsumerControlInCCC[2];
extern const u8 reportRefConsumerControlIn[2];

extern u8 bootKeyInReport;
extern u8 bootKeyInReportCCC[2];

extern u8 bootKeyOutReport;
extern const u8 hidInformation[4];

extern u8 controlPoint;
extern const u8 reportMap[78];
extern u16 extServiceUUID;

extern const u8 my_hidProtocolModeCharVal[5];
extern const u8 my_hidbootKeyInReporCharVal[5];
extern const u8 my_hidbootKeyOutReporCharVal[5];
extern const u8 my_hidReportCCinCharVal[5];
extern const u8 my_hidReportKEYinCharVal[5];
extern const u8 my_hidReportKEYoutCharVal[5];
extern const u8 my_hidReportMapCharVal[5];
extern const u8 my_hidinformationCharVal[5];
extern const u8 my_hidCtrlPointCharVal[5];

#endif /* HID_PROFILE_H_ */
