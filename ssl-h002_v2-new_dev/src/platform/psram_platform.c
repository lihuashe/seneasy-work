#include <stddef.h>
#include "psram_platform.h"
#include "rtl876x.h"
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "flash_device.h"
#include "trace.h"

#define SPIC_FIFO_SIZE             256
#define PSRAM_MAGIC_PATTERN        0x5A5A12A5
#define PSRAM_CAL_ADDR             0x6802030  //0x7002030
#define DELAY_CNT_MAX              32
#define FMC_MAIN1                  0x6800000

/** @defgroup FLASH_DRIVER_Registers_Definitions Flash Driver Registers Definitions
  * @brief
  * @{
  */
/*****************************************************************************************
* Registers Definitions --------------------------------------------------------*
********************  Bits definition for SPIC_CTRLR0 register  *******************/
#define BIT_CK_MTIMES(x)        (((x) & 0x0000001F) << 23)
#define BIT_FAST_RD(x)          (((x) & 0x00000001) << 22)
#define BIT_CMD_CH(x)           (((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)          (((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)          (((x) & 0x00000003) << 16)
#define BIT_TMOD(x)             (((x) & 0x00000003) << 8)
#define BIT_SCPOL               (0x00000001 << 7)
#define BIT_SCPH                (0x00000001 << 6)

/********************  Bits definition for SPIC_CTRLR1 register  *******************/
#define BIT_NDF(x)                  ((x) & 0xfffff)

/********************  Bits definition for SPIC_SSIENR register  *******************/
#define BIT_ATCK_CMD                (0x00000001 << 1)
#define BIT_SPIC_EN             (0x00000001)

/********************  Bits definition for SPIC_BAUDR register  *******************/
#define BIT_SCKDV(x)                ((x) & 0x0fff)

/********************  Bits definition for SPIC_SR register  *******************/
#define BIT_TXE                 (0x00000001 << 5)
#define BIT_RFF                 (0x00000001 << 4)
#define BIT_RFNE                (0x00000001 << 3)
#define BIT_TFE                 (0x00000001 << 2)
#define BIT_TFNF                    (0x00000001 << 1)
#define BIT_BUSY                    (0x00000001)

/********************  Bits definition for SPIC_IMR register  *******************/
#define BIT_ACSIM                               (0x00000001 << 11)
#define BIT_RXSIM                               (0x00000001 << 10)
#define BIT_TXSIM                               (0x00000001 << 9)
#define BIT_ACEIM                               (0x00000001 << 8)
#define BIT_BYEIM                               (0x00000001 << 7)
#define BIT_WBEIM                               (0x00000001 << 6)
#define BIT_FSEIM                               (0x00000001 << 5)
#define BIT_RXFIM                               (0x00000001 << 4)
#define BIT_RXOIM                               (0x00000001 << 3)
#define BIT_RXUIM                               (0x00000001 << 2)
#define BIT_TXOIM                               (0x00000001 << 1)
#define BIT_TXEIM                               (0x00000001)

/********************  Bits definition for SPIC_ISR register  *******************/
#define BIT_ACSIS                               (0x00000001 << 11)
#define BIT_RXSIS                               (0x00000001 << 10)
#define BIT_TXSIS                               (0x00000001 << 9)
#define BIT_ACEIS                               (0x00000001 << 8)
#define BIT_BYEIS                               (0x00000001 << 7)
#define BIT_WBEIS                               (0x00000001 << 6)
#define BIT_FSEIS                               (0x00000001 << 5)
#define BIT_RXFIS                               (0x00000001 << 4)
#define BIT_RXOIS                               (0x00000001 << 3)
#define BIT_RXUIS                               (0x00000001 << 2)
#define BIT_TXOIS                               (0x00000001 << 1)
#define BIT_TXEIS                               (0x00000001)

/********************  Bits definition for SPIC_RISR register  *******************/
#define BIT_ACEIR                               (0x00000001 << 8)
#define BIT_BYEIR                               (0x00000001 << 7)
#define BIT_WBEIR                               (0x00000001 << 6)
#define BIT_FSEIR                               (0x00000001 << 5)
#define BIT_RXFIR                               (0x00000001 << 4)
#define BIT_RXOIR                               (0x00000001 << 3)
#define BIT_RXUIR                               (0x00000001 << 2)
#define BIT_TXOIR                               (0x00000001 << 1)
#define BIT_TXEIR                               (0x00000001)

