/**
 * @file     boards.h
 * @version  V1.0
 * @date     2022/09/22
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

#ifndef __BOARDS_H__
#define __BOARDS_H__

// MXD2670F03UC(QFN40) SoC Development Board
#if defined(MXD2670F03UC_EVB)
    #include "mxd2670f03uc_evb.h"

// MXD2670F03UD(QFN48) SoC Development Board
#elif defined(MXD2670F03UD_EVB)
    #include "mxd2670f03ud_evb.h"

// MXD2670F03UF(QFN64) SoC Development Board
#elif defined(MXD2670F03UF_EVB)
    #include "mxd2670f03uf_evb.h"

// MXD2670F04UH(BGA120) SoC Socket Development Board
#elif defined(MXD2670F04UH_EVB)
    #include "mxd2670f04uh_evb.h"

#elif defined(MXD2670F03UB_DONGLE_EVB)
    #include "mxd2670f03ub_vqfn32_dongle_evb.h"
    
// Undefine board
#else
    #error "Undefine board config"
#endif


#endif /* __BOARDS_H__ */


