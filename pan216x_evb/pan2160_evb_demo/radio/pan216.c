#include <stdbool.h>
#include "pan216.h"

uint8_t Factory_Read(uint8_t addr)
{
    uint8_t val;
    PAN216_SetPage(1);
    PAN216_WriteRegUnchecked(0x04, addr << 1);
    val = PAN216_ReadReg(0x04);
    PAN216_SetPage(0);
    Pan216_Funs.delayus(100);
    PAN216_WriteRegUnchecked(0x73, 0x01);

    return val;
}

PAN216_Err_t PAN216_FactoryCalibration(void)
{
    uint8_t val[6] = {0};

    PAN216_WriteRegUnchecked(0x73, 0x01);
    val[2] = Factory_Read(0x02);
    val[5] = Factory_Read(0x05);
if ((val[5] >> 6) != 2 || (val[2] & 0x0F) != 5)
    {
        PAN216_SetPage(1);
        PAN216_WriteReg(0x05, 0x01);
        PAN216_SetPage(0);
        return PAN216_ERR;
    }

    val[1] = Factory_Read(0x01);
    val[3] = Factory_Read(0x03);
    val[4] = Factory_Read(0x04);

    PAN216_SetPage(1);
    /* Recover cons_reg_en here */
    PAN216_WriteReg(0x05, 0x01);
    /* LDO_ANA_TRIM: 0x01[7:4]->P1 0x48[7:4] */
    /* LDO_RFFE_TRIM: 0x01[4:0]->P1 0x48[4:0] */
    PAN216_WriteReg(0x48, val[1]);
    /* IPOLY_TRIM: 0x02[7:5]->P1 0x47[6:4] */
    PAN216_WriteRegBits(0x47, (val[2] >> 5) & 0x07, BITMASK_6_4);
    /* LDO_HP_TRIM[3:0]: 0x03[3:0] -> P1 0x4C[3:0] */
    PAN216_WriteRegBits(0x4c, val[3] & 0x0f, BITMASK_4_0);
    /* FSYNVCO[4]*/
    if ((val[2] & BIT4) == 0)
    {
        PAN216_WriteReg(0x43, 0x11);
    }
    else
    {
        PAN216_WriteReg(0x43, 0x10);
    }
    PAN216_SetPage(0);
    /* VBG_TRIM_3V: 0x04[7:4]->P0 0x05[3:0] */
    /* LP_VREF_TRIM[3:0]: 0x03[7:4] -> P0 0x05[7:4] */
    PAN216_WriteReg(0x05, (val[3] & 0xF0) | (val[4] >> 4) & 0x0F);
    return PAN216_OK;
}

/**
 * @brief Initialize the PAN216 transceiver from Deep Sleep Mode to STB3 Mode.
 *
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_Init(void)
{
    PAN216_Err_t ret = PAN216_OK;
    const uint8_t gPAN216Page0Table[][2] = {PAN216_PAGE0TABLE};
    const uint8_t gPAN216Page1Table[][2] = {PAN216_PAGE1TABLE};

    /* Set Page 0 */
    PAN216_WriteRegUnchecked(0x00, 0x00);

    /* Initialize the SPI/IIC interface */
#if (INTERFACE_MODE == USE_SPI_3LINE)
    PAN216_WriteRegUnchecked(SPI_CFG, 0x83);
    PAN216_WriteRegUnchecked(SYS_CFG, 0x02);
#elif (INTERFACE_MODE == USE_I2C)
    /* Reset IIC in case of SDA of PAN216 is stuck at low */
    Pan216_Funs.i2c_reset();
#endif
#if INTERFACE_MODE == USE_SPI_4LINE
    PAN216_WriteRegUnchecked(SPI_CFG, 0x03);
    PAN216_WriteRegUnchecked(SYS_CFG, 0x03);
#endif
    /* Enter STB1 Mode */
    P_ASSERT(PAN216_WriteReg(0x02, 0x02));

    /* Turn on EN_LS, POR_NRSTL, iso_to_0*/
    P_ASSERT(PAN216_WriteReg(0x02, 0x72));

    /* Wait for LDO to be stable*/
    Pan216_Funs.delayus(200);

