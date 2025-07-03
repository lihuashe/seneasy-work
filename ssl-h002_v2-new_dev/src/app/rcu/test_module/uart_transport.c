/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     uart_transport.c
* @brief    This file provides uart transport layer driver for rcu uart test.
* @details
* @author   chenjie_jin
* @date     2020-03-03
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                                  Header Files
 *============================================================================*/
#include <board.h>
#include <uart_transport.h>
#include <data_uart_test.h>
#include <rtl876x_uart.h>
#include <rtl876x_pinmux.h>
#include <rtl876x_rcc.h>
#include <rtl876x_nvic.h>
#include <loop_queue.h>
#include <app_task.h>
#include <app_section.h>

#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST

/*============================================================================*
 *                                  Macros
 *============================================================================*/
/* Protocol defines ------------------------------------------------------------*/
/* Protocol header define */
#define PACKET_HEAD                     ((uint8_t)0x87)
#define PACKET_HEAD_LEN                 ((uint8_t)0x01)
#define OPCODE_LEN                      ((uint8_t)0x02)
#define CRC_DATA_LEN                    ((uint16_t)0x02)
/* Configure data uart receive trigger level */
#define UART_RX_TRIGGER_LEVEL           4
#define UART_RX_TRIGGER_VALUE           4

#define  FAR
typedef unsigned char    UCHAR, BYTE, * PUCHAR, * PBYTE;
typedef unsigned short   WORD, USHORT, * PUSHORT, * PWORD;
typedef unsigned char    FAR *LPBYTE;

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
static T_LOOP_QUEUE_DEF *p_uart_loop_queue = NULL;
/* UART packet data structure */
static T_UART_PACKET_DEF uart_packet;
/* CRC check function */
extern uint16_t btxfcs(WORD fcs, BYTE FAR *cp, WORD len);

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void uart_packet_struct_init(T_UART_PACKET_DEF *p_packet);
static void data_uart_test_interrupt_handler(void);

static void data_uart_test_interrupt_handler(void) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief   Initializes UART packet data structure.
 * @param   p_packet - point to UART packet structure.
 * @return  none
 * @retval  void
 */
void uart_packet_struct_init(T_UART_PACKET_DEF *p_packet)
{
    /* Initialize UART packet data structure */
    p_packet->buf_index   = 0;
    p_packet->crc_len     = 0;
    p_packet->payload_len = 0;
    p_packet->uart_status = WaitHeader;
}

/******************************************************************
 * @brief   Data UART interrupt handle.
 * @param   none
 * @return  none
 * @retval  void
 */
