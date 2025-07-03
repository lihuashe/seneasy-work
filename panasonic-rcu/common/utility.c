/********************************************************************************************************
 * @file    utility.c
 *
 * @brief   This is the source file for B85
 *
 * @author  BLE GROUP
 * @date    06,2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#include "utility.h"
#include <string.h>




// general swap/endianness utils

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

	if ((unsigned int)n >= f->size)
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

const char *hex_to_str(const void *buf, u8 len)
{
	static const char hex[] = "0123456789abcdef";
	static char str[301];
	const u8 *b = buf;
	u8 i;

	len = min(len, (sizeof(str) - 1) / 3);

	for (i = 0; i < len; i++) {
		str[i * 3]     = hex[b[i] >> 4];
		str[i * 3 + 1] = hex[b[i] & 0xf];
		str[i * 3 + 2] = ' ';
	}

	str[i * 3] = '\0';

	return str;
}



// 初始化环形缓冲区
void RingBuffer_Init(RingBuffer *rbuf, u8 *ext_buf, u32 size) {
    rbuf->buffer = ext_buf;
    rbuf->size = size;
    rbuf->head = 0;
    rbuf->tail = 0;
}

// 判断缓冲区是否为空
bool RingBuffer_IsEmpty(const RingBuffer *rbuf) {
    return (rbuf->head == rbuf->tail);
}

// 判断缓冲区是否已满
bool RingBuffer_IsFull(const RingBuffer *rbuf) {
    return ((rbuf->head + 1) % rbuf->size) == rbuf->tail;
}

// 获取可用空间（可写入字节数）
u32 RingBuffer_Avail(const RingBuffer *rbuf) {
    ENTER_CRITICAL();
    u32 used = (rbuf->head >= rbuf->tail) ? 
                   (rbuf->head - rbuf->tail) : 
                   (rbuf->size - rbuf->tail + rbuf->head);
    EXIT_CRITICAL();
    return (rbuf->size - 1 - used); // 保留一个位置区分空/满
}

// 获取已用空间（可读取字节数）
u32 RingBuffer_Used(const RingBuffer *rbuf) {
    ENTER_CRITICAL();
    u32 used = (rbuf->head >= rbuf->tail) ? 
                   (rbuf->head - rbuf->tail) : 
                   (rbuf->size - rbuf->tail + rbuf->head);
    EXIT_CRITICAL();
    return used;
}

// 写入数据（返回实际写入长度）
bool RingBuffer_Write(RingBuffer *rbuf, const u8 *data, u32 len) {
    ENTER_CRITICAL();
    
    u32 avail = RingBuffer_Avail(rbuf);
    if (avail < len) {
        EXIT_CRITICAL();
        return false; // 空间不足
    }

    u32 offset = 0;
    while (len--) {
        rbuf->buffer[rbuf->head] = data[offset++];
        rbuf->head = (rbuf->head + 1) % rbuf->size;
    }
    
    EXIT_CRITICAL();
    return true;
}

// 读取数据（返回实际读取长度）
u32 RingBuffer_Read(RingBuffer *rbuf, u8 *data, u32 max_len) {
    ENTER_CRITICAL();
    
    u32 used = RingBuffer_Used(rbuf);
    u32 read_len = (used > max_len) ? max_len : used;
    
    u32 offset = 0;
    while (read_len--) {
        data[offset++] = rbuf->buffer[rbuf->tail];
        rbuf->tail = (rbuf->tail + 1) % rbuf->size;
    }
    
    EXIT_CRITICAL();
    return offset;
}

// 查看数据不移除（Peek）
u32 RingBuffer_Peek(const RingBuffer *rbuf, u8 *data, u32 max_len) {
    ENTER_CRITICAL();
    
    u32 used = RingBuffer_Used(rbuf);
    u32 copy_len = (used > max_len) ? max_len : used;
    u32 temp_tail = rbuf->tail;
    
    for (u32 i = 0; i < copy_len; i++) {
        data[i] = rbuf->buffer[temp_tail];
        temp_tail = (temp_tail + 1) % rbuf->size;
    }
    
    EXIT_CRITICAL();
    return copy_len;
}

// 清空缓冲区
void RingBuffer_Clear(RingBuffer *rbuf) {
    ENTER_CRITICAL();
    rbuf->head = 0;
    rbuf->tail = 0;
    EXIT_CRITICAL();
}

/**
 *
 */
uint8_t get_checksum(void *p, uint8_t p_len)
{
    uint8_t *payload = p;
    uint32_t checksum = 0;
    for (size_t i = 0; i < p_len; i++)
    {
        checksum += payload[i];
    }
    return checksum & 0xFF;
}