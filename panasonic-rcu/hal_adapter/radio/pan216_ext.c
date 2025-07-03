#include "pan216_ext.h"

#if (!EASY_RF)
PAN216_Err_t PAN216_ReadVCOCode(void)
{
    my_log("VCO code: %02X %02X %02X %02X %02X\n", PAN216_ReadPageReg(PAN216_PAGE1, 0x71) & 0x0f,
           PAN216_ReadPageReg(PAN216_PAGE1, 0x72) & 0xf0,
           PAN216_ReadPageReg(PAN216_PAGE1, 0x72) & 0x0f,
           PAN216_ReadPageReg(PAN216_PAGE1, 0x73) & 0xf0,
           PAN216_ReadPageReg(PAN216_PAGE1, 0x73) & 0x0f);
    return PAN216_OK;
}

PAN216_Err_t PAN216_ReadTpCode(void)
{
    my_log("Tp code: %02X %02X %02X %02X\n", PAN216_ReadPageReg(PAN216_PAGE1, 0x23) & 0x1f,
           PAN216_ReadPageReg(PAN216_PAGE1, 0x24) & 0x1f,
           PAN216_ReadPageReg(PAN216_PAGE1, 0x25) & 0x1f,
           PAN216_ReadPageReg(PAN216_PAGE1, 0x26) & 0x1f);
    return PAN216_OK;
}

/**
 * @brief Read calibration values for BW and DCOC modes
 * @param mode Calibration mode (FILTER_OFFSET_CAL_MODE, FILTER_BW_CAL_MODE, FILTER_DCOC_CAL_MODE)
 * @param bw Bandwidth selection (0:1M, 1:2M, 2:250K) - only used for FILTER_BW_CAL_MODE
 */
void ReadBWDcocCal(uint8_t mode, uint8_t bw)
{
    if (mode == FILTER_OFFSET_CAL_MODE)
    {
        uint8_t t1, t2;
        u16 dcofst_inphase_dc_word_gain3;
        u16 dcofst_quad_dc_word_gain3;
        t1 = PAN216_ReadReg(0x74) & BIT1;
        t2 = PAN216_ReadReg(0x7B);
        dcofst_inphase_dc_word_gain3 = ((u16)t1 << 8) | (u16)t2;
        t1 = PAN216_ReadReg(0x74) & BIT0;
        t2 = PAN216_ReadReg(0x7C);
        dcofst_quad_dc_word_gain3 = ((u16)t1 << 8) | (u16)t2;
        my_log("OFFSET_CAL: dcofst_inphase_dc_word_gain3: %04X, dcofst_quad_dc_word_gain3: %04X\n",
               dcofst_inphase_dc_word_gain3, dcofst_quad_dc_word_gain3);
    }
    else if (mode == FILTER_BW_CAL_MODE)
    {
        if (bw == 0)
        {
            uint8_t rxfil_cal_out_1 = PAN216_ReadReg(0x6B);
            my_log("rx 1M->0x6B: %04X\n", rxfil_cal_out_1 & 0x7f);
        }
        else if (bw == 1)
        {
            uint8_t rxfil_cal_out_2 = PAN216_ReadReg(0x6C);
            my_log("rx 2M->0x6C: %04X\n", rxfil_cal_out_2 & 0x7f);
        }
        else if (bw == 2)
        {
            uint8_t rxfil_cal_out_3 = PAN216_ReadReg(0x6D);
            my_log("rx 250K->0x6D: %04X\n", rxfil_cal_out_3 & 0x7f);
        }
    }
    else if (mode == FILTER_DCOC_CAL_MODE)
    {
        uint8_t t1, t2;
        u16 dcofst_inphase_dc_word_gain0;
        u16 dcofst_quad_dc_word_gain0;
        u16 dcofst_inphase_dc_word_gain1;
        u16 dcofst_quad_dc_word_gain1;
        u16 dcofst_inphase_dc_word_gain2;
        u16 dcofst_quad_dc_word_gain2;
        u16 dcofst_inphase_dc_word_gain3;
        u16 dcofst_quad_dc_word_gain3;
        t1 = (PAN216_ReadReg(0x74) & BIT7) >> 7;
        t2 = PAN216_ReadReg(0x75);
        dcofst_inphase_dc_word_gain0 = ((u16)t1 << 8) | (u16)t2;
        t1 = (PAN216_ReadReg(0x74) & BIT6) >> 6;
        t2 = PAN216_ReadReg(0x76);
        dcofst_quad_dc_word_gain0 = ((u16)t1 << 8) | (u16)t2;
        t1 = (PAN216_ReadReg(0x74) & BIT5) >> 5;
        t2 = PAN216_ReadReg(0x77);
        dcofst_inphase_dc_word_gain1 = ((u16)t1 << 8) | (u16)t2;
        t1 = (PAN216_ReadReg(0x74) & BIT4) >> 4;
        t2 = PAN216_ReadReg(0x78);
        dcofst_quad_dc_word_gain1 = ((u16)t1 << 8) | (u16)t2;
        t1 = (PAN216_ReadReg(0x74) & BIT3) >> 3;
        t2 = PAN216_ReadReg(0x79);
        dcofst_inphase_dc_word_gain2 = ((u16)t1 << 8) | (u16)t2;
        t1 = (PAN216_ReadReg(0x74) & BIT2) >> 2;
        t2 = PAN216_ReadReg(0x7A);
        dcofst_quad_dc_word_gain2 = ((u16)t1 << 8) | (u16)t2;
        t1 = (PAN216_ReadReg(0x74) & BIT1) >> 1;
        t2 = PAN216_ReadReg(0x7B);
        dcofst_inphase_dc_word_gain3 = ((u16)t1 << 8) | (u16)t2;
        t1 = PAN216_ReadReg(0x74) & BIT0;
        t2 = PAN216_ReadReg(0x7C);
        dcofst_quad_dc_word_gain3 = ((u16)t1 << 8) | (u16)t2;
        my_log("DCOC_CAL: 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x\n",
               dcofst_inphase_dc_word_gain0, dcofst_quad_dc_word_gain0,
               dcofst_inphase_dc_word_gain1, dcofst_quad_dc_word_gain1,
               dcofst_inphase_dc_word_gain2, dcofst_quad_dc_word_gain2,
               dcofst_inphase_dc_word_gain3, dcofst_quad_dc_word_gain3);
    }
}

static uint8_t _gChipMode = PAN216_CHIPMODE_XN297;
static uint8_t _gDataRate = PAN216_DR_1Mbps;
static uint8_t _gS2S8Mode = PAN216_PRIMODE_DIS;

const struct
{
    uint8_t page;
    uint8_t addr;
    uint8_t mask;
    uint8_t config[15];
} TxDemodTable[] = {
    {1, 0x32, 0x1f, {30, 31, 16, 24, 31, 16, 16, 31, 16, 16, 31, 16, 24, 31, 16}}, //  gauss_scale
    {1, 0x33, 0x3f, {25, 63, 28, 27, 50, 28, 25, 26, 25, 28, 28, 28, 27, 50, 28}}, //  df_sel
};

