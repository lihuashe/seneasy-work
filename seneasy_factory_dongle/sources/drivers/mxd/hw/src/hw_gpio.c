/**********************************************************************************************************************
 * @file     hw_gpio.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/1/11
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

#include "RegMXD2670.h"
#include "type_def.h"
#include "err_def.h"


/**********************************************************************************************************************
 *                                             GPIO Function
 *********************************************************************************************************************/

/**
 * @brief  Configure gpio pins.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: Which pins need to be configure.
 * @param  u32Cfg: Pin configuration.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_pin_cfg(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, uint32_t u32Cfg)
{
    uint8_t i = 0;
    uint8_t pinNum = 0;
    uint8_t pinTemp = 0;
    uint8_t iomuxIdx = 0;
    uint32_t pin = u32Pin;

    if (!(IS_VALID_GPIO_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    if ((GPIOA) == (pstGPIO))
    {
        pinNum = GPIO_PORT_A_NUM;
        iomuxIdx = 0;
    }
    else
    {
        pinNum = GPIO_PORT_B_NUM;
        iomuxIdx = GPIO_PORT_A_NUM;
    }

    for (i = 0; ((i < pinNum) && (pin)); i++)
    {
        if (pin & (0x01 << i))
        {
            pin ^= (0x01 << i);
            pinTemp = i + iomuxIdx;
            IOMUX->IOMUX_CFG[pinTemp] = u32Cfg;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Initialize a GPIO.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: Which pins need to be config.
 * @param  u32Cfg: The GPIO Pin config.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_pin_init(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, uint32_t u32Cfg)
{
    return rom_hw_gpio_set_pin_cfg(pstGPIO, u32Pin, u32Cfg);
}

/**
 * @brief  Config gpio input and output mode.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be config, @ref EN_GPIO_PIN_T.
 * @param  enMode: Pin input or output mode, @ref EN_GPIO_PIN_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_pin_input_output(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, EN_GPIO_PIN_MODE_T enMode)
{
    uint8_t i = 0;
    uint8_t pinNum = 0;
    uint8_t pinTemp = 0;
    uint8_t iomuxIdx = 0;
    uint32_t pin = u32Pin;
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (!IS_VALID_GPIO_MODE(enMode)))
    {
        return ERR_PARA_ERR;
    }

    if ((GPIOA) == (pstGPIO))
    {
        pinNum = GPIO_PORT_A_NUM;
        iomuxIdx = 0;
    }
    else
    {
        pinNum = GPIO_PORT_B_NUM;
        iomuxIdx = GPIO_PORT_A_NUM;
    }

    for (i = 0; ((i < pinNum) && (pin)); i++)
    {
        if (pin & (0x01 << i))
        {
            pin ^= (0x01 << i);
            pinTemp = i + iomuxIdx;

            u32Cfg = IOMUX->IOMUX_CFG[pinTemp];
            u32Cfg &= ~(GPIO_MODE_MSK << GPIO_MODE_SHIFT);
            u32Cfg |= (enMode & GPIO_MODE_MSK) << GPIO_MODE_SHIFT;
            IOMUX->IOMUX_CFG[pinTemp] = u32Cfg;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Sets or clears indicated pins output level.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be set or clear, @ref EN_GPIO_PIN_T.
 * @param  enLevel: Output level status, @ref EN_GPIO_LEVEL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_pin_output_level(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, EN_GPIO_LEVEL_T enLevel)
{
    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (!IS_VALID_GPIO_LEVEL(enLevel)))
    {
        return ERR_PARA_ERR;
    }

    if (GPIO_LEVEL_LO == enLevel)
    {
        pstGPIO->GPIO_OUT_CLR = u32Pin;
    }
    else
    {
        pstGPIO->GPIO_OUT_SET = u32Pin;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Toggle indicated pins output level.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be toggle, @ref EN_GPIO_PIN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_toggle_pin_output_level(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin)
{
    if (!(IS_VALID_GPIO_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    if (pstGPIO->GPIO_OUT_STATUS & u32Pin)
    {
        pstGPIO->GPIO_OUT_CLR = u32Pin;
    }
    else
    {
        pstGPIO->GPIO_OUT_SET = u32Pin;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Get indicated pins output status.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be get status, @ref EN_GPIO_PIN_T.
 * @param  pu32Level: Pins status, @ref EN_GPIO_LEVEL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_get_pin_output_level(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, uint32_t *pu32Level)
{
    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (NULL == pu32Level))
    {
        return ERR_PARA_ERR;
    }

    *pu32Level = pstGPIO->GPIO_OUT_STATUS & u32Pin;

    return ERR_STA_OK;
}

/**
 * @brief  Get indicated pins input status.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be get status, @ref EN_GPIO_PIN_T.
 * @param  pu32Level: Pins status, @ref EN_GPIO_LEVEL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_get_pin_input_level(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, uint32_t *pu32Level)
{
    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (NULL == pu32Level))
    {
        return ERR_PARA_ERR;
    }

    *pu32Level = pstGPIO->GPIO_IN_STATUS & u32Pin;

    return ERR_STA_OK;
}

/**
 * @brief  Set indicated pin peripheral function.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be set, @ref EN_GPIO_PIN_T.
 * @param  enPID: Pin peripheral function choice, @ref EN_GPIO_PID_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_pin_pid(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, EN_GPIO_PID_T enPID)
{
    uint8_t i = 0;
    uint8_t pinNum = 0;
    uint8_t pinTemp = 0;
    uint8_t iomuxIdx = 0;
    uint32_t pin = u32Pin;
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (!IS_VALID_GPIO_PID(enPID)))
    {
        return ERR_PARA_ERR;
    }

    if ((GPIOA) == (pstGPIO))
    {
        pinNum = GPIO_PORT_A_NUM;
        iomuxIdx = 0;
    }
    else
    {
        pinNum = GPIO_PORT_B_NUM;
        iomuxIdx = GPIO_PORT_A_NUM;
    }

    for (i = 0; ((i < pinNum) && (pin)); i++)
    {
        if (pin & (0x01 << i))
        {
            pin ^= (0x01 << i);
            pinTemp = i + iomuxIdx;

            u32Cfg = IOMUX->IOMUX_CFG[pinTemp];
            u32Cfg &= ~(GPIO_PID_MASK << GPIO_PID_SHIFT);
            u32Cfg |= (enPID & GPIO_PID_MASK) << GPIO_PID_SHIFT;
            IOMUX->IOMUX_CFG[pinTemp] = u32Cfg;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Enable specific io for qspi function.
 * @param  enCh: QSPI channel, @ref EN_QSPI_FIXED_GPIO_EN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_enable_qspi_pid(EN_QSPI_FIXED_GPIO_EN_T enCh)
{
    if (IOMUX_QSPI_FIXED_GPIO_EN_MSK < enCh)
    {
        return ERR_STA_ERROR;
    }

    IOMUX->IOMUX_QSPI_CTRL |= enCh << IOMUX_QSPI_FIXED_GPIO_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable specific io for qspi function.
 * @param  enCh: QSPI channel, @ref EN_QSPI_FIXED_GPIO_EN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_disable_qspi_pid(EN_QSPI_FIXED_GPIO_EN_T enCh)
{
    if (IOMUX_QSPI_FIXED_GPIO_EN_MSK < enCh)
    {
        return ERR_STA_ERROR;
    }

    IOMUX->IOMUX_QSPI_CTRL &= ~(enCh << IOMUX_QSPI_FIXED_GPIO_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Config indicated pins pull up or pull down.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be config, @ref EN_GPIO_PIN_T.
 * @param  enMode: Pin pull mode, @ref EN_GPIO_PULL_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_pin_pull_mode(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, EN_GPIO_PULL_MODE_T enMode)
{
    uint8_t i = 0;
    uint8_t pinNum = 0;
    uint8_t pinTemp = 0;
    uint8_t iomuxIdx = 0;
    uint32_t pin = u32Pin;
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (!IS_VALID_GPIO_PULL_MODE(enMode)))
    {
        return ERR_PARA_ERR;
    }

    if ((GPIOA) == (pstGPIO))
    {
        pinNum = GPIO_PORT_A_NUM;
        iomuxIdx = 0;
    }
    else
    {
        pinNum = GPIO_PORT_B_NUM;
        iomuxIdx = GPIO_PORT_A_NUM;
    }

    for (i = 0; ((i < pinNum) && (pin)); i++)
    {
        if (pin & (0x01 << i))
        {
            pin ^= (0x01 << i);
            pinTemp = i + iomuxIdx;

            u32Cfg = IOMUX->IOMUX_CFG[pinTemp];
            u32Cfg &= ~(GPIO_PULL_MSK << GPIO_PULL_SHIFT);
            u32Cfg |= (enMode & GPIO_PULL_MSK) << GPIO_PULL_SHIFT;
            IOMUX->IOMUX_CFG[pinTemp] = u32Cfg;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Config indicated pins drive strength.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be config, @ref EN_GPIO_PIN_T.
 * @param  enStrength: Pin drive strength, @ref EN_GPIO_DRIVE_STRENGTH_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_pin_drive_strength(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin,
                                                EN_GPIO_DRV_STRENGTH_T enStrength)
{
    uint8_t i = 0;
    uint8_t pinNum = 0;
    uint8_t pinTemp = 0;
    uint8_t iomuxIdx = 0;
    uint32_t pin = u32Pin;
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (!IS_GPIO_DRV_STRENGTH_VALID(enStrength)))
    {
        return ERR_PARA_ERR;
    }

    if ((GPIOA) == (pstGPIO))
    {
        pinNum = GPIO_PORT_A_NUM;
        iomuxIdx = 0;
    }
    else
    {
        pinNum = GPIO_PORT_B_NUM;
        iomuxIdx = GPIO_PORT_A_NUM;
    }

    for (i = 0; ((i < pinNum) && (pin)); i++)
    {
        if (pin & (0x01 << i))
        {
            pin ^= (0x01 << i);
            pinTemp = i + iomuxIdx;

            u32Cfg = IOMUX->IOMUX_CFG[pinTemp];
            u32Cfg &= ~(GPIO_DRV_STRENGTH_MSK << GPIO_DRV_STRENGTH_SHIFT);
            u32Cfg |= (enStrength & GPIO_DRV_STRENGTH_MSK) << GPIO_DRV_STRENGTH_SHIFT;
            IOMUX->IOMUX_CFG[pinTemp] = u32Cfg;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Set indicated pin interrupt type.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be set, @ref EN_GPIO_PIN_T.
 * @param  enCh: Interrupt handle sel, @ref EN_GPIO_INT_CH_T.
 * @param  enType: Interrupt type, @ref EN_GPIO_INT_TYPE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_pin_interrupt_type(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin,
                                                EN_GPIO_INT_CH_T enCh, EN_GPIO_INT_TYPE_T enType)
{
    uint8_t i = 0;

    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (!IS_VALID_GPIO_INT_CH(enCh)) || (!IS_VALID_GPIO_INT_TYPE(enType)))
    {
        return ERR_PARA_ERR;
    }

    switch (enType)
    {
        case GPIO_INT_HIGH_LEVEL:
        {
            pstGPIO->GPIO_INT_POL &= ~u32Pin;
            pstGPIO->GPIO_INT_EORL &= ~u32Pin;
            pstGPIO->GPIO_INT_EDGE &= ~u32Pin;
            break;
        }
        case GPIO_INT_LOW_LEVEL:
        {
            pstGPIO->GPIO_INT_POL |= u32Pin;
            pstGPIO->GPIO_INT_EORL &= ~u32Pin;
            pstGPIO->GPIO_INT_EDGE &= ~u32Pin;
            break;
        }
        case GPIO_INT_RISING_EDGE:
        {
            pstGPIO->GPIO_INT_POL &= ~u32Pin;
            pstGPIO->GPIO_INT_EORL |= u32Pin;
            pstGPIO->GPIO_INT_EDGE &= ~u32Pin;
            break;
        }
        case GPIO_INT_FALLING_EDGE:
        {
            pstGPIO->GPIO_INT_POL |= u32Pin;
            pstGPIO->GPIO_INT_EORL |= u32Pin;
            pstGPIO->GPIO_INT_EDGE &= ~u32Pin;
            break;
        }
        case GPIO_INT_BOTH_EDGE:
        {
            pstGPIO->GPIO_INT_EORL |= u32Pin;
            pstGPIO->GPIO_INT_EDGE |= u32Pin;
            break;
        }
    }

    for (i = 0; i < GPIO_INT_CH_MAX; i++)
    {
        pstGPIO->GPIO_INT_VECTOR[i] &= ~u32Pin;
    }

    pstGPIO->GPIO_INT_VECTOR[enCh] |= u32Pin;

    return ERR_STA_OK;
}

/**
 * @brief  Get indicated pins interrupt flag(status).
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: Get indicate pins interrupt flag, @ref EN_GPIO_PIN_T.
 * @param  pu32Msk: Indicate which interrupt flag will be read.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_get_pin_interrupt_flag(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, uint32_t *pu32Msk)
{
    if (!(IS_VALID_GPIO_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    *pu32Msk = pstGPIO->GPIO_INT_FLAG & u32Pin;

    return ERR_STA_OK;
}

/**
 * @brief  Clear indicated pins interrupt flag(status).
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: Indicate pins interrupt flag will be cleared, @ref EN_GPIO_PIN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_clear_pin_interrupt_flag(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin)
{
    if (!(IS_VALID_GPIO_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    pstGPIO->GPIO_INT_CLR = u32Pin;

    return ERR_STA_OK;
}

/**
 * @brief  Enable indicated pins interrupt.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: Which pins need to be config, @ref EN_GPIO_PIN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_enable_pin_interrupt(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin)
{
    if (!(IS_VALID_GPIO_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    pstGPIO->GPIO_INT_EN |= u32Pin;

    return ERR_STA_OK;
}

/**
 * @brief  Disable indicated pins interrupt.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: Which pins need to be config, @ref EN_GPIO_PIN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_disable_pin_interrupt(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin)
{
    if (!(IS_VALID_GPIO_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    pstGPIO->GPIO_INT_EN &= ~u32Pin;
    pstGPIO->GPIO_INT_CLR = u32Pin;

    return ERR_STA_OK;
}

/**
 * @brief  Set all pins wakeup debounce.
 *         All pins share one wakeup debounce time.
 * @param  u8Time: Debounce time, 0~63.
 * @param  enUnit: Debounce unit, @ref EN_GPIO_WAKEUP_DEBOUNCE_UNIT_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_pin_wakeup_debounce(uint8_t u8Time, EN_GPIO_WAKEUP_DEBOUNCE_UNIT_T enUnit)
{
    if (!IS_VALID_GPIO_WAKEUP_DEBOUNCE_UNIT(enUnit))
    {
        return ERR_PARA_ERR;
    }

    GPIO_WAKEUP->GPIO_WAKEUP_DEBOUNCE = (((u8Time & GPIO_WAKEUP_DEB_VAL_MSK) << GPIO_WAKEUP_DEB_VAL_SHIFT) |
                                         ((enUnit & GPIO_WAKEUP_DEB_UNIT_MSK) << GPIO_WAKEUP_DEB_UNIT_SHIFT));

    GPIO_WAKEUP->GPIO_WAKEUP_TRIG = GPIO_WAKEUP_TRIG_MSK << GPIO_WAKEUP_TRIG_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Enable indicated pins wakeup function.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be enable, @ref EN_GPIO_PIN_T.
 * @param  enPol: Wakeup polarity, @ref EN_GPIO_WAKEUP_POL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_enable_pin_wakeup(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, EN_GPIO_WAKEUP_POL_T enPol)
{
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_GPIO_HANDLE(pstGPIO))) || (!IS_VALID_GPIO_WAKEUP_POL(enPol)))
    {
        return ERR_PARA_ERR;
    }

    pstGPIO->GPIO_WAKEUP_EN |= u32Pin;

    u32Cfg = pstGPIO->GPIO_WAKEUP_POL;
    if (GPIO_WAKEUP_HIGH_LEVEL == enPol)
    {
        u32Cfg &= ~u32Pin;
    }
    else
    {
        u32Cfg |= u32Pin;
    }
    pstGPIO->GPIO_WAKEUP_POL = u32Cfg;

    GPIO_WAKEUP->GPIO_WAKEUP_TRIG = GPIO_WAKEUP_TRIG_MSK << GPIO_WAKEUP_TRIG_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable indicated pins wakeup function.
 * @param  pstGPIO: GPIO peripheral handle, Should be GPIOA / GPIOB.
 * @param  u32Pin: The pins which would be disable, @ref EN_GPIO_PIN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_disable_pin_wakeup(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin)
{
    if (!(IS_VALID_GPIO_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    pstGPIO->GPIO_WAKEUP_EN &= ~u32Pin;
    GPIO_WAKEUP->GPIO_WAKEUP_TRIG = GPIO_WAKEUP_TRIG_MSK << GPIO_WAKEUP_TRIG_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Clear all pins wakeup flag.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_clear_wakeup_flag(void)
{
    GPIO_WAKEUP->GPIO_WAKEUP_CLR = GPIO_WAKEUP_CLR_MSK << GPIO_WAKEUP_CLR_SHIFT;

    return ERR_STA_OK;
}




/**********************************************************************************************************************
 *                                             GPIO SWD Function
 *********************************************************************************************************************/

