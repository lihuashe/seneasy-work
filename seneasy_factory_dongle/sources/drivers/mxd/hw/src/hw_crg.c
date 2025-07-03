/**********************************************************************************************************************
 * @file     hw_crg.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/01/11
 * @history
 * @note     Clock Gate and Reset.
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
#define CRG_CLK_DIV_TIMEOUT_US (100)




/**********************************************************************************************************************
 *                                             CRG Clock Source
 *********************************************************************************************************************/

/**
 * @brief  Select Low Power Clock(LPWR_CLK, 32K Clock) Source.
 * @param  enSrc: LPWR_CLK source selection. @ref EN_LPWR_CLK_SRC_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_lpwr_clk_src(EN_LPWR_CLK_SRC_T enSrc)
{
    uint32_t src = 0;
    uint32_t u32Cfg = 0;

    if (!IS_VALID_LPWR_CLK_SRC(enSrc))
    {
        return ERR_STA_ERROR;
    }

    src = (CRG->CRG_CLK_CFG >> CRG_LPWR_CLK_SRC_SEL_SHIFT) & CRG_LPWR_CLK_SRC_SEL_MSK;

    if (src != enSrc)
    {
        u32Cfg = CRG->CRG_CLK_CFG;

        u32Cfg &= ~(CRG_LPWR_CLK_SRC_SEL_MSK << CRG_LPWR_CLK_SRC_SEL_SHIFT);
        u32Cfg |= (enSrc & CRG_LPWR_CLK_SRC_SEL_MSK) << CRG_LPWR_CLK_SRC_SEL_SHIFT;

        CRG->CRG_CLK_CFG = u32Cfg;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Get Low Power Clock(LPWR_CLK, 32K Clock) Source.
 * @param  pu32Src: Point to LPWR_CLK source. @ref EN_LPWR_CLK_SRC_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_get_lpwr_clk_src(uint32_t *pu32Src)
{
    if (NULL == pu32Src)
    {
        return ERR_STA_ERROR;
    }

    *pu32Src = (CRG->CRG_CLK_CFG >> CRG_LPWR_CLK_SRC_SEL_SHIFT) & CRG_LPWR_CLK_SRC_SEL_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Check System Clock(SYS_CLK) Source is RC_HCLK or not.
 * @return status: false(0) - SYS_CLK source is not RC_HCLK.
 *                 true(1)  - SYS_CLK source is RC_HCLK.
 */
bool rom_hw_crg_sys_clk_src_is_rc_hclk(void)
{
    uint32_t u32Src = 0;

    u32Src = CRG->CRG_SYS_CLK_SRC & CRG_SYS_CLK_SRC_MSK;

    return (u32Src == SYS_CLK_SRC_RC_HCLK);
}

