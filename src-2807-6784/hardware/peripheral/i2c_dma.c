/**
 * @file i2c.c
 * @brief
 * @date Wed 31 May 2017 07:15:09 PM CST
 * @author XIHAO
 *
 * @addtogroup
 * @ingroup
 * @details
 *
 * @{
 */

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"
#include "i2c_dma.h"

/*********************************************************************
 * MACROS
 */
#define I2C_TX_FIFO_DEPTH       16
#define I2C_RX_FIFO_DEPTH       16

#define I2CD_NO_ERROR           0x00   /**< @brief No error.            */
#define I2CD_BUS_ERROR          0x01   /**< @brief Bus Error.           */
#define I2CD_ARBITRATION_LOST   0x02   /**< @brief Arbitration Lost.    */
#define I2CD_ACK_FAILURE        0x04   /**< @brief Acknowledge Failure. */
#define I2CD_OVERRUN            0x08   /**< @brief Overrun/Underrun.    */
#define I2CD_PEC_ERROR          0x10   /**< @brief PEC Error in reception. */
#define I2CD_SMB_ALERT          0x40   /**< @brief SMBus Alert.         */

#define I2C_INTR_DEFAULT_MASK   (I2C_INTR_RX_FULL | I2C_INTR_TX_EMPTY | I2C_INTR_TX_ABRT | I2C_INTR_STOP_DET)
#define I2C_INTR_SLAVE_MASK     (I2C_INTR_RD_REQ | I2C_INTR_TX_ABRT | I2C_INTR_RX_FULL | I2C_INTR_STOP_DET)

#define I2C_DEFAULT_TIMEOUT     330000 // 0.5s@CPU64MHz

#define I2C_DEAD_WORKAROUND


/**
 * @brief  dma id of i2c mapping table
 **/

#define  I2C_0_TX_DMA_ID I2C_TX_DMA_ID
#define  I2C_0_RX_DMA_ID I2C_RX_DMA_ID
#if defined(CONFIG_XH98xx)
#define I2C_1_TX_DMA_ID I2C1_TX_DMA_ID
#define I2C_2_TX_DMA_ID I2C2_TX_DMA_ID
#define I2C_1_RX_DMA_ID I2C1_RX_DMA_ID
#define I2C_2_RX_DMA_ID I2C2_RX_DMA_ID
#elif defined(CONFIG_XH98xxP)
#define  I2C_1_TX_DMA_ID I2C2_TX_DMA_ID
#define  I2C_2_TX_DMA_ID SPI_MST1_TX_DMA_ID
#define  I2C_1_RX_DMA_ID I2C2_RX_DMA_ID
#define  I2C_2_RX_DMA_ID SPI_MST1_RX_DMA_ID
#endif

/*********************************************************************
 * GLOBAL FUNCTION
 */
//extern int pinmux_search(pinmux_t func);


/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
    __IO uint32_t *cpm_cfg;
    cpm_clk_t      cpm_type;
    pinmux_t       pinmux_sck;
    pinmux_t       pinmux_sda;
} i2c_friend_t;

typedef struct
{
    uint8_t *tx_buf1;
    uint32_t tx_len1;
    uint8_t *tx_buf2;
    uint32_t tx_len2;

    uint8_t *rx_buf;
    uint32_t rx_len;
}i2c_op_t;

#ifdef I2C_DEAD_WORKAROUND
typedef struct
{
    uint32_t speed[I2C_MODULE_NUM];
}i2c_env_t;
#endif

/*********************************************************************
 * CONSTANTS
 */
static const i2c_friend_t i2c_friend_tbl[I2C_MODULE_NUM] =
{
    {&XH_PSO->I2C_CFG, CPM_I2C_CLK,   PINMUX_I2C_MST_SCK_CFG, PINMUX_I2C_MST_SDA_CFG},
#if I2C_MODULE_NUM > 1
    {&XH_PSO->I2C1_CFG, CPM_I2C1_CLK, PINMUX_I2C1_MST_SCK_CFG,PINMUX_I2C1_MST_SDA_CFG},
#endif
#if I2C_MODULE_NUM > 2
    {&XH_PSO->I2C2_CFG, CPM_I2C2_CLK, PINMUX_I2C2_MST_SCK_CFG,PINMUX_I2C2_MST_SDA_CFG},
#endif
};