void data_uart_test_interrupt_handler(void)
{
    /* read interrupt id */
    uint32_t int_status = UART_GetIID(UART0);

    UART_DBG_BUFFER(MODULE_UART, LEVEL_INFO, "[data_uart_test_interrupt_handler] int_status = %d", 1,
                    int_status);

    /* disable interrupt */
    UART_INTConfig(UART0, UART_INT_RD_AVA | UART_INT_RX_LINE_STS, DISABLE);

    if (UART_GetFlagStatus(UART0, UART_FLAG_RX_IDLE) == SET)
    {
        //clear Flag
        UART_INTConfig(UART0, UART_INT_RX_IDLE, DISABLE);
        /* Send Msg to App task */
        T_IO_MSG uart_test_msg;
        uart_test_msg.type  = IO_MSG_TYPE_UART;
        uart_test_msg.subtype = IO_MSG_UART_RX;
        uart_test_msg.u.buf    = (void *)(&uart_packet);

        if (false == app_send_msg_to_apptask(&uart_test_msg))
        {
            APP_PRINT_ERROR0("[data_uart_test_interrupt_handler] Send IO_MSG_TYPE_UART message failed!");
        }
        //enable idle interrupt again
        UART_ClearRxFIFO(UART0);
        UART_INTConfig(UART0, UART_INT_RX_IDLE, ENABLE);
    }

    switch (int_status)
    {
    /* tx fifo empty, not enable */
    case UART_INT_ID_TX_EMPTY:
        {
            /* do nothing */
            break;
        }

    /* rx data valiable */
    case UART_INT_ID_RX_LEVEL_REACH:
        {
            uint8_t temp_buf[UART_RX_TRIGGER_VALUE];
            UART_ReceiveData(UART0, temp_buf, UART_RX_TRIGGER_VALUE);
            if (!loop_queue_is_full(p_uart_loop_queue, UART_RX_TRIGGER_VALUE))
            {
                loop_queue_write_buf(p_uart_loop_queue, temp_buf, UART_RX_TRIGGER_VALUE, true);
            }
            break;
        }

    /* rx time out */
    case UART_INT_ID_RX_DATA_TIMEOUT:
        {
            uint8_t temp_data;
            while (UART_GetFlagStatus(UART0, UART_FLAG_RX_DATA_AVA) == SET)
            {
                UART_ReceiveData(UART0, &temp_data, 1);
                if (!loop_queue_is_full(p_uart_loop_queue, 1))
                {
                    loop_queue_write_buf(p_uart_loop_queue, &temp_data, 1, true);
                }
            }
            break;
        }

    /* receive line status interrupt */
    case UART_INT_ID_LINE_STATUS:
        {
            UART_GetFlagStatus(UART0, UART_FLAG_RX_OVERRUN);
            UART_DBG_BUFFER(MODULE_APP, LEVEL_INFO, "line status error!", 0);
            break;
        }

    default:
        break;
    }
    /* enable interrupt again */
    UART_INTConfig(UART0, UART_INT_RD_AVA | UART_INT_RX_LINE_STS, ENABLE);
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/******************************************************************
 * @brief   Initializes UART peripheral.
 * @param   buadrate_opt - buadrate value.
 * @return  none
 * @retval  void
 */
void data_uart_init(uint8_t buadrate_opt)
{
    /* pinmux configuration */
    UART_DeInit(UART0);
    Pinmux_Config(MP_TEST_UART_TX_PIN, UART0_TX);
    Pinmux_Config(MP_TEST_UART_RX_PIN, UART0_RX);


    /* pad configuration */
    Pad_Config(MP_TEST_UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(MP_TEST_UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);

    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);

    /* uart init */
    UART_InitTypeDef uartInitStruct;
    UART_StructInit(&uartInitStruct);
    uartInitStruct.UART_RxThdLevel = UART_RX_TRIGGER_LEVEL;

    if (buadrate_opt == CHANGE_BAUDRATE_OPTION_1M)
    {
        /* set baudrate to 1M */
        uartInitStruct.UART_Div = 4;
        uartInitStruct.UART_Ovsr = 5;
        uartInitStruct.UART_OvsrAdj = 0;
        uartInitStruct.UART_Parity = UART_PARITY_NO_PARTY;
    }
    else if (buadrate_opt == CHANGE_BAUDRATE_OPTION_2M)
    {
        /* set baudrate to 2M */
        uartInitStruct.UART_Div = 2;
        uartInitStruct.UART_Ovsr = 5;
        uartInitStruct.UART_OvsrAdj = 0;
        uartInitStruct.UART_Parity = UART_PARITY_NO_PARTY;
    }
    else
    {
        /* set baudrate to 115200 */
        uartInitStruct.UART_Div = 20;
        uartInitStruct.UART_Ovsr = 12;
        uartInitStruct.UART_OvsrAdj = 0x252;
        uartInitStruct.UART_Parity = UART_PARITY_NO_PARTY;
    }

    UART_Init(UART0, &uartInitStruct);

    /* NVIC init */
    UART_INTConfig(UART0, UART_INT_RD_AVA | UART_INT_RX_LINE_STS | UART_INT_RX_IDLE, ENABLE);
    NVIC_ClearPendingIRQ(UART0_IRQn);

    /*  Enable UART IRQ  */
    NVIC_InitTypeDef nvic_init_struct;
    nvic_init_struct.NVIC_IRQChannel         = UART0_IRQn;
    nvic_init_struct.NVIC_IRQChannelCmd      = ENABLE;
    nvic_init_struct.NVIC_IRQChannelPriority = 5;
    NVIC_Init(&nvic_init_struct);
}

/******************************************************************
 * @brief   Initializes loop queue and UART packet data structure to their default reset values.
 * @param   none
 * @return  none
 * @retval  void
 */
void data_uart_transport_init(void)
{
    /* Initialize loop queue */
    if (p_uart_loop_queue == NULL)
    {
        p_uart_loop_queue = loop_queue_init(UART_LOOP_QUEUE_MAX_SIZE, sizeof(uint8_t), RAM_TYPE_DATA_ON);
    }

    /* Initialize UART packet data structure */
    uart_packet_struct_init(&uart_packet);

    /* Update Data UART interrupt handle */
    RamVectorTableUpdate(Uart0_VECTORn, data_uart_test_interrupt_handler);

    /* Initialize Data UART peripheral */
    data_uart_init(CHANGE_BAUDRATE_OPTION_115200);
}

/******************************************************************
 * @brief   decode uart packet.
 * @param   p_queue_struct - point to loop queue data structure.
 * @param   p_packet - point to UART packet struct.
 * @return  result
 * @retval  true or false
 */
static bool uart_packet_decode(T_LOOP_QUEUE_DEF *p_queue_struct, T_UART_PACKET_DEF *p_packet)
{
    bool is_paket_complete = false;
    uint16_t opcode = 0;
    uint8_t param_len = 0;
    uint8_t *p_loop_buf = (uint8_t *)p_queue_struct->p_buf;

    UART_DBG_BUFFER(MODULE_UART, LEVEL_INFO, "[uart_packet_decode] decode uart packet", 0);
    UART_DBG_BUFFER(MODULE_UART, LEVEL_INFO, "[uart_packet_decode] %d %d", 2,
                    p_queue_struct->read_index,
                    p_queue_struct->write_index);

    while (p_queue_struct->read_index != p_queue_struct->write_index)
    {
        switch (p_packet->uart_status)
        {
        case WaitHeader:
            {
                /* Header: one byte */
                if (p_loop_buf[p_queue_struct->read_index] == PACKET_HEAD)
                {
                    if (true == loop_queue_read_buf(p_queue_struct, &p_packet->uart_buf[p_packet->buf_index], 1))
                    {
                        p_packet->buf_index++;
                    }
                    p_packet->uart_status = WaitCMD;
                }
                else
                {
                    p_queue_struct->read_index++;
                    UART_DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Error header = 0x%x!", 1,
                                    p_loop_buf[p_queue_struct->read_index]);
                }
                break;
            }
        case WaitCMD:
            {
                while (!loop_queue_is_empty(p_uart_loop_queue))
                {
                    /* Obtain command field data */
                    if (true == loop_queue_read_buf(p_queue_struct, &p_packet->uart_buf[p_packet->buf_index], 1))
                    {
                        p_packet->buf_index++;
                    }

                    /* CMD: two bytes */
                    if (p_packet->buf_index == (PACKET_HEAD_LEN + OPCODE_LEN))
                    {
                        opcode = (p_packet->uart_buf[p_packet->buf_index - 1] << 8) + p_packet->uart_buf[p_packet->buf_index
                                 - 2];

                        if ((opcode >= UART_FN_BEGIN) && (opcode < UART_FN_END))
                        {
                            param_len = uart_test_func_map[opcode & UART_FN_MASK].param_len;
                        }
                        else
                        {
                            param_len = 0;
                        }
                        p_packet->uart_status = WaitParams;
                        break;
                    }
                }
                break;
            }

        case WaitParams:
            {
                /* Obtain parameter field data */
                if (true == loop_queue_read_buf(p_queue_struct, &p_packet->uart_buf[p_packet->buf_index],
                                                param_len))
                {
                    p_packet->buf_index += param_len;
                }
                p_packet->uart_status = WaitCRC;
                break;
            }

        case WaitCRC:
            {
                uint16_t vailid_data_size = loop_queue_get_vailid_data_size(p_uart_loop_queue);
                if ((vailid_data_size != 0) && (p_packet->crc_len < CRC_DATA_LEN))
                {
                    if (vailid_data_size >= CRC_DATA_LEN - p_packet->crc_len)
                    {
                        if (true == loop_queue_read_buf(p_queue_struct, &p_packet->uart_buf[p_packet->buf_index],
                                                        CRC_DATA_LEN - p_packet->crc_len))
                        {
                            p_packet->buf_index += CRC_DATA_LEN - p_packet->crc_len;
                        }
                    }
                    else
                    {
                        if (true == loop_queue_read_buf(p_queue_struct, &p_packet->uart_buf[p_packet->buf_index],
                                                        vailid_data_size))
                        {
                            p_packet->buf_index += vailid_data_size;
                        }
                    }
                }

                /* CRC check */
                if (btxfcs(0, p_packet->uart_buf,
                           (p_packet->buf_index) - CRC_DATA_LEN) == ((p_packet->uart_buf[p_packet->buf_index
                                                                                         - 1] << 8) + p_packet->uart_buf[p_packet->buf_index - 2]))
                {
                    is_paket_complete = true;
                }
                else
                {
                    /* CRC check error */
                    UART_DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "CRC check Error!", 0);
                }

                /* Reset decoding status */
                uart_packet_struct_init(p_packet);
                return is_paket_complete;
            }
        default:
            {
                break;
            }
        }
    }

    return is_paket_complete;
}