/**
 * @brief  Enable m0plus or m33 swd port. If config swd port to general io, need disable swd port first.
 * @param  pstGPIO: GPIO peripheral handle, Should be SWD_M0P / SWD_M33.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_enable_swd(stGPIO_SWD_Handle_t *pstGPIO)
{
    if (!(IS_VALID_GPIO_SWD_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    pstGPIO->GPIO_SWD_DISABLE &= ~(GPIO_SWD_DISABLE_MSK << GPIO_SWD_DISABLE_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Disable m0plus or m33 swd port. If config swd port to general io, need disable swd port first.
 * @param  pstGPIO: GPIO peripheral handle, Should be SWD_M0P / SWD_M33.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_disable_swd(stGPIO_SWD_Handle_t *pstGPIO)
{
    if (!(IS_VALID_GPIO_SWD_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    pstGPIO->GPIO_SWD_DISABLE |= GPIO_SWD_DISABLE_MSK << GPIO_SWD_DISABLE_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Config indicated pin(swdio or swclk) input or output mode.
 * @param  pstGPIO: GPIO peripheral handle, Should be SWD_M0P / SWD_M33.
 * @param  u8Pin: The pins which would be config, @ref EN_GPIO_SW_NUM_T.
 * @param  enMode: Pin input or output mode, @ref EN_GPIO_PIN_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_sw_input_output(stGPIO_SWD_Handle_t *pstGPIO, uint8_t u8Pin,
                                             EN_GPIO_PIN_MODE_T enMode)
{
    uint8_t i = 0;
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_GPIO_SWD_HANDLE(pstGPIO)) || (!IS_VALID_GPIO_MODE(enMode)))
    {
        return ERR_PARA_ERR;
    }

    for (i = 0; i < GPIO_SWD_PIN_NUM_MAX; i++)
    {
        if (u8Pin & (0x01 << i))
        {
            u32Cfg = pstGPIO->GPIO_SWD_CFG[i];

            u32Cfg &= ~(GPIO_SWD_MODE_MSK << GPIO_SWD_MODE_SHIFT);
            u32Cfg |= (enMode & GPIO_SWD_MODE_MSK) << GPIO_SWD_MODE_SHIFT;

            pstGPIO->GPIO_SWD_CFG[i] = u32Cfg;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Set indicated pin(swdio or swclk) output status.
 * @param  pstGPIO: GPIO peripheral handle, Should be SWD_M0P / SWD_M33.
 * @param  u8Pin: The pins which would be set or clear, @ref EN_GPIO_SW_NUM_T.
 * @param  enLevel: Output level status, @ref EN_GPIO_LEVEL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_sw_output_level(stGPIO_SWD_Handle_t *pstGPIO, uint8_t u8Pin, EN_GPIO_LEVEL_T enLevel)
{
    if (!(IS_VALID_GPIO_SWD_HANDLE(pstGPIO)) || (!IS_VALID_GPIO_LEVEL(enLevel)))
    {
        return ERR_PARA_ERR;
    }

    if (GPIO_LEVEL_LO == enLevel)
    {
        pstGPIO->GPIO_SWD_OUT_CLR = u8Pin;
    }
    else
    {
        pstGPIO->GPIO_SWD_OUT_SET = u8Pin;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Get indicated pin(swdio or swclk) input status.
 * @param  pstGPIO: GPIO peripheral handle, Should be SWD_M0P / SWD_M33.
 * @param  u8Pin: The pins which would be get, @ref EN_GPIO_SW_NUM_T.
 * @param  pu8Level: Pin level, @ref EN_GPIO_LEVEL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_get_sw_input_level(stGPIO_SWD_Handle_t *pstGPIO, uint8_t u8Pin, uint8_t *pu8Level)
{
    if (!(IS_VALID_GPIO_SWD_HANDLE(pstGPIO)) || (NULL == pu8Level))
    {
        return ERR_PARA_ERR;
    }

    *pu8Level = pstGPIO->GPIO_SWD_IN_STATUS & u8Pin;

    return ERR_STA_OK;
}

/**
 * @brief  Config indicated pin(swdio or swclk) pull up or pull down.
 * @param  pstGPIO: GPIO peripheral handle, Should be SWD_M0P / SWD_M33.
 * @param  u8Pin: The pins which would be config, @ref EN_GPIO_SW_NUM_T.
 * @param  enMode: Pin pull mode, @ref EN_GPIO_PULL_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_sw_pull_mode(stGPIO_SWD_Handle_t *pstGPIO, uint8_t u8Pin, EN_GPIO_PULL_MODE_T enMode)
{
    uint8_t i;
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_GPIO_SWD_HANDLE(pstGPIO)) || (!IS_VALID_GPIO_PULL_MODE(enMode)))
    {
        return ERR_PARA_ERR;
    }

    for (i = 0; i < GPIO_SWD_PIN_NUM_MAX; i++)
    {
        if (u8Pin & (0x01 << i))
        {
            u32Cfg = pstGPIO->GPIO_SWD_CFG[i];
            u32Cfg &= ~(GPIO_SWD_PULL_MSK << GPIO_SWD_PULL_SHIFT);
            u32Cfg |= (enMode & GPIO_SWD_PULL_MSK) << GPIO_SWD_PULL_SHIFT;
            pstGPIO->GPIO_SWD_CFG[i] = u32Cfg;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Config indicated pin(swdio or swclk) drive strength.
 * @param  pstGPIO: GPIO peripheral handle, Should be SWD_M0P / SWD_M33.
 * @param  enPin: The pins which would be config, @ref EN_GPIO_SW_NUM_T.
 * @param  enStrength: Pin drive strength, @ref EN_GPIO_DRV_STRENGTH_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_sw_drive_strength(stGPIO_SWD_Handle_t *pstGPIO, uint8_t u8Pin,
                                               EN_GPIO_DRV_STRENGTH_T enStrength)
{
    uint8_t i;
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_GPIO_SWD_HANDLE(pstGPIO))) || (!IS_GPIO_DRV_STRENGTH_VALID(enStrength)))
    {
        return ERR_PARA_ERR;
    }

    for (i = 0; i < GPIO_SWD_PIN_NUM_MAX; i++)
    {
        if (u8Pin & (0x01 << i))
        {
            u32Cfg = pstGPIO->GPIO_SWD_CFG[i];

            u32Cfg &= ~(GPIO_SWD_DRV_STRENGTH_MSK << GPIO_SWD_DRV_STRENGTH_SHIFT);
            u32Cfg |= (enStrength & GPIO_SWD_DRV_STRENGTH_MSK) << GPIO_SWD_DRV_STRENGTH_SHIFT;

            pstGPIO->GPIO_SWD_CFG[i] = u32Cfg;
        }
    }

    return ERR_STA_OK;
}




/**********************************************************************************************************************
 *                                             GPIO IR Function
 *********************************************************************************************************************/

