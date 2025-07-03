/**
 * @file audio.c
 * @brief 
 * @date Fri, Aug  9, 2019 10:45:08 AM
 * @author liqiang
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
#include "audio.h"


/*********************************************************************
 * MACROS
 */
 
#define PMU_LP_AUDIO     (PMU_LP_I2S)
typedef enum
{
    AUDIO_STATE_IDLE,
    AUDIO_STATE_OPEN,
    AUDIO_STATE_OPEN_NEXT,
    AUDIO_STATE_OPENED,
    AUDIO_STATE_STOP
}audio_state_e;


/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uint8_t au_sel_bias;
    uint8_t state;
    audio_callback_t audio_cb;
//    co_timer_t       co_timer;
}audio_env_t;

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static co_timer_t audio_open_timer;

static audio_env_t audio_env = {
    .au_sel_bias = 5,
    .state       = AUDIO_STATE_IDLE,
    .audio_cb    = NULL,
};

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void audio_open_timer_handler(co_timer_t *timer, void *param)
{
    //APP_LOG_INF("audio_open_timer_handler %d\n", audio_env.state);

    if (audio_env.state == AUDIO_STATE_OPEN)
    {
        /// t3 open ldo_12
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_PD_AU_AULDO12_REG,  0));
        co_delay_us(10);

//        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_PGA, 1));
//        co_delay_us(10);
//        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_PGA, 0));
//        co_delay_us(10);

        /// t4 open clk
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_CLK_REG, 0));

        audio_env.state = AUDIO_STATE_OPEN_NEXT;
        co_timer_set(&audio_open_timer, 50, TIMER_ONE_SHOT, audio_open_timer_handler, NULL);
    }
    else if (audio_env.state == AUDIO_STATE_OPEN_NEXT)
    {
        /// t5 close auref bypass
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_EN_REF_BYP_REG, 0));
        co_delay_us(10);

        /// 2) open pga adc
        /// t0 open pga
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_PGA_REG, 0));
        co_delay_us(5);

        /// t1 open adc bias
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUADC_BIAS_REG, 0));
        co_delay_us(5);

        /// t2 open adc core
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUADC_CORE_REG, 0));
        co_delay_us(1);

        /// t3 reset adc
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_RST_ADC_REG, 1));
        co_delay_us(10);

        /// t4 clear reset_adc
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_RST_ADC_REG, 0));

//            REGW(&XH_AUDIO->ADC_CTRL, MASK_1REG(AU_ADC_ANTI_CLIP, 1));

        /* switch to auadc mode */
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_ADC_MODE, 0));

        audio_env.state = AUDIO_STATE_OPENED;
        if (audio_env.audio_cb != NULL)
        {
            audio_env.audio_cb(NULL);
        }
    }
		
    //log_debug("audio_open_timer_handler_end %d\n", audio_env.state);
}
/*********************************************************************
 * PUBLIC FUNCTIONS
 */


/**
 * @brief audio open clock.
 *
 * @return None
 **/
void audio_open(audio_callback_t callback)
{
    if (audio_env.state != AUDIO_STATE_IDLE)
    {
        return;
    }
    audio_env.audio_cb = callback;
		
    REGW1(&XH_PMU->ANA_PD, PMU_ANA_PD_LDO_ANADIG_SEL_MASK);
    XH_PMU_UPD_RDY();

    REGW0(&XH_PMU->ANA_PD, PMU_ANA_PD_LDO_ANADIG_MASK);
    XH_PMU_UPD_RDY();

    CPM_ANA_AHB_CLK_ENABLE();
    REGW1(&XH_DAIF->CLK_CFG, DAIF_EN_CKO16M_XTAL_AUDIO_MASK);
    CPM_ANA_CLK_RESTORE();
    REGW(&XH_PMU->CLK_CTRL_1, MASK_1REG(PMU_SEL_XTAL32M_GM, 15));

    /// open audio clock
    REGW(&XH_PSO->AUDIO_CFG, MASK_3REG(CPM_AUDIO_GATE_EN, 0, CPM_AUDIO_12M_GATE_EN, 0, CPM_AUDIO_SOFT_RESET, 0));
    XH_PSO_UPD_RDY();

    /// pga, adc, ldo, bias, 1: control by reg. 0: control by fsm 
    REGW(&XH_AUDIO->CODEC_ANA_PWR_1, MASK_2REG(AU_AUDIO_ADC_CTRL, 1, AU_AUDIO_LDO_CTRL, 1));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_PGA_REG, 0));

    /// 1) open ldo, bias
    /// t0 open iref
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUDIO_IREF_VBAT_AU_REG, 0));
    co_delay_us(10);
