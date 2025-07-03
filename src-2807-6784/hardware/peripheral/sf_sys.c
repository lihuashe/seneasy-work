/**
 * @file sf_sys.c
 * @brief 
 * @date Tue, Jul 16, 2019  2:29:06 PM
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
// xflash as flash memory
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
#define SFS_XFLASH_CSN_PIN  7
#define SFS_XFLASH_MISO_PIN 2
#define SFS_XFLASH_CLK_PIN  9
#define SFS_XFLASH_MOSI_PIN 8
//#define SFS_XFLASH_HD_PIN   15
//#define SFS_XFLASH_WP_PIN   17
//#define SFS_XFLASH_PWR_PIN  3
#else
#define SFS_XFLASH_MISO_PIN 7
#define SFS_XFLASH_CSN_PIN  8
#define SFS_XFLASH_CLK_PIN  9
#define SFS_XFLASH_MOSI_PIN 10
#endif

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */
static XH_SF_Type * const sfs_regobj_tbl[SFS_LOCATE_NUM] = {XH_SF, XH_SF1};

#ifdef CONFIG_XH98xx
#define SFS_CS_DECLARE(cs_locate)    uint32_t cs = (uint32_t)(cs_locate)
#else
#define SFS_CS_DECLARE(cs_locate)    const uint32_t cs = 0;
#endif

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
sfs_env_t sfs_env = {
    /* locate     */    SFS_IFLASH,
    /* auto_disen */    false,
#ifdef CONFIG_XIP_FLASH_ALL
    /* xip        */    true,
#else
    /* xip        */    false,
#endif
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if 0
/**
 * @brief sfs_xflash_quad_pinmux()
 *
 * @return 
 **/
static void sfs_xflash_quad_pinmux(void)
{
    pmu_pin_mode_set(BITMASK(SFS_XFLASH_WP_PIN), PMU_PIN_MODE_PU);
    pmu_pin_mode_set(BITMASK(SFS_XFLASH_HD_PIN), PMU_PIN_MODE_PU);

    pinmux_config(SFS_XFLASH_WP_PIN, PINMUX_SFLASH1_WP_CFG);
    pinmux_config(SFS_XFLASH_HD_PIN, PINMUX_SFLASH1_HD_CFG);
}
#endif

/**
 * @brief sfs_xflash_pinmux()
 *
 * @return 
 **/
static void sfs_xflash_pinmux(void)
{
    pmu_pin_mode_set(BITMASK(SFS_XFLASH_CSN_PIN),  PMU_PIN_MODE_PU);
    pmu_pin_mode_set(BITMASK(SFS_XFLASH_MISO_PIN), PMU_PIN_MODE_PD);

    pinmux_config(SFS_XFLASH_CSN_PIN,  PINMUX_SFLASH1_CSN_CFG);
    pinmux_config(SFS_XFLASH_MISO_PIN, PINMUX_SFLASH1_SO_CFG);
    pinmux_config(SFS_XFLASH_CLK_PIN,  PINMUX_SFLASH1_CK_CFG);
    pinmux_config(SFS_XFLASH_MOSI_PIN, PINMUX_SFLASH1_SI_CFG);
}

#if 0
/**
 * @brief  sfs xflash power on
 **/
static void sfs_xflash_power_on(void)
{
    // mode,driven,pinmux
    pmu_pin_mode_set(BITMASK(SFS_XFLASH_PWR_PIN), PMU_PIN_MODE_PP);
    pmu_pin_driven_current_set(BITMASK(SFS_XFLASH_PWR_PIN), PMU_PIN_DRIVEN_CURRENT_MAX);
    pinmux_config(SFS_XFLASH_PWR_PIN, PINMUX_GPIO_MODE_CFG);

    // init gpio
    gpio_open_clock();
    gpio_write(BITMASK(SFS_XFLASH_PWR_PIN), GPIO_HIGH);
    gpio_set_direction(BITMASK(SFS_XFLASH_PWR_PIN), GPIO_OUTPUT);

    // wait ready
    co_delay_ms(10);
}
#endif

/**
 * @brief  sfs xflash open
 **/
static void sfs_xflash_open(void)
{
#if 0
    sfs_xflash_power_on();
#endif

    sfs_xflash_pinmux();
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief sf raw opration
 *
 * JUST FOR TEST
 *
 * @param[in] locate  0:inside 1:outside 0xFF:auto
 * @param[in] ctrl  1:read 2:write
 * @param[in] cmd  sf command
 * @param[in] cmd_bits  sf command bits
 * @param[in] data  sf data
 * @param[in] data_bytes  sf data bytes
 *
 * @return errno
 **/
void sfs_raw_op(uint8_t locate, uint8_t ctrl,
                uint32_t cmd[2], uint8_t cmd_bits,
                void *data, uint16_t data_bytes)
{
    sfb_rw_params_t param;
    XH_SF_Type *sf = locate < SFS_LOCATE_NUM ? sfs_regobj_tbl[locate] : sfs_cur_obj();
    SFS_CS_DECLARE(locate);

    param.cmd[0] = cmd[0];
    param.cmd[1] = cmd[1];
    param.cmd_bits = cmd_bits;
    param.data = data;
    param.data_bytes = data_bytes;

    if(ctrl == 1) // read
        sfb_read_nodma(sf, cs, &param);
    else // write
        sfb_write_nodma(sf, cs, &param);
}

/**
 * @brief config
 *
 * @note Don't use `width = 4` in XH98xx_A1
 *
 * @param[in] freq_hz  frequency in Hz
 * @param[in] width  width
 * @param[in] delay  delay
 **/
void sfs_config(uint32_t freq_hz, sf_width_t width, uint8_t delay)
{
    sf_config_t sfrconfig;

#if 0
    if (width==SF_WIDTH_4LINE && sfs_env.locate==SFS_XFLASH)
        sfs_xflash_quad_pinmux();
#endif

    sfrconfig.freq_hz = freq_hz;
    sfrconfig.width = width;
    sfrconfig.delay = delay;
    sf_config(SFS_CUR_OBJ, &sfrconfig);
}

/**
 * @brief probe
 *
 * @param[in] locate  locate
 * @param[in] freq_hz  frequency in Hz
 *
 * @return errno
 **/
int sfs_probe(sfs_locate_t locate, uint32_t freq_hz)
{
    sf_config_t sfrconfig;
    XH_SF_Type *sf = sfs_regobj_tbl[locate];
    SFS_CS_DECLARE(locate);
    sf_status_t status = sf_status(sf, cs);
    bool detected;

    if (status == SF_STATUS_ABSENT)
    {
        return -ENODEV;
    }
    else if (status == SF_STATUS_PRESENT)
    {
        sfs_env.locate = locate;
        return 0;
    }

    // pinmux for xflash
    if(locate == SFS_XFLASH)
        sfs_xflash_open();

    // open
    sf_enable(sf, cs);

    // config
    sfrconfig.freq_hz = freq_hz;
    sfrconfig.delay = 5;
    sfrconfig.width = SF_WIDTH_1LINE;
    sf_config(sf, cs, &sfrconfig);

    // detect
    detected = sf_detect(sf, cs);

    if (!detected)
    {
        // try leave lowpower mode
        sf_deep_power_down_leave(sf, cs);

        // re-detect
        detected = sf_detect(sf, cs);
    }

    if (!detected)
    {
        // disable SF
        sf_disable(sf, cs);

        // delay 50ms
        co_delay_ms(50);

        // open
        sf_enable(sf, cs);

        // delay 10ms
        co_delay_ms(10);

        // re-detect
        detected = sf_detect(sf, cs);
    }

    if (detected)
    {
        sfs_env.locate = locate;
        return 0;
    }
    else
    {
        return -ENODEV;
    }
}

/**
 * @brief select
 *
 * @param[in] locate  locate
 *
 * @return errno
 **/
int sfs_select(sfs_locate_t locate)
{
    SFS_CS_DECLARE(locate);

    if(sf_status(sfs_regobj_tbl[locate], cs) != SF_STATUS_PRESENT)
        return -ENODEV;

    sfs_env.locate = locate;
    return 0;
}

/**
 * @brief  sfs located
 *
 * @return located
 **/
sfs_locate_t sfs_located(void)
{
    return sfs_env.locate;
}

/**
 * @brief  sfs auto disen set
 *
 * @param[in] auto_disen  auto disen
 **/
void sfs_auto_disen_set(bool auto_disen)
{
    sfs_env.auto_disen = auto_disen;
}

/**
 * @brief  sfs xip set
 *
 * @param[in] xip  xip
 **/
void sfs_xip_set(bool xip)
{
    sfs_env.xip = xip;
}

/**
 * @brief  sfs lowpower enter
 *
 * @param[in] power_status  power status
 **/
void sfs_lowpower_enter(co_power_status_t power_status)
{
    // SF XIP
    if (sfs_env.xip)
    {
        // auto disable
        if (sfs_env.auto_disen || power_status==POWER_DEEP_SLEEP)
            sfs_disable();
        else
            sfs_deep_power_down_enter();
    }
}

/**
 * @brief  sfs lowpower leave
 *
 * @param[in] power_status  power status
 **/
void sfs_lowpower_leave(co_power_status_t power_status)
{
    // SF XIP
    if (sfs_env.xip)
    {
        sfs_enable();

        // auto enable
        if (sfs_env.auto_disen || power_status==POWER_DEEP_SLEEP)
            ;
        else
            sfs_deep_power_down_leave();
    }

    // always enable cache
    sfs_cache_enable(true);
}

/**
 * @brief  sfs cur obj
 *
 * @return
 **/
XH_SF_Type *sfs_cur_obj(void)
{
    return sfs_regobj_tbl[sfs_env.locate];
}

/**
 * @brief cache enable
 *
 * @param[in] enable  enable
 **/
void sfs_cache_enable(bool enable)
{
    if(enable)
    {
        XH_HCACHE->CONFIG = 0;
        XH_HCACHE->CTRL = HCACHE_CTRL_CEN_MASK;
        while((XH_HCACHE->STATUS & HCACHE_STATUS_CSTS_MASK) == 0);
    }
    else
    {
        XH_HCACHE->CONFIG = HCACHE_CONFIG_GCLKDIS_MASK;
        XH_HCACHE->CTRL = 0;
        while((XH_HCACHE->STATUS & HCACHE_STATUS_CSTS_MASK) != 0);
    }
}

/**
 * @brief  sfs cache invalidate all
 **/
void sfs_cache_invalidate_all(void)
{
    XH_HCACHE->MAINT0 = HCACHE_MAINT0_INVALL_MASK;
}

/** @} */