#if (XTAL_FREQ == XTAL_FREQ_16M)
    /* Set 16M crystal */
    PAN216_WriteReg(0x37, 0xE0); // This register is also configured in PAN216_SetDataRate()
    PAN216_SetPage(1);
    // PAN216_WriteReg(0x3f, 0xd4); // bit3:0 -> 4 default
    // PAN216_WriteReg(0x40, 0x28); // bit3 -> 1 default
    PAN216_WriteReg(0x41, 0xA6); // bit2 -> 1
#else
    /* Set 32M crystal by default */
    PAN216_SetPage(1);
    PAN216_WriteReg(0x3f, 0xD2);
    PAN216_WriteReg(0x40, 0x20);
    PAN216_WriteReg(0x41, 0xA2);
#endif

    /* Enter STB2 Mode */
    P_ASSERT(PAN216_WriteReg(0x02, 0x73));

    /* Wait for FSYNXO_CLKRDY, then turn off FSYNXO_STARTUP_FAST */
    while ((PAN216_ReadReg(0x6f) & BIT7) != BIT7)
        ;
    PAN216_WriteReg(0x4c, 0x68);
    
    /* Reset cons_reg_en, recover it later in PAN216_FactoryCalibration() */
    PAN216_WriteReg(0x05, 0x00);

    /* Enter STB3 Mode */
    P_ASSERT(PAN216_WriteReg(0x02, 0x74));

    /* Write preconfigured registers on Page 1 */
    for (int i = 0; i < (sizeof(gPAN216Page1Table) / sizeof(gPAN216Page1Table[0])); i++)
    {
        PAN216_WriteReg(gPAN216Page1Table[i][0], gPAN216Page1Table[i][1]);
    }

    PAN216_SetPage(0);
    /* Write preconfigured registers on Page 0 */
    for (int i = 0; i < (sizeof(gPAN216Page0Table) / sizeof(gPAN216Page0Table[0])); i++)
    {
        PAN216_WriteReg(gPAN216Page0Table[i][0], gPAN216Page0Table[i][1]);
    }

    P_ASSERT(PAN216_FactoryCalibration());

#if EASY_RF
    PAN216_Calibration();
    PAN216_FinishCalibration();
    if (EASY_RF_TxPower == PAN216_TXPWR_0dBm_LOWPWR || EASY_RF_TxPower == PAN216_TXPWR_n4dBm)
    {
        PAN216_SetPage(1);
        uint8_t tmp = PAN216_ReadReg(0x48);
        PAN216_WriteReg(0x48, (tmp & 0xF0) | 0x0C);
        PAN216_SetPage(0);
    }
#else
    /* If EASY_RF is not defined, the user should call PAN216_SetUpConfig() to set the
     * configuration. PAN216_Calibration() will be called inside PAN216_SetUpConfig() to make
     * sure PAN216 works properly. Refer to PAN216_SetDataRate() for more details.
     */
#endif
    return ret;
}



/**
 * @brief This function is used to calibrate the PAN216 transceiver after the ChipMode and DataRate
 * are set. The calibration process includes VCO, TP, BW, and DCOC calibration.
 * @note It should be called every time the transceiver is powered up from deep sleep mode. Going to
 *      Sleep Mode and waking up does not require calibration.
 */
