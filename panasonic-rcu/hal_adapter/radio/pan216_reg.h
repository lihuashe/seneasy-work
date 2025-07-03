#ifndef PAN216_REG_H
#define PAN216_REG_H

#include <stdbool.h>
#include <stdint.h>
#include "pan216_port.h"

#define BIT0           0x01
#define BIT1           0x02
#define BIT2           0x04
#define BIT3           0x08
#define BIT4           0x10
#define BIT5           0x20
#define BIT6           0x40
#define BIT7           0x80

#define BITMASK_1_0    0x03
#define BITMASK_2_0    0x07
#define BITMASK_3_0    0x0F
#define BITMASK_4_0    0x1F
#define BITMASK_5_0    0x3F
#define BITMASK_6_0    0x7F
#define BITMASK_7_0    0xFF
#define BITMASK_2_1    0x06
#define BITMASK_3_1    0x0E
#define BITMASK_4_1    0x1E
#define BITMASK_5_1    0x3E
#define BITMASK_6_1    0x7E
#define BITMASK_7_1    0xFE
#define BITMASK_3_2    0x0C
#define BITMASK_4_2    0x1C
#define BITMASK_5_2    0x3C
#define BITMASK_6_2    0x7C
#define BITMASK_7_2    0xFC
#define BITMASK_4_3    0x18
#define BITMASK_5_3    0x38
#define BITMASK_6_3    0x78
#define BITMASK_7_3    0xF8
#define BITMASK_5_4    0x30
#define BITMASK_6_4    0x70
#define BITMASK_7_4    0xF0
#define BITMASK_6_5    0x60
#define BITMASK_7_5    0xE0
#define BITMASK_7_6    0xC0

#define GET_SHIFT(v)   ((__ctz(v))) // bit shift in a uint8_t value

#define P_ASSERT(fn)\
    do{                           \
        if (PAN216_OK != fn){     \
            return PAN216_ERR;    \
        }                         \
    }while(0);

/* result */
typedef enum
{
    PAN216_OK,  // Operate ok
    PAN216_ERR, // Operate fail
} PAN216_Err_t;

/* Mode */
typedef enum
{
    PAN216_STATE_DEEP_SLEEP = 0, // Deep sleep state
    PAN216_STATE_SLEEP,    // Sleep state
    PAN216_STATE_STB1,     // Standby1 state
    PAN216_STATE_STB2,     // Standby2 state
    PAN216_STATE_STB3,     // Standby3 state
    PAN216_STATE_TX,       // Sending data
    PAN216_STATE_RX,       // Receiving data
    PAN216_STATE_UNDEFINED, // Undefined state
    PAN216_STATE_TRXTRANS, //
    PAN216_STATE_ARD_WAIT, //
    PAN216_STATE_IDLE,     // Idle state, waiting for commands or events
} PAN216_State_t;

#define REG_CLEAR_ALL_BITS   0x00

/* Page Select */
#define PAN216_PAGE0         0x00
#define PAN216_PAGE1         0x01

/* page0  */
# define PAGE_CFG                                                       0X00 // default:0x00
/* BIT0  */
# define PAGE_CFG_REG_PAGE_SEL                                          0X01 // default:0b0

/* page0  */
# define TRX_FIFO                                                       0X01 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TRX_FIFO_FIFO_READ_WRITE_ACESS_POINT                           0XFF // default:0b0

/* page0  */
# define STATE_CFG                                                      0X02 // default:0x00
/* BIT7  */
# define STATE_CFG_TX_FIFO_READY                                        0X80 // default:0b0
/* BIT6  */
# define STATE_CFG_REG_LEVEL_SHIFT_EN                                   0X40 // default:0b0
/* BIT5  */
# define STATE_CFG_POR_NRSTL                                            0X20 // default:0b0
/* BIT4  */
# define STATE_CFG_ISO_TO_0                                             0X10 // default:0b0
/* BIT2|BIT1|BIT0  */
# define STATE_CFG_REG_OPERATE_MODE                                     0X07 // default:0b0

/* page0  */
# define SYS_CFG                                                        0X03 // default:0x02
/* BIT7  */
# define SYS_CFG_REG_TEST_MODE_EN                                       0X80 // default:0b0
/* BIT6  */
# define SYS_CFG_REG_LDO_LOW_EN                                         0X40 // default:0b0
/* BIT2  */
# define SYS_CFG_IRQ_MOSI_MUX_EN                                        0X04 // default:0b0
/* BIT1  */
# define SYS_CFG_REG_SOFT_RSTL                                          0X02 // default:0b1
/* BIT0  */
# define SYS_CFG_PAD_SEL_MISO                                           0X01 // default:0b0

/* page0  */
# define SPI_CFG                                                        0X04 // default:0x73
/* BIT7  */
# define SPI_CFG_REG_SPI3_REN                                            0X80 // default:0b0
/* BIT6  */
# define SPI_CFG_REG_MOSI_PUEN                                          0X40 // default:0b1
/* BIT5  */
# define SPI_CFG_REG_CSN_PUEN                                           0X20 // default:0b1
/* BIT4  */
# define SPI_CFG_REG_SCK_PUEN                                           0X10 // default:0b1
/* BIT3  */
# define SPI_CFG_REG_IN_PAD_MODE                                        0X08 // default:0b0
/* BIT2|BIT1|BIT0  */
# define SPI_CFG_REG_OUT_PAD_MODE                                       0X07 // default:0b11

/* page0  */
# define IPTAT_CFG                                                      0X05 // default:0x88
/* BIT7|BIT6|BIT5|BIT4  */
# define IPTAT_CFG_LP_VREF_TRIM                                         0XF0 // default:0b1000
/* BIT3|BIT2|BIT1|BIT0  */
# define IPTAT_CFG_PTAT_VBG_TRIM                                        0X0F // default:0b1000

/* page0  */
# define LP_CFG                                                         0X06 // default:0x05
/* BIT7  */
# define LP_CFG_EN_LDO_HP                                               0X80 // default:0b0
/* BIT6  */
# define LP_CFG_PWR_UP                                                  0X40 // default:0b0
/* BIT5  */
# define LP_CFG_EN_PM                                                   0X20 // default:0b0
/* BIT4  */
# define LP_CFG_CE_INT                                                  0X10 // default:0b0
/* BIT3  */
# define LP_CFG_IRQ_I2C_MUX_EN                                          0X08 // default:0b0
/* BIT2|BIT1  */
# define LP_CFG_PTAT_TEMP_TRIM                                          0X06 // default:0b10
/* BIT0  */
# define LP_CFG_EN_DIG_COREPOWER                                        0X01 // default:0b1

/* page0  */
# define WMODE_CFG0                                                     0X07 // default:0x49
/* BIT7|BIT6  */
# define WMODE_CFG0_CRC_MODE_1_0                                        0XC0 // default:0b1
/* BIT5  */
# define WMODE_CFG0_CHIP_MODE                                           0X20 // default:0b0
/* BIT4  */
# define WMODE_CFG0_NORDIC_ENHANCE                                      0X10 // default:0b0
/* BIT3  */
# define WMODE_CFG0_SCR_ENABLE                                          0X08 // default:0b1
/* BIT2  */
# define WMODE_CFG0_ACCADDR_CRC_DIS                                     0X04 // default:0b0
/* BIT1  */
# define WMODE_CFG0_TX_NOACK_EN                                         0X02 // default:0b0
/* BIT0  */
# define WMODE_CFG0_ENDIAN                                              0X01 // default:0b1

/* page0  */
# define WMODE_CFG1                                                     0X08 // default:0x83
/* BIT7  */
# define WMODE_CFG1_RX_GOON                                             0X80 // default:0b1
/* BIT6  */
# define WMODE_CFG1_PRI_EXIT_RX                                         0X40 // default:0b0
/* BIT5  */
# define WMODE_CFG1_FIFO_128_EN                                         0X20 // default:0b0
/* BIT4  */
# define WMODE_CFG1_DPY_EN                                              0X10 // default:0b0
/* BIT3  */
# define WMODE_CFG1_ENHANCE                                             0X08 // default:0b0
/* BIT2  */
# define WMODE_CFG1_NORMAL_M1                                           0X04 // default:0b0
/* BIT1|BIT0  */
# define WMODE_CFG1_ADDR_BYTE_LENGTH                                    0X03 // default:0b11

/* page0  */
# define RXPLLEN_CFG                                                    0X09 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define RXPLLEN_CFG_RX_PAYLOAD_LENGTH                                  0XFF // default:0b0

/* page0  */
# define TXPLLEN_CFG                                                    0X0A // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXPLLEN_CFG_TX_PAYLOAD_LENGTH                                  0XFF // default:0b0

/* page0  */
# define RFIRQ_CFG                                                      0X0B // default:0x00
/* BIT7  */
# define RFIRQ_CFG_TX_IRQ_MASK                                          0X80 // default:0b0
/* BIT6  */
# define RFIRQ_CFG_TX_MAX_RT_IRQ_MASK                                   0X40 // default:0b0
/* BIT5  */
# define RFIRQ_CFG_RX_ADDR_ERR_MASK                                     0X20 // default:0b0
/* BIT4  */
# define RFIRQ_CFG_RX_CRC_ERR_IRQ_MASK                                  0X10 // default:0b0
/* BIT3  */
# define RFIRQ_CFG_RX_LENGTH_ERR_IRQ_MASK                               0X08 // default:0b0
/* BIT2  */
# define RFIRQ_CFG_RX_PID_ERR_IRQ_MASK                                  0X04 // default:0b0
/* BIT1  */
# define RFIRQ_CFG_RX_TIMEOUT_IRQ_MASK                                  0X02 // default:0b0
/* BIT0  */
# define RFIRQ_CFG_RX_IRQ_MASK                                          0X01 // default:0b0

/* page0  */
# define PID_CFG                                                        0X0C // default:0x00
/* BIT7  */
# define PID_CFG_PID_MANUAL_EN                                          0X80 // default:0b0
/* BIT6|BIT5|BIT4  */
# define PID_CFG_ADDR_ERR_THR_2_0                                       0X70 // default:0b0
/* BIT3|BIT2  */
# define PID_CFG_RX_PID_MANUAL                                          0X0C // default:0b0
/* BIT1|BIT0  */
# define PID_CFG_TX_PID_MANUAL                                          0X03 // default:0b0

/* page0  */
# define TRXTWTL_CFG                                                    0X0D // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TRXTWTL_CFG_TRX_TRANS_WAIT_TIME_7_0                            0XFF // default:0b0

/* page0  */
# define TRXTWTH_CFG                                                    0X0E // default:0x00
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TRXTWTH_CFG_TRX_TRANS_WAIT_TIME_14_8                           0X7F // default:0b0

