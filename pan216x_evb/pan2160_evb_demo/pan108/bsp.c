
#include "bsp.h"
#include "base_types.h"

volatile uint32_t g_SystemTick = 0;

void BSP_ClockInit(void)
{
    CLK_RefClkSrcConfig(CLK_SYS_SRCSEL_XTH);
    CLK_PCLK1Config(1);
    CLK_PCLK2Config(1);

    CLK_SYSCLKConfig(CLK_DPLL_REF_CLKSEL_XTH, CLK_DPLL_OUT_64M);
    CLK_RefClkSrcConfig(CLK_SYS_SRCSEL_DPLL);

    //APB Enable
    CLK_AHBPeriphClockCmd(CLK_AHBPeriph_All, ENABLE);
    CLK_APB1PeriphClockCmd(CLK_APB1Periph_All, ENABLE);
    CLK_APB2PeriphClockCmd(CLK_APB2Periph_All, ENABLE);
}

void BSP_GpioInit(void)
{
    SYS_ConfigMFP(P1_0, SYS_MFP_GPIO); // LED2
    SYS_ConfigMFP(P1_1, SYS_MFP_GPIO); // LED3

    GPIO_SetModeByPin(P1_0, GPIO_MODE_OUTPUT); // LED2
    GPIO_SetModeByPin(P1_1, GPIO_MODE_OUTPUT); // LED3
    
    P10 = 0;  // 0:off; 1:on
    P11 = 0;  // 0:off; 1:on
		 
}

void BSP_TriggerGpioInit(void)
{
    SYS_ConfigMFP(P3_0, SYS_MFP_GPIO);
    GPIO_SetModeByPin(P3_0, GPIO_MODE_OUTPUT);
}

void BSP_UartInit(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    GPIO_EnableDigitalPath(P1, BIT5);
    SYS_SET_MFP(P1, 4, UART1_TX);
    SYS_SET_MFP(P1, 5, UART1_RX);

    /* Relock protected registers */
    SYS_LockReg();
    
    UART_InitTypeDef Init_Struct;

    Init_Struct.UART_BaudRate = 1000000;
    Init_Struct.UART_LineCtrl = Uart_Line_8n1;

    /* Init UART0 for printf */
    UART_Init(UART1, &Init_Struct);
    UART_EnableFifo(UART1);
}

void BSP_Timer0Init(void)
{
    // Select Timer clock source
    CLK_SetTmrClkSrc(TIMER0, CLK_APB1_TMR0SEL_APB1CLK);

    // Set Timer work mode and expected clock frequency
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 16000000);

    // Set compare value (to 16M)
    // expect_timeout = tmrCmpValue / tmrExpCntFreq = 16M / 16MHz = 1s
    // real_timeout = tmrCmpValue / tmrRealCntFreq = 16M / tmrRealCntFreq
    TIMER_SetCmpValue(TIMER0, 16000);

    // Enable interrupt
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

    // Start Timer counting
    TIMER_Start(TIMER0);
}

void TMR0_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER0))
    {
        // Clear Timer interrupt flag
        TIMER_ClearIntFlag(TIMER0);
        g_SystemTick++;
        
//        if(g_SystemTick >= 1000)
//        {
//            g_SystemTick = 0;
//            SYS_TEST("1 second.\n");
//        }
    }
}

void BSP_KeyInit(void)
{
	GPIO_EnablePullupPath(P0, BIT1);
	GPIO_EnablePullupPath(P1, BIT6);
	GPIO_EnablePullupPath(P1, BIT7);
    GPIO_EnablePullupPath(P3, BIT1);
	GPIO_EnablePullupPath(P3, BIT0);

    SYS_ConfigMFP(P0_1, SYS_MFP_GPIO);
    SYS_ConfigMFP(P1_6, SYS_MFP_GPIO);
    SYS_ConfigMFP(P1_7, SYS_MFP_GPIO);
    SYS_ConfigMFP(P3_1, SYS_MFP_GPIO);
    SYS_ConfigMFP(P3_0, SYS_MFP_GPIO);

    GPIO_SetModeByPin(P0_1, GPIO_MODE_QUASI);
    GPIO_SetModeByPin(P1_6, GPIO_MODE_QUASI);
    GPIO_SetModeByPin(P1_7, GPIO_MODE_QUASI);
    GPIO_SetModeByPin(P3_1, GPIO_MODE_QUASI);
    GPIO_SetModeByPin(P3_0, GPIO_MODE_INPUT);

}

