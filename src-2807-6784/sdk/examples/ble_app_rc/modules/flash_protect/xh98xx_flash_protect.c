#include <stdlib.h>  // standard lib functions
#include <stddef.h>  // standard definitions
#include <stdint.h>  // standard integer definition
#include <stdbool.h> // boolean definition
#include <stdio.h>
#include "peripheral.h"
#include <string.h>
#include "co.h"
#include "XH98xx_flash_protect.h"

extern void sfb_read_nodma(XH_SF_Type *sf, uint32_t cs, sfb_rw_params_t *param);
extern XH_SF_Type *sfb_critical_object_get(void);
extern void sf_wait_sr_no_busy(XH_SF_Type *sf, uint32_t cs);
extern XH_SF_Type *sfb_critical_object_get(void);
extern void sf_write_enable(XH_SF_Type *sf, uint32_t cs);
static volatile uint32_t flash_id;
#define inside_FLASH_SF  XH_SF
#define inside_FLASH_CS  0

#define SF_WRITE_BEGIN(sf, cs)                          \
    do                                                  \
    {                                                   \
        uint32_t irq_save = 0;                          \
        if (sfb_critical_object_get() == sf)            \
            CO_DISABLE_IRQ_EX_EXCEPT_HIGHEST(irq_save); \
        sf_write_enable(sf, cs);
#define SF_WRITE_END(sf, cs)                        \
    sf_wait_sr_no_busy(sf, cs);                     \
    if (sfb_critical_object_get() == sf)            \
        CO_RESTORE_IRQ_EX_EXCEPT_HIGHEST(irq_save); \
    }                                               \
    while (0)

#define SF_RW_PARAM_CMD_DECLARE(name, cmd, data, len) \
    sfb_rw_params_t name = {{{((cmd) << 24), 0}}, 8, (data), (len)}
#define SF_RW_PARAM_CMD_ADDR_DECLARE(name, cmd, addr, data, len) \
    sfb_rw_params_t name = {{{((cmd) << 24) | (addr), 0}}, 32, (data), (len)}
#define SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(name, cmd, addr, data, len) \
    sfb_rw_params_t name = {{{((cmd) << 24) | (addr), 0}}, 40, (data), (len)}

// xFlash Control
uint8_t xflash_read_sr(XH_SF_Type *sf, uint32_t cs)
{
    uint8_t sr = 0;
    SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_RDSR, &sr, 1);
    sfb_read_nodma(sf, cs, &param);
    return sr;
}

uint8_t xflash_read_sr2(XH_SF_Type *sf, uint32_t cs)
{
    uint8_t sr2 = 0;
    SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_RDSR2, &sr2, 1);
    sfb_read_nodma(sf, cs, &param);
    if (sr2 == 0xFF)
        sr2 = 0;
    return sr2;
}

/**
 * @brief write protect for entire chip
 *
 * @param sf
 * @param cs
 * @return int
 */
void sf_Protect_all(XH_SF_Type *sf, uint32_t cs)
{
          
    sf_lock_blocks(XH_SF, 0, _2Mbit_0to3_00000);
}

/**
 * @brief  if Write Protect enable, Keil/Jlink  cann't unlock it .
 * should use isp tools unlock it first before Use jlink debug or download
 *
 * @param sf
 * @param cs
 * @param xblock  refer to protect_block_t
 * @return int
 */
