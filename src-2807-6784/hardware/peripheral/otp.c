/**
 * @file otp.c
 * @brief 
 * @date Wed 14 Oct 2020 03:03:19 PM CST
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
//otp controller state
#define ST_REG_POS       9
#define ST_REG_MASK      (7<<ST_REG_POS)
#define ST_POWER_ON      (0<<ST_REG_POS)
#define ST_STANDBY       (1<<ST_REG_POS)
#define ST_DEEP_STANDBY  (2<<ST_REG_POS)
#define ST_POWER_OFF     (3<<ST_REG_POS)
#define ST_PROGRAM       (4<<ST_REG_POS)
#define ST_BUSY          (5<<ST_REG_POS)
//command
#define BLOCK_PROG_RAM   0x00005a5a
#define WORD_PROG        0x00000123
#define INI_MARGIN_CHK   0x00004567
#define POWER_DOWN       0x00000110
#define POWER_UP         0x00000220
#define ENTER_STB        0x00000330
#define ENTER_DEEP_STB   0x00000440
#define RMODE_SWITCH     0x00000550
//write protect
#define DIS_W_PROTECT    0x11335577
#define EN_W_PROTECT     0xaabbccdd

//read mode
#define RMODE_USER       0x0
#define RMODE_INI_MARGIN 0x1
#define RMODE_PGM_MARGIN 0x4
#define RMODE_HT_INI     0x9
#define RMODE_HT_PGM     0xc

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    bool xip;
}otp_env_t;

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
otp_env_t otp_env = {false};

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */
/**
 * @brief  otp power enable
 *
 * @param[in] enable  enable
 **/
static void otp_power_enable(bool enable)
{
    if (enable)
    {
        if ((XH_OTP->STATUS & ST_REG_MASK) != ST_POWER_ON)
        {
            // Word Prog
            XH_OTP->COMMAND = POWER_UP;

            // Execute
            XH_OTP->EXECUTE = 1;

            // wait ready (digital issue: must delay 1 clock)
            // 52us
            while((XH_OTP->STATUS & ST_REG_MASK) != ST_POWER_ON);
            while((XH_OTP->STATUS & ST_REG_MASK) != ST_POWER_ON);
        }
    }
    else
    {
        // USE deep standby
        if ((XH_OTP->STATUS & ST_REG_MASK) != ST_DEEP_STANDBY)
        {
            // wait ready
            while(!(XH_OTP->STATUS & 0x01));

            // Word Prog
            XH_OTP->COMMAND = ENTER_DEEP_STB;

            // Execute
            XH_OTP->EXECUTE = 1;

            // wait ready (digital issue: must delay 1 clock)
            // 3us
            while((XH_OTP->STATUS & ST_REG_MASK) != ST_DEEP_STANDBY);
            while((XH_OTP->STATUS & ST_REG_MASK) != ST_DEEP_STANDBY);
        }
    }
}

/**
 * @brief  otp low voltage detection enable
 *
 * @param[in] enable  enable
 **/
static void otp_low_voltage_detection_enable(bool enable)
{
    // Enable low voltage reset
    if (enable)
        XH_PMU->FLASH_LOW_VOL_CTRL_0 = 0x5555;
    else
        XH_PMU->FLASH_LOW_VOL_CTRL_1 = 0xAAAA;
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  otp open
 **/
void otp_open(void)
{
    //OTP clock MUST be same as CPU

    // wait OTP power ready
    while(!(XH_PMU->FLASH_LOW_VOL_CTRL_0 & PMU_OTP_POWER_READY_SYNC_MASK));
    // 60us
    co_delay_10us(12);

    // Enable low voltage reset
    otp_low_voltage_detection_enable(true);

    // power on
    otp_power_enable(true);
}

/**
 * @brief  otp close
 **/
void otp_close(void)
{
    // power off
    otp_power_enable(false);
}

/**
 * @brief  opt write
 *
 * @param[in] addr  address. Must be 4bytes aign
 * @param[in] data  data
 * @param[in] length  length. Must be 4bytes aign
 **/
void otp_write(uint32_t addr, const void *data, uint32_t length)
{
    int i;
    uint32_t wdata;
    const uint8_t *pdata = data;

    co_assert((addr & 3) == 0);
    co_assert((length & 3) == 0);

    // wait ready
    while(!(XH_OTP->STATUS & 0x01));

    // Write enable
    XH_OTP->W_PROTECT = DIS_W_PROTECT;

    // Word Prog
    XH_OTP->COMMAND = WORD_PROG;

    // Write
    for (i=0; i<length; i+=4)
    {
        memcpy(&wdata, &pdata[i], 4);

        XH_OTP->ADDR = addr + i;
        XH_OTP->WDATA = wdata;
        XH_OTP->EXECUTE = 1;

        // wait ready (digital issue: must delay 1 clock)
        while(!(XH_OTP->STATUS & 0x01));
        while(!(XH_OTP->STATUS & 0x01));
    }

    // Write disable
    XH_OTP->W_PROTECT = EN_W_PROTECT;
}

#if 0
/**
 * @brief  otp write with sram
 *
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void otp_write_with_sram(uint32_t addr, const void *data, uint32_t length)
{
    co_assert((addr & 3) == 0);
    co_assert((length & 3) == 0);
    // data must be in SRAM (2nd block: 12kB-24kB)
    co_assert(((uint32_t)data & 3) == 0);
    co_assert(((uint32_t)data & XH_BUS_BASE_MASK) == XH_BUS_SRAM_BASE);
    co_assert(((uint32_t)data & XH_BUS_OFFSET_MASK) >= 12*1024);

    // wait ready
    while(!(XH_OTP->STATUS & 0x01));

    // Write enable
    XH_OTP->W_PROTECT = DIS_W_PROTECT;

    // RAM Prog
    XH_OTP->ADDR = addr;
    XH_OTP->RAM_BADDR_L = (length << 16) | (((uint32_t)data & XH_BUS_OFFSET_MASK) - 0x3000);
    XH_OTP->COMMAND = BLOCK_PROG_RAM;
    XH_OTP->EXECUTE = 1;

    // wait ready (digital issue: must delay 1 clock)
    while(!(XH_OTP->STATUS & 0x01));
    while(!(XH_OTP->STATUS & 0x01));

    // Write disable
    XH_OTP->W_PROTECT = EN_W_PROTECT;
}
#endif

/**
 * @brief  otp xip set
 *
 * @param[in] xip  xip
 **/
void otp_xip_set(bool xip)
{
    otp_env.xip = xip;
}

/**
 * @brief  otp lowpower enter
 *
 * @param[in] power_status  power status
 **/
void otp_lowpower_enter(co_power_status_t power_status)
{
    if (otp_env.xip)
    {
        // Workaround digital issue: must remap to ROM, otherwise sleep will crash
        pmu_memery_remap(PMU_REMAP_FROM_ROM);
    }
}

/**
 * @brief  otp lowpower leave
 *
 * @param[in] power_status  power status
 **/
void otp_lowpower_leave(co_power_status_t power_status)
{
    if (otp_env.xip)
    {
        // Workaround digital issue
        pmu_memery_remap(PMU_REMAP_FROM_OTP);
    }
    else
    {
        // Workaround digital issue: they force open otp that lead 1.1mA current leak, so close it to prevent it.
        otp_close();
    }

}

/** @} */