/*********************************************************************
 * LOCAL VARIABLES
 */
#ifdef I2C_DEAD_WORKAROUND
static i2c_env_t i2c_env;
#endif

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief  i2c index
 *
 * @param[in] i2c  i2c device address
 *
 * @return index
 **/
static int i2c_index(XH_I2C_Type *i2c)
{
    return i2c==XH_I2C  ? 0 :
#if I2C_MODULE_NUM > 2
           i2c==XH_I2C2 ? 2 :
#endif
#if I2C_MODULE_NUM > 3
#error I2C_MODULE_NUM too larger!
#endif
                        1;
}

#ifdef I2C_DEAD_WORKAROUND
/**
 * @brief  i2c dead workaround
* @param[in] i2c  i2c device address
 **/
static void i2c_dead_workaround(XH_I2C_Type *i2c)
{
    int unlock_timeout = 200;
    int index = i2c_index(i2c);
    pinmux_t pinmux_sck = i2c_friend_tbl[index].pinmux_sck;
    pinmux_t pinmux_sda = i2c_friend_tbl[index].pinmux_sda;

    // get pin number
    int scl_pin = i2c_pinmux_search(pinmux_sck);
    int sda_pin = i2c_pinmux_search(pinmux_sda);
    co_assert (scl_pin>=0 && sda_pin>=0);

    // pinmux to  gpio
    pinmux_config(scl_pin, PINMUX_GPIO_MODE_CFG);
    pinmux_config(sda_pin, PINMUX_GPIO_MODE_CFG);

    // gpio mode
    gpio_set_direction(BITMASK(scl_pin), GPIO_OUTPUT);
    gpio_set_direction(BITMASK(sda_pin), GPIO_INPUT);

    // unlock
    while((gpio_read(BITMASK(sda_pin)) == 0) && (--unlock_timeout > 0))
    {
        gpio_toggle(BITMASK(scl_pin));
        co_delay_10us(10);
    }

    // restore
    pinmux_config(scl_pin, pinmux_sck);
    pinmux_config(sda_pin, pinmux_sda);

    // reset
    i2c_open_x(i2c, I2C_MODE_MASTER, i2c_env.speed[index]);
}
#endif

/**
 * @brief i2c_set_mode()
 * @param[in] i2c  i2c device address
 * @param[in] mode i2c mode
 *
 * @return
 **/
static void i2c_set_mode(XH_I2C_Type *i2c, i2c_mode_t mode)
{
    switch(mode)
    {
        case I2C_MODE_MASTER:
            i2c->CON = I2C_CON_MASTER | I2C_CON_SLAVE_DISABLE | I2C_CON_RESTART_EN;
            break;
        case I2C_MODE_SLAVE:
            i2c->CON = I2C_CON_RESTART_EN;
            break;
        case I2C_MODE_SMBUS_DEVICE:
            i2c->CON = I2C_CON_10BITADDR_SLAVE | I2C_CON_RESTART_EN;
            break;
        case I2C_MODE_SMBUS_HOST:
            i2c->CON = I2C_CON_MASTER | I2C_CON_SLAVE_DISABLE | I2C_CON_10BITADDR_MASTER | I2C_CON_RESTART_EN;
            break;
    }
}

/**
 * @brief i2c_scl_hcnt()
 *
 * @param[in] ic_clk clk
 * @param[in] tsymbol tsymbol
 * @param[in] tf tf
 * @param[in] cond Condition
 * @param[in] offset offset
 *
 * @return value
 **/
