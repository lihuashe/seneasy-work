/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     tvssu_profile.c
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
#include "tvssu_profile.h"
#include "stack/ble/ble.h"
#include "dis_profile.h"
#include "utility.h"
#include "app_att.h"
#if (TL_SZ119_TVSSU)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
extern int SSP_Profile_WriteAttrCB(void *p);
const u8 my_SimpleSetupServiceUUID[16] = {0x71,0x01,0x11,0x79,0x9E,0xCD,0x72,0x8E,0x08,0x47,0xDA,0xEF,0xCB,0x51,0x8D,0xC8};
const u8 my_SimpleSetupUUID[16] =        {0x2A,0x09,0x4D,0xA9,0xE1,0x83,0xCF,0xBE,0xDA,0x4A,0xE7,0xCE,0xCA,0xDD,0x3D,0xEA};
    
u8 simplesetupValueInCCC[2];
u8 my_SimpleSetupData[20] = {0x00};
const u8  my_SimpleSetupName[13] ={'M','Y','S','I','M','P','L','E','S','E','T','U','P'};

const u8 ss_CharVal[19] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP |CHAR_PROP_NOTIFY,
    U16_LO(SIMPLESETUP_CMD_OUT_DP_H),U16_HI(SIMPLESETUP_CMD_OUT_DP_H),
    0x2A,0x09,0x4D,0xA9,0xE1,0x83,0xCF,0xBE,0xDA,0x4A,0xE7,0xCE,0xCA,0xDD,0x3D,0xEA
    };

/******************************************************************
 * @brief  seneasy_tvssu_attr_write_cb
 * @param  p
 * @return int
 * @retval void
 */
int seneasy_tvssu_attr_write_cb(void *p) 
{
    unsigned char *pdata = p;
    pdata -= 4;
    SSP_Profile_WriteAttrCB(pdata);
    return 1;
}
#endif

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
