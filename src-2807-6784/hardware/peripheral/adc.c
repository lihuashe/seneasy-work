/**
 * @file adc.c
 * @brief 
 * @date Tue 16 May 2017 02:52:22 PM CST
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
#include "co.h"
#include "co_debug.h"

/*********************************************************************
 * MACROS
 */
/* use default calib data */
#ifndef ADC_USE_DEFAULT_CALIB_DATA
#define ADC_USE_DEFAULT_CALIB_DATA 1
#endif

/* enable or disable debug log */
//#define DEBUG_ADC_ENALBE 

#ifdef ADC_USE_CALIB
#define GPADC_OUT_IDEAL_DIFF(ideal_2,ideal_1)       ((ideal_2) - (ideal_1))

#define GPADC_OUT_UPPER_BOUND(ideal)                ((float)(ideal) * 1.2)
#define GPADC_OUT_LOW_BOUND(ideal)                  ((float)(ideal) * 0.8)
#endif

#define TEMPERATURE_CALCULATE   25.0

#define BITMAP_NEW_GAIN_ERROR   0x8
#define BITMAP_NEW_TEMP_COMPEN  0x10

/*********************************************************************
 * TYPEDEFS
 */

#ifdef ADC_USE_CALIB
typedef struct
{
    int8_t vosTemp_set;
    uint16_t adcOut_03v;
    uint16_t adcOut_30v;
    uint16_t gainErr_set;
    int16_t vos_set;
    float vosTemp_cal;
    float gainErr_cal;

} adc_calib_t;
static adc_calib_t adc_para[ADC_SAMPLING_CYCLES_MAX];

typedef struct
{
    uint16_t code_0p6v;
    uint16_t code_1p25v;
    uint16_t code_shrt;
    float adcOut_vbat;

} adc_calib_compen_t;
#endif

typedef struct
{
    float sampling_cycles;
    uint8_t recv_num;
    bool  use_efuse;            // ture:efuse   false:flash
    bool  use_new_gain_error;
    bool  use_new_temp_compen;
    adc_callback_t callback[ADC_CH_MAX];
} adc_env_t;

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
#ifdef ADC_USE_CALIB
static const uint16_t GPADC_OUT_03_IDEAL_FORMULA_1[ADC_SAMPLING_CYCLES_MAX] =
{
    241,1252,5644,23904
};
static const uint16_t GPADC_OUT_30_IDEAL_FORMULA_1[ADC_SAMPLING_CYCLES_MAX] =
{
    333,1732,7806,33059
};


static const uint16_t GPADC_OUT_03_IDEAL[ADC_SAMPLING_CYCLES_MAX] =
{
    451,1805,7219,28877
};
static const uint16_t GPADC_OUT_30_IDEAL[ADC_SAMPLING_CYCLES_MAX] =
{
    624,2496,9984,39936
};

static const int16_t TABLE_TEMPER_2_TEMPERATURE[ADC_TEMPER_MAX] =
{
    20,21,22,23,24,25,26,27
};
#endif

static const int16_t TABLE_SAMPLE_2_CYCLE[ADC_SAMPLING_CYCLES_MAX] =
{
    16,32,64,128
};

#if ADC_USE_DEFAULT_CALIB_DATA
static ft_flash_gpadc_calib_t  adc_ft_calib_config;
static cp_efuse_gpadc_calib_t  adc_cp_calib_config;
static float adc_ft_calib_vbat;
#endif

static adc_env_t adc_env;

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */
/*********************************************************************
 * @func adc_round()
         brief note: ADC round

 * @parameters:
		 value: value

 * @return
         value
 *********************************************************************/
static int adc_round(float value)
{
    if (value > 0)
        value = value + 0.5;
    else
        value = value - 0.5;

    return (int)value;
}

/*********************************************************************
 * @func adc_dump()
         brief note: dump ADC registert

 * @parameters:
         none

 * @return
         void
 *********************************************************************/
void adc_dump(void)
{
    APP_LOG_INF("ADC_CFG1=%x\r\n", XH_GPADC->ADC_CFG1);
    APP_LOG_INF("ADC_CFG2=%x\r\n", XH_GPADC->ADC_CFG2);
    APP_LOG_INF("CODEC_ANA_CTRL_1=%x\r\n", XH_AUDIO->CODEC_ANA_CTRL_1);
    APP_LOG_INF("CODEC_ANA_CTRL_2=%x\r\n", XH_AUDIO->CODEC_ANA_CTRL_2);
    APP_LOG_INF("CH_0_CFG=%x\r\n", XH_GPADC->CH_0_CFG);
    APP_LOG_INF("CH_8_CFG=%x\r\n", XH_GPADC->CH_8_CFG);
}

/*********************************************************************
 * @func adc_is_running()
         brief note: ADC is running or not

 * @parameters:
		 none

 * @return
         bool
 *********************************************************************/
bool adc_is_running(void)
{
    if (REGR(&XH_PSO->AUDIO_CFG, MASK_POS(CPM_AUDIO_GATE_EN)) && REGR(&XH_PSO->AUDIO_CFG, MASK_POS(CPM_AUDIO_12M_GATE_EN)))
        return false;
    else
        return true;
}

/*********************************************************************
 * @func adc_open()
         brief note: ADC power on

 * @parameters:
         none

 * @return
         void
 *********************************************************************/
void adc_open(void)
{
    /* open ana clk */
    pmu_ana_enable(true, PMU_ANA_ADC);

    /* daif config */
    REGW1(&XH_DAIF->CLK_CFG, DAIF_EN_CKO16M_XTAL_AUDIO_MASK);
    REGW1(&XH_DAIF->PD_CFG1, DAIF_PD_LDO_ANADIG_ME_MASK);
    REGW1(&XH_DAIF->PD_CFG1, DAIF_PD_LDO_ANADIG_MO_MASK);

    /* open ldo anadig */
    REGW1(&XH_PMU->ANA_PD, PMU_ANA_PD_LDO_ANADIG_SEL_MASK);
    XH_PMU_UPD_RDY();
    REGW0(&XH_PMU->ANA_PD, PMU_ANA_PD_LDO_ANADIG_MASK);
    XH_PMU_UPD_RDY();

    /* change xtal32m gm adjust xtal32m ampl */
    REGW(&XH_PMU->CLK_CTRL_1, MASK_1REG(PMU_SEL_XTAL32M_GM, 15));

    /* open audio clock */
    REGW(&XH_PSO->AUDIO_CFG, MASK_3REG(CPM_AUDIO_GATE_EN, 0, CPM_AUDIO_12M_GATE_EN, 0, CPM_AUDIO_SOFT_RESET, 0));
    XH_PSO_UPD_RDY();

    /* pga, adc, ldo, bias, 1: control by reg. 0: control by fsm */
    REGW(&XH_AUDIO->CODEC_ANA_PWR_1, MASK_2REG(AU_AUDIO_ADC_CTRL, 1, AU_AUDIO_LDO_CTRL, 1));

    /* 1) open ldo, bias */
    /* t0 open iref, select audio inner iref, adjust 2.8V DRV LDO and 1.2V LDO output*/
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUDIO_IREF_VBAT_AU_REG, 0));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_2REG(AU_CTRL_AU_AULDO28, 3, AU_CTRL_AU_AULDO12, 3));
    co_delay_us(2);

    /* t1 open ldo_28 */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_PD_AU_AULDO28_REG, 0));
    co_delay_us(2);

    /* t2 open auref/biasgen and bypass cap */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUREF_REG, 0));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_EN_REF_CAPBYP, 1));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_2REG(AU_PD_AU_BIAS_GEN_REG, 0, AU_AU_EN_REF_BYP_REG, 1));
    co_delay_us(2);

    /* t3 open ldo_12 */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_PD_AU_AULDO12_REG, 0));
    co_delay_us(2);

    /* t4 open clk */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_CLK_REG, 0));
    co_delay_us(2);

    /* 2) open buffer, pga, adc */
    /* t0 open pga */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_PGA_REG, 0));
    co_delay_us(5);

    /* t1 open pga buffer */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_GP_PGABUF, 0));
    co_delay_us(1);

    /* t2 open adc bias */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUADC_BIAS_REG, 0));
    co_delay_us(5);

    /* t3 open adc core */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUADC_CORE_REG, 0));
    co_delay_us(1);

    /* t4 reset adc */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_RST_ADC_REG, 1));
    co_delay_us(1);

    /* t5 clear reset adc */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_RST_ADC_REG, 0));
    co_delay_us(5);

    /* switch to gpadc mode */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_ADC_MODE, 1));
 /*
  1.If VDD_AUDIO on the board is connected to the capacitor, AU_EN_LDO28_OFFCHIPCAP register needs to be set to 1, otherwise set to 0.
  2.When the VDD_AUDIO connection capacitor, due to the capacitor charge and discharge of the capacitor. Ar this timer you need to add 
   delay after VDD_AUDIO is opened according to the size of the capacitor, the delay will be different ,you can according to the oscilloscope 
   grab VDD_AUDIO startup time, to set the delay time. 
*/   
    /* 3) add configuration */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_3REG(AU_AU_PGA_GAIN, 0xD, AU_EN_PGAIN_OPEN, 0, AU_SEL_INNERIREF_VBAT_AU, 0));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_7REG(AU_SEL_GPBUF_INNER_IREF, 0, AU_SEL_GPADC_MUX, 1, AU_AU_LDO28_MODE, 0,
                                                                                            AU_EN_LDO28_OFFCHIPCAP, 1, AU_SEL_INNERIREF_AUVREF_VBAT_AU, 0,
                                                                                            AU_EN_GPBUF_CHOPPER, 1, AU_PD_CHOPPER_CLKGEN, 0));
    co_delay_us(100);
}

