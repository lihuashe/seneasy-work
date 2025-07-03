
#include "stdint.h"
#include "board.h"
#include "ir_nec_encode.h"

#if IR_FUN


#define ABS_F(a,b) (a>b) ? (a-b):(b-a) ;

static uint32_t HL_Time2TxBufCount(uint32_t a, uint32_t b)
{
    return ((a & 0x80000000) | ((a & 0x7FFFFFFF) / b));
}

static uint8_t BIT_N(uint32_t a, uint32_t b)
{
    return ((a >> b) & 0x01);
}

const IRDA_SPEC NEC_SPEC =
{
    2,                                                 //  HeaderLen;
    {PULSE_HIGH | 9000, PULSE_LOW | (4500 - 26)  },    //uint32_t HeaderContext[MAX_HEADDER_LEN];
    {PULSE_HIGH | 560, PULSE_LOW | (560 - 26)    },    //Log0[MAX_LOG_WAVFORM_SIZE];
    {PULSE_HIGH | 560, PULSE_LOW | (1690 - 26)   },    //Log1[MAX_LOG_WAVFORM_SIZE];
    PULSE_HIGH | 560,
    560,
    4
};

static int irda_txbuf(IRDA_BUF *pIrdabuf, IRDA_SPEC *pSpec)
{
    int i = 0, n = 0;
    uint16_t  BaseTime = 1000 / pIrdabuf->CarrierFrequency_kHz;
    uint16_t  bufLen = 0;

    uint32_t  Log1[MAX_LOG_WAVFORM_SIZE];
    uint32_t  Log0[MAX_LOG_WAVFORM_SIZE];
    uint8_t   Code = 0;

    if (pIrdabuf->CarrierFrequency_kHz == 0)
    {
        return IRDA_ERROR;
    }

    BaseTime = 1000 / pIrdabuf->CarrierFrequency_kHz;

    for (i = 0; i < MAX_LOG_WAVFORM_SIZE; i++)
    {
        Log1[i] = HL_Time2TxBufCount(pSpec->Log1_Context[i],
                                     BaseTime); // (pSpec->Log1_Context[i] &0x80000000 )| ((pSpec->Log1_Time[i]&0x7FFFFFFF) / BaseTime);
        Log0[i] = HL_Time2TxBufCount(pSpec->Log0_Context[i],
                                     BaseTime); //(pSpec->Log0_Context[i] &0x80000000 )| ((pSpec->Log0_Time[i]&0x7FFFFFFF) / BaseTime);

    }

    //header
    for (i = 0; i < pSpec->HeaderLen; i++)
    {
        pIrdabuf->pBuf[i] =  HL_Time2TxBufCount(pSpec->Header_Context[i], BaseTime);
    }

    bufLen = pSpec->HeaderLen;
    for (i = 0; i < pIrdabuf->CodeLen; i++)
    {
        Code = pIrdabuf->Code[i];
        for (n = 0; n < 8; n++)
        {
            /* Change encoding bit order */
            if (BIT_N(Code, n) == 0x01) //if (BIT_N(Code,7-n)== 0x01)
            {
                //log 1
                pIrdabuf->pBuf[bufLen] = Log1[0];
                pIrdabuf->pBuf[bufLen + 1] = Log1[1];
            }
            else
            {
                //log 0
                pIrdabuf->pBuf[bufLen] = Log0[0];
                pIrdabuf->pBuf[bufLen + 1] = Log0[1];
            }
            bufLen += MAX_LOG_WAVFORM_SIZE;
        }
    }

    //Stop
    if (pSpec->Stop_Context != 0x0000)
    {
        pIrdabuf->pBuf[bufLen] = HL_Time2TxBufCount(pSpec->Stop_Context, BaseTime);
        bufLen += 1;
    }
    pIrdabuf->pBufLen = bufLen;

    return IRDA_SUCCEED;
}

