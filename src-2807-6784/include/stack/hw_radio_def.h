/*****************************************************************************
 *
 * MODULE NAME:    hw_radio_defs.h
 * PROJECT CODE:   XpertBlue
 * DESCRIPTION:    max2829 radio driver for Tabasco.
 * CREATION DATE:  2014.07.14
 *
 *****************************************************************************/
#ifndef HW_RADIO_DEFS
#define HW_RADIO_DEFS
#include "features.h"
#include "hw_hab_defs.h"
#include "hw_register.h"
#if (HW_RADIO == HWradio_MAX2829)

/*****************************************************************************
* PHY Configuration: FunctionalSpec 7.4.1
******************************************************************************/
//#define HWhab_PHY_CFG              0x37016a98
/*reference clock divide by this value to generate the 1MHz clock*/
#define PHY_CFG_REG_REF_CLK_DIV              8     /*[0:4]*/
/*1 means an output, TAB clock comes from PLL, or from RF */
#define PHY_CFG_REG_REF_CLK_DIR              1      /**/
/* 0 the clock is permanently active, not gate */
#define PHY_CFG_REG_PHY_CLK_GATE             0
/* 1: the power down clock will be divide by 10 */
#define PHY_CFG_REG_LPOCLK_DIV               0
/*0: GIO7 output pad configure as open drain*/
#define PHY_CFG_REG_OPEN_DRAIN               0
/*1: reset pad is an output*/
#define PHY_CFG_REG_RST_DIR                  1
/*0:the txdata pad is shared between the tx and rx */
#define PHY_CFG_REG_TXDATA_DIR               0
/*00: normal mode.data taken from jal_txdata  */
#define PHY_CFG_REG_TXDATA_FRC               0
/*11: rxbdw signal is an output */
#define PHY_CFG_REG_RXBDW_DIR                0x03
/*1: the reference clock is inverted */
#define PHY_CFG_REG_REFCLK_INV               0
/*1: RXBDW goes high after detect a sync word */
#define PHY_CFG_REG_SYNC_INV                 1
/*0: the receive data is synchronised to the falling edge of REFCLK*/
#define PHY_CFG_REG_SYNC_RX                  0
/*b'0x:the TXDATA is not synchronised to TRCLK */
#define PHY_CFG_REG_SYNC_TX                  0x00
#define PHY_CFG_REG_TX_INV                   0
#define PHY_CFG_REG_TXDIR_INV                0
#define PHY_CFG_REG_TX_FMT                   0
#define PHY_CFG_REG_RX_FMT                   0
#define PHY_CFG_REG_DAT_BYPASS               1
#define PHY_CFG_REG_OCL_BYPASS               1
#define PHY_CFG_REG_GAU_BYPASS               1
#define PHY_CFG_REG_LPO_BYPASS               1
#define PHY_CFG_REG_RX_INV                   0
#define HWhab_PHY_CFG (		      \
    (PHY_CFG_REG_REF_CLK_DIV  << 0) | \
    (PHY_CFG_REG_REF_CLK_DIR  << 5) | \
    (PHY_CFG_REG_PHY_CLK_GATE << 6) | \
    (PHY_CFG_REG_LPOCLK_DIV   << 7) | \
    (PHY_CFG_REG_OPEN_DRAIN   << 8) | \
    (PHY_CFG_REG_RST_DIR      << 9) | \
    (PHY_CFG_REG_TXDATA_DIR   << 10) | \
    (PHY_CFG_REG_TXDATA_FRC   << 11) | \
    (PHY_CFG_REG_RXBDW_DIR    << 13) | \
    (PHY_CFG_REG_REFCLK_INV   << 15) | \
    (PHY_CFG_REG_SYNC_INV     << 16) | \
    (PHY_CFG_REG_SYNC_RX      << 17) | \
    (PHY_CFG_REG_SYNC_TX      << 18) | \
    (PHY_CFG_REG_TX_INV       << 20) | \
    (PHY_CFG_REG_TXDIR_INV    << 21) | \
    (PHY_CFG_REG_TX_FMT       << 22) | \
    (PHY_CFG_REG_RX_FMT       << 23) | \
    (PHY_CFG_REG_DAT_BYPASS   << 25) | \
    (PHY_CFG_REG_OCL_BYPASS   << 26) | \
    (PHY_CFG_REG_GAU_BYPASS   << 28) | \
    (PHY_CFG_REG_LPO_BYPASS   << 29) | \
    (PHY_CFG_REG_RX_INV       << 31) )