/********************  Bits definition for SPIC_CTRLR2 register  *******************/
#define BIT_RX_FIFO_ENTRY(x)           (((x) & 0x0000000f) << 8)
#define BIT_FIFO_ENTRY(x)           (((x) & 0x0000000f) << 4)
#define BIT_SEQ_EN              (0x00000001 << 3)
/* Indicate the WPn input pin of SPI Flash is connected to:
   0(default): WP=spi_sout[2], 1:WP=spi_sout[3]. */
#define BIT_WPN_DNUM            (0x00000001 << 2)
/*write protect function.*/
#define BIT_WPN_SET             (0x00000001 << 1)
/* SO pin of SPI Flash.0: SO connects to spi_sout[0]. 1(default): SO connects to spi_sout[1].*/
#define BIT_SO_DUM              (0x00000001)

/********************  Bits definition for SPIC_ADDR_LENGTH register  *******************/
#define BIT_ADDR_PHASE_LENGTH(x)    ((x) & 0x00000003)

/********************  Bits definition for SPIC_AUTO_LENGTH register  *******************/
#define BIT_CS_H_WR_DUM_LEN(x)  (((x) & 0x0000000f) << 28)
#define BIT_CS_H_RD_DUM_LEN(x)  (((x) & 0x00000003) << 26)
#define BIT_AUTO_DUM_LEN(x)     (((x) & 0x000000ff) << 18)
#define BIT_AUTO_ADDR_LENGTH(x) (((x) & 0x00000003) << 16)
#define BIT_IN_PHYSICAL_CYC(x)  (((x) & 0x0000000f) << 12)
#define BIT_RD_DUMMY_LENGTH(x)  (((x) & 0x00000fff))

/********************  Bits definition for SPIC_VALID_CMD register  *******************/
#define BIT_SEQ_TRANS_EN        (0x00000001 << 14)
#define BIT_CTRLR0_CH           (0x00000001 << 12)
#define BIT_PRM_EN              (0x00000001 << 11)
#define BIT_WR_AUTOCHECKSTATUS  (0x00000001 << 10)
#define BIT_WR_BLOCKING         (0x00000001 << 9)
#define BIT_WR_QUAD_II          (0x00000001 << 8)
#define BIT_WR_QUAD_I           (0x00000001 << 7)
#define BIT_WR_DUAL_II          (0x00000001 << 6)
#define BIT_WR_DUAL_I           (0x00000001 << 5)
#define BIT_RD_QUAD_IO          (0x00000001 << 4)
#define BIT_RD_QUAD_O           (0x00000001 << 3)
#define BIT_RD_DUAL_IO          (0x00000001 << 2)
#define BIT_RD_DUAL_I           (0x00000001 << 1)
#define BIT_FRD_SINGEL          (0x00000001)


/** End of FLASH_DRIVER_Registers_Definitions
  * @}
  */

typedef enum
{
    CHANN_SINGLE = 0,
    CHANN_DUAL = 1,
    CHANN_QUAD = 2
} T_CHANN_TYPE;

typedef enum
{
    READ_CMD            = 0x03,
    FAST_READ_QUAD_CMD  = 0xEB,
    WRITE_CMD           = 0x02,
    QUAD_WRITE_CMD      = 0x38,
    RDID_CMD            = 0x9F,
    RESET_EN_CMD        = 0x66,
    RESET_CMD           = 0x99,
} PSRAM_CMD;

/**
  * @brief  PSRAM Init structure definition
  */
typedef struct
{
    uint8_t curr_rd_cmd;        /* current used read cmd */
    uint8_t curr_wr_cmd;        /* current used write cmd */
    uint8_t rd_data_ch;         /* read data channel */
    uint8_t rd_addr_ch;         /* read address channel */
    uint8_t rd_st_dummy_cycle;  /* read status dummy cycle */
    uint8_t bit_mode;
} T_PSRAM_DEVICE_INFO;