void BSP_OLEDInit(void)
{
    SYS_ConfigMFP(P2_2, SYS_MFP_GPIO); // OLED_DC
    SYS_ConfigMFP(P2_0, SYS_MFP_GPIO); // OLED_CS
    SYS_ConfigMFP(P2_6, SYS_MFP_GPIO); // OLED_RST
    SYS_ConfigMFP(P2_1, SYS_MFP_GPIO); // OLED_SCLK
    SYS_ConfigMFP(P2_3, SYS_MFP_GPIO); // OLED_SDIN

    GPIO_SetModeByPin(P2_2, GPIO_MODE_OUTPUT); // OLED_DC
    GPIO_SetModeByPin(P2_0, GPIO_MODE_OUTPUT); // OLED_CS
    GPIO_SetModeByPin(P2_6, GPIO_MODE_OUTPUT); // OLED_RST
    GPIO_SetModeByPin(P2_1, GPIO_MODE_OUTPUT); // OLED_SCLK
    GPIO_SetModeByPin(P2_3, GPIO_MODE_OUTPUT); // OLED_SDIN

    P22 = 0; // OLED_DC
    P20 = 0; // OLED_CS
    P26 = 0; // OLED_RST
    P21 = 0; // OLED_SCLK
    P23 = 0; // OLED_SDIN
}

void BSP_Led2Toggle(void)
{
    static bool FlagStatus = 0;

    FlagStatus = !FlagStatus;
    LED2_PIN = FlagStatus;
}

void BSP_Led3Toggle(void)
{
    static bool FlagStatus = 0;

    FlagStatus = !FlagStatus;
    LED3_PIN = FlagStatus;
}

void BSP_IICInit(void)
{
    CLK_AHBPeriphClockCmd(CLK_AHBPeriph_GPIO, ENABLE);

    SYS_ConfigMFP(P4_1, SYS_MFP_GPIO); // cs
    SYS_ConfigMFP(P4_0, SYS_MFP_GPIO); // scl/sck
    SYS_ConfigMFP(P4_3, SYS_MFP_GPIO); // sda/mosi
    SYS_ConfigMFP(P4_2, SYS_MFP_GPIO); // miso
    SYS_ConfigMFP(P4_5, SYS_MFP_GPIO); // IRQ pin

    GPIO_SetModeByPin(P4_1, GPIO_MODE_OUTPUT); // cs
    GPIO_SetModeByPin(P4_0, GPIO_MODE_OUTPUT); // scl/sck
    GPIO_SetModeByPin(P4_3, GPIO_MODE_OUTPUT); // sda/mosi
    GPIO_SetModeByPin(P4_2, GPIO_MODE_INPUT);  // miso
    GPIO_SetModeByPin(P4_5, GPIO_MODE_INPUT);  // irq
                                               // GPIO_EnablePullupPath(P4, BIT5);

    P41 = 1; // cs = 1;
    P40 = 1; // scl = 1;
    P43 = 1; // sda = 1;
}

