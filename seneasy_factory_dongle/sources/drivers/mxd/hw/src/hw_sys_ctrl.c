/**********************************************************************************************************************
 * @file     hw_sys_ctrl.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/01/17
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
#include "err_def.h"

#include "type_def.h"


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define KBYTE (1024)
#define CP_STARTUP_CODE_SIZE_MAX (1024) // Limited by HW design, DO NOT MODIFY!!!




/**
 * @brief  Enable cp(cm0p) remap function.
 * @param  u32Addr: The real address of cp in the bus. Align to KB.
 * @param  u16Size: The size of remap code. Align to 4 bytes.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_enable_cp_remap(uint32_t u32Addr, uint16_t u16Size)
{
    uint16_t addr;
    uint16_t size;

    if ((0 != (u32Addr % KBYTE)) || (0 != (u16Size % 4)))
    {
        return ERR_PARA_ERR;
    }

    if ((0 == u16Size) || (u16Size > CP_STARTUP_CODE_SIZE_MAX))
    {
        return ERR_PARA_ERR;
    }

    addr = u32Addr / KBYTE;
    size = (u16Size / 4) - 1;

    SYS_CTRL->SYS_CTRL_CP_REMAP_CFG = ((CP_MEM_REMAP_EN_MSK << CP_MEM_REMAP_EN_SHIFT) |
                                       ((size & CP_MEM_RANG_LOWER_MSK) << CP_MEM_RANG_LOWER_SHIFT) |
                                       ((addr & CP_MEM_SIZE_MSK) << CP_MEM_SIZE_SHIFT));

    return ERR_STA_OK;
}

/**
 * @brief  Disable cp remap function
 */
void rom_hw_sys_ctrl_disable_cp_remap(void)
{
    SYS_CTRL->SYS_CTRL_CP_REMAP_CFG &= ~(CP_MEM_REMAP_EN_MSK << CP_MEM_REMAP_EN_SHIFT);
}

/**
 * @brief  Write The Common Register which Used for MP and CP.
 * @param  enReg: The common register index, @ref EN_SYS_CTRL_COM_REG_T.
 * @param  u32Val: The Value which will write to the common register.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_write_com_reg(EN_SYS_CTRL_COM_REG_T enReg, uint32_t u32Val)
{
    if (!(IS_VALID_SYS_CTRL_COM_REG(enReg)))
    {
        return ERR_PARA_ERR;
    }

    SYS_CTRL->SYS_CTRL_COM_REG[enReg] = u32Val;

    return ERR_STA_OK;
}

/**
 * @brief  Read The Common Register which Used for MP and CP.
 * @param  enReg: The common register index, @ref EN_SYS_CTRL_COM_REG_T.
 * @param  pu32Val: The Value which read from the common register.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_read_com_reg(EN_SYS_CTRL_COM_REG_T enReg, uint32_t *pu32Val)
{
    if ((!(IS_VALID_SYS_CTRL_COM_REG(enReg))) || (NULL == pu32Val))
    {
        return ERR_PARA_ERR;
    }

    *pu32Val = SYS_CTRL->SYS_CTRL_COM_REG[enReg];

    return ERR_STA_OK;
}

/**
 * @brief  Lock device.
 * @param  u8LockId: Lock id, @ref EN_SYS_CTRL_LOCK_ID_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_lock_device(uint8_t u8LockId)
{
    uint8_t lockIdx;
    uint32_t lockVal;

    if (u8LockId >= SYS_CTRL_LOCK_MAX)
    {
        return ERR_PARA_ERR;
    }

    lockIdx = SYS_CTRL_LOCK_IDX(u8LockId);
    lockVal = ((SYS_CTRL_LOCK_MSK) << (SYS_CTRL_LOCK_SHIFT(u8LockId)));

    if (lockVal & (SYS_CTRL->LOCK[lockIdx].SYS_CTRL_LOCK_STA))
    {
        return ERR_STA_BUSY; /* Device already locked */
    }

    SYS_CTRL->LOCK[lockIdx].SYS_CTRL_LOCK_SET = lockVal;

    if (!(lockVal & (SYS_CTRL->LOCK[lockIdx].SYS_CTRL_LOCK_STA)))
    {
        return ERR_STA_ERROR; /* Lock device fail */
    }

    return ERR_STA_OK;
}

/**
 * @brief  Release device lock.
 * @param  u8LockId: Lock id, @ref EN_SYS_CTRL_LOCK_ID_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_release_device(uint8_t u8LockId)
{
    uint8_t lockIdx;
    uint32_t lockVal;

    if (u8LockId >= SYS_CTRL_LOCK_MAX)
    {
        return ERR_PARA_ERR;
    }

    lockIdx = SYS_CTRL_LOCK_IDX(u8LockId);
    lockVal = ((SYS_CTRL_LOCK_MSK) << (SYS_CTRL_LOCK_SHIFT(u8LockId)));

    SYS_CTRL->LOCK[lockIdx].SYS_CTRL_LOCK_CLR = lockVal;

    if (lockVal & (SYS_CTRL->LOCK[lockIdx].SYS_CTRL_LOCK_STA))
    {
        return ERR_STA_ERROR; /* Release lock device fail */
    }

    return ERR_STA_OK;
}