/**
 * @brief  Select DCXO_HCLK Clock Frequency.
 * @param  enFreq: DCXO high clock source frequency. 16MHz or 32MHz crystal oscillator. @ref EN_DCXO_HCLK_FREQ_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_dcxo_hclk_freq(EN_DCXO_HCLK_FREQ_T enFreq)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_DCXO_HCLK_FREQ(enFreq))
    {
        return ERR_STA_ERROR;
    }

    u32Cfg = CRG->CRG_CLK_CFG;

    u32Cfg &= ~(CRG_DCXO_HCLK_FREQ_SEL_MSK << CRG_DCXO_HCLK_FREQ_SEL_SHIFT);
    u32Cfg |= (enFreq & CRG_DCXO_HCLK_FREQ_SEL_MSK) << CRG_DCXO_HCLK_FREQ_SEL_SHIFT;

    CRG->CRG_CLK_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Select System Clock(SYS_CLK) Source.
 * @param  enSrc: System clock source selection. @ref EN_SYS_CLK_SEL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_sys_clk_src(EN_SYS_CLK_SEL_T enSrc)
{
    if (!IS_VALID_SYS_CLK_SEL(enSrc))
    {
        return ERR_STA_ERROR;
    }

    CRG->CRG_SYS_CLK_CTRL = enSrc << CRG_SYS_CLK_SEL_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Get System Clock(SYS_CLK) Source.
 * @param  pu32Src: Point to system clock source. @ref EN_SYS_CLK_SEL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_get_sys_clk_src(uint32_t *pu32Src)
{
    if (NULL == pu32Src)
    {
        return ERR_STA_ERROR;
    }

    *pu32Src = CRG->CRG_SYS_CLK_SRC & CRG_SYS_CLK_SRC_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Select Audio ADC Clock(ADC_CLK) Source Clock.
 * @param  enADC: ADC channel, @ref EN_CRG_ADC_T.
 * @param  enSrc: The source clock of ADC_CLK, @ref EN_ADC_CLK_SRC_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_adc_clk_src(EN_CRG_ADC_T enADC, EN_ADC_CLK_SRC_T enSrc)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_ADC_CLK_SRC(enSrc))
    {
        return ERR_PARA_ERR;
    }

    switch (enADC)
    {
    case CRG_ADC0:
    {
        u32Cfg = CRG->CRG_CLK_DIV_CFG1;

        u32Cfg &= ~(CRG_ADC_CLK_SRC_SEL_MSK << CRG_ADC0_CLK_SRC_SEL_SHIFT);
        u32Cfg |= (enSrc & CRG_ADC_CLK_SRC_SEL_MSK) << CRG_ADC0_CLK_SRC_SEL_SHIFT;

        CRG->CRG_CLK_DIV_CFG1 = u32Cfg;
        break;
    }
    case CRG_ADC1:
    {
        u32Cfg = CRG->CRG_CLK_DIV_CFG2;

        u32Cfg &= ~(CRG_ADC_CLK_SRC_SEL_MSK << CRG_ADC1_CLK_SRC_SEL_SHIFT);
        u32Cfg |= (enSrc & CRG_ADC_CLK_SRC_SEL_MSK) << CRG_ADC1_CLK_SRC_SEL_SHIFT;

        CRG->CRG_CLK_DIV_CFG2 = u32Cfg;
        break;
    }
    default:
    {
        return ERR_PARA_ERR;
    }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Select GTIM Clock(GTIMn_CLK) Source.
 * @param  enGTIM: GTIM channel, @ref EN_CRG_GTIM_T.
 * @param  enSrc: The source of GTIM clock, @ref EN_GTIM_CLK_SRC_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_gtim_clk_src(EN_CRG_GTIM_T enGTIM, EN_GTIM_CLK_SRC_T enSrc)
{
    uint8_t u8RegShift = 0;
    uint32_t u32Cfg = 0;

    if (!IS_VALID_GTIM_CLK_SRC(enSrc))
    {
        return ERR_PARA_ERR;
    }

    switch (enGTIM)
    {
    case CRG_GTIM0:
    {
        u8RegShift = CRG_GTIM0_CLK_SRC_SHIFT;
        break;
    }
    case CRG_GTIM1:
    {
        u8RegShift = CRG_GTIM1_CLK_SRC_SHIFT;
        break;
    }
    case CRG_GTIM2:
    {
        u8RegShift = CRG_GTIM2_CLK_SRC_SHIFT;
        break;
    }
    case CRG_GTIM3:
    {
        u8RegShift = CRG_GTIM3_CLK_SRC_SHIFT;
        break;
    }
    default:
    {
        return ERR_PARA_ERR;
    }
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG1;

    u32Cfg &= ~(CRG_GTIM_CLK_SRC_MSK << u8RegShift);
    u32Cfg |= (enSrc & CRG_GTIM_CLK_SRC_MSK) << u8RegShift;

    CRG->CRG_CLK_DIV_CFG1 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Get GPTimer Clock(GTIMn_CLK) Source.
 * @param  enGTIM: GTIM channel, @ref EN_CRG_GTIM_T.
 * @param  pu32Src: Point to GPTimer source, @ref EN_GTIM_CLK_SRC_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_get_gtim_clk_src(EN_CRG_GTIM_T enGTIM, uint32_t *pu32Src)
{
    uint8_t u8RegShift = 0;

    if (NULL == pu32Src)
    {
        return ERR_STA_ERROR;
    }

    switch (enGTIM)
    {
    case CRG_GTIM0:
    {
        u8RegShift = CRG_GTIM0_CLK_SRC_SHIFT;
        break;
    }
    case CRG_GTIM1:
    {
        u8RegShift = CRG_GTIM1_CLK_SRC_SHIFT;
        break;
    }
    case CRG_GTIM2:
    {
        u8RegShift = CRG_GTIM2_CLK_SRC_SHIFT;
        break;
    }
    case CRG_GTIM3:
    {
        u8RegShift = CRG_GTIM3_CLK_SRC_SHIFT;
        break;
    }
    default:
    {
        return ERR_PARA_ERR;
    }
    }

    *pu32Src = (CRG->CRG_CLK_CFG >> u8RegShift) & CRG_GTIM_CLK_SRC_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Select QSPI Clock(QSPIn_CLK) Source.
 * @param  enQSPI: QSPI channel, @ref EN_CRG_QSPI_T.
 * @param  enSrc: The source of QSPI clock, @ref EN_QSPI_CLK_SRC_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_qspi_clk_src(EN_CRG_QSPI_T enQSPI, EN_QSPI_CLK_SRC_T enSrc)
{
    uint8_t u8Shift = 0;
    uint32_t u32Cfg = 0;

    if (!IS_VALID_QSPI_CLK_SRC(enSrc))
    {
        return ERR_PARA_ERR;
    }

    switch (enQSPI)
    {
    case CRG_QSPI0:
    {
        u8Shift = CRG_QSPI0_CLK_SRC_SHIFT;
        break;
    }
    case CRG_QSPI1:
    {
        u8Shift = CRG_QSPI1_CLK_SRC_SHIFT;
        break;
    }
    case CRG_QSPI2:
    {
        u8Shift = CRG_QSPI2_CLK_SRC_SHIFT;
        break;
    }
    case CRG_QSPI3:
    {
        u8Shift = CRG_QSPI3_CLK_SRC_SHIFT;
        break;
    }
    default:
    {
        return ERR_PARA_ERR;
    }
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG2;

    u32Cfg &= ~(CRG_QSPI_CLK_SRC_MSK << u8Shift);
    u32Cfg |= (enSrc & CRG_QSPI_CLK_SRC_MSK) << u8Shift;

    CRG->CRG_CLK_DIV_CFG2 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Configure Analog Peripheral Outout Clock(ANA_OUT_CLK) output from gpio PB15.
 *         Need set pid of PB15 to PID_TEST_CLK_OUT.
 *         ==> rom_hw_gpio_set_pin_pid( GPIOB, GPIO_PIN_15, PID_DEBUG_MODE);
 * @param  enClk: The clock which would be output, @ref EN_ANA_OUT_CLK_SRC_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_ana_output_clk_src(EN_ANA_OUT_CLK_SRC_T enClk)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_ANA_OUT_CLK_SRC(enClk))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG1;

    u32Cfg &= ~(CRG_ANA_OUT_CLK_SRC_SEL_MSK << CRG_ANA_OUT_CLK_SRC_SEL_SHIFT);
    u32Cfg |= (enClk & CRG_ANA_OUT_CLK_SRC_SEL_MSK) << CRG_ANA_OUT_CLK_SRC_SEL_SHIFT;

    CRG->CRG_CLK_DIV_CFG1 = u32Cfg;

    return ERR_STA_OK;
}

/**********************************************************************************************************************
 *                                             CRG Clock Division
 *********************************************************************************************************************/