/*********************************************************************
 * @func adc_close()
         brief note: ADC power down

 * @parameters:
         none

 * @return
         void
 *********************************************************************/
void adc_close(void)
{
    /* 3) close ext configuration */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_EN_PGAIN_OPEN, 0));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_2REG(AU_EN_LDO28_OFFCHIPCAP, 0, AU_PD_CHOPPER_CLKGEN, 1));
		
    /* 2) close buffer, pga, adc */
    /* t3 close adc core */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUADC_CORE_REG, 1));
		
    /* t2 close adc bias */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUADC_BIAS_REG, 1));
		
    /* t1 close pga buffer */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_GP_PGABUF, 1));
		
    /* t0 close pga */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_PGA_REG, 1));
		
	/* 1) close ldo, bias */
    /* t4 close clk */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AU_CLK_REG, 1));		

    /* t3 close ldo_12 */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_EN_GPBUF_CHOPPER, 0));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_PD_AU_AULDO12_REG, 1));

    /* t2 close auref/biasgen and bypass cap */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_2REG(AU_PD_AU_BIAS_GEN_REG, 1, AU_AU_EN_REF_BYP_REG, 0));
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_EN_REF_CAPBYP, 0));
	REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUREF_REG, 1));

    /* t1 close ldo_28 */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_PD_AU_AULDO28_REG, 1));
		
    /* t0 close iref */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_AUDIO_IREF_VBAT_AU_REG, 1));
		
	/* exit gpadc mode */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_2, MASK_1REG(AU_ADC_MODE, 0));

    /* reset audio */
    REGW(&XH_PSO->AUDIO_CFG, MASK_1REG(CPM_AUDIO_SOFT_RESET, 1));
    XH_PSO_UPD_RDY();

    /* close audio clock */
    REGW(&XH_PSO->AUDIO_CFG, MASK_2REG(CPM_AUDIO_GATE_EN, 1, CPM_AUDIO_12M_GATE_EN, 1));
    XH_PSO_UPD_RDY();

    /* close ldo anadig */
    REGW1(&XH_PMU->ANA_PD, PMU_ANA_PD_LDO_ANADIG_MASK);
    XH_PMU_UPD_RDY();
    REGW0(&XH_PMU->ANA_PD, PMU_ANA_PD_LDO_ANADIG_SEL_MASK);
    XH_PMU_UPD_RDY();

    /* daif config */
    REGW0(&XH_DAIF->CLK_CFG, DAIF_EN_CKO16M_XTAL_AUDIO_MASK);
    REGW0(&XH_DAIF->PD_CFG1, DAIF_PD_LDO_ANADIG_ME_MASK);

    /* close ana clk */
    pmu_ana_enable(false, PMU_ANA_ADC);
}

/*********************************************************************
 * @func adc_calib_comm()
         brief note: get gpadc out value

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

         compen:    0: adc calibrating first, output value is code
                    1: adc is already calibrated, output value is detailed

 * @return
         value:     code when the compen is 0
                    voltage(uinit is V) or temperature when the compen is 1
 *********************************************************************/
static uint16_t adc_calib_comm(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles, bool compen)
{
    uint16_t value = 0;
    uint16_t status = 1;

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("\r\nadc_calib_in: ch=%d, sampling cycles=%d,compen=%d\r\n", channel, sampling_cycles, compen);
#endif

    /* set digital sampling cycles, default 0x4 */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_AVG_CFG, 0x4));

    /* channel enable */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_VECT, 1 << channel));
    
    /* adc mode, 0:continuous conversion  1:single conversion */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_LIFE, 1));

    /* set adc vref */
    REGW(((uint32_t *)&XH_GPADC->CH_0_CFG) + channel, MASK_1REG(GPADC_MODE_VERF, ADC_VREF_1P25V));

    /* set adc pga gain */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_PGA_GAIN, ADC_PGA_GAIN_0P125));

    /* set analog sampling cycles */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_GP_BIT_SEL, sampling_cycles));

    /* set AUTO_COMPEN, default 0 */
    REGW(&XH_GPADC->SMP_OUT_CTL, MASK_1REG(GPADC_AUTO_COMPEN, compen));

    /* fix digital circuit bug: reset ADC by reg before GPADC_ADC_START set 1, ensure steady analog output */
    REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_RST_ME, 1));
    REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_RST_MO, 0));

    /* power-on ADC */
    REGW(&XH_GPADC->ADC_CFG0, MASK_1REG(GPADC_ADC_START, 1));

    while(1)
    {
        /* Get the status of adc calibrate */
        status = REGR(&XH_GPADC->ADC_CFG0, MASK_POS(GPADC_ADC_START));

        /* If the status equals 0, the calibrate procedure is done */
        if (0 == status)
        {
            /* no-reset ADC by reg */
            REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_RST_MO, 1));
            REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_RST_ME, 0));
            break;
        }
    }

    /* We can get the calibrate value from responding vector */
    value = REGR(((uint32_t *)&XH_GPADC->CH_0_DATA) + channel, MASK_POS(GPADC_DATA_LOW));

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("adc_calib_out: v=%d\r\n", value);
#endif

    return value;
}

#ifdef ADC_USE_CALIB
/*********************************************************************
 * @func adc_calib_temper
         brief note: get the temper value by calibrate adc channel 0

 * @parameters:
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

         temper:    0x0: 20'C    0x4: 24'C
                    0x1: 21'C    0x5: 25'C
                    0x2: 22'C    0x6: 26'C
                    0x3: 23'C    0x7: 27'C

         gainErr_val:   adc gainErr

         vos_val:   adc vos

 * @return
         vosTemp
 *********************************************************************/
static float adc_calib_temper(adc_sampling_cycles_t sampling_cycles, adc_temper_refer_t temper, float gainErr_val, int16_t vos_val)
{
    uint16_t value = 0;
    float vbe = 0;
    float vcm_val = 1.25;
    float gain_val = 8;
    int16_t cycles_val = TABLE_SAMPLE_2_CYCLE[sampling_cycles % ADC_SAMPLING_CYCLES_MAX];
    float vosTemp = 0;

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("\r\nadc_calib_temper_in: sampling_cycles=%d, gainErr=%f, vos=%x\r\n",
                                                            sampling_cycles, gainErr_val, vos_val);
#endif

    /* power on GP_TEMP_GEN */
    REGW(&XH_GPADC->VOS_TEMP, MASK_1REG(GPADC_PD_GP_TEMP_GEN, 0));

    /* set temp_sel[26:24], if the temperature is not 25 at the time of calibration, modify this register*/
    REGW(&XH_GPADC->VOS_TEMP, MASK_1REG(GPADC_TEMP_SEL, temper));

    /* calibrate the temperature by the specific channel: ADC_CH_0 */
    value = adc_calib_comm(ADC_CH_0, sampling_cycles, 0);

    /* power down GP_TEMP_GEN */
    REGW(&XH_GPADC->VOS_TEMP, MASK_1REG(GPADC_PD_GP_TEMP_GEN, 1));

    /* calculate the vbe */
    vbe = ((float)(value - vos_val) / gainErr_val / cycles_val / cycles_val / 2 - 1) * gain_val + vcm_val;

    /* calculate the vosTemp */
    vosTemp = (680.2 - (vbe * 1000)) / 1.878;
    vosTemp = vosTemp + (TEMPERATURE_CALCULATE - TABLE_TEMPER_2_TEMPERATURE[temper % ADC_TEMPER_MAX]);

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("adc_calib_temper_out: value=%d, vbe=%f, vosTemp=%f\r\n",
                                                            value, vbe, vosTemp);
#endif

    return vosTemp;
}
#endif

