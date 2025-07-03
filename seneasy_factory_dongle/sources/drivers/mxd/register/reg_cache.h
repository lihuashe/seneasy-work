/**
 * @file     reg_cache.h
 * @version  V1.0
 * @date     2020/03/31
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

#ifndef __REG_CACHE_H__
#define __REG_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

//=====================================================================================================================
// DEFAULT CONFIG
//=====================================================================================================================
#define CACHE_READ_MODE_DEFAULT(MODE)                                                       \
( ((MODE & MEM_CACHE_READ_CMD_MSK) << MEM_CACHE_READ_CMD_SHIFT)         | /* bit0~bit7 */   \
  ((SFLASH_CMD_SUS & MEM_CACHE_SUS_CMD_MSK) << MEM_CACHE_HPM_CMD_SHIFT) | /* bit8~bit15 */  \
  ((SFLASH_CMD_RES & MEM_CACHE_RES_CMD_MSK) << MEM_CACHE_HPM_CMD_SHIFT) | /* bit16~bit23 */ \
  ((SFLASH_CMD_HPM & MEM_CACHE_HPM_CMD_MSK) << MEM_CACHE_HPM_CMD_SHIFT)   /* bit24~bit31 */ \
)


//=====================================================================================================================
// TYPEDEF STRUCT
//=====================================================================================================================
typedef struct
{
    __IOM uint32_t CACHE_CTRL;              /* [0x00](RW) */
    __OM  uint32_t CACHE_CMD_EN;            /* [0x04](WO) */
    __IM  uint32_t CACHE_CMD_BUSY;          /* [0x08](RO) */

} stCACHE_Handle_t, *pstCACHE_Handle_t;


//=====================================================================================================================
// [0x00]CACHE_CTRL Register
//=====================================================================================================================
#define CACHE_CTRL_DEFAULT                 (0x00000000)

//=============================================================================
// CACHE_CTRL[6] CACHE_WRAP Define
#define CACHE_WRAP_SHIFT                   (6)
#define CACHE_WRAP_MSK                     ((0x01 << 1) - 1)

//=============================================================================
// CACHE_CTRL[5:1] CACHE_CMD Define
#define CACHE_CMD_SHIFT                    (1)
#define CACHE_CMD_MSK                      ((0x01 << 5) - 1)

//=============================================================================
// CACHE_CTRL[0] CACHE_EN Define
#define CACHE_EN_SHIFT                     (0)
#define CACHE_EN_MSK                       ((0x01 << 1) - 1)

typedef enum
{
    CACHE_DISABLE  = 0x00,
    CACHE_ENABLE   = 0x01,

} EN_CACHE_ENABLE_T;

#define IS_VALID_CACHE_ENABLE(EN)   (((EN) == CACHE_DISABLE) || \
                                     ((EN) == CACHE_ENABLE))


//=====================================================================================================================
// [0x04]CACHE_CMD_EN Register
//=====================================================================================================================
#define CACHE_CMD_EN_SHIFT                 (0)
#define CACHE_CMD_EN_MSK                   ((0x01 << 1) - 1)

typedef enum
{
    CACHE_CMD_DISABLE  = 0,
    CACHE_CMD_ENABLE   = 1,

} EN_CACHE_CMD_ENABLE_T;

#define IS_VALID_CACHE_CMD_ENABLE(EN)   (((EN) == CACHE_CMD_DISABLE) || \
                                         ((EN) == CACHE_CMD_ENABLE))

//=====================================================================================================================
// [0x08]CACHE_CMD_BUSY Register
//=====================================================================================================================
#define CACHE_CMD_STATUS_SHIFT             (0)
#define CACHE_CMD_STATUS_MSK               ((0x01 << 1) - 1)
#define CACHE_CMD_STATUS_BUSY              ((CACHE_CMD_BUSY & CACHE_CMD_STATUS_MSK) << CACHE_CMD_STATUS_SHIFT)

typedef enum
{
    CACHE_CMD_FINISHED  = 0,
    CACHE_CMD_BUSY      = 1,

} EN_CACHE_CMD_STATUS_T;



#ifdef __cplusplus
}
#endif

#endif /* __REG_CACHE_H__ */