void PAN216_Calibration(void)
{
    /* Set Page 1 */
    PAN216_WriteReg(0x00, 0x01);
    /* Start VCO calibration */
    PAN216_WriteReg(0x1b, BIT3);
    /* Wait for VCO calibration to complete, 30 (32M Crystal)/ 48 (16M Crystal) us minimum */
    Pan216_Funs.delayus(300);
    // while (((PAN216_ReadReg(0x70) & BIT6) != BIT6))
    //     ;
    /* Start TP calibration */
    PAN216_WriteReg(0x1b, BIT4);
    /* Wait for TP calibration to complete, 44 ms minimum */
    Pan216_Funs.delayms(55);
    /* Enter RX mode */
    PAN216_WriteReg(0x02, 0x76);
    Pan216_Funs.delayus(200);
    /* Start Offset calibration */
    PAN216_WriteReg(0x1b, BIT5);
    /* Wait for Offset calibration to complete, 370 (32M Crystal)/ 590 (16M Crystal) us minimum */
    Pan216_Funs.delayus(700);
    // while (((PAN216_ReadReg(0x7f) & BIT7) != BIT7));
    /* Start BW calibration */
    PAN216_WriteReg(0x1b, BIT6);
    /* Wait for BW calibration to complete, 340 us minimum */
    Pan216_Funs.delayus(400);
    // while (((PAN216_ReadReg(0x6d) & BIT7) != BIT7))
    //     ;
    /* Start DCOC calibration */
    PAN216_WriteReg(0x1b, BIT7);
    /* Wait for DCOC calibration to complete, 1030 us minimum */
    Pan216_Funs.delayus(1100);
    // while (((PAN216_ReadReg(0x7f) & BIT7) != BIT7))
    //  ;
}

void PAN216_FinishCalibration(void)
{
    /* Stop DCOC calibration */
    PAN216_WriteReg(0x1b, 0x00);

    /* Enter STB3 mode */
    PAN216_WriteReg(0x02, 0x74);
    /* Set Page 0 */
    PAN216_WriteReg(0x00, 0x00);
}

/**
 * @brief Enter Sleep Mode from Standby Mode
 *
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_EnterSleep(void)
{
    P_ASSERT(PAN216_WriteReg(0x02, 0x74));
#if INTERFACE_MODE == USE_I2C
    P_ASSERT(PAN216_WriteReg(0x02, 0x71));
#else
    P_ASSERT(PAN216_WriteReg(0x02, 0x21));
#endif

    return PAN216_OK;
}

/**
 * @brief Exit Sleep Mode and enter Standby Mode
 *
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_ExitSleep(void)
{
    P_ASSERT(PAN216_WriteReg(0x02, 0x22)); // stb1
    P_ASSERT(PAN216_WriteReg(0x02, 0x74)); // stb3
    Pan216_Funs.delayus(500);              // waiting for CLKRDY, 440us(32M)

    return PAN216_OK;
}

/**
 * @brief Exit the TX state and enter standby mode. Make sure IRQ is cleared before calling this
 * function
 *
 * @param None
 *
 * @return Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_ExitTxMode(void)
{
    P_ASSERT(PAN216_WriteReg(0x02, 0x74));

    return PAN216_OK;
}

/**
 * @brief Exit the RX mode and enter standby mode. Make sure IRQ is cleared before calling this
 * function
 *
 * @param None
 *
 * @return Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_ExitRxMode(void)
{
    P_ASSERT(PAN216_WriteReg(0x02, 0x74));

    return PAN216_OK;
}

/**
 * @brief Prepares the PAN216 transceiver to enter TX or RX mode.
 *
 * This function configures the transceiver to switch between TX and RX modes. It sets the
 * appropriate registers to ensure the transceiver is ready for transmission or reception.
 * If the transceiver is already in the desired mode, this function does nothing.
 *
 * @param mode The desired mode to set, either PAN216_TX or PAN216_RX.
 *
 * @return PAN216_Err_t Result of the operation
 *         - PAN216_OK: Operation was successful
 *         - PAN216_ERR: Operation failed
 */
PAN216_Err_t PAN216_PrepareTRxWithAck(PAN216_TRXMode_t Mode, bool EnAck)
{
    if (EnAck == true)
    {
        /* Uses Tx confiruration if ACK is enabled */
        Mode = PAN216_TX;
    }

    if (Mode == PAN216_TX)
    {
        PAN216_WriteReg(0x00, 0x01);
        PAN216_WriteReg(0x42, 0xa4);
        PAN216_WriteReg(0x00, 0x00);
    }
    else if (Mode == PAN216_RX)
    {
        PAN216_WriteReg(0x00, 0x01);
        PAN216_WriteReg(0x42, 0xa0);
        PAN216_WriteReg(0x00, 0x00);
    }

    return PAN216_OK;
}