/**
 * @brief  Enable CP or MP indicated Peripheral interrupt.
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u8Irq: Indicate which interrupt will be enable, @ref IRQn_Type.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_enable_peri_int(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t u8Irq)
{
    if ((!(IS_VALID_SYS_CTRL_CORE_SEL(enCore))) || (SYS_CTRL_PERI_INT_MAX <= u8Irq))
    {
        return ERR_PARA_ERR;
    }

    if (SYS_CTRL_CP == enCore)
    {
        SYS_CTRL->SYS_CTRL_CP_PERI_INT_EN |= (0x01 << u8Irq);
    }
    else
    {
        if (SYS_CTRL_MP_PERI_INT0_MAX > u8Irq)
        {
            SYS_CTRL->SYS_CTRL_MP_PERI_INT_EN0 |= (0x01 << u8Irq);
        }
        else
        {
            SYS_CTRL->SYS_CTRL_MP_PERI_INT_EN1 |= (0x01 << (u8Irq - SYS_CTRL_MP_PERI_INT0_MAX));
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Disable CP or MP indicated Peripheral interrupt.
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u8Irq: Indicate which interrupt will be disable, @ref IRQn_Type.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_disable_peri_int(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t u8Irq)
{
    if ((!(IS_VALID_SYS_CTRL_CORE_SEL(enCore))) || (SYS_CTRL_PERI_INT_MAX <= u8Irq))
    {
        return ERR_PARA_ERR;
    }

    if (SYS_CTRL_CP == enCore)
    {
        SYS_CTRL->SYS_CTRL_CP_PERI_INT_EN &= ~(0x01 << u8Irq);
    }
    else
    {
        if (u8Irq < SYS_CTRL_MP_PERI_INT0_MAX)
        {
            SYS_CTRL->SYS_CTRL_MP_PERI_INT_EN0 &= ~(0x01 << u8Irq);
        }
        else
        {
            SYS_CTRL->SYS_CTRL_MP_PERI_INT_EN1 &= ~(0x01 << (u8Irq - SYS_CTRL_MP_PERI_INT0_MAX));
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Enable or disable CP or MP indicated Peripheral interrupt, each bit corresponding a peripheral.
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u32Reg0: Indicate which interrupt will be enable or disable, @ref IRQn_Type.
 * @param  u32Reg1: Indicate which interrupt will be enable or disable, @ref IRQn_Type.
 *         0: Disable xx peripheral interrupt.
 *         1: Enable xx peripheral interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_peri_int_ctrl(EN_SYS_CTRL_CORE_SEL_T enCore, uint32_t u32Reg0, uint32_t u32Reg1)
{
    if (!(IS_VALID_SYS_CTRL_CORE_SEL(enCore)))
    {
        return ERR_PARA_ERR;
    }

    if (SYS_CTRL_CP == enCore)
    {
        SYS_CTRL->SYS_CTRL_CP_PERI_INT_EN = u32Reg0;
    }
    else
    {
        SYS_CTRL->SYS_CTRL_MP_PERI_INT_EN0 = u32Reg0;
        SYS_CTRL->SYS_CTRL_MP_PERI_INT_EN1 = u32Reg1;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Get CP or MP indicated SoftWare interrupt flag(status).
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  pu8Msk: Indicate which interrupt flag will be read.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_get_sw_int_flag(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t *pu8Msk)
{
    if (!(IS_VALID_SYS_CTRL_CORE_SEL(enCore)) || NULL == pu8Msk)
    {
        return ERR_PARA_ERR;
    }

    if (SYS_CTRL_CP == enCore)
    {
        *pu8Msk = SYS_CTRL->SYS_CTRL_CP_SW_INT_FLAG & SYS_CTRL_SW_INT_MASK;
    }
    else
    {
        *pu8Msk = SYS_CTRL->SYS_CTRL_MP_SW_INT_FLAG & SYS_CTRL_SW_INT_MASK;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Clear CP or MP indicated SoftWare interrupt flag(status).
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u8Msk: Indicate which flag will be cleared.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_clear_sw_int_flag(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t u8Msk)
{
    if (!(IS_VALID_SYS_CTRL_CORE_SEL(enCore)) || (u8Msk > SYS_CTRL_SW_INT_MASK))
    {
        return ERR_PARA_ERR;
    }

    if (SYS_CTRL_CP == enCore)
    {
        SYS_CTRL->SYS_CTRL_CP_SW_INT_CLR = u8Msk;
    }
    else
    {
        SYS_CTRL->SYS_CTRL_MP_SW_INT_CLR = u8Msk;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Trigger CP or MP indicated SoftWare interrupt.
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u8IntEn: Indicate which interrupt will be enable,
 *                  bit-1 means enable
 *                  bit-0 means no impact
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_trigger_sw_int(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t u8IntEn)
{
    if (!(IS_VALID_SYS_CTRL_CORE_SEL(enCore)) || (u8IntEn > SYS_CTRL_SW_INT_MASK))
    {
        return ERR_PARA_ERR;
    }

    if (SYS_CTRL_CP == enCore)
    {
        SYS_CTRL->SYS_CTRL_CP_SW_INT_TRIG = u8IntEn;
    }
    else
    {
        SYS_CTRL->SYS_CTRL_MP_SW_INT_TRIG = u8IntEn;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Control gptimer Gtimer start to work or stop working synchronously.
 * @param  u16SyncCtrl: Start or stop timer. @ref EN_SYS_CTRL_TIMER_SYNC_CTRL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_gtim_sync_ctrl(uint16_t u16SyncCtrl)
{
    if (u16SyncCtrl > TIMER_SYNC_CTRL_MASK)
    {
        return ERR_PARA_ERR;
    }

    SYS_CTRL->SYS_CTRL_TIMER_SYNC = u16SyncCtrl & TIMER_SYNC_CTRL_MASK;

    return ERR_STA_OK;
}

/**
 * @brief  Get USB phy interrupt flag.
 * @param  pu32Flag: Interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_get_usb_phy_int_flag(uint32_t *pu32Flag)
{
    if (NULL == pu32Flag)
    {
        return ERR_PARA_ERR;
    }

    *pu32Flag = ((SYS_CTRL->SYS_CTRL_USB_INT_FLAG >> USB_PHY_INT_FALG_SHIFT) & USB_PHY_INT_FALG_MSK);

    return ERR_STA_OK;
}

/**
 * @brief  Clear USB phy interrupt flag.
 * @param  u32Flag: interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_clear_usb_phy_int_flag(uint32_t u32Flag)
{
    SYS_CTRL->SYS_CTRL_USB_INT_CLR = (u32Flag & USB_PHY_INT_CLR_MSK) << USB_PHY_INT_CLR_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Enable USB phy interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_enable_usb_phy_pd_int(void)
{
    SYS_CTRL->SYS_CTRL_USB_INT_EN |= USB_PHY_INT_EN_MSK << USB_PHY_INT_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable USB phy interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_disable_usb_phy_pd_int(void)
{
    SYS_CTRL->SYS_CTRL_USB_INT_EN &= ~(USB_PHY_INT_EN_MSK << USB_PHY_INT_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Get USB phy por event interrupt.
 * @param  pu32Flag: Interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_get_usb_phy_por_event_int_flag(uint32_t *pu32Flag)
{
    if (NULL == pu32Flag)
    {
        return ERR_PARA_ERR;
    }

    *pu32Flag = ((SYS_CTRL->SYS_CTRL_USB_INT_EVENT_FLAG >> USB_PHY_INT_EVENT_SHIFT) & USB_PHY_INT_EVENT_MSK);

    return ERR_STA_OK;
}

/**
 * @brief  Enable wdt1 wakeup interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_enable_wdt_wakeup_int(void)
{
    SYS_CTRL->SYS_CTRL_WDT_WAKEUP_INT_EN |= (WDT_WAKEUP_INT_EN_MSK << WDT_WAKEUP_INT_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Disable wdt1 wakeup interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_disable_wdt_wakeup_int(void)
{
    SYS_CTRL->SYS_CTRL_WDT_WAKEUP_INT_EN &= ~(WDT_WAKEUP_INT_EN_MSK << WDT_WAKEUP_INT_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Get wdt1 wakeup interrupt flag.
 * @param  pu32Flag: Interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_get_wdt_wakeup_int_flag(uint32_t *pu32Flag)
{
    if (NULL == pu32Flag)
    {
        return ERR_PARA_ERR;
    }

    *pu32Flag = (SYS_CTRL->SYS_CTRL_WDT_WAKEUP_INT_FLAG >> WDT_WAKEUP_INT_FLAG_SHIFT) & WDT_WAKEUP_INT_FLAG_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Clear wdt1 wakeup interrupt flag.
 * @param  u32Flag: interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_sys_ctrl_clear_wdt_wakeup_int_flag(uint32_t u32Flag)
{
    SYS_CTRL->SYS_CTRL_WDT_WAKEUP_INT_CLR |= (u32Flag & WDT_WAKEUP_INT_CLR_MSK) << WDT_WAKEUP_INT_CLR_SHIFT;

    return ERR_STA_OK;
}