T_PSRAM_DEVICE_INFO psram_device_info =
{
    .curr_rd_cmd = READ_CMD,
    .curr_wr_cmd = WRITE_CMD,
    .rd_data_ch  = CHANN_SINGLE,
    .rd_addr_ch  = CHANN_SINGLE,
};

extern void spic_enable(SPIC_TypeDef *spic, uint32_t enable);
extern void spic_set_rd_dummy_length(SPIC_TypeDef *spic, uint8_t dummy_len);

SPIC_TypeDef *spic = NULL;

static uint16_t g_def_dummy_len[3] = {0x0, 0x8, 0xC};/* default dummy length base for each bit mode */

static uint32_t convert_psram_addr_to_offset(uint32_t addr)
{
    return (addr - FMC_MAIN1);
}

/**
 * @brief set spic enable or disable.
 *
 * @param enable          DISABLE to disable cache, ENABLE to enable cache.
 * @return
 * @note: spic should be disabled before programming any register in user mode
*/
void spic_enable(SPIC_TypeDef *spic, uint32_t enable)
{
    spic->ssienr = enable;
}

/**
 * @brief set dummy_cycle in autolen register[11:0].
 *
 * @return
*/

static void spic_set_rd_dummy_length(SPIC_TypeDef *spic, uint8_t dummy_len)
{
    uint32_t data = BIT_RD_DUMMY_LENGTH(dummy_len);
    HAL_UPDATE32(&spic->auto_length, 0xfff, data);
}

void psram_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_FLASH1, APBPeriph_FLASH1_CLOCK, (FunctionalState)ENABLE);

    Pinmux_Config(P1_7, IDLE_MODE);
    Pinmux_Config(P3_6, IDLE_MODE);
    Pinmux_Config(P3_2, IDLE_MODE);
    Pinmux_Config(P3_3, IDLE_MODE);
    Pinmux_Config(P3_4, IDLE_MODE);
    Pinmux_Config(P3_5, IDLE_MODE);

    Pad_Config(P1_7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
               PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
               PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
               PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
               PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
               PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
               PAD_OUT_DISABLE, PAD_OUT_LOW);

    /* spic1 enable controller */
    SYSBLKCTRL->u_210.BITS_210.BIT_SOC_FLASH_EN1 = 1;

    /* spic1 master enable */
    SYSBLKCTRL->u_2A8.BITS_2A8.SPIC1_MASTER_EN = 1;

    /* spic1 & spic2 share clock source enable bit */
    HAL_WRITE32(PERIPH_REG_BASE, 0x2D0, HAL_READ32(PERIPH_REG_BASE, 0x2D0) | BIT5);

    /* set cs and baudrate */
    SPIC1->ssienr = 0;
    SPIC1->ser = BIT0;
    SPIC1->baudr = 1;

    /* ctrlr2[17:16] = 2'b11 to remove WREN+RDSR (i.e., only PP cmd issue) */
    SPIC1->ctrlr2 |= (BIT16 | BIT17);

    /* use ctrlr0 CMD_CH/DATA_CH/ADDR_CH in auto mode */
    //BIT_BAND(SPIC1->valid_cmd, 12) = 1; /* CTRLR0_CH */

    spic = SPIC1;
}

/*
What are the differences between 1 bit mode and 4 bit mode?
only 1 bit and 4 bit supported

address length always 3
1 bit:
    1-1-1
4 bit:
    1-4-4


auto mode bit num:
1 bit:
    VALID_CMD: no bits or clear 4 bit bits

4 bit:
    VALID_CMD: BIT_WR_QUAD_II(WR) / RD_QUAD_IO(RD)
    READ_QUAD_ADDR_DATA[0:7]: 0xEB (PRM_EN disable)
    WRITE_QUAD_ADDR_DATA:     0x38

user mode bit num:
    ctrlr0: CMD_CH, DATA_CH, ADDR_CH

*/
void psram_config(T_FLASH_MODE bit_mode)
{
    if (bit_mode == FLASH_MODE_1BIT)
    {
        spic->ssienr = 0;
        spic->ctrlr0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));
        spic->ssienr = 1;

        psram_device_info.curr_rd_cmd = READ_CMD;
        psram_device_info.curr_wr_cmd = WRITE_CMD;
        psram_device_info.rd_addr_ch  = CHANN_SINGLE;
        psram_device_info.rd_data_ch  = CHANN_SINGLE;

        psram_device_info.bit_mode = bit_mode;
    }
    else if (bit_mode == FLASH_MODE_4BIT)
    {
        spic->ssienr = 0;
        spic->ctrlr0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));
        spic->ctrlr0 |= (BIT_ADDR_CH(bit_mode) | BIT_DATA_CH(bit_mode));
        spic->valid_cmd |= (BIT4 | BIT8);
        spic->ssienr = 1;

        psram_device_info.curr_rd_cmd = FAST_READ_QUAD_CMD;
        psram_device_info.curr_wr_cmd = QUAD_WRITE_CMD;
        psram_device_info.rd_addr_ch  = CHANN_QUAD;
        psram_device_info.rd_data_ch  = CHANN_QUAD;

        psram_device_info.bit_mode = bit_mode;
    }
    else
    {
        DBG_DIRECT("PSRAM doesn't support %d bit mode.", 1 << bit_mode);
        while (1);
    }
}