static uint32_t i2c_scl_hcnt(uint32_t ic_clk, uint32_t tsymbol, uint32_t tf, int cond, int offset)
{
    /*
     * DesignWare I2C core doesn't seem to have solid strategy to meet
     * the tHD;STA timing spec.  Configuring _HCNT based on tHIGH spec
     * will result in violation of the tHD;STA spec.
     */
    if (cond)
        /*
         * Conditional expression:
         *
         *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
         *
         * This is based on the manuals, and represents an ideal
         * configuration.  The resulting I2C bus speed will be
         * faster than any of the others.
         *
         * If your hardware is free from tHD;STA issue, try this one.
         */
        return (ic_clk * tsymbol + 5000) / 10000 - 8 + offset;
    else
        /*
         * Conditional expression:
         *
         *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
         *
         * This is just experimental rule; the tHD;STA period turned
         * out to be proportinal to (_HCNT + 3).  With this setting,
         * we could meet both tHIGH and tHD;STA timing specs.
         *
         * If unsure, you'd better to take this alternative.
         *
         * The reason why we need to take into account "tf" here,
         * is the same as described in i2c_lld_scl_lcnt().
         */
        return (ic_clk * (tsymbol + tf) + 5000) / 10000 - 3 + offset;
}

/**
 * @brief i2c_scl_lcnt()
 *
 * @param[in] ic_clk ic clk
 * @param[in] tLOW low time
 * @param[in] tf tf 
 * @param[in] offset offset
 *
 * @return value
 **/
static uint32_t i2c_scl_lcnt(uint32_t ic_clk, uint32_t tlow, uint32_t tf, int offset)
{
    /*
     * Conditional expression:
     *
     *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
     *
     * DW I2C core starts counting the SCL CNTs for the LOW period
     * of the SCL clock (tLOW) as soon as it pulls the SCL line.
     * In order to meet the tLOW timing spec, we need to take into
     * account the fall time of SCL signal (tf).  Default tf value
     * should be 0.3 us, for safety.
     */
    return ((ic_clk * (tlow + tf) + 5000) / 10000) - 1 + offset;
}

/**
 * @brief i2c_set_speed()
 *
 * @param[in] i2c  i2c device address
 * @param[in] speed i2c speed
 *
 * @return 
 **/
static void i2c_set_speed(XH_I2C_Type *i2c, uint32_t speed)
{
    uint32_t clk = cpm_get_clock(CPM_I2C_CLK);

    if (speed <= 100000) //100kbps
    {
        /* set standard and fast speed deviders for high/low periods */
        /* Standard-mode @100k period=10us */
        i2c->SS_SCL_HCNT = i2c_scl_hcnt(clk/1000,
                40,/* tHD;STA = tHIGH = 4.0 us */
                3, /* tf = 0.3 us */
                0, /* 0: default, 1: Ideal */
                0);/* No offset */
        i2c->SS_SCL_LCNT = i2c_scl_lcnt(clk/1000,
                47,/* tLOW = 4.7 us */
                3, /* tf = 0.3 us */
                0);/* No offset */

        /* Standard mode clock_div calculate: Tlow/Thigh = 1/1.*/
        /* Sets the Maximum Rise Time for standard mode.*/
        i2c->CON = (i2c->CON & ~I2C_CON_SPEED_MASK) | I2C_CON_SPEED_STD;
    }
    else// if (speed <= 400000) //400kbps
    {
        /* Fast-mode @400k period=2.5us */
        i2c->FS_SCL_HCNT = i2c_scl_hcnt(clk/1000,
                6, /* tHD;STA = tHIGH = 0.6 us */
                3, /* tf = 0.3 us */
                0, /* 0: default, 1: Ideal */
                0);/* No offset */
        i2c->FS_SCL_LCNT = i2c_scl_lcnt(clk/1000,
                13,/* tLOW = 1.3 us */
                3, /* tf = 0.3 us */
                0);/* No offset */

        /* Sets the Maximum Rise Time for fast mode.*/
        i2c->CON = (i2c->CON & ~I2C_CON_SPEED_MASK) | I2C_CON_SPEED_FAST;
    }
}

/**
 * @brief i2c_read_clear_intrbits()
 *
 * @param[in] i2c  i2c device address
 * @param[in] err  error number
 *
 * @return intr status
 **/
