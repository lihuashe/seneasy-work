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

    //定时器4的中断处理程序
    if(T4UIE&&T4UIF)			
    {
        T4UIF=1;					//写1清零标志位
        timer_isr();
    }

    if(UR1RXNE&&UR1RXNEF)				//接收中断
    {
        uart_rx_isr();
        NOP();
	}
    
	if(UR1TCEN&&UR1TCF)					//发送中断
    {
        UR1TCF=1;
        
        uart_tx_isr();
        NOP();
    }
}

void power_on_init(void)
{
    OSCCON = 0b01110001; // 系统时钟选择为内部振荡器16MHz,分频比为1:1

    INTCON = 0b11000000; // 使能全局中断

    PORTA = 0b00000000;
    PORTB = 0b00000100;
    PORTC = 0b00000000;

    WPUA = 0b00000000; // 弱上拉的开关，0-关，1-开
    WPUB = 0b00000000;
    WPUC = 0b00000000;

    WPDA = 0b00000000; // 弱下拉的开关，0-关，1-开
    WPDB = 0b00000000;
    WPDC = 0b00000000;

    TRISA = 0b00000100; // 输入输出设置，0-输出，1-输入,TIM1_CH1,PA0-输出
    TRISB = 0b00000000;
    TRISC = 0b00000000; // TIM1_CH1N,PC0-输出

    PSRC0 = 0b11111111; // 源电流设置最大
    PSRC1 = 0b11111111;
    PSRC2 = 0b00001111;

    PSINK0 = 0b11111111; // 灌电流设置最大
    PSINK1 = 0b11111111;
    PSINK2 = 0b00000011;

    ANSELA = 0b00000000; // 设置对应的IO为数字IO

    AFP0 = 0b01010000;  //配置USART1_TX引脚为PB6,TIM2_CH1引脚为PB0
    AFP1 = 0b01001000;  //配置USART1_RX引脚为PA2,TIM1_CH2引脚为PB0
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
