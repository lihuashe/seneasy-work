/**********************************************************************************************************************
 * @file     hw_pmu.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/1/10
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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "type_def.h"
#include "utility.h"


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define ROM_LDO_ACT_SETUP_TIME_US          ( 200 )
#define ROM_LDO_DIG_SETUP_TIME_US          ( 200 )
#define ROM_LDO_RET_SETUP_TIME_US          ( 200 )

#define ROM_DCDC_ACT_SETUP_TIME_US         ( 300 )
#define ROM_DCDC_RET_SETUP_TIME_US         ( 300 )

#define ROM_RC_LCLK_OUTPUT_SETUP_TIME_US   ( 300 )
#define ROM_RC_HCLK_OUTPUT_SETUP_TIME_US   ( 100 )




/**********************************************************************************************************************
 *                                             PMU Interrupt Driver
 *********************************************************************************************************************/

/**
 * @brief  Get pmu interrupt flag(status) by read the PMU_LUT_INT_FLAG register.
 * @param  pu32IntMsk: Indicate which interrupt flag will be read
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_get_interrupt_flag(uint32_t *pu32IntMsk)
{
    if (NULL == pu32IntMsk)
    {
        return ERR_PARA_ERR;
    }

    *pu32IntMsk = PMU->PMU_LUT_ROW_INT_FLAG & PMU_INT_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Clear pmu interrupt flag(status) by write the PMU_LUT_INT_CLR register.
 * @param  u32IntMsk: Indicate which flag will be cleared.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_clear_interrupt_flag(uint32_t u32IntMsk)
{
    if (u32IntMsk > PMU_INT_MSK)
    {
        return ERR_PARA_ERR;
    }

    PMU->PMU_LUT_ROW_INT_CLR = u32IntMsk;

    return ERR_STA_OK;
}

/**
 * @brief  PMU wakeup source selection.
 * @param  u8LutIdx: lut index, need < 16.
 * @param  enTrigSel: Lut trigger selection, @ref PUM_LUT_TRIG_SEL_T.
 * @param  enLutTrigId: Lut trigger id selection, @ref EN_LUT_TRIG_ID_T.
 * @param  enLutAction: Lut action selection, @ref EN_LUT_ACT_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_wakeup_source(uint8_t u8LutIdx, PUM_LUT_TRIG_SEL_T enTrigSel,
                                          EN_LUT_TRIG_ID_T enLutTrigId, EN_LUT_ACT_T enLutAction)
{
    if (u8LutIdx >= PMU_LUT_MAX)
    {
        return ERR_PARA_ERR;
    }

    PMU->PMU_LUT_ROW[u8LutIdx] = (((enTrigSel & LUT_TRIG_SEL_MSK) << LUT_TRIG_SEL_SHIFT)
                                | ((enLutTrigId & LUT_TRIG_ID_MSK) << LUT_TRIG_ID_SHIFT)
                                | ((enLutAction & LUT_ACTION_MSK) << LUT_ACTION_SHIFT));

    return ERR_STA_OK;
}

/**
 * @brief  PMU gpio without debounce wakeup source selection.
 * @param  u8LutIdx: lut index, need < 16.
 * @param  enTrigSel: Lut trigger config, must select
 *                    LUT_TRIG_ID_GPIOA,
 *                    LUT_TRIG_ID_GPIOB,
 *                    LUT_TRIG_ID_GPIOC
 * @param  u32Pin: Which pin need to be configure, Only one pin can be configured at the same time, @ref EN_GPIO_PIN_T.
 * @param  u8LutAction: Lut action config, @ref EN_LUT_ACT_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_gpio_wakeup_source(uint8_t u8LutIdx, PUM_LUT_TRIG_SEL_T enTrigSel,
                                               uint32_t u32Pin, EN_LUT_ACT_T enLutAction)
{
    uint8_t i = 0;
    uint8_t u8PinNum = 0;

    if (PMU_LUT_MAX <= u8LutIdx)
    {
        return ERR_PARA_ERR;
    }

    switch (enTrigSel)
    {
        case LUT_TRIG_ID_GPIOA:
        {
            u8PinNum = GPIO_PORT_A_NUM;
            break;
        }
        case LUT_TRIG_ID_GPIOB:
        {
            u8PinNum = GPIO_PORT_B_NUM;
            break;
        }
        default: break;
    }

    for (i = 0; i < u8PinNum; i++)
    {
        if (u32Pin & (0x01 << i))
        {
            break;
        }
    }

    if (i < u8PinNum)
    {
        PMU->PMU_LUT_ROW[u8LutIdx] = (((enTrigSel & LUT_TRIG_SEL_MSK) << LUT_TRIG_SEL_SHIFT)
                                    | ((i & LUT_TRIG_ID_MSK) << LUT_TRIG_ID_SHIFT)
                                    | ((enLutAction & LUT_ACTION_MSK) << LUT_ACTION_SHIFT));
    }
    else
    {
        return ERR_PARA_ERR;
    }

    return ERR_STA_OK;
}




/**********************************************************************************************************************
                                             PMU Sram Power Driver
**********************************************************************************************************************/

