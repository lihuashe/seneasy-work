/**
 * @file     flash_prog.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2020/01/03
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "sflash.h"
#include "hw_crg.h"
#include "hw_pmu.h"
#include "hw_efuse.h"
#include "hw_gpio.h"
#include "hw_flash.h"
#include "hw_sys_ctrl.h"
#include "hw_uart.h"
#include "hw_wdt.h"
#include "hal_wdt.h"

#include "app_cfg.h"
#include "boards.h"
#include "debug.h"

#include "FlashOS.H"
#include "flash_prog.h"

/**
 * @brief  Local variables.
 */
static uint32_t m_u32FlashSectorEraseTime = P25Q16SU_SECTOR_ERASE_TIME_US;
static uint32_t m_u32FlashChipEraseTime   = P25Q16SU_CHIP_ERASE_TIME_US;
static uint32_t m_u32FlashProgramTime     = P25Q16SU_PROGRAM_TIME_US;

/**
 * @brief  Initialize Flash Programming Functions.
 * @param  u32Addr: Device Base Address.
 * @param  u32Clk: Clock Frequency (Hz).
 * @param  enCode: Function Code(1: Erase, 2: Program, 3: Verify).
 * @return 0: OK, 1: Failed.
 */
static void flash_init(void)
{
    uint8_t                u8ChipDef = 0;
    uint32_t               u32ID;
    EN_CHIP_DEF_MEM_TYPE_T enMemType;

    rom_hw_efuse_read_bytes(EFUSE_CHIP_DEFINE_ADDR, &u8ChipDef, sizeof(u8ChipDef));

    enMemType = (u8ChipDef >> EFUSE_CHIP_DEF_MEM_TYPE_SHIFT) & EFUSE_CHIP_DEF_MEM_TYPE_MSK;

    rom_hw_flash_init(FLASH, 3);

    if (EX_MEM_XIP_FLASH_FIXED_IO == enMemType)
    {
        rom_hw_gpio_enable_qspi_pid(QSPI0_FIXED_GPIO_EN);
    }
    else if (EX_MEM_XIP_FLASH_ANY_IO == enMemType)
    {
        uint8_t io[6];
        uint8_t ioMask = EFUSE_FLASH_RESELECT_IO_MSK;

        rom_hw_efuse_read_bytes(EFUSE_FLASH_RESELECT_IO_ADDR, io, sizeof(io));
        PRINTF("io: %02X %02X %02X %02X %02X %02X\n", io[0], io[1], io[2], io[3], io[4], io[5]);

        rom_hw_gpio_set_pin_pid(GPION((io[0] & ioMask)), GPIO_PIN_N((io[0] & ioMask)), PID_QSPI0_CLK);
        rom_hw_gpio_set_pin_pid(GPION((io[1] & ioMask)), GPIO_PIN_N((io[1] & ioMask)), PID_QSPI0_CS);
        rom_hw_gpio_set_pin_pid(GPION((io[2] & ioMask)), GPIO_PIN_N((io[2] & ioMask)), PID_QSPI0_SI);
        rom_hw_gpio_set_pin_pid(GPION((io[3] & ioMask)), GPIO_PIN_N((io[3] & ioMask)), PID_QSPI0_SO);
        rom_hw_gpio_set_pin_pid(GPION((io[4] & ioMask)), GPIO_PIN_N((io[4] & ioMask)), PID_QSPI0_WP);
        rom_hw_gpio_set_pin_pid(GPION((io[5] & ioMask)), GPIO_PIN_N((io[5] & ioMask)), PID_QSPI0_HOLD);
    }

    rom_hw_flash_exit_abnormal_state(FLASH);
    rom_hw_flash_get_id(FLASH, &u32ID);
    PRINTF("Flash id: %08X\n", u32ID);

    if (P25Q40SU_CHIP_ID == u32ID)
    {
        m_u32FlashProgramTime     = P25Q40SU_PROGRAM_TIME_US;
        m_u32FlashSectorEraseTime = P25Q40SU_SECTOR_ERASE_TIME_US;
        m_u32FlashChipEraseTime   = P25Q40SU_CHIP_ERASE_TIME_US;
    }
    else if (P25Q80SU_CHIP_ID == u32ID)
    {
        m_u32FlashProgramTime     = P25Q80SU_PROGRAM_TIME_US;
        m_u32FlashSectorEraseTime = P25Q80SU_SECTOR_ERASE_TIME_US;
        m_u32FlashChipEraseTime   = P25Q80SU_CHIP_ERASE_TIME_US;
    }
    else
    {
        m_u32FlashProgramTime     = P25Q16SU_PROGRAM_TIME_US;
        m_u32FlashSectorEraseTime = P25Q16SU_SECTOR_ERASE_TIME_US;
        m_u32FlashChipEraseTime   = P25Q16SU_CHIP_ERASE_TIME_US;
    }

    PRINTF("Flash time: %d, %d, %d\n", m_u32FlashProgramTime, m_u32FlashSectorEraseTime, m_u32FlashChipEraseTime);
}