/*********************************************************************
 * @func adc_compen_comm()
         brief note: get gpadc out value as compensation param

 * @parameters:
         pd_gp_pgabuf:   false: power on gp_pgabuf
                         true: power down gp_pgabuf

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

         vref:   0x0: ADC_VREF_0P6V
                 0x1: ADC_VREF_1P25V

 * @return
         value: gpadc out
 *********************************************************************/
static uint16_t adc_compen_comm(bool pd_gp_pgabuf, adc_sampling_cycles_t sampling_cycles, uint8_t vref)
{
    uint16_t value = 0;
    uint16_t status = 1;

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("\r\nadc_compen_in: vref=%d, sampling cycles=%d\r\n", vref, sampling_cycles);
#endif

    /* power down vbat detector */
    REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_PD_GP_POWER_ME, 1));
    REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_PD_GP_POWER_MO, 1));

    /* set digital sampling cycles, default 0x4 */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_AVG_CFG, 0x4));

    /* channel enable */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_VECT, 1 << ADC_CH_1));

    /* set adc vref */
    REGW(((uint32_t *)&XH_GPADC->CH_0_CFG) + ADC_CH_1, MASK_1REG(GPADC_MODE_VERF, vref));

    if(pd_gp_pgabuf)
    {
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_GP_PGABUF, 1));
    }

    /* set adc pga gain */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_PGA_GAIN, ADC_PGA_GAIN_0P125));

    /* set analog sampling cycles */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_GP_BIT_SEL, sampling_cycles));

    /* set AUTO_COMPEN, default 0 */
    REGW(&XH_GPADC->SMP_OUT_CTL, MASK_1REG(GPADC_AUTO_COMPEN, 0));

    /* fix digital circuit bug: reset ADC by reg before GPADC_ADC_START set 1, ensure steady analog output */
    REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_RST_ME, 1));
    REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_RST_MO, 0));

    /* power-on ADC */
    REGW(&XH_GPADC->ADC_CFG0, MASK_1REG(GPADC_ADC_START, 1));

    while(1)
    {
        /* Get the status of adc calibrate */
        status = REGR(&XH_GPADC->ADC_CFG0, MASK_POS(GPADC_ADC_START));

        /* If the status equals 0, the calibrate procedure is done */
        if (0 == status)
        {
            /* no-reset ADC by reg */
            REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_RST_MO, 1));
            REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_RST_ME, 0));
            break;
        }
    }

    /* We can get the calibrate value from responding vector */
    value = REGR(((uint32_t *)&XH_GPADC->CH_0_DATA) + ADC_CH_1, MASK_POS(GPADC_DATA_LOW));

    /* restore vbat detector */
    REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_PD_GP_POWER_MO, 0));
    REGW(&XH_GPADC->ADC_DBG1, MASK_1REG(GPADC_PD_GP_POWER_ME, 0));

    if(pd_gp_pgabuf)
    {
        REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_PD_GP_PGABUF, 0));
    }

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("adc_compen_out: v=%d\r\n", value);
#endif

    return value;
}

/*********************************************************************
 * @func adc_set_new_calibarate_param()
         brief note: calculate new calibarate param

 * @parameters:
         ft_flash_gpadc_calib_t      *config;
         ft_flash_gpadc_temp_calib_t *compen_param;

 * @return
         void
 *********************************************************************/
static void adc_set_new_calibarate_param(ft_flash_gpadc_calib_t *config, ft_flash_gpadc_temp_calib_t *compen_param, 
                                         ft_flash_gpadc_temp_calib_new_t *compen_param_new)
{
    ft_flash_gpadc_calib_t config_new = {0};
    uint16_t code_0p6v_new  = 0;
    uint16_t code_1p25v_new = 0;
    uint16_t code_short_new = 0;

    uint16_t code_0p6v_old  = 0;
    uint16_t code_1p25v_old = 0; 
    uint16_t code_short_old = 0;

    int sum_code_0p6v =0;
    int sum_code_1p25v =0;
    int sum_code_shrt =0;

    uint16_t cycles = 0;
    float delta1 = 0;  
    float delta2 = 0;  
    float constant = 0; 
    float vos_core_old = 0;
    float vos_core_new = 0;
    float gain_error_all = 0; 

    adc_open();

    for (uint8_t i = ADC_SAMPLING_CYCLES_32; i < ADC_SAMPLING_CYCLES_MAX; i++)
    {
        for(uint32_t j = 0; j < 100; j++)
        {            
            sum_code_0p6v += adc_compen_comm(false, ADC_SAMPLING_CYCLES_128, ADC_VREF_0P6V);
            sum_code_1p25v += adc_compen_comm(false, i, ADC_VREF_1P25V); 
            sum_code_shrt += adc_compen_comm(true, ADC_SAMPLING_CYCLES_128, ADC_VREF_1P25V);
        } 
        
        code_0p6v_new = (uint16_t)(sum_code_0p6v/100); 
        code_1p25v_new = (uint16_t)(sum_code_1p25v/100); 
        code_short_new = (uint16_t)(sum_code_shrt/100); 

        sum_code_0p6v = 0;
        sum_code_1p25v = 0;
        sum_code_shrt = 0;

        code_0p6v_old  = compen_param->data[ADC_SAMPLING_CYCLES_128-1].code_0p6;
        code_1p25v_old = compen_param->data[i-1].code_1p25;
        code_short_old  = compen_param_new->code_short[ADC_SAMPLING_CYCLES_128-1];

        cycles = TABLE_SAMPLE_2_CYCLE[ADC_SAMPLING_CYCLES_128 % ADC_SAMPLING_CYCLES_MAX];
        constant = (float)(cycles-2)*(cycles-1)*7/4;

        vos_core_old = (float)code_short_old - constant;
        vos_core_new = (float)code_short_new - constant;

        delta2 = (float)(code_0p6v_new - vos_core_new)/constant-1;
        delta1 = (float)(code_0p6v_old - vos_core_old)/constant-1;

        config_new.data[i-1].gain_error = (float)delta2*(config->data[i-1].gain_error)/delta1;    
        gain_error_all = (config_new.data[i-1].gain_error)/(config->data[i-1].gain_error);

        if(config->data[i-1].vos > 0x1FFF){
            config_new.data[i-1].vos = (code_1p25v_new - (uint16_t)((float)(code_1p25v_old - (config->data[i-1].vos-0x4000))*gain_error_all)) & 0x3fff;
        }
        else
        {
            config_new.data[i-1].vos = (code_1p25v_new - (uint16_t)((float)(code_1p25v_old - config->data[i-1].vos)*gain_error_all)) & 0x3fff;
        }
        config_new.data[i-1].vos_temp = config->data[i-1].vos_temp;
    }

    memcpy(&adc_ft_calib_config, &config_new, sizeof(ft_flash_gpadc_calib_t));

    adc_close();
}

#if ADC_USE_DEFAULT_CALIB_DATA
/*********************************************************************
 * @func adc_set_calibarate_param()
         brief note: adc config calibrate data

 * @parameters:
         cpft_adc_calib_t *config;

 * @return
         void
 *********************************************************************/
