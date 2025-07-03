#include "fct_uart.h"
#include "py32f0xx_hal.h"
#include "time_utils.h"
#include <string.h>

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
volatile static uint32_t last_rx_clock_t = 0;
static RingBuffer *uart_rx_buf;
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

UART_HandleTypeDef UartHandle;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

void uart_rx_irq()
{
    /* UART in mode Receiver */
    if ((__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&UartHandle, UART_IT_RXNE) != RESET))
    {
        /* Receive data */
        uint8_t c = (uint8_t)(UartHandle.Instance->DR & (uint8_t)0x00FF);
        RingBuffer_Write(uart_rx_buf, &c, 1);

        last_rx_clock_t = clock_time();
    }
}

void fct_uart_init(RingBuffer *rbuf)
{
    __HAL_RCC_USART1_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    UartHandle.Instance = USART1;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartHandle);

    uart_rx_buf = rbuf;
    /* Enable RXNE interrupt */
    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
    /* Enable NVIC */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

bool fct_uart_isRecv()
{
    return !RingBuffer_IsEmpty(uart_rx_buf) && clock_time_exceed(last_rx_clock_t, 15);
}

HAL_StatusTypeDef fct_uart_sendto(uint8_t *pData, uint8_t len)
{
    /* Wait for the end of the transfer */
    static u8 _buf[RING_BUFFER_SIZE]; // 缓冲区大小
    memcpy(_buf, pData, len);
    u8 wait_ms = 0;
    while (HAL_UART_GetState(&UartHandle) == HAL_UART_STATE_BUSY_TX)
    {
        if (wait_ms > 5)
            return HAL_TIMEOUT;
        HAL_Delay(1);
        wait_ms++;
    }
    return HAL_UART_Transmit_IT(&UartHandle, _buf, len);
}