void BSP_3LineSPIInit(void)
{
    CLK_AHBPeriphClockCmd(CLK_AHBPeriph_GPIO, ENABLE);

    SYS_ConfigMFP(P4_0, SYS_MFP_GPIO); // sck
    SYS_ConfigMFP(P4_1, SYS_MFP_GPIO); // csn
    SYS_ConfigMFP(P4_2, SYS_MFP_GPIO); // miso
    SYS_ConfigMFP(P4_3, SYS_MFP_GPIO); // mosi
    SYS_ConfigMFP(P4_5, SYS_MFP_GPIO); // irq

    GPIO_SetModeByPin(P4_1, GPIO_MODE_OUTPUT); // cs
    GPIO_SetModeByPin(P4_0, GPIO_MODE_OUTPUT); // sck
    GPIO_SetModeByPin(P4_3, GPIO_MODE_OUTPUT); // mosi
    GPIO_SetModeByPin(P4_2, GPIO_MODE_INPUT);  // miso
    GPIO_SetModeByPin(P4_5, GPIO_MODE_INPUT);  // irq

    P41 = 1; // cs = 1;
    P40 = 0; // sck = 0;
    P43 = 0; // mosi

    GPIO_EnablePullupPath(P4, BIT3);
}

void BSP_4LineSPIInit(void)
{
    CLK_AHBPeriphClockCmd(CLK_AHBPeriph_GPIO, ENABLE);

    SYS_ConfigMFP(P4_0, SYS_MFP_GPIO); // sck
    SYS_ConfigMFP(P4_1, SYS_MFP_GPIO); // csn
    SYS_ConfigMFP(P4_2, SYS_MFP_GPIO); // miso
    SYS_ConfigMFP(P4_3, SYS_MFP_GPIO); // mosi
    SYS_ConfigMFP(P4_5, SYS_MFP_GPIO); // irq

    GPIO_SetModeByPin(P4_1, GPIO_MODE_OUTPUT); // cs
    GPIO_SetModeByPin(P4_0, GPIO_MODE_OUTPUT); // sck
    GPIO_SetModeByPin(P4_3, GPIO_MODE_OUTPUT); // mosi
    GPIO_SetModeByPin(P4_2, GPIO_MODE_INPUT);  // miso
    GPIO_SetModeByPin(P4_5, GPIO_MODE_INPUT);  // irq

    P41 = 1; // cs = 1;
    P40 = 0; // sck = 0;
    P43 = 0; // mosi = 0;
}

void SYS_ConfigMFP(uint8_t PinID, uint32_t PinFunction)
{
    uint8_t port = PinID / 0x10;
    uint8_t bit = PinID % 0x10;

    (&SYS->P0_MFP)[port] = ((&SYS->P0_MFP)[port] & ~(SYS_MFP_P00_Msk << bit)) | PinFunction;
}

/* GPIO Driver Wrapper Functions */
void GPIO_SetModeByPin(uint8_t PinID, GPIO_ModeDef Mode)
{
    uint8_t port = PinID / 0x10;
    uint8_t bit = PinID % 0x10;

    GPIO_SetMode((GPIO_T*)(P0_BASE + 0x40 * port), BIT0 << bit, Mode);

    if (Mode == GPIO_MODE_INPUT || Mode == GPIO_MODE_QUASI)
    {
        if (PinID == P5_6 || PinID == P4_6 || PinID == P4_7)
        {
            // These Pins' DINOFF bits should be synced to 3v area
            CLK_Wait3vSyncReady();
        }
    }
}

#pragma O0 // disable optimization, otherwise the delay time will be inaccurate

/**
 * @brief BSP_DelayMs
 * @param[in] <time> ms
 * @return none
 * @note user should modify this function according to the actual situation
 */
void BSP_DelayMs(const uint32_t ms)
{
    int i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 8000; j++)
        {
            __NOP();
        }
    }
}

/**
 * @brief BSP_DelayUs
 * @param[in] <time> us
 * @return none
 * @note user should modify this function according to the actual situation
 */
void BSP_DelayUs(const uint32_t us)
{
    int i, j;

    for (i = 0; i < us; i++)
    {
        for (j = 0; j < 10; j++)
        {
            __NOP();
        }
    }
}
