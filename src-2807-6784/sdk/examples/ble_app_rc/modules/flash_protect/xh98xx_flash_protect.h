#ifndef ___XH98xx__FLASH__PROTECT__H__
#define ___XH98xx__FLASH__PROTECT__H__
//#include "rwip_config.h" // RW SW configuration

//#include "arch.h"    // architectural platform definitions
#include <stdlib.h>  // standard lib functions
#include <stddef.h>  // standard definitions
#include <stdint.h>  // standard integer definition
#include <stdbool.h> // boolean definition
#include <stdio.h>
//#include "rwip.h" // RW SW initialization
//#include "dbg.h"
#include "peripheral.h"
//#include "sysdump.h"
//#include "app.h"
#include <string.h>
#include "co.h"


/// cmd bytes

/*********************************************************************
 * MACROS
 */
#define SPI_CMD_WREN 0x06u      /* Write enable */
#define SPI_CMD_WRDI 0x04u      /* Write disable */
#define SPI_CMD_RDSR 0x05u      /* Read status register */
#define SPI_CMD_RDSR2 0x35u     /* Read status register (high byte) */
#define SPI_CMD_WRSR 0x01u      /* Write status register */
#define SPI_CMD_READ 0x03u      /* Read data bytes (low frequency) */
#define SPI_CMD_FAST_READ 0x0Bu /* Read data bytes (high frequency) */
#define SPI_CMD_DUAL_READ 0x3Bu /* Dual output fast read: XH98xx doesn't support 0xBB */
#define SPI_CMD_QUAD_READ 0x6Bu /* Quad output fast read: PN25F04C doesn't support 0x6B, but XH98xx doesn't support 0xEB */
#define SPI_CMD_PP 0x02u        /* Page program (up to page in 256 bytes) */
#define SPI_CMD_RDID 0x9fu      /* Read JEDEC ID */
#define SPI_CMD_RDUID 0x4Bu     /* Read UID */

#define SPI_CMD_SE 0x20u     /* Sector erase (usually 4KiB) */
#define SPI_CMD_BE_32K 0x52u /* Erase 32KiB block */
#define SPI_CMD_BE_64K 0xD8u /* Erase 64KiB block */
#define SPI_CMD_CE 0x60u     /* Erase whole flash chip, or 0xC7 */

#define SPI_CMD_ENDP 0xB9u  /* Enter Deep Power-Down */
#define SPI_CMD_EXDP 0xABu  /* Exit Deep Power-Down */
#define SPI_CMD_RDSEC 0x48u /* Read Security Registers */
#define SPI_CMD_ERSEC 0x44u /* Erase Security Registers */
#define SPI_CMD_PPSEC 0x42u /* Program Security Registers */

#define SPI_CMD_EQPI 0x38u /* Enable Quad Peripheral Interface: XH98xx doesn't support */

#define SR_WIP 0x01  /* Write in progress */
#define SR_WEL 0x02  /* Write enable latch */
#define SR_BP0 0x04  /* Block protect 0 */
#define SR_BP1 0x08  /* Block protect 1 */
#define SR_BP2 0x10  /* Block protect 2 */
#define SR_SRP 0x80  /* SR write protect */
#define SR_QE 0x0200 /* Quad enable */

#define id_XH98xx 0x854412
//==============================

typedef enum
{
    // Flash ID 0x856015
    // WPS=0,CMP bit = 1
    // BP4 BP3 BP2 BP1 BP0
    _16Mbit_0to31_00000 = 0x00,                 // 000000H-1FFFFFH 2MB ALL
    _16Mbit_0to30_00001 = 0x01,                 // 000000H-1EFFFFH 1984KB Lower 31/32
    _16Mbit_0to29_00010 = 0x02,                 // 000000H-1DFFFFH 1920KB Lower 15/16
    _16Mbit_0to27_00011 = 0x03,                 // 000000H-1BFFFFH 1792KB Lower 7/8
    _16Mbit_0to23_00100 = 0x04,                 // 000000H-17FFFFH 1536KB Lower 3/4
    _16Mbit_0to15_00101 = 0x05,                 // 000000H-0FFFFFH 1MB Lower 1/2
    _16Mbit_1to31_01001 = 0x09,                 // 010000H-1FFFFFH 1984KB Upper 31/32
    _16Mbit_2to31_01010 = 0x0a,                 // 020000H-1FFFFFH 1920KB Upper 15/16
    _16Mbit_4to31_01011 = 0x0b,                 // 040000H-1FFFFFH 1792KB Upper 7/8
    _16Mbit_8to31_01100 = 0x0c,                 // 080000H-1FFFFFH 1536KB Upper 3/4
    _16Mbit_16to31_01101 = 0x0d,                // 100000H-1FFFFFH 1MB Upper 1/2
    _16Mbit_0to00_01110 = 0x0e,                 // NONE NONE NONE
    _16Mbit_All_Protect = _16Mbit_0to31_00000,  // Write Protect All
    _16Mbit_NONE_Protect = _16Mbit_0to00_01110, // Write Protect None

} protect_16Mbit_block_t;

