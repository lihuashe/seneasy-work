/**
 * @file pinmux.h
 * @brief pinmux driver
 * @date Thu 27 Apr 2017 07:21:17 PM CST
 * @author XIHAO
 *
 * @defgroup Pinmux Pinmux
 * @ingroup PERIPHERAL
 * @brief Pinmux driver
 * @details Pinmux driver
 *
 * @{
 */

#ifndef __PINMUX_H__
#define __PINMUX_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */


/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */
/// pinmux function type
typedef enum
{
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
    PINMUX_JTAG_MODE_CFG         = 0,
    PINMUX_DBG_MODE_CFG          = 1,
    PINMUX_SPI0_MST_SDA_I_CFG    = 3,
    PINMUX_SPI0_MST_SDA_O_CFG    = 4,
    PINMUX_SPI0_MST_CSN_CFG      = 5,
    PINMUX_SPI0_MST_SCK_CFG      = 6,
    PINMUX_SFLASH_SI_CFG         = 8,
    PINMUX_SFLASH_SO_CFG         = 9,
    PINMUX_SFLASH_HD_CFG         = 10,
    PINMUX_SFLASH_WP_CFG         = 11,
    PINMUX_SFLASH_CK_CFG         = 12,
    PINMUX_SFLASH_CSN_CFG        = 13,
    PINMUX_UART1_SDA_I_CFG       = 15,
    PINMUX_UART1_SDA_O_CFG       = 16,
    PINMUX_UART1_CTS_I_N_CFG     = 17,
    PINMUX_UART1_RTS_O_N_CFG     = 18,
    PINMUX_TX_EXT_PD_CFG         = 19,
    PINMUX_RX_EXT_PD_CFG         = 20,
    PINMUX_GPIO_ADC_CFG          = 26,
    PINMUX_GPIO_MODE_CFG         = 28,
    PINMUX_SFLASH1_CSN_1_CFG     = 24,
    PINMUX_SFLASH1_SI_CFG        = 25,
    PINMUX_SFLASH1_SO_CFG        = 29,
    PINMUX_SFLASH1_HD_CFG        = 30,
    PINMUX_SFLASH1_WP_CFG        = 31,
    PINMUX_SFLASH1_CK_CFG        = 32,
    PINMUX_SFLASH1_CSN_CFG       = 33,
    PINMUX_TIMER0_ETR_CFG        = 34,
    PINMUX_TIMER1_ETR_CFG        = 35,
    PINMUX_TIMER2_ETR_CFG        = 36,
    PINMUX_TIMER0_BKIN_CFG       = 37,
    PINMUX_TIMER1_BKIN_CFG       = 38,
    PINMUX_TIMER2_BKIN_CFG       = 39,
    PINMUX_TIMER0_IO_0_CFG       = 40,
    PINMUX_TIMER0_IO_1_CFG       = 41,
    PINMUX_TIMER0_TOGGLE_N_0_CFG = 44,
    PINMUX_TIMER1_IO_0_CFG       = 47,
    PINMUX_TIMER1_IO_1_CFG       = 48,
    PINMUX_TIMER1_TOGGLE_N_0_CFG = 51,
    PINMUX_TIMER2_IO_0_CFG       = 54,
    PINMUX_TIMER2_IO_1_CFG       = 55,
    PINMUX_TIMER2_TOGGLE_N_0_CFG = 58,
    PINMUX_UART0_SDA_I_CFG       = 59,  // XH98xx
    PINMUX_UART0_SDA_O_CFG       = 60,  // XH98xx
    PINMUX_UART2_SDA_I_CFG       = 61,  // XH98xx
    PINMUX_UART2_SDA_O_CFG       = 62,  // XH98xx

    // Multiplex above pinmux
    PINMUX_MULTIPLEX_BEGIN_CFG   = 100 + 0,
    PINMUX_SPI0_SLV_SDA_O_CFG    = 100 + 3,
    PINMUX_SPI0_SLV_SDA_I_CFG    = 100 + 4,
    PINMUX_SPI0_SLV_CSN_CFG      = 100 + 5,
    PINMUX_SPI0_SLV_SCK_CFG      = 100 + 6,
    PINMUX_LCD1_CSN_1_CFG        = 100 + 24,
    PINMUX_LCD1_SDA_CFG          = 100 + 25,
    PINMUX_LCD1_DCX_RS_CFG       = 100 + 31,
    PINMUX_LCD1_SCL_CFG          = 100 + 32,
    PINMUX_LCD1_CSN_CFG          = 100 + 33,

    // Analog mode
    PINMUX_ANALOG_BEGIN_CFG      = 200,
    PINMUX_ANALOG_CH3_PIN2_CFG,
    PINMUX_ANALOG_CH4_PIN3_CFG,
    PINMUX_ANALOG_CH5_PIN7_CFG,
    PINMUX_ANALOG_CH6_PIN8_CFG,
#else
    PINMUX_JTAG_MODE_CFG         = 0,
    PINMUX_DBG_MODE_CFG          = 1,
    PINMUX_I2C0_MST_SCK_CFG      = 2,
    PINMUX_I2C0_MST_SDA_CFG      = 3,
    PINMUX_UART0_SDA_O_CFG       = 4,
    PINMUX_UART0_SDA_I_CFG       = 5,
    PINMUX_IRTX_OUT_CFG          = 7,
    PINMUX_SFLASH1_SI_CFG        = 8,
    PINMUX_SFLASH1_SO_CFG        = 9,
    PINMUX_SFLASH1_HD_CFG        = 10,
    PINMUX_SFLASH1_WP_CFG        = 11,
    PINMUX_SFLASH1_CK_CFG        = 12,
    PINMUX_SFLASH1_CSN_CFG       = 13,
    PINMUX_UART1_SDA_I_CFG       = 15,
    PINMUX_UART1_SDA_O_CFG       = 16,
    PINMUX_UART1_CTS_I_N_CFG     = 17,
    PINMUX_UART1_RTS_O_N_CFG     = 18,
    PINMUX_TX_EXT_PD_CFG         = 19,
    PINMUX_RX_EXT_PD_CFG         = 20,
    PINMUX_DMIC_IN_CFG           = 26,
    PINMUX_DMIC_CLK_CFG          = 27,
    PINMUX_GPIO_MODE_CFG         = 28,
    PINMUX_TIMER0_ETR_CFG        = 34,
    PINMUX_TIMER1_ETR_CFG        = 35,
    PINMUX_TIMER2_ETR_CFG        = 36,
    PINMUX_TIMER0_BKIN_CFG       = 37,
    PINMUX_TIMER1_BKIN_CFG       = 38,
    PINMUX_TIMER2_BKIN_CFG       = 39,
    PINMUX_TIMER0_IO_0_CFG       = 40,
    PINMUX_TIMER0_IO_1_CFG       = 41,
    PINMUX_TIMER0_IO_2_CFG       = 42,
    PINMUX_TIMER0_IO_3_CFG       = 43,
    PINMUX_TIMER0_TOGGLE_N_0_CFG = 44,
    PINMUX_TIMER0_TOGGLE_N_1_CFG = 45,
    PINMUX_TIMER0_TOGGLE_N_2_CFG = 46,
    PINMUX_TIMER1_IO_0_CFG       = 47,
    PINMUX_TIMER1_TOGGLE_N_0_CFG = 51,
    PINMUX_TIMER2_IO_0_CFG       = 54,
    PINMUX_TIMER2_IO_1_CFG       = 55,
    PINMUX_TIMER2_IO_2_CFG       = 56,
    PINMUX_TIMER2_IO_3_CFG       = 57,
    PINMUX_TIMER2_TOGGLE_N_0_CFG = 58,
    PINMUX_TIMER2_TOGGLE_N_1_CFG = 59,
    PINMUX_TIMER2_TOGGLE_N_2_CFG = 60,
    PINMUX_I2S_SDI_CFG           = 61,
    PINMUX_I2S_TX_WS_CFG         = 62,
    PINMUX_I2S_TX_SCLK_CFG       = 63,
    PINMUX_I2S_SDO_0_CFG         = 64, /* output */
    PINMUX_I2S_SDO_1_CFG         = 65, /* internal codec output */
    PINMUX_I2S_RX_WS_CFG         = 66,
    PINMUX_I2S_RX_SCLK_CFG       = 67,

    // Multiplex above pinmux
    PINMUX_MULTIPLEX_BEGIN_CFG   = 100 + 0,
    PINMUX_I2C0_SLV_SCK_CFG      = 100 + 2,
    PINMUX_I2C0_SLV_SDA_CFG      = 100 + 3,

    // Analog mode
    PINMUX_ANALOG_BEGIN_CFG      = 200,
    PINMUX_ANALOG_CH2_PIN13_CFG,
    PINMUX_ANALOG_CH3_PIN8_CFG,
    PINMUX_ANALOG_CH4_PIN9_CFG,
    PINMUX_ANALOG_CH5_PIN10_CFG,
    PINMUX_ANALOG_CH6_PIN11_CFG,
    PINMUX_ANALOG_CH7_PIN12_CFG,
    PINMUX_ANALOG_CH8_PIN2_CFG,
    PINMUX_ANALOG_CH9_PIN3_CFG,
#endif
}pinmux_t;

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */
/**
 * @brief pinmux config
 *
 * @param[in] pin  pin: from 0 to 30
 * @param[in] func  pin function
 *
 * @return None
 **/
void pinmux_config(uint32_t pin, pinmux_t func);

/// @cond
/**
 * @brief pinmux restore
 *
 * Just for system call after sleep
 *
 * @return None
 **/
void pinmux_restore(void);
/// @endcond

#ifdef __cplusplus
}
#endif

#endif

/** @} */

