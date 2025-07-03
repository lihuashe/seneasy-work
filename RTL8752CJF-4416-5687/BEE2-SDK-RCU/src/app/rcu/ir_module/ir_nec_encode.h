
#ifndef _IR_NEC_ENCODE_H_
#define _IR_NEC_ENCODE_H_

#include "stdint.h"

#define  IR_NEC_DECODE    0

#define PULSE_HIGH                  ((uint32_t)0x80000000)
#define PULSE_LOW                   0x0
#define LOG_HIGH                    1
#define LOG_LOW                     0

#define MAX_HEADDER_LEN             16
#define MAX_LOG_WAVFORM_SIZE        2
#define MAX_CODE_SIZE               12

typedef struct
{
    uint16_t  HeaderLen;
    uint32_t Header_Context[MAX_HEADDER_LEN];
    uint32_t Log0_Context[MAX_LOG_WAVFORM_SIZE];
    uint32_t Log1_Context[MAX_LOG_WAVFORM_SIZE];
    uint32_t Stop_Context;
    uint32_t Dif_Base;
    uint32_t Dif_Divisor;
} IRDA_SPEC;

typedef struct
{
    uint8_t    CarrierFrequency_kHz;
    uint8_t    Code[MAX_CODE_SIZE];
    uint8_t    CodeLen;
    uint16_t   pBufLen;
    //uint16_t   pBuf[MAX_IRDA_BUF_SIZE];
    uint32_t   *pBuf;
} IRDA_BUF;

enum irda_ret
{
    IRDA_ERROR        = -1,
    IRDA_SUCCEED      = 0,
    IRDA_HEADER_ERROR = 1,
    IRDA_DATA_ERROR
};

int irda_nec_encode(uint8_t address, uint8_t command, IRDA_BUF *pIrda);

#endif
