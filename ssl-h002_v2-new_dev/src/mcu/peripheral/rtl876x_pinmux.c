/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_pinmux.c
* @brief    This file provides all the PINMUX firmware functions.
* @details
* @author   Yuan
* @date     2020-11-09
* @version  v1.0.0
*********************************************************************************************************
*/

#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"

const uint16_t PINADDR_TABLE[MAX_PIN_NUM] =  //static
{
    0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xa0, 0xa2, 0xa4,
    0xea, 0xec, 0xee, 0xf0, 0xf2, 0x80, 0x82, 0x84,
    0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce,
    0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x00,
    0xa6, 0xa8, 0xaa, 0xac, 0xd0, 0x96, 0x98
};

const uint16_t WKSTATUS_TABLE[MAX_PIN_NUM] =
{
    //       Px-0                Px-1               Px-2                Px-3                Px-4                Px-5                Px-6                Px-7
    ((0 << 12) | 0xf4), ((1 << 12) | 0xf4), ((2 << 12) | 0xf4), ((3 << 12) | 0xf4), ((4 << 12) | 0xf4), ((0 << 12) | 0xae), ((1 << 12) | 0xae), ((2 << 12) | 0xae),
    ((5 << 12) | 0xf4), ((6 << 12) | 0xf4), ((7 << 12) | 0xf4), ((8 << 12) | 0xf4), ((9 << 12) | 0xf4), ((0 << 12) | 0x9a), ((1 << 12) | 0x9a), ((2 << 12) | 0x9a),
    ((0 << 12) | 0xd2), ((1 << 12) | 0xd2), ((2 << 12) | 0xd2), ((3 << 12) | 0xd2), ((4 << 12) | 0xd2), ((5 << 12) | 0xd2), ((6 << 12) | 0xd2), ((7 << 12) | 0xd2),
    ((3 << 12) | 0x9a), ((4 << 12) | 0x9a), ((5 << 12) | 0x9a), ((6 << 12) | 0x9a), ((7 << 12) | 0x9a), ((8 << 12) | 0x9a), ((9 << 12) | 0x9a), ((0 << 00) | 0x00),
    ((3 << 12) | 0xae), ((4 << 12) | 0xae), ((5 << 12) | 0xae), ((6 << 12) | 0xae), ((8 << 12) | 0xd2), ((11 << 12) | 0x9a), ((12 << 12) | 0x9a)
};

/**
  * @brief  Reset all pin to default value.
  * @param  None.
  * @note: two SWD pins will also be reset. Please use this function carefully.
  * @retval None
  */
void Pinmux_Reset(void)
{
    uint8_t i;

    for (i = 0; i < 10; i++)
    {
        PINMUX->CFG[i] = 0x00;
    }

    return;
}

/**
  * @brief  Deinit the IO function of one pin.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval None
  */
void Pinmux_Deinit(uint8_t Pin_Num)
{
    uint8_t pinmux_reg_num;

    pinmux_reg_num = Pin_Num >> 2;
    PINMUX->CFG[pinmux_reg_num] &= ~(0xff << ((Pin_Num % 4) << 3));
    return;
}

/**
  * @brief  Config pin to its corresponding IO function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  Pin_Func: mean one IO function, please refer to rtl876x_pinmux.h "Pin_Function_Number" part.
  * @retval None
  */
void Pinmux_Config(uint8_t Pin_Num, uint8_t Pin_Func)
{
    uint8_t pinmux_reg_num;
    uint8_t reg_offset;

    pinmux_reg_num = Pin_Num >> 2;
    reg_offset = (Pin_Num & 0x03) << 3;

    PINMUX->CFG[pinmux_reg_num] = (PINMUX->CFG[pinmux_reg_num] & ~(0xFF << reg_offset))
                                  | Pin_Func << reg_offset;

    return;
}

/**
  * @brief  config the corresponding pad.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  AON_PAD_MODE: use software mode or pinmux mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_SW_MODE: use software mode.
  *     @arg PAD_PINMUX_MODE: use pinmux mode.
  * @param  AON_PAD_PwrOn: config power of pad.
  *     This parameter can be one of the following values:
  *     @arg PAD_NOT_PWRON: shutdown power of pad.
  *     @arg PAD_IS_PWRON: enable power of pad.
  * @param  AON_PAD_Pull: config pad pull mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_PULL_NONE: no pull.
  *     @arg PAD_PULL_UP: pull this pin up.
  *     @arg PAD_PULL_DOWN: pull thi pin down.
  * @param  AON_PAD_E: config pad out put function.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_DISABLE: disable pin output.
  *     @arg PAD_OUT_ENABLE: enable pad output.
  * @param  AON_PAD_O: config pin output level.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_LOW: pad output low.
  *     @arg PAD_OUT_HIGH: pad output high.
  * @retval None
  */