#define PHY_CFG_MAX2829 HWhab_PHY_CFG //0x360062b8 0x360162b8
#define PHY_CFG_XH98xx  HWhab_PHY_CFG
#define PHY_CFG_32768HZ (HWhab_PHY_CFG & (~(1u<<7)))
#define PHY_CFG_32000HZ (HWhab_PHY_CFG |   (1u<<7))

/*****************************************************************************
* Serial Interface CFG: FunctionalSpec 5.9.1
******************************************************************************/
// 14-bit data plus 4-bit address  ==> 0-bit address, 31-bit data (lower 4-bit is address)
// clk pol - 1. data clocked out on rising edge.
// data pol - 0. data not inverted
// serial enable - 1
// clk low - 1. number of refclk cycles for which SPI clk is low.
// clk high -1. number of refclk cycles for which SPI clk is high.
// clk byp - 1. bypass, i.e. without divided
// sel pol - 1. active low select enable.
// set pol - 0. normal SPI mode.
#define SER_CFG_MAX2829 ( (0<<0) | (31<<5) | (1<<10) | (0<<11) | (1<<12) | (1<<16) | (1<<20) | (1<<23) | (1<<24) | (0<<27)) //0x01111640 0x019117e0
// 13-bit address plus 18-bit data
#define SER_CFG_XH98xx  ((13<<0) | (31<<5) | (1<<10) | (0<<11) | (1<<12) | (1<<16) | (1<<20) | (1<<23) | (1<<24) | (0<<27)) //           0x019117ed

/*****************************************************************************
* The definitions for Radio Setup:
******************************************************************************/
#define HW_RADIO_TIME_TX_SLOT     390       /* max radio time tx slot (acl/syn) */
#define HW_RADIO_TIME_RX_SLOT     390       /* max radio time rx slot (acl/syn) */
#define HW_RADIO_TIME_TX_ID_PACKET_SLOT 80  /* radio time tx slot (id) */
#define HW_RADIO_EXTEND_WINDOW        10   /*extend window is 10us */

#define HW_RADIO_INDEPENDENT_TX_RX_SETUP_TIMES 1

//MAX2829 PHY timings in us
#define HW_RADIO_CHANNEL_SWITCH_TIME   25
#define HW_RADIO_STANDBY_TX_TIME       1
#define HW_RADIO_STANDBY_RX_TIME       2
#define HW_RADIO_MAX2829_TIME_T_PA  7       /* txena pull high before PA enable */
#define HW_RADIO_MAX2829_TIME_T_PD  0

#define HW_RADIO_TX_SETUP_TIME     ( HW_RADIO_CHANNEL_SWITCH_TIME + HW_RADIO_STANDBY_TX_TIME + HW_RADIO_MAX2829_TIME_T_PA+60 )      /* tx setup time */
#define HW_RADIO_RX_SETUP_TIME     ( HW_RADIO_CHANNEL_SWITCH_TIME + HW_RADIO_STANDBY_RX_TIME + HW_RADIO_MAX2829_TIME_T_PA+60 )      /* rx setup time */
#define HW_RADIO_TX_HOLD_TIME      HW_RADIO_MAX2829_TIME_T_PD
#define HW_RADIO_RX_HOLD_TIME      HW_RADIO_MAX2829_TIME_T_PD

