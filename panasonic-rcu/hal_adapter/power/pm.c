/**
 * @file mp.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief
 * @version 0.1
 * @date 2025-03-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "pm.h"
#include "key_driver.h"
#include "key_evt_handle.h"
#include "py32f0xx_hal.h"
#include "led_driver.h"
#include "pan216.h"
#include "time_utils.h"

extern void APP_ErrorHandler(void);
extern void HAL_MspInit(void);
extern void HAL_MspDeInit(void);

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
ADC_HandleTypeDef AdcHandle;
ADC_ChannelConfTypeDef sConfig;

#define LPTIM_WAKEUP_TMS 5000 // use LSI div 32
#define IWDG_TIME_MS 6000 / 2 // use LSI div 64
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#if PM_IWDG_EN
IWDG_HandleTypeDef IwdgHandle;
LPTIM_HandleTypeDef LPTIMConf;
#endif
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
#if PM_IWDG_EN
/**
 * @brief  This function handles LPTIM1 interrupt request.
 */
void LPTIM1_IRQHandler(void)
{
    HAL_IWDG_Refresh(&IwdgHandle);
    HAL_LPTIM_IRQHandler(&LPTIMConf);
}

static void lptim_init()
{
    RCC_PeriphCLKInitTypeDef LPTIM_RCC = {0};

    /* LPTIM clock configuration */
    LPTIM_RCC.PeriphClockSelection = RCC_PERIPHCLK_LPTIM;   /* Select peripheral clock: LPTIM */
    LPTIM_RCC.LptimClockSelection = RCC_LPTIMCLKSOURCE_LSI; /* Select LPTIM clock source: LSI */
    /* Peripheral clock initialization */
    if (HAL_RCCEx_PeriphCLKConfig(&LPTIM_RCC) != HAL_OK)
    {
        APP_ErrorHandler();
    }

    /* Enable LPTIM clock */
    __HAL_RCC_LPTIM_CLK_ENABLE();

    /* LPTIM configuration */
    LPTIMConf.Instance = LPTIM;                         /* LPTIM */
    LPTIMConf.Init.Prescaler = LPTIM_PRESCALER_DIV32;   /* Prescaler: 32 */
    LPTIMConf.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE; /* Immediate update mode */
    /* Initialize LPTIM */
    if (HAL_LPTIM_Init(&LPTIMConf) != HAL_OK)
    {
        APP_ErrorHandler();
    }
    /* Set interrupt priority */
    HAL_NVIC_SetPriority(LPTIM1_IRQn, 1, 0);
    /* Enable TIM1 interrupt */
    HAL_NVIC_EnableIRQ(LPTIM1_IRQn);
}

static void lptim_start(void)
{
    /* Enable autoreload interrupt */
    __HAL_LPTIM_ENABLE_IT(&LPTIMConf, LPTIM_IT_ARRM);

    /* Enable LPTIM */
    __HAL_LPTIM_ENABLE(&LPTIMConf);

    /* Load autoreload value */
    __HAL_LPTIM_AUTORELOAD_SET(&LPTIMConf, LPTIM_WAKEUP_TMS);

    /* Delay 120us */
    delay_us(120);

    /* Start single count mode */
    __HAL_LPTIM_START_SINGLE(&LPTIMConf);
}

static void lptim_stop(void)
{
    /* Disable LPTIM */
    __HAL_LPTIM_DISABLE(&LPTIMConf);
}
#endif

/**
 * @brief  System Clock Configuration
 * @param  None
 * @retval None
 */
static void pm_systemClockConfig(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    /* Oscillator Configuration */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE; /* Select oscillators HSE, HSI, LSI, LSE */
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;                                                                                              /* Enable HSI */
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                                                                                              /* HSI not divided */
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;                                                                     /* Configure HSI clock as 8MHz */
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                                                                                             /* Disable HSE */
    RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;  /* Disable LSI */
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF; /* Disable LSE */
    /*RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM;*/
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF; /* Disable PLL */
    // RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE; /* Select HSE as PLL source */
    /* Configure oscillators */
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        APP_ErrorHandler();
    }
}

static void pm_LSIConfig(uint32_t LSIState)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    /* System clock configuration */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI; /* Select configure LSI */
    RCC_OscInitStruct.LSIState = LSIState;                     /* Enable LSI */
    /* Config LSI */
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        APP_ErrorHandler();
    }
}

static void pm_entry_sleep_before()
{
    led_driver_sleep();
    PAN216_EnterSleep();
#if 0
    // 切换时钟为内部高速时钟
    pm_set_sysclock(RCC_SYSCLKSOURCE_HSI);
#endif
    /* Suspend SysTick */
    HAL_SuspendTick();
}