static uint32_t i2c_read_clear_intrbits(XH_I2C_Type *i2c, uint32_t *err)
{
    uint32_t dummy, stat = i2c->INTR_STAT;
    uint32_t errors = 0;

    /* Do not use the IC_CLR_INTR register to clear interrupts. */
    if (stat & I2C_INTR_RX_UNDER)
    {
        errors |= I2CD_OVERRUN;
        dummy = i2c->CLR_RX_UNDER;
    }

    if (stat & I2C_INTR_RX_OVER)
    {
        errors |= I2CD_OVERRUN;
        dummy = i2c->CLR_RX_OVER;
    }

    if (stat & I2C_INTR_TX_OVER)
    {
        errors |= I2CD_OVERRUN;
        dummy = i2c->CLR_TX_OVER;
    }

    if (stat & I2C_INTR_RD_REQ)
        dummy = i2c->CLR_RD_REQ;

    if (stat & I2C_INTR_TX_ABRT)
    {
        /*
         * The IC_TX_ABRT_SOURCE register is cleared whenever
         * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
         */
        dummy = i2c->TX_ABRT_SOURCE;
        if (dummy & I2C_TX_ARB_LOST)
            errors |= I2CD_ARBITRATION_LOST;
        if (dummy & 0x1f/*I2C_TX_ABRT_xxx_NOACK*/)
            errors |= I2CD_ACK_FAILURE;
        if (dummy & 0xfe0)
            errors |= I2CD_BUS_ERROR; /* it is trigged by wrong sw behaviours */
        dummy = i2c->CLR_TX_ABRT;
    }

    if (stat & I2C_INTR_RX_DONE)
        dummy = i2c->CLR_RX_DONE;

    if (stat & I2C_INTR_ACTIVITY)
        dummy = i2c->CLR_ACTIVITY;

    if (stat & I2C_INTR_STOP_DET)
        dummy = i2c->CLR_STOP_DET;

    if (stat & I2C_INTR_START_DET)
        dummy = i2c->CLR_START_DET;

    if (stat & I2C_INTR_GEN_CALL)
        dummy = i2c->CLR_GEN_CALL;

    if(err)
        *err |= errors;

    (void)dummy;
    return stat;
}

/**
 * @brief i2c_read_pio()
 *
 * @param[in] i2c i2c device address
 * @param[in] op i2c operate data strcture
 *
 * @return 
 **/
static void i2c_read_pio(XH_I2C_Type *i2c, i2c_op_t *op)
{
    uint32_t rx_valid;

    if(op->tx_len1>0 || op->tx_len2>0)
        return;

    if(op->rx_len==0)
        return;

    rx_valid = i2c->RXFLR;

    for (; op->rx_len > 0 && rx_valid > 0; op->rx_len--, rx_valid--)
        *op->rx_buf++ = i2c->DATA_CMD;
}

/**
 * @brief i2c_xfer_pio()
 *
 * @param[in] i2c i2c device address
 * @param[in] op i2c operate data strcture
 *
 * @return 
 **/
static void i2c_xfer_pio(XH_I2C_Type *i2c, i2c_op_t *op)
{
    uint32_t tx_limit;

    if(op->tx_len1==0 && op->tx_len2==0)
        return;

    tx_limit = I2C_TX_FIFO_DEPTH - i2c->TXFLR;

    for(; op->tx_len1 > 0 && tx_limit > 0; tx_limit--,op->tx_len1--)
        i2c->DATA_CMD = *op->tx_buf1++;

    if(op->tx_len1 == 0)
    {
        for(; op->tx_len2 > 0 && tx_limit > 0; tx_limit--,op->tx_len2--)
            i2c->DATA_CMD = *op->tx_buf2++;
    }

    if(op->tx_len1==0 && op->tx_len2==0)
    {
        if(op->rx_len > 0)
        {
            // BUG: tx to rx must be delay
            co_delay_us(9);
            i2c->CON1 = op->rx_len | I2C_CON1_RX_ENABLE | I2C_CON1_READBYTES_UPDATE;
        }
    }
}

/**
 * @brief i2c_serve_interrupt()
 *
 * @param[in] i2c i2c device address
 * @param[in] op i2c operate data strcture
 * @param[in] err error number
 *
 * @return if the transmission finished
 **/
