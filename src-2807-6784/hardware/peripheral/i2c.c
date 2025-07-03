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


/*********************************************************************
 * MACROS
 */
#define MSA_REG_SPI_I2C 0x00
#define MSA_REG_WHO_AM_I 0x01
//#define MSA_REG_INT_LATCH 0x21
#define MSA_REG_FREEFALL_DUR 0x22

#define I2C_TX_FIFO_DEPTH       16
#define I2C_RX_FIFO_DEPTH       16
#define I2CD_NO_ERROR           0x00   /**< @brief No error.            */
#define I2CD_BUS_ERROR          0x01   /**< @brief Bus Error.           */
#define I2CD_ARBITRATION_LOST   0x02   /**< @brief Arbitration Lost.    */
#define I2CD_ACK_FAILURE        0x04   /**< @brief Acknowledge Failure. */
#define I2CD_OVERRUN            0x08   /**< @brief Overrun/Underrun.    */
#define I2CD_PEC_ERROR          0x10   /**< @brief PEC Error in reception. */
#define I2CD_TIMEOUT            0x20   /**< @brief Hardware timeout.    */
#define I2CD_SMB_ALERT          0x40   /**< @brief SMBus Alert.         */
#define I2C_INTR_MASTER_MASK    (I2C_INTR_RX_FULL | I2C_INTR_TX_EMPTY | I2C_INTR_TX_ABRT | I2C_INTR_STOP_DET)
#define I2C_INTR_SLAVE_MASK     (I2C_INTR_RX_FULL | I2C_INTR_RD_REQ   | I2C_INTR_TX_ABRT | I2C_INTR_STOP_DET)
#define I2C_DEFAULT_TIMEOUT     2000000 //1000//2000000//4620000 // 7s@CPU64MHz
#define I2C_DEAD_WORKAROUND
#define PINMUX_PIN_NUM          23

typedef struct
{
    __IO uint32_t *cpm_cfg;
    cpm_clk_t      cpm_type;
    pinmux_t       pinmux_sck;
    pinmux_t       pinmux_sda;
} i2c_friend_t;



typedef struct
{
    bool running;
    i2c_complete_callback_t callback;
    i2c_op_t op;
	XH_I2C_Type *i2c;
    i2c_mode_t mode;
#ifdef I2C_DEAD_WORKAROUND
    uint32_t speed;
#endif

} i2c_env_t;

#define SLAVE_REGISTER_ADDR_1 0x0D
#define SLAVE_REGISTER_ADDR_2 0x0E
static uint8_t sg_slave_register_1[16] = {100, 99, 98, 97, 96, 95};
static uint8_t sg_slave_register_2[16] = {10, 9, 8, 7, 6};

static const i2c_friend_t i2c_friend_tbl[I2C_MODULE_NUM] =
{
    {&XH_PSO->I2C_CFG, CPM_I2C_CLK,   PINMUX_I2C0_MST_SCK_CFG, PINMUX_I2C0_MST_SDA_CFG},
#if I2C_MODULE_NUM > 1
    {&XH_PSO->I2C1_CFG, CPM_I2C1_CLK, PINMUX_I2C1_MST_SCK_CFG, PINMUX_I2C1_MST_SDA_CFG},
#endif
#if I2C_MODULE_NUM > 2
    {&XH_PSO->I2C2_CFG, CPM_I2C2_CLK, PINMUX_I2C2_MST_SCK_CFG, PINMUX_I2C2_MST_SDA_CFG},
#endif
};

static i2c_env_t sg_i2c_paras[I2C_MODULE_NUM];

static uint8_t sgSlaveRecvData[64];
static uint8_t sgSlaveRecvDataBytes = 0;
void printfSgSlaveRecvData()
{
//    log_debug_array(sgSlaveRecvData, sgSlaveRecvDataBytes);
}

//static void send_register_data(XH_I2C_Type* i2c, i2c_op_t* op);

static int i2c_index(XH_I2C_Type *i2c)
{
#if I2C_MODULE_NUM == 1
	return i2c == XH_I2C ? 0 : 1;
#else
	if(i2c == XH_I2C)
	{
		return 0;
	}
	if(i2c == XH_I2C1)
	{
		return 1;
	}
	if(i2c == XH_I2C2)
	{
		return 2;
	}
	return 0;
#endif
}

static inline i2c_env_t* getSgPara(XH_I2C_Type *i2c)
{
	return &sg_i2c_paras[i2c_index(i2c)];
}