/* page0  */
# define PIPE0_RXADDR0_CFG                                              0X0F // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE0_RXADDR0_CFG_RX_ADDDR_7_0                                 0XFF // default:0b11001100

/* page0  */
# define PIPE0_RXADDR1_CFG                                              0X10 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE0_RXADDR1_CFG_RX_ADDDR_15_8                                0XFF // default:0b11001100

/* page0  */
# define PIPE0_RXADDR2_CFG                                              0X11 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE0_RXADDR2_CFG_RX_ADDDR_23_16                               0XFF // default:0b11001100

/* page0  */
# define PIPE0_RXADDR3_CFG                                              0X12 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE0_RXADDR3_CFG_RX_ADDDR_31_24                               0XFF // default:0b11001100

/* page0  */
# define PIPE0_RXADDR4_CFG                                              0X13 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE0_RXADDR4_CFG_RX_ADDDR_39_32                               0XFF // default:0b11001100

/* page0  */
# define TXADDR0_CFG                                                    0X14 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXADDR0_CFG_TX_ADDR_7_0                                        0XFF // default:0b11001100

/* page0  */
# define TXADDR1_CFG                                                    0X15 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXADDR1_CFG_TX_ADDR_15_8                                       0XFF // default:0b11001100

/* page0  */
# define TXADDR2_CFG                                                    0X16 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXADDR2_CFG_TX_ADDR_23_16                                      0XFF // default:0b11001100

/* page0  */
# define TXADDR3_CFG                                                    0X17 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXADDR3_CFG_TX_ADDR_31_24                                      0XFF // default:0b11001100

/* page0  */
# define TXADDR4_CFG                                                    0X18 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXADDR4_CFG_TX_ADDR_39_32                                      0XFF // default:0b11001100

/* page0  */
# define PKT_EXT_CFG                                                    0X19 // default:0x00
/* BIT7  */
# define PKT_EXT_CFG_W_RX_MAX_CTRL_EN                                   0X80 // default:0b0
/* BIT6  */
# define PKT_EXT_CFG_HDR_LEN_EXIST                                      0X40 // default:0b0
/* BIT5|BIT4  */
# define PKT_EXT_CFG_HDR_LEN_NUMB                                       0X30 // default:0b0
/* BIT3  */
# define PKT_EXT_CFG_PRI_TX_FEC                                         0X08 // default:0b0
/* BIT2  */
# define PKT_EXT_CFG_PRI_RX_FEC                                         0X04 // default:0b0
/* BIT1|BIT0  */
# define PKT_EXT_CFG_PRI_CI_MODE                                        0X03 // default:0b0

/* page0  */
# define SCR_CFG                                                        0X1A // default:0x7f
/* BIT7  */
# define SCR_CFG_ACCADDR_SCR_DIS                                        0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define SCR_CFG_SCR_INI                                                0X7F // default:0b1111111

/* page0  */
# define TXHDR0_CFG                                                     0X1B // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXHDR0_CFG_TX_HEADER0                                          0XFF // default:0b0

/* page0  */
# define TXHDR1_CFG                                                     0X1C // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXHDR1_CFG_TX_HEADER1                                          0XFF // default:0b0

/* page0  */
# define TXRAMADDR_CFG                                                  0X1D // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define TXRAMADDR_CFG_TX_RAM_START_ADDR                                0XFF // default:0b0

/* page0  */
# define RXRAMADDR_CFG                                                  0X1E // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define RXRAMADDR_CFG_RX_RAM_START_ADDR                                0XFF // default:0b0

/* page0  */
# define RXPIPE_CFG                                                     0X1F // default:0x01
/* BIT7  */
# define RXPIPE_CFG_TX_INVERT_EN                                        0X80 // default:0b0
/* BIT6  */
# define RXPIPE_CFG_RX_INVERT_EN                                        0X40 // default:0b0
/* BIT5  */
# define RXPIPE_CFG_ADDR_EN_5                                           0X20 // default:0b0
/* BIT4  */
# define RXPIPE_CFG_ADDR_EN_4                                           0X10 // default:0b0
/* BIT3  */
# define RXPIPE_CFG_ADDR_EN_3                                           0X08 // default:0b0
/* BIT2  */
# define RXPIPE_CFG_ADDR_EN_2                                           0X04 // default:0b0
/* BIT1  */
# define RXPIPE_CFG_ADDR_EN_1                                           0X02 // default:0b0
/* BIT0  */
# define RXPIPE_CFG_ADDR_EN_0                                           0X01 // default:0b1

/* page0  */
# define PIPE1_RXADDR0_CFG                                              0X20 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE1_RXADDR0_CFG_W_PRI_ADDR_1_7_0                             0XFF // default:0b11001100

/* page0  */
# define PIPE1_RXADDR1_CFG                                              0X21 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE1_RXADDR1_CFG_W_PRI_ADDR_1_15_8                            0XFF // default:0b11001100

/* page0  */
# define PIPE1_RXADDR2_CFG                                              0X22 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE1_RXADDR2_CFG_W_PRI_ADDR_1_23_16                           0XFF // default:0b11001100

/* page0  */
# define PIPE1_RXADDR3_CFG                                              0X23 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE1_RXADDR3_CFG_W_PRI_ADDR_1_31_24                           0XFF // default:0b11001100

/* page0  */
# define PIPE1_RXADDR4_CFG                                              0X24 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE1_RXADDR4_CFG_W_PRI_ADDR_1_39_32                           0XFF // default:0b11001100

/* page0  */
# define PIPE2_RXADDR0_CFG                                              0X25 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE2_RXADDR0_CFG_W_PRI_ADDR_2_7_0                             0XFF // default:0b11001100

/* page0  */
# define PIPE3_RXADDR0_CFG                                              0X26 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE3_RXADDR0_CFG_W_PRI_ADDR_3_7_0                             0XFF // default:0b11001100

/* page0  */
# define PIPE4_RXADDR0_CFG                                              0X27 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE4_RXADDR0_CFG_W_PRI_ADDR_4_7_0                             0XFF // default:0b11001100

/* page0  */
# define PIPE5_RXADDR0_CFG                                              0X28 // default:0xcc
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define PIPE5_RXADDR0_CFG_W_PRI_ADDR_5_7_0                             0XFF // default:0b11001100

/* page0  */
# define TXAUTO_CFG                                                     0X29 // default:0x03
/* BIT7|BIT6|BIT5|BIT4  */
# define TXAUTO_CFG_ARD                                                 0XF0 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define TXAUTO_CFG_ARC_3_0                                             0X0F // default:0b11

/* page0  */
# define TRXMODE_CFG                                                    0X2A // default:0x01
/* BIT7  */
# define TRXMODE_CFG_REG_TX_CFG_MODE                                    0X80 // default:0b0
/* BIT6|BIT5  */
# define TRXMODE_CFG_REG_RX_CFG_MODE                                    0X60 // default:0b0
/* BIT4  */
# define TRXMODE_CFG_PRE_2BYTE_MODE                                     0X10 // default:0b0
/* BIT3  */
# define TRXMODE_CFG_W_PRE_SYNC_12B_EN                                  0X08 // default:0b0
/* BIT2  */
# define TRXMODE_CFG_W_PRE_SYNC_8B_EN                                   0X04 // default:0b0
/* BIT1  */
# define TRXMODE_CFG_W_PRE_SYNC_4B_EN                                   0X02 // default:0b0
/* BIT0  */
# define TRXMODE_CFG_W_PRE_SYNC_EN                                      0X01 // default:0b1

/* page0  */
# define RXTIMEOUTL_CFG                                                 0X2B // default:0xd0
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define RXTIMEOUTL_CFG_REG_RX_TIMEOUT_7_0                              0XFF // default:0b11010000

/* page0  */
# define RXTIMEOUTH_CFG                                                 0X2C // default:0x07
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define RXTIMEOUTH_CFG_REG_RX_TIMEOUT_15_8                             0XFF // default:0b111

/* page0  */
# define BLEMATCH_CFG0                                                  0X2D // default:0x00
/* BIT7  */
# define BLEMATCH_CFG0_SNIF_EN                                          0X80 // default:0b0
/* BIT6|BIT5|BIT4  */
# define BLEMATCH_CFG0_WL_MATCH_MODE                                    0X70 // default:0b0
/* BIT3|BIT2  */
# define BLEMATCH_CFG0_BLELEN_MATCH_MODE                                0X0C // default:0b0
/* BIT1|BIT0  */
# define BLEMATCH_CFG0_PATT_MATCH_THRESHOLD_9_8                         0X03 // default:0b0

/* page0  */
# define BLEMATCH_CFG1                                                  0X2E // default:0x28
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define BLEMATCH_CFG1_PATT_MATCH_THRESHOLD_7_0                         0XFF // default:0b101000

/* page0  */
# define WLIST0_CFG                                                     0X2F // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define WLIST0_CFG_WL_ADVA_7_0                                         0XFF // default:0b0

/* page0  */
# define WLIST1_CFG                                                     0X30 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define WLIST1_CFG_WL_ADVA_15_8                                        0XFF // default:0b0

/* page0  */
# define WLIST2_CFG                                                     0X31 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define WLIST2_CFG_WL_ADVA_23_16                                       0XFF // default:0b0

/* page0  */
# define WLIST3_CFG                                                     0X32 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define WLIST3_CFG_WL_ADVA_31_24                                       0XFF // default:0b0

/* page0  */
# define WLIST4_CFG                                                     0X33 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define WLIST4_CFG_WL_ADVA_39_32                                       0XFF // default:0b0

/* page0  */
# define WLIST5_CFG                                                     0X34 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define WLIST5_CFG_WL_ADVA_47_40                                       0XFF // default:0b0

/* page0  */
# define BLEMATCHSTART_CFG                                              0X35 // default:0x07
/* BIT7  */
# define BLEMATCHSTART_CFG_W_LQI_EN_2BYTE                               0X80 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define BLEMATCHSTART_CFG_PLD_START_BYTE                               0X3F // default:0b111

/* page0  */
# define REG_P0_0X36                                                    0X36 // default:0x45
/* BIT7  */
# define REG_P0_0X36_DEMOD_DFE_EN_OVRD                                  0X80 // default:0b0
/* BIT6  */
# define REG_P0_0X36_DEMOD_EN_DFE                                       0X40 // default:0b1
/* BIT5|BIT4  */
# define REG_P0_0X36_BW_MODE                                            0X30 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X36_DEMOD_COEFF_UNFILT                                 0X0F // default:0b101

/* page0  */
# define REG_P0_0X37                                                    0X37 // default:0x60
/* BIT7  */
# define REG_P0_0X37_OSC_SEL                                            0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X37_ZCONES_OFFSET                                      0X7F // default:0b1100000

