/******************************************************************************
 * @file     types.h
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

#pragma once
#include <stdbool.h>


typedef unsigned char u8 ;

typedef signed char s8;

typedef unsigned short u16;

typedef signed short s16;

typedef int s32;

typedef unsigned int u32;

typedef long long s64;

typedef unsigned long long u64;

#ifndef NULL
#define NULL     0
#endif

#ifndef __cplusplus

#ifndef bool
typedef unsigned char bool;
#endif

#ifndef FALSE
#define FALSE     0
#endif
#ifndef TRUE
#define TRUE     (!FALSE)
#endif

#ifndef	false
#define false     FALSE
#endif
#ifndef	true
#define true     TRUE
#endif

#endif

// There is no way to directly recognise whether a typedef is defined
// http://stackoverflow.com/questions/3517174/how-to-check-if-a-datatype-is-defined-with-typedef
#ifdef __GNUC__
typedef    u16    wchar_t;
#endif

#ifndef WIN32
typedef u32 size_t;
#endif

#define U32_MAX ((u32)0xffffffff)
#define U16_MAX ((u16)0xffff)
#define U8_MAX ((u8)0xff)
#define U31_MAX ((u32)0x7fffffff)
#define U15_MAX ((u16)0x7fff)
#define U7_MAX ((u8)0x7f)


#ifdef WIN32
#   ifndef FALSE
#        define FALSE 0
#    endif

#   ifndef TRUE
#        define TRUE 1
#   endif
#endif

typedef u32 UTCTime;
typedef u32 arg_t;
typedef u32 status_t;