const struct
{
    PAN216_ChipMode_t ChipMode;
    PAN216_DataRate_t DataRate;
    uint8_t index;
} TxDemodIndex[] = {
    {PAN216_CHIPMODE_XN297, PAN216_DR_1Mbps, 3},   
    {PAN216_CHIPMODE_XN297, PAN216_DR_2Mbps, 4},
    {PAN216_CHIPMODE_XN297, PAN216_DR_250Kbps, 5}, 
{PAN216_CHIPMODE_FS01, PAN216_DR_1Mbps, 6},
    {PAN216_CHIPMODE_FS01, PAN216_DR_2Mbps, 7},    
    {PAN216_CHIPMODE_FS01, PAN216_DR_250Kbps, 8},
    {PAN216_CHIPMODE_FS32, PAN216_DR_1Mbps, 9},    
    {PAN216_CHIPMODE_FS32, PAN216_DR_2Mbps, 10},
    {PAN216_CHIPMODE_FS32, PAN216_DR_250Kbps, 11},
    {PAN216_CHIPMODE_BLE, PAN216_DR_1Mbps, 12},
    {PAN216_CHIPMODE_BLE, PAN216_DR_2Mbps, 13},    
    {PAN216_CHIPMODE_BLE, PAN216_DR_250Kbps, 14},
};

void WriteTxDemodConfig(void)
{
    for (int i = 0; i < sizeof(TxDemodIndex) / sizeof(TxDemodIndex[0]); i++)
    {
        if (_gChipMode == TxDemodIndex[i].ChipMode && _gDataRate == TxDemodIndex[i].DataRate)
        {
            uint8_t index = TxDemodIndex[i].index;
            PAN216_WritePageRegBits(TxDemodTable[0].page, TxDemodTable[0].addr,
                                    TxDemodTable[0].config[index], TxDemodTable[0].mask);
            PAN216_WritePageRegBits(TxDemodTable[1].page, TxDemodTable[1].addr,
                                    TxDemodTable[1].config[index], TxDemodTable[1].mask);
            return;
        }
    }
}

const struct
{
    uint8_t page;
    uint8_t addr;
    uint8_t cfg[6]; // CFG0 to CFG5
    uint8_t mask;   // Mask for the address field
} RxDemodTable[] = {
    {0, 0x38, {0x10, 0x10, 0x10, 0x0b, 0x0b, 0x13}, BITMASK_4_0}, //  Zcones_scale
    {0, 0x38, {0x00, 0x00, 0x02, 0x00, 0x02, 0x00}, BIT6 | BIT5}, //  Zcnormal_shift
    {0, 0x37, {0x60, 0x60, 0x60, 0x6B, 0x6B, 0x5a}, BITMASK_6_0}, //  Zcones_offset
    {0, 0x36, {0, 0, 1, 0, 1, 0}, BIT7},                          //  Demod_dfe_en_ovrd
    {0, 0x36, {1, 1, 0, 1, 0, 1}, BIT6},                          //  Demod_en_dfe
    {0, 0x36, {5, 5, 0, 4, 0, 6}, BITMASK_3_0},                   //  Demod_coeff_unfilt
    {1, 0x07, {7, 7, 7, 5, 7, 8}, BITMASK_7_4},                   //  Demod_unfilt_thresh
    {1, 0x07, {5, 5, 5, 4, 5, 6}, BITMASK_3_0},                   //  Demod_ffe_thresh
    {1, 0x0d, {9, 9, 9, 6, 9, 0x0b}, BITMASK_5_0},                //  Demod_dfe_coeff
    {1, 0x0f, {0x0f, 0x0f, 0x0f, 0x0a, 0x0f, 0x12}, BITMASK_4_0}, //  Demod_dfe_ff_coeff
    {1, 0x0e, {0, 1, 1, 0, 0, 0}, BIT7},                          //  Demod_agg_cdr_ovrd
    {1, 0x0e, {1, 0, 0, 1, 1, 1}, BIT6},                          //  Demod_aggressive_cdr
    {1, 0x15, {1, 0, 0, 1, 1, 1}, BIT6},                          //  I_long_rang_cdr_en
    {1, 0x5c, {1, 0, 0, 1, 1, 1}, BIT7},                          //  I_cdr_init_en
    {1, 0x5d, {0, 1, 1, 0, 0, 0}, BIT6},                          //  I_samp_mid_action_en
    {1, 0x0a, {1, 0, 0, 1, 0, 1}, BIT7},                          //  clr_offset_est_en
};

const struct
{
    PAN216_ChipMode_t ChipMode;
    PAN216_DataRate_t DataRate;
    PAN216_S2S8Mode_t S2S8Mode;
    uint8_t index;
} RxDemodIndex[] = {
    {PAN216_CHIPMODE_XN297, PAN216_DR_1Mbps, PAN216_PRIMODE_DIS, 0},
    {PAN216_CHIPMODE_XN297, PAN216_DR_2Mbps, PAN216_PRIMODE_DIS, 0},
    {PAN216_CHIPMODE_XN297, PAN216_DR_250Kbps, PAN216_PRIMODE_DIS, 4},
{PAN216_CHIPMODE_FS01, PAN216_DR_1Mbps, PAN216_PRIMODE_DIS, 3},
    {PAN216_CHIPMODE_FS01, PAN216_DR_2Mbps, PAN216_PRIMODE_DIS, 3},
    {PAN216_CHIPMODE_FS01, PAN216_DR_250Kbps, PAN216_PRIMODE_DIS, 4},
    {PAN216_CHIPMODE_FS32, PAN216_DR_1Mbps, PAN216_PRIMODE_DIS, 3},
    {PAN216_CHIPMODE_FS32, PAN216_DR_2Mbps, PAN216_PRIMODE_DIS, 3},
    {PAN216_CHIPMODE_FS32, PAN216_DR_250Kbps, PAN216_PRIMODE_DIS, 4},
    {PAN216_CHIPMODE_BLE, PAN216_DR_1Mbps, PAN216_PRIMODE_DIS, 0},
    {PAN216_CHIPMODE_BLE, PAN216_DR_1Mbps, PAN216_PRIMODE_S2, 1},
    {PAN216_CHIPMODE_BLE, PAN216_DR_1Mbps, PAN216_PRIMODE_S8, 1},
    {PAN216_CHIPMODE_BLE, PAN216_DR_2Mbps, PAN216_PRIMODE_DIS, 0},
    {PAN216_CHIPMODE_BLE, PAN216_DR_250Kbps, PAN216_PRIMODE_DIS, 4},
    {PAN216_CHIPMODE_BLE, PAN216_DR_250Kbps, PAN216_PRIMODE_S2, 2},
    {PAN216_CHIPMODE_BLE, PAN216_DR_250Kbps, PAN216_PRIMODE_S8, 2},
};

