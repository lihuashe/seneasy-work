/**
 * @file calib.c
 * @brief
 * @date 2016/12/26 20:15:02
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
#include "features.h"
#include "peripheral.h"
#include "co.h"

/*********************************************************************
 * MACROS
 */

//#define CALIB_DEBUG
#ifdef CALIB_DEBUG
#define calib_log(format, ...) log_debug(format, ## __VA_ARGS__)
#else
#define calib_log(format, ...)
#endif

// ct_fdoubler use same with lpf
#define RC_IF_GREATER_DO(sum, lpf, tia, auadc) if (rc_sum > (sum)) calib_sys_rc_value_set(lpf, tia, auadc);
#define RC_EI_GREATER_DO(sum, lpf, tia, auadc) else if (rc_sum > (sum)) calib_sys_rc_value_set(lpf, tia, auadc);
#define RC_EL_GREATER_DO(lpf, tia, auadc)      else calib_sys_rc_value_set(lpf, tia, auadc);

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
    struct
    {
//        uint16_t vco_afc_lut[80];
//        uint8_t  vco_amp;
        uint32_t kdco_1m[2];
        uint32_t kdco_2m[2];
//        uint32_t dcoc_lut[63];
        uint16_t agc_pif_os;
        uint16_t agc_pif_os_2m; // not for XH98xx
//        uint32_t txfe_pa_gain[18];
#if !defined(CONFIG_XH98xxD) && !defined(CONFIG_XH98xxF)
        uint16_t tia_dcoc_lut[9];
#endif
    }rf;

    struct
    {
        // calib with Power ON
        uint32_t rc_tia_lpf;
        uint8_t rc_tune_auadc;
    }sys;
}calib_t;

typedef struct
{
    uint8_t padr1s_ct_mo;
    uint8_t pacore_ct_mo;
    uint8_t tx_atten_mo;
}calib_rxfe_pa_t;

/*********************************************************************
 * CONSTANTS
 */
const static calib_rxfe_pa_t txfe_pa_table[] =
{
    /*                          #1      #2      #lose */
    /* 1  */ {0x1, 0x1, 0x0}, //-25.05  -25.12  0
    /* 2  */ {0x1, 0x1, 0x1}, //-21.09  -21.96  0
    /* 3  */ {0x1, 0x1, 0x2}, //-17.08  -17.01  0
    /* 4  */ {0x1, 0x1, 0x3}, //-4.98   -4.63   0
    /* 5  */ {0x2, 0x1, 0x3}, //-2.24   -2.04   0
    /* 6  */ {0x1, 0x2, 0x3}, //-1.7    -1.39   0
    /* 7  */ {0x2, 0x2, 0x3}, // 0.81    1.16   0
    /* 8  */ {0x4, 0x2, 0x3}, // 1.14    1.39   0
    /* 9  */ {0x4, 0x3, 0x3}, // 2.76    3      0
    /* 10 */ {0x4, 0x4, 0x3}, // 3.58    3.79   0
    /* 11 */ {0x4, 0x5, 0x3}, // 3.92    4.06   0
    /* 12 */ {0x4, 0x6, 0x3}, // 4.24    4.38   0
    /* 13 */ {0x4, 0x7, 0x3}, // 4.52    4.65   0
    /* 14 */ {0x4, 0x8, 0x3}, // 4.74    4.86   0
    /* 15 */ {0x4, 0x9, 0x3}, // 4.91    5      0
    /* 16 */ {0x4, 0xA, 0x3}, // 5.04    5.12   0
    /* 17 */ {0x4, 0xB, 0x3}, // 5.15    5.23   0
    /* 18 */ {0x4, 0xC, 0x3}, // 5.21    5.32   0
};

/*********************************************************************
 * LOCAL VARIABLES
 */

/*static*/ calib_t calib_env; // NOTE: Must in unint section

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief  calib patch
 *
 * @param[in] temperature  temperature
 **/
static void calib_patch(int16_t temperature)
{
}

#if 0
/**
 * @brief  adcoc
 **/
static uint32_t adcoc(int adc_out, uint32_t sel_dcoc_fs)
{
    int adc_tmp;

    if(sel_dcoc_fs)
        adc_tmp = (adc_out - 176) / 5;
    else
        adc_tmp = (adc_out - 96) / 10;

    if(adc_tmp < 0)
        adc_tmp = 0;
    else if (adc_tmp > 31)
        adc_tmp = 31;

    return adc_tmp;
}
#endif

#if 0
/**
 * @brief  calib rf track reset
 **/
static void calib_rf_track_reset(void)
{
    int i;

    for(i=0; i<80; ++i)
    {
        // read
        REGW(&XH_DAIF->PLL_LUT_DBG, MASK_2REG(DAIF_PLL_LUT_LU, 1, DAIF_PLL_LUT_IDX, i));
        // wait lut read done
        while(XH_DAIF->PLL_LUT_DBG & DAIF_PLL_LUT_LU_MASK);

        // write
        REGW(&XH_DAIF->PLL_LUT_DBG, MASK_4REG(
                    DAIF_PLL_LUT_DATA_TRACK, 0x24,
                    DAIF_PLL_LUT_DATA_TUNE, REGR(&XH_DAIF->PLL_LUT_DBG, MASK_POS(DAIF_PLL_LUT_DATA_TUNE)),
                    DAIF_PLL_LUT_IDX, i,
                    DAIF_PLL_LUT_WR, 1));
        // wait lut wr done
        while(XH_DAIF->PLL_LUT_DBG & DAIF_PLL_LUT_WR_MASK);

    }
}
#endif

/**
 * @brief calib_rf_afc()
 *
 * Auto frequency calibration
 *
 * @return
 **/
static void calib_rf_afc(void)
{
    int i;

    //sw power on analog blocks
    // power on pll analog
    REGW(&XH_DAIF->PD_CFG1, MASK_4REG(DAIF_PD_PLL_ALL_ME,1, DAIF_PD_PLL_ALL_MO,0, DAIF_PD_VCDET_ME,1, DAIF_PD_VCDET_MO,0));
#if !defined(CONFIG_XH98xxD) && !defined(CONFIG_XH98xxF)
    REGW(&XH_DAIF->PD_CFG2, MASK_2REG(DAIF_PD_TXDAC_PLL_MO,0, DAIF_PD_TXDAC_PLL_ME,1));
#endif
#if defined(CONFIG_XH98xxF)
    REGW1(&XH_DAIF->PLL_CTRL0, DAIF_PLL_POWER_MASK);
#endif

    // digital ctrl
    REGW(&XH_DAIF->VCO_CTRL0, MASK_1REG(DAIF_AFC_EN_ME,0));

    //initial afc/vtrack table
    for(i=0; i<80; ++i)
    {
        REGW(&XH_DAIF->PLL_LUT_DBG, MASK_3REG(
                    DAIF_PLL_LUT_DATA, 0x1124,
                    DAIF_PLL_LUT_IDX, i,
                    DAIF_PLL_LUT_WR, 1));
        // wait lut wr done
        while(XH_DAIF->PLL_LUT_DBG & DAIF_PLL_LUT_WR_MASK);
    }

    //=================VCO AFC TEST===============================
    //do amp cali
    REGW1(&XH_DAIF->VCO_CTRL1, DAIF_PEAKDET_START_MASK);
    // wait amp done
    while(XH_DAIF->VCO_CTRL1 & DAIF_PEAKDET_START_MASK);

    //do vco afc
    REGW1(&XH_DAIF->VCO_CTRL0, DAIF_AFC_START_MASK);
    // wait afc done
    while(XH_DAIF->VCO_CTRL0 & DAIF_AFC_START_MASK);

    //pd_lotx power on
    REGW(&XH_DAIF->PD_CFG1, MASK_2REG(DAIF_PD_LOTX_MO,0, DAIF_PD_LOTX_ME,1));

    //do kdco -- 1M
    REGW(&XH_DAIF->VCO_CTRL1, MASK_2REG(DAIF_BLE_1M_2M_SEL_MO,0, DAIF_BLE_1M_2M_SEL_ME,1));
    // try kdco 1M mode
    REGW1(&XH_DAIF->VCO_CTRL0, DAIF_KDCO_START_MASK);
    //wait kdco done
    while(XH_DAIF->VCO_CTRL0 & DAIF_KDCO_START_MASK);

    //do kdco -- 2M
    REGW(&XH_DAIF->VCO_CTRL1, MASK_2REG(DAIF_BLE_1M_2M_SEL_MO,1, DAIF_BLE_1M_2M_SEL_ME,1));
    // try kdco 2M mode
    REGW1(&XH_DAIF->VCO_CTRL0, DAIF_KDCO_START_MASK);
    //wait kdco done
    while(XH_DAIF->VCO_CTRL0 & DAIF_KDCO_START_MASK);

    // restore 1m/2m FSM ctrl
    REGW(&XH_DAIF->VCO_CTRL1, MASK_1REG(DAIF_BLE_1M_2M_SEL_ME,0));

    //make analog blocks power controlled by trx FSM
    // change power control by FSM
    //pd_pll_all
    REGW(&XH_DAIF->PD_CFG1, MASK_2REG(DAIF_PD_PLL_ALL_ME,0, DAIF_PD_VCDET_ME,0));
#if !defined(CONFIG_XH98xxD) && !defined(CONFIG_XH98xxF)
    REGW(&XH_DAIF->PD_CFG2, MASK_1REG(DAIF_PD_TXDAC_PLL_ME,0));
#endif
    //pd_lotx
    REGW(&XH_DAIF->PD_CFG1, MASK_1REG(DAIF_PD_LOTX_ME,0));
}