/*****************************************************************************
* Register bit definitions
******************************************************************************/
#define mBIT(X) (((uint32_t)1) << (X))
/*****************************************************************************
*
* XH98xx REGISTER:
* bit 31 = 0 for SPI block to read
*          1 for SPI block to write
* bit [30:29] = b'00 spi
*               b'01 codec
*               b'10 modem
* bit 28 = 0 read radio
*          1 write radio
* bit [27:19]   - reg address, b'27=1 is alwyas for H'1xx
* bit 18 = 0 low 16-bit
*        = 1 high 16-bit
* bit [17:16]   - reserved
* bit [15:0]    - reg value
*
******************************************************************************/
#define READ_REG_XH98xx(n)        ( 0x08000000 | (0<<28) | ((n)<<19) )
#define WRITE_REG_XH98xx(n,val)   ( 0x88000000 | (1<<28) | ((n)<<19) | ((val)<<0) )
#define WRITE_REG130(val)         WRITE_REG_XH98xx(0x30, (val) | 0xF)  //no regF in max2829
#define WRITE_REG_RXSTART()       WRITE_REG_XH98xx(0x30, (1<<7) | (1<<4) | 0xF) //no regF in max2829
#define WRITE_REG_RXSTOP()        WRITE_REG_XH98xx(0x30, (1<<6) | (1<<4) | 0xE)
#define WRITE_REG_RXDUMMY_NOW()   WRITE_REG_XH98xx(0x000, 0x00FF)
#define WRITE_REG_RXDUMMY()       WRITE_REG_XH98xx(0x000, 0xFFFF)
#define WRITE_REG_TXDUMMY()       WRITE_REG_XH98xx(0x000, 0xFFFE)
#define WRITE_XH98xx_FREQ_CHANGE0(ch) ( 0x88000000 | (1<<28) | ((0x44)<<19) | ((2402+ch)<<0) )            //modify freq from set change=0 to change=1.
#define WRITE_XH98xx_FREQ_CHANGE1(ch) ( 0x88000000 | (1<<28) | ((0x44)<<19) | ((2402+ch)<<0) | (1<<12) ) 

/*****************************************************************************
*
* MAX2829 REGISTER WRITES:
* bit 31 = 1 for SPI block to write
* reg value     - bit 17..4
* reg address N - bit 3..0
*
******************************************************************************/
#define READ_REG(n) (n)
#define WRITE_REG(n,val)   ( 0x80000000 | ((val)<<4) | (n) )

//MAX2829 register definition
//Tx modify Tx frequency(REG3 and REG4) and Tx gain(REGC)
//Rx modify Rx frequency(REG3 and REG4) and Rx gain(REGB)
#define WRITE_REG0                 WRITE_REG(0x00, 0x1140)                    /*Register 0*/
#define WRITE_REG1                 WRITE_REG(0x01, 0x00CA)                    /*Register 1*/
#define WRITE_REG2                 WRITE_REG(0x02, 0x1007)                    /*standby */
#define WRITE_REG3                 WRITE_REG(0x03, 0x30A2)                    /*Intger-Divider Ratio*/
#define WRITE_REG4                 WRITE_REG(0x04, 0x1DDD)                    /*Fractional-Divider Ratio*/
#define WRITE_REG5                 WRITE_REG(0x05, 0x1824)                    /*Band select and PLL*/
#define WRITE_REG6                 WRITE_REG(0x06, 0x1C00)                    /*calibration*/
#define WRITE_REG7                 WRITE_REG(0x07, 0x002A)                    /*lowpass filter*/
#define WRITE_REG8                 WRITE_REG(0x08, 0x1C25)                    /*Rx control/RSSI*/
#define WRITE_REG9                 WRITE_REG(0x09, 0x0603)                    /*Tx linearity/baseband gain*/
#define WRITE_REGA                 WRITE_REG(0x0A, 0x03C0)                    /*PA bias DAC*/
#define WRITE_REGB                 WRITE_REG(0x0B, 0x002B)                    /*Rx Gain 28dB*/
//#define WRITE_REGB                 WRITE_REG(0x0B, 0x006C)                    /*Rx Gain 60dB*/
//#define WRITE_REGB                 WRITE_REG(0x0B, 0x0076)                    /*Rx Gain 80dB*/
//#define WRITE_REGB                 WRITE_REG(0x0B, 0x007B)                    /*Rx Gain 90dB*/
//#define WRITE_REGB                 WRITE_REG(0x0B, 0x007F)                    /*Rx Gain 97dB (MAX)*/
#define WRITE_REGC                 WRITE_REG(0x0C, 0x003F)                    /*Tx VGA Gain*/
#define MAX2829_REG_NUM 13
#define WRITE_REG8_EXT_PIN_EN      WRITE_REG(0x8,0x0C25)                      /*if used AGC*/
/*write tx/rx freq*/
#define WRITE_INT_FREQ_REG(val)         ( WRITE_REG(0x03, (val)) )
#define WRITE_FRA_FREQ_REG(val)         ( WRITE_REG(0x04, (val)) )