int irda_nec_encode(uint8_t address, uint8_t command, IRDA_BUF *pIrda)
{
    int ret = 0;
    IRDA_BUF *nec_Irdabuf = pIrda;

    nec_Irdabuf->CarrierFrequency_kHz = 38; //38kHz
    nec_Irdabuf->CodeLen = 4;
    nec_Irdabuf->Code[0] = address;
    nec_Irdabuf->Code[1] = ~address;
    nec_Irdabuf->Code[2] = command;
    nec_Irdabuf->Code[3] = ~command;

    ret = irda_txbuf(nec_Irdabuf, (IRDA_SPEC *)&NEC_SPEC);

#ifdef DBG_ON
    DBGPRINTF("=========== NEC Tx buf===========================\n");
    DBGPRINTF("address = 0x%x command = 0x%x \n", address, command);
    DBGPRINTF("BufLen= 0x%x\n", pIrda->pBufLen);

    for (i = 0; i < pIrda->pBufLen; i++)
    {
        if (i % 8 == 0)
        {
            DBGPRINTF("\n");
        }
        DBGPRINTF("%.4x ,", pIrda->pBuf[i]);

    }
    DBGPRINTF("\n");
    DBGPRINTF("============= ret = %d===============\n", ret);
#endif

    return ret;
}

#if IR_NEC_DECODE

static uint32_t HL_WAVE(uint32_t a)
{
    return ((a & 0x80000000) >> 15);
}

static uint32_t TxBufCount2Time(uint32_t a, uint32_t b)
{
    return ((a & 0x7FFFFFFF) * b);
}

int irda_invse_Pulse1(uint32_t *buf, uint16_t length)
{
    uint16_t i = 0;

    for (i = 0; i < length; i++)
    {
        if (buf[i] > PULSE_HIGH)
        {
            buf[i] = buf[i] & (~PULSE_HIGH);
        }
        else
        {
            buf[i] = buf[i] | PULSE_HIGH;
        }
    }
    return IRDA_SUCCEED;
}

static int irda_rxbuf_LogData_Check(uint32_t *buf,
                                    uint32_t *pLog,
                                    uint16_t LogWavFormLen,
                                    uint32_t BaseTime,
                                    uint32_t Dif_Base,
                                    uint32_t Dif_BaseTime)
{
    uint16_t  Time1 = 0, Time2 = 0;
    uint16_t  i = 0;
    uint16_t  BufValue;
    uint16_t  Dif = 0;
    uint16_t  temp = 0;
    for (i = 0; i < LogWavFormLen; i++)
    {
        BufValue = buf[i];

        //check H_L
        if (HL_WAVE(BufValue) != HL_WAVE(pLog[i]))
        {
            return -1;
        }
        //Check Time
        Time1 =  TxBufCount2Time(BufValue, BaseTime);
        Time2 =  pLog[i] & 0x7FFFFFFF;

        Dif = (Time2 / Dif_Base) * Dif_BaseTime;

        temp =  ABS_F(Time1, Time2);
        if (temp > Dif)
        {
            return -1;
        }
    }

    return 0;
}