/**
 * @brief  Initialize Flash Programming Functions.
 * @param  u32Addr: Device Base Address.
 * @param  u32Clk: Clock Frequency (Hz).
 * @param  enCode: Function Code(1: Erase, 2: Program, 3: Verify).
 * @return 0: OK, 1: Failed.
 */
int Init(unsigned long u32Addr, unsigned long u32Clk, unsigned long enCode)
{
    int      retCode = 0;
    uint32_t u32Val  = 0;

    // Disable wdt.
    rom_hw_wdt_disable(WDT0);

    rom_hw_sys_ctrl_read_com_reg(SYS_CTRL_COM_REG_INDEX, &u32Val);
    if (SYS_CTRL_COM_REG_VAL != u32Val)
    {
        rom_hw_sys_ctrl_write_com_reg(SYS_CTRL_COM_REG_INDEX, SYS_CTRL_COM_REG_VAL);

#if APP_DEBUG_ENABLED
        rom_hw_pmu_set_rc_hclk_tune(0x56);
        app_debug_init(GPIO_PORT_APP_DEBUG_TX, GPIO_PIN_APP_DEBUG_TX, UART_HANDLE_APP_DEBUG, UART_BPS_APP_DEBUG, NULL);
#endif

        flash_init();
    }

    switch (enCode)
    {
        case FUNC_CODE_ERASE:
        {
            PRINTF("Init case 1. Start to erase.\n");
            break;
        }
        case FUNC_CODE_PROGM:
        {
            PRINTF("Init case 2. Start to program.\n");
            break;
        }
        case FUNC_CODE_VERFIY:
        {
            PRINTF("Init case 3. Start to verify.\r\n");
            break;
        }
        default:
        {
            PRINTF("Init case default.\r\n");
            break;
        }
    }

    return retCode; // Finished without Errors
}

/**
 * @brief  Deinitialize Flash Programming Functions.
 * @param  enCode: Function Code(1: Erase, 2: Program, 3: Verify).
 * @return 0: OK, 1: Failed.
 */