/* page0  */
# define REG_P0_0X38                                                    0X38 // default:0x10
/* BIT6|BIT5  */
# define REG_P0_0X38_ZCNORMAL_SHIFT                                     0X60 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X38_ZCONES_SCALE                                       0X1F // default:0b10000

/* page0  */
# define REG_P0_0X39                                                    0X39 // default:0x4e
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X39_RF_CH                                              0XFF // default:0b1001110

/* page0  */
# define REG_P0_0X3A                                                    0X3A // default:0x57
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X3A_CH_OFFSET_7_0                                      0XFF // default:0b1010111

/* page0  */
# define REG_P0_0X3B                                                    0X3B // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X3B_FREQUENCY_OFFSET_OVRDWD_7_0                        0XFF // default:0b0

/* page0  */
# define REG_P0_0X3C                                                    0X3C // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X3C_FREQUENCY_OFFSET_OVRDWD_15_8                       0XFF // default:0b0

/* page0  */
# define REG_P0_0X3D                                                    0X3D // default:0x00
/* BIT7  */
# define REG_P0_0X3D_FREQUENCY_OFFSET_OVRDWD_SEL                        0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X3D_FREQUENCY_OFFSET_OVRDWD_22_16                      0X7F // default:0b0

/* page0  */
# define REG_P0_0X3E                                                    0X3E // default:0x1e
/* BIT6  */
# define REG_P0_0X3E_SDNINT_SEL                                         0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X3E_SDNINT_OVRD_5_0                                    0X3F // default:0b11110

/* page0  */
# define REG_P0_0X3F                                                    0X3F // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X3F_FREQUENCY_CORRECTION_RX_7_0                        0XFF // default:0b0

/* page0  */
# define REG_P0_0X40                                                    0X40 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X40_FREQUENCY_CORRECTION_RX_15_8                       0XFF // default:0b0

/* page0  */
# define REG_P0_0X41                                                    0X41 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X41_FREQUENCY_CORRECTION_TX_7_0                        0XFF // default:0b0

/* page0  */
# define REG_P0_0X42                                                    0X42 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X42_FREQUENCY_CORRECTION_TX_15_8                       0XFF // default:0b0

/* page0  */
# define REG_P0_0X43                                                    0X43 // default:0x32
/* BIT7  */
# define REG_P0_0X43_RXADC_MODE_MANUL_EN                                0X80 // default:0b0
/* BIT6  */
# define REG_P0_0X43_RXADC_MODE_SEL                                     0X40 // default:0b0
/* BIT5|BIT4  */
# define REG_P0_0X43_TXPA_MODE_SEL_1_0                                  0X30 // default:0b11
/* BIT3  */
# define REG_P0_0X43_EN_RXADCCLK                                        0X08 // default:0b0
/* BIT2  */
# define REG_P0_0X43_FSYNVCO_TXCTK                                      0X04 // default:0b0
/* BIT1|BIT0  */
# define REG_P0_0X43_RXFLTR_IF_1_0                                      0X03 // default:0b10

/* page0  */
# define REG_P0_0X44                                                    0X44 // default:0x7c
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P0_0X44_TXPA_POUT_CRNT_3_0                                 0XF0 // default:0b111
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X44_TXPA_LDO_SEL_3_0                                   0X0F // default:0b1100

/* page0  */
# define REG_P0_0X45                                                    0X45 // default:0x00
/* BIT3|BIT2  */
# define REG_P0_0X45_OCLK_SEL_1_0                                       0X0C // default:0b0
/* BIT1|BIT0  */
# define REG_P0_0X45_PAD_IRQ_MUX_1_0                                    0X03 // default:0b0

/* page0  */
# define REG_P0_0X46                                                    0X46 // default:0x00
/* BIT7  */
# define REG_P0_0X46_RSSI_READ_CLK_EN                                   0X80 // default:0b0
/* BIT6  */
# define REG_P0_0X46_RSSI_READ_EN                                       0X40 // default:0b0
/* BIT5  */
# define REG_P0_0X46_AGC_EN_OVRD_SEL                                    0X20 // default:0b0
/* BIT4  */
# define REG_P0_0X46_AGC_EN_OVRD                                        0X10 // default:0b0
/* BIT2|BIT1|BIT0  */
# define REG_P0_0X46_AGC_IQFLTR_MODE                                    0X07 // default:0b0

/* page0  */
# define REG_P0_0X47                                                    0X47 // default:0x3f
/* BIT7|BIT6  */
# define REG_P0_0X47_AGC_FLTROVRD_MD                                    0XC0 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X47_AGC_IQFLTR_COEFF                                   0X3F // default:0b111111

/* page0  */
# define REG_P0_0X48                                                    0X48 // default:0x02
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X48_AGC_WAIT_TIME_7_0                                  0XFF // default:0b10

/* page0  */
# define REG_P0_0X49                                                    0X49 // default:0x00
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X49_AGC_WAIT_TIME_12_8                                 0X1F // default:0b0

/* page0  */
# define REG_P0_0X4A                                                    0X4A // default:0x40
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X4A_AGC_WAIT_TIME_SWITCH_7_0                           0XFF // default:0b1000000

/* page0  */
# define REG_P0_0X4B                                                    0X4B // default:0x40
/* BIT6  */
# define REG_P0_0X4B_SWITCH_TMR_EN                                      0X40 // default:0b1
/* BIT5  */
# define REG_P0_0X4B_AGC_SWITCH_FREEZE                                  0X20 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X4B_AGC_WAIT_TIME_SWITCH_12_8                          0X1F // default:0b0

/* page0  */
# define REG_P0_0X4D                                                    0X4D // default:0x81
/* BIT7  */
# define REG_P0_0X4D_AGC_FREEZE                                         0X80 // default:0b1
/* BIT4  */
# define REG_P0_0X4D_AGC_ITERATION_FREEZE                               0X10 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X4D_AGC_ITERATION_LIMIT                                0X0F // default:0b1

/* page0  */
# define REG_P0_0X4E                                                    0X4E // default:0x3f
/* BIT6  */
# define REG_P0_0X4E_GAIN_SEL                                           0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X4E_GAIN_OVRD                                          0X3F // default:0b111111

/* page0  */
# define REG_P0_0X4F                                                    0X4F // default:0x3f
/* BIT7  */
# define REG_P0_0X4F_ENABLE_AVG_RSSI_RESULT                             0X80 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X4F_RSSI_IQFLTR_COEFF                                  0X3F // default:0b100000

/* page0  */
# define REG_P0_0X50                                                    0X50 // default:0x30
/* BIT6|BIT5|BIT4  */
# define REG_P0_0X50_RSSI_IQFLTR_MODE                                   0X70 // default:0b11
/* BIT1|BIT0  */
# define REG_P0_0X50_AVG_MODE_RSSI_RESULT                               0X03 // default:0b0

/* page0  */
# define REG_P0_0X51                                                    0X51 // default:0x20
/* BIT7  */
# define REG_P0_0X51_ENABLE_AVG_RSSI_RESULT_2                           0X80 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X51_RSSI_IQFLTR_COEFF_2                                0X3F // default:0b100000

/* page0  */
# define REG_P0_0X52                                                    0X52 // default:0x30
/* BIT6|BIT5|BIT4  */
# define REG_P0_0X52_RSSI_IQFLTR_MODE_2                                 0X70 // default:0b11
/* BIT1|BIT0  */
# define REG_P0_0X52_AVG_MODE_RSSI_RESULT_2                             0X03 // default:0b0

/* page0  */
# define REG_P0_0X53                                                    0X53 // default:0x11
/* BIT6  */
# define REG_P0_0X53_RSSI_ERR_FLAG_EN                                   0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X53_RSSI_GAP_CFG_5_0                                   0X3F // default:0b10001

/* page0  */
# define REG_P0_0X54                                                    0X54 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X54_RSSI_THRESHOLD                                     0XFF // default:0b0

/* page0  */
# define REG_P0_0X55                                                    0X55 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X55_RSSI_TH_1                                          0XFF // default:0b0

/* page0  */
# define REG_P0_0X56                                                    0X56 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X56_RSSI_TH_2                                          0XFF // default:0b0

/* page0  */
# define REG_P0_0X57                                                    0X57 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X57_RSSI_TH_3                                          0XFF // default:0b0

/* page0  */
# define REG_P0_0X58                                                    0X58 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X58_RSSI_INIT                                          0XFF // default:0b0

/* page0  */
# define REG_P0_0X59                                                    0X59 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X59_RSSI_HYS                                           0XFF // default:0b0

/* page0  */
# define REG_P0_0X5A                                                    0X5A // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X5A_RF_RSSI_FIX_0                                      0XFF // default:0b0

/* page0  */
# define REG_P0_0X5B                                                    0X5B // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X5B_RF_RSSI_FIX_1                                      0XFF // default:0b0

/* page0  */
# define REG_P0_0X5C                                                    0X5C // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X5C_RF_RSSI_FIX_2                                      0XFF // default:0b0

/* page0  */
# define REG_P0_0X5D                                                    0X5D // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X5D_RF_RSSI_FIX_3                                      0XFF // default:0b0

/* page0  */
# define REG_P0_0X5E                                                    0X5E // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X5E_GAIN_WORD_0                                        0XFF // default:0b0

/* page0  */
# define REG_P0_0X5F                                                    0X5F // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X5F_GAIN_WORD_1                                        0XFF // default:0b0

/* page0  */
# define REG_P0_0X60                                                    0X60 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X60_GAIN_WORD_2                                        0XFF // default:0b0

/* page0  */
# define REG_P0_0X61                                                    0X61 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X61_GAIN_WORD_3                                        0XFF // default:0b0

/* page0  */
# define REG_P0_0X62                                                    0X62 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X62_RSSI_CONV_GAIN                                     0XFF // default:0b0

/* page0  */
# define REG_P0_0X63                                                    0X63 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X63_RSSI_BYPASS                                        0XFF // default:0b0

/* page0  */
# define REG_P0_0X64                                                    0X64 // default:0x20
/* BIT7|BIT6|BIT5  */
# define REG_P0_0X64_REG_LDO_ANA_CLOSE_TIME                             0XE0 // default:0b1
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X64_REG_LDO_ANA_SETUP_TIME                             0X1F // default:0b0

/* page0  */
# define REG_P0_0X65                                                    0X65 // default:0x10
/* BIT7  */
# define REG_P0_0X65_TX_FSYN_ALWAYS_ON                                  0X80 // default:0b0
/* BIT6|BIT5|BIT4  */
# define REG_P0_0X65_REG_TX_RFPLL_CLOSE_TIME                            0X70 // default:0b1
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X65_REG_TX_RFPLL_SETUP_TIME                            0X0F // default:0b0

/* page0  */
# define REG_P0_0X66                                                    0X66 // default:0x64
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X66_REG_TX_ANA_SETUP_TIME                              0XFF // default:0b1100100