/**
 * @brief calib_rf_agc()
 *
 * RX agc calibration
 *
 * @return
 **/
static void calib_rf_agc(void)
{
    int i;
    uint32_t agc_pif_os[2];

    REGW(&XH_DAIF->LNA_MIX_CFG, MASK_6REG(DAIF_GAIN_LNA_MO,5, DAIF_GAIN_LNA_ME,1,
        DAIF_GAIN_ATTEN_MO,2, DAIF_GAIN_ATTEN_ME,1,
        DAIF_EN_ATTEN_MO,0, DAIF_EN_ATTEN_ME,1));

    REGW(&XH_DAIF->TIA_LPF_CFG, MASK_2REG(DAIF_GAIN_TIA_MO,2, DAIF_GAIN_TIA_ME,1));

    REGW(&XH_DAIF->PD_CFG1, MASK_2REG(DAIF_RST_PDTIF_MO,0, DAIF_RST_PDTIF_ME,1));

    REGW(&XH_DAIF->TRX_SW_CFG, MASK_2REG(DAIF_TRX_EN_ME,1, DAIF_TRX_EN_MO,0));

    REGW(&XH_DAIF->MAIN_ST_CFG1, MASK_1REG(DAIF_AGC_SYNC_TIME,0xF));
    REGW(&XH_DAIF->AGC_CFG2, MASK_1REG(DAIF_PIF_OS_FLAG, 0));

    // frequency
    REGW(&XH_DAIF->FREQ_CFG0, MASK_2REG(DAIF_FREQ_REG_ME,1, DAIF_FREQ_REG_MO,2404));

    for (i=0; i<2; ++i)
    {
        REGW(&XH_DAIF->TIA_LPF_CFG, MASK_2REG(DAIF_TIA_MOD_SEL_ME,(i==0) ? 1 : 0, DAIF_TIA_MOD_SEL_MO,1));

        // Do RX
        REGW(&XH_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,1, DAIF_RX_EN_MO,0, DAIF_TX_EN_MO,0));
        co_delay_10us(1);
        REGW(&XH_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,1, DAIF_RX_EN_MO,1, DAIF_TX_EN_MO,0));
        co_delay_10us(15);

        //polling agc idle
        XH_DAIF->DBG_REG = 0x1B;
        while(XH_DAIF->DBG_REG & (0xF<<16));

        REGW(&XH_DAIF->PD_CFG0, MASK_6REG(DAIF_PD_AGCAMP_MO,0, DAIF_PD_AGCAMP_ME,1,
            DAIF_PD_PDTIF_MO,0, DAIF_PD_PDTIF_ME,1, DAIF_PKDADC_PD_MO,0, DAIF_PKDADC_PD_ME,1));
        REGW(&XH_DAIF->PD_CFG1, MASK_6REG(DAIF_PD_LDO_IF_MO,0, DAIF_PD_LDO_IF_ME,1,
            DAIF_PD_LDO_ANADIG_MO,0, DAIF_PD_LDO_ANADIG_ME,1, DAIF_PD_LDO_PKDADC_MO,0, DAIF_PD_LDO_PKDADC_ME,1));
        REGW(&XH_DAIF->AGC_CFG0, MASK_3REG(DAIF_OS_OFFSET_EN,0, DAIF_PDTIF_CALI_MO,0, DAIF_PDTIF_CALI_ME,1));

        //polling agc idle
        co_delay_10us(1);
        XH_DAIF->DBG_REG = 0x1B;
        while(XH_DAIF->DBG_REG & (0xF<<16));

        REGW(&XH_DAIF->AGC_CFG0, MASK_1REG(DAIF_OS_OFFSET_EN,1));

        //polling agc done
        co_delay_10us(1);
        XH_DAIF->DBG_REG = 0x1B;
        while(XH_DAIF->DBG_REG & (0xF<<16));
        XH_DAIF->DBG_REG = 0;

        // save calib data
        agc_pif_os[i] = XH_DAIF->AGC_CFG3 & DAIF_PIF_OS_MASK;
    }

    calib_env.rf.agc_pif_os = agc_pif_os[0];
    calib_env.rf.agc_pif_os_2m = agc_pif_os[1];
    REGW(&XH_DAIF->AGC_CFG2, MASK_2REG(DAIF_PIF_OS_FLAG,1, DAIF_PIF_OS_REG,calib_env.rf.agc_pif_os));
    REGW(&XH_DAIF->AGC_CFG3, MASK_1REG(DAIF_PIF_OS_REG_2M, calib_env.rf.agc_pif_os_2m));

    REGW(&XH_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,0, DAIF_RX_EN_MO,0, DAIF_TX_EN_MO,0));

//    REGW(&XH_DAIF->PD_CFG0, MASK_3REG(DAIF_PD_AGCAMP_ME,0, DAIF_PD_PDTIF_ME,0, DAIF_PKDADC_PD_ME,0)); // BUG: PKDADC/AGCAMP/TIF must open (XH98xxD analog fix it? They are doing...)
    REGW(&XH_DAIF->PD_CFG1, MASK_3REG(DAIF_PD_LDO_IF_ME,0, DAIF_PD_LDO_ANADIG_ME,0, DAIF_PD_LDO_PKDADC_ME,0));
    REGW(&XH_DAIF->AGC_CFG0, MASK_2REG(DAIF_OS_OFFSET_EN,0, DAIF_PDTIF_CALI_ME,0));

    REGW(&XH_DAIF->TRX_SW_CFG, MASK_1REG(DAIF_TRX_EN_ME,0));
    REGW(&XH_DAIF->FREQ_CFG0, MASK_1REG(DAIF_FREQ_REG_ME,0));
    REGW(&XH_DAIF->PD_CFG1, MASK_1REG(DAIF_RST_PDTIF_ME,0));
    REGW(&XH_DAIF->MAIN_ST_CFG1, MASK_1REG(DAIF_AGC_SYNC_TIME,0));

    REGW(&XH_DAIF->LNA_MIX_CFG, MASK_3REG(DAIF_GAIN_LNA_ME,0, DAIF_GAIN_ATTEN_ME,0, DAIF_EN_ATTEN_ME,0));
    REGW(&XH_DAIF->TIA_LPF_CFG, MASK_1REG(DAIF_GAIN_TIA_ME,0));
}

