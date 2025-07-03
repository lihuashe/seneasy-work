/**
 * @file led_driver.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief
 * @version 0.1
 * @date 2025-03-05
 *
 *
 */
#include "led_driver.h"
#include "py32f0xx_hal.h"
#include "my_log.h"
#include "utility.h"
/*============================================================================*
 *                              Local Variables
 *============================================================================*/

#define R_LED1_PIN GPIO_PIN_15
#define R_LED1_PORT GPIOA

#define R_LED2_4_PIN GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5
#define R_LED2_4_PORT GPIOB

#define C_LED2_5_PIN GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
#define C_LED2_5_PORT GPIOA

#define C_LED1_PIN GPIO_PIN_8
#define C_LED1_PORT GPIOB
#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER

static uint8_t led_matrix[4];

#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL
LED_HandleTypeDef hled;

#endif
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER

Led g_led[LED_NUM] =
    {
        LED_ITEM(0, 0), // -LED0
        LED_ITEM(0, 0), // LED1
        LED_ITEM(0, 1), // LED2
        LED_ITEM(0, 2), // LED3
        LED_ITEM(0, 3), // LED4
        LED_ITEM(0, 4), // LED5
        LED_ITEM(1, 0), // LED6
        LED_ITEM(1, 1), // LED7
        LED_ITEM(1, 2), // LED8
        LED_ITEM(1, 3), // LED9
        LED_ITEM(1, 4), // LED10
        LED_ITEM(2, 0), // LED11
        LED_ITEM(2, 1), // LED12
        LED_ITEM(2, 2), // LED13
        LED_ITEM(2, 3), // LED14
        LED_ITEM(2, 4), // LED15
        LED_ITEM(3, 0), // LED16
        LED_ITEM(3, 1), // LED17
        LED_ITEM(3, 2), // LED18
        LED_ITEM(3, 3), // LED19

};
TIM_HandleTypeDef LED_TimHandle;
#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL

Led g_led[LED_NUM] =
    {
        LED_ITEM(LED_COM0, LED_SEG_A), // LED0
        LED_ITEM(LED_COM0, LED_SEG_A), // LED1
        LED_ITEM(LED_COM0, LED_SEG_B), // LED2
        LED_ITEM(LED_COM0, LED_SEG_C), // LED3
        LED_ITEM(LED_COM0, LED_SEG_D), // LED4
        LED_ITEM(LED_COM0, LED_SEG_E), // LED5
        LED_ITEM(LED_COM1, LED_SEG_A), // LED6
        LED_ITEM(LED_COM1, LED_SEG_B), // LED7
        LED_ITEM(LED_COM1, LED_SEG_C), // LED8
        LED_ITEM(LED_COM1, LED_SEG_D), // LED9
        LED_ITEM(LED_COM1, LED_SEG_E), // LED10
        LED_ITEM(LED_COM2, LED_SEG_A), // LED11
        LED_ITEM(LED_COM2, LED_SEG_B), // LED12
        LED_ITEM(LED_COM2, LED_SEG_C), // LED13
        LED_ITEM(LED_COM2, LED_SEG_D), // LED14
        LED_ITEM(LED_COM2, LED_SEG_E), // LED15
        LED_ITEM(LED_COM3, LED_SEG_A), // LED16
        LED_ITEM(LED_COM3, LED_SEG_B), // LED17
        LED_ITEM(LED_COM3, LED_SEG_C), // LED18
        LED_ITEM(LED_COM3, LED_SEG_D), // LED19
};
#endif

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER

void led_tim16_irq()
{
    static volatile uint8_t current_row = 0;
    HAL_GPIO_WritePin(R_LED1_PORT, R_LED1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(R_LED2_4_PORT, R_LED2_4_PIN, GPIO_PIN_SET);

    HAL_GPIO_WritePin(C_LED1_PORT, C_LED1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(C_LED2_5_PORT, C_LED2_5_PIN, GPIO_PIN_RESET);
    // 切换至下一行
    switch (current_row)
    {
    case 0:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
        break;
    case 1:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
        break;
    case 2:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
        break;
    case 3:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
        break;
    }
    // 更新列数据
    uint8_t col_data = led_matrix[current_row];
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, (col_data & 0x01));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, (col_data & 0x02));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (col_data & 0x04));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, (col_data & 0x08));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (col_data & 0x10));
    current_row = (current_row + 1) % 4;
}
/**
 * @brief led引脚配置、初始化
 *
 */
