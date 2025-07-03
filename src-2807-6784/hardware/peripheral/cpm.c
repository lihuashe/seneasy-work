/*********************************************************************
 * @file cpm.c
 * @brief Clock & Power Module's low level routine
 * @version 1.0
 * @date Wed 19 Nov 2014 04:11:47 PM CST
 * @author XIHAO
 *
 * @note 
 */

/*********************************************************************
 * INCLUDES
 */
#include "xh98xx_config.h"
#include "cpm.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    struct
    {
        struct
        {
            uint32_t CPU_CFG;
            uint32_t RAM_CFG;
        }store_reg;
    }lowpower;
}cpm_env_t;

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static __IO uint32_t * const cpm_clock_cfg_table[] =
{
    /* CPM_TOP_CLK     */ NULL,
    /* CPM_CPU_CLK     */ &XH_PSO->CPU_CFG,
    /* CPM_SF0_CLK     */ &XH_PSO->SF0_CFG,
    /* CPM_SF1_CLK     */ &XH_PSO->SF1_CFG,
    /* CPM_TIM0_CLK    */ &XH_PSO->TIM_CFG[0],
    /* CPM_TIM1_CLK    */ &XH_PSO->TIM_CFG[1],
    /* CPM_TIM2_CLK    */ &XH_PSO->TIM_CFG[2],
    /* CPM_UART1_CLK   */ &XH_PSO->UART1_CFG,
    /* CPM_SPI_CLK     */ &XH_PSO->CPU_CFG, // with APB clock
#ifdef CONFIG_XH98xx
    /* CPM_I2C_CLK     */ &XH_PSO->I2C_CFG,
    /* CPM_IRTX_CLK    */ &XH_PSO->IRTX_CFG,
    /* CPM_I2S_CLK     */ &XH_PSO->I2S_CFG,
    /* CPM_UART0_CLK   */ &XH_PSO->UART0_CFG,
#endif
};

static cpm_env_t cpm_env;

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief cpm_get_top_clock()
 *
 * @return 
 **/
static uint32_t cpm_get_top_clock_mhz(void)
{
    uint32_t clk;

    if(XH_PMU->MISC_CTRL & PMU_MISC_CLK_64M_SEL_MASK)
    {
        if (XH_PMU->XTAL32M_CNS0 & PMU_XTAL32M_SEL_CPUCLK_MASK)
            clk = 64; // RC32M*2 or XTAL32M*2
        else
            clk = 32; // XTAL32M
    }
    else
    {
        clk = 32; // RC32M
    }

    return clk;
}

/**
 * @brief  cpm cpu cfg set
 *
 * @param[in] cfg  cfg
 **/
