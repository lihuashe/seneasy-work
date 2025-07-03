/**
 * @file     rand.h
 * @version  V1.0
 * @date     2023/7/11
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

#ifndef __RAND_H__
#define __RAND_H__

/**
 * @brief  Define
 */
#define SYS_RAND_MAX 0xFFFFFFFF

/**
 * @brief  Rand init.
 * @param  u32Seed: Rand seed.
 */
extern void rom_rand_init(uint32_t u32Seed);

/**
 * @brief  Get rand data of 32bit.
 * @return uint32_t: Rand data.
 */
extern uint32_t rom_get_rand(void);

/**
 * @brief  Get rand data of 128bit
 * @param  pu8Buf: Point to data buffer which saved.
 */
extern void rom_get_rand_128(uint8_t *pu8Buf);

#endif /* __RAND_H__ */