static int irda_rxbuf(IRDA_BUF *pIrdabuf, IRDA_SPEC *pSpec)
{
    uint32_t  BaseTime = 0;
    uint32_t  Dif_BaseTime = 0;
    uint32_t  Dif = 0;
    uint32_t  BufCount = 0;
    uint32_t  temp = 0;
    uint32_t  Time1, Time2;
    uint8_t   BitNum = 0;
    uint8_t   ByteNum = 0;
    uint8_t   data;
    uint8_t   BIT_TYPE = 0;
    uint16_t  BufLen = 0;

    if (pIrdabuf->CarrierFrequency_kHz == 0)
    {
        return IRDA_ERROR;
    }

    if (irda_invse_Pulse1(pIrdabuf->pBuf, pIrdabuf->pBufLen) != 0)
    {
        return IRDA_ERROR;
    }

    BaseTime = 1000 / pIrdabuf->CarrierFrequency_kHz;

    if (pSpec->Dif_Divisor == 0)
    {
        pSpec->Dif_Divisor = 1;
    }

    Dif_BaseTime = pSpec->Dif_Base / pSpec->Dif_Divisor;

    if (pSpec->Stop_Context != 0x0000)
    {
        BufLen = pIrdabuf->pBufLen -= 2;
    }
    else
    {
        BufLen = pIrdabuf->pBufLen - 1;
    }

    //Check header
    while (BufCount < pSpec->HeaderLen)
    {
        if (HL_WAVE(pIrdabuf->pBuf[BufCount]) != HL_WAVE(pIrdabuf->pBuf[BufCount]))
        {
            return IRDA_HEADER_ERROR;
        }

        Time1 =  TxBufCount2Time(pIrdabuf->pBuf[BufCount], BaseTime);
        Time2 =  pSpec->Header_Context[BufCount] & 0x7FFFFFFF;

        Dif = Time2 / (pSpec->Dif_Base) * Dif_BaseTime;

        temp =  ABS_F(Time1, Time2);
        // printf("[%d]%d",BufCount,temp);

        if (temp > Dif)
        {
            return IRDA_HEADER_ERROR;
        }
        BufCount++;
    }

    BitNum = 0;
    ByteNum = 0;
    data = 0;

    while (BufCount < BufLen)
    {
        //check log 0
        if (irda_rxbuf_LogData_Check(&pIrdabuf->pBuf[BufCount],
                                     pSpec->Log0_Context,
                                     MAX_LOG_WAVFORM_SIZE,
                                     BaseTime,
                                     pSpec->Dif_Base,
                                     Dif_BaseTime) == 0)
        {
            BIT_TYPE = LOG_LOW;
            BufCount += MAX_LOG_WAVFORM_SIZE;
        }
        else if (irda_rxbuf_LogData_Check(&pIrdabuf->pBuf[BufCount],
                                          pSpec->Log1_Context,
                                          MAX_LOG_WAVFORM_SIZE,
                                          BaseTime,
                                          pSpec->Dif_Base,
                                          Dif_BaseTime) == 0)
        {
            BIT_TYPE = LOG_HIGH;
            BufCount += MAX_LOG_WAVFORM_SIZE;
        }
        else
        {
//            DBG_BUFFER(MODULE_IR,
//                       LEVEL_ERROR,
//                       "IRDA_DATA_ERROR",
//                       0);
            return IRDA_DATA_ERROR;
        }
        /* Change data's bit order */
        data |= (BIT_TYPE << BitNum);
        //data |= (BIT_TYPE<<(7-BitNum));
        BitNum++;
#ifdef DBG_ON
        DBGPRINTF("(%d/%d) data = %x --> %d \n", BitNum, ByteNum, data, BIT_TYPE);
#endif
        if (BitNum >= 8)
        {
            pIrdabuf->Code[ByteNum] = data;
            ByteNum++;
            BitNum = 0;
            data = 0;
            pIrdabuf->CodeLen = ByteNum;

        }
    }
    return IRDA_SUCCEED;
}
int irda_nec_decode(uint8_t *address, uint8_t *command, IRDA_BUF *pIrda)
{
//  IRDA_BUF *nec_Irdabuf = &Irdabuf;
    int ret = 0;
    //pIrda->CarrierFrequency_kHz = pIrda->CarrierFrequency_kHz;
    pIrda->CodeLen = 0;
    pIrda->Code[0] = 0;
    pIrda->Code[1] = 0;
    pIrda->Code[2] =  0;
    pIrda->Code[3] =  0;

    ret = irda_rxbuf(pIrda, (IRDA_SPEC *)&NEC_SPEC);

    *address = pIrda->Code[0];
    *command =  pIrda->Code[2];

#ifdef DBG_ON
    DBGPRINTF("=========== NEC Rx Code===========================\n");
    DBGPRINTF("address = 0x%x command = 0x%x \n", *address, *command);
    DBGPRINTF("\n");
#endif
    return ret;
}
#endif /*endif @IR_NEC_DECODE*/

#endif /*endif @IR_FUN*/