#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
/**
 * @brief  calib rf dcoc diff get
 *
 * @param[in] i  i
 * @param[in] q  q
 * @param[in] is_iout  is iout
 *
 * @return
 **/
static uint32_t calib_rf_dcoc_diff_get(uint32_t i, uint32_t q, uint32_t is_iout)
{
    uint32_t reg, n, diff;

    REGW(&XH_DAIF->DCOC_CFG, MASK_2REG(DAIF_DIN_DCOC_I_MO,i, DAIF_DIN_DCOC_Q_MO,q));
    REGW0(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);
    co_delay_10us(1);

    reg = 0;
    for (n=0; n<20; ++n)
    {
        if (is_iout)
            reg += REGR(&XH_DAIF->RX_ADC_OUT, MASK_POS(DAIF_RXADC_IOUT));
        else
            reg += REGR(&XH_DAIF->RX_ADC_OUT, MASK_POS(DAIF_RXADC_QOUT));
    }
    reg /= 20;
    diff = reg>256 ? reg-256 : 256-reg;

    calib_log("%04X ", reg);

    REGW1(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);

    return diff;
}

/**
 * @brief calib_rf_dcoc()
 *
 * @return
 **/
static void calib_rf_dcoc(void)
{
    int filt_gidx, rx_gidx, sel_dcoc_fs, lut_idx, iq, rxout;

    // Not fsm ctrl
    XH_DAIF->TRX_SW_CFG = 2;

    //power on analog blocks
    // enable trx dbg and rx_en
    REGW1(&XH_DAIF->VCO_CTRL0, DAIF_RX_EN_MO_MASK | DAIF_TRX_DBG_MASK);
    co_delay_10us(12); // wait 80us to make sure pll stable

    // reset adc > 1
    REGW1(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK | DAIF_RST_ME_MASK);

    REGW(&XH_DAIF->DCOC_CFG, MASK_2REG(DAIF_DIN_DCOC_I_ME,1, DAIF_DIN_DCOC_Q_ME,1));
//    REGW(&XH_DAIF->DCOC_CFG1, MASK_2REG(DAIF_OFFSET_I_ME,1, DAIF_OFFSET_Q_ME, 1));

    // ana dcoc I/Q
    sel_dcoc_fs = 0;
    REGW(&XH_DAIF->DCOC_CFG, MASK_1REG(DAIF_SEL_DCOC_FS, sel_dcoc_fs));
    REGW(&XH_DAIF->TIA_LPF_CFG, MASK_3REG(DAIF_FILTER_LP_CP_SEL,1, DAIF_TIA_LP_CP_SEL,1, DAIF_TIA_SWAP,1));
    filt_gidx = 2;
    {
        REGW(&XH_DAIF->TIA_LPF_CFG, MASK_2REG(DAIF_GAIN_LPF_ME,1, DAIF_GAIN_LPF_MO,filt_gidx));

        for(rx_gidx = 0; rx_gidx <= 8; rx_gidx++)
        {
            lut_idx = filt_gidx * 9 + rx_gidx;

#if 0
            // enable en_dcoc 0
            REGW(&XH_DAIF->DCOC_CFG, MASK_4REG(
                        DAIF_DCOC_CALI, 1u,
                        DAIF_DCOC_FILT_GIDX, filt_gidx,
                        DAIF_DCOC_RX_GIDX, rx_gidx,
                        DAIF_EN_DCOC, 0));

            REGW0(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);

            co_delay_10us(3);

            // reset adc > 0
            uint32_t rxadc_qout_reg = REGR(&XH_DAIF->RX_ADC_OUT, MASK_POS(DAIF_RXADC_QOUT));
            uint32_t rxadc_iout_reg = REGR(&XH_DAIF->RX_ADC_OUT, MASK_POS(DAIF_RXADC_IOUT));

            uint32_t adcoc_i = adcoc(rxadc_iout_reg, sel_dcoc_fs);
            uint32_t adcoc_q = adcoc(rxadc_qout_reg, sel_dcoc_fs);

            REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_2REG(
                        DAIF_ANA_DCOC_I, adcoc_i,
                        DAIF_ANA_DCOC_Q, adcoc_q));

            REGW1(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);
#else
            uint16_t idiff1=0xFFFF, qdiff1=0xFFFF, idiff2=0xFFFF, qdiff2=0xFFFF, diff;
            uint16_t iindex1=0, qindex1=0, iindex2=0, qindex2=0;
            uint32_t i;

            // enable en_dcoc 0
            REGW(&XH_DAIF->DCOC_CFG, MASK_6REG(
                        DAIF_DCOC_CALI, 1u,
                        DAIF_DCOC_FILT_GIDX, filt_gidx,
                        DAIF_DCOC_RX_GIDX, rx_gidx,
                        DAIF_EN_DCOC, 1,
                        DAIF_DIN_DCOC_I_ME, 1,
                        DAIF_DIN_DCOC_Q_ME, 1));

            calib_log("-----------------filt_gidx=%d rx_gidx=%d-----------------FILTER=%d TIA=%d SWAP=%d-----------------\n",
                    filt_gidx, rx_gidx,
                    REGR(&XH_DAIF->TIA_LPF_CFG, MASK_POS(DAIF_FILTER_LP_CP_SEL)),
                    REGR(&XH_DAIF->TIA_LPF_CFG, MASK_POS(DAIF_TIA_LP_CP_SEL)),
                    REGR(&XH_DAIF->TIA_LPF_CFG, MASK_POS(DAIF_TIA_SWAP)));

            for (i=0; i<0x20; ++i)
            {
                diff = calib_rf_dcoc_diff_get(i, 0x1F, 1);
                if (diff < idiff1)
                {
                    idiff1 = diff;
                    iindex1 = i;
                }
            }
            calib_log("\n");

            for (i=0; i<0x20; ++i)
            {
                diff = calib_rf_dcoc_diff_get(iindex1, i, 0);
                if (diff < qdiff1)
                {
                    qdiff1 = diff;
                    qindex1 = i;
                }
            }
            calib_log("\n");

            for (i=0; i<0x20; ++i)
            {
                diff = calib_rf_dcoc_diff_get(i, 0x1F, 0);
                if (diff < idiff2)
                {
                    idiff2 = diff;
                    iindex2 = i;
                }
            }
            calib_log("\n");

            for (i=0; i<0x20; ++i)
            {
                diff = calib_rf_dcoc_diff_get(iindex2, i, 1);
                if (diff < qdiff2)
                {
                    qdiff2 = diff;
                    qindex2 = i;
                }
            }
            calib_log("\n");

            if(MAX(idiff1, qdiff1) < MAX(idiff2, qdiff2))
                REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_2REG(DAIF_ANA_DCOC_I,iindex1, DAIF_ANA_DCOC_Q,qindex1));
            else
                REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_2REG(DAIF_ANA_DCOC_I,iindex2, DAIF_ANA_DCOC_Q,qindex2));
#endif
        }
    }

    for (filt_gidx = 0; filt_gidx <= 6; filt_gidx++)
    {
        if (filt_gidx == 2)
            continue;

        for(rx_gidx = 0; rx_gidx <= 8; rx_gidx++)
        {
            lut_idx = filt_gidx * 9 + rx_gidx;
            XH_DAIF->DCOC_LUT[lut_idx] = XH_DAIF->DCOC_LUT[2*9 + rx_gidx];
        }
    }

    // HWctrl
    REGW(&XH_DAIF->DCOC_CFG, MASK_2REG(DAIF_DIN_DCOC_I_ME,0, DAIF_DIN_DCOC_Q_ME,0));