#ifdef I2C_DEAD_WORKAROUND
int i2c_pinmux_search(pinmux_t func)
{
	int i;
	uint8_t *p = (uint8_t *)XH_SYS->PINMUX;

	if(func > PINMUX_ANALOG_BEGIN_CFG)
		func = PINMUX_GPIO_MODE_CFG;

	if(func > PINMUX_MULTIPLEX_BEGIN_CFG)
		func -= (int)PINMUX_MULTIPLEX_BEGIN_CFG;

	for (i = 0; i < PINMUX_PIN_NUM; ++i)
	{
		if ((p[i]&0x7F) == (uint8_t)func)
		return i;
	}

	return -1;
}

static void i2c_dead_workaround(XH_I2C_Type *i2c)
{
    int unlock_timeout = 200;
    int index = i2c_index(i2c);
	i2c_env_t *sg_i2c_para = &sg_i2c_paras[index];
    pinmux_t pinmux_sck = i2c_friend_tbl[index].pinmux_sck;
    pinmux_t pinmux_sda = i2c_friend_tbl[index].pinmux_sda;
    int scl_pin = i2c_pinmux_search(pinmux_sck);
    int sda_pin = i2c_pinmux_search(pinmux_sda);
    co_assert (scl_pin>=0 && sda_pin>=0);
    pinmux_config(scl_pin, PINMUX_GPIO_MODE_CFG);
    pinmux_config(sda_pin, PINMUX_GPIO_MODE_CFG);
    gpio_set_direction(BITMASK(scl_pin), GPIO_OUTPUT);
    gpio_set_direction(BITMASK(sda_pin), GPIO_INPUT);
    while((gpio_read(BITMASK(sda_pin)) == 0) && (--unlock_timeout > 0))
    {
        gpio_toggle(BITMASK(scl_pin));
        co_delay_10us(10);
    }
    pinmux_config(scl_pin, pinmux_sck);
    pinmux_config(sda_pin, pinmux_sda);
    i2c_open(i2c, I2C_MODE_MASTER, sg_i2c_para->speed);
}
#endif

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

static uint32_t i2c_scl_hcnt(uint32_t ic_clk, uint32_t tsymbol, uint32_t tf, int cond, int offset)
{
    if (cond)
        return (ic_clk * tsymbol + 5000) / 10000 - 8 + offset;
    else
        return (ic_clk * (tsymbol + tf) + 5000) / 10000 - 3 + offset;
}

static uint32_t i2c_scl_lcnt(uint32_t ic_clk, uint32_t tlow, uint32_t tf, int offset)
{
    return ((ic_clk * (tlow + tf) + 5000) / 10000) - 1 + offset;
}

static void i2c_set_speed(XH_I2C_Type *i2c, uint32_t speed)
{
    int index = i2c_index(i2c);
    cpm_clk_t cpm_clk;
    switch(index)
    {
        case 0:
            cpm_clk = CPM_I2C_CLK;
            break;
        default:
            cpm_clk = CPM_I2C_CLK;
            break;
    }

    uint32_t clk = cpm_get_clock(cpm_clk);
    if (speed <= 100000)
    {
        i2c->SS_SCL_HCNT = i2c_scl_hcnt(clk/1000, 40, 3, 0, 0);
        i2c->SS_SCL_LCNT = i2c_scl_lcnt(clk/1000, 47, 3, 0);
        i2c->CON = (i2c->CON & ~I2C_CON_SPEED_MASK) | I2C_CON_SPEED_STD;
    }
    else if (speed <= 400000)
    {
        i2c->FS_SCL_HCNT = i2c_scl_hcnt(clk/1000, 6, 3, 0, 0);
        i2c->FS_SCL_LCNT = i2c_scl_lcnt(clk/1000, 13, 3, 0);
        i2c->CON = (i2c->CON & ~I2C_CON_SPEED_MASK) | I2C_CON_SPEED_FAST;
    }
    else
    {
        i2c->XH_SCL_HCNT = 6;
        i2c->XH_SCL_LCNT = 8;
        i2c->CON = (i2c->CON & ~I2C_CON_SPEED_MASK) | I2C_CON_SPEED_HIGH;
    }
}

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

static inline uint32_t getSmaller(const uint32_t a, const uint32_t b)
{
    return a > b ? b : a;
}

static void i2c_read_pio(XH_I2C_Type *i2c, i2c_op_t *op)
{
    uint32_t rx_valid;

    if(op->tx_len1>0 || op->tx_len2>0)
        return;

    if(op->rx_len==0)
        return;

    rx_valid = i2c->RXFLR;
	op->rx_len_real += getSmaller(op->rx_len, rx_valid);
    for (; op->rx_len > 0 && rx_valid > 0; op->rx_len--, rx_valid--)
        *op->rx_buf++ = i2c->DATA_CMD;

    /*i2c_env_t* sg_i2c_para = getSgPara(i2c);
    if(sg_i2c_para->mode == I2C_MODE_SLAVE && op->rx_len_real > 0)
    {
        co_delay_us(9);
        i2c->CON1 = I2C_CON1_TX_ENABLE;

        send_register_data(i2c, op);
    }*/
}

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