void Pad_Config(uint8_t Pin_Num,
                PAD_Mode AON_PAD_Mode,
                PAD_PWR_Mode AON_PAD_PwrOn,
                PAD_Pull_Mode AON_PAD_Pull,
                PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                PAD_OUTPUT_VAL AON_PAD_O)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);

    /* Clear reg value first*/
    tmpVal &= ~0xF;

    /* Pull Config */
    if (AON_PAD_Pull == PAD_PULL_UP)
    {
        tmpVal |= Pull_En;
    }
    else if (AON_PAD_Pull == PAD_PULL_DOWN)
    {
        tmpVal |= (Pull_En | Pull_Direction);
    }

    /* Output Config */
    tmpVal |= (AON_PAD_O | (AON_PAD_E << 1));

    if (AON_PAD_Mode)
    {
        tmpVal |= Pin_Mode;
    }
    else
    {
        tmpVal &= ~Pin_Mode;
    }

    btaon_fast_write_safe(addr, tmpVal | SHDN);

    /* Pad control mode */
    if (AON_PAD_PwrOn == PAD_NOT_PWRON)
    {
        tmpVal &= ~SHDN;
        btaon_fast_write_safe(addr, tmpVal);
    }

}

//1ms~64ms
static void System_WakeUpDebounceTime(uint8_t time)
{
    uint16_t  tmpVal;
    tmpVal = btaon_fast_read_safe(0x2a);
    /* clear reg value first */
    tmpVal &= ~(0x3f00);
    /* set value */
    tmpVal |= (time & 0x3f) << 8;
    /* clear debounce status */
    tmpVal |= (BIT15 | BIT14);
    btaon_fast_write_safe(0x2a, tmpVal);
}
/**
  * @brief  Enable pin wakeup function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  Polarity: PAD_WAKEUP_POL_HIGH--use high level wakeup, PAD_WAKEUP_POL_LOW-- use low level wakeup.
  * @param[in] DebounceEn: Enable delay function.
  *      \arg PAD_WK_DEBOUNCE_DISABLE: Disable delay function.
  *      \arg PAD_WK_DEBOUNCE_ENABLE: Enable delay function.
  * @param[in] DebounceTime: Set debounce time, range from 0~63ms.
  * @retval None
  */
void System_WakeUpPinEnable(uint8_t Pin_Num, uint8_t Polarity, uint8_t DebounceEn,
                            uint8_t DebounceTime)
{
    if (DebounceEn == ENABLE)
    {
        System_WakeUpPinDisable(Pin_Num);
        if (DebounceTime >= 64)
        {
            DebounceTime = 63;
        }
        System_WakeUpDebounceTime(DebounceTime);
    }
    else
    {
        System_DebounceWakeupStatus();
    }
    Pad_WakeupPolarityValue(Pin_Num, Polarity);
    Pad_WKDebounceConfig(Pin_Num, DebounceEn);
    Pad_WakeupEnableValue(Pin_Num, 1);
}

/**
  * @brief  Disable pin wakeup function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval None
  */
void System_WakeUpPinDisable(uint8_t Pin_Num)
{
    Pad_WakeupEnableValue(Pin_Num, 0);
}

/**
  * @brief  Check debounce wake up status.
  * @note:  Call this API will clear the debunce wakeup status bit.
  * @param  None
  * @retval Debounce wakeup status
  */
uint8_t System_DebounceWakeupStatus(void)
{
    return Pad_DebounceWakeupStatus();
}

/**
  * @brief  Check wake up pin interrupt status.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval Pin interrupt status
  */
uint8_t System_WakeUpInterruptValue(uint8_t Pin_Num)
{
    return Pad_WakeupInterruptValue(Pin_Num);
}

