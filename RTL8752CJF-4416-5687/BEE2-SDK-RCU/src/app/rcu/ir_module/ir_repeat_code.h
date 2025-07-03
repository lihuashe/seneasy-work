/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file         ir_repeat_code.h
* @brief
* @details
* @author       yuyin_zhang
* @date         2017-11-28
* @version      v1.0
* *********************************************************************************************************
*/


#ifndef _IR_REPEAT_CODE_
#define _IR_REPEAT_CODE_


/* Carrier waveform data type bit select */
#define PACK_TYPE               ((uint32_t)31)
/* Carrier waveform data type select */
#define PULSE_HIGH              ((uint32_t)0x80000000)
/* No carrier waveform data type select */
#define PULSE_LOW               0x0
/* IR data mask */
#define IR_DATA_MSK             0x7fffffffUL

#define MAX_REPEAT_CODE_LEN     4
#define SOFTWARE_AVOIDANCE      1


typedef struct
{
    uint32_t    RepeatCodeLen;
    uint32_t    RepeatCodeBuf[MAX_REPEAT_CODE_LEN];
} IR_RepeatCodeTypeDef;

typedef struct _IR_REPEAT_TYPE_
{
    /* Unit of carrierFreq is KHz */
    uint8_t     carrierFreq;
    uint32_t    irBuf[MAX_REPEAT_CODE_LEN];
    uint32_t    bufLen;
} IR_RepeatDataTypeDef;

bool IR_Repeat_Code_Encode(uint8_t frequency_KHz, IR_RepeatDataTypeDef *IR_DataStruct);


#endif