void WriteRxDemodConfig(void)
{
    for (int i = 0; i < sizeof(RxDemodIndex) / sizeof(RxDemodIndex[0]); i++)
    {
        if (_gChipMode == RxDemodIndex[i].ChipMode && _gDataRate == RxDemodIndex[i].DataRate &&
            _gS2S8Mode == RxDemodIndex[i].S2S8Mode)
        {
            for (int j = 0; j < 16; j++)
            {
                PAN216_WritePageRegBits(RxDemodTable[j].page, RxDemodTable[j].addr,
                                        RxDemodTable[j].cfg[RxDemodIndex[i].index],
                                        RxDemodTable[j].mask);
            }
            return;
        }
    }
}

PAN216_Err_t PAN216_DRModConfig(void)
{
    switch (_gDataRate)
    {
    case PAN216_DR_1Mbps:
        PAN216_WritePageRegBits(PAN216_PAGE0, 0x43, 0, BIT2);
        PAN216_WritePageRegBits(PAN216_PAGE0, 0x43, 2, BIT1 | BIT0);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x3A, 0, BIT7 | BIT6);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x49, 1, BIT7);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x49, 4, BIT6 | BIT5 | BIT4);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x4C, 0, BIT5);
        break;
    case PAN216_DR_250Kbps:
        PAN216_WritePageRegBits(PAN216_PAGE0, 0x43, 0, BIT2);
        PAN216_WritePageRegBits(PAN216_PAGE0, 0x43, 3, BIT1 | BIT0);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x3A, 1, BIT7 | BIT6);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x49, 0, BIT7);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x49, 4, BIT6 | BIT5 | BIT4);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x4C, 0, BIT5);
        break;
    case PAN216_DR_2Mbps:
        PAN216_WritePageRegBits(PAN216_PAGE0, 0x43, 1, BIT2);
        PAN216_WritePageRegBits(PAN216_PAGE0, 0x43, 2, BIT1 | BIT0);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x3A, 0, BIT7 | BIT6);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x49, 1, BIT7);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x49, 4, BIT6 | BIT5 | BIT4);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x4C, 1, BIT5);
        break;
    default:
        break;
    }
    return PAN216_OK;
}

PAN216_Err_t PAN216_EnableFifo128bytes(bool NewState)
{
    if (NewState)
    {
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG1, 1, WMODE_CFG1_FIFO_128_EN));
    }
    else
    {
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG1, 0, WMODE_CFG1_FIFO_128_EN));
    }

    return PAN216_OK;
}

/**
 * @brief Set the transmit mode of PAN216 transceiver.
 *
 * @param TxMode The transmit mode to be set.
 *        - PAN216_TX_MODE_SINGLE: single transmission
 *        - PAN216_TX_MODE_CONTINOUS: continuous transmission
 *
 * @return Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetTxMode(PAN216_TxMode_t TxMode)
{
    switch (TxMode)
    {
    case PAN216_TX_MODE_SINGLE:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TRXMODE_CFG, PAN216_TX_MODE_SINGLE,
                                         TRXMODE_CFG_REG_TX_CFG_MODE));
        break;
    case PAN216_TX_MODE_CONTINOUS:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TRXMODE_CFG, PAN216_TX_MODE_CONTINOUS,
                                         TRXMODE_CFG_REG_TX_CFG_MODE));
        break;
    default:
        return PAN216_ERR;
    }

    return PAN216_OK;
}

/**
 * @brief Set the receive mode of PAN216 transceiver.It has to be initialized  as STB3 mode before
 * entering RX mode.
 *
 * @param TxMode The receive mode to be set.
 *        - PAN216_RX_MODE_SINGLE: single reception
 *        - PAN216_RX_MODE_SINGLE_WITH_TIMEOUT: single reception with timeout
 *        - PAN216_RX_MODE_CONTINOUS: continuous reception
 *
 * @return Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetRxMode(PAN216_RxMode_t RxMode)
{
    switch (RxMode)
    {
    case PAN216_RX_MODE_SINGLE:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TRXMODE_CFG, PAN216_RX_MODE_SINGLE,
                                         TRXMODE_CFG_REG_RX_CFG_MODE));
        break;
    case PAN216_RX_MODE_SINGLE_WITH_TIMEOUT:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TRXMODE_CFG,
                                         PAN216_RX_MODE_SINGLE_WITH_TIMEOUT,
                                         TRXMODE_CFG_REG_RX_CFG_MODE));
        break;
    case PAN216_RX_MODE_CONTINOUS:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TRXMODE_CFG, PAN216_RX_MODE_CONTINOUS,
                                         TRXMODE_CFG_REG_RX_CFG_MODE));
        break;
    default:
        break;
    }

    return PAN216_OK;
}

/**
 * @brief  Performs a soft reset of the PAN216 transceiver module.
 *
 * @param  None
 *
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 * @note This function is designed for general operations.
 */
PAN216_Err_t PAN216_SoftReset(void)
{
    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, STATE_CFG, 0, STATE_CFG_POR_NRSTL));
    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, SYS_CFG, 0, SYS_CFG_REG_SOFT_RSTL));
    Pan216_Funs.delayms(1);
    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, STATE_CFG, 1, STATE_CFG_POR_NRSTL));
    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, SYS_CFG, 1, SYS_CFG_REG_SOFT_RSTL));

    return PAN216_OK;
}

PAN216_State_t PAN216_GetState(void)
{
    uint8_t idleFlag;
    PAN216_State_t State;

    idleFlag = PAN216_ReadReg(0x7D) >> 7; // bit7为1表示mac处于IDLE
    State = (PAN216_State_t)(PAN216_ReadReg(0x7F) >> 6);

    if (State == 0)
    {
        State = idleFlag > 0 ? PAN216_STATE_IDLE : PAN216_STATE_ARD_WAIT;
    }

    return State;
}