int sf_lock_blocks(XH_SF_Type *sf, uint32_t cs, uint8_t Protect_blocks)
{
    // Status Register1: SRP0 BP4 BP3 BP2 BP1 BP0  WEL WIP
    // Status Register2: SUS1 CMP LB3 LB2 LB1 SUS2 QE  SRP1
    uint16_t sr16bit;
    uint8_t sr1;
    uint8_t sr2;
    uint8_t BP_5bit;
    uint8_t cmp_bit;
    uint8_t SPR0 = 0;
    uint8_t SPR1 = 0;
    uint8_t LB_BIT = 0;

    
    flash_id = sf_read_id(sf, cs);
    //log_debug("\n flash_id=0X%08X", flash_id);
    if ((flash_id == 0x856013)    // for PUYA
        || (flash_id == 0x856014) // for TongFang
        || (flash_id == 0x856015) 
        || (flash_id == 0xeb6014) 
		  
    )
    {
        // sr1=0x34,sr2=0x40
        cmp_bit = 1;
        BP_5bit = Protect_blocks;
        //---------------------
        sr1 = xflash_read_sr(sf, cs);
        sr2 = xflash_read_sr2(sf, cs);

        //printf("\n sr1=0x%02x,sr2=0x%02x\n", sr1, sr2);
        //---------------------
        sr1 &= ~0xfc; // clear bp_5bit
        sr1 |= (SPR0 << 7) | (BP_5bit << 2);
        //---------------------
        sr2 &= ~0x41; // clear CMP & SRP1
        sr2 |= (cmp_bit << 6) | (SPR1);
        //---------------------
        //printf("\n ->sr1=0x%02x,sr2=0x%02x\n", sr1, sr2);
        sr16bit = (uint16_t)(sr2 << 8) | (sr1);
        sf_write_sr_16bits(sf, cs, sr16bit);

        sr1 = xflash_read_sr(sf, cs);
        sr2 = xflash_read_sr2(sf, cs);
        //printf("\n->sr1=0x%02x,sr2=0x%02x\n", sr1, sr2);
    }
		else if( flash_id == 0x854412)//no sr2 
		{
					uint8_t sr8bit;
				 BP_5bit = Protect_blocks;
        //---------------------
        sr1 = xflash_read_sr(sf, cs);
       // sr2 = xflash_read_sr2(sf, cs);

        //log_debug("\n sr1=0x%02x,sr2=0x%02x\n", sr1, sr2);
        //---------------------
        sr1 &= ~0xfc; // clear bp_5bit
        sr1 |= (SPR0 << 7) | (BP_5bit << 2);
        //---------------------
       // sr2 &= ~0x41; // clear CMP & SRP1
        //sr2 |= (cmp_bit << 6) | (SPR1);
        //---------------------
        //log_debug("\n ->sr1=0x%02x,sr2=0x%02x\n", sr1, sr2);
        sr8bit =  (sr1);
        sfs_write_sr(sr8bit);

        sr1 = xflash_read_sr(sf, cs);
       // sr2 = xflash_read_sr2(sf, cs);
        //log_debug("\n->sr1=0x%02x,sr2=0x%02x\n", sr1, sr2);
		}
    return 0;
}

//==============================

#define  TEST_PROTECT 

#ifdef TEST_PROTECT
CO_ALIGN(8)
uint8_t write_buffer[32];
CO_ALIGN(8)
uint8_t read_buffer[32];
CO_ALIGN(8)
uint8_t buffer_FF[32]; // user to verify Erase Data
CO_ALIGN(8)
uint8_t buffer_00[32];

#if 1 
/**
 * @brief read uid
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] data  read uid buffer
 * @param[in] length  length
 **/


void sf_read_uid_ex(XH_SF_Type *sf, uint32_t cs, void *data, uint32_t length)
{
    SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(param, SPI_CMD_RDUID, 0, data, length);
    sfb_read_nodma(sf, cs, &param);
}

#endif
/**芯片flash保护开启*/
void app_flash_protect_on(void)
{
	flash_id = sf_read_id(XH_SF, inside_FLASH_CS);;
	
    if (flash_id == id_XH98xx)
	{
		sf_lock_blocks(XH_SF, 0, _2Mbit_0to2_01001_854412); // Write Protect for All
	}	
}

/**芯片flash保护关闭*/
void app_flash_protect_off(void)
{
    if (flash_id == id_XH98xx)
	{
		sf_lock_blocks(XH_SF, 0, _2Mbit_0to0_00000_854412); // Write Protect for None
	}
}

#endif 