/******************************************************************
 * @brief   Response of uart command.
 * @param   opcode
 * @param   status
 * @param   p_payload - point to payload of uart response.
 * @param   payload_length - length of payload.
 * @return  none
 * @retval  void
 */
void data_uart_cmd_response(uint16_t opcode, uint8_t status, uint8_t *p_payload,
                            uint32_t payload_length)
{
    T_UART_PACKET_DEF response_packet;
    T_UART_PACKET_DEF *p_resp_packet = &response_packet;

    p_resp_packet->buf_index = 0;
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = PACKET_HEAD;
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = opcode & 0xff;
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = opcode >> 8;
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = status;
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = payload_length & 0xff;
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = payload_length >> 8;
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = payload_length >> 16;
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = payload_length >> 24;

    while (payload_length--)
    {
        p_resp_packet->uart_buf[p_resp_packet->buf_index++] = *p_payload++;
    }

    /* Add CRC data */
    uint16_t crc_calc = btxfcs(0, p_resp_packet->uart_buf, p_resp_packet->buf_index);
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = (uint8_t)(crc_calc & 0xff);
    p_resp_packet->uart_buf[p_resp_packet->buf_index++] = (uint8_t)(crc_calc >> 8);

    /* send block bytes(16 bytes) */
    uint32_t i = 0;
    for (i = 0; i < (p_resp_packet->buf_index / UART_TX_FIFO_SIZE); i++)
    {
        UART_SendData(UART0, &(p_resp_packet->uart_buf[UART_TX_FIFO_SIZE * i]), UART_TX_FIFO_SIZE);
        /* wait tx fifo empty */
        while (UART_GetFlagStatus(UART0, UART_FLAG_TX_EMPTY) != SET);
    }

    /* send left bytes */
    UART_SendData(UART0, &(p_resp_packet->uart_buf[UART_TX_FIFO_SIZE * i]),
                  p_resp_packet->buf_index % UART_TX_FIFO_SIZE);
    /* wait tx fifo empty */
    while (UART_GetFlagStatus(UART0, UART_FLAG_TX_EMPTY) != SET);
}

/******************************************************************
 * @brief   decode uart packet.
 * @param   p_packet -  point to UART packet struct.
 * @return  result
 * @retval  true or false
 */
bool data_uart_packet_decode(T_UART_PACKET_DEF *p_packet)
{
    return uart_packet_decode(p_uart_loop_queue, p_packet);
}
#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

