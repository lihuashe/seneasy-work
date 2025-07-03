
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include "trace.h"
#include "rtl876x_pinmux.h"
#include "os_timer.h"
#include "swtimer.h"
#include "rtl876x_uart.h"
#include "rtl876x_rcc.h"
#include "rtl876x_nvic.h"
#include "rtl876x.h"
#include "uart_driver.h"
#include "app_msg.h"
#include "app_task.h"
#include "platform_utils.h"



UART_RX_BUF g_uart_rx = {};

/**
 * @brief 复用串口1初始化
 * @param baud_rate 定义波特率
 */
void uart_init(uint32_t baud_rate)
{
    // 先重置 UART
    UART_DeInit(UART1);
    
    Pinmux_Config(UART_TX_PIN, UART1_TX);
    Pinmux_Config(UART_RX_PIN, UART1_RX);
    
    /* pad configuration */
    Pad_Config(UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
              PAD_OUT_LOW);

    // 启用 UART 时钟
    RCC_PeriphClockCmd(APBPeriph_UART1, APBPeriph_UART1_CLOCK, ENABLE);

    // 初始化 UART 配置结构体
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);


    // 根据波特率设置 UART 参数
    if (baud_rate == 1000000) // 1M
    {
        UART_InitStruct.UART_Div = 4;
        UART_InitStruct.UART_Ovsr = 5;
        UART_InitStruct.UART_OvsrAdj = 0;
    }
    else if (baud_rate == 2000000) // 2M
    {
        UART_InitStruct.UART_Div = 2;
        UART_InitStruct.UART_Ovsr = 5;
        UART_InitStruct.UART_OvsrAdj = 0;
    }
    else // 默认设置为 115200
    {
        UART_InitStruct.UART_Div = 20;
        UART_InitStruct.UART_Ovsr = 12;
        UART_InitStruct.UART_OvsrAdj = 0x252;
    }

    // 设置其他初始化参数
    UART_InitStruct.UART_TxDmaEn = ENABLE;
    UART_InitStruct.UART_RxDmaEn = ENABLE;
    UART_InitStruct.UART_TxThdLevel = 4; //1~29
    UART_InitStruct.UART_RxThdLevel = 4;
    UART_InitStruct.UART_RxWaterLevel = 6; 


    // 初始化 UART
    UART_Init(UART1, &UART_InitStruct);

    /*  Enable UART IRQ  */
    NVIC_InitTypeDef nvic_init_struct;
    nvic_init_struct.NVIC_IRQChannel         = UART1_IRQn;
    nvic_init_struct.NVIC_IRQChannelCmd      = ENABLE;
    nvic_init_struct.NVIC_IRQChannelPriority = 4;
    NVIC_Init(&nvic_init_struct);

    UART_INTConfig(UART1, UART_INT_RD_AVA | UART_INT_RX_LINE_STS | UART_INT_RX_IDLE, ENABLE);
    NVIC_ClearPendingIRQ(UART1_IRQn);
}


/**
 * @brief 发送连续数据
 * @param UARTx 哪一个串口
 * @param pSend_Buf 数据起始地址
 * @param vCount 数据长度
 */
void uart_senddata(UART_TypeDef *UARTx, const uint8_t *pSend_Buf, uint16_t vCount)
{
    uint8_t count;

    while (vCount / UART_TX_FIFO_SIZE > 0)
    {
        while (UART_GetFlagStatus(UARTx, UART_FLAG_TX_FIFO_EMPTY) == 0);
        for (count = UART_TX_FIFO_SIZE; count > 0; count--)
        {
            UARTx->RB_THR = *pSend_Buf++;
        }
        vCount -= UART_TX_FIFO_SIZE;
    }

    while (UART_GetFlagStatus(UARTx, UART_FLAG_TX_FIFO_EMPTY) == 0);
    while (vCount--)
    {
        UARTx->RB_THR = *pSend_Buf++;
    }
}

/**
 * @brief 发送msg给产测函数
 * @param p_msg 
 * @return 
 */