static bool i2c_serve_interrupt(XH_I2C_Type *i2c, i2c_op_t *op, uint32_t *errors)
{
    uint32_t stat;

    stat = i2c_read_clear_intrbits(i2c, errors);

    if (stat & I2C_INTR_RX_FULL)
        i2c_read_pio(i2c, op);

    if (stat & (I2C_INTR_TX_EMPTY | I2C_INTR_RD_REQ))
        i2c_xfer_pio(i2c, op);

    if (stat & I2C_INTR_STOP_DET)
    {
        i2c->ENABLE = 0;
        return true;
    }

    return false;
}

/**
 * @brief i2c_master_transmit_ex()
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] op  i2c transmit data strcture
 * @param[in] timeout i2c timeout
 *
 * @return if transmit success
 **/
static bool i2c_master_transmit_ex(XH_I2C_Type *i2c, uint16_t addr, i2c_op_t *op, uint32_t timeout)
{
    uint32_t errors = 0;
    uint32_t count = 0;
    __IO uint32_t dummy;
    bool res;

    i2c->ENABLE = 0;
    i2c->TAR = addr;
    i2c->CON1 = op->tx_len1 ? I2C_CON1_TX_ENABLE : (op->rx_len | I2C_CON1_RX_ENABLE | I2C_CON1_READBYTES_UPDATE);
    i2c->ENABLE = 1;
    dummy = i2c->CLR_INTR; (void) dummy;
    i2c->INTR_MASK = I2C_INTR_DEFAULT_MASK;

    while(1)
    {
        res = i2c_serve_interrupt(i2c, op, &errors);
        if(res)
            break;

        if(timeout && count++>timeout)
        {
            errors |= I2CD_TIMEOUT;

#ifdef I2C_DEAD_WORKAROUND
            i2c_dead_workaround(i2c);
#endif

            break;
        }
    }

    return errors ? false : true;
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief i2c initialize
 *
 * @param[in] i2c  i2c device address
 * @param[in] mode  mode
 * @param[in] speed  speed
 *
 * @return None
 **/
void i2c_open_x(XH_I2C_Type *i2c, i2c_mode_t mode, uint32_t speed)
{
    int index = i2c_index(i2c);
    __IO uint32_t *cpm_cfg = i2c_friend_tbl[index].cpm_cfg;

    // Open clock
    register_set1(cpm_cfg, CPM_I2C_SOFT_RESET_MASK);
    register_set0(cpm_cfg, CPM_I2C_GATE_EN_MASK);
    XH_PSO_UPD_RDY();

    i2c->ENABLE = 0;

    i2c_set_mode(i2c, mode);
    i2c_set_speed(i2c, speed);

    i2c->TX_TL = 0;/* tx fifo has one byte or below then trigger the tx empty interrupt.*/
    i2c->RX_TL = 0;/* rx fifo has received one byte then trigger the rx full interrupt.*/

    if(I2C_MODE_SLAVE == mode)
    {
        /* setting I2C slave addr */
        i2c->SAR = I2C_SLAVE_DEFAULT_ADDR;
    }

//    tx_fifo_depth = ((i2c->COMP_PARAM_1 >> 16) & 0xff) + 1;
//    rx_fifo_depth = ((i2c->COMP_PARAM_1 >> 8) & 0xff) + 1;

    /* disable interrupts */
    i2c->INTR_MASK = 0;

#ifdef I2C_DEAD_WORKAROUND
    i2c_env.speed[index] = speed;
#endif
}

/**
 * @brief i2c close
 * 
 * @param[in] i2c  i2c device address
 * @return None
 **/
void i2c_close_x(XH_I2C_Type *i2c)
{
    // Open clock
    register_set1(&XH_PSO->I2C_CFG, CPM_I2C_GATE_EN_MASK);

    XH_PSO_UPD_RDY();
}

/**
 * @brief i2c master transmit data
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] tx_buf  transmit data buffer
 * @param[in] tx_len  transmit data length
 * @param[in] rx_buf receive data buffer
 * @param[in] rx_len receive buffer length
 *
 * @return if transmission success
 **/
bool i2c_master_transmit_x(XH_I2C_Type *i2c, uint16_t addr, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len)
{
    i2c_op_t op = {tx_buf, tx_len, NULL, 0, rx_buf, rx_len};
    return i2c_master_transmit_ex(i2c, addr, &op, I2C_DEFAULT_TIMEOUT);
}

/**
 * @brief i2c master write data
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] tx_buf  transmit data buffer
 * @param[in] tx_len  transmit data length
 *
 * @retval true success
 * @retval false fail
 **/
bool i2c_master_write_x(XH_I2C_Type *i2c, uint16_t addr, uint8_t *tx_buf, uint32_t tx_len)
{
    i2c_op_t op = {tx_buf, tx_len, NULL, 0, NULL, 0};
    return i2c_master_transmit_ex(i2c, addr, &op, I2C_DEFAULT_TIMEOUT);
}

/**
 * @brief i2c master read data
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  slave address
 * @param[in] rx_buf  receive data buffer
 * @param[in] rx_len  receive buffer length
 *
 * @retval true success
 * @retval false fail
 **/
bool i2c_master_read_x(XH_I2C_Type *i2c, uint16_t addr, uint8_t *rx_buf, uint32_t rx_len)
{
    i2c_op_t op = {NULL, 0, NULL, 0, rx_buf, rx_len};
    return i2c_master_transmit_ex(i2c, addr, &op, I2C_DEFAULT_TIMEOUT);
}

/**
 * @brief i2c master read memery (EEPROM)
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] offset  memery offset
 * @param[in] alen  memery offset bytes
 * @param[in] rx_buf  receive data buffer
 * @param[in] rx_len  receive data lenght
 *
 * @retval true success
 * @retval false fail
 **/
bool i2c_master_read_mem_x(XH_I2C_Type *i2c, uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *rx_buf, uint32_t rx_len)
{
    i2c_op_t op = {(uint8_t *)&offset, alen, NULL, 0, rx_buf, rx_len};
    return i2c_master_transmit_ex(i2c, addr, &op, I2C_DEFAULT_TIMEOUT);
}

/**
 * @brief i2c master write memery (EEPROM)
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] offset  memery offset
 * @param[in] alen  memery offset bytes
 * @param[in] tx_buf  transmit data buffer
 * @param[in] tx_len  transmit data length
 *
 * @retval true success
 * @retval false fail
 **/
bool i2c_master_write_mem_x(XH_I2C_Type *i2c, uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *tx_buf, uint32_t tx_len)
{
    i2c_op_t op = {(uint8_t *)&offset, alen, tx_buf, tx_len, NULL, 0};
    return i2c_master_transmit_ex(i2c, addr, &op, I2C_DEFAULT_TIMEOUT);
}

/** @} */

/**
 * @brief i2c dma callback handler function
 *
 * @param[in] status  i2c dma status
 * @param[in] cur_src_addr  i2c dma current source addr
 * @param[in] cur_dst_addr  i2c dma current destination addr
 * @param[in] xfer_size  i2c dma trasfer size
 *
 **/
static void i2c_dma_dummy_handler(dma_status_t status, uint32_t cur_src_addr, uint32_t cur_dst_addr, uint32_t xfer_size)
{

}

/**
 * @brief i2c tx dma config
 *
 * @param[in] i2c  i2c device address
 * @param[in] dma  i2c dma data structure
 * @param[in] callback  i2c dma callback function
 *
 * @return dma channel info
 **/
static XH_DMA_CH_Type *i2c_tx_dma_config(XH_I2C_Type *i2c, XH_DMA_CH_Type *dma, dma_callback_t callback)
{
    if(dma == NULL)
        dma = dma_allocate();
    
    if(dma)
    {
        bool res;
        dma_config_t dconfig;		
		
		switch((unsigned int)i2c)
		{
			case XH_I2C_BASE:
				dconfig.slave_id = I2C0_TX_DMA_ID;
				break;

			default:
				dconfig.slave_id = I2C0_TX_DMA_ID;
				break;
		}
		
        dconfig.direction      = DMA_MEM_TO_DEV;
        dconfig.src_addr_width = DMA_SLAVE_BUSWIDTH_8BITS;
        dconfig.dst_addr_width = DMA_SLAVE_BUSWIDTH_8BITS;
        dconfig.src_burst      = DMA_BURST_LEN_1UNITS;//DMA_BURST_LEN_4UNITS;//
        dconfig.dst_burst      = DMA_BURST_LEN_1UNITS;//DMA_BURST_LEN_8UNITS;//
        dconfig.dev_flow_ctrl  = false;
        dconfig.priority       = 0;
        dconfig.callback       = callback;
        dconfig.lli.enable     = false;

        res = dma_config(dma, &dconfig);
        if(!res)
        {
            dma_release(dma);
            return NULL;
        }

        // Must be (DMATDLR + dst_burst <= 32)
        i2c->DMA_TDLR = 2;
    }

    return dma;
}

/**
 * @brief i2c rx dma config
 *
 * @param[in] i2c  i2c device address
 * @param[in] dma  i2c dma data structure
 * @param[in] callback  i2c dma callback function
 *
 * @return dma channel info
 **/
static XH_DMA_CH_Type *i2c_rx_dma_config(XH_I2C_Type *i2c, XH_DMA_CH_Type *dma, dma_callback_t callback)
{
    if(dma == NULL)
        dma = dma_allocate();
     
    if(dma)
    {
        bool res;
        dma_config_t dconfig;

		switch((unsigned int)i2c)
		{
			case XH_I2C_BASE:
				dconfig.slave_id = I2C0_RX_DMA_ID;
				break;

			default:
				dconfig.slave_id = I2C0_RX_DMA_ID;
				break;
		}

        dconfig.direction      = DMA_DEV_TO_MEM;
        dconfig.src_addr_width = DMA_SLAVE_BUSWIDTH_8BITS;//DMA_SLAVE_BUSWIDTH_8BITS;
        dconfig.dst_addr_width = DMA_SLAVE_BUSWIDTH_8BITS;//DMA_SLAVE_BUSWIDTH_8BITS;
        dconfig.src_burst      = DMA_BURST_LEN_1UNITS;
        dconfig.dst_burst      = DMA_BURST_LEN_1UNITS;
        dconfig.dev_flow_ctrl  = false;
        dconfig.priority       = 0;
        dconfig.callback       = callback;
        dconfig.lli.enable     = false;

        res = dma_config(dma, &dconfig);
        if(!res)
        {
            dma_release(dma);
            return NULL;
        }

        // Must be (DMARDLR >= src_burst - 1)
        i2c->DMA_RDLR = 0;
    }

    return dma;
}

/**
 * @brief i2c dma config
 *
 * @param[in] i2c  i2c device address
 * @param[in] dma  i2c dma data structure
 * @param[in] callback  i2c dma callback function
 *
 * @return
 **/
void i2c_dma_config(XH_I2C_Type *i2c, i2c_dma_t *dma, dma_callback_t callback)
{
/**
 * @brief the reason of delay
 * dma may not finish init, you must nop a little while, whien you try to call dma_config or XH_SYS->MON |= (1u << 16).
 * if not, i2c can not read the right data.
 * in this demo, 5 __nop() at least
**/
	__nop();__nop();__nop();__nop();__nop();	
	__nop();__nop();__nop();__nop();__nop();
	
/**
 * @brief deal i2c channel 2 specially
 * only XH98xx && XH98xx have i2c 2 channel at the moment
 * 
**/
#if defined(CONFIG_XH98xx) || defined(CONFIG_XH98xxP)
    if(i2c == XH_I2C2)
    {
        XH_SYS->MON |= (1u << 16);
	    //XH_SYS->MON &= ~(1u << 16);
    }
#endif
    
    i2c->DMA_CR = 0;//3;

    dma->callback = callback;
    
    dma->tx_dma = i2c_tx_dma_config(i2c, dma->tx_dma, i2c_dma_dummy_handler);
	dma->rx_dma = i2c_rx_dma_config(i2c, dma->rx_dma, i2c_dma_dummy_handler);//NULL);//
}

/**
 * @brief i2c master write memery(EEPROM) through dma 
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] offset  memery offset
 * @param[in] alen  memery offset bytes
 * @param[in] tx_buf  send data buffer
 * @param[in] tx_len  send data length
 * 
 **/
void i2c_master_write_mem_dma(XH_I2C_Type *i2c,i2c_dma_t *dma, uint16_t addr, uint32_t offset,uint32_t alen, uint8_t *tx_buf, uint32_t tx_len)
{
    
    if(tx_len == 0)
		return;

    i2c->DMA_CR = 2;

    i2c->ENABLE = 0;
    i2c->TAR = addr;
    i2c->CON1 = I2C_CON1_TX_ENABLE;

    i2c->INTR_MASK = 0;
    i2c->ENABLE = 1;
 
//    i2c->DATA_CMD = offset>>0;
//    i2c->DATA_CMD = offset>>8;
    
    switch(alen)
    {
        case 1:
             i2c->DATA_CMD = offset>>0;    
             break; 
        case 2: 
             i2c->DATA_CMD = offset>>8;
             i2c->DATA_CMD = offset>>0;
             break; 
        case 3: 
            i2c->DATA_CMD = offset>>16;
            i2c->DATA_CMD = offset>>8;
            i2c->DATA_CMD = offset>>0;   
            break;
        case 4: 
            i2c->DATA_CMD = offset>>24;
            i2c->DATA_CMD = offset>>16;
            i2c->DATA_CMD = offset>>8;
            i2c->DATA_CMD = offset>>0;   
            break;
        default:
            break;         
    }

    dma_start(dma->tx_dma, (uint32_t)tx_buf, (uint32_t)&i2c->DATA_CMD, tx_len);

    i2c->INTR_MASK =  I2C_INTR_DEFAULT_MASK;//  I2C_INTR_TX_ABRT | I2C_INTR_STOP_DET;//    

//    if(dma->callback == NULL)
    {
        dma_wait_stop(dma->tx_dma);
    }
}

/**
 * @brief i2c master read memery(EEPROM) through dma 
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] i2c_dma_t i2c dma data structure
 * @param[in] offset  memery offset
 * @param[in] alen  memery offset bytes
 * @param[in] rx_buf  recv data buffer
 * @param[in] rx_len  recv data length
 *
 * @retval true success
 * @retval false fail
 **/
void i2c_master_read_mem_dma(XH_I2C_Type *i2c,i2c_dma_t *dma, uint16_t addr, uint32_t offset,uint32_t alen, uint8_t *rx_buf, uint32_t rx_len)
{
	if(rx_len == 0)
        return;
	
    i2c->ENABLE = 0;
    
    i2c->TAR = addr;
    
    //i2c->CON1 |= I2C_CON1_RX_ENABLE;
    //i2c->INTR_MASK = I2C_INTR_DEFAULT_MASK;//I2C_INTR_STOP_DET;
    
    i2c->DMA_CR = 1;
//    i2c->CON1 |= ((rx_len<<0) | I2C_CON1_READBYTES_UPDATE |I2C_CON1_RX_ENABLE);
    dma_start(dma->rx_dma, (uint32_t)&i2c->DATA_CMD, (uint32_t)rx_buf, rx_len);

	i2c->INTR_MASK = I2C_INTR_RX_FULL | I2C_INTR_STOP_DET;//I2C_INTR_DEFAULT_MASK;//I2C_INTR_RX_FULL  | I2C_INTR_STOP_DET; //
    
    i2c->ENABLE = 1;
	
//    i2c->DATA_CMD = offset>>0;
//    i2c->DATA_CMD = offset>>8;
    
    switch(alen)
    {
        case 1:
             i2c->DATA_CMD = offset>>0;    
             break; 
        case 2: 
             i2c->DATA_CMD = offset>>8;
             i2c->DATA_CMD = offset>>0;
             break; 
        case 3: 
            i2c->DATA_CMD = offset>>16;
            i2c->DATA_CMD = offset>>8;
            i2c->DATA_CMD = offset>>0;   
            break;
        case 4: 
            i2c->DATA_CMD = offset>>24;
            i2c->DATA_CMD = offset>>16;
            i2c->DATA_CMD = offset>>8;
            i2c->DATA_CMD = offset>>0;   
            break;
        default:
            break;         
    }
    
    for(uint16_t i = 0; i < 0xFF; i++)
    {
    }
    
    i2c->CON1 |= ((rx_len<<0) | I2C_CON1_READBYTES_UPDATE |I2C_CON1_RX_ENABLE);

    //if(dma->callback == NULL)
    {
        dma_wait_stop(dma->rx_dma);
    }
}