typedef enum
{
    // BP4 BP3 BP2 BP1 BP0
    _8Mbit_0to15_00000 = 0x00,                // Write Protect for entire chips
    _8Mbit_0to14_00001 = 0x01,                // Lower_15of16, Write Protect 000000H_0EFFFFH_960KB
    _8Mbit_0to13_00010 = 0x02,                // Lower_7of8, Write Protect 000000H_0DFFFFH_896KB
    _8Mbit_0to11_00011 = 0x03,                // Lower_3of4, Write Protect 000000H_0BFFFFH_768KB
    _8Mbit_0to07_00100 = 0x04,                // Lower_1of2, Write Protect 000000H_07FFFFH_512KB
    _8Mbit_1to15_01001 = 0x09,                // Upper_15of16, Write Protect 03200H_0FFFFFH_960KB
    _8Mbit_2to15_01010 = 0x0A,                // Upper_7of8, Write Protect 020000H_0FFFFFH_896KB
    _8Mbit_4to15_01011 = 0x0B,                // Upper_3of4, Write Protect 040000H_0FFFFFH_768KB
    _8Mbit_8to15_01100 = 0x0C,                // Upper_1of2, Write Protect 080000H_0FFFFFH_512KB
    _8Mbit_0to00_01101 = 0x0D,                // Write Protect None
    _8Mbit_All_Protect = _8Mbit_0to15_00000,  // Write Protect All
    _8Mbit_NONE_Protect = _8Mbit_0to00_01101, // Write Protect None
} protect_8Mbit_block_t;


typedef enum
{
    // Flash ID 0x856013
    // WPS=0,CMP bit = 1
    // BP4 BP3 BP2 BP1 BP0
    // BP4 BP3 BP2 BP1 BP0 Blocks Addresses Density Portion
    _4Mbit_0to7_00000 = 0x00,                // 000000H-07FFFFH 512KB ALL
    _4Mbit_0to6_00001 = 0x01,                // 000000H-06FFFFH 448KB Lower 7/8
    _4Mbit_0to5_00010 = 0x02,                // 000000H-05FFFFH 384KB Lower 3/4
    _4Mbit_0to3_00011 = 0x03,                // 000000H-03FFFFH 256KB Lower 1/2
    _4Mbit_1to7_01001 = 0x09,                // 010000H-07FFFFH 448KB Upper 7/8
    _4Mbit_2to7_01010 = 0x0a,                // 020000H-07FFFFH 384KB Upper 3/4
    _4Mbit_4to7_01011 = 0x0b,                // 040000H-07FFFFH 256KB Upper 1/2
    _4Mbit_0to0_01100 = 0x0c,                // NONE NONE NONE NONE
    _4Mbit_All_Protect = _4Mbit_0to7_00000,  // Write Protect All
    _4Mbit_NONE_Protect = _4Mbit_0to0_01100, // Write Protect None
} protect_4Mbit_block_t;
typedef enum
{
    // Flash ID 0x856012
    // CMP bit = 1
    // BP4 BP3 BP2 BP1 BP0 Blocks Addresses Density Portion
    _2Mbit_0to3_00000 = 0x00,                // 000000H-03FFFFH 256KB ALL
    _2Mbit_0to2_00001 = 0x01,                // 000000H-02FFFFH 192KB Lower 3/4
    _2Mbit_0to1_00010 = 0x02,                // 000000H-01FFFFH 128KB Lower 1/2
    _2Mbit_1to3_01001 = 0x09,                // 010000H-03FFFFH 192KB Upper 3/4
    _2Mbit_2to3_01010 = 0x0a,                // 020000H-03FFFFH 128KB Upper 1/2
    _2Mbit_0to0_01011 = 0x0b,                // NONE NONE NONE
    _2Mbit_All_Protect = _2Mbit_0to3_00000,  // Write Protect All
    _2Mbit_NONE_Protect = _2Mbit_0to0_01011, // Write Protect None
} protect_2Mbit_block_t;

typedef enum
{
    // Flash ID 0x854412  
    // BP4 BP3 BP2 BP1 BP0 Blocks Addresses Density Portion
    _2Mbit_0to3_00011_854412 = 0x03,                // 000000H-03FFFFH 256KB ALL
    _2Mbit_0to2_01001_854412 = 0x0a,                // 000000H-02FFFFH 128KB Lower 3/4
    _2Mbit_0to1_01001_854412 = 0x09,                // 000000H-01FFFFH 64KB Lower 1/2
    _2Mbit_1to3_00001_854412 = 0x01,                // 010000H-03FFFFH 64KB Upper 3/4
    _2Mbit_2to3_00010_854412 = 0x02,                // 020000H-03FFFFH 128KB Upper 1/2
    _2Mbit_0to0_00000_854412 = 0x00,                // NONE NONE NONE
    _2Mbit_All_Protect_854412 = _2Mbit_0to3_00011_854412,  // Write Protect All
    _2Mbit_NONE_Protect_854412 = _2Mbit_0to0_00000_854412, // Write Protect None
} protect_2Mbit_block_t_854412;


uint8_t xflash_read_sr(XH_SF_Type *sf, uint32_t cs);
uint8_t xflash_read_sr2(XH_SF_Type *sf, uint32_t cs);
void sf_Protect_all(XH_SF_Type *sf, uint32_t cs);
int sf_lock_blocks(XH_SF_Type *sf, uint32_t cs, uint8_t Protect_blocks);
void app_flash_protect_on(void);
void app_flash_protect_off(void);
#endif