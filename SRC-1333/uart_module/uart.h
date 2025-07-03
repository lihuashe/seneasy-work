#ifndef _UART_H
#define _UART_H

#include "SYSCFG.h";
#include "stdint.h";
#include "stdbool.h";
#include "timer.h";
#include "FT61F0AX.h";

#define RECMAXLEN   256  /* 接收数据总长度 */
#define SENDMAXLEN  99  /* 发送数据总长度 */

typedef enum{
    UART_STATE_IDLE,       //空闲
    UART_STATE_HEAD,       //帧头
    UART_STATE_TYPE,       //数据类型
    UART_STATE_LEN,        //数据长度
    UART_STATE_DATA,       //数据
    UART_STATE_SUCCESS,    //接收数据完成
}uart_state_t;

typedef enum{
    DATA_TYPE_WRITE,       //写入
    DATA_TYPE_READ,        //读取
}uart_data_type_t;

extern uint8_t rx_data_len;
extern uint8_t rx_data_num;
extern uint8_t rx_data[RECMAXLEN];

extern uart_data_type_t uart_data_type;
extern uart_state_t uart_rx_state;

void uart_init(void);
void uart_rx_isr(void);
void uart_tx_isr(void);
uint8_t uart_read_len();
void uart_send_packet(uint8_t * packet, uint8_t len);
void send_nec_finish(void);
void send_unicersal_finish(void);

#endif
