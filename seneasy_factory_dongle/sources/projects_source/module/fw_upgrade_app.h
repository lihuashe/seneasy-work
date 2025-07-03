/**********************************************************************************************************************
 * @file     fw_upgrade_app.h
 * @version  V3.0
 * @date     2022/12/2
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
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#ifndef __FW_UPGRADE_APP_H_
#define __FW_UPGRADE_APP_H_


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef struct __attribute__((aligned(4)))
{
    //Information
    uint16_t u16MXDFlag;
    uint8_t  u8ChipType;
    uint8_t  u8UpgradeDst;

    uint8_t  u8RomVer;
    uint8_t  u8Boot2Ver;
    uint16_t u16AppCodeVer;
    uint32_t u32BinCodeSize;
    uint32_t u32BinCodeCrc;

} stUpgradeFileInfo_t;



/**
 * @brief  Firmware upgrade init.
 * @param  u8Interface: Communication interface, @ref enFwUpgradePhyInterface_t.
 */
extern uint8_t fw_upgrade_init(uint8_t u8Interface);


#endif /* __FW_UPGRADE_APP_H_ */

