#include "timer.h";
#include "uart.h";

uint16_t system_time = 0; //1ms��ʱ��
uint16_t over_time = 0;   //��ʱʱ��

 /*-------------------------------------------------
 * ��������tim4_init
 * ���ܣ�  ��ʼ��TIM1
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
void tim4_init(void)
{
    PCKEN |= 0B00001000;				//ʹ��TIMER4ģ��ʱ��
    TIM4CR1 = 0B00000101;				//�����Զ�װ�أ�ʹ�ܼ�����
    TIM4IER = 0B00000001;				//��������ж�
    TIM4SR = 0B00000000;	
    TIM4EGR = 0B00000000;	
    TIM4CNTR = 0;  
    TIM4PSCR = 0B00000110;			    //Ԥ��Ƶ����ֵ,64��Ƶ
    TIM4ARR = 248;  					//�Զ�װ��ֵ

    T4CEN = 0;                         //ʧ��TIMER4������
}

 /*-------------------------------------------------
 * ��������system_time_callback
 * ���ܣ�  ��ʱ��TIM4��ʱ�ص�����
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
static void system_time_callback(void)
{
    if (uart_rx_state == UART_STATE_DATA)    //���ݴ���ȱʧ��������
    {
        rx_data_len = 4;
        rx_data_num = 0;
    }
    uart_rx_state = UART_STATE_IDLE;
}

 /*-------------------------------------------------
 * ��������timer_isr
 * ���ܣ�  ��ʱ��TIM4�ж�
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
void timer_isr(void)
{
    system_time++;
    if (system_time == over_time)
    {
        T4CEN = 0;         //ʧ��TIMER4������
        system_time = 0;    //��ʱ����0
        system_time_callback();
    }
    
}

 /*-------------------------------------------------
 * ��������exit_system_timer
 * ���ܣ�  �˳���ʱ����ʱ
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
void exit_system_timer(void)
{
    T4CEN = 0;         //ʧ��TIMER4������
    system_time = 0;    //��ʱ����0
}

 /*-------------------------------------------------
 * ��������system_tick_exceed
 * ���ܣ�  ��ʱ��TIM4��ʱ������(1ms��ʱ)
 * ���룺  ��
 * �����  ��
 --------------------------------------------------*/
void system_tick_exceed(uint16_t time)
{
    if (T4CEN == 0)
    {
        T4CEN = 1;      //ʹ��TIMER4������
    }
    system_time = 0;    //��ʱ����0
    over_time = time;
}