//    // enable phy 16m
//    REGW0(&XH_PSO->BTPHY_CFG, CPM_PHY_GATE_EN_MASK);
//    XH_PSO_XTAL32M_UPD_RDY();

    // digi dcoc I/Q
    REGW(&XH_DAIF->TIA_LPF_CFG, MASK_2REG(DAIF_FILTER_LP_CP_SEL,1, DAIF_TIA_LP_CP_SEL,1));
    for (iq=0; iq<2; ++iq)
    {
        XH_PHY->IQ_IN_SWAP = iq;
        for(filt_gidx = 0; filt_gidx <= 6; filt_gidx++)
        {
            for(rx_gidx = 0; rx_gidx <= 8; rx_gidx++)
            {
                lut_idx = filt_gidx * 9 + rx_gidx;

                // enable en_dcoc 1
                REGW(&XH_DAIF->DCOC_CFG, MASK_4REG(
                            DAIF_DCOC_CALI, 1u,
                            DAIF_DCOC_FILT_GIDX, filt_gidx,
                            DAIF_DCOC_RX_GIDX, rx_gidx,
                            DAIF_EN_DCOC, 1));

                REGW1(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);

                // 10us;
                co_delay_10us(1);

                // reset adc > 0
                REGW0(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);

                // 30us;
                co_delay_10us(3);

                rxout= ((XH_PHY->RX_OUT >> 3) & 0x7F) | ((XH_PHY->RX_OUT >> 4) & 0x80); // {phy_rdata[11], phy_rdata[9:3]}
                if(iq)
                    REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_1REG(DAIF_RXQ_OFFSET, rxout));
                else
                    REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_1REG(DAIF_RXI_OFFSET, rxout));
            }
        }
    }

    // HWctrl
    REGW0(&XH_DAIF->VCO_CTRL0, DAIF_RX_EN_MO_MASK | DAIF_TRX_DBG_MASK);
    // HWctrl
//    REGW(&XH_DAIF->DCOC_CFG1, MASK_2REG(DAIF_OFFSET_I_ME,0, DAIF_OFFSET_Q_ME,0));
    // HWctrl
    REGW0(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK | DAIF_RST_ME_MASK);

//    // Disable phy 16m
//    REGW1(&XH_PSO->BTPHY_CFG, CPM_PHY_GATE_EN_MASK);
//    XH_PSO_XTAL32M_UPD_RDY();

    // fsm
    XH_DAIF->TRX_SW_CFG = 0;

    // Disable dcdc cali
    REGW(&XH_DAIF->DCOC_CFG, MASK_1REG(DAIF_DCOC_CALI, 0));
}
#else

/**
 * @brief  calib rf dcoc get
 *
 * @param[in] i  i
 * @param[in] q  q
 * @param[in] is_iout  is iout
 *
 * @return
 **/
static uint32_t calib_rf_dcoc_get(uint32_t i, uint32_t q, uint32_t is_iout)
{
    uint32_t reg, n;

    REGW(&XH_DAIF->DCOC_CFG, MASK_2REG(DAIF_DIN_DCOC_I_MO,i, DAIF_DIN_DCOC_Q_MO,q));
    REGW0(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);
    co_delay_10us(1);

    reg = 0;
    for (n=0; n<20; ++n)
    {
        if (is_iout)
            reg += REGR(&XH_DAIF->RX_ADC_OUT, MASK_POS(DAIF_RXADC_IOUT));
        else
            reg += REGR(&XH_DAIF->RX_ADC_OUT, MASK_POS(DAIF_RXADC_QOUT));
    }
    reg /= 20;

//    diff = reg>256 ? reg-256 : 256-reg;

    REGW1(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);

    return reg;
}

static uint32_t calib_rf_tia_dcoc_get(uint32_t i, uint32_t q, uint32_t is_iout)
{
    uint32_t reg, n;

    REGW(&XH_DAIF->TIA_DCOC_CFG, MASK_2REG(DAIF_TIA_DIN_DCOC_I_MO,i, DAIF_TIA_DIN_DCOC_Q_MO,q));
    REGW0(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);
    co_delay_10us(1);

    reg = 0;
    for (n=0; n<20; ++n)
    {
        if (is_iout)
            reg += REGR(&XH_DAIF->RX_ADC_OUT, MASK_POS(DAIF_RXADC_IOUT));
        else
            reg += REGR(&XH_DAIF->RX_ADC_OUT, MASK_POS(DAIF_RXADC_QOUT));
    }
    reg /= 20;

//    diff = reg>256 ? reg-256 : 256-reg;

    REGW1(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);

    return reg;
}

/**
 * @brief  calib rf dcoc
 **/
static void calib_rf_dcoc(void)
{
    uint8_t filt_gidx, rx_gidx, lut_idx, iq, idx, din_val ,din_dcoc[2];
    uint32_t sig,rxout;
    // Not fsm ctrl
    XH_DAIF->TRX_SW_CFG = 2;

    //power on analog blocks
    // enable trx dbg and rx_en
    REGW1(&XH_DAIF->VCO_CTRL0, DAIF_RX_EN_MO_MASK | DAIF_TRX_DBG_MASK);
    co_delay_10us(12); // wait 80us to make sure pll stable

    // reset adc > 1
    REGW1(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK | DAIF_RST_ME_MASK);

    //    REGW(&XH_DAIF->DCOC_CFG1, MASK_2REG(DAIF_OFFSET_I_ME,1, DAIF_OFFSET_Q_ME, 1));    //digi dcoc reg contrl

    //*********** ana dcoc I/Q  == TIA_PART + Filter_PART ***************//

    //***TIA PART***//
    //config TIA /Filter mode ,  dcoc mode
    REGW(&XH_DAIF->TIA_LPF_CFG, MASK_4REG(DAIF_FILTER_LP_CP_SEL,0, DAIF_TIA_LP_CP_SEL,0, DAIF_TIA_SWAP,1, DAIF_GAIN_LPF_ME,0));
    REGW(&XH_DAIF->DCOC_CFG, MASK_2REG(DAIF_EN_DCOC,0, DAIF_SEL_DCOC_FS, 0));   //shutdowm filter dcoc
    REGW(&XH_DAIF->TIA_DCOC_CFG, MASK_4REG(DAIF_TIA_EN_DCOC,1, DAIF_TIA_SEL_DCOC_FS, 0, DAIF_TIA_DIN_DCOC_I_ME,1, DAIF_TIA_DIN_DCOC_Q_ME,1));  //ctrl tia dcoc from reg

    filt_gidx=2;
    for(rx_gidx = 0; rx_gidx <= 8; rx_gidx++)
    {
        REGW(&XH_DAIF->DCOC_CFG, MASK_3REG(DAIF_DCOC_CALI,1,
                    DAIF_DCOC_FILT_GIDX, filt_gidx ,DAIF_DCOC_RX_GIDX, rx_gidx));//doco_cali=1,gdix ctrl gain
        REGW(&XH_DAIF->TIA_DCOC_CFG, MASK_1REG(DAIF_TIA_DCOC_CALI,1));//tia_doco_cali=1,gdix ctrl gain
        co_delay_10us(1);     // wait 10us to make sure dc voltage stable
        for(iq=0;iq<2;iq++)
        {
            din_val=0b10000;
            for(idx=5;idx>0;idx--)
            {
                din_val |= (1<<(idx-1));
                sig=calib_rf_tia_dcoc_get(din_val, din_val, iq);
                sig=sig>256;
                if(sig)
                    din_val |= (1<<(idx-1));
                else
                    din_val &=~(1<<(idx-1));
            }
            din_dcoc[iq]=din_val;
        }
        REGW(&XH_DAIF->TIA_DCOC_LUT[rx_gidx], MASK_2REG(DAIF_ANA_TIA_DCOC_I,din_dcoc[1], DAIF_ANA_TIA_DCOC_Q,din_dcoc[0]));
    }

    //***Filter PART  (optional)***//
#if 1   //Filter dcoc
    //config TIA /Filter mode ,  dcoc mode
    REGW(&XH_DAIF->TIA_LPF_CFG, MASK_3REG(DAIF_FILTER_LP_CP_SEL,0, DAIF_TIA_LP_CP_SEL,1, DAIF_TIA_SWAP,1));
    REGW(&XH_DAIF->DCOC_CFG, MASK_4REG(DAIF_EN_DCOC,1, DAIF_SEL_DCOC_FS, 1, DAIF_DIN_DCOC_I_ME,1, DAIF_DIN_DCOC_Q_ME,1));          //ctrl filter dcoc from reg
    REGW(&XH_DAIF->TIA_DCOC_CFG, MASK_4REG( DAIF_TIA_EN_DCOC,1, DAIF_TIA_SEL_DCOC_FS, 0, DAIF_TIA_DIN_DCOC_I_ME,0, DAIF_TIA_DIN_DCOC_Q_ME,0)); //start tia dcoc from fsm

    for (filt_gidx = 0; filt_gidx <= 6; filt_gidx++)
    {
        for(rx_gidx = 0; rx_gidx <= 8; rx_gidx++)
        {
            lut_idx = filt_gidx * 9 + rx_gidx;
            REGW(&XH_DAIF->DCOC_CFG, MASK_3REG(DAIF_DCOC_CALI,1,
                        DAIF_DCOC_FILT_GIDX, filt_gidx ,DAIF_DCOC_RX_GIDX, rx_gidx));//doco_cali=1,gdix ctrl gain
            co_delay_10us(1);   // wait 10us to make sure dc voltage stable
            for(iq=0;iq<2;iq++)
            {
                din_val=0b10000;
                for(idx=5;idx>0;idx--)
                {
                    din_val |= (1<<(idx-1));
                    sig=calib_rf_dcoc_get(din_val, din_val, iq);
                    sig=sig>256;
                    if(sig)
                        din_val |= (1<<(idx-1));
                    else
                        din_val &=~(1<<(idx-1));
                }
                din_dcoc[iq]=din_val;
            }
            REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_2REG(DAIF_ANA_DCOC_I,din_dcoc[1], DAIF_ANA_DCOC_Q,din_dcoc[0]));
        }
    }

