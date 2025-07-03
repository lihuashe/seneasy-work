/**
 * @file efuse.c
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

#ifndef CONFIG_EFUSE_ABSENT

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */
/**
 * @brief  efuse power enable
 *
 * @param[in] enable  enable
 **/
static void efuse_write_power_enable(bool enable)
{
    if (enable)
    {
        /// open audio clock
        XH_PSO->AUDIO_CFG = 0;
        XH_PSO_UPD_RDY();

        // power on
        REGW(&XH_AUDIO->CODEC_ANA_PWR_1, MASK_1REG(AU_AUDIO_LDO_CTRL, 1)); // AU_PD_AU_AULDO28_REG control by REG
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_SEL_INNERIREF_VBAT_AU, 0));
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_4REG(AU_PD_AU_AULDO28_REG,0, AU_CTRL_AU_AULDO28,2, AU_AU_LDO28_MODE,1, AU_EN_LDO28_OFFCHIPCAP,1));

        // delay 50us to wait ldo stable
        co_delay_10us(10);
    }
    else
    {
        // power off
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_PD_AU_AULDO28_REG,1));
    }
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  efuse open
 **/
void efuse_open(void)
{
    REGW1(&XH_PSO->EFUSE_CFG, CPM_EFUSE_SOFT_RESET_MASK);
    REGW0(&XH_PSO->EFUSE_CFG, CPM_EFUSE_GATE_EN_MASK);
    XH_PSO_UPD_RDY();

    // Digital BUG: must be odd (default is 0x140)
    REGW(&XH_EFUSE->PROGRAM_CFG1, MASK_1REG(EFUSE_PROGRAM_CFG1_T_PGM, 0x141));
}

/**
 * @brief  efuse close
 **/
void efuse_close(void)
{
    REGW1(&XH_PSO->EFUSE_CFG, CPM_EFUSE_GATE_EN_MASK);
    XH_PSO_UPD_RDY();
}

/**
 * @brief  opt write
 *
 * @param[in] addr  address
 * @param[in] data  data
 * @param[in] length  length
 **/
void efuse_write(uint32_t addr, const void *data, uint32_t length)
{
    const uint8_t *pdata = data;

    efuse_write_power_enable(true);

    // enable efuse program
    XH_EFUSE->PROGRAM_ENABLE |= EFUSE_PROGRAM_ENABLE;
    while(!(XH_EFUSE->STATUS & EFUSE_PROGRAM_START_MASK));

    for (uint32_t i = 0; i < length; i ++)
    {
        XH_EFUSE->PROGRAM_ADDRESS = addr++;
        XH_EFUSE->PROGRAM_DATA = pdata[i];

        XH_EFUSE->PROGRAM_START |= EFUSE_PROGRAM_START_MASK;
        while(XH_EFUSE->PROGRAM_START & EFUSE_PROGRAM_START_MASK);
    }

    // disable efuse
    XH_EFUSE->PROGRAM_ENABLE = EFUSE_PROGRAM_DISABLE;

    efuse_write_power_enable(false);
}

/**
 * @brief  opt read
 *
 * @param[in] addr  address
 * @param[in] data  data
 * @param[in] length  length
 **/
void efuse_read(uint32_t addr, void *data, uint32_t length)
{
    memcpy(data, (void*)(XH_EFUSE->READ_DATA + addr), length);
}

#endif

/** @} */

