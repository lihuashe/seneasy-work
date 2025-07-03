/**
 * @file     reg_efuse.h
 * @version  V1.0
 * @date     2023/5/25
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

#ifndef __REG_EFUSE_H__
#define __REG_EFUSE_H__

#ifdef __cplusplus
extern "C" {
#endif

//=====================================================================================================================
// TYPEDEF STRUCT
//=====================================================================================================================
typedef struct
{
    __OM  uint32_t EFUSE_CMD;         /* [0x00](WO) */
    __IOM uint32_t EFUSE_CFG;         /* [0x04](RW) */
    __IOM uint32_t EFUSE_ADDR;        /* [0x08](RW) */
    __IOM uint32_t EFUSE_DATA;        /* [0x0C](RW) */
    __IOM uint32_t EFUSE_POWER;       /* [0x10](RW) */
    __IM  uint32_t EFUSE_FLAG;        /* [0x14](RO) */

} stEFUSE_Handle_t, *pstEFUSE_Handle_t;


//=====================================================================================================================
// [0x00](WO) EFUSE_CMD
//=====================================================================================================================
//=============================================================================
// EFUSE_CMD[1] EFUSE_CMD_CLR
#define EFUSE_CMD_CLR_SHIFT                (1)
#define EFUSE_CMD_CLR_MSK                  ((0x01 << 1) - 1)

//=============================================================================
// EFUSE_CMD[0] EFUSE_CMD_PROGM
#define EFUSE_CMD_PROGM_SHIFT              (0)
#define EFUSE_CMD_PROGM_MSK                ((0x01 << 1) - 1)


//=====================================================================================================================
// [0x04](RW) EFUSE_CFG
//=====================================================================================================================
//=============================================================================
// EFUSE_CFG[19:16] EFUSE_TPD_TIME
#define EFUSE_TPD_TIME_SHIFT               (16)
#define EFUSE_TPD_TIME_MSK                 ((0x01 << 4) - 1)

//=============================================================================
// EFUSE_CFG[15:14] EFUSE_TPP_TIME
#define EFUSE_TRPW_TIME_SHIFT              (14)
#define EFUSE_TRPW_TIME_MSK                ((0x01 << 2) - 1)

//=============================================================================
// EFUSE_CFG[13:8] EFUSE_TPI_TIME
#define EFUSE_TPI_TIME_SHIFT               (8)
#define EFUSE_TPI_TIME_MSK                 ((0x01 << 6) - 1)

//=============================================================================
// EFUSE_CFG[16] EFUSE_TPP_TIME
#define EFUSE_TPP_TIME_SHIFT               (0)
#define EFUSE_TPP_TIME_MSK                 ((0x01 << 8) - 1)


//=====================================================================================================================
// [0x08](RW) EFUSE_ADDR
//=====================================================================================================================
//=============================================================================
// EFUSE_ADDR[5:0]  EFUSE_ADDR
#define EFUSE_ADDR_SHIFT                   (0)
#define EFUSE_ADDR_MSK                     ((0x01 << 6) - 1)


//=====================================================================================================================
// [0x0C](RW) EFUSE_DATA
//=====================================================================================================================
//=============================================================================
// EFUSE_DATA[7:0] EFUSE_DATA
#define EFUSE_DATA_SHIFT                   (0)
#define EFUSE_DATA_MSK                     ((0x01 << 8) - 1)


//=====================================================================================================================
// [0x10](RW) EFUSE_POWER
//=====================================================================================================================
//=============================================================================
// EFUSE_POWER[0]  EFUSE_POWER_EN
#define EFUSE_POWER_EN_SHIFT               (0)
#define EFUSE_POWER_EN_MSK                 ((0x01 << 1) - 1)


//=====================================================================================================================
// [0x14](RO) EFUSE_FLAG
//=====================================================================================================================
#define EFUSE_FLAG_END_SHIFT               (0)
#define EFUSE_FLAG_END_MSK                 ((0x01 << 1) - 1)


#ifdef __cplusplus
}
#endif

#endif /* __REG_EFUSE_H__ */