#else   //no Filter dcoc
    for (filt_gidx = 0; filt_gidx <= 6; filt_gidx++)
    {
        for(rx_gidx = 0; rx_gidx <= 8; rx_gidx++)
        {
            lut_idx = filt_gidx * 9 + rx_gidx;
            REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_2REG(DAIF_ANA_DCOC_I,0b10000, DAIF_ANA_DCOC_Q,0b10000));
            co_delay_us(1);  //wait > 5 cpu clk after gain change
        }
    }
#endif  //Filter dcoc

    // HWctrl ana dcoc
    REGW(&XH_DAIF->TIA_LPF_CFG, MASK_2REG(DAIF_FILTER_LP_CP_SEL, 1, DAIF_TIA_LP_CP_SEL, 1));
    //    REGW(&XH_DAIF->TIA_DCOC_CFG, MASK_2REG(DAIF_TIA_DIN_DCOC_I_ME,0, DAIF_TIA_DIN_DCOC_Q_ME,0));  //set before
    REGW(&XH_DAIF->DCOC_CFG, MASK_2REG(DAIF_DIN_DCOC_I_ME,0,  DAIF_DIN_DCOC_Q_ME, 0));



    //    // enable phy 16m
    //    REGW0(&XH_PSO->BTPHY_CFG, CPM_PHY_GATE_EN_MASK);
    //    XH_PSO_XTAL32M_UPD_RDY();
    //*********** dig dcoc I/Q ***************//

    for (iq=0; iq<2; ++iq)
    {
        XH_PHY->IQ_IN_SWAP = iq;
        for(filt_gidx = 0; filt_gidx <= 6; filt_gidx++)
        {
            for(rx_gidx = 0; rx_gidx <= 8; rx_gidx++)
            {
                lut_idx = filt_gidx * 9 + rx_gidx;
                // enable en_dcoc 1
                REGW(&XH_DAIF->DCOC_CFG, MASK_4REG(            //doco_cali=1,gdix ctrl gain
                            DAIF_DCOC_CALI, 1u,
                            DAIF_DCOC_FILT_GIDX, filt_gidx,
                            DAIF_DCOC_RX_GIDX, rx_gidx,
                            DAIF_EN_DCOC, 1));

                REGW1(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);

                // 10us;
                co_delay_10us(1);

                // reset adc > 0
                REGW0(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK);

                // 30us;
                co_delay_10us(3);

                rxout= ((XH_PHY->RX_OUT >> 3) & 0x7F) | ((XH_PHY->RX_OUT >> 4) & 0x80); // {phy_rdata[11], phy_rdata[9:3]}
            if(iq)
                REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_1REG(DAIF_RXQ_OFFSET, rxout));
            else
                REGW(&XH_DAIF->DCOC_LUT[lut_idx], MASK_1REG(DAIF_RXI_OFFSET, rxout));
            }
        }
    }

    // HWctrl vco
    REGW0(&XH_DAIF->VCO_CTRL0, DAIF_RX_EN_MO_MASK | DAIF_TRX_DBG_MASK);
    // HWctrl
    //    REGW(&XH_DAIF->DCOC_CFG1, MASK_2REG(DAIF_OFFSET_I_ME,0, DAIF_OFFSET_Q_ME,0));
    // HWctrl rxadc
    REGW0(&XH_DAIF->RXADC_CTRL, DAIF_RST_MO_MASK | DAIF_RST_ME_MASK);

    //    // Disable phy 16m
    //    REGW1(&XH_PSO->BTPHY_CFG, CPM_PHY_GATE_EN_MASK);
    //    XH_PSO_XTAL32M_UPD_RDY();

    // fsm trx
    XH_DAIF->TRX_SW_CFG = 0;

    co_delay_us(1);  //wait > 5 cpu clk after gain change
    // Disable dcoc cali
    REGW(&XH_DAIF->DCOC_CFG, MASK_1REG(DAIF_DCOC_CALI, 0));
    // Disable tia dcoc cali
    REGW(&XH_DAIF->TIA_DCOC_CFG, MASK_1REG(DAIF_TIA_DCOC_CALI, 0));
}
#endif

/**
 * @brief  calib rf txfe pacns reg set
 *
 * @param[in] index  index
 * @param[in] padr2s_pct_val  padr2s pct val
 * @param[in] padr2s_nct_val  padr2s nct val
 **/
static void calib_rf_txfe_pacns_reg_set(uint32_t index, uint32_t padr2s_pct_val, uint32_t padr2s_nct_val)
{
    uint32_t padr1s_ct_val = txfe_pa_table[index].padr1s_ct_mo;
    uint32_t pacore_ct_val = txfe_pa_table[index].pacore_ct_mo;
    uint32_t tx_atten_val  = txfe_pa_table[index].tx_atten_mo;

    REGW(&XH_DAIF->PA_CNS, MASK_6REG(DAIF_PA_DBG,1, DAIF_PADR1S_CT_MO,padr1s_ct_val, DAIF_PADR2S_PCT_MO,padr2s_pct_val,
                DAIF_PADR2S_NCT_MO,padr2s_nct_val, DAIF_PACORE_CT_MO,pacore_ct_val, DAIF_TX_ATTEN_MO,tx_atten_val));
    co_delay_us(2);
}

/**
 * @brief  calib txfe
 **/
