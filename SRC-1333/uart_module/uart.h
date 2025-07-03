#ifndef _UART_H
#define _UART_H

#include "SYSCFG.h";
#include "stdint.h";
#include "stdbool.h";
#include "timer.h";
#include "FT61F0AX.h";

#define RECMAXLEN   256  /* ���������ܳ��� */
#define SENDMAXLEN  99  /* ���������ܳ��� */

typedef enum{
    UART_STATE_IDLE,       //����
    UART_STATE_HEAD,       //֡ͷ
    UART_STATE_TYPE,       //��������
    UART_STATE_LEN,        //���ݳ���
    UART_STATE_DATA,       //����
    UART_STATE_SUCCESS,    //�����������
}uart_state_t;

typedef enum{
    DATA_TYPE_WRITE,       //д��
    DATA_TYPE_READ,        //��ȡ
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