/**
 * @brief Exit the RX mode and enter standby mode. Make sure IRQ is cleared before calling this
 * function
 *
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_TxStart(void)
{
    P_ASSERT(PAN216_WriteReg(0x02, 0x74));
    P_ASSERT(PAN216_WriteReg(0x02, 0x75));

    return PAN216_OK;
}

/**
 * @brief Exit the RX mode and enter standby mode. Make sure IRQ is cleared before calling this
 function

 *
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_RxStart(void)
{
    P_ASSERT(PAN216_WriteReg(0x02, 0x74));
    P_ASSERT(PAN216_WriteReg(0x02, 0x76));

    return PAN216_OK;
}

/**
 * @brief Sets the frequency channel of the PAN216 transceiver
 *
 * @param Channel The desired radio frequency channel, as a value from 0 to 83
 *
 * @note The actual frequency will be (2400 + Channel)MHz
 * @note The PLOS_CNT[7:4] bits of the OBSERVER_TX register will be reset
 *
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetChannel(uint8_t Channel)
{
    return PAN216_WriteReg(REG_P0_0X39, Channel);
}

/**
 * @brief Sets the address width of the PAN216 transceiver
 *
 * This function is used to set the RX/TX address field width of the PAN216 transceiver.
 * The address width is specified as a value between 3 and 5.
 *
 * @param AddrWidth The desired address width, as a value from 3 to 5
 *          - 00 2bytes
 *          - 01 3bytes
 *          - 10 4bytes
 *          - 11 5bytes
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetAddrWidth(PAN216_AddrWidth_t AddrWidth)
{
    P_ASSERT(PAN216_WriteRegBits(WMODE_CFG1, AddrWidth - 2, WMODE_CFG1_ADDR_BYTE_LENGTH));

    return PAN216_OK;
}

/**
 * @brief Set static RX address for a specified pipe
 *
 * @param Pipe - pipe to configure address, one of PAN216_Pipe_t values
 * @param Addr - pointer to the buffer with address
 * @param len - length of address
 * @note Pipe can be a number from 0 to 5 (RX pipes)
 * @note Buffer length must be equal to current address width of transceiver
 * @note For pipes[2..5] only the first byte of address will be written because other bytes of
 * address equals to pipe1
 * @note For pipes[2..5] only the first byte of address will be written because pipes 1-5 share the
 * four most significant address bytes
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetRxAddr(PAN216_Pipe_t Pipe, uint8_t *Addr, uint8_t len)
{

    switch (Pipe)
    {
    case PAN216_PIPE0:
        P_ASSERT(PAN216_WriteRegs(PIPE0_RXADDR0_CFG, Addr, len));
        break;
    case PAN216_PIPE1:
        P_ASSERT(PAN216_WriteRegs(PIPE1_RXADDR0_CFG, Addr, len));
        break;
    case PAN216_PIPE2:
        P_ASSERT(PAN216_WriteReg(PIPE2_RXADDR0_CFG, Addr[0]));
        break;
    case PAN216_PIPE3:
        P_ASSERT(PAN216_WriteReg(PIPE3_RXADDR0_CFG, Addr[0]));
        break;
    case PAN216_PIPE4:
        P_ASSERT(PAN216_WriteReg(PIPE4_RXADDR0_CFG, Addr[0]));
        break;
    case PAN216_PIPE5:
        P_ASSERT(PAN216_WriteReg(PIPE5_RXADDR0_CFG, Addr[0]));
        break;
    default:
        return PAN216_ERR;
    }

    return PAN216_OK;
}

/**
 * @brief Set static TX address for the transceiver
 *
 * @param Addr Pointer to the buffer with address
 * @param AddrWidth TX address field width, value from 3 to 5
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_SetTxAddr(uint8_t *Addr, uint8_t len)
{
    return PAN216_WriteRegs(TXADDR0_CFG, Addr, len);
}

/**
 * @brief Get the pipe number for the payload available for reading from RX FIFO
 *
 * @return uint8_t Pipe number or 0xFF if the RX FIFO is empty
 *         - 0x00: Pipe 0
 *         - 0x01: Pipe 1
 *         - 0x02: Pipe 2
 *         - 0x03: Pipe 3
 *         - 0x04: Pipe 4
 *         - 0x05: Pipe 5
 * @return uint8_t Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Pipe_t PAN216_GetRxPipeNum(void)
{
    uint8_t pipeNum = (PAN216_ReadReg(STATUS0) >> 4) & 0x07;

    return (PAN216_Pipe_t)pipeNum;
}

/**
 * @brief Get pending IRQ flags
 *
 * @return Current status of RX_DONE, TX_DONE, RX_TIMEOUT, and MAX_RT bits of the STATUS register
 */
