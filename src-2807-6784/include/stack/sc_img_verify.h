/* ----------------------------------------------------------------------------
 * Copyright (c) 2020-2030 XIHAO Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of XhMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------- */

/**
 * @file     sc_img_verify.h
 * @brief    verify that the specified image is valid
 * @date     3 Feb 2023
 * @author   XIHAO SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __SC_IMG_VERIFY_H
#define __SC_IMG_VERIFY_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "peripheral.h"
#include "sha256.h"
#include "uECC.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
#define SC_IMG_VERIFY_HASH_LEN                     (256 / 8)
#define SC_IMG_VERIFY_PUBLIC_KEY_LEN               (512 / 8)
#define SC_IMG_VERIFY_DIG_SIGN_LEN                 (512 / 8)

#define SC_IMG_VERIFY_PUBKEY_HASH_BASE             ((1 << 12) | 0)

#define SC_IMG_VERIFY_EFUSE_VER_BASE               0
#define SC_IMG_VERIFY_EFUSE_VER_SIZE               8
#define SC_IMG_VERIFY_SECURE_SWITCH_BASE           9
#define SC_IMG_VERIFY_SECURE_SWITCH_SIZE           1

#define SC_IMG_VERIFY_MBR0_BASE                    (64 * 1024)
#define SC_IMG_VERIFY_MBR1_BASE                    (68 * 1024)


/*******************************************************************************
 * TYPEDEFS
 */
typedef enum {
    IMG_VERIFY_ERROR_OK,
    IMG_VERIFY_ERROR_MAGIC,
    IMG_VERIFY_ERROR_SIGN,
    IMG_VERIFY_ERROR_VERSION,
    IMG_VERIFY_ERROR_HASH,
    IMG_VERIFY_ERROR_PUBKEY,
    IMG_VERIFY_ERROR_CRC,
    IMG_VERIFY_ERROR_PARAMETER,
} sc_img_verify_error_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
extern sc_img_verify_error_t sc_img_verify(uint32_t mbr_verify_addr);

#ifdef __cplusplus
}
#endif

#endif  /* __SC_IMG_VERIFY_H */


/** @} */