void Pad_OutputControlValue(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= Output_Val;
    }
    else
    {
        tmpVal &= ~Output_Val;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_OutputEnableValue(uint8_t Pin_Num, uint8_t value)//0xf6
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= Output_En;
    }
    else
    {
        tmpVal &= ~Output_En;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_PullEnableValue(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= Pull_En;
    }
    else
    {
        tmpVal &= ~Pull_En;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_PullUpOrDownValue(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= Pull_Direction;
    }
    else
    {
        tmpVal &= ~Pull_Direction;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_PullConfigValue(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= Pull_Resistance;
    }
    else
    {
        tmpVal &= ~(Pull_Resistance);
    }
    btaon_fast_write_safe(addr, tmpVal);
}

void Pad_PowerOrShutDownValue(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= SHDN;
    }
    else
    {
        tmpVal &= ~SHDN;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_ControlSelectValue(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= Pin_Mode;
    }
    else
    {
        tmpVal &= ~Pin_Mode;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_WakeupEnableValue(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= WakeUp_En;
    }
    else
    {
        tmpVal &= ~WakeUp_En;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_WakeupPolarityValue(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= WKPOL;
    }
    else
    {
        tmpVal &= ~WKPOL;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_WKDebounceConfig(uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal;
    uint8_t addr = PINADDR_TABLE[Pin_Num];

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= Pin_Debounce;
    }
    else
    {
        tmpVal &= ~Pin_Debounce;
    }
    btaon_fast_write_safe((addr), tmpVal);
}

uint8_t Pad_WakeupInterruptValue(uint8_t Pin_Num)
{
    uint16_t reg_temp;
    uint16_t bit_temp;
    uint16_t temp_value = 0;
    uint8_t int_value = RESET;

    reg_temp = (WKSTATUS_TABLE[Pin_Num] & ~(0xf000));
    bit_temp = BIT((WKSTATUS_TABLE[Pin_Num]  & (0xf000)) >> 12);
    temp_value = btaon_fast_read_safe(reg_temp);
    if (temp_value & bit_temp)
    {
        int_value = SET;
    }
    return int_value;
}

uint8_t Pad_DebounceWakeupStatus(void)
{
    uint16_t value16 = 0;
    uint8_t status_value = RESET;

    value16 = btaon_fast_read_safe(0x2a);
    if (value16 & BIT15)
    {
        status_value = SET;
    }
    //Write 1 to clear debounceWakeupStatus
    btaon_fast_write_safe(0x2a, (value16 | BIT15));
    return status_value;
}

void Pad_ClearWakeupINTPendingBit(uint8_t Pin_Num)
{
    uint16_t reg_temp;
    uint16_t bit_temp;
    uint16_t reg_value = 0;

    reg_temp = (WKSTATUS_TABLE[Pin_Num] & ~(0xf000));
    bit_temp = BIT((WKSTATUS_TABLE[Pin_Num]  & (0xf000)) >> 12);
    reg_value = btaon_fast_read_safe(reg_temp) | bit_temp;
    btaon_fast_write_safe(reg_temp, reg_value);
}

/**
  * @brief Clear all wake up pin interrupt pending bit.
  * @retun None
  */
void Pad_ClearAllWakeupINT(void)
{
    uint16_t tmpVal;

    tmpVal = btaon_fast_read_safe(0x9a);
    tmpVal |= 0x1BFF;
    btaon_fast_write_safe(0x9a, tmpVal);

    tmpVal = btaon_fast_read_safe(0xae);
    tmpVal |= 0x7F;
    btaon_fast_write_safe(0xae, tmpVal);

    tmpVal = btaon_fast_read_safe(0xd2);
    tmpVal |= 0x1FF;
    btaon_fast_write_safe(0xd2, tmpVal);

    tmpVal = btaon_fast_read_safe(0xf4);
    tmpVal |= 0x3FF;
    btaon_fast_write_safe(0xf4, tmpVal);
}

/**
  * @brief  Spic0 master enable.
  * @param  value: 0:Disable 1:Enable .
  * @retval None
  */
void Spic0_control(uint8_t value)
{
    if (value)
    {
        HAL_WRITE32(SYSTEM_REG_BASE, REG_TEST_MODE, HAL_READ32(SYSTEM_REG_BASE, REG_TEST_MODE) | BIT(24));
    }
    else
    {
        HAL_WRITE32(SYSTEM_REG_BASE, REG_TEST_MODE, HAL_READ32(SYSTEM_REG_BASE,
                                                               REG_TEST_MODE) & (~BIT(24)));
    }
}
/**
  * @brief  Spic1 master enable.
  * @param  value: 0:Disable 1:Enable .
  * @retval None
  */
void Spic1_control(uint8_t value)
{
    if (value)
    {
        HAL_WRITE32(SYSTEM_REG_BASE, REG_TEST_MODE, HAL_READ32(SYSTEM_REG_BASE, REG_TEST_MODE) | BIT(8));
    }
    else
    {
        HAL_WRITE32(SYSTEM_REG_BASE, REG_TEST_MODE, HAL_READ32(SYSTEM_REG_BASE, REG_TEST_MODE) & (~BIT(8)));
    }
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor *****END OF FILE****/
