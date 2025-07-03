
#include "rtl876x.h"
#include "ir_repeat_code.h"
/*!
* @ User can modify this data structure to send user-defined repeat code!!!
* @ brief: IR repeat code protocol structure.
* @ note: Store parameters of repeat code protocol.
* @ parameters Configuration:
* @ PULSE_HIGH|9000 is 9ms of high voltage.
* @ PULSE_LOW|2250 is low 2.25ms of low voltage.
* @ ULSE_HIGH |560 is 560us of high voltage.
*/
const IR_RepeatCodeTypeDef REPEAT_CODE_PROTOCOL =
{
    3,                                                 /* length of repeat code */
    {PULSE_HIGH | 9000, PULSE_LOW | 2250, PULSE_HIGH | 560}  /* Buffer of repeat code: high 9ms, low 2.25ms, high 560us */
};

/**
  * @brief    unit convert.
  * @param  time: time of waveform.
  * @param  carrier_cycle: cycle of carrier.
  * @retval   vaule of data whose unit is cycle of carrier.
  */
static uint32_t ConvertToCarrierCycle(uint32_t time, uint32_t freq)
{
    return ((time & PULSE_HIGH) | ((time & IR_DATA_MSK) * freq / 1000));
}


/**
  * @brief    Encode repeat code to packet.
  * @param  IR_DataStruct: pointer to struct which store repeat code data.
  * @param  IR_Protocol: pointer to specify IR protocol structure.
  * @retval None
  */
static bool Repeat_Code_EncodePacket(IR_RepeatDataTypeDef *IR_DataStruct,
                                     IR_RepeatCodeTypeDef *IR_Protocol)
{
    uint16_t index = 0;

    /* Error handle */
    if (IR_DataStruct->carrierFreq == 0)
    {
        return false;
    }

    /* Encode repeat code */
    for (index = 0; index < IR_Protocol->RepeatCodeLen; index++)
    {
        IR_DataStruct->irBuf[index] =  ConvertToCarrierCycle(IR_Protocol->RepeatCodeBuf[index],
                                                             IR_DataStruct->carrierFreq);
    }

    IR_DataStruct->bufLen = IR_Protocol->RepeatCodeLen;


#if SOFTWARE_AVOIDANCE
    IR_DataStruct->irBuf[index] = 0;
    IR_DataStruct->bufLen++;
#endif
    //APP_PRINT_INFO1("IR Repeat code len:%d!!!", IR_DataStruct->bufLen);
    return true;
}

/**
  * @brief  Encode data of repeat code protocol.
  * @param  frequency: carrier frequency whose uinit is KHz.
  * @param  IR_DataStruct: pointer to struct which store repeat code data.
  * @retval None
  */
bool IR_Repeat_Code_Encode(uint8_t frequency_KHz, IR_RepeatDataTypeDef *IR_DataStruct)
{
    IR_DataStruct->carrierFreq = frequency_KHz;

    return Repeat_Code_EncodePacket(IR_DataStruct, (IR_RepeatCodeTypeDef *)(&REPEAT_CODE_PROTOCOL));
}
