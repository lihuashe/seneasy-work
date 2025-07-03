/**********************************************************************************************************************
 * @file     stack_utility.h
 * @version  V1.0
 * @date     2020/01/03
 * @history
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#ifndef __STACK_UTILITY_H__
#define __STACK_UTILITY_H__


/**********************************************************************************************************************
 * @brief  Compares the first n characters of the object pointed to by pu8Buf1 to the 
 *         first n characters of the object pointed to by pu8Buf2.
 * @param  pu8Buf1: The object pointed to by pu8Buf1.
 * @param  pu8Buf2: The object pointed to by pu8Buf2.
 * @param  u16Len: Length of buf.
 * @output 
 * @return uint8_t: The object pointed to by pu8Buf1 is equal to 
 *                  the object pointed to by pu8Buf2 return 0, or return 1.
 *********************************************************************************************************************/
extern uint8_t rom_stack_utility_memcmp2 ( const uint8_t* pu8Buf1, const uint8_t* pu8Buf2, uint16_t u16Len );


/**********************************************************************************************************************
 * @brief  Transform data of uint8_t to data of uint16_t by little endian.
 * @param  pu8Buf: Point to data buf
 * @param  pos: Position of data which transform
 * @output 
 * @return uint16_t
 *********************************************************************************************************************/
extern uint16_t rom_stack_utility_little_endian_read_16 ( const uint8_t* pu8Buf, int32_t pos );


/**********************************************************************************************************************
 * @brief  Transform data of uint8_t to data of uint24_t by little endian.
 * @param  pu8Buf: Point to data buf
 * @param  pos: Position of data which transform
 * @output 
 * @return uint24_t
 *********************************************************************************************************************/
extern uint32_t rom_stack_utility_little_endian_read_24 ( const uint8_t* pu8Buf, int32_t pos );


/**********************************************************************************************************************
 * @brief  Transform data of uint8_t to data of uint32_t by little endian.
 * @param  pu8Buf: Point to data buf
 * @param  pos: Position of data which transform
 * @output 
 * @return uint32_t
 *********************************************************************************************************************/
extern uint32_t rom_stack_utility_little_endian_read_32 ( const uint8_t* pu8Buf, int32_t pos );


/**********************************************************************************************************************
 * @brief  Transform data of uint16_t to data of uint8_t by little endian.
 * @param  pu8Buf: Point to data buf of uint8_t
 * @param  pos: Position of data which transform
 * @param  value: The data of uint16_t
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void rom_stack_utility_little_endian_store_16 ( uint8_t* pu8Buf, int32_t pos, uint16_t value );


/**********************************************************************************************************************
 * @brief  Transform data of uint32_t to data of uint8_t by little endian.
 * @param  pu8Buf: Point to data buf of uint8_t
 * @param  pos: Position of data which transform
 * @param  value: The data of uint32_t
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void rom_stack_utility_little_endian_store_32 ( uint8_t* pu8Buf, int32_t pos, uint32_t value );


/**********************************************************************************************************************
 * @brief  Get mode
 * @param  value:
 * @param  mod:
 * @output 
 * @return uint32_t:
 *********************************************************************************************************************/
extern uint32_t rom_stack_utility_get_mod ( uint32_t value, uint32_t mod );
 


/**
 * @brief Swap buffer content
 *
 * In-place memory swap, where final content will be reversed.
 * I.e.: buf[n] will be put in buf[end-n]
 * Where n is an index and 'end' the last index of buf.
 *
 * @param buf A valid pointer on a memory area to swap
 * @param length Size of buf memory area
 */
static inline void sys_mem_swap ( void* buf, uint16_t length )
{
    uint16_t i;

    for ( i = 0; i < ( length / 2 ); i++ )
    {
        uint8_t tmp = ( ( uint8_t* ) buf ) [i];

        ( ( uint8_t* ) buf ) [i] = ( ( uint8_t* ) buf ) [length - 1 - i];
        ( ( uint8_t* ) buf ) [length - 1 - i] = tmp;
    }
}
/**
 * @brief Swap one buffer content into another
 *
 * Copy the content of src buffer into dst buffer in reversed order,
 * i.e.: src[n] will be put in dst[end-n]
 * Where n is an index and 'end' the last index in both arrays.
 * The 2 memory pointers must be pointing to different areas, and have
 * a minimum size of given length.
 *
 * @param dst A valid pointer on a memory area where to copy the data in
 * @param src A valid pointer on a memory area where to copy the data from
 * @param length Size of both dst and src memory areas
 */
static inline void sys_memcpy_swap ( uint8_t* dst, const uint8_t* src, uint16_t length )
{
    src += length - 1;

    for ( ; length > 0; length-- )
    {
        * ( ( uint8_t* ) dst++ ) = * ( ( uint8_t* ) src-- );
    }
}


#endif /* __UTILITY_H__ */