static pmu_lowpower_peripheral_t getPLPFromIndex(const int index)
{
    switch(index)
    {
        case 0:
            return PMU_LP_I2C0;
        default:
            return PMU_LP_I2C0;
    }
}

static pmu_lowpower_peripheral_t getPLP(XH_I2C_Type* i2c)
{
    int index = i2c_index(i2c);
    return getPLPFromIndex(index);
}

static bool i2c_transmit(XH_I2C_Type *i2c, i2c_mode_t mode, uint16_t addr, i2c_op_t* op, uint32_t timeout, i2c_complete_callback_t callback)
{
	i2c_env_t* sg_i2c_para = getSgPara(i2c);
    if (sg_i2c_para->running)
        return false;

	pmu_lowpower_peripheral_t plp = getPLP(i2c);

    sg_i2c_para->mode = mode;
    sg_i2c_para->callback = callback;
    sg_i2c_para->op = *op;
    sg_i2c_para->op.rx_buf_real = sg_i2c_para->op.rx_buf;
    sg_i2c_para->op.rx_len_real = 0;

    i2c->ENABLE = 0;
    i2c->TAR = addr;

    i2c->CON1 = op->tx_len1 ? I2C_CON1_TX_ENABLE : (op->rx_len | I2C_CON1_RX_ENABLE | I2C_CON1_READBYTES_UPDATE);

    __IO uint32_t dummy;
    dummy = i2c->CLR_INTR; (void) dummy;
    if(I2C_MODE_MASTER == mode) {
        if(0 !=op->rx_len) {
            i2c->INTR_MASK = I2C_INTR_MASTER_MASK;//I2C_INTR_RX_FULL;
        }
        else {
            i2c->INTR_MASK = I2C_INTR_MASTER_MASK;
        }


    }
    else {
        if(0 !=op->rx_len) {
            i2c->INTR_MASK = I2C_INTR_RX_FULL;
        }
        else {
            i2c->INTR_MASK = I2C_INTR_RD_REQ; //������Ҫ��ȡ�ӵ�����ʱ���ӵĴ�λ��1
        }
    }
    //i2c->INTR_MASK = mode == I2C_MODE_MASTER ? I2C_INTR_MASTER_MASK : I2C_INTR_SLAVE_MASK;

	if(callback)
	{
        int index = i2c_index(i2c);
        IRQn_Type irq_type;
        switch(index)
        {
            case 0:
                irq_type = I2C_IRQn;
                break;
            default:
                irq_type = I2C_IRQn;
                break;
        }

		sg_i2c_para->running = true;
		sg_i2c_para->i2c = i2c;

        if(mode == I2C_MODE_SLAVE)
        {
            //sg_i2c_para->callback = NULL;
        }

		pmu_lowpower_prevent(plp);
		NVIC_ClearPendingIRQ(irq_type);
		NVIC_SetPriority(irq_type, IRQ_PRIORITY_NORMAL);
		NVIC_EnableIRQ(irq_type);
		i2c->ENABLE = 1;
		return true;
	}

	uint32_t errors = 0;
	uint32_t count = 0;
	bool res;
	i2c->ENABLE = 1;
	while (true)
	{
		res = i2c_serve_interrupt(i2c, op, &errors);
		if (res)
			break;

		if (timeout && count++ > timeout)
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

void i2c_open(XH_I2C_Type *i2c, i2c_mode_t mode, uint32_t speed)
{
    if(speed != 100 * 1000 && speed != 400 * 1000 && speed != 3400 * 1000)
    {
        return;
    }

	i2c_env_t *sg_i2c_para = getSgPara(i2c);
    int index = i2c_index(i2c);
	memset(sg_i2c_para, 0, sizeof(i2c_env_t));
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
    sg_i2c_para->speed = speed;
#endif
}

void i2c_close(XH_I2C_Type *i2c)
{
    int index = i2c_index(i2c);
    __IO uint32_t *cpm_cfg = i2c_friend_tbl[index].cpm_cfg;

    // Open clock
    register_set1(cpm_cfg, CPM_I2C_GATE_EN_MASK);
    XH_PSO_UPD_RDY();

	i2c_env_t *sg_i2c_para = getSgPara(i2c);
	memset(sg_i2c_para, 0, sizeof(i2c_env_t));
}

bool i2c_master_write(XH_I2C_Type *i2c, uint16_t addr, uint8_t* tx_buf, uint32_t tx_len, i2c_complete_callback_t cb)
{
    i2c_op_t op = { tx_buf, tx_len, NULL, 0, NULL, 0, NULL, 0};
    return i2c_transmit(i2c, I2C_MODE_MASTER, addr, &op, I2C_DEFAULT_TIMEOUT, cb);
}

bool i2c_master_read(XH_I2C_Type *i2c, uint16_t addr, uint8_t* rx_buf, uint32_t rx_len, i2c_complete_callback_t cb)
{
    i2c_op_t op = { NULL, 0, NULL, 0, rx_buf, rx_len, NULL, 0};
    return i2c_transmit(i2c, I2C_MODE_MASTER, addr, &op, I2C_DEFAULT_TIMEOUT, cb);
}


bool i2c_slave_write(XH_I2C_Type *i2c, uint8_t* tx_buf, uint32_t tx_len, i2c_complete_callback_t cb)
{
    i2c_op_t op = { tx_buf, tx_len, NULL, 0, NULL, 0, NULL, 0};
    return i2c_transmit(i2c, I2C_MODE_SLAVE, I2C_SLAVE_DEFAULT_ADDR, &op, I2C_DEFAULT_TIMEOUT, cb);
}

bool i2c_slave_read(XH_I2C_Type *i2c, uint8_t* rx_buf, uint32_t rx_len, i2c_complete_callback_t cb)
{
    i2c_op_t op = { NULL, 0, NULL, 0, rx_buf, rx_len, NULL, 0};
    i2c_transmit(i2c, I2C_MODE_SLAVE, I2C_SLAVE_DEFAULT_ADDR, &op, I2C_DEFAULT_TIMEOUT, cb);
	return true;
}

static bool i2c_master_transmit_ex(uint16_t addr, i2c_op_t *op, uint32_t timeout)
{
    uint32_t errors = 0;
    uint32_t count = 0;
    bool res;

    XH_I2C->ENABLE = 0;
    XH_I2C->TAR = addr;
    XH_I2C->CON1 = op->tx_len1 ? I2C_CON1_TX_ENABLE : (op->rx_len | I2C_CON1_RX_ENABLE | I2C_CON1_READBYTES_UPDATE);
    XH_I2C->ENABLE = 1;
    //XH_I2C->INTR_MASK = I2C_INTR_DEFAULT_MASK;
    XH_I2C->INTR_MASK = I2C_INTR_MASTER_MASK;


    while(1)
    {
        res = i2c_serve_interrupt(XH_I2C, op, &errors);
        if(res)
            break;

        if(timeout && count++>timeout)
        {
            errors |= I2CD_TIMEOUT;

//#ifdef I2C_DEAD_WORKAROUND
//            i2c_dead_workaround();
//#endif

            break;
        }
    }

    return errors ? false : true;
}
bool i2c_master_write_mem(uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *tx_buf, uint32_t tx_len)
{
    i2c_op_t op = {(uint8_t *)&offset, alen, tx_buf, tx_len, NULL, 0};
    return i2c_master_transmit_ex(addr, &op, I2C_DEFAULT_TIMEOUT);
}
bool i2c_master_read_mem(uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *rx_buf, uint32_t rx_len)
{
    i2c_op_t op = {(uint8_t *)&offset, alen, NULL, 0, rx_buf, rx_len};
    return i2c_master_transmit_ex(addr, &op, I2C_DEFAULT_TIMEOUT);
}

void I2Cx_IRQHandler(const int index)
{
    uint32_t error = 0;
    i2c_env_t* sg_i2c_para = &sg_i2c_paras[index];
    XH_I2C_Type* i2c = sg_i2c_para->i2c;
    pmu_lowpower_peripheral_t plp = getPLPFromIndex(index);

    bool res = i2c_serve_interrupt(i2c, &sg_i2c_para->op, &error);
    if (res)
    {
        i2c_complete_callback_t callback = sg_i2c_para->callback;
        pmu_lowpower_allow(plp);
        sg_i2c_para->callback = NULL;
        sg_i2c_para->running = false;
        if (callback)
            callback(sg_i2c_para->op.rx_buf_real, sg_i2c_para->op.rx_len_real);
    }
}

//void I2C_IRQHandler(void)
//{
//    I2Cx_IRQHandler(0);
//}

void send_register_data(XH_I2C_Type* i2c, i2c_op_t* op)
{
    sgSlaveRecvDataBytes = op->rx_len;
    memcpy(sgSlaveRecvData, op->rx_buf_real, op->rx_len);

    uint8_t addr = op->rx_buf_real[0];
    op->rx_len = 0;
    op->tx_len1 = 0;
    op->tx_len2 = 0;
    switch(addr)
    {
        case SLAVE_REGISTER_ADDR_1:
            op->tx_buf2 = sg_slave_register_1;
            op->tx_len2 = 3;
            break;
        case SLAVE_REGISTER_ADDR_2:
            op->tx_buf2 = sg_slave_register_2;
            op->tx_len2 = 3;
            break;
        default:
            break;
    }
}
