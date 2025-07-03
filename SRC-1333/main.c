#include	"SYSCFG.h"
#include 	"FT61F0AX.h"

#include "ir_module/ir_module.h"
#include "cli.h"
#include "uart.h"
#include "timer.h"

const uint8_t version[8] @ 0x1000 = { 2, 7, 12, 19, 0, 0, 0, 0 };

void interrupt ISR(void)
{
    if(IS_TIM1_INT())			
    {
        tim1_isr();
    }

    //��ʱ��4���жϴ������
    if(T4UIE&&T4UIF)			
    {
        T4UIF=1;					//д1�����־λ
        timer_isr();
    }

    if(UR1RXNE&&UR1RXNEF)				//�����ж�
    {
        uart_rx_isr();
        NOP();
	}
    
	if(UR1TCEN&&UR1TCF)					//�����ж�
    {
        UR1TCF=1;
        
        uart_tx_isr();
        NOP();
    }
}

void power_on_init(void)
{
    OSCCON = 0b01110001; // ϵͳʱ��ѡ��Ϊ�ڲ�����16MHz,��Ƶ��Ϊ1:1

    INTCON = 0b11000000; // ʹ��ȫ���ж�

    PORTA = 0b00000000;
    PORTB = 0b00000100;
    PORTC = 0b00000000;

    WPUA = 0b00000000; // �������Ŀ��أ�0-�أ�1-��
    WPUB = 0b00000000;
    WPUC = 0b00000000;

    WPDA = 0b00000000; // �������Ŀ��أ�0-�أ�1-��
    WPDB = 0b00000000;
    WPDC = 0b00000000;

    TRISA = 0b00000100; // ����������ã�0-�����1-����,TIM1_CH1,PA0-���
    TRISB = 0b00000000;
    TRISC = 0b00000000; // TIM1_CH1N,PC0-���

    PSRC0 = 0b11111111; // Դ�����������
    PSRC1 = 0b11111111;
    PSRC2 = 0b00001111;

    PSINK0 = 0b11111111; // ������������
    PSINK1 = 0b11111111;
    PSINK2 = 0b00000011;

    ANSELA = 0b00000000; // ���ö�Ӧ��IOΪ����IO

    AFP0 = 0b01010000;  //����USART1_TX����ΪPB6,TIM2_CH1����ΪPB0
    AFP1 = 0b01001000;  //����USART1_RX����ΪPA2,TIM1_CH2����ΪPB0
}

void DelayUs(uint8_t Time)
{
    uint8_t a;
    for(a = 0;a < Time; a++)
    {
        NOP();
    } 
}

void DelayMs(uint8_t Time)
{
    uint8_t a,b;
    for(a = 0; a < Time; a++)
    {
        for( b = 0; b<5; b++)
        {
            DelayUs(197);
        }
    }
}

void main(void)
{
    power_on_init();

    ir_protocol_init();

    uart_init();

    tim4_init();

    cli_init();

    while(1)
    {
        cli_task();
    }
}