void adc_set_calibarate_param(cpft_adc_calib_t *config)
{
    cpft_sys_area_t cpft_flash_data;

    if(config->flash != NULL)
    {
        adc_env.use_efuse = false;

        memcpy(&cpft_flash_data, (void *)((0x00400000 + sfs_capacity()) - sizeof(cpft_sys_area_t)), sizeof(cpft_sys_area_t));
        
        if(cpft_flash_data.bitmap.data & BITMAP_NEW_GAIN_ERROR)
        {
            adc_env.use_new_gain_error = true;
        }
        else
        {
            adc_env.use_new_gain_error = false;
        }
        
        if(cpft_flash_data.bitmap.data & BITMAP_NEW_TEMP_COMPEN)
        {
            adc_env.use_new_temp_compen = true;
        if(config->flash_temp != NULL && config->flash_temp_new != NULL)
        {
            adc_set_new_calibarate_param(config->flash, config->flash_temp, config->flash_temp_new);
        }
            adc_ft_calib_vbat = config->flash_temp_new->vout_vbat;
        }
        else
        {
            adc_env.use_new_temp_compen = false;
            memcpy(&adc_ft_calib_config, config->flash, sizeof(ft_flash_gpadc_calib_t));
        }
    }
    else if(config->efuse != NULL)
    {
        adc_env.use_efuse = true;
        memcpy(&adc_cp_calib_config, config->efuse, sizeof(cp_efuse_gpadc_calib_t));
    }

}
#endif  //ADC_USE_DEFAULT_CALIB_DATA

/*********************************************************************
 * @func adc_temperature_cal_by_sw()
         brief note: adc read by channel

 * @parameters:
         out: gpadc code
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         temperature
 *********************************************************************/
float adc_temperature_cal_by_sw(int out, adc_sampling_cycles_t sampling_cycles)
{
    float ret = 0;
    float val = 0;
    float vcm = 1.25;
    float gain_err_cal_val = 0;
    float vos_temp_cal_val = 0;
    int16_t cycles = TABLE_SAMPLE_2_CYCLE[sampling_cycles % ADC_SAMPLING_CYCLES_MAX];
    float gain_cal = 8;
    int16_t vos_cal_val = 0;

#ifdef ADC_USE_CALIB

    vos_cal_val = adc_para[sampling_cycles].vos_set;
    gain_err_cal_val = adc_para[sampling_cycles].gainErr_cal;
    vos_temp_cal_val = adc_para[sampling_cycles].vosTemp_cal;

#else

    if(adc_env.use_efuse)
    {
        if(adc_cp_calib_config.vos > 0x1FFF)
        {
            /* vos < 0 */
            vos_cal_val = adc_cp_calib_config.vos - 0x4000;  
        }
        else
        {
            /* vos >= 0 */
            vos_cal_val = adc_cp_calib_config.vos;
        }
        gain_err_cal_val = (8192.0 / (float)adc_cp_calib_config.gain_error);  
        vos_temp_cal_val = (float)((int8_t)adc_cp_calib_config.vos_temp) / 4;      
    }
    else
    {
        if(adc_ft_calib_config.data[sampling_cycles-1].vos > 0x1FFF)
        {
            /* vos < 0 */
            vos_cal_val = adc_ft_calib_config.data[sampling_cycles-1].vos - 0x4000;  
        }
        else
        {
            /* vos >= 0 */
            vos_cal_val = adc_ft_calib_config.data[sampling_cycles-1].vos;
        }
        if(adc_env.use_new_gain_error)
        {
            gain_err_cal_val = adc_ft_calib_config.data[sampling_cycles-1].gain_error;
        }
        else
        {
            gain_err_cal_val = (512.0 / (float)((uint32_t)(adc_ft_calib_config.data[sampling_cycles-1].gain_error) & 0x3FF));
        }
        vos_temp_cal_val = (float)((int8_t)adc_ft_calib_config.data[sampling_cycles-1].vos_temp) / 4;
    }

#endif

    val = (float)(out - vos_cal_val) / gain_err_cal_val / 2 / cycles / cycles - 1;
    val = val * gain_cal + vcm;

    ret = (680.2 - (val * 1000)) / 1.878 + TEMPERATURE_CALCULATE - vos_temp_cal_val;

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("adc_temperature_cal_by_sw: value=%d, vbe=%f, vos=%x, gain_err=%f, vosTemp=%f\r\n", 
                                          out, val, vos_cal_val, gain_err_cal_val, vos_temp_cal_val);
    APP_LOG_INF("adc_temperature_cal_by_sw: %f\r\n", ret);
#endif

    return adc_round(ret);
}

/*********************************************************************
 * @func adc_voltage_cal_by_sw()
         brief note: adc read by channel

 * @parameters:
         out: gpadc code
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         voltage (mV)
 *********************************************************************/
float adc_voltage_cal_by_sw(int out, adc_sampling_cycles_t sampling_cycles)
{
    float ret = 0;
    float vcm = 1.25;
    float gain_err_cal_val = 0;
    int16_t cycles = TABLE_SAMPLE_2_CYCLE[sampling_cycles % ADC_SAMPLING_CYCLES_MAX];
    float gain_cal = 8;
    int16_t vos_cal_val = 0;

#ifdef ADC_USE_CALIB

    vos_cal_val = adc_para[sampling_cycles].vos_set;
    gain_err_cal_val = adc_para[sampling_cycles].gainErr_cal;

#else

    if(adc_env.use_efuse)
    {
        if(adc_cp_calib_config.vos > 0x1FFF)
        {
            /* vos < 0 */
            vos_cal_val = adc_cp_calib_config.vos - 0x4000;  
        }
        else
        {
            /* vos >= 0 */
            vos_cal_val = adc_cp_calib_config.vos;
        }
        gain_err_cal_val = (8192.0 / (float)adc_cp_calib_config.gain_error);        
    }
    else
    {
        if(adc_ft_calib_config.data[sampling_cycles-1].vos > 0x1FFF)
        {
            /* vos < 0 */
            vos_cal_val = adc_ft_calib_config.data[sampling_cycles-1].vos - 0x4000;  
        }
        else
        {
            /* vos >= 0 */
            vos_cal_val = adc_ft_calib_config.data[sampling_cycles-1].vos;
        }
        if(adc_env.use_new_gain_error)
        {
            gain_err_cal_val = adc_ft_calib_config.data[sampling_cycles-1].gain_error;
        }
        else
        {
            gain_err_cal_val = (512.0 / (float)((uint32_t)(adc_ft_calib_config.data[sampling_cycles-1].gain_error) & 0x3FF));
        }
    }

#endif

    ret = (float)(out - vos_cal_val) / gain_err_cal_val / 2 / cycles / cycles - 1;
    ret = ret * gain_cal + vcm;
    ret = ret * 1000; // v->mv

#ifdef DEBUG_ADC_ENALBE
    printf("adc_voltage_cal_by_sw: value=%d, vos=%x, gain_err=%f\r\n",
                                                        out, vos_cal_val, gain_err_cal_val);

    printf("adc_voltage_cal_by_sw: %f\r\n",ret);
#endif

    return ret;
}

/*********************************************************************
 * @func adc_voltage_read_by_channel()
         brief note: adc read by channel

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         voltage (mV)
 *********************************************************************/
uint16_t adc_voltage_read_by_channel(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles)
{
    adc_open();

    int ret_raw = (int)adc_calib_comm(channel, sampling_cycles, 0);
    float ret = adc_voltage_cal_by_sw(ret_raw, sampling_cycles);

    if(ADC_CH_1 == channel)
    {
        if(adc_env.use_new_temp_compen)
        {
        float k = 1 - (float)(adc_ft_calib_vbat - 1100.0) / 1100.0;
        ret = 3 * k * ret;
    }
        else
        {
            ret = 3 * ret;
        }
    }

    adc_close();

    return ret;
}

/*********************************************************************
 * @func adc_vbat_voltage_read()
         brief note: adc read vbat voltage

 * @parameters:
         none 

 * @return
         voltage (mV)
 *********************************************************************/
uint16_t adc_vbat_voltage_read(void)
{
    uint16_t ret = adc_voltage_read_by_channel(ADC_CH_1, ADC_SAMPLING_CYCLES_128);

    return ret;
}

/*********************************************************************
 * @func adc_temperature_read_normal()
         brief note: adc temperature read

 * @parameters:
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         temperature
 *********************************************************************/