//   REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_SEL_INNERIREF_AUVREF_VBAT_AU, 0));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_SEL_BIAS_VBAT_AU, audio_env.au_sel_bias));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_2REG(AU_CTRL_AU_AULDO28, 3, AU_CTRL_AU_AULDO12,3));
//   REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_AULDO28MOD, 0));//0:2.0V, 1:3.3V
    /// t1 open ldo_28
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_PD_AU_AULDO28_REG, 0));
    co_delay_us(10);
    /// t2 open auref, open auref bypass
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_2REG(AU_PD_AUREF_REG, 0, AU_AU_EN_REF_BYP_REG, 1));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_BIAS_GEN_REG, 0));
//    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_EN_PGAIN_OPEN,  0));  
//    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_GP_PGABUF, 1));
//    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PGA_GAIN, 1));

    pmu_ana_enable(true, PMU_ANA_ADC);
    XH_DAIF->PD_CFG1 = 0x401FD;
    XH_AUDIO->CODEC_ANA_CTRL_1 = 0x400860B4;
    XH_AUDIO->CODEC_ANA_CTRL_2 = 0x208AF380;
    XH_GPADC->ADC_DBG1 &= ~(1<<5);
    XH_GPADC->ADC_DBG1 |= (1<<21);
    pmu_ana_enable(false, PMU_ANA_ADC);

    audio_env.state = AUDIO_STATE_OPEN;

    co_timer_set(&audio_open_timer, 100, TIMER_ONE_SHOT, audio_open_timer_handler, NULL);
    pmu_lowpower_prevent(PMU_LP_AUDIO);
}

/**
 * @brief audio clock clock.
 *
 * @return None
 **/
void audio_close(void)
{
    if (audio_env.state == AUDIO_STATE_IDLE)
    {
        return;
    }
    audio_env.state = AUDIO_STATE_STOP;
    co_timer_del(&audio_open_timer);
  
    REGW(&XH_AUDIO->CODEC_CLK_CTRL_1, MASK_2REG(AU_DMIC_CLK_CTRL, 0, AU_DMIC_CLK_SEL, 0));

    /// disable digital mic
    REGW(&XH_AUDIO->ADC_CTRL, MASK_1REG(AU_DMIC_EN, 0));
    REGW(&XH_AUDIO->ADC_CTRL, MASK_1REG(AU_ADC_ANTI_CLIP, 0));
    
    /// reset audio
    REGW(&XH_PSO->AUDIO_CFG, MASK_1REG(CPM_AUDIO_SOFT_RESET, 1));
    XH_PSO_UPD_RDY();
    /// close audio clock
    REGW(&XH_PSO->AUDIO_CFG, MASK_2REG(CPM_AUDIO_GATE_EN, 1, CPM_AUDIO_12M_GATE_EN, 1));
    XH_PSO_UPD_RDY();

    CPM_ANA_AHB_CLK_ENABLE();
    REGW0(&XH_DAIF->CLK_CFG, DAIF_EN_CKO16M_XTAL_AUDIO_MASK);
    CPM_ANA_CLK_RESTORE();

    pmu_ana_enable(true, PMU_ANA_ADC);
    XH_DAIF->PD_CFG1 = 0x001F9;
    XH_GPADC->ADC_DBG1 |= (1<<5);
    XH_GPADC->ADC_DBG1 &= ~(1<<21);
    pmu_ana_enable(false, PMU_ANA_ADC);

    REGW1(&XH_PMU->ANA_PD, PMU_ANA_PD_LDO_ANADIG_MASK);
    XH_PMU_UPD_RDY();

    REGW0(&XH_PMU->ANA_PD, PMU_ANA_REG_PD_BUF_DIG_MASK);
    XH_PMU_UPD_RDY();

    audio_env.state = AUDIO_STATE_IDLE;
    pmu_lowpower_allow(PMU_LP_AUDIO);
}

/**
 * @brief Audio configuration
 *
 * @param[in] config audio config structure
 *
 * @return None
 **/
void audio_set_config(const audio_config_t *config)
{
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_3REG(AU_AU_PGA_GAIN, config->pga_gain,
                                                AU_AU_SEL_PGAIN, config->sel_pga_input,
                                                AU_AUADC_GAIN, config->adc_gain));
    
    audio_set_adc_vol_ctrl(0x01608674);
    //audio_set_adc_vol_ctrl(0x01646416);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_11, 0xcbed6256);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_12, 0xe7b7117f);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_13, 0x117f);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_21, 0xd59c5ea3);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_22, 0xcbe12186);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_23, 0x2186);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_31, 0xc56b6500);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_32, 0xf3d60cb5);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_33, 0x0cb5);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_41, 0xde5f5b59);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_42, 0xe567118f);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_43, 0x118f);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_51, 0xc1866709);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_52, 0x0b1e0cb5);
    audio_set_deci_filt(AUDIO_ADC_DECI_FILT_53, 0x0cb5);
}