static void calib_rf_txfe(void)
{
    int i;
    uint32_t padr2s_pct_val, padr2s_nct_val;

    // Power on
    REGW(&XH_DAIF->PD_CFG0, MASK_6REG(DAIF_PD_TXCALI_MO,0, DAIF_PD_TXCALI_ME,1, DAIF_PD_LDO_PA_MO,0, DAIF_PD_LDO_PA_ME,1u, DAIF_PD_PA_MO,0, DAIF_PD_PA_ME,1));
    REGW(&XH_DAIF->PD_CFG1, MASK_6REG(DAIF_PD_LDO_IF_MO,0, DAIF_PD_LDO_IF_ME,1, DAIF_PD_PLL_ALL_MO,0, DAIF_PD_PLL_ALL_ME,1, DAIF_PD_LOTX_MO,0, DAIF_PD_LOTX_ME,1));
//    REGW(&XH_DAIF->PD_CFG2, MASK_2REG(DAIF_PD_PLL_BUF_MO,0, DAIF_PD_PLL_BUF_ME,1));

    co_delay_us(10);

    //tx fe loop
    for(i=17; i>=0; i--)
    {
        // tx fe calib for gain

        padr2s_pct_val = 8;
        padr2s_nct_val = 8;

        calib_rf_txfe_pacns_reg_set(i, padr2s_pct_val, padr2s_nct_val);

        if(XH_DAIF->PA_CNS & DAIF_TXCALI_OUT_SYNC_MASK)
        {
            while(1)
            {
                if(--padr2s_nct_val == 0)
                    break;
                calib_rf_txfe_pacns_reg_set(i, padr2s_pct_val, padr2s_nct_val);
                if(!(XH_DAIF->PA_CNS & DAIF_TXCALI_OUT_SYNC_MASK))
                    break;

                if(++padr2s_pct_val == 16)
                    break;
                calib_rf_txfe_pacns_reg_set(i, padr2s_pct_val, padr2s_nct_val);
                if(!(XH_DAIF->PA_CNS & DAIF_TXCALI_OUT_SYNC_MASK))
                    break;
            }
        }
        else
        {
            while(1)
            {
                if(--padr2s_pct_val == 0)
                    break;
                calib_rf_txfe_pacns_reg_set(i, padr2s_pct_val, padr2s_nct_val);
                if(XH_DAIF->PA_CNS & DAIF_TXCALI_OUT_SYNC_MASK)
                    break;

                if(++padr2s_nct_val == 16)
                    break;
                calib_rf_txfe_pacns_reg_set(i, padr2s_pct_val, padr2s_nct_val);
                if(XH_DAIF->PA_CNS & DAIF_TXCALI_OUT_SYNC_MASK)
                    break;
            }
        }

//        log_info("p=%d n=%d\n", padr2s_pct_val, padr2s_nct_val);

        if(padr2s_pct_val == 0)  padr2s_pct_val = 1;
        if(padr2s_nct_val == 0)  padr2s_nct_val = 1;
        if(padr2s_pct_val == 16) padr2s_pct_val = 15;
        if(padr2s_nct_val == 16) padr2s_nct_val = 15;

        // XH98xxD move to PMU
        REGWA(&XH_PMU->PA_GAIN_REG[17-i], MASK_5REG(
                DAIF_PA_GAIN_PADR2S_PCT_MO, padr2s_pct_val,
                DAIF_PA_GAIN_PADR2S_NCT_MO, padr2s_nct_val,
                DAIF_PA_GAIN_PADR1S_CT_MO, txfe_pa_table[i].padr1s_ct_mo,
                DAIF_PA_GAIN_PACORE_CT_MO, txfe_pa_table[i].pacore_ct_mo,
                DAIF_PA_GAIN_TX_ATTEN_MO, txfe_pa_table[i].tx_atten_mo));

        //some delay between each gain
        co_delay_us(5);
    }

    // HWctrl
    REGW(&XH_DAIF->PD_CFG0, MASK_3REG(DAIF_PD_TXCALI_ME,0, DAIF_PD_LDO_PA_ME,0, DAIF_PD_PA_ME,0));
    REGW(&XH_DAIF->PD_CFG1, MASK_3REG(DAIF_PD_LDO_IF_ME,0, DAIF_PD_PLL_ALL_ME,0, DAIF_PD_LOTX_ME,0));
//    REGW(&XH_DAIF->PD_CFG2, MASK_1REG(DAIF_PD_PLL_BUF_ME,0));

    //clear pa_dbg
    REGW(&XH_DAIF->PA_CNS, MASK_1REG(DAIF_PA_DBG, 0));
}

/**
 * @brief  calib sys rc value set
 *
 * @param[in] lpf  lpf
 * @param[in] tia  tia
 **/
static void calib_sys_rc_value_set(uint32_t lpf, uint32_t tia, uint32_t auadc)
{
    REGW(&XH_DAIF->TIA_LPF_CFG, MASK_2REG(DAIF_FILTER_CTUNE,lpf, DAIF_TIA_CTUNE,tia));
    REGW(&XH_PMU->CLK_CTRL_1, MASK_1REG(PMU_REG_CTUNE_FDOUB, lpf));
    calib_env.sys.rc_tune_auadc = auadc;
}

/**
 * @brief calib_rf_store()
 *
 * @return
 **/
static void calib_rf_store(void)
{
    // vco afc
//    for(i=0; i<80; i++)
//    {
//        REGWA(&XH_DAIF->PLL_LUT_DBG, MASK_2REG(DAIF_PLL_LUT_LU,1, DAIF_PLL_LUT_IDX,i));
//        while(XH_DAIF->PLL_LUT_DBG & DAIF_PLL_LUT_LU_MASK);
//        calib_env.rf.vco_afc_lut[i] = XH_DAIF->PLL_LUT_DBG & DAIF_PLL_LUT_DATA_MASK;
//    }

    // vco amp
//    XH_DAIF->DBG_REG = 0x18;
//    calib_env.rf.vco_amp = (XH_DAIF->DBG_REG >> 26) & 0x7;
//    XH_DAIF->DBG_REG = 0;

    // kdco 1M
    calib_env.rf.kdco_1m[0] = XH_DAIF->KDCO_LUT_1M_REG0;
    calib_env.rf.kdco_1m[1] = XH_DAIF->KDCO_LUT_1M_REG1;

    // kdco 2M
    calib_env.rf.kdco_2m[0] = XH_DAIF->KDCO_LUT_2M_REG0;
    calib_env.rf.kdco_2m[1] = XH_DAIF->KDCO_LUT_2M_REG1;

    // AGC save (saved in calib_rf_agc)
//    calib_env.rf.agc_pif_os = XH_DAIF->AGC_CFG3 & DAIF_PIF_OS_MASK;

    // DCDC save
//    for(i=0; i<63; ++i)
//        calib_env.rf.dcoc_lut[i] = XH_DAIF->DCOC_LUT[i];

    // TIA DCOC save
#if !defined(CONFIG_XH98xxD) && !defined(CONFIG_XH98xxF)
    for(int i=0; i<9; ++i)
        calib_env.rf.tia_dcoc_lut[i] = (uint16_t)XH_DAIF->TIA_DCOC_LUT[i];
#endif

    // TXFE
//    for(i=0; i<18; ++i)
//        calib_env.rf.txfe_pa_gain[i] = XH_DAIF->PA_GAIN_REG[i];

    // afc en = 0
    REGW(&XH_DAIF->VCO_CTRL0, MASK_2REG(DAIF_AFC_EN_MO,0, DAIF_AFC_EN_ME,1));

    // PA ctrl by RW
    // SEL=0: REG ctrl PA
    // SEL=1: RW ctrl PA
    REGW1(&XH_DAIF->PA_CNS, DAIF_PA_GAIN_IDX_SEL_MASK);
    // REGW(&XH_DAIF->PA_CNS, MASK_2REG(DAIF_PA_GAIN_IDX_SEL,0, DAIF_PA_GAIN_IDX_REG,18));
}

#if 0
/**
 * @brief calib_sys_store()
 *
 * store: xtal24m_ldo, rc, rc24m, (rc32k)
 *
 * @return None
 **/
static void calib_sys_store(void)
{
    // rc
    calib_env.sys.rc_tia_lpf = XH_DAIF->TIA_LPF_CFG;

    // rc32m
    REGW(&XH_PMU->CLK_CTRL_1, MASK_2REG(PMU_CTUNE_OSC_SEL, 1,
                PMU_REG_CTUNE_OSC, REGR(&XH_DAIF->RC32M_TUN,MASK_POS(DAIF_RC_32M_TUNE))));
}
#endif

#if 0
/**
 * @brief  calib rc32k accuracy check
 *
 * @return accuracy
 **/