void led_matrix_init()
{
    GPIO_InitTypeDef GPIO_Initure = {0};

    GPIO_Initure.Pin = R_LED2_4_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Initure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(R_LED2_4_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = R_LED1_PIN;
    HAL_GPIO_Init(R_LED1_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = C_LED2_5_PIN;
    HAL_GPIO_Init(C_LED2_5_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = C_LED1_PIN;
    HAL_GPIO_Init(C_LED1_PORT, &GPIO_Initure);

    LED_TimHandle.Instance = TIM16;                                        /* Select TIM16 */
    LED_TimHandle.Init.Period = 12800 - 1;                                 /* Auto-reload value */
    LED_TimHandle.Init.Prescaler = 5 - 1;                                  /* Prescaler of 1000-1 */
    LED_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;             /* Clock not divided */
    LED_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;                   /* Up counting mode */
    LED_TimHandle.Init.RepetitionCounter = 1 - 1;                          /* No repetition */
    LED_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* Auto-reload register not buffered */
    if (HAL_TIM_Base_Init(&LED_TimHandle) != HAL_OK)                       /* Initialize TIM16 */
    {
#if __DEBUG__ >= __DEBUG_ERR__
        my_log("\033[1;31m>E%d[%s] LED Time Init Fail\n\033[0m", __LINE__, __FUNCTION__);
#endif
    }
    if (HAL_TIM_Base_Start_IT(&LED_TimHandle) != HAL_OK)
    {
#if __DEBUG__ >= __DEBUG_ERR__
        my_log("\033[1;31m>E%d[%s] LED Time Init Fail\n\033[0m", __LINE__, __FUNCTION__);
#endif
    }

    /* Set interrupt priority */
    HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
    /* Enable TIM1 interrupt */
    HAL_NVIC_EnableIRQ(TIM16_IRQn);
    for (int i = 0; i < sizeof(g_led) / sizeof(Led); i++)
    {
        g_led[i].id = i;
    }
}

void led_matrix_ctrl(Led *this, bool _isOn)
{
    this->isOn = _isOn;
    if (_isOn)
        led_matrix[this->row] |= 1 << this->col;
    else
        led_matrix[this->row] &= ~(1 << this->col);
}

#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL

void led_seg_init()
{
    GPIO_InitTypeDef GPIO_Initure = {0};

    GPIO_Initure.Pin = GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Alternate = GPIO_AF6_LED;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    // GPIO_Initure.Mode = GPIO_MODE_AF_PP;
    // GPIO_Initure.Pull = GPIO_PULLUP;
    // GPIO_Initure.Alternate = GPIO_AF6_LED;
    // GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);

    /* Configure PA 0-3 as SEG B-E */
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    // GPIO_Initure.Mode = GPIO_MODE_AF_PP;
    // GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Alternate = GPIO_AF3_LED;
    // GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    /* Configure PB8 as SEG A */
    GPIO_Initure.Pin = GPIO_PIN_8;
    // GPIO_Initure.Mode = GPIO_MODE_AF_PP;
    // GPIO_Initure.Pull = GPIO_PULLUP;
    // GPIO_Initure.Alternate = GPIO_AF3_LED;
    // GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);

    /* LED initialization */
    hled.Instance = LED;
    hled.Init.ComNum = 4 - 1;              /* Enable 4 COM ports */
    hled.Init.ComDrive = LED_COMDRIVE_LOW; /* LED COM output */
    hled.Init.Prescaler = 10 - 1;          /* Fpclk/(PR+1) */
    hled.Init.LightTime = 240;             /* Time each LED is illuminated (240*fLED, where fLED is the LED module counting clock frequency)  */
    hled.Init.DeadTime = 10;               /* Interval time between two LED switches, must not be zero */
    HAL_LED_Init(&hled);
    for (int i = 0; i < ARRAY_SIZE(g_led); i++)
    {
        g_led[i].id = i;
    }
}

HAL_StatusTypeDef led_seg_ctrl(Led *this, bool _isOn)
{
    this->isOn = _isOn;
    uint32_t position = 0x00U;
    uint32_t chcurrent = 0x00U;
    uint32_t *pTmp;
    if (hled.State == HAL_LED_STATE_READY)
    {
        /* Process Locked */
        __HAL_LOCK(&hled);
        hled.State = HAL_LED_STATE_BUSY;

        while ((this->com >> position) != 0)
        {
            /* Get the COM channel position */
            chcurrent = this->com & (1U << position);

            if (chcurrent)
            {
                pTmp = ((uint32_t *)&hled.Instance->DR0) + position;
                u32 dr = READ_REG(*pTmp);
                if (_isOn)
                    dr |= 1 << this->seg;
                else
                    dr &= ~(1 << this->seg);

                WRITE_REG(*pTmp, dr);
            }

            position++;
        }

        /* Process Unlocked */
        __HAL_UNLOCK(&hled);
        hled.State = HAL_LED_STATE_READY;

        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }
}

#endif

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/**
 * @brief Led Init
 *
 */
void led_init()
{
    
#if __DEBUG__ >= __DEBUG_INFO__
    my_log(">I%d[%s] \n", __LINE__, __FUNCTION__);
#endif

#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER
    led_matrix_init();

#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL
    led_seg_init();
#endif
}

/**
 * @brief 打开led
 *
 * @param id led ID
 */
void led_on(unsigned char id)
{
#if __DEBUG__ >= __DEBUG_ERR__
    if (id >= LED_NUM)
    {
        my_log("\033[1;31m>E%d[%s] out of range id:%d \n\033[0m", __LINE__, __FUNCTION__, id);
    }
#endif
#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER
    led_matrix_ctrl(&g_led[id], 1);

#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL
    led_seg_ctrl(&g_led[id], 1);

#endif
}

/**
 * @brief 关闭led
 *
 * @param id led ID
 */
void led_off(unsigned char id)
{
#if __DEBUG__ >= __DEBUG_ERR__
    if (id >= LED_NUM)
    {
        my_log("\033[1;31m>E%d[%s] out of range id:%d \n\033[0m", __LINE__, __FUNCTION__, id);
    }
#endif
#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER
    led_matrix_ctrl(&g_led[id], 0);

#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL
    led_seg_ctrl(&g_led[id], 0);

#endif
}

/**
 * @brief 翻转led状态
 *
 * @param id led ID
 */
void led_toggle(unsigned char id)
{
#if __DEBUG__ >= __DEBUG_ERR__
    if (id >= LED_NUM)
    {
        my_log("\033[1;31m>E%d[%s] out of range id:%d \n\033[0m", __LINE__, __FUNCTION__, id);
    }
#endif
    Led *this = &g_led[id];
#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER
    led_matrix_ctrl(this, !this->isOn);

#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL
    led_seg_ctrl(this, !this->isOn);

#endif
}

bool led_state(unsigned char id)
{
#if __DEBUG__ >= __DEBUG_ERR__
    if (id >= LED_NUM)
    {
        my_log("\033[1;31m>E%d[%s] out of range id:%d \n\033[0m", __LINE__, __FUNCTION__, id);
    }
#endif
    return g_led[id].isOn;
}

void led_set_brightness(unsigned char id, int val)
{
#if __DEBUG__ >= __DEBUG_ERR__
    if (id >= LED_NUM)
    {
        my_log("\033[1;31m>E%d[%s] out of range id:%d \n\033[0m", __LINE__, __FUNCTION__, id);
    }
#endif        
}

unsigned char led_get_brightness(unsigned char id)
{
#if __DEBUG__ >= __DEBUG_ERR__
    if (id >= LED_NUM)
    {
        my_log("\033[1;31m>E%d[%s] out of range id:%d \n\033[0m", __LINE__, __FUNCTION__, id);
    }
#endif
    return 0;
}


void led_driver_sleep()
{
#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER
    HAL_GPIO_WritePin(R_LED1_PORT, R_LED1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(R_LED2_4_PORT, R_LED2_4_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(C_LED2_5_PORT, C_LED2_5_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(C_LED1_PORT, C_LED1_PIN, GPIO_PIN_RESET);
#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL
    GPIO_InitTypeDef GPIO_Initure = {0};

    GPIO_Initure.Pin = R_LED2_4_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Initure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(R_LED2_4_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = R_LED1_PIN;
    HAL_GPIO_Init(R_LED1_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = C_LED2_5_PIN;
    HAL_GPIO_Init(C_LED2_5_PORT, &GPIO_Initure);

    GPIO_Initure.Pin = C_LED1_PIN;
    HAL_GPIO_Init(C_LED1_PORT, &GPIO_Initure);
    HAL_GPIO_WritePin(R_LED1_PORT, R_LED1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(R_LED2_4_PORT, R_LED2_4_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(C_LED2_5_PORT, C_LED2_5_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(C_LED1_PORT, C_LED1_PIN, GPIO_PIN_RESET);

#endif
}

void led_driver_wakeup()
{
    led_seg_init();
}