static void cpm_cpu_cfg_set(uint32_t cfg)
{
    XH_PSO->CPU_CFG = cfg;
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief cpm_init()
 *
 * Don't use global variable
 *
 * @return 
 **/
void cpm_init(void)
{
}

/**
 * @brief  cpm get cpu clock mhz
 *
 * @return
 **/
uint32_t cpm_get_cpu_clock_mhz(void)
{
    uint32_t div;

    if (XH_PSO->CPU_CFG & CPM_CPU_DIV_SEL_MASK)
        div = REGR(&XH_PSO->CPU_CFG, MASK_POS(CPM_CPU_DIV_COEFF));
    else
        div = 1;

    return cpm_get_top_clock_mhz() / div;
}

/**
 * @brief Get specified peripheral clock
 *
 * @param[in] type  clock source type
 *
 * @return clock in Hz
 */
uint32_t cpm_get_clock(cpm_clk_t type)
{
    uint32_t div = 0;
    uint32_t cfg = *cpm_clock_cfg_table[type];
    uint32_t top_clk = cpm_get_top_clock_mhz() * 1000000;

    switch (type)
    {
        case CPM_TOP_CLK:
            div = 1;
            break;

        case CPM_UART0_CLK:
        case CPM_SPI_CLK:
        case CPM_CPU_CLK:
            return cpm_get_cpu_clock_mhz() * 1000000;

        case CPM_I2C_CLK:
        case CPM_IRTX_CLK:
        case CPM_I2S_CLK:
        case CPM_TIM0_CLK: case CPM_TIM1_CLK: case CPM_TIM2_CLK:
        case CPM_SF0_CLK: case CPM_SF1_CLK:
            if (register_get(&cfg, MASK_POS(CPM_GATE_EN)))
                div = 0; // Gate
            else if (!register_get(&cfg, MASK_POS(CPM_DIV_SEL)))
                div = 1; // Original clock
            else if (register_get(&cfg, MASK_POS(CPM_DIV_EN)))
                div = register_get(&cfg, MASK_POS(CPM_DIV_COEFF)); // Divided clock
            else
                div = 0; // No clock
            break;

        case CPM_UART1_CLK:
            if (register_get(&cfg, MASK_POS(CPM_UART_GATE_EN)))
                div = 0; // Gate
            else if (!register_get(&cfg, MASK_POS(CPM_UART_DIV_SEL)))
                div = 1; // Original clock
            else if (register_get(&cfg, MASK_POS(CPM_UART_DIV_EN)))
                // clk = (256*top_clk)/(256*int + frc)
                return ((uint64_t)top_clk<<8) / ((register_get(&cfg, MASK_POS(CPM_UART_DIV_COEFF_INT))<<8) +
                                        register_get(&cfg, MASK_POS(CPM_UART_DIV_COEFF_FRC)));
            else
                div = 0; // No clock
            break;

//        case CPM_RTC_CLK:
//        case CPM_WDT_CLK:
//            return 32768;

        default:
            return 0;
    }

    return div ? top_clk/div : 0;
}

/**
 * @brief Set specified peripheral clock()
 *
 * @param[in] type  clock source type
 * @param[in] clk  clock in Hz. gate if 0
 *
 * @retval true success
 * @retval false fail
 **/
bool cpm_set_clock(cpm_clk_t type, uint32_t clk)
{
    __IO uint32_t *cfg = cpm_clock_cfg_table[type];
    uint32_t top_clk = cpm_get_top_clock_mhz() * 1000000;

    switch (type)
    {
        case CPM_TOP_CLK:
            break;

        case CPM_CPU_CLK:
            co_assert(clk != 0);
            if(clk >= top_clk)
                register_set(cfg, MASK_2REG(CPM_CPU_DIV_SEL,0, CPM_CPU_DIV_EN,1));
            else
                register_set(cfg, MASK_3REG(CPM_CPU_DIV_COEFF,top_clk/clk, CPM_CPU_DIV_SEL,1, CPM_CPU_DIV_EN,1));
            break;

        case CPM_I2C_CLK:
        case CPM_IRTX_CLK:
        case CPM_I2S_CLK:
        case CPM_TIM0_CLK: case CPM_TIM1_CLK: case CPM_TIM2_CLK:
        case CPM_SF0_CLK:  case CPM_SF1_CLK:
            if(clk == 0)
                register_set1(cfg, CPM_GATE_EN_MASK); // Gate It
            else if(clk >= top_clk)
                register_set(cfg, MASK_3REG(CPM_DIV_SEL,0, CPM_DIV_EN,0, CPM_GATE_EN,0)); // Select original clock
            else
                register_set(cfg, MASK_4REG(CPM_DIV_COEFF,top_clk/clk,
                            CPM_DIV_SEL,1, CPM_DIV_EN,1, CPM_GATE_EN,0)); // Divided It
            break;

        case CPM_UART1_CLK:
            if(clk == 0)
                register_set1(cfg, CPM_UART_GATE_EN_MASK); // Gate It
            else if(clk >= top_clk)
                register_set(cfg, MASK_3REG(CPM_UART_DIV_SEL,0, CPM_UART_DIV_EN,0, CPM_UART_GATE_EN,0));
            else
            {
                uint32_t div_x256 = ((uint64_t)top_clk << 8) / clk;
                uint32_t int_div = div_x256 >> 8;
                uint32_t frc_div = ((div_x256 % 256) << 8) / 256;
                register_set(cfg, MASK_5REG(CPM_UART_DIV_COEFF_FRC, frc_div,
                                            CPM_UART_DIV_COEFF_INT, int_div,
                                            CPM_UART_DIV_SEL,       1,
                                            CPM_UART_DIV_EN,        1,
                                            CPM_UART_GATE_EN,       0));
            }
            break;

//        case CPM_RTC_CLK: case CPM_SPI_CLK:
        default:
            return false; // Don't div it
    }

    // Update it
    XH_PSO_UPD_RDY();

    return true;
}

#if 0
/**
 * @brief Set specified peripheral clock()
 *
 * @param[in] type  clock source type
 * @param[in] div  div
 *
 * @retval true success
 * @retval false fail
 **/
bool cpm_set_clock_div(cpm_clk_t type, uint32_t div)
{
    __IO uint32_t *cfg = cpm_clock_cfg_table[type];

    switch (type)
    {
        case CPM_TOP_CLK:
            break;

        case CPM_CPU_CLK:
            if(div < 2)
                register_set(cfg, MASK_2REG(CPM_CPU_DIV_SEL,0, CPM_CPU_DIV_EN,1));
            else
                register_set(cfg, MASK_3REG(CPM_CPU_DIV_COEFF,div, CPM_CPU_DIV_SEL,1, CPM_CPU_DIV_EN,1));
            break;

//        case CPM_I2C_CLK:  case CPM_I2C1_CLK: case CPM_I2C2_CLK:
        case CPM_TIM0_CLK: case CPM_TIM1_CLK: case CPM_TIM2_CLK:
        case CPM_SF0_CLK:  case CPM_SF1_CLK:
            if(div <= 1)
                register_set(cfg, MASK_3REG(CPM_DIV_SEL,0, CPM_DIV_EN,0, CPM_GATE_EN,0));
            else
                register_set(cfg, MASK_4REG(CPM_DIV_COEFF,div,
                            CPM_DIV_SEL,1, CPM_DIV_EN,1, CPM_GATE_EN,0));
            break;

//        case CPM_UART1_CLK: case CPM_RTC_CLK:
//        case CPM_SPI_CLK:
        default:
            return false; // Don't div it
    }

    // Update it
    XH_PSO_UPD_RDY();

    return true;
}
#endif

/**
 * @brief cpm_store()
 *
 * Just for system call before sleep
 *
 * @return None
 **/
void cpm_store(void)
{
    cpm_env.lowpower.store_reg.CPU_CFG = XH_PSO->CPU_CFG;
    cpm_env.lowpower.store_reg.RAM_CFG = XH_PSO->RAM_CFG;
}

/**
 * @brief cpm_restore()
 *
 * Just for system call after sleep
 *
 * @return None
 **/
void cpm_restore(void)
{
    cpm_cpu_cfg_set(cpm_env.lowpower.store_reg.CPU_CFG);

    XH_PSO->RAM_CFG = cpm_env.lowpower.store_reg.RAM_CFG;
    XH_PSO_UPD_RDY();
}