/* page0  */
# define REG_P0_0X67                                                    0X67 // default:0x11
/* BIT6|BIT5|BIT4  */
# define REG_P0_0X67_REG_RX_RFPLL_CLOSE_TIME                            0X70 // default:0b1
/* BIT2|BIT1|BIT0  */
# define REG_P0_0X67_REG_TX_ANA_CLOSE_TIME                              0X07 // default:0b1

/* page0  */
# define REG_P0_0X68                                                    0X68 // default:0x19
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X68_REG_RX_RFPLL_SETUP_TIME                            0X3F // default:0b11001

/* page0  */
# define REG_P0_0X69                                                    0X69 // default:0x28
/* BIT7|BIT6|BIT5  */
# define REG_P0_0X69_REG_RX_ANA_CLOSE_TIME                              0XE0 // default:0b1
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X69_REG_RX_ANA_SETUP_TIME                              0X1F // default:0b1000

/* page0  */
# define REG_P0_0X6A                                                    0X6A // default:0x00
/* BIT7  */
# define REG_P0_0X6A_REG_TX_EN_DIG                                      0X80 // default:0b0
/* BIT6  */
# define REG_P0_0X6A_REG_RX_EN_DIG                                      0X40 // default:0b0
/* BIT5  */
# define REG_P0_0X6A_REG_TX_RFPLL_EN                                    0X20 // default:0b0
/* BIT4  */
# define REG_P0_0X6A_REG_RX_RFPLL_EN                                    0X10 // default:0b0
/* BIT3  */
# define REG_P0_0X6A_REG_TX_EN_ANA                                      0X08 // default:0b0
/* BIT2  */
# define REG_P0_0X6A_REG_RX_EN_ANA                                      0X04 // default:0b0
/* BIT1  */
# define REG_P0_0X6A_REG_OSC_BUF_EN                                     0X02 // default:0b0
/* BIT0  */
# define REG_P0_0X6A_REG_LDO_ANA_EN                                     0X01 // default:0b0

/* page0  */
# define REG_P0_0X6B                                                    0X6B // default:0x00
/* BIT7  */
# define REG_P0_0X6B_REG_EN_PA                                          0X80 // default:0b0
/* BIT6  */
# define REG_P0_0X6B_REG_EN_RAMP                                        0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X6B_REG_RAMP_DLY_TIME_UP                               0X3F // default:0b0

/* page0  */
# define REG_P0_0X6C                                                    0X6C // default:0x00
/* BIT7  */
# define REG_P0_0X6C_REG_EN_PA_BUF                                      0X80 // default:0b0
/* BIT6  */
# define REG_P0_0X6C_REG_PA_OSIDE_EN                                    0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X6C_REG_PA_OSIDE_TIME                                  0X3F // default:0b0

/* page0  */
# define REG_P0_0X6D                                                    0X6D // default:0x00
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X6D_REG_PA2_DLY_TIME_DN                                0X3F // default:0b0

/* page0  */
# define REG_P0_0X6E                                                    0X6E // default:0x40
/* BIT6|BIT5|BIT4  */
# define REG_P0_0X6E_PA_RAM_DN_DLY_SEL                                  0X70 // default:0b100
/* BIT2|BIT1|BIT0  */
# define REG_P0_0X6E_PA_RAM_UP_DLY_SEL                                  0X07 // default:0b0

/* page0  */
# define REG_P0_0X6F                                                    0X6F // default:0x00
/* BIT6  */
# define REG_P0_0X6F_ENH_NOACK_RX_CONT_DIS                              0X40 // default:0b0
/* BIT5  */
# define REG_P0_0X6F_I_NDC_PREAMBLE_SEL                                 0X20 // default:0b0
/* BIT4  */
# define REG_P0_0X6F_PID_LOW_SEL                                        0X10 // default:0b0
/* BIT3  */
# define REG_P0_0X6F_IRQ_HIGH_EN                                        0X08 // default:0b0
/* BIT2|BIT1|BIT0  */
# define REG_P0_0X6F_ACK_PIPE                                           0X07 // default:0b0

/* page0  */
# define REG_P0_0X70                                                    0X70 // default:0x24
/* BIT2|BIT1|BIT0  */
# define REG_P0_0X70_I_PH_SHFT_K3_10_8                                  0X07 // default:0b100
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P0_0X70_I_PH_SHFT_K4_3_0                                   0XF0 // default:0b10

/* page0  */
# define REG_P0_0X71                                                    0X71 // default:0x5e
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X71_I_PH_SHFT_K4_10_4                                  0X7F // default:0b1011110

/* page0  */
# define REG_P0_0X72                                                    0X72 // default:0x00

/* page0  */
# define RFIRQFLG                                                       0X73 // default:0x00
/* BIT7  */
# define RFIRQFLG_TX_IRQ                                                0X80 // default:0b0
/* BIT6  */
# define RFIRQFLG_TX_MAX_RT_IRQ                                         0X40 // default:0b0
/* BIT5  */
# define RFIRQFLG_RX_ADDR_ERR_IRQ                                       0X20 // default:0b0
/* BIT4  */
# define RFIRQFLG_RX_CRC_ERR_IRQ                                        0X10 // default:0b0
/* BIT3  */
# define RFIRQFLG_RX_LENGTH_ERR_IRQ                                     0X08 // default:0b0
/* BIT2  */
# define RFIRQFLG_RX_PID_ERR_IRQ                                        0X04 // default:0b0
/* BIT1  */
# define RFIRQFLG_RX_TIMEOUT_IRQ                                        0X02 // default:0b0
/* BIT0  */
# define RFIRQFLG_RX_IRQ_                                               0X01 // default:0b0

/* page0  */
# define STATUS0                                                        0X74 // default:0x0c
/* BIT7  */
# define STATUS0_RX_CI_ERR                                              0X80 // default:0b0
/* BIT6|BIT5|BIT4  */
# define STATUS0_RX_SYNC_ADDR_2_0                                       0X70 // default:0b0
/* BIT3|BIT2  */
# define STATUS0_RX_PID                                                 0X0C // default:0b11
/* BIT1|BIT0  */
# define STATUS0_TX_PID                                                 0X03 // default:0b0

/* page0  */
# define REG_P0_0X74                                                    0X74 // default:0x00
/* BIT7  */
# define REG_P0_0X74_DCOFST_INPHASE_DC_WORD_GAIN0_8                     0X80 // default:0b0
/* BIT6  */
# define REG_P0_0X74_DCOFST_QUAD_DC_WORD_GAIN0_8                        0X40 // default:0b0
/* BIT5  */
# define REG_P0_0X74_DCOFST_INPHASE_DC_WORD_GAIN1_8                     0X20 // default:0b0
/* BIT4  */
# define REG_P0_0X74_DCOFST_QUAD_DC_WORD_GAIN1_8                        0X10 // default:0b0
/* BIT3  */
# define REG_P0_0X74_DCOFST_INPHASE_DC_WORD_GAIN2_8                     0X08 // default:0b0
/* BIT2  */
# define REG_P0_0X74_DCOFST_QUAD_DC_WORD_GAIN2_8                        0X04 // default:0b0
/* BIT1  */
# define REG_P0_0X74_DCOFST_INPHASE_DC_WORD_GAIN3_8                     0X02 // default:0b0
/* BIT0  */
# define REG_P0_0X74_DCOFST_QUAD_DC_WORD_GAIN3_8                        0X01 // default:0b0

/* page0  */
# define STATUS1                                                        0X75 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define STATUS1_RX_HEADER_7_0                                          0XFF // default:0b0

/* page0  */
# define STATUS2                                                        0X76 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define STATUS2_RX_HEADER_15_8                                         0XFF // default:0b0

/* page0  */
# define STATUS3                                                        0X77 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define STATUS3_RX_PAYLOAD_LENGTH                                      0XFF // default:0b0

/* page0  */
# define REG_P0_0X78                                                    0X78 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X78_O_AGC_SWITCH_CNT                                   0XFF // default:0b0

/* page0  */
# define REG_P0_0X79                                                    0X79 // default:0x30
/* BIT7  */
# define REG_P0_0X79_EFUSE_WR_IRQ                                       0X80 // default:0b0
/* BIT5|BIT4  */
# define REG_P0_0X79_O_GAIN_INDEX                                       0X30 // default:0b11
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X79_O_AGC_ITERATION_CNT                                0X0F // default:0b0

/* page0  */
# define REG_P0_0X7A                                                    0X7A // default:0x48
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X7A_O_RSSI_OUT_7_0                                     0XFF // default:0b1001000

/* page0  */
# define REG_P0_0X7B                                                    0X7B // default:0x02
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X7B_O_RSSI_OUT_13_8                                    0X3F // default:0b10

/* page0  */
# define REG_P0_0X7C                                                    0X7C // default:0x44
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X7C_O_RSSI_ABS_VAL_7_0                                 0XFF // default:0b1000100

/* page0  */
# define REG_P0_0X7D                                                    0X7D // default:0x3e
/* BIT7  */
# define REG_P0_0X7D_MAC_IS_IDLE                                        0X80 // default:0b0
/* BIT6  */
# define REG_P0_0X7D_DEMOD_ACK                                          0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X7D_O_RSSI_ABS_VAL_13_8                                0X3F // default:0b111110

/* page0  */
# define REG_P0_0X7E                                                    0X7E // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X7E_O_RSSI_CURRENT_7_0                                 0XFF // default:0b0

/* page0  */
# define REG_P0_0X7F                                                    0X7F // default:0x00
/* BIT7|BIT6  */
# define REG_P0_0X7F_MAC_STA                                            0XC0 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P0_0X7F_O_RSSI_CURRENT_13_8                                0X3F // default:0b0

/* page1  */
# define REG_P1_0X04                                                    0X04 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X04_EFUSE_ADDR                                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X05                                                    0X05 // default:0x01
/* BIT7|BIT6  */
# define REG_P1_0X05_RESERVE                                            0XC0 // default:0b0
/* BIT5  */
# define REG_P1_0X05_AVDD_MANUL_EN                                      0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X05_AVDD_MANUL                                         0X10 // default:0b0
/* BIT0  */
# define REG_P1_0X05_CONS_REG_EN                                        0X01 // default:0b1

/* page1  */
# define REG_P1_0X06                                                    0X06 // default:0x96
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X06_DEMOD_MAX_PEAK_TROUGH_DIFF                         0XFF // default:0b10010110

/* page1  */
# define REG_P1_0X07                                                    0X07 // default:0x75
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X07_DEMOD_UNFILT_THRESH                                0XF0 // default:0b111
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X07_DEMOD_FFE_THRESH                                   0X0F // default:0b101