static uint32_t calib_rc32k_count_by_xtal32m(uint32_t win_32k_num)
{
    //check rc32k clock again with intr enabled
    REGWA(&XH_DAIF->CLK_CHK_CNS, MASK_5REG(
                DAIF_START,1, DAIF_INTR_EN,1, DAIF_INTR_ST,0, DAIF_REF_SEL,0,
                DAIF_WIN_CNT_THRSH, win_32k_num)); // win_cnt: 32k period number
    // wait rc 32k check done -- 2
    while(XH_DAIF->CLK_CHK_CNS & DAIF_START_MASK);

    // got rc 32k check status
    return XH_DAIF->CLK_CHK_STATUS; // in win_cnt, how many 32MHz number
}
#endif

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

#if 0
/**
 * @brief  calib rc32k accuracy check
 *
 * @return ppm
 **/
int calib_rc32k_accuracy_check(uint32_t win_32k_num)
{
    // >>> 16*(1.0/32768.0)/(1.0/32000000)
    // 15625.0 (62ppm)
    // >>> 32*(1.0/32768.0)/(1.0/32000000)
    // 31250.0 (32ppm)
    // >>> 64*(1.0/32768.0)/(1.0/32000000)
    // 62500.0 (15ppm)

    int clk_num_32m;
    int clk_num_32m_std = win_32k_num * 32000000LL / 32768;

    CPM_ANA_CLK_ENABLE();

    REGW1(&XH_DAIF->CLK_ENS, DAIF_CC_CLK_EN_MASK);

    clk_num_32m = calib_rc32k_count_by_xtal32m(win_32k_num);

    REGW0(&XH_DAIF->CLK_ENS, DAIF_CC_CLK_EN_MASK);

    CPM_ANA_CLK_RESTORE();

    return 1000000 * (clk_num_32m - clk_num_32m_std) / clk_num_32m_std;
}
#endif

/**
 * @brief  calib rc32k begin
 **/
CO_USED_IN_ROM
void calib_rc32k_start(void)
{
    // Eanble depend clock
    pmu_ana_enable(true, PMU_ANA_CALIB_RC32K);

    // PMU reg bypass disable
    REGW(&XH_PMU->CLK_CTRL_1, MASK_1REG(PMU_RC_32K_RCTUNE_SEL, 0));
    while((XH_PMU->STATUS_READ & PMU_SEL_RC32K_TUNE_MASK) != 0);

    // un-reset
    REGW1(&XH_DAIF->RC32K_TUN, DAIF_RC_32K_TUNE_SWRSTN_MASK);

    //rc 32k tune
    // do rc 32k tune 1
    REGW1(&XH_DAIF->RC32K_TUN, DAIF_RC_32K_TUNE_START_MASK);
}

/**
 * @brief  calib rc32k end
 **/
CO_USED_IN_ROM
void calib_rc32k_wait_finish(void)
{
    // wait rc32k tune 1 done
    while(XH_DAIF->RC32K_TUN & DAIF_RC_32K_TUNE_START_MASK);

    // Save
    REGW(&XH_PMU->CLK_CTRL_2, MASK_1REG(PMU_RTUNE_RC32K_REG, REGR(&XH_DAIF->RC32K_TUN_OUT, MASK_POS(DAIF_RTUNE))));
    REGW(&XH_PMU->CLK_CTRL_2, MASK_1REG(PMU_CTUNE_RC32K_REG, REGR(&XH_DAIF->RC32K_TUN_OUT, MASK_POS(DAIF_CTUNE))));
    REGW(&XH_PMU->CLK_CTRL_1, MASK_1REG(PMU_RCTUNE_RC32K_UPDATE_REG, 1));
    while(XH_PMU->STATUS_READ & PMU_RCTUNE_RC32K_UPDATE_STATUS_MASK);

    // use REG to switch it
    REGW(&XH_PMU->CLK_CTRL_1, MASK_1REG(PMU_RC_32K_RCTUNE_SEL, 1));
    while((XH_PMU->STATUS_READ & PMU_SEL_RC32K_TUNE_MASK) == 0);

    // reset
    REGW0(&XH_DAIF->RC32K_TUN, DAIF_RC_32K_TUNE_SWRSTN_MASK);

    // Must delay 1 32k to switch it
    co_delay_10us(6);

    // Disable depend clock
    pmu_ana_enable(false, PMU_ANA_CALIB_RC32K);
}

/**
 * @brief  calib rc32k to 62ppm
 **/
CO_USED_IN_ROM
void calib_rc32k(void)
{
#ifndef CONFIG_DCDC_ABSENT
    // calib rc32k must disable DCDC
    bool dcdc_is_enabled = pmu_dcdc_is_enabled();
    if (dcdc_is_enabled) pmu_dcdc_enable(false);
#endif

    // do calib rc32k
    calib_rc32k_start();
    calib_rc32k_wait_finish();

#ifndef CONFIG_DCDC_ABSENT
    // enable
    if (dcdc_is_enabled) pmu_dcdc_enable(true);
#endif
}

/**
 * @brief calib sys rc32m
 **/
CO_USED_IN_ROM
void calib_sys_rc32m(void)
{
    // clock
    REGW1(&XH_DAIF->CLK_ENS, DAIF_RC_32M_TUNE_CLK_EN_MASK | DAIF_RC_CALIB_CLK_EN_MASK);

    // FSM
    REGW(&XH_PMU->CLK_CTRL_1, MASK_1REG(PMU_CTUNE_OSC_SEL, 0));

    //rc 32m tune
    // do rc 32m tune 1
    REGW1(&XH_DAIF->RC32M_TUN, DAIF_RC_32M_TUNE_START_MASK);
    // wait rc32m tune 1 done
    while(XH_DAIF->RC32M_TUN & DAIF_RC_32M_TUNE_START_MASK);

    // save
#ifndef CONFIG_OTP_ABSENT
    REGW(&XH_PMU->CLK_CTRL_1, MASK_3REG(PMU_CLK_CALI_DONE,1, // flag for OTP: clock calibed done
            PMU_CTUNE_OSC_SEL,1, PMU_REG_CTUNE_OSC,REGR(&XH_DAIF->RC32M_TUN,MASK_POS(DAIF_RC_32M_TUNE))));
#else
    REGW(&XH_PMU->CLK_CTRL_1, MASK_2REG(PMU_CTUNE_OSC_SEL,1, PMU_REG_CTUNE_OSC,REGR(&XH_DAIF->RC32M_TUN,MASK_POS(DAIF_RC_32M_TUNE))));
#endif
}

/**
 * @brief calib sys rc
 **/