int UnInit(unsigned long enCode)
{
    switch (enCode)
    {
        case FUNC_CODE_ERASE:
        {
            PRINTF("UnInit case 1. Erase done.\r\n\n");
            break;
        }
        case FUNC_CODE_PROGM:
        {
            PRINTF("UnInit case 2. Program done.\r\n\n");
            break;
        }
        case FUNC_CODE_VERFIY:
        {
            PRINTF("UnInit case 3. Verify done.\r\n\n");
            break;
        }
        default:
        {
            PRINTF("UnInit 4. Default.\r\n");
            break;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Erase complete Flash Memory.
 * @return 0: OK, 1: Failed.
 */
int EraseChip(void)
{
    EN_ERR_STA_T retCode = 0;

    retCode = rom_hw_flash_chip_erase(FLASH, m_u32FlashChipEraseTime);
    return (int)retCode;
}

/**
 * @brief  Erase Sector in Flash Memory.
 * @param  u32Addr: Sector Address.
 * @return 0: OK, 1: Failed.
 */
int EraseSector(unsigned long u32Addr)
{
    uint32_t u32AddrTmp   = u32Addr;
    uint32_t u32AdrOffset = 0;

    PRINTF("Erase sector. Addr:0x%08X, ", u32AddrTmp);

    // App Code. Form .bin 0x10000000 ~ 0x1007FFFF write to chip 0x00000000 ~ 0x0007FFFF
    if ((u32AddrTmp >= FLASH_BASE_ADDR) && (u32AddrTmp < (FLASH_BASE_ADDR + FLASH_SIZE_MAX)))
    {
        u32AdrOffset = u32AddrTmp - FLASH_BASE_ADDR;

        // if(u32AdrOffset % FLASH_BYTES_PER_SECTOR)
        if (ERR_STA_OK
            != rom_hw_flash_erase(FLASH, ERASE_SECTOR, u32AdrOffset >> FLASH_BYTES_PER_SECTOR_SHIFT_BITS,
                                  m_u32FlashSectorEraseTime))
        {
            return ERR_STA_ERROR;
        }
    }
    else
    {
        PRINTF("Erase sector. Addr is illegal.");
        return ERR_STA_ERROR;
    }

    PRINTF("Offset addr:0x%08X.\r\n", u32AdrOffset);

    return ERR_STA_OK;
}

/**
 * @brief  Program Page in Flash Memory.
 * @param  u32Addr: Page Start Address.
 * @param  u32Size: Page Size.
 * @param  pBuf: Page Data.
 * @return 0: OK, 1: Failed.
 */
int ProgramPage(unsigned long u32Addr, unsigned long u32Size, unsigned char *pBuf)
{
    uint32_t u32SizeTmp = u32Size;
    uint32_t u32AddrTmp = u32Addr;

    uint16_t u16Len    = 0;
    uint32_t u32Offset = 0;

    PRINTF("Program page. Addr:0x%08X, Size:0x%08X, ", u32AddrTmp, u32SizeTmp);

    // App Code. Form .bin 0x10000000 ~ 0x1007FFFF write to chip 0x00000000 ~ 0x0007FFFF
    if ((u32AddrTmp >= FLASH_BASE_ADDR) && ((u32AddrTmp + u32SizeTmp) <= (FLASH_BASE_ADDR + FLASH_SIZE_MAX)))
    {
        u16Len    = (uint16_t)(u32SizeTmp & 0xFFFF);
        u32Offset = u32AddrTmp - FLASH_BASE_ADDR;

        if (ERR_STA_OK
            != rom_hw_flash_write_bytes(FLASH, u32Offset, pBuf, u16Len, BUS_MODE_SPI, m_u32FlashProgramTime))
        {
            return ERR_STA_ERROR;
        }
    }
    else
    {
        PRINTF("Program page. Addr is illegal.");
        return ERR_STA_ERROR;
    }

    PRINTF("Offset addr:0x%08X.\r\n", u32Offset);

    return ERR_STA_OK;
}

/**
 * @brief  Verify function.
 * @param  u32Addr: Start Address.
 * @param  u32Size: Size in bytes.
 * @param  pBuf: Page Data.
 * @return OK: u32Addr + u32Size, Failed: any other number.
 */
unsigned long Verify(unsigned long u32Addr, unsigned long u32Size, unsigned char *pBuf)
{
    uint8_t  pu8Buffer[300];
    uint32_t u32SizeTmp = u32Size;
    uint32_t u32AddrTmp = u32Addr;

    uint32_t u32Offset;
    uint16_t u16Len;

    PRINTF("Verify page. Addr:0x%08X, Size:0x%08X, ", u32AddrTmp, u32SizeTmp);

    // App Code. Form .bin 0x10000000 ~ 0x1007FFFF write to chip 0x00000000 ~ 0x0007FFFF
    if ((u32AddrTmp >= FLASH_BASE_ADDR) && ((u32AddrTmp + u32SizeTmp) <= (FLASH_BASE_ADDR + FLASH_SIZE_MAX)))
    {
        u16Len    = (uint16_t)(u32SizeTmp & 0xFFFF);
        u32Offset = u32AddrTmp - FLASH_BASE_ADDR;

        if (ERR_STA_OK != rom_hw_flash_read_bytes(FLASH, u32Offset, pu8Buffer, u16Len, BUS_MODE_SPI))
        {
            return u32Addr;
        }

        if (memcmp(pBuf, pu8Buffer, u16Len))
        {
            PRINTF("Offset addr:0x%08X.\r\n", u32Offset);
            PRINTF("Verify page failure.\r\n");

#if APP_DEBUG_ENABLED
            {
                uint16_t i;

                PRINTF("Flash file:");
                for (i = 0; i < u16Len; i++)
                {
                    PRINTF("%02X ", pu8Buffer[i]);
                }

                PRINTF("\nKeil  file:");
                for (i = 0; i < u16Len; i++)
                {
                    PRINTF("%02X ", pBuf[i]);
                }
            }
#endif

            return u32Addr;
        }
    }
    else
    {
        PRINTF("Verify. Addr is illegal.\n");
        return u32Addr;
    }

    PRINTF("Offset addr:0x%08X.\r\n", u32Offset);

    return (u32Addr + u32Size);
}