/**
 * @brief  Set part of sram or all of them to retention.
 * @param  u32SramBlock: Blocks of ram which will keep retention.
 *         Each bit correspoding a SRAM block.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_sram_block_ret(uint32_t u32SramBlock)
{
    PMU->PMU_SRAM_RET_EN |= (u32SramBlock & SRAM_BLOCK_EN_MSK) << SRAM_BLOCK_EN_SHIT;

    return ERR_STA_OK;
}

/**
 * @brief  Set part of sram or all of them to no retention(shutdown).
 * @param  u32SramBlock: Blocks of ram which will shut down.
 *         Each bit correspoding a SRAM block.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_clr_sram_block_ret(uint32_t u32SramBlock)
{
    PMU->PMU_SRAM_RET_EN &= ~((u32SramBlock & SRAM_BLOCK_EN_MSK) << SRAM_BLOCK_EN_SHIT);

    return ERR_STA_OK;
}




/**********************************************************************************************************************
                                                PMU Power Driver
**********************************************************************************************************************/

/**
 * @brief  Enable LDO_ACT Output When LDO in Active Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_enable_ldo_act_output(void)
{
    PMU->PMU_LDO_ACT_OUT_EN |= (ENABLE & LDO_ACT_OUT_EN_MSK) << LDO_ACT_OUT_EN_SHIFT;

    rom_delay_us(ROM_LDO_ACT_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Disable LDO_ACT Output When LDO in Active Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_disable_ldo_act_output(void)
{
    PMU->PMU_LDO_ACT_OUT_EN &= ~((ENABLE & LDO_ACT_OUT_EN_MSK) << LDO_ACT_OUT_EN_SHIFT);

    rom_delay_us(ROM_LDO_ACT_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Configure LDO Output Voltage which for RF Module.
 * @param  enVolt: Configure LDO_ACT output voltage by active mode, @ref EN_LDO_ACT_VOLT_T
 *                 Step is 50, unit: mV, range is 950mV to 1300mV.
 *         The voltage of LDO_ACT should be at lease 50mV higher than the voltage of LDO_DIG.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_ldo_act_voltage(EN_LDO_ACT_VOLT_T enVolt)
{
    uint32_t u32Cfg = 0;

    if (EN_LDO_ACT_950mV < enVolt)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_LDO_ACT_CFG;
    u32Cfg &= ~(LDO_ACT_VOLT_MSK << LDO_ACT_VOLT_SHIFT);
    u32Cfg |= (enVolt & LDO_ACT_VOLT_MSK) << LDO_ACT_VOLT_SHIFT;
    PMU->PMU_LDO_ACT_CFG = u32Cfg;

    rom_delay_us(ROM_LDO_ACT_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Enable LDO_1V8 Output When DCDC MISO 1.8V output in Active Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_enable_ldo_1v8_output(void)
{
    PMU->PMU_LDO_1V8_OUT_EN |= (ENABLE & LDO_1V8_OUT_EN_MSK) << LDO_1V8_OUT_EN_SHIFT;

    rom_delay_us(ROM_LDO_ACT_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Disable LDO_1V8 Output When DCDC MISO 1.8V output in Active Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_disable_ldo_1v8_output(void)
{
    PMU->PMU_LDO_1V8_OUT_EN &= ~((ENABLE & LDO_1V8_OUT_EN_MSK) << LDO_1V8_OUT_EN_SHIFT);

    rom_delay_us(ROM_LDO_ACT_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Enable LDO_DIG Output When LDO in Active Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_enable_ldo_dig_output(void)
{
    PMU->PMU_LDO_DIG_OUT_EN |= (ENABLE & LDO_DIG_OUT_EN_MSK) << LDO_DIG_OUT_EN_SHIFT;

    rom_delay_us(ROM_LDO_DIG_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Disable LDO_DIG Output When LDO in Active Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_disable_ldo_dig_output(void)
{
    PMU->PMU_LDO_DIG_OUT_EN &= ~((ENABLE & LDO_DIG_OUT_EN_MSK) << LDO_DIG_OUT_EN_SHIFT);

    rom_delay_us(ROM_LDO_DIG_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Configure LDO Output Voltage which for digital module and LDO_RET_ACT Output Voltage in Active Mode.
 * @param  enVolt: Configure LDO Output Voltage, Step is 50, unit: mV, range is 900mV to 1100mV.
 *         When SYS_CLK is 16MHz, LDO_DIG can configure to 900mV;
 *         When SYS_CLK is 128MHz, LDO_DIG should configure to 1100mV.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_ldo_dig_voltage(EN_LDO_DIG_VOLT_T enVolt)
{
    uint32_t u32Cfg = 0;

    if ((EN_LDO_DIG_900mV < enVolt) || (EN_LDO_RET_950mV < enVolt))
    {
        return ERR_PARA_ERR;
    }

    /* LDO_DIG must equal to LDO_RET_ACT! */
    /* LDO_DIG */
    u32Cfg = PMU->PMU_LDO_DIG_CFG;
    u32Cfg &= ~(LDO_DIG_VOLT_MSK << LDO_DIG_VOLT_SHIFT);
    u32Cfg |= (enVolt << LDO_DIG_VOLT_SHIFT);
    PMU->PMU_LDO_DIG_CFG = u32Cfg;

    /* LDO_RET */
    u32Cfg = PMU->PMU_LDO_RET_CFG;
    u32Cfg &= ~(LDO_RET_ACT_VOLT_MSK << LDO_RET_ACT_VOLT_SHIFT);
    u32Cfg |= (enVolt << LDO_RET_ACT_VOLT_SHIFT);
    PMU->PMU_LDO_RET_CFG = u32Cfg;

    rom_delay_us(ROM_LDO_DIG_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Get LDO output voltage which for digital module.
 * @param  pu8Volt: Pointer to the buffer to get LDO Output Voltage, @ref EN_LDO_DIG_VOLT_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_get_ldo_dig_voltage(uint8_t *pu8Volt)
{
    if (pu8Volt == NULL)
    {
        return ERR_PARA_ERR;
    }

    *pu8Volt = ((PMU->PMU_LDO_DIG_CFG >> LDO_DIG_VOLT_SHIFT) & LDO_DIG_VOLT_MSK);

    return ERR_STA_OK;
}

/**
 * @brief  Configure LDO_RET_ACT Output Voltage in Active Mode.
 * @param  enSleepVolt: Configure LDO_RET output voltage by sleep mode, @ref EN_LDO_RET_VOLT_T.
 *                      Step is 50, unit: mV, range is 750mV to 1100mV.
 *         When SYS_CLK is 16MHz, LDO_RET_ACT can configure to 900mV;
 *         When SYS_CLK is 128MHz, LDO_RET_ACT should configure to 1100mV.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_ldo_ret_act_voltage(EN_LDO_RET_VOLT_T enVolt)
{
    uint32_t u32Cfg = 0;

    if (EN_LDO_RET_950mV < enVolt)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_LDO_RET_CFG;
    u32Cfg &= ~(LDO_RET_ACT_VOLT_MSK << LDO_RET_ACT_VOLT_SHIFT);
    u32Cfg |= (enVolt << LDO_RET_ACT_VOLT_SHIFT);
    PMU->PMU_LDO_RET_CFG = u32Cfg;

    rom_delay_us(ROM_LDO_RET_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Configure LDO_RET_SLEEP Output Voltage in Sleep Mode.
 * @param  enSleepVolt: Configure LDO_RET output voltage by sleep mode, @ref EN_LDO_RET_VOLT_T.
 *                      Step is 50, unit: mV, range is 750mV to 1100mV.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_ldo_ret_sleep_voltage(EN_LDO_RET_VOLT_T enSleepVolt)
{
    uint32_t u32Cfg = 0;

    if (EN_LDO_RET_950mV < enSleepVolt)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_LDO_RET_CFG;
    u32Cfg &= ~(LDO_RET_SLEEP_VOLT_MSK << LDO_RET_SLEEP_VOLT_SHIFT);
    u32Cfg |= (enSleepVolt << LDO_RET_SLEEP_VOLT_SHIFT);
    PMU->PMU_LDO_RET_CFG = u32Cfg;

    rom_delay_us(ROM_LDO_RET_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Configure DCDC(include SISO, SIDO) parameter.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_dcdc_init(void)
{
    uint32_t u32ActCfg = 0;

    /* Configure dcdc parameter: ocp, zcd, bbm, dobbm */
    /* Start cfg pmu ocp zcd bbm dobbm Analog advice config value (1010 0101 100 001 5 5 2) */
    u32ActCfg = (((0x09 & DCDC_OCP_MSK) << DCDC_OCP_SHIFT)                 \
               | ((0x08 & DCDC_ZCD_MSK) << DCDC_ZCD_SHIFT)                 \
               | ((0x04 & DCDC_TON_MSK) << DCDC_TON_SHIFT)                 \
               | ((0x03 & DCDC_PLUSE_MSK) << DCDC_PLUSE_SHIFT)             \
               | ((0x05 & DCDC_BBM_MSK) << DCDC_BBM_SHIFT)                 \
               | ((0x02 & DCDC_HYSB_MSK) << DCDC_HYSB_SHIFT)               \
               | ((0x01 & DCDC_TOFF_MSK) << DCDC_TOFF_SHIFT)               \
               | ((0x07 & DCDC_OCP_CMP_HYS_MSK) << DCDC_OCP_CMP_HYS_SHIFT) \
               | ((0x07 & DCDC_ZCD_CMP_HYS_MSK) << DCDC_ZCD_CMP_HYS_SHIFT) \
               | ((0x01 & DCDC_DO_BBM_MSK) << DCDC_DO_BBM_SHIFT));

    PMU->PMU_DCDC_ACT_CFG = u32ActCfg;

    PMU->PMU_DCDC_FB_CFG = ((0x00 << DCDC_FB_HI_VOLT_SHIFT)
                          | (0x00 << DCDC_FB_LO_VOLT_SHIFT));

    PMU->PMU_DCDC_LPWR_ZCD_CFG = ((0x01 << DCDC_LPWR_ZCD_LO_SEL_SHIFT)
                                | (0x01 << DCDC_LPWR_ZCD_HI_SEL_SHIFT)
                                | (0x01 << DCDC_LPWR_ZCD_LO_EN_SHIFT)
                                | (0x01 << DCDC_LPWR_ZCD_HI_EN_SHIFT));

    PMU->PMU_DCDC_LPWR_CFG = ((0x0F << DCDC_LPWR_FB_LO_SHIFT)
                            | (0x00 << DCDC_LPWR_FB_HI_SHIFT)
                            | ((0x07 & DCDC_LPWR_BBM_MSK) << DCDC_LPWR_BBM_SHIFT)
                            | ((0x80 & DCDC_LPWR_P_LO_MSK) << DCDC_LPWR_P_LO_SHIFT));

    PMU->PMU_DCDC_LPWR_CLK = EN_DCDC_LPWR_CLK_4K << DCDC_LPWR_CLK_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Enable DCDC Output When DC-DC in Active Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_enable_dcdc_act_output(void)
{
    PMU->PMU_DCDC_ACT_OUT_EN |= (ENABLE & DCDC_ACT_OUT_EN_MSK) << DCDC_ACT_OUT_EN_SHIFT;

    rom_delay_us(ROM_DCDC_ACT_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Disable DCDC Output When DC-DC in Active Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_disable_dcdc_act_output(void)
{
    PMU->PMU_DCDC_ACT_OUT_EN &= ~((ENABLE & DCDC_ACT_OUT_EN_MSK) << DCDC_ACT_OUT_EN_SHIFT);

    rom_delay_us(ROM_DCDC_ACT_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Configure DCDC Output Voltage which for RF Module.
 * @param  enVolt: Configure DCDC_ACT output voltage by active mode, @ref EN_DCDC_ACT_VOLT_T.
 *                 Unit: mV, range is 950mV to 1300mV.
 *         The voltage of DCDC_ACT should be at lease 50mV higher than the voltage of LDO_DIG.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_dcdc_act_voltage(EN_DCDC_ACT_VOLT_T enVolt)
{
    uint32_t u32Cfg = 0;

    if (EN_DCDC_ACT_VOLT_950mV < enVolt)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_DCDC_FB_CFG;

    u32Cfg &= ~(DCDC_FB_LO_VOLT_MSK << DCDC_FB_LO_VOLT_SHIFT);
    u32Cfg |= (enVolt & DCDC_FB_LO_VOLT_MSK) << DCDC_FB_LO_VOLT_SHIFT;

    PMU->PMU_DCDC_FB_CFG = u32Cfg;

    rom_delay_us(ROM_DCDC_ACT_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Enable DCDC Output When DC-DC in sleep Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_enable_dcdc_ret_output(void)
{
    PMU->PMU_DCDC_LPWR_OUT_EN |= (ENABLE & DCDC_LPWR_OUT_EN_MSK) << DCDC_LPWR_OUT_EN_SHIFT;

    rom_delay_us(ROM_DCDC_RET_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Disable DCDC Output When DC-DC in Low Power Mode.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_disable_dcdc_ret_output(void)
{
    PMU->PMU_DCDC_LPWR_OUT_EN &= ~((ENABLE & DCDC_LPWR_OUT_EN_MSK) << DCDC_LPWR_OUT_EN_SHIFT);

    rom_delay_us(ROM_DCDC_RET_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Configure DCDC_LPWR Output Voltage in Sleep Mode.
 * @param  enLpwrVolt: Configure DCDC_LPWR output voltage by sleep mode, @ref EN_DCDC_LPWR_VOLT_T.
 *                     Unit: mV, range is 650mV to 1050mV.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_dcdc_ret_voltage(EN_DCDC_LPWR_VOLT_T enLpwrVolt)
{
    uint32_t u32Cfg = 0;

    if ((EN_DCDC_LPWR_VOLT_950mV < enLpwrVolt))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_DCDC_LPWR_CFG;
    u32Cfg &= ~(DCDC_LPWR_FB_LO_MSK << DCDC_LPWR_FB_LO_SHIFT);
    u32Cfg |= (enLpwrVolt << DCDC_LPWR_FB_LO_SHIFT);
    PMU->PMU_DCDC_LPWR_CFG = u32Cfg;

    rom_delay_us(ROM_DCDC_RET_SETUP_TIME_US);

    return ERR_STA_OK;
}

/**
 * @brief  Select DCDC Low Power Clock(DCDC_LPWR_CLK) Division With RC_LCLK.
 * @param  enDiv: The divisor of DCDC_LPWR_CLK, @ref EN_DCDC_LPWR_CLK_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_dcdc_ret_clk_divisor(EN_DCDC_LPWR_CLK_T enDiv)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_DCDC_LPWR_CLK(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_DCDC_LPWR_CLK;
    u32Cfg &= ~(DCDC_LPWR_CLK_MSK << DCDC_LPWR_CLK_SHIFT);
    u32Cfg |= (enDiv & DCDC_LPWR_CLK_MSK) << DCDC_LPWR_CLK_SHIFT;
    PMU->PMU_DCDC_LPWR_CLK = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  PMU sleep mode select.
 * @param  enMode: pmu sleep mode, @ref EN_PWR_MODE_T.
 *         PWR_MODE_DEEPSLEEP: RC_LCLK clock is keep working (shutdown with 32K);
 *         PWR_MODE_HIBERNATION: RC_LCLK clock will turn off (shutdown without 32K);
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_low_power_mode(EN_PWR_MODE_T enMode)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_PWR_MODE(enMode))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_SLEEP_MODE;
    u32Cfg &= ~(PWR_MODE_SEL_MSK << PWR_MODE_SEL_SHIFT);
    u32Cfg |= ((enMode & PWR_MODE_SEL_MSK) << PWR_MODE_SEL_SHIFT);
    PMU->PMU_SLEEP_MODE = u32Cfg;

    return ERR_STA_OK;
}




/**********************************************************************************************************************
 *                                              PMU Clock Driver
 *********************************************************************************************************************/

/**
 * @brief  Check RC Low Clock(RC_LCLK) is Enable or not.
 * @return status: false(0) - RC_LCLK is Disabled.
 *                 true(1)  - RC_LCLK is Enabled.
 */
bool rom_hw_pmu_rc_lclk_is_enable(void)
{
    return ((PMU->PMU_CLK_STATUS >> RC_LCLK_STATUS_SHIFT) & PMU_CLK_STATUS_MSK);
}

/**
 * @brief  Check DCXO Low Clock(DCXO_LCLK) is power on or off.
 * @return status: false(0) - DCXO_LCLK is powered off.
 *                 true(1)  - DCXO_LCLK is powered on.
 */
bool rom_hw_pmu_dcxo_lclk_is_power_on(void)
{
    return ((RTC_PDW->DCXO_LCLK_STA >> DCXO_LCLK_POWER_STA_SHIFT) & DCXO_LCLK_POWER_STA_MSK);
}

/**
 * @brief  Check DCXO Low Clock(DCXO_LCLK) is output.
 * @return status: false(0) - DCXO_LCLK output is disabled.
 *                 true(1)  - DCXO_LCLK output is enabled.
 */
bool rom_hw_pmu_dcxo_lclk_is_clk_out(void)
{
    return ((RTC_PDW->DCXO_LCLK_STA >> DCXO_LCLK_OUTPUT_STA_SHIFT) & DCXO_LCLK_OUTPUT_STA_MSK);
}

/**
 * @brief  Check RC High Clock(RC_HCLK) is Power On or Off.
 * @return status: false(0) - RC_HCLK is Powered Off.
 *                 true(1)  - RC_HCLK is Powered On.
 */
bool rom_hw_pmu_rc_hclk_is_power_on(void)
{
    return ((PMU->PMU_CLK_STATUS >> RC_HCLK_PWR_STATUS_SHIFT) & PMU_CLK_STATUS_MSK);
}

/**
 * @brief  Check DCXO High Clock(DCXO_HCLK) is Output.
 * @return status: false(0) - DCXO_HCLK output is disabled.
 *                 true(1)  - DCXO_HCLK output is enabled.
 */
bool rom_hw_pmu_dcxo_hclk_is_clk_out(void)
{
    return ((PMU->PMU_CLK_STATUS >> DCXO_HCLK_OUT_STATUS_SHIFT) & PMU_CLK_STATUS_MSK);
}

/**
 * @brief  Check DCXO High Clock(DCXO_HCLK) is Power On or Off.
 * @return status: false(0) - DCXO_HCLK is Powered Off.
 *                 true(1)  - DCXO_HCLK is Powered On.
 */
bool rom_hw_pmu_dcxo_hclk_is_power_on(void)
{
    return ((PMU->PMU_CLK_STATUS >> DCXO_HCLK_PWR_STATUS_SHIFT) & PMU_CLK_STATUS_MSK);
}

/**
 * @brief  Check RC High Clock(RC_HCLK) is Output.
 * @return status: false(0) - RC_HCLK output is disabled.
 *                 true(1)  - RC_HCLK output is enabled.
 */
bool rom_hw_pmu_rc_hclk_is_clk_out(void)
{
    return ((PMU->PMU_CLK_STATUS >> RC_HCLK_OUT_STATUS_SHIFT) & PMU_CLK_STATUS_MSK);
}

/**
 * @brief  Check PLL Clock(PLL_CLK) is Power On or Off.
 * @return status: false(0) - PLL_CLK is Powered Off.
 *                 true(1)  - PLL_CLK is Powered On.
 */
bool rom_hw_pmu_pll_clk_is_power_on(void)
{
    return ((PMU->PMU_CLK_STATUS >> PLL_CLK_PWR_STATUS_SHIFT) & PMU_CLK_STATUS_MSK);
}

/**
 * @brief  Check PLL Clock(PLL96M_CLK or PLL128M_CLK) is Output.
 * @return status: false(0) - PLL96M_CLK or PLL128M_CLK output is disabled.
 *                 true(1)  - PLL96M_CLK and PLL128M_CLK output is enabled.
 */
bool rom_hw_pmu_pll_clk_is_clk_out(void)
{
    return (((PMU->PMU_CLK_STATUS >> PLL96M_CLK_OUT_STATUS_SHIFT) & PMU_CLK_STATUS_MSK)
          | ((PMU->PMU_CLK_STATUS >> PLL128M_CLK_OUT_STATUS_SHIFT) & PMU_CLK_STATUS_MSK));
}

/**
 * @brief  Turn Clock Power On.
 * @param  enCLK: Indicate which clock will be powered on, @ref EN_CLK_POWER_CTRL_T.
 * @param  u32TimeUs: The setup time of clock. Unit: us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_turn_clk_power_on(EN_CLK_POWER_CTRL_T enCLK, uint32_t u32TimeUs)
{
    switch (enCLK)
    {
        case EN_DCXO_LCLK_POWER_CTRL:
        {
            RTC_PDW->DCXO_LCLK_CFG |= (ENABLE & DCXO_LCLK_POWER_CFG_MSK) << DCXO_LCLK_POWER_CFG_SHIFT;
            RTC_PDW->DCXO_LCLK_CTRL = (ENABLE & DCXO_LCLK_POWER_EN_MSK) << DCXO_LCLK_POWER_EN_SHIFT;

            rom_delay_us(u32TimeUs);
            break;
        }
        case EN_DCXO_HCLK_POWER_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << DCXO_HCLK_PWR_ON_SHIFT;

            rom_delay_us(u32TimeUs);
            break;
        }
        case EN_PLL_CLK_POWER_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << PLL_CLK_PWR_ON_SHIFT;

            rom_delay_us(u32TimeUs);
            break;
        }
        default:
        {
            return ERR_STA_ERROR;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Turn Clock Power Off.
 * @param  enCLK: Indicate which clock will be powered off, @ref EN_CLK_POWER_CTRL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_turn_clk_power_off(EN_CLK_POWER_CTRL_T enCLK)
{
    switch (enCLK)
    {
        case EN_DCXO_LCLK_POWER_CTRL:
        {
            RTC_PDW->DCXO_LCLK_CFG &= ~((ENABLE & DCXO_LCLK_POWER_CFG_MSK) << DCXO_LCLK_POWER_CFG_SHIFT);
            RTC_PDW->DCXO_LCLK_CTRL = (ENABLE & DCXO_LCLK_POWER_EN_MSK) << DCXO_LCLK_POWER_EN_SHIFT;
            break;
        }
        case EN_DCXO_HCLK_POWER_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << DCXO_HCLK_PWR_OFF_SHIFT;
            break;
        }
        case EN_PLL_CLK_POWER_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << PLL_CLK_PWR_OFF_SHIFT;
            break;
        }
        default:
        {
            return ERR_STA_ERROR;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Enable Clock Output.
 * @param  enCLK: Indicate which clock will be output, @ref EN_CLK_OUT_CTRL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_enable_clk_output(EN_CLK_OUT_CTRL_T enCLK)
{
    switch (enCLK)
    {
        case EN_RC_LCLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << RC_LCLK_OUT_EN_SHIFT;

            rom_delay_us(ROM_RC_LCLK_OUTPUT_SETUP_TIME_US);
            break;
        }
        case EN_RC_HCLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << RC_HCLK_OUT_EN_SHIFT;

            rom_delay_us(ROM_RC_HCLK_OUTPUT_SETUP_TIME_US);
            break;
        }
        case EN_DCXO_LCLK_OUT_CTRL:
        {
            RTC_PDW->DCXO_LCLK_CFG |= (ENABLE & DCXO_LCLK_OUTPUT_CFG_MSK) << DCXO_LCLK_OUTPUT_CFG_SHIFT;
            RTC_PDW->DCXO_LCLK_CTRL = (ENABLE & DCXO_LCLK_OUTPUT_EN_MSK) << DCXO_LCLK_OUTPUT_EN_SHIFT;
            break;
        }
        case EN_DCXO_HCLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << DCXO_HCLK_OUT_EN_SHIFT;
            break;
        }
        case EN_PLL96M_CLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << PLL96M_CLK_OUT_EN_SHIFT;
            break;
        }
        case EN_PLL128M_CLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << PLL128M_CLK_OUT_EN_SHIFT;
            break;
        }
        default:
        {
            return ERR_STA_ERROR;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Disable Clock Output.
 * @param  enCLK: Indicate which clock will be closed, @ref EN_CLK_OUT_CTRL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_disable_clk_output(EN_CLK_OUT_CTRL_T enCLK)
{
    switch (enCLK)
    {
        case EN_RC_LCLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << RC_LCLK_OUT_DIS_SHIFT;
            break;
        }
        case EN_RC_HCLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << RC_HCLK_OUT_DIS_SHIFT;
            break;
        }
        case EN_DCXO_LCLK_OUT_CTRL:
        {
            RTC_PDW->DCXO_LCLK_CFG &= ~((ENABLE & DCXO_LCLK_OUTPUT_CFG_MSK) << DCXO_LCLK_OUTPUT_CFG_SHIFT);
            RTC_PDW->DCXO_LCLK_CTRL = (ENABLE & DCXO_LCLK_OUTPUT_EN_MSK) << DCXO_LCLK_OUTPUT_EN_SHIFT;
            break;
        }
        case EN_DCXO_HCLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << DCXO_HCLK_OUT_DIS_SHIFT;
            break;
        }
        case EN_PLL96M_CLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << PLL96M_CLK_OUT_DIS_SHIFT;
            break;
        }
        case EN_PLL128M_CLK_OUT_CTRL:
        {
            PMU->PMU_CLK_CTRL = 0x01 << PLL128M_CLK_OUT_DIS_SHIFT;
            break;
        }
        default:
        {
            return ERR_STA_ERROR;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Check DCXO Low Clock(DCXO_LCLK) configuration is valid.
 * @param  pstBuf: Pointer to a stDCXOBuf_t structure that contains.
 * @param  pstPara: Pointer to a stDCXOParam_t structure that contains.
 * @return status: false(0) - DCXO_LCLK configuration is invalid.
 *                 true(1)  - DCXO_LCLK configuration is valid.
 */
bool rom_hw_pmu_dcxo_lclk_cfg_is_valid(stDCXOBuf_t *pstBuf, stDCXOParam_t *pstParam)
{
    uint32_t u32CfgRead = 0;
    uint32_t u32CfgWrite = 0;
    uint32_t u32BufRead = 0;
    uint32_t u32BufWrite = 0;

    u32BufRead = (RTC_PDW->DCXO_LCLK_BUF_STA >> DCXO_LCLK_BUF_STA_SHIFT) & DCXO_LCLK_BUF_STA_MSK;

    u32BufWrite = (((pstBuf->u8NegBuf & DCXO_LCLK_NEG_BUF_MSK) << DCXO_LCLK_NEG_BUF_SHIFT)
                 | ((pstBuf->u8PosBuf & DCXO_LCLK_POS_BUF_MSK) << DCXO_LCLK_POS_BUF_SHIFT));

    u32CfgRead = (RTC_PDW->DCXO_LCLK_CFG_STA >> DCXO_LCLK_CFG_STA_SHIFT) & DCXO_LCLK_CFG_STA_MSK;

    u32CfgWrite = (((pstParam->u8Ib & DCXO_LCLK_IB_MSK) << DCXO_LCLK_IB_SHIFT)
                 | ((pstParam->u8Ngm & DCXO_LCLK_NGM_MSK) << DCXO_LCLK_NGM_SHIFT)
                 | ((pstParam->u8Capl & DCXO_LCLK_CAPL_MSK) << DCXO_LCLK_CAPL_SHIFT)
                 | ((pstParam->u8Capr & DCXO_LCLK_CAPR_MSK) << DCXO_LCLK_CAPR_SHIFT)
                 | ((pstParam->u32Backup & DCXO_LCLK_BKUP_MSK) << DCXO_LCLK_BKUP_SHIFT));

    return ((u32BufRead == u32BufWrite) & (u32CfgRead == u32CfgWrite));
}

/**
 * @brief  Configure DCXO Low Clock(DCXO_LCLK, 32KHz) inverter, current and Load Capacitance.
 * @param  pstBuf: Pointer to a stDCXOBuf_t structure that contains,
 *                  u8PosBuf, u8NegBuf: Number of inverters. The range of value is [0:7].
 * @param  pstPara: Pointer to a stDCXOParam_t structure that contains,
 *                  u8Ib: dcxo current. The range of value is [0:7].
 *                  u8Ngm: The number of inverters in parallel with the active part of the oscillator.
 *                         The range of value is [0:7]. Suggest cfg to 2~4.
 *                  u8Cap: dcxo load capacitance. Unit: 0.1pF, the range is [0:255] which means 3.0pF~28.5pF.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_dcxo_lclk_param(stDCXOBuf_t *pstBuf, stDCXOParam_t *pstParam)
{
    if ((DCXO_LCLK_NEG_BUF_MSK < pstBuf->u8NegBuf) || (DCXO_LCLK_POS_BUF_MSK < pstBuf->u8PosBuf))
    {
        return ERR_PARA_ERR;
    }

    if ((DCXO_LCLK_IB_MSK < pstParam->u8Ib) || (DCXO_LCLK_NGM_MSK < pstParam->u8Ngm)
     || (DCXO_LCLK_CAPL_MSK < pstParam->u8Capl) || (DCXO_LCLK_CAPR_MSK < pstParam->u8Capr))
    {
        return ERR_PARA_ERR;
    }

    RTC_PDW->DCXO_LCLK_BUF = (((pstBuf->u8NegBuf & DCXO_LCLK_NEG_BUF_MSK) << DCXO_LCLK_NEG_BUF_SHIFT)
                            | ((pstBuf->u8PosBuf & DCXO_LCLK_POS_BUF_MSK) << DCXO_LCLK_POS_BUF_SHIFT));

    RTC_PDW->DCXO_LCLK_PARA_CFG = (((pstParam->u8Ib & DCXO_LCLK_IB_MSK) << DCXO_LCLK_IB_SHIFT)
                                 | ((pstParam->u8Ngm & DCXO_LCLK_NGM_MSK) << DCXO_LCLK_NGM_SHIFT)
                                 | ((pstParam->u8Capl & DCXO_LCLK_CAPL_MSK) << DCXO_LCLK_CAPL_SHIFT)
                                 | ((pstParam->u8Capr & DCXO_LCLK_CAPR_MSK) << DCXO_LCLK_CAPR_SHIFT)
                                 | ((pstParam->u32Backup & DCXO_LCLK_BKUP_MSK) << DCXO_LCLK_BKUP_SHIFT));

    RTC_PDW->DCXO_LCLK_CTRL = (ENABLE & DCXO_LCLK_CFG_EN_MSK) << DCXO_LCLK_CFG_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Configure RC32K_CLK calibration value to calibrate RC32K_CLK.
 * @param  u8Val: RC_LCLK tune value, the range is [0:255].
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_rc_lclk_tune(uint8_t u8Val)
{
    PMU->PMU_RC_LCLK_TUNE = (u8Val & RC_LCLK_TUNE_MSK) << RC_LCLK_TUNE_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Configure RC_HCLK calibration value to calibrate RC16M_CLK or RC48M_CLK.
 * @param  u8Val: RC_HCLK tune value, the range is [0:127].
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_rc_hclk_tune(uint8_t u8Val)
{
    PMU->PMU_RC_HCLK_TUNE = (u8Val & RC_HCLK_TUNE_MSK) << RC_HCLK_TUNE_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Enable or disable output RC_HCLK clock while turning on RC_HCLK.
 * @param  enMode: RC_HCLK clock output mode.
 *                 b0 - Delay 0.5 to 1.5 32K clock cycles to output RC_HCLK after turn on RC_HCLK.
 *                 b1 - Output RC_HCLK while turning on RC_HCLK immediately.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_rc_hclk_sync_out(EN_RC_HCLK_SYNC_OUT_MODE_T enMode)
{
    if (RC_HCLK_OUT_STABLE == enMode)
    {
        PMU->PMU_CLK_SYNC_OUT &= ~(RC_HCLK_SYNC_OUT_MSK << RC_HCLK_SYNC_OUT_SHIFT);
    }
    else
    {
        PMU->PMU_CLK_SYNC_OUT = RC_HCLK_SYNC_OUT_MSK << RC_HCLK_SYNC_OUT_SHIFT;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Configure DCXO High Clock(DCXO_HCLK) Power to DCDC_ACT or VDDR.
 * @param  enPwr: DCXO_HCLK power selection, @ref EN_DCXO_HCLK_PWR_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_sel_dcxo_hclk_pwr(EN_DCXO_HCLK_PWR_T enPwr)
{
    uint32_t u32Cfg = 0;

    if (EN_DCXO_HCLK_PWR_DCDC_ACT < enPwr)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_DCXO_HCLK_PWR_SEL;
    u32Cfg &= ~(DCXO_HCLK_PWR_SEL_MSK << DCXO_HCLK_PWR_SEL_SHIFT);
    u32Cfg |= (enPwr << DCXO_HCLK_PWR_SEL_SHIFT);
    PMU->PMU_DCXO_HCLK_PWR_SEL = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Configure DCXO High Clock(DCXO_HCLK) Stable Time after wakeup, unit: 31.25us.
 * @param  u8Time: the stable time, Rang is [0:255], unit is 31.25us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_dcxo_hclk_stable_time(uint8_t u8Time)
{
    uint32_t u32Cfg = 0;

    if (DCXO_HCLK_STABLE_TIME_MSK < u8Time)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_DCXO_HCLK_STABLE_TIME;
    u32Cfg &= ~(DCXO_HCLK_STABLE_TIME_MSK << DCXO_HCLK_STABLE_TIME_SHIFT);
    u32Cfg |= (u8Time << DCXO_HCLK_STABLE_TIME_SHIFT);
    PMU->PMU_DCXO_HCLK_STABLE_TIME = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Configure DCXO High Clock(DCXO_HCLK) Current and Load Capacitance.
 * @param  pstPara: Pointer to a stDCXOParam_t structure that contains,
 *                  u8Ib: dcxo current. The range of value is [0:7].
 *                  u8Ngm: The number of inverters in parallel with the active part of the oscillator.
 *                         The range of value is [0:7]. Suggest cfg to 2~4.
 *                  u8Cap: dcxo load capacitance. Unit: 0.1pF, the range is [0:255] which means 3.0pF~28.5pF.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_set_dcxo_hclk_param(stDCXOParam_t *pstParam)
{
    uint8_t cap;

    if ((DCXO_HCLK_IB_MSK < pstParam->u8Ib) || (DCXO_HCLK_NGM_MSK < pstParam->u8Ngm)
     || (DCXO_HCLK_CAPL_MSK < pstParam->u8Capl) || (DCXO_HCLK_CAPR_MSK < pstParam->u8Capr))
    {
        return ERR_PARA_ERR;
    }

    PMU->PMU_DCXO_HCLK_CFG = (((pstParam->u8Ib & DCXO_HCLK_IB_MSK) << DCXO_HCLK_IB_SHIFT)
                            | ((pstParam->u8Ngm & DCXO_HCLK_NGM_MSK) << DCXO_HCLK_NGM_SHIFT)
                            | ((pstParam->u8Capl & DCXO_HCLK_CAPL_MSK) << DCXO_HCLK_CAPL_SHIFT)
                            | ((pstParam->u8Capr & DCXO_HCLK_CAPR_MSK) << DCXO_HCLK_CAPR_SHIFT)
                            | ((pstParam->u32Backup & DCXO_HCLK_BKUP_MSK) << DCXO_HCLK_BKUP_SHIFT));

    return ERR_STA_OK;
}

/**
 * @brief  Configure USB phy iso sel.
 * @param  enIso: USB phy iso, @ref EN_USB_PHY_RX_ISO_CFG_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_pmu_sel_usb_phy_iso(EN_USB_PHY_RX_ISO_CFG_T enIso)
{
    uint32_t u32Cfg = 0;

    if (enIso > USB_PHY_SEL_MSK)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = PMU->PMU_USB_PHY_SEL;
    u32Cfg &= ~(USB_PHY_SEL_MSK << USB_PHY_SEL_SHIFT);
    u32Cfg |= (enIso & USB_PHY_SEL_MSK) << USB_PHY_SEL_SHIFT;
    PMU->PMU_USB_PHY_SEL = u32Cfg;

    return ERR_STA_OK;
}