/* page1  */
# define REG_P1_0X08                                                    0X08 // default:0x16
/* BIT7  */
# define REG_P1_0X08_CMPLXFLTR_EN                                       0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X08_DEMOD_DC_AVG_EN                                    0X40 // default:0b0
/* BIT5|BIT4  */
# define REG_P1_0X08_DEMOD_DC_AVG_MODE                                  0X30 // default:0b1
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X08_DEMOD_IQ_SCALE_TH0                                 0X0F // default:0b110

/* page1  */
# define REG_P1_0X09                                                    0X09 // default:0x86
/* BIT7  */
# define REG_P1_0X09_DAGC_EN                                            0X80 // default:0b1
/* BIT6  */
# define REG_P1_0X09_CE_BIT                                             0X40 // default:0b0
/* BIT5|BIT4  */
# define REG_P1_0X09_I_DATA_SEL                                         0X30 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X09_DEMOD_IQ_SCALE_DELAY                               0X0F // default:0b110

/* page1  */
# define REG_P1_0X0A                                                    0X0A // default:0xc8
/* BIT7  */
# define REG_P1_0X0A_CLR_OFFSET_EST_EN                                  0X80 // default:0b1
/* BIT6  */
# define REG_P1_0X0A_DAGC_MODE                                          0X40 // default:0b1
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X0A_DEMOD_IQ_SCALE_SWITCH_DLY                          0X3F // default:0b1000

/* page1  */
# define REG_P1_0X0B                                                    0X0B // default:0x10
/* BIT7  */
# define REG_P1_0X0B_DEMOD_EN_REDUCED_PREAMBLE                          0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X0B_DEMOD_GATE_SOFT_BITS                               0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X0B_DEMOD_BLE_LONG_RANGE                               0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X0B_DEMOD_EN_OFF_EST_TIMEOUT                           0X10 // default:0b1
/* BIT3  */
# define REG_P1_0X0B_DEMOD_CDR_INIT_OVRD                                0X08 // default:0b0
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X0B_DEMOD_CDR_COUNTER_INIT                             0X07 // default:0b0

/* page1  */
# define REG_P1_0X0C                                                    0X0C // default:0x50
/* BIT7  */
# define REG_P1_0X0C_POLYPHASE_FILTER_BYPASS                            0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X0C_DEMOD_CORRELATION_THRESHOLD                        0X7F // default:0b1010000

/* page1  */
# define REG_P1_0X0D                                                    0X0D // default:0x09
/* BIT7  */
# define REG_P1_0X0D_IQ_SCALE_GAIN_OVRD                                 0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X0D_I_PREAMBLE_PATTERN_EN                              0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X0D_DEMOD_DFE_COEFF                                    0X3F // default:0b1001

/* page1  */
# define REG_P1_0X0E                                                    0X0E // default:0x65
/* BIT7  */
# define REG_P1_0X0E_DEMOD_AGG_CDR_OVRD                                 0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X0E_DEMOD_AGGRESSIVE_CDR                               0X40 // default:0b1
/* BIT5  */
# define REG_P1_0X0E_DEMOD_EN_DRIFT_COMPENSATE                          0X20 // default:0b1
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X0E_DEMOD_PREAMBLE_DET_THRESH                          0X1F // default:0b101

/* page1  */
# define REG_P1_0X0F                                                    0X0F // default:0x8f
/* BIT7  */
# define REG_P1_0X0F_DEMOD_EN_CDR                                       0X80 // default:0b1
/* BIT6  */
# define REG_P1_0X0F_DEMOD_EN_CDR_IN_ACC_MATCH                          0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X0F_DEMOD_IQ_SWAP                                      0X20 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X0F_DEMOD_DFE_FF_COEFF                                 0X1F // default:0b1111

/* page1  */
# define REG_P1_0X10                                                    0X10 // default:0x3c
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X10_DEMOD_MAX_SIMILAR_DIFF                             0XFF // default:0b111100

/* page1  */
# define REG_P1_0X11                                                    0X11 // default:0x14
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X11_DEMOD_MIN_PEAK_TROUGH_DIFF                         0XFF // default:0b10100

/* page1  */
# define REG_P1_0X12                                                    0X12 // default:0xb2
/* BIT7|BIT6  */
# define REG_P1_0X12_DEMOD_RSSI_SRC                                     0XC0 // default:0b10
/* BIT5|BIT4  */
# define REG_P1_0X12_DEMOD_RSSI_SRC_2                                   0X30 // default:0b11
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X12_DEMOD_IQ_SCALE_VALUE                               0X0F // default:0b10

/* page1  */
# define REG_P1_0X13                                                    0X13 // default:0x05
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X13_DAGC_TARGET_POW                                    0XFF // default:0b101

/* page1  */
# define REG_P1_0X14                                                    0X14 // default:0x42
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X14_DEMOD_GAIN_SWITCH_TIMEOUT                          0XFF // default:0b1000010

/* page1  */
# define REG_P1_0X15                                                    0X15 // default:0x59
/* BIT7  */
# define REG_P1_0X15_ADC_SEL_NEG                                        0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X15_I_LONG_RANGE_CDR_EN                                0X40 // default:0b1
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X15_DEMOD_MIN_PK_TR_RDCD_PMBLE                         0X3F // default:0b11001

/* page1  */
# define REG_P1_0X16                                                    0X16 // default:0x88
/* BIT7  */
# define REG_P1_0X16_DEMOD_SBTRCT_OFST                                  0X80 // default:0b1
/* BIT6  */
# define REG_P1_0X16_I_CONSERVATIVE_CDR                                 0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X16_PREAMBLE_FND_CNT                                   0X3F // default:0b1000

/* page1  */
# define REG_P1_0X17                                                    0X17 // default:0x82
/* BIT7  */
# define REG_P1_0X17_S2S8_PREAMBLE_CORR_CNT_EN                          0X80 // default:0b1
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X17_S2S8_PREAMBLE_CORR_CNT_TH_2_0                      0X70 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X17_DEMOD_IQ_SCALE_GAIN_RGN_OVRD                       0X0F // default:0b10

/* page1  */
# define REG_P1_0X18                                                    0X18 // default:0x2e
/* BIT7  */
# define REG_P1_0X18_DEMOD_EN_WIDE_BW                                   0X80 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X18_DEMOD_WIDE_BW_A1                                   0X3F // default:0b101110

/* page1  */
# define REG_P1_0X19                                                    0X19 // default:0x07
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X19_DEMOD_WIDE_BW_A2                                   0X3F // default:0b111

/* page1  */
# define REG_P1_0X1A                                                    0X1A // default:0x10
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X1A_DAGC_GAIN_MANU                                     0XFF // default:0b10000

/* page1  */
# define REG_P1_0X1B                                                    0X1B // default:0x00
/* BIT7  */
# define REG_P1_0X1B_FSM_DCOC                                           0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X1B_FSM_BW_CAL                                         0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X1B_FSM_OFST_CAL                                       0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X1B_FSM_2P_CAL                                         0X10 // default:0b0
/* BIT3  */
# define REG_P1_0X1B_FSM_VCO_CAL                                        0X08 // default:0b0
/* BIT2  */
# define REG_P1_0X1B_I_OPEN_VFIL_SEL                                    0X04 // default:0b0
/* BIT1  */
# define REG_P1_0X1B_I_OPEN_VFIL_OVRD                                   0X02 // default:0b0
/* BIT0  */
# define REG_P1_0X1B_CAL_EN                                             0X01 // default:0b0

/* page1  */
# define REG_P1_0X1C                                                    0X1C // default:0xc0
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X1C_REF_COUNT_7_0                                      0XFF // default:0b11000000

/* page1  */
# define REG_P1_0X1D                                                    0X1D // default:0x03
/* BIT7|BIT6|BIT5  */
# define REG_P1_0X1D_WAIT_TEST_CNT_2_0                                  0XE0 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X1D_CAL_WAIT_REG_4_0                                   0X1F // default:0b11

/* page1  */
# define REG_P1_0X1E                                                    0X1E // default:0x44
/* BIT7  */
# define REG_P1_0X1E_I_OVRD_CAL_CTRL                                    0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X1E_I_OVRD_CAL_IN_6_0                                  0X7F // default:0b1000100

/* page1  */
# define REG_P1_0X1F                                                    0X1F // default:0x22
/* BIT7  */
# define REG_P1_0X1F_RXFLTR_CAL_EN                                      0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X1F_I_AGC_WORD                                         0X7F // default:0b100010

/* page1  */
# define REG_P1_0X20                                                    0X20 // default:0x88
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X20_ERROR_LIMIT_3_0                                    0XF0 // default:0b1000
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X20_VCO_CHGR4_OVRDWD                                   0X0F // default:0b1000

/* page1  */
# define REG_P1_0X21                                                    0X21 // default:0x88
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X21_VCO_CHGR3_OVRDWD                                   0XF0 // default:0b1000
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X21_VCO_CHGR2_OVRDWD                                   0X0F // default:0b1000

/* page1  */
# define REG_P1_0X22                                                    0X22 // default:0x88
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X22_VCO_CHGR1_OVRDWD                                   0XF0 // default:0b1000
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X22_VCO_CHGR0_OVRDWD                                   0X0F // default:0b1000

/* page1  */
# define REG_P1_0X23                                                    0X23 // default:0x70
/* BIT7|BIT6|BIT5  */
# define REG_P1_0X23_TP_DONE_SEL_2_0                                    0XE0 // default:0b11
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X23_TP_MANUL_CODE0                                     0X1F // default:0b10000

/* page1  */
# define REG_P1_0X24                                                    0X24 // default:0x10
/* BIT7  */
# define REG_P1_0X24_TWO_POINT_CLK_EN                                   0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X24_REG_2POINT_CAL_EN                                  0X40 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X24_TP_MANUL_CODE1                                     0X1F // default:0b10000

/* page1  */
# define REG_P1_0X25                                                    0X25 // default:0x70
/* BIT7  */
# define REG_P1_0X25_EN_TWO_POINT_CAL                                   0X80 // default:0b0
/* BIT6|BIT5  */
# define REG_P1_0X25_TP_CODE_GROUP_SEL_1_0                              0X60 // default:0b11
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X25_TP_MANUL_CODE2                                     0X1F // default:0b10000

/* page1  */
# define REG_P1_0X26                                                    0X26 // default:0x10
/* BIT7  */
# define REG_P1_0X26_TWO_POINT_SPI_TRIG                                 0X80 // default:0b0
/* BIT6|BIT5  */
# define REG_P1_0X26_GAUSS_CTRL_1_0                                     0X60 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X26_TP_MANUL_CODE3                                     0X1F // default:0b10000