/**
 * @brief  Enable RC High Clock(RC_HCLK) or DCXO High Clock(DCXO_HCLK) Division to LPWR_CLK.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_enable_hclk_div_lclk(void)
{
    CRG->CRG_CLK_CFG |= (ENABLE & CRG_HCLK_DIV_LCLK_EN_MSK) << CRG_HCLK_DIV_LCLK_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable RC High Clock(RC_HCLK) or DCXO High Clock(DCXO_HCLK) Division to LPWR_CLK.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_disable_hclk_div_lclk(void)
{
    CRG->CRG_CLK_CFG &= ~((ENABLE & CRG_HCLK_DIV_LCLK_EN_MSK) << CRG_HCLK_DIV_LCLK_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Select GTIM System Clock (GTIM_SCLK) division with SYS_CLK.
 * @param  enDiv: The divisor of system clock, @ref EN_GTIM_SYS_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
**/
EN_ERR_STA_T rom_hw_crg_set_gtim_sys_clk_divisor(EN_GTIM_SYS_CLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;

    if (!IS_GTIM_SYS_CLK_DIV_VALID(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG0;
    u32Cfg &= ~(CRG_GTIM_SYS_CLK_DIV_MSK << CRG_GTIM_SYS_CLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_GTIM_SYS_CLK_DIV_MSK) << CRG_GTIM_SYS_CLK_DIV_SHIFT;
    CRG->CRG_CLK_DIV_CFG0 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Select System PMU Clock(PMU_HCLK) division with SYS_CLK.
 * @param  enDiv: The divisor of apb clock, @ref EN_PMU_HCLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_pmu_hclk_divisor(EN_PMU_HCLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_PMU_CLK_DIV(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG0;

    u32Cfg &= ~(CRG_PMU_HCLK_DIV_MSK << CRG_PMU_HCLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_PMU_HCLK_DIV_MSK) << CRG_PMU_HCLK_DIV_SHIFT;

    CRG->CRG_CLK_DIV_CFG0 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Select MP APB Clock(MP_APB_CLK) division with AHB_CLK.
 * @param  enDiv: The divisor of apb clock, @ref EN_APB_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_apb_clk_divisor(EN_APB_CLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;
    uint8_t u8Count = 0;
    uint8_t u8CurrentDiv = 0;

    if (!IS_VALID_APB_CLK_DIV(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG0;

    u32Cfg &= ~(CRG_MP_APB_CLK_DIV_MSK << CRG_MP_APB_CLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_MP_APB_CLK_DIV_MSK) << CRG_MP_APB_CLK_DIV_SHIFT;

    CRG->CRG_CLK_DIV_CFG0 = u32Cfg;

    do
    {
        u8CurrentDiv = (CRG->CRG_CLK_DIV_STA >> CRG_MP_APB_CLK_DIV_STA_SHIFT) & CRG_MP_APB_CLK_DIV_STA_MSK;

        if (CRG_CLK_DIV_TIMEOUT_US < u8Count)
        {
            return ERR_STA_TIMEOUT;
        }

        u8Count++;
        rom_delay_us(1);

    } while ((enDiv) != (u8CurrentDiv));

    return ERR_STA_OK;
}

/**
 * @brief  Select CP AHB Clock(CP_AHB_CLK, CM0P) Divison With MP_AHB_CLK. (CP_AHB_CLK must >= 16MHz).
 * @param  enDiv: The divisor of ahb clock, @ref EN_AHB_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_cp_ahb_clk_divisor(EN_AHB_CLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;
    uint8_t u8Count = 0;
    uint8_t u8CurrentDiv = 0;

    if (!IS_VALID_AHB_CLK_DIV(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG0;

    u32Cfg &= ~(CRG_CP_AHB_CLK_DIV_MSK << CRG_CP_AHB_CLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_CP_AHB_CLK_DIV_MSK) << CRG_CP_AHB_CLK_DIV_SHIFT;

    CRG->CRG_CLK_DIV_CFG0 = u32Cfg;

    do
    {
        u8CurrentDiv = (CRG->CRG_CLK_DIV_STA >> CRG_CP_AHB_CLK_DIV_STA_SHIFT) & CRG_CP_AHB_CLK_DIV_STA_MSK;

        if (CRG_CLK_DIV_TIMEOUT_US < u8Count)
        {
            return ERR_STA_TIMEOUT;
        }

        u8Count++;
        rom_delay_us(1);

    } while ((enDiv) != (u8CurrentDiv));

    return ERR_STA_OK;
}

/**
 * @brief  Configure RC_HCLK frequency to 16MHz or 48MHz.
 *         16MHz: Enable rc_hclk division by 3.
 * @param  enFreq: RC_HCLK frequency, @ref EN_RC_HCLK_FREQ_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_rc_hclk_frequency(EN_RC_HCLK_FREQ_T enFreq)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_RC_HCLK_FREQ(enFreq))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG2;

    u32Cfg &= ~(CRG_RC_HCLK_DIV_EN_MSK << CRG_RC_HCLK_DIV_EN_SHIFT);
    u32Cfg |= enFreq << CRG_RC_HCLK_DIV_EN_SHIFT;

    CRG->CRG_CLK_DIV_CFG2 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Configure MP AHB Clock(MP_AHB_CLK, CM33) Divison With SYS_CLK. (MP_AHB_CLK must >= 16MHz).
 * @param  enDiv: The divisor of ahb clock, @ref EN_AHB_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_mp_ahb_clk_divisor(EN_AHB_CLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;
    uint8_t u8Count = 0;
    uint8_t u8CurrentDiv = 0;

    if (!IS_VALID_AHB_CLK_DIV(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG0;

    u32Cfg &= ~(CRG_MP_AHB_CLK_DIV_MSK << CRG_MP_AHB_CLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_MP_AHB_CLK_DIV_MSK) << CRG_MP_AHB_CLK_DIV_SHIFT;

    CRG->CRG_CLK_DIV_CFG0 = u32Cfg;

    do
    {
        u8CurrentDiv = (CRG->CRG_CLK_DIV_STA >> CRG_MP_AHB_CLK_DIV_STA_SHIFT) & CRG_MP_AHB_CLK_DIV_STA_MSK;

        if (CRG_CLK_DIV_TIMEOUT_US < u8Count)
        {
            return ERR_STA_TIMEOUT;
        }

        u8Count++;
        rom_delay_us(1);

    } while ((enDiv) != (u8CurrentDiv));

    return ERR_STA_OK;
}

/**
 * @brief  Select pll128m clock (PLL128M_CLK) Division.
 * @param  enDiv: The divisor of PLL128M_CLK, @ref EN_PLL128M_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_pll128m_clk_divisor(EN_PLL128M_CLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_PLL128M_CLK_DIV(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_CFG;

    u32Cfg &= ~(CRG_PLL128M_CLK_DIV_MSK << CRG_PLL128M_CLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_PLL128M_CLK_DIV_MSK) << CRG_PLL128M_CLK_DIV_SHIFT;

    CRG->CRG_CLK_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Configure Audio ADC Clock(ADC_CLK) division with SYS_CLK.
 * @param  enADC: Audio ADC channel, @ref EN_CRG_ADC_T.
 * @param  u8Div: The divisor of ADC_CLK. From 0 to 255, Step by 1.
 *                0-->Division by 2;
 *                1-->Division by 4;
 *                2-->Division by 6;
 *                ......
 *                255-->Division by 512;
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_adc_clk_divisor(EN_CRG_ADC_T enADC, uint8_t u8Div)
{
    uint32_t u32Cfg = 0;

    switch (enADC)
    {
    case CRG_ADC0:
    {
        u32Cfg = CRG->CRG_CLK_DIV_CFG1;

        u32Cfg &= ~(CRG_ADC_CLK_DIV_MSK << CRG_ADC0_CLK_DIV_SHIFT);
        u32Cfg |= (u8Div & CRG_ADC_CLK_DIV_MSK) << CRG_ADC0_CLK_DIV_SHIFT;

        CRG->CRG_CLK_DIV_CFG1 = u32Cfg;
        break;
    }
    case CRG_ADC1:
    {
        u32Cfg = CRG->CRG_CLK_DIV_CFG2;

        u32Cfg &= ~(CRG_ADC_CLK_DIV_MSK << CRG_ADC1_CLK_DIV_SHIFT);
        u32Cfg |= (u8Div & CRG_ADC_CLK_DIV_MSK) << CRG_ADC1_CLK_DIV_SHIFT;

        CRG->CRG_CLK_DIV_CFG2 = u32Cfg;
        break;
    }
    default:
    {
        return ERR_PARA_ERR;
    }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Configure I2S BClock (I2S_BCLK) division with PERI_CLK.
 * @param  u8Div: The divisor of I2S_BCLK, @ref EN_I2S_BCLK_DIV_T.
 *                1-->Division by 1;
 *                2-->Division by 2;
 *                4-->Division by 8;
 *                ......
 *                31-->Division by 62;
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_i2s_bclk_divisor(EN_I2S_BCLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;

    if (enDiv > EN_CRG_I2S_BCLK_DIV_62)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG1;

    u32Cfg &= ~(CRG_I2S_BCLK_DIV_MSK << CRG_I2S_BCLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_I2S_BCLK_DIV_MSK) << CRG_I2S_BCLK_DIV_SHIFT;

    CRG->CRG_CLK_DIV_CFG1 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Select CPTO Clock(CPTOn_CLK) division with CP_AHB_CLK.
 * @param  enCPTO: CPTO channel, @ref EN_CRG_CPTO_T.
 * @param  enDiv: The divisor of CPTO clock, @ref EN_CPTO_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_cpto_clk_divisor(EN_CRG_CPTO_T enCPTO, EN_CPTO_CLK_DIV_T enDiv)
{
    uint8_t u8Count = 0;
    uint8_t u8CfgShift = 0;
    uint8_t u8StaShift = 0;
    uint8_t u8CurrentDiv = 0;
    uint32_t u32Cfg = 0;

    if (!IS_VALID_CPTO_CLK_DIV(enDiv))
    {
        return ERR_PARA_ERR;
    }

    switch (enCPTO)
    {
    case CRG_CPTO0:
    {
        u8CfgShift = CRG_CPTO0_CLK_DIV_SHIFT;
        u8StaShift = CRG_CPTO0_CLK_DIV_STA_SHIFT;
        break;
    }
    case CRG_CPTO1:
    {
        u8CfgShift = CRG_CPTO1_CLK_DIV_SHIFT;
        u8StaShift = CRG_CPTO1_CLK_DIV_STA_SHIFT;
        break;
    }
    default:
    {
        return ERR_PARA_ERR;
    }
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG0;

    u32Cfg &= ~(CRG_CPTO_CLK_DIV_MSK << u8CfgShift);
    u32Cfg |= (enDiv & CRG_CPTO_CLK_DIV_MSK) << u8CfgShift;

    CRG->CRG_CLK_DIV_CFG0 = u32Cfg;

    do
    {
        u8CurrentDiv = (CRG->CRG_CLK_DIV_STA >> u8StaShift) & CRG_CPTO_CLK_DIV_STA_MSK;

        if (CRG_CLK_DIV_TIMEOUT_US < u8Count)
        {
            return ERR_STA_TIMEOUT;
        }

        u8Count++;
        rom_delay_us(1);

    } while ((enDiv) != (u8CurrentDiv));

    return ERR_STA_OK;
}

/**
 * @brief  Select LLC Clock(LLC_CLK) division with CP_AHB_CLK.
 * @param  enDiv: The divisor of llc clock, @ref EN_LLC_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_llc_clk_divisor(EN_LLC_CLK_DIV_T enDiv)
{
    uint8_t u8Count = 0;
    uint8_t u8CurrentDiv = 0;
    uint32_t u32Cfg = 0;

    if (!IS_VALID_LLC_CLK_DIV(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG0;

    u32Cfg &= ~(CRG_LLC_CLK_DIV_MSK << CRG_LLC_CLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_LLC_CLK_DIV_MSK) << CRG_LLC_CLK_DIV_SHIFT;

    CRG->CRG_CLK_DIV_CFG0 = u32Cfg;

    do
    {
        u8CurrentDiv = (CRG->CRG_CLK_DIV_STA >> CRG_LLC_CLK_DIV_STA_SHIFT) & CRG_LLC_CLK_DIV_STA_MSK;

        if (CRG_CLK_DIV_TIMEOUT_US < u8Count)
        {
            return ERR_STA_TIMEOUT;
        }

        u8Count++;
        rom_delay_us(1);

    } while ((enDiv) != (u8CurrentDiv));

    return ERR_STA_OK;
}

/**
 * @brief  Select QSPI Clock(QSPIn_CLK) division with QSPIn_CLK.
 * @param  enQSPI: QSPI channel, @ref EN_CRG_QSPI_T.
 * @param  enDiv: The divisor of QSPI clock, @ref EN_QSPI_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_qspi_clk_divisor(EN_CRG_QSPI_T enQSPI, EN_QSPI_CLK_DIV_T enDiv)
{
    uint8_t u8Shift = 0;
    uint32_t u32Cfg = 0;

    if (!IS_VALID_QSPI_CLK_DIV(enDiv))
    {
        return ERR_PARA_ERR;
    }

    switch (enQSPI)
    {
    case CRG_QSPI0:
    {
        u8Shift = CRG_QSPI0_CLK_DIV_SHIFT;
        break;
    }
    case CRG_QSPI1:
    {
        u8Shift = CRG_QSPI1_CLK_DIV_SHIFT;
        break;
    }
    case CRG_QSPI2:
    {
        u8Shift = CRG_QSPI2_CLK_DIV_SHIFT;
        break;
    }
    case CRG_QSPI3:
    {
        u8Shift = CRG_QSPI3_CLK_DIV_SHIFT;
        break;
    }
    default:
    {
        return ERR_PARA_ERR;
    }
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG2;

    u32Cfg &= ~(CRG_QSPI_CLK_DIV_MSK << u8Shift);
    u32Cfg |= (enDiv & CRG_QSPI_CLK_DIV_MSK) << u8Shift;

    CRG->CRG_CLK_DIV_CFG2 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Select System Out Clock (OUTPUT_CLK) division with SYS_CLK.
 * @param  u8Div: The divisor of OUTPUT_CLK, @ref EN_SYS_OUT_CLK_DIV_T.
 *                0-->Division by 1;
 *                1-->Division by 2;
 *                2-->Division by 4;
 *                ......
 *                15-->Division by 30;
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_set_sys_clk_out_divisor(EN_SYS_OUT_CLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;

    if (enDiv > EN_CRG_SYS_OUT_CLK_DIV_30)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG1;

    u32Cfg &= ~(CRG_SYS_OUT_CLK_DIV_MSK << CRG_SYS_OUT_CLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_SYS_OUT_CLK_DIV_MSK) << CRG_SYS_OUT_CLK_DIV_SHIFT;

    CRG->CRG_CLK_DIV_CFG1 = u32Cfg;

    return ERR_STA_OK;
}

/**********************************************************************************************************************
 *                                             CRG Clock Source
 *********************************************************************************************************************/

/**
 * @brief  Turn xx_CLK clock gating on.
 * @param  u8En: Indicate which clock gate will be turned on. @ref EN_CRG_CLK_GATE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_enable_clk_gate(uint8_t u8En)
{
    if (u8En > CRG_CLK_GATE_MAX)
    {
        return ERR_PARA_ERR;
    }

    if (u8En >= WORD_BITS)
    {
        CRG->CRG_CLK_EN1 |= 0x01 << (u8En - WORD_BITS);
    }
    else
    {
        CRG->CRG_CLK_EN0 |= 0x01 << u8En;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Turn xx_CLK clock gating off.
 * @param  u8Dis: Indicate which clock gate will be turned off. @ref EN_CRG_CLK_GATE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_disable_clk_gate(uint8_t u8Dis)
{
    if (u8Dis > CRG_CLK_GATE_MAX)
    {
        return ERR_PARA_ERR;
    }

    if (u8Dis >= WORD_BITS)
    {
        CRG->CRG_CLK_EN1 &= ~(0x01 << (u8Dis - WORD_BITS));
    }
    else
    {
        CRG->CRG_CLK_EN0 &= ~(0x01 << u8Dis);
    }

    return ERR_STA_OK;
}

/**
 * @brief  Turn xx_CLK clock gating on or off. Each bit corresponding a module.
 *         0: Turn xx_CLK clock gating off.
 *         1: Turn xx_CLK clock gating on.
 * @param  u32Reg0: Clock gate control register 0.
 * @param  u32Reg1: Clock gate control register 1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_clk_gate_ctrl(uint32_t u32Reg0, uint32_t u32Reg1)
{
    CRG->CRG_CLK_EN0 = u32Reg0;

    CRG->CRG_CLK_EN1 = (u32Reg1 & CRG_CLK_EN1_MSK) << CRG_CLK_EN1_SHIFT;

    return ERR_STA_OK;
}

/**********************************************************************************************************************
 *                                             CRG Reset Control
 *********************************************************************************************************************/

/**
 * @brief  Keeping xx Module Software Reseting.
 * @param  enReset: xx module software reset.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_keep_reset(EN_CRG_SW_RST_T enReset)
{
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_CRG_SW_RST(enReset)))
    {
        return ERR_PARA_ERR;
    }

    if (enReset <= CRG_SW_RST0_IDX_MAX)
    {
        u32Cfg = CRG->CRG_PERI_SW_RST_EN0;
        u32Cfg &= ~(0x01 << enReset);
        CRG->CRG_PERI_SW_RST_EN0 = u32Cfg;
    }
    else
    {
        u32Cfg = CRG->CRG_PERI_SW_RST_EN1;
        u32Cfg &= ~(0x01 << (enReset - CRG_SW_RST0_IDX_MAX - 1));
        CRG->CRG_PERI_SW_RST_EN1 = u32Cfg;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Release xx Module Software Reset.
 * @param  enReset: xx module software release reset.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_release_reset(EN_CRG_SW_RST_T enReset)
{
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_CRG_SW_RST(enReset)))
    {
        return ERR_PARA_ERR;
    }

    if (enReset <= CRG_SW_RST0_IDX_MAX)
    {
        u32Cfg = CRG->CRG_PERI_SW_RST_EN0;
        u32Cfg |= (0x01 << enReset);
        CRG->CRG_PERI_SW_RST_EN0 = u32Cfg;
    }
    else
    {
        u32Cfg = CRG->CRG_PERI_SW_RST_EN1;
        u32Cfg |= (0x01 << (enReset - CRG_SW_RST0_IDX_MAX - 1));
        CRG->CRG_PERI_SW_RST_EN1 = u32Cfg;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Control xx Module Software Reset. Each bit corresponding a module.
 *         0: Release xx Module Software Reset.
 *         1: Keeping xx Module Software Reseting.
 * @param  u32Reg0: Software reset control register 0
 * @param  u32Reg1: Software reset control register 1
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_reset_ctrl(uint32_t u32Reg0, uint32_t u32Reg1)
{
    CRG->CRG_PERI_SW_RST_EN0 = u32Reg0;

    CRG->CRG_PERI_SW_RST_EN1 = (u32Reg1 & CRG_PERI_SW_RST_EN1_MSK) << CRG_PERI_SW_RST_EN1_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Main-Processor(mp)(cm33) Software Reset.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_mp_reset(void)
{
    CRG->CRG_MP_SW_RST_EN = (ENABLE & CRG_MP_SW_RST_EN_MSK) << CRG_MP_SW_RST_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  CO-Processor(cp)(cm0p) Software Reset.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_cp_reset(void)
{
    CRG->CRG_CP_SW_RST_EN = (ENABLE & CRG_CP_SW_RST_EN_MSK) << CRG_CP_SW_RST_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  System Software Reset.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_system_reset(void)
{
    CRG->CRG_SYS_SW_RST_EN = CRG_SYS_SW_RST << CRG_SYS_SW_RST_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Get reset flag(status).
 * @param  pu16RstFlag: Indicate which reset flag will be read.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_get_reset_flag(uint16_t *pu16RstFlag)
{
    if (NULL == pu16RstFlag)
    {
        return ERR_PARA_ERR;
    }

    *pu16RstFlag = CRG->CRG_RST_FLG & CRG_RESET_MASK;

    return ERR_STA_OK;
}

/**
 * @brief  Clear reset flag(status).
 * @param  u16Flag: Indicate which flag will be cleared.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_clear_reset_flag(uint16_t u16Flag)
{
    if (u16Flag > CRG_RESET_MASK)
    {
        return ERR_PARA_ERR;
    }

    CRG->CRG_RST_CLR = u16Flag;

    return ERR_STA_OK;
}

/**
 * @brief  Enable WDT1 reset system.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_enable_wdt1_reset_sys(void)
{
    CRG->CRG_WDT1_RST_EN &= ~(CRG_WDT1_RST_EN_MSK << CRG_WDT1_RST_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Disable WDT1 reset system.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_disable_wdt1_reset_sys(void)
{
    CRG->CRG_WDT1_RST_EN |= CRG_WDT1_RST_EN_MSK << CRG_WDT1_RST_EN_SHIFT;

    return ERR_STA_OK;
}

/**********************************************************************************************************************
 *                                             CRG Calibration
 *********************************************************************************************************************/

/**
 * @brief  Configure Calibrate RC Clock.
 * @param  enSel: RC clock source selection, @ref EN_CRG_CALIB_CLK_SEL_T.
 * @param  u8Speed: The speed of calibration. The smaller the value, the faster the speed of calibration.
 *                  The range if [0,15].
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_cfg_cali_rc_clk(EN_CRG_CALIB_CLK_SEL_T enSel, uint8_t u8Speed)
{
    uint32_t u32Cfg = 0;

    if (!IS_VALID_CRG_CALIB_CLK_SEL(enSel))
    {
        return ERR_STA_ERROR;
    }

    u32Cfg = CRG->CRG_CALIB_CTRL;

    u32Cfg &= ~(CRG_CALIB_CLK_SEL_MSK << CRG_CALIB_CLK_SEL_SHIFT |
                CRG_CALIB_CLK_SPEED_CFG_MSK << CRG_CALIB_CLK_SPEED_CFG_SHIFT);

    u32Cfg |= (u8Speed & CRG_CALIB_CLK_SPEED_CFG_MSK) << CRG_CALIB_CLK_SPEED_CFG_SHIFT;
    u32Cfg |= (enSel & CRG_CALIB_CLK_SEL_MSK) << CRG_CALIB_CLK_SEL_SHIFT;

    CRG->CRG_CALIB_CTRL = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Enable pmu calibration.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_enable_pmu_cali(void)
{
    CRG->CRG_CALIB_CTRL |= CRG_PMU_CALIB_EN_MSK << CRG_PMU_CALIB_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable pmu calibration.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_disable_pmu_cali(void)
{
    CRG->CRG_CALIB_CTRL &= ~(CRG_PMU_CALIB_EN_MSK << CRG_PMU_CALIB_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Enable calibration.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_enable_cali(void)
{
    CRG->CRG_CALIB_CTRL |= CRG_CALIB_EN_MSK << CRG_CALIB_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable calibration.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_crg_disable_cali(void)
{
    CRG->CRG_CALIB_CTRL &= ~(CRG_CALIB_EN_MSK << CRG_CALIB_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Get cycles of DCXO16M or DCXO32M in every RC32K cycle.
 * @return uint16_t: Cycles of DCXO16M or DCXO32M in every RC32K cycle.
 */
uint16_t rom_hw_crg_get_32k_cycles_of_dcxo_hclk(void)
{
    uint16_t u16Cycles = 0;

    u16Cycles = CRG->CRG_CALIB_32K_PERIOD & CRG_CALIB_32K_PERIOD_MSK;

    return u16Cycles;
}
