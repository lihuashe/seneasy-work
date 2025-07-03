/**
 * @file     rand.c
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

#include <stdint.h>
#include <stdio.h>

#include "utility.h"
#include "rand.h"

/**
 * @brief  rand seed
 */
static uint32_t m_u32RandSeed;

/**
 * @brief  Get rand
 * @param  pu32Seed.
 */
static uint32_t get_rand_r(uint32_t *pu32Seed)
{
    uint32_t u32Result;
    uint32_t u32NextSeed = *pu32Seed;

    u32NextSeed = (u32NextSeed * 1103515245) + 12345; /* permutate seed */
    u32Result   = u32NextSeed & 0xfffc0000;           /* use only top 14 bits */

    u32NextSeed = (u32NextSeed * 1103515245) + 12345;             /* permutate seed */
    u32Result   = u32Result + ((u32NextSeed & 0xffe00000) >> 14); /* top 11 bits */

    u32NextSeed = (u32NextSeed * 1103515245) + 12345;               /* permutate seed */
    u32Result   = u32Result + ((u32NextSeed & 0xfe000000) >> (25)); /* use only top 7 bits */

    *pu32Seed = u32NextSeed;

    return (u32Result & SYS_RAND_MAX);
}

/**
 * @brief  Rand init.
 * @param  u32Seed: Rand seed.
 */
void rom_rand_init(uint32_t u32Seed)
{
    m_u32RandSeed = u32Seed;
}

/**
 * @brief  Get rand data of 32bit.
 * @return uint32_t: Rand data.
 */
uint32_t rom_get_rand(void)
{
    return (get_rand_r(&m_u32RandSeed));
}

/**
 * @brief  Get rand data of 128bit
 * @param  pu8Buf: Point to data buffer which saved.
 */
void rom_get_rand_128(uint8_t *pu8Buf)
{
    uint8_t  i;
    uint32_t u32Result;

    for (i = 0; i < 4; i++)
    {
        u32Result               = rom_get_rand();
        *(pu8Buf + (4 * i) + 0) = BREAK_UINT32(u32Result, 0);
        *(pu8Buf + (4 * i) + 1) = BREAK_UINT32(u32Result, 1);
        *(pu8Buf + (4 * i) + 2) = BREAK_UINT32(u32Result, 2);
        *(pu8Buf + (4 * i) + 3) = BREAK_UINT32(u32Result, 3);
    }
}