uint8_t PAN216_GetIRQFlags(void)
{
    return PAN216_ReadReg(RFIRQFLG);
}

/**
 * @brief Clears any pending IRQ flags of the PAN216 transceiver
 *
 * This function clears any pending IRQ flags of the PAN216 transceiver.
 *         - RF_IT_TX_IRQ
 *         - RF_IT_MAX_RT_IRQ
 *         - RF_IT_ADDR_ERR_IRQ
 *         - RF_IT_CRC_ERR_IRQ
 *         - RF_IT_LEN_ERR_IRQ
 *         - RF_IT_PID_ERR_IRQ
 *         - RF_IT_RX_TIMEOUT_IRQ
 *         - RF_IT_RX_IRQ
 * @return Result of the operation
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_ClearIRQFlags(uint8_t flags)
{
    return PAN216_WriteRegUnchecked(RFIRQFLG, flags);
}

/**
 * @brief  This function is used to enable or disable RF interrupts
 * @param  NewState: which can be ENABLE or DISABLE
 * @param  RF_IT: Which can be
 *         - RF_IT_TX_IRQ
 *         - RF_IT_MAX_RT_IRQ
 *         - RF_IT_ADDR_ERR_IRQ
 *         - RF_IT_CRC_ERR_IRQ
 *         - RF_IT_LEN_ERR_IRQ
 *         - RF_IT_PID_ERR_IRQ
 *         - RF_IT_RX_TIMEOUT_IRQ
 *         - RF_IT_RX_IRQ
 * @return Result of the operation:
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_ConfigIT(uint8_t RF_IT)
{
    return PAN216_WriteReg(RFIRQ_CFG, ~RF_IT);
}

PAN216_Err_t PAN216_EnableInterfaceMuxIRQ(bool NewState)
{
    if (NewState)
    {
#if INTERFACE_MODE == USE_SPI_3LINE
        // P_ASSERT(PAN216_WriteReg(SYS_CFG, 0x06));
        // P_ASSERT(PAN216_WriteReg(LP_CFG, 0x05));
        P_ASSERT(PAN216_SetRegBits(PAN216_PAGE0, SYS_CFG, SYS_CFG_IRQ_MOSI_MUX_EN));
        P_ASSERT(PAN216_ResetRegBits(PAN216_PAGE0, LP_CFG, LP_CFG_IRQ_I2C_MUX_EN));
#elif INTERFACE_MODE == USE_I2C
        P_ASSERT(PAN216_WriteReg(SYS_CFG, 0x03));
        P_ASSERT(PAN216_WriteReg(LP_CFG, 0x0D));
#endif
    }
    else
    {
#if INTERFACE_MODE == USE_SPI_3LINE
        P_ASSERT(PAN216_WriteReg(SYS_CFG, 0x03));
        P_ASSERT(PAN216_WriteReg(LP_CFG, 0x05));
#elif INTERFACE_MODE == USE_I2C
        P_ASSERT(PAN216_WriteReg(SYS_CFG, 0x03));
        P_ASSERT(PAN216_WriteReg(LP_CFG, 0x05));
#endif
    }

    return PAN216_OK;
}
/**
 * @brief Writes TX payload.
 *
 * @param pBuf Pointer to the buffer with payload data.
 * @param Length: Payload length in bytes.
 *
 * @return Result of the operation:
 *         - PAN216_OK
 *         - PAN216_ERR
 */
PAN216_Err_t PAN216_WritePayload(uint8_t *pBuf, uint8_t Length)
{
    P_ASSERT(PAN216_WriteFifo(TRX_FIFO, pBuf, Length));
    return PAN216_OK;
}

