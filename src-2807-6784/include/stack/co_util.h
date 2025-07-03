/*********************************************************************
 * @file co_util.h
 * @brief 
 * @version 1.0
 * @date 14/11/3 19:33:49
 * @author XIHAO
 *
 * @note 
 */

#ifndef __CO_UTIL_H__
#define __CO_UTIL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "co.h"
#include "xh98xx.h"

/*********************************************************************
 * MACROS
 */
#define co_type_cpy(dst, src, type)         memcpy(dst, src, sizeof(type))
#define co_type_set(dst, val, type)         memset(dst, val, sizeof(type))

#define countof(s)                          (sizeof(s)/sizeof((s)[0]))

#ifndef offsetof
#define offsetof(type, member)              ((unsigned int) &((type *)0)->member)
#endif
#define container_of(ptr, type, member)     ((type *)((char *)(ptr) - offsetof(type,member)))

#if !defined(MAX)
#define MAX(x, y)                           (((x) > (y)) ? (x) : (y))
#define MIN(x, y)                           (((x) < (y)) ? (x) : (y))
#endif

#if defined(CONFIG_Xihao_HOST_ADVANCED) //luwei: bluedroid
#include "bt_types.h"
#else
#define UINT32_TO_STREAM(p, u32)            {*(p)++ = (uint8_t)(u32); *(p)++ = (uint8_t)((u32) >> 8); *(p)++ = (uint8_t)((u32) >> 16); *(p)++ = (uint8_t)((u32) >> 24);}
#define UINT24_TO_STREAM(p, u24)            {*(p)++ = (uint8_t)(u24); *(p)++ = (uint8_t)((u24) >> 8); *(p)++ = (uint8_t)((u24) >> 16);}
#define UINT16_TO_STREAM(p, u16)            {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}
#define UINT8_TO_STREAM(p, u8)              {*(p)++ = (uint8_t)(u8);}
#define INT8_TO_STREAM(p, u8)               {*(p)++ = (int8_t)(u8);}
#define ARRAY32_TO_STREAM(p, a)             {register int ijk; for (ijk = 0; ijk < 32;  ijk++) *(p)++ = (uint8_t) a[31 - ijk];}
#define ARRAY16_TO_STREAM(p, a)             {register int ijk; for (ijk = 0; ijk < 16;  ijk++) *(p)++ = (uint8_t) a[15 - ijk];}
#define ARRAY8_TO_STREAM(p, a)              {register int ijk; for (ijk = 0; ijk < 8;   ijk++) *(p)++ = (uint8_t) a[7 - ijk];}
#define ARRAY_TO_STREAM(p, a, len)          {register int ijk; for (ijk = 0; ijk < len; ijk++) *(p)++ = (uint8_t) a[ijk];}
#define REVERSE_ARRAY_TO_STREAM(p, a, len)  {register int ijk; for (ijk = 0; ijk < len; ijk++) *(p)++ = (uint8_t) a[len - 1 - ijk];}

#define STREAM_TO_UINT8(u8, p)              {u8 = (uint8_t)(*(p)); (p) += 1;}
#define STREAM_TO_UINT16(u16, p)            {u16 = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); (p) += 2;}
#define STREAM_TO_UINT24(u32, p)            {u32 = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) ); (p) += 3;}
#define STREAM_TO_UINT32(u32, p)            {u32 = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) + ((((uint32_t)(*((p) + 3)))) << 24)); (p) += 4;}
#define STREAM_TO_ARRAY32(a, p)             {register int ijk; register uint8_t *_pa = (uint8_t *)a + 31; for (ijk = 0; ijk < 32; ijk++) *_pa-- = *p++;}
#define STREAM_TO_ARRAY16(a, p)             {register int ijk; register uint8_t *_pa = (uint8_t *)a + 15; for (ijk = 0; ijk < 16; ijk++) *_pa-- = *p++;}
#define STREAM_TO_ARRAY8(a, p)              {register int ijk; register uint8_t *_pa = (uint8_t *)a + 7;  for (ijk = 0; ijk < 8;  ijk++) *_pa-- = *p++;}
#define STREAM_TO_ARRAY(a, p, len)          {register int ijk; for (ijk = 0; ijk < len; ijk++) ((uint8_t *) a)[ijk] = *p++;}
#define REVERSE_STREAM_TO_ARRAY(a, p, len)  {register int ijk; register uint8_t *_pa = (uint8_t *)a + len - 1; for (ijk = 0; ijk < len; ijk++) *_pa-- = *p++;}

