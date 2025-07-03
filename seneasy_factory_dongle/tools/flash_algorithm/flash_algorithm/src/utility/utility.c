/**
 * @file     utility.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2020/01/03
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
#include <stdbool.h>

#include "utility.h"

/**
 * @brief  System delay
 * @param  u32Time: Time which will delay, unit: us.
 */
void rom_delay_us(uint32_t u32Time)
{
    uint32_t cycles;

    // fix 16M
    cycles = 16000000 / 1000000U * u32Time;

    rom_delay_cycles(cycles);
}

/**
 * @brief  System delay.
 * @param  u32Time: Time which will delay, unit: ms.
 */
void rom_delay_ms(uint32_t u32Time)
{
    rom_delay_us(u32Time * 1000);
}