/* page1  */
# define REG_P1_0X27                                                    0X27 // default:0x0a
/* BIT7|BIT6|BIT5  */
# define REG_P1_0X27_CODE_OFFSET_2_0                                    0XE0 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X27_TP_CODE_COVER_4_0                                  0X1F // default:0b1010
/* BIT7  */
# define REG_P1_0X27_HIGH_RF                                            0X80 // default:0b1

/* page1  */
# define REG_P1_0X28                                                    0X28 // default:0x00
/* BIT7  */
# define REG_P1_0X28_SD_NDIV_OVRD_SEL                                   0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X28_INT_MODE_EN                                        0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X28_SD_NDIV_OVRD_5_0                                   0X3F // default:0b0

/* page1  */
# define REG_P1_0X29                                                    0X29 // default:0x08
/* BIT7  */
# define REG_P1_0X29_VCOCALEN                                           0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X29_VCOATEN_OVRD                                       0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X29_VCOATEN_SEL                                        0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X29_VCOATCALOVRD                                       0X10 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X29_VCOATFTUNEOVRD_3_0                                 0X0F // default:0b1000

/* page1  */
# define REG_P1_0X2A                                                    0X2A // default:0x05
/* BIT7  */
# define REG_P1_0X2A_VCOATSTARTMCHCAL                                   0X80 // default:0b0
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X2A_VCOAT_CAL_MODE_2_0                                 0X70 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X2A_VCO_FREQ_COVER_3_0                                 0X0F // default:0b101

/* page1  */
# define REG_P1_0X2B                                                    0X2B // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X2B_CNTREF_MEM_DIN_7_0                                 0XFF // default:0b0

/* page1  */
# define REG_P1_0X2C                                                    0X2C // default:0x00
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X2C_CNTREF_MEM_ADDR_2_0                                0X70 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X2C_CNTREF_MEM_DIN_11_8                                0X0F // default:0b0

/* page1  */
# define REG_P1_0X2D                                                    0X2D // default:0x20
/* BIT7|BIT6  */
# define REG_P1_0X2D_VCO_READY_SEL_1_0                                  0XC0 // default:0b0
/* BIT5|BIT4  */
# define REG_P1_0X2D_I_VCO_DLY_SEL_1_0                                  0X30 // default:0b10
/* BIT3  */
# define REG_P1_0X2D_REG_VCO_CAL_EN                                     0X08 // default:0b0
/* BIT1  */
# define REG_P1_0X2D_VCOAT_CHGROVRD_SEL                                 0X02 // default:0b0
/* BIT0  */
# define REG_P1_0X2D_CNTREF_MEM_WRE                                     0X01 // default:0b0

/* page1  */
# define REG_P1_0X2E                                                    0X2E // default:0xd2
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X2E_VCOATCNTREFOVRDWD_7_0                              0XFF // default:0b11010010

/* page1  */
# define REG_P1_0X2F                                                    0X2F // default:0x05
/* BIT6  */
# define REG_P1_0X2F_VCORXMODE_OVRD                                     0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X2F_VCORXMODE_SEL                                      0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X2F_VCOATMCHCALOVRD                                    0X10 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X2F_VCOATCNTREFOVRDWD_11_8                             0X0F // default:0b101

/* page1  */
# define REG_P1_0X30                                                    0X30 // default:0x4f
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X30_VCOMXCNT_7_0                                       0XFF // default:0b1001111

/* page1  */
# define REG_P1_0X31                                                    0X31 // default:0x01
/* BIT7  */
# define REG_P1_0X31_SDEN                                               0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X31_CALDIVEN                                           0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X31_FBDIVEN                                            0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X31_LFDACEN                                            0X10 // default:0b0
/* BIT0  */
# define REG_P1_0X31_SDTXIN_POL                                         0X01 // default:0b1

/* page1  */
# define REG_P1_0X32                                                    0X32 // default:0x18
/* BIT7|BIT6  */
# define REG_P1_0X32_INBAND_DELAY_1_0                                   0XC0 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X32_GAUSS_SCALE                                        0X1F // default:0b11000

/* page1  */
# define REG_P1_0X33                                                    0X33 // default:0x1b
/* BIT7|BIT6  */
# define REG_P1_0X33_OUTBAND_DELAY_1_0                                  0XC0 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X33_DF_SEL                                             0X3F // default:0b11011

/* page1  */
# define REG_P1_0X34                                                    0X34 // default:0xa0
/* BIT7  */
# define REG_P1_0X34_CLK_EN                                             0X80 // default:0b1
/* BIT6  */
# define REG_P1_0X34_DIV2_EN                                            0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X34_DAC_BASAL_INBAND                                   0X3F // default:0b100000

/* page1  */
# define REG_P1_0X35                                                    0X35 // default:0x20
/* BIT7  */
# define REG_P1_0X35_PHASE_ADJ                                          0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X35_DAC_MODE                                           0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X35_DAC_BASAL_OUTBAND                                  0X3F // default:0b100000

/* page1  */
# define REG_P1_0X36                                                    0X36 // default:0x0b
/* BIT7  */
# define REG_P1_0X36_DS_SHIFT                                           0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X36_INV_CLK_EN                                         0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X36_MASH2_MODE                                         0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X36_SHIFT_OFFSET                                       0X10 // default:0b0
/* BIT3  */
# define REG_P1_0X36_CTL_DITHER_SHAPE                                   0X08 // default:0b1
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X36_CTL_DUTHER_LSB                                     0X07 // default:0b11

/* page1  */
# define REG_P1_0X37                                                    0X37 // default:0x13
/* BIT7|BIT6|BIT5  */
# define REG_P1_0X37_EN_RXLNA_CG                                        0XE0 // default:0b0
/* BIT4  */
# define REG_P1_0X37_EN_RXOFSTCMP                                       0X10 // default:0b1
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X37_RXLNA_ICORE                                        0X0F // default:0b11

/* page1  */
# define REG_P1_0X38                                                    0X38 // default:0x1c
/* BIT7|BIT6|BIT5  */
# define REG_P1_0X38_RXFLTR_IF                                          0XE0 // default:0b0
/* BIT4  */
# define REG_P1_0X38_EN_RXADC_VCM                                       0X10 // default:0b1
/* BIT3  */
# define REG_P1_0X38_EN_RXMXRVCM                                        0X08 // default:0b1
/* BIT2  */
# define REG_P1_0X38_RXIQ_SEL                                           0X04 // default:0b1
/* BIT0  */
# define REG_P1_0X38_TST_RX_BPF                                         0X01 // default:0b0

/* page1  */
# define REG_P1_0X39                                                    0X39 // default:0xbb
/* BIT7  */
# define REG_P1_0X39_EN_RXFLTR                                          0X80 // default:0b1
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X39_RXFLTR_IBIAS1                                      0X70 // default:0b11
/* BIT3  */
# define REG_P1_0X39_RXFLTR_IPOLYPRB                                    0X08 // default:0b1
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X39_RXFLTR_IBIAS2                                      0X07 // default:0b11

/* page1  */
# define REG_P1_0X3A                                                    0X3A // default:0x13
/* BIT7|BIT6  */
# define REG_P1_0X3A_RXFLTRTUNE_OFST                                    0XC0 // default:0b0
/* BIT5|BIT4  */
# define REG_P1_0X3A_RXFLTR_GAIN2B                                      0X30 // default:0b1
/* BIT3  */
# define REG_P1_0X3A_RXFLTR_GAIN2A                                      0X08 // default:0b0
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X3A_RXFLTR_VCM                                         0X07 // default:0b11

/* page1  */
# define REG_P1_0X3B                                                    0X3B // default:0x6a
/* BIT7  */
# define REG_P1_0X3B_EN_RXFLTRTNR                                       0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X3B_EN_RXLNA                                           0X40 // default:0b1
/* BIT5|BIT4  */
# define REG_P1_0X3B_RXOFSTCMP_TRIM                                     0X30 // default:0b10
/* BIT3  */
# define REG_P1_0X3B_EN_RXMXR                                           0X08 // default:0b1
/* BIT2  */
# define REG_P1_0X3B_EN_RXADC_BYP                                       0X04 // default:0b0
/* BIT1  */
# define REG_P1_0X3B_EN_RXADC                                           0X02 // default:0b1
/* BIT0  */
# define REG_P1_0X3B_RXADCTEST_MODE                                     0X01 // default:0b0

/* page1  */
# define REG_P1_0X3C                                                    0X3C // default:0x17
/* BIT7  */
# define REG_P1_0X3C_TXPA_POUT_CRNT                                     0X80 // default:0b0
/* BIT4  */
# define REG_P1_0X3C_TX_DCC_ISEL                                        0X10 // default:0b1
/* BIT3  */
# define REG_P1_0X3C_LDO_PA_BYPASS_EN                                   0X08 // default:0b0
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X3C_TXPA_POUT_RES                                      0X07 // default:0b111

/* page1  */
# define REG_P1_0X3D                                                    0X3D // default:0x03
/* BIT7  */
# define REG_P1_0X3D_EN_TXPA_DCC                                        0X80 // default:0b0
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X3D_TXPA_MODE_SEL                                      0X70 // default:0b0
/* BIT3  */
# define REG_P1_0X3D_TXPA_RAMP_TIME                                     0X08 // default:0b0
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X3D_TXPA_DUTY                                          0X07 // default:0b11

/* page1  */
# define REG_P1_0X3E                                                    0X3E // default:0xf5
/* BIT4  */
# define REG_P1_0X3E_EN_FSYNPFD                                         0X10 // default:0b1
/* BIT3|BIT2  */
# define REG_P1_0X3E_FSYNXO_HYS                                         0X0C // default:0b1
/* BIT1|BIT0  */
# define REG_P1_0X3E_FSYNPFD_DELAY                                      0X03 // default:0b1

/* page1  */
# define REG_P1_0X3F                                                    0X3F // default:0xd4
/* BIT7  */
# define REG_P1_0X3F_EN_FSYNCHP                                         0X80 // default:0b1
/* BIT6  */
# define REG_P1_0X3F_EN_FSYNCHP_SHIFT                                   0X40 // default:0b1
/* BIT4  */
# define REG_P1_0X3F_EN_FSYNCP_FV                                       0X10 // default:0b1
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X3F_FSYNCHP_IOUT                                       0X0F // default:0b100

/* page1  */
# define REG_P1_0X40                                                    0X40 // default:0x28
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X40_FSYNCHP_NSHIFT                                     0X70 // default:0b10
/* BIT3  */
# define REG_P1_0X40_FSYNLPF_C3                                         0X08 // default:0b1
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X40_FSYNCHP_PSHIFT                                     0X07 // default:0b0