/**
 * @brief  Enable ir tx output.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_enable_ir_tx_out(void)
{
    IOMUX->IOMUX_IR_CTRL |= (IR_TX_OUT_EN_MSK << IR_TX_OUT_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Disable ir tx output.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_disable_ir_tx_out(void)
{
    IOMUX->IOMUX_IR_CTRL &= ~(IR_TX_OUT_EN_MSK << IR_TX_OUT_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Enable ir rx amp.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_enable_ir_rx_amp(void)
{
    IOMUX->IOMUX_IR_CTRL |= (IR_RX_AMP_EN_MSK << IR_RX_AMP_EN_SHTFT);

    return ERR_STA_OK;
}

/**
 * @brief  Disable ir rx amp.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_disable_ir_rx_amp(void)
{
    IOMUX->IOMUX_IR_CTRL &= ~(IR_RX_AMP_EN_MSK << IR_RX_AMP_EN_SHTFT);

    return ERR_STA_OK;
}

/**
 * @brief  Set resistance of R in RC filter.
 * @param  u8Rtune: R = 50K*2^(u8Rtune).
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_set_ir_rx_rtune(uint8_t u8Rtune)
{
    uint32_t u32Cfg = 0;

    if (u8Rtune > IR_RX_RTUNE_MSK)
    {
        return ERR_STA_ERROR;
    }

    u32Cfg = IOMUX->IOMUX_IR_CTRL;

    u32Cfg &= ~(IR_RX_RTUNE_MSK << IR_RX_RTUNE_SHTFT);
    u32Cfg |= ((u8Rtune & IR_RX_RTUNE_MSK) << IR_RX_RTUNE_SHTFT);

    IOMUX->IOMUX_IR_CTRL = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Get ir control register vlaue.
 * @param  pu32IrCtrlValue: Pointer to ir ctrl reg vlaue.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_get_ir_ctrl_value(uint32_t *pu32IrCtrlValue)
{
    if (NULL == pu32IrCtrlValue)
    {
        return ERR_PARA_ERR;
    }

    *pu32IrCtrlValue = IOMUX->IOMUX_IR_CTRL;

    return ERR_STA_OK;
}




/**********************************************************************************************************************
 *                                             GPIO Others Function
 *********************************************************************************************************************/
 
/**
 * @brief  Enable ddcxo32k clock output from PB07 or(and) PB10.
 * @param  u8Io: output io, @ref EN_DCXO32K_OUT_CFG_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_enable_dcxo32k_output(uint8_t u8Io)
{
    if (!IS_VALID_DCXO32K_OUT_CFG(u8Io))
    {
        return ERR_PARA_ERR;
    }

    GPIO_DBG->GPIO_DCXO_LCLK_OUT_CFG |= (u8Io & GPIO_DCXO32K_OUT_CFG_MSK) << GPIO_DCXO32K_OUT_CFG_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable ddcxo32k clock output from PB07 or(and) PB10.
 * @param  u8Io: output io, @ref EN_DCXO32K_OUT_CFG_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_gpio_disable_dcxo32k_output(uint8_t u8Io)
{
    if (!IS_VALID_DCXO32K_OUT_CFG(u8Io))
    {
        return ERR_PARA_ERR;
    }

    GPIO_DBG->GPIO_DCXO_LCLK_OUT_CFG &= ~((u8Io & GPIO_DCXO32K_OUT_CFG_MSK) << GPIO_DCXO32K_OUT_CFG_SHIFT);

    return ERR_STA_OK;
}