void psram_seq_trans_enable(uint8_t enable)
{
    spic->ssienr = 0;

    if (enable)
    {
        uint8_t bit_mode = psram_device_info.bit_mode;
        uint16_t rd_dummy_len = BIT_RD_DUMMY_LENGTH(spic->auto_length);

        if (rd_dummy_len >= g_def_dummy_len[bit_mode])
        {
            HAL_UPDATE32(&spic->auto_length, (BIT18 | BIT19 | BIT20 | BIT21 | BIT22 | BIT23 | BIT24 | BIT25),
                         BIT_AUTO_DUM_LEN((spic->baudr * 2) * (8 / (1 << bit_mode))));
        }

        BIT_BAND(spic->valid_cmd, 14) = enable; /* BIT_SEQ_TRANS_EN */
    }
    else
    {
        HAL_UPDATE32(&spic->auto_length, (BIT18 | BIT19 | BIT20 | BIT21 | BIT22 | BIT23 | BIT24 | BIT25),
                     BIT_AUTO_DUM_LEN(psram_device_info.rd_st_dummy_cycle));

        BIT_BAND(spic->valid_cmd, 14) = enable; /* BIT_SEQ_TRANS_EN */
    }
}

bool psram_calibration(T_FLASH_MODE bit_mode)
{
    bool ret = false;
    uint16_t dly_cnt = g_def_dummy_len[bit_mode];

    /* adjust AUTO_LENGTH[RD_DUMMY_LENGTH], make sure read data equals magic pattern. */
    for (; dly_cnt < DELAY_CNT_MAX; ++dly_cnt)
    {
        uint32_t rd_data;

        spic_enable(spic, DISABLE);
        spic_set_rd_dummy_length(spic, dly_cnt);
        spic_enable(spic, ENABLE);

        rd_data = HAL_READ32(PSRAM_CAL_ADDR, 0);

        DBG_DIRECT("cnt=%d, [%08X]=%08X", dly_cnt, PSRAM_CAL_ADDR, rd_data);

        if (rd_data == PSRAM_MAGIC_PATTERN)
        {
            /* update to AUTO_DUM_LEN when 1 bit mode */
            if (bit_mode == FLASH_MODE_1BIT)
            {
                spic_enable(spic, DISABLE);
                spic->auto_length &= ~ BIT_AUTO_DUM_LEN(0xFF);
                spic->auto_length |= BIT_AUTO_DUM_LEN(dly_cnt);
                spic_enable(spic, ENABLE);
                psram_device_info.rd_st_dummy_cycle = dly_cnt;
            }

            /* update IN_PHYSICAL_CYC */
            if (dly_cnt >= g_def_dummy_len[bit_mode])
            {
                spic_enable(spic, DISABLE);
                spic->auto_length |= BIT_IN_PHYSICAL_CYC(dly_cnt - g_def_dummy_len[bit_mode]);
                spic_enable(spic, ENABLE);
            }

            /* get things done, break */
            break;
        }
    }

    return ret;
}