/* page1  */
# define REG_P1_0X41                                                    0X41 // default:0x91
/* BIT7  */
# define REG_P1_0X41_EN_FSYNLPF                                         0X80 // default:0b1
/* BIT6  */
# define REG_P1_0X41_EN_FSYNLPF_VCDN                                    0X40 // default:0b0
/* BIT5|BIT4  */
# define REG_P1_0X41_FSYNLPF_DAC_VSEL                                   0X30 // default:0b1
/* BIT3  */
# define REG_P1_0X41_FSYNLPF_BYP_FT                                     0X08 // default:0b0
/* BIT2  */
# define REG_P1_0X41_FSYNLPF_R3                                         0X04 // default:0b0
/* BIT1|BIT0  */
# define REG_P1_0X41_FSYNLPF_R1                                         0X03 // default:0b1

/* page1  */
# define REG_P1_0X42                                                    0X42 // default:0xa4
/* BIT7  */
# define REG_P1_0X42_EN_FSYNVCO                                         0X80 // default:0b1
/* BIT5|BIT4  */
# define REG_P1_0X42_FSYNVCO_VD_SW                                      0X30 // default:0b10
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X42_FSYNVCO_ICORE                                      0X0F // default:0b100

/* page1  */
# define REG_P1_0X43                                                    0X43 // default:0x12
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X43_FSYNVCO_IPOLYIPTAT_IBIAS                           0XF0 // default:0b1
/* BIT3  */
# define REG_P1_0X43_FSYNVCO_TXCTK                                      0X08 // default:0b0
/* BIT1|BIT0  */
# define REG_P1_0X43_FSYNVCO_FC_TUNE                                    0X03 // default:0b10

/* page1  */
# define REG_P1_0X44                                                    0X44 // default:0xcc
/* BIT7  */
# define REG_P1_0X44_EN_FSYNIQDIV                                       0X80 // default:0b1
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X44_FSYNIQDIV_IBIASN                                   0X70 // default:0b100
/* BIT3  */
# define REG_P1_0X44_EN_FSYNIQDIV_PAOUT                                 0X08 // default:0b1
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X44_FSYNIQDIV_IBIASP                                   0X07 // default:0b100

/* page1  */
# define REG_P1_0X45                                                    0X45 // default:0xe0
/* BIT4  */
# define REG_P1_0X45_EN_FSYNFBDIV_TST                                   0X10 // default:0b0
/* BIT3  */
# define REG_P1_0X45_EN_FSYNCALDIV_TST                                  0X08 // default:0b0
/* BIT1|BIT0  */
# define REG_P1_0X45_FSYNCALDIV_CALCLK                                  0X03 // default:0b0

/* page1  */
# define REG_P1_0X46                                                    0X46 // default:0xbd
/* BIT7  */
# define REG_P1_0X46_EN_RFMTCHNTWKPA                                    0X80 // default:0b1
/* BIT6  */
# define REG_P1_0X46_RFMTCHNTWK_C2VAL_RX                                0X40 // default:0b0
/* BIT5|BIT4  */
# define REG_P1_0X46_RFMTCHNTWK_C1VAL_RX                                0X30 // default:0b11
/* BIT3|BIT2  */
# define REG_P1_0X46_RFMTCHNTWK_C1VAL_TX                                0X0C // default:0b11
/* BIT1  */
# define REG_P1_0X46_EN_RFMTCHNTWK_DCOC                                 0X02 // default:0b0
/* BIT0  */
# define REG_P1_0X46_RFMTCHNTWK_C2VAL_TX                                0X01 // default:0b1

/* page1  */
# define REG_P1_0X47                                                    0X47 // default:0xb3
/* BIT7  */
# define REG_P1_0X47_EN_IPOLY                                           0X80 // default:0b1
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X47_IPOLY_TRIM                                         0X70 // default:0b11

/* page1  */
# define REG_P1_0X48                                                    0X48 // default:0x88
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X48_LDO_ANA_TRIM                                       0XF0 // default:0b1000
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X48_LDO_RFFE_TRIM                                      0X0F // default:0b1000

/* page1  */
# define REG_P1_0X49                                                    0X49 // default:0xc4
/* BIT7  */
# define REG_P1_0X49_TX_DAC_GC                                          0X80 // default:0b1
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X49_TX_DAC_ISEL                                        0X70 // default:0b100
/* BIT3  */
# define REG_P1_0X49_TX_DAC_CLKINV                                      0X08 // default:0b0
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X49_TX_DAC_VSEL                                        0X07 // default:0b100

/* page1  */
# define REG_P1_0X4A                                                    0X4A // default:0x05
/* BIT7  */
# define REG_P1_0X4A_FSYNXO_AMPSEL                                      0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X4A_RXADC_CLKINV                                       0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X4A_FSYNXO_RES                                         0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X4A_TST_FSYNXO_CLK                                     0X10 // default:0b0
/* BIT3  */
# define REG_P1_0X4A_FSYNXO_STARTUP_COUNTER                             0X08 // default:0b0
/* BIT2  */
# define REG_P1_0X4A_EN_FSYNXO_DEGLITCH                                 0X04 // default:0b1
/* BIT1  */
# define REG_P1_0X4A_FSYNXO_FAST_DLY                                    0X02 // default:0b0
/* BIT0  */
# define REG_P1_0X4A_FSYNXO_FB                                          0X01 // default:0b1

/* page1  */
# define REG_P1_0X4B                                                    0X4B // default:0xa0
/* BIT7|BIT6  */
# define REG_P1_0X4B_FSYNXO_ICORE                                       0XC0 // default:0b10
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X4B_FSYNXO_CAPSEL                                      0X3F // default:0b100000

/* page1  */
# define REG_P1_0X4C                                                    0X4C // default:0x78
/* BIT7  */
# define REG_P1_0X4C_FSYNXO_CAP2                                        0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X4C_EN_TX_DAC                                          0X40 // default:0b1
/* BIT5  */
# define REG_P1_0X4C_TX_DAC_BW                                          0X20 // default:0b1
/* BIT4  */
# define REG_P1_0X4C_FSYNXO_STARTUP_FAST                                0X10 // default:0b1
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X4C_LDO_HP_TRIM                                        0X0F // default:0b1000

/* page1  */
# define REG_P1_0X4D                                                    0X4D // default:0x00
/* BIT7  */
# define REG_P1_0X4D_EN_DFT_V                                           0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X4D_TST_LDO_OUT                                        0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X4D_TST_DAC                                            0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X4D_DFT_V_BYPASS                                       0X10 // default:0b0
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X4D_DFT_V_SEL                                          0X0F // default:0b0

/* page1  */
# define REG_P1_0X4E                                                    0X4E // default:0x30
/* BIT7  */
# define REG_P1_0X4E_I_LOAD_OVRD                                        0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X4E_I_FORCE_GAIN                                       0X40 // default:0b0
/* BIT5|BIT4  */
# define REG_P1_0X4E_I_SINGLE_REGION_INDEX                              0X30 // default:0b11
/* BIT3  */
# define REG_P1_0X4E_AUTO_CAL_EN                                        0X08 // default:0b0
/* BIT2  */
# define REG_P1_0X4E_RXDCOFSTCALEN                                      0X04 // default:0b0
/* BIT1  */
# define REG_P1_0X4E_I_SINGLE_REGION                                    0X02 // default:0b0
/* BIT0  */
# define REG_P1_0X4E_I_START_MULTI_REGION                               0X01 // default:0b0

/* page1  */
# define REG_P1_0X4F                                                    0X4F // default:0x40
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X4F_WAIT_TIME_7_0                                      0XFF // default:0b1000000

/* page1  */
# define REG_P1_0X50                                                    0X50 // default:0x8e
/* BIT7  */
# define REG_P1_0X50_I_TUNER_MODE                                       0X80 // default:0b1
/* BIT5  */
# define REG_P1_0X50_I_LOAD_RESID_OVRD                                  0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X50_I_READ_RESID_DCOC                                  0X10 // default:0b0
/* BIT3  */
# define REG_P1_0X50_I_ADC_OP_AVG_EN                                    0X08 // default:0b1
/* BIT2|BIT1  */
# define REG_P1_0X50_I_ADC_OP_AVG_MODE                                  0X06 // default:0b11
/* BIT0  */
# define REG_P1_0X50_WAIT_TIME_8                                        0X01 // default:0b0

/* page1  */
# define REG_P1_0X51                                                    0X51 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X51_I_INPHASE_DC_WORD_GAIN0_OVRD_7_0                   0XFF // default:0b0

/* page1  */
# define REG_P1_0X52                                                    0X52 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X52_I_QUAD_DC_WORD_GAIN0_OVRD_7_0                      0XFF // default:0b0

/* page1  */
# define REG_P1_0X53                                                    0X53 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X53_I_INPHASE_DC_WORD_GAIN1_OVRD_7_0                   0XFF // default:0b0

/* page1  */
# define REG_P1_0X54                                                    0X54 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X54_I_QUAD_DC_WORD_GAIN1_OVRD_7_0                      0XFF // default:0b0

/* page1  */
# define REG_P1_0X55                                                    0X55 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X55_I_INPHASE_DC_WORD_GAIN2_OVRD_7_0                   0XFF // default:0b0

/* page1  */
# define REG_P1_0X56                                                    0X56 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X56_I_QUAD_DC_WORD_GAIN2_OVRD_7_0                      0XFF // default:0b0

/* page1  */
# define REG_P1_0X57                                                    0X57 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X57_I_INPHASE_DC_WORD_GAIN3_OVRD_7_0                   0XFF // default:0b0

/* page1  */
# define REG_P1_0X58                                                    0X58 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X58_I_QUAD_DC_WORD_GAIN3_OVRD_7_0                      0XFF // default:0b0

/* page1  */
# define REG_P1_0X59                                                    0X59 // default:0x00
/* BIT7  */
# define REG_P1_0X59_I_INPHASE_DC_WORD_GAIN0_OVRD_8                     0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X59_I_QUAD_DC_WORD_GAIN0_OVRD_8                        0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X59_I_INPHASE_DC_WORD_GAIN1_OVRD_8                     0X20 // default:0b0
/* BIT4  */
# define REG_P1_0X59_I_QUAD_DC_WORD_GAIN1_OVRD_8                        0X10 // default:0b0
/* BIT3  */
# define REG_P1_0X59_I_INPHASE_DC_WORD_GAIN2_OVRD_8                     0X08 // default:0b0
/* BIT2  */
# define REG_P1_0X59_I_QUAD_DC_WORD_GAIN2_OVRD_8                        0X04 // default:0b0
/* BIT1  */
# define REG_P1_0X59_I_INPHASE_DC_WORD_GAIN3_OVRD_8                     0X02 // default:0b0
/* BIT0  */
# define REG_P1_0X59_I_QUAD_DC_WORD_GAIN3_OVRD                          0X01 // default:0b0