/**
 * @brief Writes the PAN216 transceiver's TX PID value of a enhanced shockburst packet
 *
 * @param Pid 0~3
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_WriteTxManualPid(uint8_t Pid)
{
    PAN216_WriteRegBits(PID_CFG, Pid, PID_CFG_TX_PID_MANUAL);

    return PAN216_OK;
}

/**
 * @brief Writes the PAN216 transceiver's RX PID value of a enhanced shockburst packet
 *
 * @param Pid 0~3
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_WriteRxManualPid(uint8_t Pid)
{
    PAN216_WriteRegBits(PID_CFG, Pid, PID_CFG_RX_PID_MANUAL);

    return PAN216_OK;
}

/**
 * @brief Sets the DynamicPayloadLength feature for all pipes of the PAN216 transceiver
 *
 * This function is used to enable or disable the DynamicPayloadLength feature for all
 * pipes of the PAN216 transceiver.
 *
 * @param NewState ENABLE to enable the feature, DISABLE to disable the feature
 *
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_EnableDynamicPL(uint8_t NewState)
{
    if (NewState)
    {
        P_ASSERT(PAN216_SetRegBits(PAN216_PAGE0, WMODE_CFG1, WMODE_CFG1_DPY_EN));
    }
    else
    {
        P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG1, WMODE_CFG1_DPY_EN));
    }

    return PAN216_OK;
}

/**
 * @brief Configures the CRC scheme of the PAN216 transceiver
 *
 * This function is used to configure the CRC scheme of the PAN216 transceiver.
 *
 * @param Crc The desired CRC scheme, as a value of type PAN216_Crc_t
 *
 * @note The transceiver will forcibly turn on the CRC if auto-acknowledgment is enabled
 *       for at least one receive (RX) pipe
 *
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetCrcScheme(PAN216_Crc_t Crc)
{
    if (Crc > PAN216_CRC_3byte)
    {
        return PAN216_ERR;
    }

    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, Crc, WMODE_CFG0_CRC_MODE_1_0));

    return PAN216_OK;
}

/**
 * @brief Configures the data rate of the PAN216 transceiver
 *
 * This function is used to configure the data rate of the PAN216 transceiver. The
 * data rate is specified as one of the members of PAN216_DataRate_t.
 *
 * @param DataRate The desired data rate, as one of the members of PAN216_DataRate_t
 *
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetDataRate(PAN216_DataRate_t DataRate)
{
    uint8_t DataRateVal;

    if (DataRate >= PAN216_DR_Max)
    {
        return PAN216_ERR;
    }

    switch (DataRate)
    {
    case PAN216_DR_1Mbps:
        DataRateVal = 0x00;
        _gDataRate = PAN216_DR_1Mbps;
        break;
    case PAN216_DR_2Mbps:
        DataRateVal = 0x01;
        _gDataRate = PAN216_DR_2Mbps;
        break;
    case PAN216_DR_250Kbps:
        DataRateVal = 0x03;
        _gDataRate = PAN216_DR_250Kbps;
        break;
    default:
        return PAN216_ERR;
    }

    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, REG_P0_0X36, DataRateVal, REG_P0_0X36_BW_MODE));

    PAN216_DRModConfig();
    WriteTxDemodConfig();
    WriteRxDemodConfig();
    PAN216_Calibration();
#if CALIBRATION_DEBUG_ON
    PAN216_ReadVCOCode();
    PAN216_ReadTpCode();
    ReadBWDcocCal(FILTER_OFFSET_CAL_MODE, 0);
    ReadBWDcocCal(FILTER_BW_CAL_MODE, 0);
    ReadBWDcocCal(FILTER_BW_CAL_MODE, 1);
    ReadBWDcocCal(FILTER_BW_CAL_MODE, 2);
    ReadBWDcocCal(FILTER_DCOC_CAL_MODE, 0);
#endif
    PAN216_FinishCalibration();
    return PAN216_OK;
}

PAN216_Err_t PAN216_EnableRxPipe(PAN216_Pipe_t Pipe)
{
    if (Pipe > PAN216_PIPE5)
    {
        return PAN216_ERR;
    }

    return PAN216_SetRegBits(PAN216_PAGE0, RXPIPE_CFG, 1 << Pipe);
}

PAN216_Err_t PAN216_DisableRxPipe(PAN216_Pipe_t Pipe)
{
    if (Pipe > PAN216_PIPE5)
    {
        return PAN216_ERR;
    }

    return PAN216_ResetRegBits(PAN216_PAGE0, RXPIPE_CFG, 1 << Pipe);
}

/**
 * @brief Configure a specified RX pipe
 *
 * @param Pipe        Number of the RX pipe, value from 0 to 6
 * @param PayloadLen  Payload length in bytes
 *
 * @return Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetRxPayloadLen(uint8_t PayloadLen)
{
    return PAN216_WritePageReg(PAN216_PAGE0, RXPLLEN_CFG, PayloadLen);
}

PAN216_Err_t PAN216_SetTxPayloadLen(uint8_t PayloadLen)
{
    P_ASSERT(PAN216_WritePageReg(PAN216_PAGE0, TXPLLEN_CFG, PayloadLen));
    return PAN216_OK;
}

/**
 * @brief Set enhanced shockburst mode for the PAN216 transceiver
 *
 * @param Mode Enhanced shockburst mode to set, one of PAN216_Mode_t values
 *
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetWorkMode(PAN216_WorkMode_t Mode)
{

    switch (Mode)
    {
    case PAN216_WORKMODE_NORMAL:
        P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG1,
                                     WMODE_CFG1_NORMAL_M1 | WMODE_CFG1_ENHANCE));
        break;
    case PAN216_WORKMODE_ENHANCE:
        P_ASSERT(PAN216_SetRegBits(PAN216_PAGE0, WMODE_CFG1, WMODE_CFG1_ENHANCE));
        P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG1, WMODE_CFG1_NORMAL_M1));
        break;
    default:
        return PAN216_ERR;
    }

    return PAN216_OK;
}

PAN216_Err_t PAN216_GetChipMode(PAN216_ChipMode_t *chipMode)
{
    *chipMode = (PAN216_ChipMode_t)_gChipMode;
    return PAN216_OK;
}

/**
 * @brief  This function set noack for tx
 * @param  NewState: ENABLE/DISABLE
 * @return Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 * @note  This function is called after PAN216_SetWorkMode
 */
PAN216_Err_t PAN216_EnableTxNoAck(uint8_t NewState)
{
    uint8_t modeCfg1 = PAN216_ReadPageReg(PAN216_PAGE0, WMODE_CFG1);

    if (modeCfg1 & WMODE_CFG1_ENHANCE)
    {
        if (NewState)
        {
            P_ASSERT(PAN216_SetRegBits(PAN216_PAGE0, WMODE_CFG0, WMODE_CFG0_TX_NOACK_EN));
        }
        else
        {
            P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG0, WMODE_CFG0_TX_NOACK_EN));
        }
        P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG1, WMODE_CFG1_NORMAL_M1));
    }
    else
    {
        if (NewState)
        {
            P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG1, WMODE_CFG1_NORMAL_M1));
        }
        else
        {
            P_ASSERT(PAN216_SetRegBits(PAN216_PAGE0, WMODE_CFG1, WMODE_CFG1_NORMAL_M1));
        }
        P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG0, WMODE_CFG0_TX_NOACK_EN));
    }

    return PAN216_OK;
}

/**
 * @brief After receiving a packet, reads the TxNoack bit of a enhanced shockburst packet
 *
 * @return uint8_t  1 : rx need ack
 *                  0 : rx need no ack
 */
uint8_t PAN216_ReadTxNoackBit(void)
{
    return !(PAN216_ReadReg(0x7D) & BIT6);
}

PAN216_Err_t PAN216_SetAckPipe(uint8_t pipe)
{
    if (pipe > 5)
    {
        return PAN216_ERR;
    }

    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, 0x6F, pipe, 0x07));

    return PAN216_OK;
}