CO_USED_IN_ROM
void calib_sys_rc(void)
{
    // clock
    REGW1(&XH_DAIF->CLK_ENS, DAIF_RC_32M_TUNE_CLK_EN_MASK | DAIF_RC_CALIB_CLK_EN_MASK);

    // SWctrl
    REGW(&XH_DAIF->PD_CFG1, MASK_2REG(DAIF_PD_LDO_IF_MO,0, DAIF_PD_LDO_IF_ME,1));

    //rc calib
    REGW1(&XH_DAIF->RX_RCCAL_CTRL, DAIF_START_MASK);
    // wait rc calib done
    while(XH_DAIF->RX_RCCAL_CTRL & DAIF_START_MASK);

    // rc calib result
    uint32_t rc_sum = REGR(&XH_DAIF->RX_RCCAL_STAT, MASK_POS(DAIF_CNT_T1)) + REGR(&XH_DAIF->RX_RCCAL_STAT, MASK_POS(DAIF_CNT_T2));
    rc_sum /= 2;

#ifdef CONFIG_XH98xx
    RC_IF_GREATER_DO(580-8, 0,  0,  0 )
    RC_EI_GREATER_DO(562-8, 1,  1,  1 )
    RC_EI_GREATER_DO(544-8, 2,  2,  2 )
    RC_EI_GREATER_DO(526-8, 3,  3,  3 )
    RC_EI_GREATER_DO(508-8, 4,  4,  4 )
    RC_EI_GREATER_DO(490-8, 5,  5,  5 )
    RC_EI_GREATER_DO(472-8, 6,  6,  6 )
    RC_EI_GREATER_DO(454-8, 7,  7,  7 )
    RC_EI_GREATER_DO(436-8, 8,  8,  8 )
    RC_EI_GREATER_DO(418-8, 9,  9,  9 )
    RC_EI_GREATER_DO(400-8, 10, 10, 10)
    RC_EI_GREATER_DO(382-8, 11, 11, 11)
    RC_EI_GREATER_DO(364-8, 12, 12, 12)
    RC_EI_GREATER_DO(346-8, 13, 13, 13)
    RC_EI_GREATER_DO(328-8, 14, 14, 14)
    RC_EL_GREATER_DO(       15, 15, 15)
#else
    RC_IF_GREATER_DO(538-8, 0,  0,  0 )
    RC_EI_GREATER_DO(522-8, 1,  1,  1 )
    RC_EI_GREATER_DO(506-8, 2,  2,  2 )
    RC_EI_GREATER_DO(490-8, 3,  3,  3 )
    RC_EI_GREATER_DO(474-8, 4,  4,  4 )
    RC_EI_GREATER_DO(458-8, 5,  5,  5 )
    RC_EI_GREATER_DO(442-8, 6,  6,  6 )
    RC_EI_GREATER_DO(426-8, 7,  7,  7 )
    RC_EI_GREATER_DO(410-8, 8,  8,  8 )
    RC_EI_GREATER_DO(394-8, 9,  9,  9 )
    RC_EI_GREATER_DO(378-8, 10, 10, 10)
    RC_EI_GREATER_DO(362-8, 11, 11, 11)
    RC_EI_GREATER_DO(346-8, 12, 12, 12)
    RC_EI_GREATER_DO(330-8, 13, 13, 13)
    RC_EI_GREATER_DO(314-8, 14, 14, 14)
    RC_EL_GREATER_DO(       15, 15, 15)
#endif

    // HWctrl
    REGW(&XH_DAIF->PD_CFG1, MASK_2REG(DAIF_PD_LDO_IF_MO,0, DAIF_PD_LDO_IF_ME,0));

    // save
    calib_env.sys.rc_tia_lpf = XH_DAIF->TIA_LPF_CFG;
}

/**
 * @brief calib_rf()
 *
 * @return
 **/
CO_USED_IN_ROM
void calib_rf(void)
{
    if(IS_FPGA())
        return;

    pmu_ana_enable(true, PMU_ANA_RF);

    // enable clock
    CPM_ANA_CLK_ENABLE_NOIRQ();
    uint32_t clk_ens_save = XH_DAIF->CLK_ENS;
    REGW1(&XH_DAIF->CLK_ENS, DAIF_PLL_AFC_CLK_EN_MASK | DAIF_PLL_AMP_CLK_EN_MASK | DAIF_RX_AGC_CLK_EN_MASK |
                DAIF_PLL_VTRACK_CLK_EN_MASK | DAIF_PLL_LUT_CLK_EN_MASK |
                DAIF_PLL_CLK_AFC_EN_MASK | DAIF_SDM_CLK_EN_MASK | DAIF_PLL_CLK_REF_EN_MASK);

    // patch
    calib_patch(25);

    // do calib
    calib_rf_afc();
    calib_rf_dcoc();
    calib_rf_agc();
    calib_rf_txfe();

    // store calib value
    calib_rf_store();

    // patch
    calib_patch(25);

    // restore clock
    XH_DAIF->CLK_ENS = clk_ens_save;
    CPM_ANA_CLK_RESTORE_NOIRQ();

    pmu_ana_enable(false, PMU_ANA_RF);
}

/**
 * @brief calib_rf_restore()
 *
 * @return
 **/
CO_USED_IN_ROM
void calib_rf_restore(void)
{
    if(IS_FPGA())
        return;

    CPM_ANA_CLK_ENABLE();
    uint32_t clk_ens_save = XH_DAIF->CLK_ENS;
    REGW1(&XH_DAIF->CLK_ENS, DAIF_PLL_AFC_CLK_EN_MASK | DAIF_PLL_AMP_CLK_EN_MASK | DAIF_RX_AGC_CLK_EN_MASK |
                DAIF_PLL_VTRACK_CLK_EN_MASK | DAIF_PLL_LUT_CLK_EN_MASK | DAIF_DCOC_LUT_CLK_EN_MASK |
                DAIF_PLL_CLK_AFC_EN_MASK | DAIF_SDM_CLK_EN_MASK | DAIF_PLL_CLK_REF_EN_MASK);

    // vco afc
//    for(i=0; i<80; i++)
//    {
//        REGWA(&XH_DAIF->PLL_LUT_DBG, MASK_3REG(DAIF_PLL_LUT_WR,1, DAIF_PLL_LUT_IDX,i, DAIF_PLL_LUT_DATA,calib_env.rf.vco_afc_lut[i]));
//        while(XH_DAIF->PLL_LUT_DBG & DAIF_PLL_LUT_WR_MASK);
//    }
    REGW(&XH_DAIF->VCO_CTRL0, MASK_2REG(DAIF_AFC_EN_MO,0, DAIF_AFC_EN_ME,1));

    // vco amp
//    REGW(&XH_DAIF->PLL_CTRL2, MASK_2REG(DAIF_ICON_VCO_MO,calib_env.rf.vco_amp, DAIF_ICON_VCO_ME,1));

    // kdco 1M
    XH_DAIF->KDCO_LUT_1M_REG0 = calib_env.rf.kdco_1m[0];
    XH_DAIF->KDCO_LUT_1M_REG1 = calib_env.rf.kdco_1m[1];

    // kdco 2M
    XH_DAIF->KDCO_LUT_2M_REG0 = calib_env.rf.kdco_2m[0];
    XH_DAIF->KDCO_LUT_2M_REG1 = calib_env.rf.kdco_2m[1];

    // AGC restore
    REGW(&XH_DAIF->AGC_CFG2, MASK_2REG(DAIF_PIF_OS_FLAG,1, DAIF_PIF_OS_REG,calib_env.rf.agc_pif_os));
    REGW(&XH_DAIF->AGC_CFG3, MASK_1REG(DAIF_PIF_OS_REG_2M, calib_env.rf.agc_pif_os_2m));

    // DCOC restore
//    for(i=0; i<63; ++i)
//        XH_DAIF->DCOC_LUT[i] = calib_env.rf.dcoc_lut[i];
    REGW(&XH_DAIF->DCOC_CFG,  MASK_4REG(DAIF_DIN_DCOC_I_ME,0, DAIF_DIN_DCOC_Q_ME,0, DAIF_DCOC_CALI,0, DAIF_EN_DCOC,1));
//    REGW(&XH_DAIF->DCOC_CFG1, MASK_2REG(DAIF_OFFSET_I_ME,0, DAIF_OFFSET_Q_ME,0));

    // TIA DCOC restore
#if !defined(CONFIG_XH98xxD) && !defined(CONFIG_XH98xxF)
//    REGW(&XH_DAIF->TIA_DCOC_CFG, MASK_1REG(DAIF_TIA_EN_DCOC, 1));
    for(int i=0; i<9; ++i)
        XH_DAIF->TIA_DCOC_LUT[i] = calib_env.rf.tia_dcoc_lut[i];
#endif

    // TXFE
//    for(i=0; i<18; ++i)
//        XH_DAIF->PA_GAIN_REG[i] = calib_env.rf.txfe_pa_gain[i];

    // PA ctrl by RW
    REGW1(&XH_DAIF->PA_CNS, DAIF_PA_GAIN_IDX_SEL_MASK);

    // modify default value
    calib_patch(25);

    XH_DAIF->CLK_ENS = clk_ens_save;
    CPM_ANA_CLK_RESTORE();
}

/**
 * @brief  calib sys restore
 **/
CO_USED_IN_ROM
void calib_sys_restore(void)
{
    CPM_ANA_AHB_CLK_ENABLE();

    // rc
    XH_DAIF->TIA_LPF_CFG = calib_env.sys.rc_tia_lpf;

    CPM_ANA_CLK_RESTORE();
}

/** @} */

