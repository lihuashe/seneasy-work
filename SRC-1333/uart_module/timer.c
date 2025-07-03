#include "timer.h";
#include "uart.h";

uint16_t system_time = 0; //1ms计时器
uint16_t over_time = 0;   //超时时间

 /*-------------------------------------------------
 * 函数名：tim4_init
 * 功能：  初始化TIM1
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void tim4_init(void)
{
    PCKEN |= 0B00001000;				//使能TIMER4模块时钟
    TIM4CR1 = 0B00000101;				//允许自动装载，使能计数器
    TIM4IER = 0B00000001;				//允许更新中断
    TIM4SR = 0B00000000;	
    TIM4EGR = 0B00000000;	
    TIM4CNTR = 0;  
    TIM4PSCR = 0B00000110;			    //预分频器的值,64分频
    TIM4ARR = 248;  					//自动装载值

    T4CEN = 0;                         //失能TIMER4计数器
}

 /*-------------------------------------------------
 * 函数名：system_time_callback
 * 功能：  定时器TIM4超时回调函数
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
static void system_time_callback(void)
{
    if (uart_rx_state == UART_STATE_DATA)    //数据传输缺失则丢弃数据
    {
        rx_data_len = 4;
        rx_data_num = 0;
    }
    uart_rx_state = UART_STATE_IDLE;
}

 /*-------------------------------------------------
 * 函数名：timer_isr
 * 功能：  定时器TIM4中断
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void timer_isr(void)
{
    system_time++;
    if (system_time == over_time)
    {
        T4CEN = 0;         //失能TIMER4计数器
        system_time = 0;    //计时器清0
        system_time_callback();
    }
    
}

 /*-------------------------------------------------
 * 函数名：exit_system_timer
 * 功能：  退出定时器计时
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void exit_system_timer(void)
{
    T4CEN = 0;         //失能TIMER4计数器
    system_time = 0;    //计时器清0
}

 /*-------------------------------------------------
 * 函数名：system_tick_exceed
 * 功能：  定时器TIM4计时器函数(1ms计时)
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void system_tick_exceed(uint16_t time)
{
    if (T4CEN == 0)
    {
        T4CEN = 1;      //使能TIMER4计数器
    }
    system_time = 0;    //计时器清0
    over_time = time;
}