/**
 * @brief Enable CRC calculation for address field in PAN216.
 *
 * This function allows enabling or disabling
 *  CRC calculation for the address field in PAN216.
 *
 * @note By default, address field is not included in CRC calculation.
 *
 * @param[in] NewState Boolean value indicating whether to enable or disable CRC calculation for
 * address field.
 *
 * @return Result of the operation:
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_CrcSkipAddr(bool exclude)
{
    if (exclude)
    {
        return PAN216_SetRegBits(PAN216_PAGE0, WMODE_CFG0, WMODE_CFG0_ACCADDR_CRC_DIS);
    }
    else
    {
        return PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG0, WMODE_CFG0_ACCADDR_CRC_DIS);
    }
}

/**
 * @brief whiten(scramble) configurations
 * @param addr_excluded  whiten exclude access address(already include header, payload, crc)
 * @param initital_phase initital whiten phase
 *        - channel 37: 0x53
          - channel 38: 0x33
          - channel 39: 0x73
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_WhiteSkipAddr(bool addr_excluded)
{
    PAN216_Err_t ret;

    if (addr_excluded)
    {
        ret = PAN216_SetRegBits(PAN216_PAGE0, SCR_CFG, SCR_CFG_ACCADDR_SCR_DIS);
    }
    else
    {
        ret = PAN216_ResetRegBits(PAN216_PAGE0, SCR_CFG, SCR_CFG_ACCADDR_SCR_DIS);
    }

    return ret;
}

/**
 * @brief
 *
 * @param Endian
 * @return uint8_t
 */
PAN216_Err_t PAN216_SetEndian(PAN216_Endian_t Endian)
{

    if (Endian == PAN216_ENDIAN_LITTLE)
    {
        PAN216_WritePageRegBits(PAN216_PAGE0, 0x6F, 1,
                                BIT4); // 默认为0，pid在中间；配置为1时，pid在最低两位
    }
    else
    {
        PAN216_WritePageRegBits(PAN216_PAGE0, 0x6F, 0,
                                BIT4); // 默认为0，pid在中间；配置为1时，pid在最低两位
    }

    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, Endian, WMODE_CFG0_ENDIAN));

    return PAN216_OK;
}

PAN216_Err_t PAN216_EnableWhiten(bool NewState)
{
    if (NewState)
    {
        P_ASSERT(PAN216_SetRegBits(PAN216_PAGE0, WMODE_CFG0, WMODE_CFG0_SCR_ENABLE));
    }
    else
    {
        P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, WMODE_CFG0, WMODE_CFG0_SCR_ENABLE));
    }

    return PAN216_OK;
}

/**
 * @brief  This function open the white list filtering functions
 * @param  Start : filtering the starting position
 * @param  FilterBuf : content of the filter
 * @param  FilterLen : 0 <= filter_len <= 6
 * @return Result of the operation:
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetBleWhitelist(uint8_t Start, uint8_t *FilterBuf, uint8_t FilterLen)
{
    uint8_t start_reg = WLIST0_CFG + 5;

    if (FilterLen > 6)
    {
        return PAN216_ERR;
    }

    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, BLEMATCHSTART_CFG, Start,
                                     BLEMATCHSTART_CFG_PLD_START_BYTE));
    start_reg -= (FilterLen - 1);
    P_ASSERT(PAN216_WritePageRegs(PAN216_PAGE0, start_reg, FilterBuf, FilterLen));
    return PAN216_OK;
}

/**
 * @brief  This function open the white list filtering functions
 * @param  FilterType : filtering type
 * @param  FilterLen : filter the number of bytes
 * @return Result of the operation:
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetBleLenFilter(PAN216_BleLenFilter_t FilterType)
{
    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, BLEMATCH_CFG0, FilterType,
                                     BLEMATCH_CFG0_BLELEN_MATCH_MODE));

    return PAN216_OK;
}

PAN216_Err_t PAN216_SetBleWLMatchMode(PAN216_BLEWLMatchMode_t MatchMode)
{
    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, BLEMATCH_CFG0, MatchMode,
                                     BLEMATCH_CFG0_WL_MATCH_MODE));

    return PAN216_OK;
}

/**
 * @brief Enables or disables the manual configuration of the PAN216 TX and RX PID.
 *
 * @param NewState The new state of the manual configuration.
 *                  This parameter can be ENABLE or DISABLE.
 * @return Result of the operation.
 *         - PAN216_OK.
 *         - PAN216_ERR.
 *
 * @note This function is used to enable or disable the manual configuration of the PAN216 TX and RX
 * PIDs. By default, manual configuration is disabled.
 */
PAN216_Err_t PAN216_EnableManualPid(uint8_t NewState)
{
    if (NewState)
        return PAN216_WritePageRegBits(PAN216_PAGE0, PID_CFG, 1, PID_CFG_PID_MANUAL_EN);
    else
        return PAN216_WritePageRegBits(PAN216_PAGE0, PID_CFG, 0, PID_CFG_PID_MANUAL_EN);
}

/**
 * @brief This function set the timeout period for the tx node
 *         to wait for an ACK during enhanced mode.
 * @param AckTimeoutUs: 0~65535us
 * @return Result of the operation.
 *         - PAN216_OK.
 *         - PAN216_ERR.
 * @note This function must be called after RF_SetRate.
 */
PAN216_Err_t PAN216_SetWaitAckTimeout(u16 AckTimeoutUs)
{
    uint8_t time[2] = {
        (uint8_t)(AckTimeoutUs & 0xff),
        (uint8_t)((AckTimeoutUs >> 8) & 0xff),
    };

    return PAN216_WritePageRegs(PAN216_PAGE0, RXTIMEOUTL_CFG, time, 2);
}

/**
 * @brief This function set the timeout period for the tx node
 *         to wait for an ACK during enhanced mode.
 * @param TransWaitTime: 0~32767us
 * @return Result of the operation.
 *         - PAN216_OK.
 *         - PAN216_ERR.
 * @note This function must be called after RF_SetRate.
 */
PAN216_Err_t PAN216_SetTRxTransTime(u16 TransWaitTimeUs)
{
    uint8_t time[2] = {
        (uint8_t)(TransWaitTimeUs & 0xff),
        (uint8_t)((TransWaitTimeUs >> 8) & 0xff),
    };

    return PAN216_WritePageRegs(PAN216_PAGE0, TRXTWTL_CFG, time, 2);
}

/**
 * @brief Set automatic retransmission parameters.
 *
 * @param DelayUs Auto retransmit delay, value from 250 to 4000, unit is us.
 * @param MaxCnt Count of auto retransmits, value from 0 to 15.
 *         Zero value means that the automatic retransmission is disabled.
 * @return Result of the operation.
 *         - PAN216_OK.
 *         - PAN216_ERR.
 */
PAN216_Err_t PAN216_SetAutoRetrans(u16 DelayUs, uint8_t MaxCnt)
{
    if (DelayUs < 250)
    {
        DelayUs = 250;
    }

    DelayUs /= 250;
    DelayUs--;

    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TXAUTO_CFG, (uint8_t)DelayUs, TXAUTO_CFG_ARD));
    P_ASSERT(
        PAN216_WritePageRegBits(PAN216_PAGE0, TXAUTO_CFG, (uint8_t)MaxCnt, TXAUTO_CFG_ARC_3_0));

    return PAN216_OK;
}