#define UINT32_TO_BE_STREAM(p, u32)         {*(p)++ = (uint8_t)((u32) >> 24);  *(p)++ = (uint8_t)((u32) >> 16); *(p)++ = (uint8_t)((u32) >> 8); *(p)++ = (uint8_t)(u32); }
#define UINT24_TO_BE_STREAM(p, u24)         {*(p)++ = (uint8_t)((u24) >> 16); *(p)++ = (uint8_t)((u24) >> 8); *(p)++ = (uint8_t)(u24);}
#define UINT16_TO_BE_STREAM(p, u16)         {*(p)++ = (uint8_t)((u16) >> 8); *(p)++ = (uint8_t)(u16);}
#define UINT8_TO_BE_STREAM(p, u8)           {*(p)++ = (uint8_t)(u8);}
#define ARRAY_TO_BE_STREAM(p, a, len)       {register int ijk; for (ijk = 0; ijk < len; ijk++) *(p)++ = (uint8_t) a[ijk];}

#define BE_STREAM_TO_UINT8(u8, p)           {u8 = (uint8_t)(*(p)); (p) += 1;}
#define BE_STREAM_TO_UINT16(u16, p)         {u16 = (uint16_t)(((uint16_t)(*(p)) << 8) + (uint16_t)(*((p) + 1))); (p) += 2;}
#define BE_STREAM_TO_UINT24(u32, p)         {u32 = (((uint32_t)(*((p) + 2))) + ((uint32_t)(*((p) + 1)) << 8) + ((uint32_t)(*(p)) << 16)); (p) += 3;}
#define BE_STREAM_TO_UINT32(u32, p)         {u32 = ((uint32_t)(*((p) + 3)) + ((uint32_t)(*((p) + 2)) << 8) + ((uint32_t)(*((p) + 1)) << 16) + ((uint32_t)(*(p)) << 24)); (p) += 4;}
#define BE_STREAM_TO_ARRAY(p, a, len)       {register int ijk; for (ijk = 0; ijk < len; ijk++) ((uint8_t *) a)[ijk] = *p++;}
#endif

#define READ_UINT8(buffer)                  ( ((uint8_t *)(buffer))[0])
#define READ_UINT16(buffer)                 ( ((uint16_t) ((uint8_t *)(buffer))[0]) | (((uint16_t)((uint8_t *)(buffer))[1]) << 8))
#define READ_UINT24(buffer)                 ( ((uint32_t) ((uint8_t *)(buffer))[0]) | (((uint32_t)((uint8_t *)(buffer))[1]) << 8) | (((uint32_t)((uint8_t *)(buffer))[2]) << 16))
#define READ_UINT32(buffer)                 ( ((uint32_t) ((uint8_t *)(buffer))[0]) | (((uint32_t)((uint8_t *)(buffer))[1]) << 8) | (((uint32_t)((uint8_t *)(buffer))[2]) << 16) | (((uint32_t) ((uint8_t *)(buffer))[3])) << 24)

#define STATIC_INLINE                       __STATIC_INLINE

/*********************************************************************
 * TYPEDEFS
 */
/*
 * Defentions for HCI Error Codes that are past in the events
 */