PAN216_Err_t PAN216_SendPacket(uint8_t *pBuf, uint8_t Length)
{
    P_ASSERT(PAN216_WritePayload(pBuf, Length));
    P_ASSERT(PAN216_TxStart());
    return PAN216_OK;
}

PAN216_Err_t PAN216_SendPacketContinuous(uint8_t *pBuf, uint8_t Length)
{
    P_ASSERT(PAN216_WritePayload(pBuf, Length));

    // if tx_en is 0, trigger tx
    if ((PAN216_ReadReg(STATE_CFG) & STATE_CFG_REG_OPERATE_MODE) != STATE_CMD_TX)
    {
        PAN216_TxStart();
    }
    P_ASSERT(PAN216_WriteReg(STATE_CFG, 0x75));
    P_ASSERT(PAN216_WriteReg(STATE_CFG, 0xf5));

    return PAN216_OK;
}

/**
 * @brief This function reads the length of the payload
 *
 * @return the length of the payload
 */
uint8_t PAN216_GetRecvLen(void)
{
    return PAN216_ReadReg(STATUS3);
}

PAN216_Err_t PAN216_ReadNordicPktHeader(uint8_t *Header0, uint8_t *Header1, uint8_t *Length)
{
    if (Header0 != NULL)
    {
        *Header0 = PAN216_ReadReg(STATUS1);
    }

    if (Header1 != NULL)
    {
        *Header1 = PAN216_ReadReg(STATUS2);
    }

    if (Length != NULL)
    {
        *Length = PAN216_ReadReg(STATUS3);
    }

    return PAN216_OK;
}

PAN216_Err_t PAN216_SetWhiteInitVal(uint8_t val)
{
    return PAN216_WriteRegBits(SCR_CFG, val, SCR_CFG_SCR_INI);
}

/**
 * @brief Get the current RSSI code
 *
 * @param None
 *
 * @return RSSI code
 */
u16 PAN216_GetRssiCode(void)
{
    u16 CurrCode;
    int codedBm;
    u8 RssiCode[2];
    float th1 = PAN216_ReadReg(0x55) - 256 + 7.5f;
    float th2 = PAN216_ReadReg(0x56) - 256 + 7.5f;
    float th3 = PAN216_ReadReg(0x57) - 256 + 7.5f;
    static u16 lastCode = 0;
    P_ASSERT(PAN216_ReadRegs(0x7A, RssiCode, 2));
    CurrCode = ((RssiCode[0] + ((u16)RssiCode[1] << 8)) & 0x3FFF); // get origin packet rssi code
    codedBm = (int)(CurrCode - 16384) / 4;
    u8 index = ((PAN216_ReadReg(0x79) & (BIT4 | BIT5)) >> 4);
    if ((codedBm >= th3 && index == 3) || (codedBm >= th2 && index == 2) || (codedBm >= th1 &&
    index == 1))
    {
        return lastCode;
    }

   lastCode = CurrCode;

   return CurrCode;
}

/**
 * @brief Get the Packet RSSI code in dBm.
 *
 * @param None
 *
 * @return dBm
 */
int PAN216_GetRssiDBm(void)
{
    return (int)(PAN216_GetRssiCode() - 16384) / 4;
}

/**
 * @brief Get Real Time RSSI in dBm.
 *
 * @param None
 *
 * @return RSSI in dBm
 */
int PAN216_GetRealTimeRSSI(void)
{
    uint8_t RssiCode[2];
    uint8_t tmp = PAN216_ReadReg(0x46);
    tmp = tmp & BITMASK_5_0;

    /* Lock up */
    P_ASSERT(PAN216_WriteReg(0x46, tmp));
    P_ASSERT(PAN216_WriteReg(0x46, tmp | BIT7));
    P_ASSERT(PAN216_WriteReg(0x46, tmp | BIT7 | BIT6));
    /* Read RSSI Code */
    P_ASSERT(PAN216_ReadRegs(0x7E, RssiCode, 2));
    int rssi = (int)(((RssiCode[0] + ((uint16_t)RssiCode[1] << 8)) & 0x3FFF) - 16384) / 4;
    P_ASSERT(PAN216_WriteReg(0x46, tmp));
    return rssi;
}
