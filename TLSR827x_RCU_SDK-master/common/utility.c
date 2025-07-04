/******************************************************************************
 * @file     utility.c
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

#include "../tl_common.h"
#include "drivers.h"
#include "utility.h"





/****************************************************************************
 * @fn          addrExtCmp
 *
 * @brief       Compare two extended addresses.
 *
 * input parameters
 *
 * @param       pAddr1        - Pointer to first address.
 * @param       pAddr2        - Pointer to second address.
 *
 * output parameters
 *
 * @return      TRUE if addresses are equal, FALSE otherwise
 */
u8 addrExtCmp(const u8 * pAddr1, const u8 * pAddr2)
{
  u8 i;

  for (i = 8; i != 0; i--)
  {
    if (*pAddr1++ != *pAddr2++)
    {
      return FALSE;
    }
  }
  return TRUE;
}


/*
void freeTimerEvent(void **arg)
{
    if ( *arg != NULL ) {
#if (__DEBUG_BUFM__)
        if ( SUCCESS != ev_buf_free((u8*)*arg) ) {
            while(1);
        }
#else
        ev_buf_free((u8*)*arg);
#endif
        *arg = NULL;
    }
}
*/
void freeTimerTask(void **arg)
{
    if ( *arg == NULL ) {
        return;
    }
//    EV_SCHEDULE_HIGH_TASK((ev_task_callback_t)freeTimerEvent, (void*)arg);
}



// general swap/endianess utils

void swapN(unsigned char *p, int n)
{
    int i, c;
    for (i=0; i<n/2; i++)
    {
        c = p[i];
        p[i] = p[n - 1 - i];
        p[n - 1 - i] = c;
    }
}

void swapX(const u8 *src, u8 *dst, int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[len - 1 - i] = src[i];
}

void swap24(u8 dst[3], const u8 src[3])
{
    swapX(src, dst, 3);
}

void swap32(u8 dst[4], const u8 src[4])
{
    swapX(src, dst, 4);
}

void swap48(u8 dst[7], const u8 src[7])
{
    swapX(src, dst, 6);
}

void swap56(u8 dst[7], const u8 src[7])
{
    swapX(src, dst, 7);
}

void swap64(u8 dst[8], const u8 src[8])
{
    swapX(src, dst, 8);
}

void swap128(u8 dst[16], const u8 src[16])
{
    swapX(src, dst, 16);
}

void net_store_16(u8 *buffer, u16 pos, u16 value)
{
    buffer[pos++] = value >> 8;
    buffer[pos++] = value;
}


void flip_addr(u8 *dest, u8 *src){
    dest[0] = src[5];
    dest[1] = src[4];
    dest[2] = src[3];
    dest[3] = src[2];
    dest[4] = src[1];
    dest[5] = src[0];
}

void store_16(u8 *buffer, u16 pos, u16 value){
    buffer[pos++] = value;
    buffer[pos++] = value >> 8;
}



void my_fifo_init (my_fifo_t *f, int s, u8 n, u8 *p)
{
    f->size = s;
    f->num = n;
    f->wptr = 0;
    f->rptr = 0;
    f->p = p;
}

u8* my_fifo_wptr (my_fifo_t *f)
{
    if (((f->wptr - f->rptr) & 255) < f->num)
    {
        return f->p + (f->wptr & (f->num-1)) * f->size;
    }
    return 0;
}

void my_fifo_next (my_fifo_t *f)
{
    f->wptr++;
}

int my_fifo_push (my_fifo_t *f, u8 *p, int n)
{
    if (((f->wptr - f->rptr) & 255) >= f->num)
    {
        return -1;
    }

    if (n >= f->size)
    {
        return -1;
    }
    u8 *pd = f->p + (f->wptr++ & (f->num-1)) * f->size;
    *pd++ = n & 0xff;
    *pd++ = (n >> 8) & 0xff;
    memcpy (pd, p, n);
    return 0;
}

void my_fifo_pop (my_fifo_t *f)
{
    f->rptr++;
}

u8 * my_fifo_get (my_fifo_t *f)
{
    if (f->rptr != f->wptr)
    {
        u8 *p = f->p + (f->rptr & (f->num-1)) * f->size;
        return p;
    }
    return 0;
}