typedef enum
{
    ERR_SUCCESS                             = 0x00,
    ERR_PENDING                             = 0x00,
    ERR_ILLEGAL_COMMAND                     = 0x01,
    ERR_NO_CONNECTION                       = 0x02,
    ERR_HW_FAILURE                          = 0x03,
    ERR_PAGE_TIMEOUT                        = 0x04,
    ERR_AUTH_FAILURE                        = 0x05,
    ERR_KEY_MISSING                         = 0x06,
    ERR_MEMORY_FULL                         = 0x07,
    ERR_CONNECTION_TOUT                     = 0x08,
    ERR_MAX_NUM_OF_CONNECTIONS              = 0x09,
    ERR_MAX_NUM_OF_SCOS                     = 0x0A,
    ERR_CONNECTION_EXISTS                   = 0x0B,
    ERR_COMMAND_DISALLOWED                  = 0x0C,
    ERR_HOST_REJECT_RESOURCES               = 0x0D,
    ERR_HOST_REJECT_SECURITY                = 0x0E,
    ERR_HOST_REJECT_DEVICE                  = 0x0F,
    ERR_HOST_TIMEOUT                        = 0x10,
    ERR_UNSUPPORTED_VALUE                   = 0x11,
    ERR_ILLEGAL_PARAMETER_FMT               = 0x12,
    ERR_PEER_USER                           = 0x13,
    ERR_PEER_LOW_RESOURCES                  = 0x14,
    ERR_PEER_POWER_OFF                      = 0x15,
    ERR_CONN_CAUSE_LOCAL_HOST               = 0x16,
    ERR_REPEATED_ATTEMPTS                   = 0x17,
    ERR_PAIRING_NOT_ALLOWED                 = 0x18,
    ERR_UNKNOWN_LMP_PDU                     = 0x19,
    ERR_UNSUPPORTED_LMP_FEATURE             = 0x1A,
    ERR_SCO_OFFSET_REJECTED                 = 0x1B,
    ERR_SCO_INTERVAL_REJECTED               = 0x1C,
    ERR_SCO_AIR_MODE                        = 0x1D,
    ERR_INVALID_LMP_PARAM                   = 0x1E,
    ERR_UNSPECIFIED                         = 0x1F,
    ERR_UNSUPPORTED_PARAMETER_VALUE         = 0x20,
    ERR_ROLE_CHANGE_NOT_ALLOWED             = 0x21,
    ERR_LMP_RESPONSE_TIMEOUT                = 0x22,
    ERR_LMP_ERR_TRANS_COLLISION             = 0x23,
    ERR_LMP_PDU_NOT_ALLOWED                 = 0x24,
    ERR_ENCRY_MODE_NOT_ACCEPTABLE           = 0x25,
    ERR_UNIT_KEY_USED                       = 0x26,
    ERR_QOS_NOT_SUPPORTED                   = 0x27,
    ERR_INSTANT_PASSED                      = 0x28,
    ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED = 0x29,
    ERR_DIFF_TRANSACTION_COLLISION          = 0x2A,
    ERR_UNDEFINED_0x2B                      = 0x2B,
    ERR_QOS_UNACCEPTABLE_PARAM              = 0x2C,
    ERR_QOS_REJECTED                        = 0x2D,
    ERR_CHAN_CLASSIF_NOT_SUPPORTED          = 0x2E,
    ERR_INSUFFCIENT_SECURITY                = 0x2F,
    ERR_PARAM_OUT_OF_RANGE                  = 0x30,
    ERR_UNDEFINED_0x31                      = 0x31,
    ERR_ROLE_SWITCH_PENDING                 = 0x32,
    ERR_UNDEFINED_0x33                      = 0x33,
    ERR_RESERVED_SLOT_VIOLATION             = 0x34,
    ERR_ROLE_SWITCH_FAILED                  = 0x35,
    ERR_INQ_RSP_DATA_TOO_LARGE              = 0x36,
    ERR_SIMPLE_PAIRING_NOT_SUPPORTED        = 0x37,
    ERR_HOST_BUSY_PAIRING                   = 0x38,
    ERR_REJ_NO_SUITABLE_CHANNEL             = 0x39,
    ERR_CONTROLLER_BUSY                     = 0x3A,
    ERR_UNACCEPT_CONN_PARAM                 = 0x3B,
    ERR_DIRECTED_ADVERTISING_TIMEOUT        = 0x3C,
    ERR_CONN_TOUT_DUE_TO_MIC_FAILURE        = 0x3D,
    ERR_CONN_FAILED_ESTABLISHMENT           = 0x3E,
    ERR_MAC_CONNECTION_FAILED               = 0x3F,
}co_error_t;

