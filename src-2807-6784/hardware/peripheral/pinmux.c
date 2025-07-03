/**
 * @file pinmux.c
 * @brief 
 * @date Thu 27 Apr 2017 07:21:13 PM CST
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
#include "xh98xx_config.h"
#include "pinmux.h"

/*********************************************************************
 * MACROS
 */
#ifdef CONFIG_XH98xx
#define PINMUX_PIN_NUM 23
#else
#define PINMUX_PIN_NUM 19
#endif

#define PINMUX_REG_NUM (PINMUX_PIN_NUM/4 + 1)

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uint32_t reg_save[PINMUX_REG_NUM];
}pinmux_env_t;

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static pinmux_env_t pinmux_env = {{0x00000000, 0x00000000, 0x00001a00, 0x00000000, 0x00000000}};

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief pinmux config
 *
 * @param[in] pin  pin: from 0 to 30
 * @param[in] func  pin function
 *
 * @return None
 **/
void pinmux_config(uint32_t pin, pinmux_t func)
{
    uint32_t i, p;

    if(pin >= PINMUX_PIN_NUM)
        return;

    // Index and place
    i = pin/4;
    p = (pin % 4) * 8;

    if(func > PINMUX_ANALOG_BEGIN_CFG)
        func = PINMUX_GPIO_MODE_CFG;

    if(func > PINMUX_MULTIPLEX_BEGIN_CFG)
        func -= 100;

    register_set(&XH_SYS->PINMUX[i], SYS_PINMUX_MASK(func, p));

    // Save it for sleep
    pinmux_env.reg_save[i] = XH_SYS->PINMUX[i];
}

/**
 * @brief pinmux restore
 *
 * Just for system call after sleep
 *
 * @return None
 **/
void pinmux_restore(void)
{
    int i;
    for(i=1; i<PINMUX_REG_NUM; ++i)
        XH_SYS->PINMUX[i] = pinmux_env.reg_save[i];

    // pinmux[0] include GATE_CPUCLK flag
    XH_SYS->PINMUX[0] = (XH_SYS->PINMUX[0] & SYS_PINMUX_SYSPLL_GT_CPUCLK_HW_CTRL_MASK) | pinmux_env.reg_save[0];
}

/** @} */