int adc_temperature_read_normal(adc_sampling_cycles_t sampling_cycles)
{
    adc_open();

    /* power on GP_TEMP_GEN */
    REGW(&XH_GPADC->VOS_TEMP, MASK_1REG(GPADC_PD_GP_TEMP_GEN, 0));

    int ret_raw = (int)adc_calib_comm(ADC_CH_0, sampling_cycles, 0);
    float ret = adc_temperature_cal_by_sw(ret_raw, sampling_cycles);

    /* power down GP_TEMP_GEN */
    REGW(&XH_GPADC->VOS_TEMP, MASK_1REG(GPADC_PD_GP_TEMP_GEN, 1));

    adc_close();

    return ret;
}

/*********************************************************************
 * @func adc_temperature_read()
         brief note: adc temperature read for system init

 * @parameters:
         none

 * @return
         temperature
 *********************************************************************/
int adc_temperature_read(void)
{
    int temp = adc_temperature_read_normal(ADC_SAMPLING_CYCLES_128);

    return temp;
}

#ifdef ADC_USE_CALIB
/*********************************************************************
 * @func adc_convertAdcToEfuse()
         brief note: convert the struct data from adc_calib_t to EFUSE

 * @parameters:
         adc_calib_t *pData;
         cp_sys_area_t* pCalib

 * @return
         void
 *********************************************************************/
static void adc_convertAdcToEfuse(adc_calib_t *pData, cp_sys_area_t *pCalib)
{
    pCalib->analog_calib.adc_gain_error0 = (pData->gainErr_set & 0x1F);
    pCalib->gpadc_calib.adc_gain_error1 = ((pData->gainErr_set >> 5) & 0x1FF);
    pCalib->gpadc_calib.vos = (pData->vos_set & 0x3FFF);
    pCalib->gpadc_calib.vos_temp = pData->vosTemp_set;

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("\r\nEfuse_data=%x%x,%08x,%08x\r\n", pCalib->gpadc_calib.adc_gain_error1,
                                                   pCalib->analog_calib.adc_gain_error0,
                                                   pCalib->gpadc_calib.vos,
                                                   pCalib->gpadc_calib.vos_temp);
#endif
}

/*********************************************************************
 * @func adc_convertAdcToFlash()
         brief note: convert the struct data from adc_calib_t to Flash

 * @parameters:
         adc_calib_t *pData;
         cpft_sys_area_t* pCalib;
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         void
 *********************************************************************/
static void adc_convertAdcToFlash(adc_calib_t *pData, cpft_sys_area_t *pCalib, adc_sampling_cycles_t sampling_cycles)
{
    switch(sampling_cycles)
    {
    case ADC_SAMPLING_CYCLES_32:
        pCalib->gpadc32.gain_error = ((pData->gainErr_set >> 4) & 0x3FF);
        pCalib->gpadc32.vos = (pData->vos_set & 0x3FFF);
        pCalib->gpadc32.vos_temp = pData->vosTemp_set;
        pCalib->gpadc_gain_error_new.gain_error_32 = pData->gainErr_cal;

#ifdef DEBUG_ADC_ENALBE
        APP_LOG_INF("\r\nFlash_data=%08x,%08x,%08x,gain_error_new=%f\r\n", pCalib->gpadc32.gain_error,
                                                                         pCalib->gpadc32.vos,
                                                                         pCalib->gpadc32.vos_temp,
                                                                         pCalib->gpadc_gain_error_new.gain_error_32
                                                                         );
#endif
        break;

    case ADC_SAMPLING_CYCLES_64:
        pCalib->gpadc64.gain_error = ((pData->gainErr_set >> 4) & 0x3FF);
        pCalib->gpadc64.vos = (pData->vos_set & 0x3FFF);
        pCalib->gpadc64.vos_temp = pData->vosTemp_set;
        pCalib->gpadc_gain_error_new.gain_error_64 = pData->gainErr_cal;

#ifdef DEBUG_ADC_ENALBE
        APP_LOG_INF("\r\nFlash_data=%08x,%08x,%08x,gain_error_new=%f\r\n", pCalib->gpadc64.gain_error,
                                                                         pCalib->gpadc64.vos,
                                                                         pCalib->gpadc64.vos_temp,
                                                                         pCalib->gpadc_gain_error_new.gain_error_64
                                                                         );
#endif
        break;

    case ADC_SAMPLING_CYCLES_128:
        pCalib->gpadc128.gain_error = ((pData->gainErr_set >> 4) & 0x3FF);
        pCalib->gpadc128.vos = (pData->vos_set & 0x3FFF);
        pCalib->gpadc128.vos_temp = pData->vosTemp_set;
        pCalib->gpadc_gain_error_new.gain_error_128 = pData->gainErr_cal;

#ifdef DEBUG_ADC_ENALBE
        APP_LOG_INF("\r\nFlash_data=%08x,%08x,%08x,gain_error_new=%f\r\n", pCalib->gpadc128.gain_error,
                                                                         pCalib->gpadc128.vos,
                                                                         pCalib->gpadc128.vos_temp,
                                                                         pCalib->gpadc_gain_error_new.gain_error_128
                                                                         );
#endif				
        break;

    default:
        break;
    }
}

/*********************************************************************
 * @func adc_convertCompenToFlash()
         brief note: convert the compensation param to Flash

 * @parameters:
         adc_calib_compen_t* pData
         cpft_gpadc_calib_t* pCalib;
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         void
 *********************************************************************/
static void adc_convertCompenToFlash(adc_calib_compen_t *pData, cpft_sys_area_t *pCalib, adc_sampling_cycles_t sampling_cycles)
{
    switch(sampling_cycles)
    {
    case ADC_SAMPLING_CYCLES_32:
        pCalib->gpadc_compen32.gpadc_code_0p6  = (pData->code_0p6v  & 0xFFFF);
        pCalib->gpadc_compen32.gpadc_code_1p25 = (pData->code_1p25v & 0xFFFF);
        pCalib->gpadc_compen_new.gpadc_code_short_32 = (pData->code_shrt & 0xFFFF);

        break;

    case ADC_SAMPLING_CYCLES_64:
        pCalib->gpadc_compen64.gpadc_code_0p6  = (pData->code_0p6v  & 0xFFFF);
        pCalib->gpadc_compen64.gpadc_code_1p25 = (pData->code_1p25v & 0xFFFF);
        pCalib->gpadc_compen_new.gpadc_code_short_64 = (pData->code_shrt & 0xFFFF);

        break;

    case ADC_SAMPLING_CYCLES_128:
        pCalib->gpadc_compen128.gpadc_code_0p6  = (pData->code_0p6v  & 0xFFFF);
        pCalib->gpadc_compen128.gpadc_code_1p25 = (pData->code_1p25v & 0xFFFF);
        pCalib->gpadc_compen_new.gpadc_code_short_128 = (pData->code_shrt & 0xFFFF);
        pCalib->gpadc_compen_new.gpadc_vout_vbat = pData->adcOut_vbat;
			
        break;

    default:
        break;
    }
}

/*********************************************************************
 * @func adc_calib_first_step()
         brief note: calib the gpadc according to the cmd from FT, the first step use 0.8V on GPIO3

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

 * @return
         bool: false: fail   true: succ
 *********************************************************************/
bool adc_calib_first_step(adc_channel_t channel)
{
    adc_open();

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("\nadc_calib_first_step:\r\n");
#endif

    /* sampling cycles 32/64/128 */
    for (uint8_t i = ADC_SAMPLING_CYCLES_32; i < ADC_SAMPLING_CYCLES_MAX; i++)
    {
        /* when vip = 0.3v, get gpadc_out_real[15:0] */
        adc_para[i].adcOut_03v = adc_calib_comm(channel, i, 0);
        if ((adc_para[i].adcOut_03v > GPADC_OUT_UPPER_BOUND(GPADC_OUT_03_IDEAL_FORMULA_1[i])) ||
            (adc_para[i].adcOut_03v < GPADC_OUT_LOW_BOUND(GPADC_OUT_03_IDEAL_FORMULA_1[i])))
        {
            return false;
        }

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("vip=0.3v gpadc_out_real[%d]=%d\r\n", i, adc_para[i].adcOut_03v);
#endif
    }
    return true;
}

/*********************************************************************
 * @func adc_calib_second_step()
         brief note: calib the gpadc according to the cmd from FT

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         temper:    0x0: 20'C    0x4: 24'C
                    0x1: 21'C    0x5: 25'C
                    0x2: 22'C    0x6: 26'C
                    0x3: 23'C    0x7: 27'C
         calib_sel: 0x0: ADC_CALIB_CP
                    0x1: ADC_CALIB_FT

 * @output
         cpft_gpadc_calib_t* pCalibDataSave

 * @return
         bool: false: fail   true: succ
 *********************************************************************/