static void pm_wakeup_from_sleep()
{
    HAL_ResumeTick();
    led_driver_wakeup();
    pm_systemClockConfig();
    PAN216_ExitSleep();
#if 0
    pm_set_sysclock(RCC_SYSCLKSOURCE_HSI);
#endif
}

/**
 * @brief  ADC configuration function
 * @param  None
 * @retval None
 */
static void adc_config(void)
{
    __HAL_RCC_ADC_FORCE_RESET();
    __HAL_RCC_ADC_RELEASE_RESET(); /* Reset ADC */
    __HAL_RCC_ADC_CLK_ENABLE();    /* Enable ADC clock */

    AdcHandle.Instance = ADC1;

    /* ADC calibration */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
    {
        APP_ErrorHandler();
    }

    AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;            /* Set ADC clock */
    AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;                      /* 12-bit resolution for converted data */
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                      /* Right-alignment for converted data */
    AdcHandle.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;            /* Scan sequence direction: forward */
    AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* Single sampling */
    AdcHandle.Init.LowPowerAutoWait = ENABLE;                            /* Enable wait for conversion mode*/
    AdcHandle.Init.ContinuousConvMode = DISABLE;                         /* Single conversion mode */
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;                      /* Disable discontinuous mode */
    AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* Software triggering */
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* No external trigger edge */
    AdcHandle.Init.DMAContinuousRequests = DISABLE;                      /* Disable DMA */
    AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* When an overload occurs, overwrite the previous value  */
    AdcHandle.Init.SamplingTimeCommon = ADC_SAMPLETIME_239CYCLES_5;      /* Set sampling time */

    /* ADC initialization */
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        APP_ErrorHandler();
    }

    sConfig.Channel = ADC_CHANNEL_VREFINT;  /* ADC channel selection */
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER; /* Set the rank for the ADC channel order */

    /* Configure ADC channels */
    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        APP_ErrorHandler();
    }
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
void pm_init()
{
#if __DEBUG__ >= __DEBUG_INFO__
    my_log(">I%d[%s] \n", __LINE__, __FUNCTION__);
#endif
    pm_systemClockConfig();
    pm_set_sysclock(RCC_SYSCLKSOURCE_HSI);
    PWR_StopModeConfigTypeDef PwrStopModeConf = {0};

    /* VCORE = 1.0V  when enter stop mode */
    PwrStopModeConf.LPVoltSelection = PWR_STOPMOD_LPR_VOLT_SCALE2;
    PwrStopModeConf.FlashDelay = PWR_WAKEUP_FLASH_DELAY_5US;
    PwrStopModeConf.WakeUpHsiEnableTime = PWR_WAKEUP_HSIEN_AFTER_MR;
    PwrStopModeConf.RegulatorSwitchDelay = PWR_WAKEUP_LPR_TO_MR_DELAY_2US;
    PwrStopModeConf.SramRetentionVolt = PWR_SRAM_RETENTION_VOLT_VOS;
    HAL_PWR_ConfigStopMode(&PwrStopModeConf);
#if PM_IWDG_EN
    lptim_init();

    IwdgHandle.Instance = IWDG; /* Select IWDG */
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_64;
    IwdgHandle.Init.Reload = (IWDG_TIME_MS);
    HAL_IWDG_Init(&IwdgHandle);
#endif

    adc_config();
}

void pm_set_sysclock(uint32_t clk_source)
{
    HAL_SuspendTick();
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    /* Clock source configuration */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; /* Select clock types HCLK, SYSCLK, PCLK1 */
    RCC_ClkInitStruct.SYSCLKSource = clk_source;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; /* AHB clock not divide */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  /* APB clock not divided */
    /* Configure clock source */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        APP_ErrorHandler();
    }
    HAL_ResumeTick();
}

int pm_entry_sleep()
{
    pm_entry_sleep_before();

    while (!key_get_pressed_num())
    {
        key_config_wakeupPin();
#if PM_IWDG_EN
        lptim_start();
#endif
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);
#if PM_IWDG_EN
        lptim_stop();
#endif
        key_reset_wakeupPin();
        kb_fast_scan(key_evt_handle);
    }

    pm_wakeup_from_sleep();
    return 0;
}

void pm_refreshIWDG()
{
#if PM_IWDG_EN
    HAL_IWDG_Refresh(&IwdgHandle);
#endif
}

uint16_t pm_VCC()
{
    uint32_t adc_value;
    uint16_t T_VCC;
    HAL_ADC_Start(&AdcHandle);

    /* Waiting for ADC conversion completed */
    HAL_ADC_PollForConversion(&AdcHandle, 100);

    /* Get ADC value */
    adc_value = HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);

    /* Calculate VCC voltage */
    T_VCC = (4095 * 1200) / adc_value;
    return T_VCC;
}