//define maximum tx VGA gain
#if ( ((WRITE_REG9 >> 4) & 0x03) == 0x00 )
#define MAX2829_TX_VGA_GAIN_MAX       -5
#elif ( ((WRITE_REG9 >> 4) & 0x03) == 0x01)
#define MAX2829_TX_VGA_GAIN_MAX      -3
#elif ( ((WRITE_REG9 >> 4) & 0x03) == 0x02)
#define MAX2829_TX_VGA_GAIN_MAX      -1       /*actual -1.5dBm*/
#else
#define MAX2829_TX_VGA_GAIN_MAX       0       /*max baseband gain*/
#endif
/* hw_radio_all.c */
#define WRITE_REGn_TX_POWER(power_level)      (WRITE_REG(0x0C, 0x0000 | (g_sys_rf_cfg.power_ctrl_tab[(power_level)])))

/*hw_radio_all.c,  used to control XH98xx TX power: the 1st level PGA and 2nd level PA power*/
#define XH98xx_PGAC_MASK  0x000F    /*Reg2[3:0] indicates PGA power*/
#define WRITE_XH98xx_TX_PGA_POWER(power_level)  ( WRITE_REG_XH98xx(0x02, ( (xh98xx_tx_power_reg_shadow.pgac & (~XH98xx_PGAC_MASK)) | (g_sys_rf_cfg.power_ctrl_tab[power_level] & (~XH98xx_PGAC_MASK) ) ) ) )

#define XH98xx_PA_MASK  0x0003     /*Reg69[1:0] indicates PA power level*/
#define WRITE_XH98xx_TX_PA_POWER(power_level) (WRITE_REG_XH98xx(0x69, ( (xh98xx_tx_power_reg_shadow.pa & (~XH98xx_PA_MASK))  | ((g_sys_rf_cfg.power_ctrl_tab[power_level]>>4) & XH98xx_PA_MASK) ) ) )

 