bool adc_calib_second_step(adc_channel_t channel, adc_temper_refer_t temp, adc_calib_sel_t calib_sel, void *pCalibDataSave)
{
#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("\nadc_calib_second_step:\r\n");
#endif

    /* sampling cycles 32/64/128 */
    for (uint8_t i = ADC_SAMPLING_CYCLES_32; i < ADC_SAMPLING_CYCLES_MAX; i++)
    {
        /* when vip = 3.0v, get gpadc_out_real[9:0] */
        adc_para[i].adcOut_30v = adc_calib_comm(channel, i, 0);
        if ((adc_para[i].adcOut_30v > GPADC_OUT_UPPER_BOUND(GPADC_OUT_30_IDEAL_FORMULA_1[i])) ||
            (adc_para[i].adcOut_30v < GPADC_OUT_LOW_BOUND(GPADC_OUT_30_IDEAL_FORMULA_1[i])))
        {
            return false;
        }

        /* calib gain err[9:0] */
        adc_para[i].gainErr_cal = (float)(adc_para[i].adcOut_30v - adc_para[i].adcOut_03v) /
                                  (float)GPADC_OUT_IDEAL_DIFF(GPADC_OUT_30_IDEAL[i], GPADC_OUT_03_IDEAL[i]);
        adc_para[i].gainErr_set = (uint16_t)(8192.0 / adc_para[i].gainErr_cal);

        /* calib vos[9:0] */
        adc_para[i].vos_set = (int16_t)((float)adc_para[i].adcOut_30v - adc_para[i].gainErr_cal * (float)GPADC_OUT_30_IDEAL[i]);

        /* calib temperature */
        adc_para[i].vosTemp_cal = adc_calib_temper(i, temp, adc_para[i].gainErr_cal, adc_para[i].vos_set);
        adc_para[i].vosTemp_set = (int8_t)(adc_para[i].vosTemp_cal * 4);

        if(ADC_CALIB_CP == calib_sel)
        {
            if (pCalibDataSave != NULL && ADC_SAMPLING_CYCLES_128 == i)
            {
                adc_convertAdcToEfuse((adc_calib_t *)(&adc_para[i]), (cp_sys_area_t *)pCalibDataSave);
            }
        }
        else if(ADC_CALIB_FT == calib_sel)
        {
            if (pCalibDataSave != NULL)
            {
                adc_convertAdcToFlash((adc_calib_t *)(&adc_para[i]), (cpft_sys_area_t *)pCalibDataSave, i);
            }
        }

#ifdef DEBUG_ADC_ENALBE
        APP_LOG_INF("\nvip=3.0v, gpadc_out_real[%d]=%d\r\n", i, adc_para[i].adcOut_30v);
        APP_LOG_INF("gainErr_cal[%d] = %f, gainErr_set[%d] = %x\r\n", i, adc_para[i].gainErr_cal, i, adc_para[i].gainErr_set);
        APP_LOG_INF("vos_set[%d]: %x\r\n", i, adc_para[i].vos_set);
        APP_LOG_INF("vosTemp_cal[%d] = %f, vosTemp_set[%d] = %x\r\n", i, adc_para[i].vosTemp_cal, i, adc_para[i].vosTemp_set);
#endif
    }

    adc_close();

    return true;
}

/*********************************************************************
 * @func adc_calib_third_step()
         brief note: start adc to get vbat output volt and a output volt value and judge the result

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         int inputVoltValue :input volt at the unit of micro v

 * @return
         bool: false: fail   true: succ
 *********************************************************************/
bool adc_calib_third_step(adc_channel_t channel, int inputVoltValue)
{
    int sVoltOut = 0;
    int sVoltDiff = 0;
    int VbatVolt = 1100;
    int sTempOut = 0;

#ifdef DEBUG_ADC_ENALBE
    APP_LOG_INF("\nadc_calib_third_step:\r\n");
#endif

    /* start adc to get output volt value and judge the result */
    for (uint8_t i = ADC_SAMPLING_CYCLES_32; i < ADC_SAMPLING_CYCLES_MAX; i++)
    {
        /* start adc to get output volt value and judge the result */
        sVoltOut = adc_voltage_read_by_channel(channel, i);
        sVoltDiff = sVoltOut - inputVoltValue;

#ifdef DEBUG_ADC_ENALBE
        APP_LOG_INF("inputVolt = %d, sampling cycles = %d\r\n", inputVoltValue, i);
        APP_LOG_INF("gpadc_out: %d, diff: %d\r\n", sVoltOut, sVoltDiff);
#endif

        if ((sVoltDiff < -50) || (sVoltDiff > 50))
        {
            return false;
        }

        /* start adc to get vbat output volt value and judge the result */
        sVoltOut = adc_voltage_read_by_channel(ADC_CH_1, i);
        sVoltDiff = sVoltOut - VbatVolt;

#ifdef DEBUG_ADC_ENALBE
        APP_LOG_INF("vbat_out: %d, diff: %d\r\n", sVoltOut, sVoltDiff);
#endif

        if ((sVoltDiff < -50) || (sVoltDiff > 50))
        {
            return false;
        }

        /* start adc to get temperature output and print the result */
        sTempOut = adc_temperature_read();

#ifdef DEBUG_ADC_ENALBE
        APP_LOG_INF("temp_out: %d'C\n", sTempOut);
#endif
    }

	return true;
}

/*********************************************************************
 * @func adc_calib_compen_step()
         brief note: recording the gpadc compensation calibration param

 * @parameters:
         calib_sel: 0x0: ADC_CALIB_CP
                    0x1: ADC_CALIB_FT

 * @output
         cpft_sys_area_t *pCalibDataSave

 * @return
         bool: false: fail   true: succ
 *********************************************************************/
bool adc_calib_compen_step(adc_calib_sel_t calib_sel, cpft_sys_area_t *pCalibDataSave)
{
    adc_calib_compen_t adc_compen_para = {0};

    int sum_code_0p6v = 0;
    int sum_code_1p25v = 0;
    int sum_code_shrt = 0;
    float sum_vout_vbat = 0;
    int ret_raw = 0;

    if(ADC_CALIB_FT == calib_sel)
    {
        if (pCalibDataSave != NULL)
        {
            adc_open();

            for (uint8_t i = ADC_SAMPLING_CYCLES_32; i < ADC_SAMPLING_CYCLES_MAX; i++)
            {
                for(uint32_t j = 0; j < 100; j++)
                { 
                    sum_code_0p6v += adc_compen_comm(false, i, ADC_VREF_0P6V);
                    sum_code_1p25v += adc_compen_comm(false, i, ADC_VREF_1P25V);
                    sum_code_shrt += adc_compen_comm(true, i, ADC_VREF_1P25V);
                    if(ADC_SAMPLING_CYCLES_128 == i)
                    {
                        ret_raw = (int)adc_calib_comm(ADC_CH_1, i, 0);
                        sum_vout_vbat += adc_voltage_cal_by_sw(ret_raw, i);
                    }
                }

                adc_compen_para.code_0p6v = (uint16_t)(sum_code_0p6v/100);
                adc_compen_para.code_1p25v = (uint16_t)(sum_code_1p25v/100);
                adc_compen_para.code_shrt = (uint16_t)(sum_code_shrt/100);
                if(ADC_SAMPLING_CYCLES_128 == i)
                {
                    adc_compen_para.adcOut_vbat = (float)(sum_vout_vbat/100);
                }

                sum_code_0p6v = 0;
                sum_code_1p25v = 0;
                sum_code_shrt = 0;
                sum_vout_vbat = 0;

                adc_convertCompenToFlash(&adc_compen_para, pCalibDataSave, i);
            }

            adc_close();
        }
        else
        {
            return false;
        }
    }
    
	return true;
}
#endif  //ADC_USE_CALIB

/*********************************************************************
 * @func adc_voltage_read_single_begin_nostop()
         brief note: read voltage single channel nostop begin

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         void
 *********************************************************************/
