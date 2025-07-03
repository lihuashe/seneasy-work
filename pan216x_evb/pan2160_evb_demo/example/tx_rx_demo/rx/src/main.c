#include "PanSeries.h"
#include "bsp.h"
#include "pan216_ext.h"
#include "pan_gpio.h"

static int count = 0;
u8 RxBuf[46] = {0};

#if (!EASY_RF)
RFConfig_t gRfConfig = {
    .Channel = 78,
    .TxPower = PAN216_TXPWR_0dBm,
    .DataRate = PAN216_DR_1Mbps,
    .ChipMode = PAN216_CHIPMODE_FS01,
    .EnWhite = false,
    .Crc = PAN216_CRC_off,
    .TxLen = 42,
    .RxLen = 42,
    .TxAddrWidth = PAN216_WIDTH_4BYTES,
    .TxAddr = {0xCC, 0xCC, 0xCC, 0xCC,0xCC},
    .RxAddrWidth = PAN216_WIDTH_4BYTES,
    .RxAddr =
        {
            {true, {0xCC, 0xCC, 0xCC, 0xCC, 0xCC}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
        },

    // Do not edit the following fields
    .WorkMode = PAN216_WORKMODE_NORMAL,
    .EnDPL = false,
    .EnTxNoAck = true,
    .crcSkipAddr = false,
    .Endian = PAN216_ENDIAN_BIG,
    .EnRxPlLenLimit = false,
    .EnManuPid = false,
    .TRxDelayTimeUs = 0,
    .RxTimeoutUs = 30000,
    .AutoDelayUs = 0,
    .AutoMaxCnt = 0,
    .TxMode = PAN216_TX_MODE_SINGLE,
    .RxMode = PAN216_RX_MODE_CONTINOUS,
};
#endif

int main(void)
{
    BSP_ClockInit();
    BSP_UartInit();
    BSP_GpioInit();
    SYS_TEST("CPU @ %dHz\n", SystemCoreClock);

#if (INTERFACE_MODE == USE_SPI_3LINE)
    SYS_TEST("SPI-3LINE\n");
#elif (INTERFACE_MODE == USE_I2C)
    SYS_TEST("IIC\n");
#endif
#if INTERFACE_MODE == USE_SPI_4LINE
    SYS_TEST("SPI-4LINE\n");
#endif

    RF_BusInit();
    PAN216_Err_t ret = PAN216_Init();
    if (ret != PAN216_OK)
        goto err_init;
#if (!EASY_RF)
    ret = PAN216_SetUpConfig(&gRfConfig);
    if (ret != PAN216_OK)
        goto err_setup;
#endif

    PAN216_EnableInterfaceMuxIRQ(true);
    PAN216_ConfigIT(RF_IT_RX_IRQ | RF_IT_CRC_ERR_IRQ);
    PAN216_ClearIRQFlags(0xFF);
    PAN216_PrepareTRxWithAck(PAN216_RX, false);
    PAN216_RxStart();

    while (1)
    {
        while (!IRQ_DETECTED)
            ;
        u8 flag = PAN216_GetIRQFlags();

        if (flag & RF_IT_RX_IRQ)
        {
            PAN216_ReadFifo(TRX_FIFO, RxBuf, 46);
            PAN216_ClearIRQFlags(RF_IT_RX_IRQ);
            printf("Rx[%d]:\r\n", ++count);
            printHex(RxBuf, 46);
        }
        else if (flag == RF_IT_CRC_ERR_IRQ)
        {
            PAN216_ClearIRQFlags(flag);
            printf(">> RF_IT_CRC_ERR_IRQ%d\r\n", flag);
        }
    }
err_init:
    printf("PAN216_Init failed.\r\n");
    while (1)
        ;
#if (!EASY_RF)
err_setup:
    printf("PAN216_SetUpConfig failed.\r\n");
    while (1)
        ;
#endif
}