typedef enum
{
    SOC_WORKMODE = 0,
    SOC_VHCI_WORKMODE,
    CONTROLLER_WORKMODE,
    HOST_WORKMODE,
    NETWORK_WORKMODE,
    IDLE_WORKMODE = 0xff,
}co_workmode_t;

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief co_ctz()
 *
 * @param[in] x  
 *
 * @return 
 **/
__STATIC_INLINE int co_ctz (uint32_t x)
{
    #if defined (__CC_ARM)
    #ifdef __RBIT // armv7+
    return __CLZ(__RBIT(x));
    #else
    int c = __CLZ(x & -x);
    return x ? 31 - c : c;
    #endif
    #elif defined(__GNUC__)
    return x ? __builtin_ctz(x) : 32;
    #else
    int n = 0;
    if (x == 0) return 32;
    if ((x & 0x0000FFFF) == 0) { n += 16; x >>= 16; }
    if ((x & 0x000000FF) == 0) { n +=  8; x >>=  8; }
    if ((x & 0x0000000F) == 0) { n +=  4; x >>=  4; }
    if ((x & 0x00000003) == 0) { n +=  2; x >>=  2; }
    if ((x & 0x00000001) == 0) { n +=  1; }
    return n;
    #endif // defined(__arm__)
}

/**
 * @brief co_array_left_move()
 *
 * @param[in] array  
 * @param[in] len  
 * @param[in] move  
 *
 * @return 
 **/
__STATIC_INLINE void co_array_left_move(void *array, unsigned len, unsigned move)
{
    memmove((uint8_t *)(array) - move, array, len);
}

/**
 * @brief co_array_reversal()
 *
 * @param[in] array  
 * @param[in] len  
 *
 * @return 
 **/
__STATIC_INLINE void co_array_reversal(void *array, unsigned len)
{
    unsigned half_len = len / 2;
    uint8_t *p = array;
    uint8_t temp;
    int i, j;

    for(i=0, j=len-1; i<half_len; ++i, --j)
    {
        temp = p[i];
        p[i] = p[j];
        p[j] = temp;
    }
}

/**
 * @brief co_array_is_all_zero()
 *
 * @param[in] array  
 * @param[in] len  
 *
 * @return 
 **/
__STATIC_INLINE bool co_array_is_all_zero(const void *array, unsigned len)
{
    while(len--)
        if(((uint8_t *)array)[len])
            return false;

    return true;
}

/**
 * @brief co_bdaddr_copy()
 *
 * @param[in] dst  
 * @param[in] src  
 *
 * @return 
 **/
__STATIC_INLINE void co_bdaddr_copy(bd_addr_t dst, const bd_addr_t src)
{
    co_type_cpy(dst, src, bd_addr_t);
}

/**
 * @brief co_bdaddr_same()
 *
 * @param[in] a  
 * @param[in] b  
 *
 * @return 
 **/
__STATIC_INLINE bool co_bdaddr_same(const bd_addr_t a, const bd_addr_t b)
{
    return memcmp(a, b, sizeof(bd_addr_t)) == 0;
}

/**
 * @brief co_crc16_ccitt()
 *
 * @param[in] crcinit  
 * @param[in] buf  
 * @param[in] len  
 *
 * @return 
 **/
uint16_t co_crc16_ccitt(uint16_t crcinit, const void *buf, unsigned len);

#ifdef __cplusplus
}
#endif

#endif