void adc_voltage_read_single_begin_nostop(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles)
{
    adc_open();

    if(ADC_CH_0 == channel)
    {
        /* power on GP_TEMP_GEN */
        REGW(&XH_GPADC->VOS_TEMP, MASK_1REG(GPADC_PD_GP_TEMP_GEN, 0));
    }

    /* adc mode, 0:continuous conversion  1:single conversion */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_LIFE, 0));

    /* set digital sampling cycles, default 0x4 */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_AVG_CFG, 0x4));

    /* channel enable */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_VECT, 1 << channel));

    /* set adc vref */
    REGW(((uint32_t *)&XH_GPADC->CH_0_CFG) + channel, MASK_1REG(GPADC_MODE_VERF, ADC_VREF_1P25V));

    /* set adc pga gain */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_PGA_GAIN, ADC_PGA_GAIN_0P125));

    /* set analog sampling cycles */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_GP_BIT_SEL, sampling_cycles));

    /* set AUTO_COMPEN, default 0 */
    REGW(&XH_GPADC->SMP_OUT_CTL, MASK_1REG(GPADC_AUTO_COMPEN, 0));

    /* power-on ADC */
    REGW(&XH_GPADC->ADC_CFG0, MASK_1REG(GPADC_ADC_START, 1));

    /* set software triggle, the trigger is always true */
    REGW(&XH_GPADC->ADC_SW_TRIGGER, MASK_1REG(GPADC_SW_TRIGGER_TRUE, 1));

    while ((XH_GPADC->INTR & (1 << channel)) != (1 << channel));

    XH_GPADC->INTR = XH_GPADC->INTR;

    /* receive first wrong data and abandon */
    REGR(((uint32_t *)&XH_GPADC->CH_0_DATA) + channel, MASK_POS(GPADC_DATA_LOW));
}

/*********************************************************************
 * @func adc_voltage_read_single_nostop()
         brief note: read voltage single channel nostop

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

 * @return
         voltage (mV)
 *********************************************************************/
uint16_t adc_voltage_read_single_nostop(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles)
{
    while ((XH_GPADC->INTR & (1 << channel)) != (1 << channel));

    XH_GPADC->INTR = XH_GPADC->INTR;

    int ret_raw = REGR(((uint32_t *)&XH_GPADC->CH_0_DATA) + channel, MASK_POS(GPADC_DATA_LOW));

    float ret = adc_voltage_cal_by_sw(ret_raw, sampling_cycles);

    return ret;
}

/*********************************************************************
 * @func adc_voltage_read_multi_begin_nostop()
         brief note: read voltage multi channel nostop begin

 * @parameters:
         chBmp:     channel select

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         void
 *********************************************************************/
void adc_voltage_read_multi_begin_nostop(uint16_t chBmp, adc_sampling_cycles_t sampling_cycles)
{
    adc_open();

    /* adc mode, 0:continuous conversion  1:single conversion */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_LIFE, 0));

    /* set digital sampling cycles, default 0x4 */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_AVG_CFG, 0x4));

    /* interruption order, 0:ch9 to ch0 1:ch0 to ch9 */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SCANDIR, 1));

    /* channel enable */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_VECT, chBmp));

    for (int32_t i = 0; i < ADC_CH_MAX; i++)
    {
        if (0x01 == ((chBmp >> i) & 0x1))
        {
            REGW(((uint32_t *)&XH_GPADC->CH_0_CFG) + i, MASK_1REG(GPADC_MODE_VERF, ADC_VREF_1P25V));
        }
    }

    /* set adc pga gain */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_PGA_GAIN, ADC_PGA_GAIN_0P125));

    /* set analog sampling cycles */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_GP_BIT_SEL, sampling_cycles));

    /* set AUTO_COMPEN, default 0 */
    REGW(&XH_GPADC->SMP_OUT_CTL, MASK_1REG(GPADC_AUTO_COMPEN, 0));

    /* set the interval of interruption, minimum interval:0x0~0xE */
    REGW(&XH_GPADC->DLY_CFG, MASK_1REG(GPADC_CFG_CHG_DLY, 0xE));

    /* mask adc over interrupt */
    REGW(&XH_GPADC->INTR, MASK_1REG(GPADC_OVR, 1));

    REGW(&XH_GPADC->ADC_CFG0, MASK_1REG(GPADC_ADC_START, 0x1));

    REGW(&XH_GPADC->ADC_SW_TRIGGER, MASK_1REG(GPADC_SW_TRIGGER_TRUE, 1));
    
    /* receive first wrong data and abandon */
    for (int32_t j = 0; j < ADC_CH_MAX; j++)
    {
        if (0x01 == ((chBmp >> j) & 0x1))
        {
            /* check interrupt from ch0 to ch9 */
            while ((XH_GPADC->INTR & (1 << j)) != (1 << j));

            XH_GPADC->INTR = XH_GPADC->INTR;

            REGR(((uint32_t *)&XH_GPADC->CH_0_DATA) + j, MASK_POS(GPADC_DATA_LOW));
        }
    }
}

/*********************************************************************
 * @func adc_voltage_read_multi_nostop()
         brief note: read voltage multi channel nostop

 * @parameters:
         chBmp:     channel select

         cha_val:   voltage (mV)

 * @return
         void
 *********************************************************************/
void adc_voltage_read_multi_nostop(uint16_t chBmp, float *cha_val,
                                   adc_sampling_cycles_t sampling_cycles)
{
    for (int32_t j = 0; j < ADC_CH_MAX; j++)
    {
        if (0x01 == ((chBmp >> j) & 0x1))
        {
            /* check interrupt from ch0 to ch9 */
            while((XH_GPADC->INTR & (1 << j)) != (1 << j));

            XH_GPADC->INTR = XH_GPADC->INTR;

            int ret_raw = REGR(((uint32_t *)&XH_GPADC->CH_0_DATA) + j, MASK_POS(GPADC_DATA_LOW));

            *cha_val++ = adc_voltage_cal_by_sw(ret_raw, sampling_cycles);
        }
    }
}

/*********************************************************************
 * @func adc_temperature_read_begin_nostop()
         brief note: read temperature nostop begin

 * @parameters:
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         void
 *********************************************************************/
void adc_temperature_read_begin_nostop(adc_sampling_cycles_t sampling_cycles)
{
    adc_voltage_read_single_begin_nostop(ADC_CH_0, sampling_cycles);
}

/*********************************************************************
 * @func adc_temperature_read_nostop()
         brief note: read temperature nostop

 * @parameters:
         none

 * @return
         temperature
 *********************************************************************/
int adc_temperature_read_nostop(adc_sampling_cycles_t sampling_cycles)
{
    /* power-on ADC */
    REGW(&XH_GPADC->ADC_CFG0, MASK_1REG(GPADC_ADC_START, 1));

    /* set software triggle, the trigger is always true */
    REGW(&XH_GPADC->ADC_SW_TRIGGER, MASK_1REG(GPADC_SW_TRIGGER_TRUE, 1));

    while ((XH_GPADC->INTR & (1 << ADC_CH_0)) != (1 << ADC_CH_0));

    XH_GPADC->INTR = XH_GPADC->INTR;

    int ret_raw = REGR(((uint32_t *)&XH_GPADC->CH_0_DATA) + ADC_CH_0, MASK_POS(GPADC_DATA_LOW));

    float ret = adc_temperature_cal_by_sw(ret_raw, sampling_cycles);

    return ret;
}

#ifdef ADC_DMA_ENALBE
/*********************************************************************
 * @func adc_dma_config()
         brief note: ADC DMA configuration

 * @parameters:
         dma: NULL: ADC will allocate a new dma; Not NULL: use it as adc dma

         config: ADC dma configuration structure

         src_addr: the adc output address

 * @return
         NULL No DMA valid or error happen
         object Current used DMA object
 *********************************************************************/