void psram_try_high_speed(void)
{
    /*
    1 bit calibration: 1 bit and RDSR rd dummy length
    use auto write to write a magic pattern before calibration
    */
    uint32_t *p_magic_pattern = (uint32_t *)PSRAM_CAL_ADDR;

    *p_magic_pattern = PSRAM_MAGIC_PATTERN;

    /* start 1 bit calibration and get 1 bit rd dummy length */
    /* do calibration -->
            get a rd_dummy_len_val
            set AUTO_LENGTH[RD_DUMMY_LENGTH] and AUTO_LENGTH[AUTO_DUM_LEN] = rd_dummy_len_val
            set AUTO_LENGTH[IN_PHYSICAL_CYC] = rd_dummy_len_val - g_def_dummy_len
    */
    psram_config(FLASH_MODE_1BIT);

    psram_calibration(FLASH_MODE_1BIT);

    DBG_DIRECT("1 bit: [%08X]=%08X", p_magic_pattern, *p_magic_pattern);

    /* start 4 bit calibration and get 4 bit rd dummy length */
    /* switch to 4 bit mode */
    /* do calibration */
    psram_config(FLASH_MODE_4BIT);

    psram_calibration(FLASH_MODE_4BIT);

    DBG_DIRECT("4 bit: [%08X]=%08X", p_magic_pattern, *p_magic_pattern);
}

bool psram_cmd_rx(uint8_t cmd, uint8_t read_len, uint8_t *read_buf)
{
    return true;
}

bool psram_cmd_tx(uint8_t cmd, uint8_t data_len, uint8_t *data_buf)
{
    return true;
}

bool psram_read(uint32_t addr, uint32_t len, uint8_t *data)
{
    return true;
}

/**
 * @brief program data to psram via user mode
 * @param start_addr    start address where is going to be written in flash
 * @param data_len      data length to be written
 * @param data          data buffer to be written
 * @return
 * @note 1. SPIC only supports SPIC_FIFO_SIZE FIFO, so max SPIC_FIFO_SIZE-4 word allowed to be written.
 *       2. start address should be 4 byte align
*/
static bool psram_write_internal(uint32_t start_addr, uint32_t data_len, uint8_t *data)
{
    bool retval = true;
    uint8_t cmd_pp = psram_device_info.curr_wr_cmd, i;
    uint32_t wr_addr;
    uint32_t *data_word = (uint32_t *)data;

    /* the test show that enable spic before push fifo make transfer faster, but we can't push by
    byte. if we want to push by byte, we should disable spic first. */
    spic_enable(spic, ENABLE);
    wr_addr = cmd_pp | __REV(start_addr);
    spic->dr[0].word = wr_addr;

    for (i = 0; i < data_len / 4; i++)
    {
        spic->dr[0].word = data_word[i];
    }

    while (spic->sr & BIT_BUSY);

    spic_enable(spic, DISABLE);
    return retval;
}

/**
 * @brief program data to flash via user mode
 * @param start_addr    start address where is going to be written in flash
 * @param data_len      data length to be written
 * @param data          data buffer to be written
 * @return true if success
 * @note start address should be 4 byte align
*/
bool psram_write(uint32_t addr, uint32_t len, uint8_t *data)
{
    const uint32_t blksize = SPIC_FIFO_SIZE - 4;
    uint32_t tmp_addr = convert_psram_addr_to_offset(addr);
    uint8_t *tmp_data = data;
    int remain_size = len;

    spic_enable(spic, DISABLE);
    spic->ctrlr0 &= ~(BIT_TMOD(3)); /* tx mode */

    while (remain_size > 0)
    {
        uint32_t write_size = remain_size > blksize ? blksize : remain_size;

        extern bool psram_write_internal(uint32_t start_addr, uint32_t data_len, uint8_t *data);
        if (!psram_write_internal(tmp_addr, write_size, tmp_data))
        {
            return false;
        }

        tmp_addr += write_size;
        tmp_data += write_size;
        remain_size -= write_size;
    }

    return true;
}

void psram_deinit(void)
{
    /* clock disable */
    RCC_PeriphClockCmd(APBPeriph_FLASH1, APBPeriph_FLASH1_CLOCK, (FunctionalState)DISABLE);

    /* spic1 enable controller */
    SYSBLKCTRL->u_210.BITS_210.BIT_SOC_FLASH_EN1 = 0;

    /* spic1 master enable */
    SYSBLKCTRL->u_2A8.BITS_2A8.SPIC1_MASTER_EN = 0;
}
