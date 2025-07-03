/**
 * @file     reg_wdt.h
 * @version  V1.0 
 * @date     2023/6/14
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

#ifndef __REG_WDT_H__
#define __REG_WDT_H__


#ifdef __cplusplus
 extern "C" {
#endif


//=====================================================================================================================
// TYPEDEF STRUCT
//=====================================================================================================================
typedef struct
{
    __IM  uint32_t WDT_INT_FLAG;            /*[0x00](RO) */
    __IOM uint32_t WDT_INT_EN;              /*[0x04](RW) */
    __OM  uint32_t WDT_INT_CLR;             /*[0x08](WO) */
    __OM  uint32_t WDT_TRIG_CFG;            /*[0x0C](WO) */
    __IOM uint32_t WDT_EN;                  /*[0x10](RW) */
    __IOM uint32_t WDT_COMPARE;             /*[0x14](RW) */
    __IM  uint32_t WDT_COUNTER;             /*[0x18](RO) */
    __IOM uint32_t WDT_RESET_EN;            /*[0x1C](RW) */
    __IOM uint32_t WDT_RESET_DELAY_TIME;    /*[0x20](RW) */
    __IM  uint32_t WDT_CFG_VALID_FLAG;      /*[0x24](RO) */
    __OM  uint32_t WDT_CFG_VALID_CLR;       /*[0x28](WO) */
    __IOM uint32_t WDT_EXT_CG_EN;           /*[0x2C](RW) */

} stWDT_Handle_t,*pstWDT_Handle_t;

#define IS_VALID_WDT_HANDLE(pstWDT)  ((WDT0) == (pstWDT) || (WDT1) == (pstWDT))


//=====================================================================================================================
// [0x00~0x08] WDT_INT_FLAG / WDT_INT_EN / WDT_INT_CLR
//=====================================================================================================================
#define WDT_INT_OVERFLOW_SHIFT             (1)
#define WDT_INT_MATCH_SHIFT                (0)

typedef enum
{
    WDT_INT_MATCH       = (0x01 << WDT_INT_MATCH_SHIFT),
    WDT_INT_OVERFLOW    = (0x01 << WDT_INT_OVERFLOW_SHIFT),

} EN_WDT_INT_FLAG_T;

#define WDT_INT_MASK      (0x03)


//=====================================================================================================================
// [0x0C]WDT_TRIG_CFG Register 
//=====================================================================================================================
//=============================================================================
// WDT_TRIG_CFG[4] WDT_TRIG_RESET_DELAY_VALID Define
#define WDT_TRIG_RESET_DELAY_VALID_SHIFT   (4)
#define WDT_TRIG_RESET_DELAY_VALID_MSK     ((0x01 << 1) - 1)
#define WDT_TRIG_RESET_DELAY_VALID         (0x01 << WDT_TRIG_RESET_DELAY_VALID_SHIFT)

//=============================================================================
// WDT_TRIG_CFG[3] WDT_TRIG_ENABLE_VALID Define
#define WDT_TRIG_ENABLE_VALID_SHIFT        (3)
#define WDT_TRIG_ENABLE_VALID_MSK          ((0x01 << 1) - 1)
#define WDT_TRIG_ENABLE_VALID              (0x01 << WDT_TRIG_ENABLE_VALID_SHIFT)

//=============================================================================
// WDT_TRIG_CFG[2] WDT_TRIG_OVERFLOW_VALID Define
#define WDT_TRIG_OVERFLOW_VALID_SHIFT      (2)
#define WDT_TRIG_OVERFLOW_VALID_MSK        ((0x01 << 1) - 1)

//=============================================================================
// WDT_TRIG_CFG[1] WDT_TRIG_COMPARE_VALID Define
#define WDT_TRIG_COMPARE_VALID_SHIFT       (1)
#define WDT_TRIG_COMPARE_VALID_MSK         ((0x01 << 1) - 1)
#define WDT_TRIG_COMPARE_VALID             (0x01 << WDT_TRIG_COMPARE_VALID_SHIFT)