PAN216_Err_t PAN216_RxLengthLimit(bool NewState)
{
    if (NewState)
    {
        return PAN216_SetRegBits(PAN216_PAGE0, PKT_EXT_CFG, PKT_EXT_CFG_W_RX_MAX_CTRL_EN);
    }
    else
    {
        return PAN216_ResetRegBits(PAN216_PAGE0, PKT_EXT_CFG, PKT_EXT_CFG_W_RX_MAX_CTRL_EN);
    }
}

#define POWER_TABLE_SIZE 6

uint8_t sop8_power_table_reg[POWER_TABLE_SIZE][3] = {
    //{PAN216_PAGE1, 0x48, 0x0f}, //
    {PAN216_PAGE1, 0x3C, 0x07}, //
    {PAN216_PAGE0, 0x43, 0x30}, //
    {PAN216_PAGE0, 0x44, 0xf0}, //
    {PAN216_PAGE0, 0x44, 0x0f}, //
    {PAN216_PAGE1, 0x46, BIT0}, //
    {PAN216_PAGE1, 0x46, 0x0c}, //
};

const struct
{
    PAN216_TxPower_t power_level;
    int val[POWER_TABLE_SIZE];
} sop8_power_table[] = {
    {PAN216_TXPWR_9_5dBm,       {0x07, 0x03, 0x08, 0x0c, 0x00, 0x00}},
    {PAN216_TXPWR_9dBm,         {0x07, 0x03, 0x08, 0x08, 0x00, 0x00}},
    {PAN216_TXPWR_8dBm,         {0x07, 0x03, 0x08, 0x06, 0x00, 0x00}},
    {PAN216_TXPWR_7dBm,         {0x07, 0x03, 0x08, 0x03, 0x00, 0x00}},
    {PAN216_TXPWR_6dBm,         {0x07, 0x03, 0x08, 0x04, 0x00, 0x01}},
    {PAN216_TXPWR_5dBm,         {0x07, 0x03, 0x08, 0x02, 0x00, 0x01}},
    {PAN216_TXPWR_4dBm,         {0x07, 0x03, 0x08, 0x00, 0x00, 0x01}},
    {PAN216_TXPWR_3dBm,         {0x07, 0x03, 0x08, 0x00, 0x00, 0x02}},
    {PAN216_TXPWR_2dBm,         {0x03, 0x03, 0x08, 0x02, 0x00, 0x03}},
    {PAN216_TXPWR_1dBm,         {0x03, 0x03, 0x08, 0x00, 0x00, 0x03}},
    {PAN216_TXPWR_0dBm,         {0x03, 0x03, 0x08, 0x04, 0x01, 0x03}},
    {PAN216_TXPWR_0dBm_LOWPWR,  {0x07, 0x01, 0x08, 0x0F, 0x00, 0x00}},
    {PAN216_TXPWR_n1dBm,        {0x04, 0x03, 0x08, 0x00, 0x01, 0x03}},
    {PAN216_TXPWR_n2dBm,        {0x07, 0x01, 0x0F, 0x0F, 0x00, 0x01}},
    {PAN216_TXPWR_n4dBm,        {0x07, 0x01, 0x0F, 0x0F, 0x01, 0x03}},
    {PAN216_TXPWR_n5dBm,        {0x07, 0x01, 0x0F, 0x0F, 0x01, 0x03}},
    {PAN216_TXPWR_n7dBm,        {0x03, 0x01, 0x08, 0x08, 0x01, 0x03}},
    {PAN216_TXPWR_n8dBm,        {0x03, 0x01, 0x08, 0x04, 0x01, 0x01}},
    {PAN216_TXPWR_n10dBm,       {0x03, 0x01, 0x08, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n11dBm,       {0x03, 0x01, 0x06, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n12dBm,       {0x03, 0x01, 0x05, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n14dBm,       {0x03, 0x01, 0x04, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n16dBm,       {0x03, 0x01, 0x03, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n19dBm,       {0x03, 0x01, 0x02, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n23dBm,       {0x03, 0x01, 0x01, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n25dBm,       {0x02, 0x01, 0x01, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n28dBm,       {0x01, 0x01, 0x01, 0x08, 0x01, 0x00}},
    {PAN216_TXPWR_n33dBm,       {0x03, 0x01, 0x00, 0x00, 0x01, 0x00}},
    {PAN216_TXPWR_n37dBm,       {0x00, 0x01, 0x00, 0x00, 0x00, 0x00}},
    {PAN216_TXPWR_n40dBm,       {0x00, 0x01, 0x00, 0x00, 0x01, 0x00}},

};

/**
 * @brief Configures the RF output power in TX mode of the PAN216 transceiver
 *
 * This function is used to set the RF output power of the PAN216 transceiver in TX mode.
 *
 * @param TxPower The desired RF output power, as one of the members of PAN216_TxPower_t
 *
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetTxPower(PAN216_TxPower_t TxPower)
{
    PAN216_Err_t ret = PAN216_OK;
    uint8_t i;

    for (i = 0; i < sizeof(sop8_power_table) / sizeof(sop8_power_table[0]); i++)
    {
        if (sop8_power_table[i].power_level == TxPower)
        {
            for (uint8_t j = 0; j < POWER_TABLE_SIZE; j++)
            {
                PAN216_WritePageRegBits(sop8_power_table_reg[j][0], sop8_power_table_reg[j][1],
                                        sop8_power_table[i].val[j], sop8_power_table_reg[j][2]);
            }
            break;
        }
    }

    if (TxPower == PAN216_TXPWR_0dBm_LOWPWR || TxPower == PAN216_TXPWR_n4dBm)
    {
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x48, 0x0C, BITMASK_3_0);
    }

    return ret;
}

/**
 * @brief Set Nordic packet header format
 * @param HeaderEn Enable/disable header (1/0)
 * @param HeaderLen Header length in bytes (valid only if HeaderEn=1)
 * @note Only valid in Nordic normal mode
 * @return PAN216_Err_t Operation result
 */
PAN216_Err_t PAN216_SetNordicPktHeader(uint8_t HeaderEn, uint8_t HeaderLen)
{
    P_ASSERT(
        PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, !!HeaderEn, PKT_EXT_CFG_HDR_LEN_EXIST));

    P_ASSERT(
        PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, HeaderLen, PKT_EXT_CFG_HDR_LEN_NUMB));

    return PAN216_OK;
}

PAN216_Err_t PAN216_WriteNordicPktHeader(uint8_t Header0, uint8_t Header1,
                                         uint8_t Length) // 写nordic包头内容
{
    P_ASSERT(PAN216_WritePageReg(PAN216_PAGE0, TXHDR0_CFG, Header0));
    P_ASSERT(PAN216_WritePageReg(PAN216_PAGE0, TXHDR1_CFG, Header1));
    P_ASSERT(PAN216_WritePageReg(PAN216_PAGE0, TXPLLEN_CFG, Length));

    return PAN216_OK;
}

PAN216_Err_t PAN216_SetS2S8Mode(PAN216_S2S8Mode_t mode)
{
    if (mode == PAN216_PRIMODE_DIS)
    {
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TRXMODE_CFG, 1, TRXMODE_CFG_W_PRE_SYNC_EN));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 0, PKT_EXT_CFG_PRI_CI_MODE));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 0, PKT_EXT_CFG_PRI_TX_FEC));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 0, PKT_EXT_CFG_PRI_RX_FEC));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE1, 0x0B, 0, BIT5));
        _gS2S8Mode = PAN216_PRIMODE_DIS;
    }
    else if (mode == PAN216_PRIMODE_S2)
    {
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TRXMODE_CFG, 0, TRXMODE_CFG_W_PRE_SYNC_EN));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 1, PKT_EXT_CFG_PRI_CI_MODE));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 1, PKT_EXT_CFG_PRI_TX_FEC));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 1, PKT_EXT_CFG_PRI_RX_FEC));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE1, 0x0B, 1, BIT5));
        _gS2S8Mode = PAN216_PRIMODE_S2;
    }
    else if (mode == PAN216_PRIMODE_S8)
    {
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, TRXMODE_CFG, 0, TRXMODE_CFG_W_PRE_SYNC_EN));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 0, PKT_EXT_CFG_PRI_CI_MODE));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 1, PKT_EXT_CFG_PRI_TX_FEC));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, PKT_EXT_CFG, 1, PKT_EXT_CFG_PRI_RX_FEC));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE1, 0x0B, 1, BIT5));
        _gS2S8Mode = PAN216_PRIMODE_S8;
    }

    return PAN216_OK;
}