/**
 * @brief  audio set calibarate param
 *
 * @param[in] au_sel_bias  au sel bias
 **/
void audio_set_calibarate_param(uint8_t au_sel_bias)
{
    audio_env.au_sel_bias = au_sel_bias;
}

/**
 * @brief Audio setting adc vol
 *
 * @param[in] audio vol control value
 *
 * @return None
 **/
void audio_set_adc_vol_ctrl(uint32_t val)
{
    REGW(&XH_AUDIO->ADC_CTRL, MASK_1REG(AU_ADC_ANTI_CLIP, 0));
    REGW(&XH_AUDIO->ADC_VOL_CTRL, 0xFFFFFFFF, val);
    
    if(REGR(&XH_AUDIO->ADC_CTRL, MASK_POS(AU_ADC_EN)))
    {
        // must delay 100us and enable au_adc_en, for modify AU_ADC_ANTI_CLIP
        co_delay_us(100); 
        REGW(&XH_AUDIO->ADC_CTRL, MASK_1REG(AU_ADC_ANTI_CLIP, 1));
    }
}

/**
 * @brief Audio setting deci filt
 *
 * @param[in] audio vol control value
 *
 * @return None
 **/
void audio_set_deci_filt(audio_adc_deci_filt_t deci_filt, uint32_t val)
{
    volatile uint32_t *p_adc_deci_filt_reg = &XH_AUDIO->ADC_DECI_FILT_11 + deci_filt;
    *p_adc_deci_filt_reg = val;
}

/**
 * @brief Audio setting iir filt
 *
 * @param[in] audio vol control value
 *
 * @return None
 **/
void audio_set_iir_filt(audio_adc_iir_filt_t iir_filt, uint32_t val)
{
    volatile uint32_t *p_adc_iir_filt_reg = &XH_AUDIO->ADC_IIR_FILT_11 + iir_filt;
    *p_adc_iir_filt_reg = val;
}

/**
 * @brief audio open for dmic.
 *
 * @return None
 **/
void audio_dmic_open(const audio_dmic_config_t* config)
{
    /// open audio clock
    REGW(&XH_PSO->AUDIO_CFG, MASK_3REG(CPM_AUDIO_GATE_EN, 0, CPM_AUDIO_12M_GATE_EN, 0, CPM_AUDIO_SOFT_RESET, 0));
    XH_PSO_UPD_RDY();

    /// select digital mic 
    REGW(&XH_AUDIO->ADC_CTRL, MASK_1REG(AU_DMIC_EN, 1));
    /// set clock
    REGW(&XH_AUDIO->CODEC_CLK_CTRL_1, MASK_2REG(AU_DMIC_CLK_CTRL, config->ctrl, AU_DMIC_CLK_SEL, config->clk));
}

void audio_set_debug_bus(void)
{
    XH_SYS->MON = 0x0801;

    pinmux_config(5, PINMUX_DBG_MODE_CFG); 
    pinmux_config(6, PINMUX_DBG_MODE_CFG); 
    pinmux_config(7, PINMUX_DBG_MODE_CFG); 
    pinmux_config(11, PINMUX_DBG_MODE_CFG); 
    pinmux_config(12, PINMUX_DBG_MODE_CFG); 
    pinmux_config(13, PINMUX_DBG_MODE_CFG); 
    pinmux_config(14, PINMUX_DBG_MODE_CFG); 

    pmu_pin_driven_current_set(BITMASK(5), PMU_PIN_DRIVEN_CURRENT_2MA);
    pmu_pin_driven_current_set(BITMASK(6), PMU_PIN_DRIVEN_CURRENT_2MA);
    pmu_pin_driven_current_set(BITMASK(7), PMU_PIN_DRIVEN_CURRENT_2MA);
    pmu_pin_driven_current_set(BITMASK(11), PMU_PIN_DRIVEN_CURRENT_2MA);
    pmu_pin_driven_current_set(BITMASK(12), PMU_PIN_DRIVEN_CURRENT_2MA);
    pmu_pin_driven_current_set(BITMASK(13), PMU_PIN_DRIVEN_CURRENT_2MA);
    pmu_pin_driven_current_set(BITMASK(14), PMU_PIN_DRIVEN_CURRENT_2MA);

}

/** @} */