XH_DMA_CH_Type *adc_dma_config(XH_DMA_CH_Type *dma, const adc_dma_config_t *config, uint32_t *src_addr)
{
    if (dma == NULL)
        dma = dma_allocate();

    if (dma)
    {
        bool res;
        dma_id_t id;
        dma_config_t dconfig;
        dma_slave_buswidth_t buswidth = DMA_SLAVE_BUSWIDTH_32BITS;

        id = ADC_DMA_ID;
        *src_addr = (uint32_t)&XH_GPADC->CH_DMA_DATA;

        dconfig.slave_id = id;
        dconfig.direction = DMA_DEV_TO_MEM;
        dconfig.src_addr_width = buswidth;
        dconfig.dst_addr_width = buswidth;
        dconfig.src_burst = DMA_BURST_LEN_1UNITS;
        dconfig.dst_burst = DMA_BURST_LEN_1UNITS;
        dconfig.dev_flow_ctrl = false;
        dconfig.priority = 0;
        dconfig.callback = config->callback;

        dconfig.lli.enable = false;
        dconfig.lli.use_fifo = config->use_fifo;
        dconfig.lli.src_addr = *src_addr;
        dconfig.lli.dst_addr = (uint32_t)config->buffer;
        dconfig.lli.block_num = config->block_num;
        dconfig.lli.block_len = config->buffer_len / config->block_num;
        dconfig.lli.llip = config->block_llip;

        res = dma_config(dma, &dconfig);
        if (!res)
        {
            dma_release(dma);
            return NULL;
        }

        /* Enable ADC */
        REGW1(&XH_GPADC->ADC_CFG1, GPADC_DMA_EN_MASK);
        REGW1(&XH_GPADC->ADC_SW_TRIGGER, GPADC_SW_TRIGGER_MASK);

        APP_LOG_INF("adc dma config %08x, %08x\n", XH_GPADC->ADC_CFG1, XH_GPADC->ADC_SW_TRIGGER);
    }

    return dma;
}
#endif

/*********************************************************************
 * @func adc_voltage_read_by_channel_irq()
         brief note: use irq handle data

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

         cb:  adc_callback

 * @return
         bool
 *********************************************************************/
bool adc_voltage_read_by_channel_irq(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles, adc_callback_t cb)
{
    adc_open();

    /* set interrupt */
    XH_GPADC->INTR_MSK = 0;
    XH_GPADC->INTR = GPADC_INTR_ALL_MASK;

    /* adc mode, 0:continuous conversion  1:single conversion */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_LIFE, 0));

    /* set digital sampling cycles, default 0x4 */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_AVG_CFG, 0x4));

    /* channel enable */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_VECT, 1 << channel));

    /* set adc vref */
    REGW(((uint32_t *)&XH_GPADC->CH_0_CFG) + channel, MASK_1REG(GPADC_MODE_VERF, ADC_VREF_1P25V));

    /* set adc pga gain */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_PGA_GAIN, ADC_PGA_GAIN_0P125));

    /* set analog sampling cycles */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_GP_BIT_SEL, sampling_cycles));

    /* set AUTO_COMPEN, default 0 */
    REGW(&XH_GPADC->SMP_OUT_CTL, MASK_1REG(GPADC_AUTO_COMPEN, 0));

    XH_GPADC->INTR_MSK |= (1 << channel);

    /* bind callback */
    adc_env.callback[channel] = cb;

    adc_env.sampling_cycles = sampling_cycles;

    if (cb != NULL)
    {
        NVIC_SetPriority(ADC_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(ADC_IRQn);
        NVIC_EnableIRQ(ADC_IRQn);

        /* power-on ADC */
        REGW(&XH_GPADC->ADC_CFG0, MASK_1REG(GPADC_ADC_START, 1));

        /* set software triggle, the trigger is always true */
        REGW(&XH_GPADC->ADC_SW_TRIGGER, MASK_1REG(GPADC_SW_TRIGGER_TRUE, 1));

        return true;
    }
    else
    {
        return false;
    }
}

/*********************************************************************
 * @func adc_temperature_read_irq()
         brief note: use irq handle data

 * @parameters:
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

         cb:  adc_callback

 * @return
         bool
 *********************************************************************/
bool adc_temperature_read_irq(adc_sampling_cycles_t sampling_cycles, adc_callback_t cb)
{
    adc_open();

    /* power on GP_TEMP_GEN */
    REGW(&XH_GPADC->VOS_TEMP, MASK_1REG(GPADC_PD_GP_TEMP_GEN, 0));

    /* set interrupt */
    XH_GPADC->INTR_MSK = 0;
    XH_GPADC->INTR = GPADC_INTR_ALL_MASK;

    /* adc mode, 0:continuous conversion  1:single conversion */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_LIFE, 2));

    /* set digital sampling cycles, default 0x4 */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_AVG_CFG, 0x4));

    /* channel enable */
    REGW(&XH_GPADC->ADC_CFG2, MASK_1REG(GPADC_SEQ_VECT, 1 << ADC_CH_0));

    /* set adc vref */
    REGW(((uint32_t *)&XH_GPADC->CH_0_CFG) + ADC_CH_0, MASK_1REG(GPADC_MODE_VERF, ADC_VREF_1P25V));

    /* set adc pga gain */
    REGW(&XH_AUDIO->CODEC_ANA_CTRL_1, MASK_1REG(AU_AU_PGA_GAIN, ADC_PGA_GAIN_0P125));

    /* set analog sampling cycles */
    REGW(&XH_GPADC->ADC_CFG1, MASK_1REG(GPADC_GP_BIT_SEL, sampling_cycles));

    /* set AUTO_COMPEN, default 0 */
    REGW(&XH_GPADC->SMP_OUT_CTL, MASK_1REG(GPADC_AUTO_COMPEN, 0));

    XH_GPADC->INTR_MSK |= (1 << ADC_CH_0);

    /* bind callback */
    adc_env.callback[ADC_CH_0] = cb;

    adc_env.sampling_cycles = sampling_cycles;

    if (cb != NULL)
    {
        NVIC_SetPriority(ADC_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(ADC_IRQn);
        NVIC_EnableIRQ(ADC_IRQn);

        /* power-on ADC */
        REGW(&XH_GPADC->ADC_CFG0, MASK_1REG(GPADC_ADC_START, 1));

        /* set software triggle, the trigger is always true */
        REGW(&XH_GPADC->ADC_SW_TRIGGER, MASK_1REG(GPADC_SW_TRIGGER_TRUE, 1));

        return true;
    }
    else
    {
        return false;
    }
}

/*********************************************************************
 * @func ADC_IRQHandler()

 * @parameters:
         none

 * @return
         void
 *********************************************************************/
void ADC_IRQHandler(void)
{
    float ret = 0;
    int valueData = 0;

    uint32_t intr_mask = XH_GPADC->INTR_MSK;
    XH_GPADC->INTR_MSK = 0;

    uint32_t src = XH_GPADC->INTR & GPADC_INTR_ALL_MASK;
    XH_GPADC->INTR = src;

    uint8_t i = 0;
    for (i = 0; i < ADC_CH_MAX; i++)
    {
        if (adc_env.callback[i] != NULL)
        {
            if (((src & GPADC_EOC_0_MASK) || (src & GPADC_EOC_1_MASK) || (src & GPADC_EOC_2_MASK) || (src & GPADC_EOC_3_MASK) || 
				 (src & GPADC_EOC_4_MASK) || (src & GPADC_EOC_5_MASK) || (src & GPADC_EOC_6_MASK) || (src & GPADC_EOC_8_MASK) || 
				 (src & GPADC_EOC_9_MASK)) && (src & (1 << i)))
            {
                valueData = REGR(((uint32_t *)&XH_GPADC->CH_0_DATA) + i, MASK_POS(GPADC_DATA_LOW));

                if(ADC_CH_0 == i)
                {
                    ret = adc_temperature_cal_by_sw(valueData, adc_env.sampling_cycles);

                    /* power down GP_TEMP_GEN */
                    REGW(&XH_GPADC->VOS_TEMP, MASK_1REG(GPADC_PD_GP_TEMP_GEN, 1));
                }
                else
                {
                    ret = adc_voltage_cal_by_sw(valueData, adc_env.sampling_cycles);
                }
                adc_env.recv_num++;
                
            }
            else if (adc_env.callback[i] != NULL &&
                    ((src & GPADC_EOS_MASK) || (src & GPADC_EOA_MASK) || (src & GPADC_OVR_MASK) || (src & (1 << i))))
            {
                ret = 0;
                adc_env.recv_num = 2;;
            }
            break;
        }
    }

    XH_GPADC->INTR_MSK = intr_mask;

    /* abandon first wrong data */
    if(adc_env.recv_num == 2)
    {
        /* disable interrupt */
        NVIC_DisableIRQ(ADC_IRQn);

        adc_close();
        
        adc_env.recv_num = 0;
        
        adc_env.callback[i](src, (int)ret);
    }
}

/** @} */