/**
 * @brief Set the PAN216 IRQ pin mux
 *
 * @param Mux
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_setIRQPinMux(PAN216_IRQPinMux_t Mux)
{
    if (PAN216_IRQ_PINMUX_PACTRL == Mux)
    {
        P_ASSERT(PAN216_WriteRegBits(0x6C, 1, BIT6));
    }
    else
    {
        P_ASSERT(PAN216_WriteRegBits(0x6C, 0, BIT6));
        P_ASSERT(PAN216_WriteRegBits(0x45, 3, 0x0C)); // SEL 16M OCLK
    }

    P_ASSERT(PAN216_WriteRegBits(0x45, Mux, 0x03));

    return PAN216_OK;
}

/**
 * @brief Set the Chip frame format type mode and endian type of PAN216 transceiver
 *
 * @param chipMode supported chip format frame type
 *          - PAN216_CHIPMODE_XN297
 *          - ...
 *          - PAN216_CHIPMODE_BLE
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_SetChipMode(PAN216_ChipMode_t chipMode, PAN216_Endian_t endian,
                                bool crcSkipAddr)
{
    _gChipMode = chipMode;

    switch (chipMode)
    {
    case PAN216_CHIPMODE_XN297:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, 0, WMODE_CFG0_CHIP_MODE));
        P_ASSERT(PAN216_SetEndian(PAN216_ENDIAN_BIG));
        P_ASSERT(PAN216_CrcSkipAddr(false));
        P_ASSERT(PAN216_WhiteSkipAddr(false));
        P_ASSERT(PAN216_SetWhiteInitVal(0x7F));
        break;

case PAN216_CHIPMODE_FS01:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, 1, WMODE_CFG0_CHIP_MODE));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, 0, WMODE_CFG0_NORDIC_ENHANCE));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, REG_P0_0X6F, 0, REG_P0_0X6F_I_NDC_PREAMBLE_SEL)); 
        P_ASSERT(PAN216_SetEndian(PAN216_ENDIAN_BIG));
        P_ASSERT(PAN216_CrcSkipAddr(false));
        P_ASSERT(PAN216_WhiteSkipAddr(false));
        P_ASSERT(PAN216_SetWhiteInitVal(0x7F));
        break;

    case PAN216_CHIPMODE_FS32:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, 1, WMODE_CFG0_CHIP_MODE));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, 1, WMODE_CFG0_NORDIC_ENHANCE));
        P_ASSERT(PAN216_WritePageRegBits(
            PAN216_PAGE0, REG_P0_0X6F, 1,
            REG_P0_0X6F_I_NDC_PREAMBLE_SEL));
        P_ASSERT(PAN216_SetEndian(endian));
        P_ASSERT(PAN216_CrcSkipAddr(crcSkipAddr));
        P_ASSERT(PAN216_WhiteSkipAddr(true));
        P_ASSERT(PAN216_SetWhiteInitVal(0x7F));
        break;
    case PAN216_CHIPMODE_BLE:
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, 1, WMODE_CFG0_CHIP_MODE));
        P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG0, 1,
                                         WMODE_CFG0_NORDIC_ENHANCE));
        P_ASSERT(PAN216_SetEndian(PAN216_ENDIAN_LITTLE));
        P_ASSERT(PAN216_CrcSkipAddr(true));
        P_ASSERT(PAN216_WhiteSkipAddr(true));

        break;
    default:
        return PAN216_ERR;
    }

    // when recv error occurs, the chip will enter rx mode automatically
    P_ASSERT(PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG1, 1, WMODE_CFG1_RX_GOON));

    return PAN216_OK;
}

PAN216_Err_t PAN216_SetUpConfig(RFConfig_t *pCfg)
{
    P_ASSERT(
        PAN216_SetChipMode((PAN216_ChipMode_t)pCfg->ChipMode, pCfg->Endian, pCfg->crcSkipAddr));
    P_ASSERT(PAN216_SetChannel(pCfg->Channel));
    P_ASSERT(PAN216_SetDataRate((PAN216_DataRate_t)pCfg->DataRate));
    P_ASSERT(PAN216_SetCrcScheme((PAN216_Crc_t)pCfg->Crc));
    P_ASSERT(PAN216_SetTxPayloadLen(pCfg->TxLen));
    P_ASSERT(PAN216_SetRxPayloadLen(pCfg->RxLen));
    P_ASSERT(PAN216_SetAddrWidth((PAN216_AddrWidth_t)pCfg->TxAddrWidth));
    P_ASSERT(PAN216_SetTxAddr(pCfg->TxAddr, pCfg->TxAddrWidth));

    for (int i = 0; i < PIPE_MAX_NUM; i++)
    {
        if (pCfg->RxAddr[i].EnPipe)
        {
            P_ASSERT(PAN216_EnableRxPipe((PAN216_Pipe_t)i));
            P_ASSERT(PAN216_SetRxAddr((PAN216_Pipe_t)i, pCfg->RxAddr[i].Addr, pCfg->RxAddrWidth));
        }
        else
        {
            P_ASSERT(PAN216_DisableRxPipe((PAN216_Pipe_t)i));
        }
    }

    P_ASSERT(PAN216_EnableWhiten(pCfg->EnWhite));
    if (pCfg->EnTxNoAck == 0)
    {
        PAN216_SetTRxTransTime(pCfg->TRxDelayTimeUs);
        PAN216_SetAutoRetrans(pCfg->AutoDelayUs, pCfg->AutoMaxCnt);
        PAN216_SetWaitAckTimeout(pCfg->RxTimeoutUs);
        P_ASSERT(PAN216_EnableFifo128bytes(false));
    }
    else
    {
        PAN216_SetWaitAckTimeout(pCfg->RxTimeoutUs); // us
        P_ASSERT(PAN216_EnableFifo128bytes(true));
    }
    /* Enhanced parameters */
    P_ASSERT(PAN216_SetWorkMode(pCfg->WorkMode));
    P_ASSERT(PAN216_EnableDynamicPL(pCfg->EnDPL));
    P_ASSERT(PAN216_EnableTxNoAck(pCfg->EnTxNoAck));
    P_ASSERT(PAN216_RxLengthLimit(pCfg->EnRxPlLenLimit));

    PAN216_SetTxPower((PAN216_TxPower_t)pCfg->TxPower);

    if (pCfg->WorkMode == PAN216_WORKMODE_ENHANCE)
    {
        PAN216_SetTxMode(PAN216_TX_MODE_SINGLE);
        PAN216_SetRxMode(PAN216_RX_MODE_SINGLE);
    }
    else if (pCfg->WorkMode == PAN216_WORKMODE_NORMAL)
    {
        PAN216_SetTxMode(pCfg->TxMode);
        PAN216_SetRxMode(pCfg->RxMode);
    }

    return PAN216_OK;
}

