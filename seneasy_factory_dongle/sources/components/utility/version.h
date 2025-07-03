/**
 * @file     version.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/05/25
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

#ifndef __VERSION_H__
#define __VERSION_H__

/**
 * @brief  Version Define.
 */
#define MXD_BLE_SOC_FLAG        (0x2A4B)
#define ROM_VERSION             (0x02)       // ROM Version
#define BOOT2_VERSION           (0x11)       // 2nd-boot Version
#define SDK_VERSION             (0x02100210) // SDK Version: V2.1, Patch Version: V2.1
#define USER_PATCH_VERSION      (0x0000)     // User patch Version

#define BOOT2_CODE_ENCRYPT_FLAG (0x45)
#define FW_UPGRADE_FLAG         (0x476A6778)

#endif /* __VERSION_H__ */