bool uart_send_msg_to_task(T_IO_MSG *p_msg)
{
    if (os_msg_send(product_test_io_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_ERROR0("send_io_msg_to_product fail");
        return false;
    }
   
    return true;
}

/**
 * @brief 串口1中断
 * @param  
 */
void data_uart1_interrupt_handler(void)
{
    /* 读取中断 ID */
    uint32_t int_status = UART_GetIID(UART1);
    static bool is_get_done = false;
    static uint8_t save_rx_count = 0;

    /* 禁用中断 */
    UART_INTConfig(UART1, UART_INT_RD_AVA | UART_INT_RX_LINE_STS, DISABLE);

    if (UART_GetFlagStatus(UART1, UART_FLAG_RX_IDLE) == SET)
    {
        //clear Flag
        save_rx_count = 0;
        is_get_done = true;  
        UART_INTConfig(UART1, UART_INT_RX_IDLE, DISABLE);
        /* Send Msg to App task */
        T_IO_MSG uart_test_msg;
        uart_test_msg.type  = IO_MSG_TYPE_UART;
        uart_test_msg.subtype = IO_MSG_SUBTYPE_PRODUCTION;
        uart_test_msg.u.param = DEBUG_MODE;
        uart_test_msg.u.buf   = (void *)(&g_uart_rx);

        if (false == uart_send_msg_to_task(&uart_test_msg))
        {
            PRINT("[data_uart_test_interrupt_handler] Send IO_MSG_TYPE_UART message failed!");
        }
        //enable idle interrupt again
        UART_ClearRxFIFO(UART1);
        UART_INTConfig(UART1, UART_INT_RX_IDLE, ENABLE);
    }
    
    switch (int_status)
    {
        /* rx data valiable */
        case UART_INT_ID_RX_LEVEL_REACH:
            {
                //PRINT("UART_INT_ID_RX_LEVEL_REACH!\n");
                save_rx_count++;//检测连续rx中断
                static uint8_t temp_buf[UART_RX_TRIGGER_VALUE];
                if (is_get_done == true)
                {
                    is_get_done = false;
                    memset((void *)&temp_buf, 0, UART_RX_TRIGGER_VALUE);
                }
                
                if(is_get_done == false)
                {
                    for (uint8_t i = 0; i < UART_RX_TRIGGER_VALUE; i++)
                    {
                        if (temp_buf[i] != 0xfe)
                        {
                            UART_ReceiveData(UART1, (void *)&temp_buf[i], 1);

                            for (volatile int j = 0; j < 600; j++);
                        }
                    }
                }

                g_uart_rx.rx_get = true;
                g_uart_rx.rx_buf = (uint8_t *)temp_buf;
                g_uart_rx.buf_len = UART_RX_TRIGGER_VALUE;
                break;
            }

        /* rx time out */
        case UART_INT_ID_RX_DATA_TIMEOUT:
            {
                //PRINT("UART_INT_ID_RX_DATA_TIMEOUT!\n");
                uint8_t temp_data;
                while (UART_GetFlagStatus(UART1, UART_FLAG_RX_DATA_AVA) == SET)
                {
                    UART_ReceiveData(UART1, &temp_data, 1);
                    // 处理接收到的数据
                    // ...
                }
                break;
            }

        /* receive line status interrupt */
        case UART_INT_ID_LINE_STATUS:
            {
                //PRINT("UART_INT_ID_LINE_STATUS!\n");
                UART_GetFlagStatus(UART1, UART_FLAG_RX_OVERRUN);
                break;
            }

        default:
            break;
    }
    
    //如果连续触发中断却不进入产测，就是在进行充电
    if (save_rx_count >= 2)
    {
        UART_INTConfig(UART1, UART_INT_RD_AVA | UART_INT_RX_LINE_STS | UART_INT_RX_IDLE, DISABLE);//失能串口
    }
    else
    {
        /* 重新启用中断 */
        UART_INTConfig(UART1, UART_INT_RD_AVA  | UART_INT_RX_LINE_STS, ENABLE);   
    }
   
}


int uart_putchar(int ch)
{
    UART_SendData(UART1, (uint8_t *)&ch, 1); // 发送字符到 UART1
    /* wait tx fifo empty */
    while (UART_GetFlagState(UART0, UART_FLAG_THR_TSR_EMPTY) != SET);
    return ch; // 返回字符
}