PAN216_Err_t PAN216_BleConfig(BLEConfig_t *config)
{
    uint8_t BleAddr[4] = {0xd6, 0xbe, 0x89, 0x8e}; // 实际使用地址

    P_ASSERT(PAN216_SetChipMode(PAN216_CHIPMODE_BLE, PAN216_ENDIAN_LITTLE, true));
    P_ASSERT(PAN216_SetChannel(config->Channel));
    P_ASSERT(PAN216_SetS2S8Mode(config->S2S8Mode));
    P_ASSERT(PAN216_SetDataRate(config->DataRate));
    P_ASSERT(PAN216_SetWorkMode(PAN216_WORKMODE_NORMAL));
    P_ASSERT(PAN216_SetAddrWidth(PAN216_WIDTH_4BYTES));
    P_ASSERT(PAN216_SetTxAddr(BleAddr, 4));
    P_ASSERT(PAN216_EnableRxPipe(PAN216_PIPE0));
    P_ASSERT(PAN216_SetRxAddr(PAN216_PIPE0, BleAddr, 4));
    P_ASSERT(PAN216_SetAckPipe(0));

    P_ASSERT(PAN216_SetRxPayloadLen(config->RxLen));
    P_ASSERT(PAN216_WriteNordicPktHeader(config->BLEHead0, config->BLEHead1, config->TxLen));
    P_ASSERT(PAN216_SetTxPayloadLen(config->TxLen));

    P_ASSERT(PAN216_SetCrcScheme(PAN216_CRC_3byte));
    P_ASSERT(PAN216_EnableDynamicPL(true));

    PAN216_WritePageRegBits(PAN216_PAGE0, WMODE_CFG1, 1, WMODE_CFG1_RX_GOON);
    P_ASSERT(PAN216_EnableWhiten(true));
    P_ASSERT(PAN216_SetWhiteInitVal(config->WhiteInit));

    if (config->BLEHeadNum == 0)
    {
        P_ASSERT(PAN216_SetNordicPktHeader(false, 0));
    }
    else
    {
        P_ASSERT(PAN216_SetNordicPktHeader(true, config->BLEHeadNum));
    }
    P_ASSERT(PAN216_SetBleWLMatchMode(config->WhiteListMatchMode));
    P_ASSERT(
        PAN216_SetBleWhitelist(config->WhiteListOffset, config->WhiteList, config->WhiteListLen));
    P_ASSERT(PAN216_SetBleLenFilter(config->LengthFilterMode));
    P_ASSERT(PAN216_EnableFifo128bytes(ENABLE));
    PAN216_SetTxPower(config->TxPower);
    PAN216_SetTxMode(PAN216_TX_MODE_SINGLE);
    PAN216_SetRxMode(PAN216_RX_MODE_CONTINOUS);

    return PAN216_OK;
}

/**
 * @brief Enter Tx Carrier Wave from STB3 mode.
 *
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_TxCarrierWave(void)
{
    PAN216_WritePageReg(PAN216_PAGE0, 0x02, 0x74);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x03, 1, BIT6);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x06, 1, BIT7);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x06, 1, BIT5);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x06, 1, BIT6);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x06, 1, BIT4);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x6A, 1, BIT1);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x6A, 1, BIT0);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x6A, 1, BIT5);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x6A, 1, BIT3);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x6B, 1, BIT7);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x6B, 1, BIT6);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x03, 1, BIT7);

    return PAN216_OK;
}

/**
 * @brief Enter Tx Carrier Wave from Deepsleep mode.
 *
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_ExitCarrierWave(void)
{
    PAN216_WritePageReg(PAN216_PAGE0, 0x6A, 0);
    PAN216_WritePageReg(PAN216_PAGE0, 0x6B, 0);
    PAN216_WritePageReg(PAN216_PAGE0, 0x06, 5);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x03, 0, BIT7);
    PAN216_WritePageRegBits(PAN216_PAGE0, 0x03, 0, BIT6);
    return PAN216_OK;
}

/*
寄存器                  寄存器地址   默认值(250kbps/other)    寄存器说明
Inner                   0x5E        0x64/0x3a               较小理想deviation值
outter                  0x5F        0x64/0x3a               较大理想deviation值
Lqi_en                  0x5C[0]     0x01                    LQI模块使能
Lqi_calculate_length    0x5D[5:0]   0d32/0d60(十进制)        LQI值计算累加时间
Lqi_offset_en           0x5C[2]     0x0                     频偏纠正计算LQI使能
*/
PAN216_Err_t PAN216_ConfigLQI(PAN216_DataRate_t DataRate)
{
    if (DataRate == PAN216_DR_250Kbps)
    {
        PAN216_WritePageReg(PAN216_PAGE1, 0x5E, 0x64);
        PAN216_WritePageReg(PAN216_PAGE1, 0x5F, 0x64);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x5C, 0x01, BIT0);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x5D, 32, BITMASK_5_0);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x5C, 0x00, BIT2);
    }
    else
    {
        PAN216_WritePageReg(PAN216_PAGE1, 0x5E, 0x3A);
        PAN216_WritePageReg(PAN216_PAGE1, 0x5F, 0x3A);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x5C, 0x01, BIT0);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x5D, 60, BITMASK_5_0);
        PAN216_WritePageRegBits(PAN216_PAGE1, 0x5C, 0x00, BIT2);
    }

    return PAN216_OK;
}

uint8_t PAN216_ReadLQI(void)
{
    return (PAN216_ReadPageReg(PAN216_PAGE1, 0x7F) & BITMASK_6_0);
}
#endif