//=============================================================================
// WDT_TRIG_CFG[0] WDT_TRIG_CLEAR_COUNT_VALID Define
#define WDT_TRIG_CLEAR_COUNT_VALID_SHIFT   (0)
#define WDT_TRIG_CLEAR_COUNT_VALID_MSK     ((0x01 << 1) - 1)
#define WDT_TRIG_CLEAR_COUNT_VALID         (0x01 << WDT_TRIG_CLEAR_COUNT_VALID_SHIFT)


//=====================================================================================================================
// [0x10](RW) WDT_EN Register 
//=====================================================================================================================
//=============================================================================
// WDT_EN[6:0] WDT_EN Define 
#define WDT_ENABLE_SHIFT                   (0)
#define WDT_ENABLE_MSK                     ((0x01 << 7) - 1)
#define WDT_ENABLE                         (0x5A)
#define WDT_DISABLE                        (0)


//=====================================================================================================================
// [0x14]WDT_COMPARE Register 
//=====================================================================================================================
//=============================================================================
// WDT_COMPARE[31:0] WDT_COMPARE Define 
#define WDT_COMPARE_CNT_SHIFT              (0)
#define WDT_COMPARE_CNT_MSK                (WORD_MSK)

//=====================================================================================================================
// [0x18]WDT_COUNTER Register 
//=====================================================================================================================
//=============================================================================
// WDT_COUNTER[31:0] WDT_COUNTER Define 
#define WDT_COUNTER_SHIFT                  (0)
#define WDT_COUNTER_MSK                    (WORD_MSK)


//=====================================================================================================================
// [0x1C](RW) WDT_RESET_EN Register 
//=====================================================================================================================
//=============================================================================
// WDT_RESET_EN[2:0] WDT_RESET_EN Define 
#define WDT_RESET_EN_SHIFT                 (0)
#define WDT_RESET_EN_MSK                   ((0x01 << 3) - 1)
#define WDT_RESET_ENABLE                   (1)
#define WDT_RESET_DISABLE                  (7)


//=====================================================================================================================
// [0x20](RW) WDT_RESET_DELAY_TIME Register 
//=====================================================================================================================
//=============================================================================
// WDT_RESET_DELAY_TIME[15:0] WDT_RESET_DELAY_TIME Define 
#define WDT_RESET_DELAY_TIME_SHIFT         (0)
#define WDT_RESET_DELAY_TIME_MSK           ((0x01 << 16) - 1)


//=====================================================================================================================
// [0x24](RO) WDT_CFG_VALID_FLAG Register 
//=====================================================================================================================
#define WDT_RESET_DELAY_VALID_SHIFT        (4)
#define WDT_ENABLE_VALID_SHIFT             (3)
#define WDT_OVERFLOW_VALID_SHIFT           (2)
#define WDT_COMPARE_CNT_VALID_SHIFT        (1)
#define WDT_CLEAR_CNT_VALID_SHIFT          (0)

typedef enum
{
    WDT_CLEAR_CNT_VALID   = (0x01 << WDT_CLEAR_CNT_VALID_SHIFT),
    WDT_COMPARE_CNT_VALID = (0x01 << WDT_COMPARE_CNT_VALID_SHIFT),
    WDT_OVERFLOW_VALID    = (0x01 << WDT_OVERFLOW_VALID_SHIFT),
    WDT_ENABLE_VALID      = (0x01 << WDT_ENABLE_VALID_SHIFT),
    WDT_RESET_DELAY_VALID = (0x01 << WDT_RESET_DELAY_VALID_SHIFT),

} EN_WDT_CFG_VALID_FLAG_T;

#define WDT_CFG_VALID_FLAG_MASK            (0x1F)


#ifdef __cplusplus
}
#endif

#endif /* __REG_WDT_H__ */