/* page1  */
# define REG_P1_0X5A                                                    0X5A // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X5A_I_INPH_RESIDUAL_GAIN0_OVRD                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X5B                                                    0X5B // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X5B_I_QUAD_RESIDUAL_GAIN0_OVRD                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X5C                                                    0X5C // default:0xf0
/* BIT7  */
# define REG_P1_0X5C_I_CDR_INIT_EN                                      0X80 // default:0b1
/* BIT6|BIT5|BIT4  */
# define REG_P1_0X5C_I_CDR_EARLY                                        0X70 // default:0b111
/* BIT3  */
# define REG_P1_0X5C_I_S2S8_PREAMBLE_CHIP_CNT_TH_SEL                    0X08 // default:0b0
/* BIT2  */
# define REG_P1_0X5C_I_LQI_OFFSET_EN                                    0X04 // default:0b0
/* BIT1  */
# define REG_P1_0X5C_I_LQI_ERR_FLAG_EN                                  0X02 // default:0b0
/* BIT0  */
# define REG_P1_0X5C_I_LQI_EN                                           0X01 // default:0b0

/* page1  */
# define REG_P1_0X5D                                                    0X5D // default:0x20
/* BIT6  */
# define REG_P1_0X5D_I_SAMP_MID_ACTION_EN                               0X40 // default:0b0
/* BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X5D_I_LQI_LENGTH                                       0X3F // default:0b100000

/* page1  */
# define REG_P1_0X5E                                                    0X5E // default:0x64
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X5E_I_LQI_INNER                                        0XFF // default:0b1100100

/* page1  */
# define REG_P1_0X60                                                    0X60 // default:0xc8
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X60_I_LQI_THRESHOLD                                    0XFF // default:0b11001000

/* page1  */
# define REG_P1_0X61                                                    0X61 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X61_I_INPH_RESIDUAL_GAIN1_OVRD                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X62                                                    0X62 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X62_I_QUAD_RESIDUAL_GAIN1_OVRD                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X63                                                    0X63 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X63_I_INPH_RESIDUAL_GAIN2_OVRD                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X64                                                    0X64 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X64_I_QUAD_RESIDUAL_GAIN2_OVRD                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X65                                                    0X65 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X65_I_INPH_RESIDUAL_GAIN3_OVRD                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X66                                                    0X66 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X66_I_QUAD_RESIDUAL_GAIN3_OVRD                         0XFF // default:0b0

/* page1  */
# define REG_P1_0X67                                                    0X67 // default:0xe2
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X67_I_PH_SHFT_K1_7_0                                   0XFF // default:0b11100010

/* page1  */
# define REG_P1_0X68                                                    0X68 // default:0x45
/* BIT2|BIT1|BIT0  */
# define REG_P1_0X68_I_PH_SHFT_K1_10_8                                  0X07 // default:0b101
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X68_I_PH_SHFT_K2_3_0                                   0XF0 // default:0b100

/* page1  */
# define REG_P1_0X69                                                    0X69 // default:0x36
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X69_I_PH_SHFT_K2_10_4                                  0X7F // default:0b110110

/* page1  */
# define REG_P1_0X6A                                                    0X6A // default:0x9c
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X6A_I_PH_SHFT_K3_7_0                                   0XFF // default:0b10011100

/* page1  */
# define REG_P1_0X6B                                                    0X6B // default:0x40
/* BIT7  */
# define REG_P1_0X6B_RXFIL_CAL_DONE_1                                   0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X6B_RXFIL_CAL_OUT_1                                    0X7F // default:0b1000000

/* page1  */
# define REG_P1_0X6C                                                    0X6C // default:0x40
/* BIT7  */
# define REG_P1_0X6C_RXFIL_CAL_DONE_2                                   0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X6C_RXFIL_CAL_OUT_2                                    0X7F // default:0b1000000

/* page1  */
# define REG_P1_0X6D                                                    0X6D // default:0x40
/* BIT7  */
# define REG_P1_0X6D_RXFIL_CAL_DONE_3                                   0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X6D_RXFIL_CAL_OUT_3                                    0X7F // default:0b1000000

/* page1  */
# define REG_P1_0X6E                                                    0X6E // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X6E_SCOUNTER_7_0                                       0XFF // default:0b0

/* page1  */
# define REG_P1_0X6F                                                    0X6F // default:0x64
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X6F_I_LQI_OUTER                                        0XFF // default:0b1100100
/* BIT7  */
# define REG_P1_0X6F_XO_CLK_RDY                                         0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2  */
# define REG_P1_0X6F_TWO_POINT_CODE_OUT                                 0X7C // default:0b10000
/* BIT1|BIT0  */
# define REG_P1_0X6F_SCOUNTER_9_8                                       0X03 // default:0b0
/* BIT7  */
# define REG_P1_0X6F_XO_CLK_RDY                                         0X80 // default:0b0
/* BIT6|BIT5|BIT4|BIT3|BIT2  */
# define REG_P1_0X6F_TWO_POINT_CODE_OUT                                 0X7C // default:0b10000

/* page1  */
# define REG_P1_0X70                                                    0X70 // default:0x10
/* BIT7  */
# define REG_P1_0X70_TWO_POINT_CAL_DONE                                 0X80 // default:0b0
/* BIT6  */
# define REG_P1_0X70_VCOAT_MCHCALDONE                                   0X40 // default:0b0
/* BIT5  */
# define REG_P1_0X70_VCO_READY                                          0X20 // default:0b0
/* BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X70_TWO_POINT_CODE                                     0X1F // default:0b10000

/* page1  */
# define REG_P1_0X71                                                    0X71 // default:0x88
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X71_VCO_CODE                                           0XF0 // default:0b1000
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X71_VCOAT_GRP4FTUNE                                    0X0F // default:0b1000

/* page1  */
# define REG_P1_0X72                                                    0X72 // default:0x88
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X72_VCOAT_GRP3FTUNE                                    0XF0 // default:0b1000
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X72_VCOAT_GRP2FTUNE                                    0X0F // default:0b1000

/* page1  */
# define REG_P1_0X73                                                    0X73 // default:0x88
/* BIT7|BIT6|BIT5|BIT4  */
# define REG_P1_0X73_VCOAT_GRP1FTUNE                                    0XF0 // default:0b1000
/* BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X73_VCOAT_GRP0FTUNE                                    0X0F // default:0b1000

/* page1  */
# define REG_P1_0X75                                                    0X75 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X75_DCOFST_INPHASE_DC_WORD_GAIN0_7_0                   0XFF // default:0b0

/* page1  */
# define REG_P1_0X76                                                    0X76 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X76_DCOFST_QUAD_DC_WORD_GAIN0_7_0                      0XFF // default:0b0

/* page1  */
# define REG_P1_0X77                                                    0X77 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X77_DCOFST_INPHASE_DC_WORD_GAIN1_7_0                   0XFF // default:0b0

/* page1  */
# define REG_P1_0X78                                                    0X78 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X78_DCOFST_QUAD_DC_WORD_GAIN1_7_0                      0XFF // default:0b0

/* page1  */
# define REG_P1_0X79                                                    0X79 // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X79_DCOFST_INPHASE_DC_WORD_GAIN2_7_0                   0XFF // default:0b0

/* page1  */
# define REG_P1_0X7A                                                    0X7A // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X7A_DCOFST_QUAD_DC_WORD_GAIN2_7_0                      0XFF // default:0b0

/* page1  */
# define REG_P1_0X7B                                                    0X7B // default:0x00
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X7B_DCOFST_INPHASE_DC_WORD_GAIN3_7_0                   0X7F // default:0b0

/* page1  */
# define REG_P1_0X7C                                                    0X7C // default:0x00
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X7C_DCOFST_QUAD_DC_WORD_GAIN3_7_0                      0X7F // default:0b0

/* page1  */
# define REG_P1_0X7D                                                    0X7D // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1  */
# define REG_P1_0X7D_O_INPH_RESIDUAL_GAINX_7_1                          0XFE // default:0b0
/* BIT0  */
# define REG_P1_0X7D_I_FILQ_OUT_R                                       0X01 // default:0b0

/* page1  */
# define REG_P1_0X7E                                                    0X7E // default:0x00
/* BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X7E_O_QUAD_RESIDUAL_GAINX_7_1                          0XFF // default:0b0
/* BIT0  */
# define REG_P1_0X7E_I_FILI_OUT_R                                       0X01 // default:0b0

/* page1  */
# define REG_P1_0X7F                                                    0X7F // default:0xc8
/* BIT7  */
# define REG_P1_0X7F_REGION_BIN_SRCH_DONE                               0X80 // default:0b1
/* BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0  */
# define REG_P1_0X7F_O_LQI_ST                                           0X7F // default:0b0



uint8_t __ctz(uint8_t val);
PAN216_Err_t PAN216_SetPage(uint8_t page);
uint8_t PAN216_ReadPageReg(uint8_t page, uint8_t addr);
PAN216_Err_t PAN216_WritePageReg(uint8_t page, uint8_t addr, uint8_t value);
PAN216_Err_t PAN216_WriteReg(uint8_t addr, uint8_t value);
PAN216_Err_t PAN216_WriteRegUnchecked(uint8_t addr, uint8_t value);
PAN216_Err_t PAN216_WriteRegs(uint8_t addr, uint8_t *buffer, uint8_t len);
PAN216_Err_t PAN216_WriteRegBits(u8 addr, u8 val, u8 mask);
uint8_t PAN216_ReadReg(uint8_t addr);
PAN216_Err_t PAN216_WriteFifo(uint8_t addr, uint8_t *buffer, uint8_t size);
PAN216_Err_t PAN216_ReadFifo(uint8_t addr, uint8_t *buffer, uint8_t size);
PAN216_Err_t PAN216_WritePageRegs(uint8_t page, uint8_t addr, uint8_t *buffer, uint8_t len);
PAN216_Err_t PAN216_ReadPageRegs(uint8_t page, uint8_t addr, uint8_t *buffer, uint8_t len);
PAN216_Err_t PAN216_WriteRegWithMask(uint8_t page, uint8_t addr, uint8_t mask, uint8_t value);
uint8_t PAN216_ReadRegWithMask(uint8_t page, uint8_t addr, uint8_t mask);
PAN216_Err_t PAN216_WritePageRegUnchecked(uint8_t page, uint8_t addr, uint8_t value);
PAN216_Err_t PAN216_SetRegBits(uint8_t page, uint8_t addr, uint8_t bit_mask);
PAN216_Err_t PAN216_ResetRegBits(uint8_t page, uint8_t addr, uint8_t bit_mask);
PAN216_Err_t PAN216_WritePageRegBits(u8 page, u8 addr, u8 val, u8 mask);
PAN216_Err_t PAN216_ReadRegs(uint8_t addr,uint8_t *buffer,uint8_t len);

#endif