/*****************************************************************************
* GIO override macros
******************************************************************************/
#define HWradio_SetGIOLowReg(GIO_NAME,value)  \
        mHWreg_Assign_Register(HW_RADIO_GIO_LOW_##GIO_NAME,  (( mHWreg_Get_Register(HW_RADIO_GIO_LOW_##GIO_NAME) & \
                                                             (~(HW_RADIO_GIO_##GIO_NAME##_MASK))) | \
                                                               ((value) << HW_RADIO_GIO_##GIO_NAME##_SHIFT)) )
#define HWradio_SetGIOHighReg(GIO_NAME,value) \
        mHWreg_Assign_Register(HW_RADIO_GIO_HIGH_##GIO_NAME, ((mHWreg_Get_Register(HW_RADIO_GIO_HIGH_##GIO_NAME) & \
                                                             (~(HW_RADIO_GIO_##GIO_NAME##_MASK))) | \
                                                             ((value) << HW_RADIO_GIO_##GIO_NAME##_SHIFT) ) )
#endif // HW_RADIO
#endif // HW_RADIO_DEFS


#ifndef __HW_RADIO_DEFS_H__
#define __HW_RADIO_DEFS_H__

#include <stdint.h>
#include "sys_types.h"
#include "features.h"

/*
 * Selects appropriate hw_radio_defs.h on HW_RADIO definition - as alternative
 * to selecting these directly at makefile level (simplifies situation for IDEs).
 */

/*
 * Below: Defs common to all radio drivers can now be migrated to this file also.
 */


/*****************************************************************************
*
* Generic definitions common to all radio drivers / non RFIC specific values.
*
******************************************************************************/

#define HW_RADIO_SLOT_TIME_POS_TX_START         (0u)
#define HW_RADIO_SLOT_TIME_POS_TX_MID           (312u)
#define HW_RADIO_SLOT_TIME_POS_RX_START         (625u)
#define HW_RADIO_SLOT_TIME_POS_RX_MID           (937u)
#define HW_RADIO_TOTAL_SLOT_TIME                (1250u)
#define HW_RADIO_NORMAL_TIME_FOR_SPI_WRITE      (16u)

#define HW_RADIO_LE_TIFS                        (150u)
#define HW_RADIO_LE_TIFS_B4_TX                  (0x0700u) // LE_check = 3. Tx_next = 1.
#define HW_RADIO_LE_TIFS_B4_RX                  (0x0600u) // LE_check = 3. Tx_next = 0.


/*****************************************************************************
*
* Defines to support high/low override control of GIO lines (debug purposes):
*
* HWradio_SetOverrideLow(GIO_N);
* HWradio_SetOverrideHigh(GIO_N);
*
******************************************************************************/
#define HW_RADIO_GIO_LOW_GIO_0              HAB_GIO_HIGH_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_HIGH_GIO_0             HAB_GIO_LOW_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_0    0x00008000

#define HW_RADIO_GIO_LOW_GIO_1              HAB_GIO_HIGH_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_HIGH_GIO_1             HAB_GIO_LOW_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_1    0x80000000

#define HW_RADIO_GIO_LOW_GIO_2              HAB_GIO_HIGH_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_HIGH_GIO_2             HAB_GIO_LOW_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_2    0x00008000

#define HW_RADIO_GIO_LOW_GIO_3              HAB_GIO_HIGH_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_HIGH_GIO_3             HAB_GIO_LOW_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_3    0x80000000

#define HW_RADIO_GIO_LOW_GIO_4              HAB_GIO_HIGH_CTRL_5_AND_4_REG
#define HW_RADIO_GIO_HIGH_GIO_4             HAB_GIO_LOW_CTRL_5_AND_4_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_4    0x00008000

#define HW_RADIO_GIO_LOW_GIO_5              HAB_GIO_HIGH_CTRL_5_AND_4_REG
#define HW_RADIO_GIO_HIGH_GIO_5             HAB_GIO_LOW_CTRL_5_AND_4_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_5    0x80000000

#define HW_RADIO_GIO_LOW_GIO_6              HAB_GIO_HIGH_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_HIGH_GIO_6             HAB_GIO_LOW_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_6    0x00008000

#define HW_RADIO_GIO_LOW_GIO_7              HAB_GIO_HIGH_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_HIGH_GIO_7             HAB_GIO_LOW_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_7    0x80000000

#define HW_RADIO_GIO_LOW_GIO_8              HAB_GIO_HIGH_CTRL_9_AND_8_REG
#define HW_RADIO_GIO_HIGH_GIO_8             HAB_GIO_LOW_CTRL_9_AND_8_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_8    0x00008000

#define HW_RADIO_GIO_LOW_GIO_9              HAB_GIO_HIGH_CTRL_9_AND_8_REG
#define HW_RADIO_GIO_HIGH_GIO_9             HAB_GIO_LOW_CTRL_9_AND_8_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_9    0x80000000

#define HW_RADIO_GIO_LOW_GIO_A              HAB_GIO_HIGH_CTRL_B_AND_A_REG
#define HW_RADIO_GIO_HIGH_GIO_A             HAB_GIO_LOW_CTRL_B_AND_A_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_A    0x00008000

#define HW_RADIO_GIO_LOW_GIO_B              HAB_GIO_HIGH_CTRL_B_AND_A_REG
#define HW_RADIO_GIO_HIGH_GIO_B             HAB_GIO_LOW_CTRL_B_AND_A_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_B    0x80000000

/*******************************************************************************
 * The following is a define the location of GIO lines in the MAX2829
 *
 * GIO 0   used to control TXENA of MAX2829.
 * GIO 1   used to control RXENA of MAX2829.
 * GIO 2   used to control #SHDN of MAX2829.
 *
 *******************************************************************************/
#define HW_RADIO_GIO_HIGH_TXENA                                HAB_GIO_HIGH_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_LOW_TXENA                                 HAB_GIO_LOW_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_TXENA                       0x00008000
#define HW_RADIO_GIO_TXENA_SHIFT                               0
#define HW_RADIO_GIO_TXENA_MASK                                0x0000FFFF
#define HW_RADIO_GIO_HIGH_RXENA                                HAB_GIO_HIGH_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_LOW_RXENA                                 HAB_GIO_LOW_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_RXENA                       0x80000000
#define HW_RADIO_GIO_RXENA_SHIFT                               16
#define HW_RADIO_GIO_RXENA_MASK                                0xFFFF0000
#define HW_RADIO_GIO_HIGH_RXENA_AND_TXENA                      HAB_GIO_HIGH_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_LOW_RXENA_AND_TXENA                       HAB_GIO_LOW_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_RXENA_AND_TXENA             0x80008000
#define HW_RADIO_GIO_HIGH_SHDN                                 HAB_GIO_HIGH_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_LOW_SHDN                                  HAB_GIO_LOW_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_SHDN                        0x00008000
#define HW_RADIO_GIO_SHDN_SHIFT                                0x00
#define HW_RADIO_GIO_SHDN_MASK                                 0x0000FFFF
#define HW_RADIO_GIO_HIGH_TXENA_PAGE                           HAB_GIO_HIGH_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_LOW_TXENA_PAGE                            HAB_GIO_LOW_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_TXENA_PAGE                  0x00008000
#define HW_RADIO_GIO_TXENA_PAGE_SHIFT                          0x00
#define HW_RADIO_GIO_TXENA_PAGE_MASK                           0x0000FFFF
#define HW_RADIO_GIO_HIGH_RXENA_PAGE                           HAB_GIO_HIGH_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_LOW_RXENA_PAGE                            HAB_GIO_LOW_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_RXENA_PAGE                  0x80000000
#define HW_RADIO_GIO_RXENA_PAGE_SHIFT                          16
#define HW_RADIO_GIO_RXENA_PAGE_MASK                           0xFFFF0000
#define HW_RADIO_GIO_HIGH_RXENA_PAGE_AND_TXENA_PAGE            HAB_GIO_HIGH_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_LOW_RXENA_PAGE_AND_TXENA_PAGE             HAB_GIO_LOW_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_RXENA_PAGE_AND_TXENA_PAGE   0x80008000
#define HW_RADIO_COMB_RXENA_AND_TXENA_DOUBLE_WIN               HAB_GIO_CTRL_COMB_3_2_1_0_REG

/*****************************************************************************
* GIO override macros
******************************************************************************/
#define HWradio_SetOverrideLow(GIO_NAME)	\
				mHWreg_Logical_OR_With_Register(HW_RADIO_GIO_LOW_##GIO_NAME, \
					HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME); \
				mHWreg_Logical_AND_With_Register(HW_RADIO_GIO_HIGH_##GIO_NAME, \
					~(HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME))

#define HWradio_SetOverrideHigh(GIO_NAME)	\
				mHWreg_Logical_OR_With_Register(HW_RADIO_GIO_HIGH_##GIO_NAME, \
					HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME); \
				mHWreg_Logical_AND_With_Register(HW_RADIO_GIO_LOW_##GIO_NAME, \
					~(HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME))

#define HWradio_CancelOverride(GIO_NAME) \
				mHWreg_Logical_AND_With_Register(HW_RADIO_GIO_LOW_##GIO_NAME, \
					~(HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME)); \
				mHWreg_Logical_AND_With_Register(HW_RADIO_GIO_HIGH_##GIO_NAME, \
					~(HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME))


/* PHY Type */
#define PHY_TYPE_MAX2829   0
#define PHY_TYPE_XH98xx    1
#define PHY_TYPE_BU9468MUV 2

#endif

