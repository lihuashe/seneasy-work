/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ir_driver.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "ir_driver.h"
#include "rc_ir.h"
#include "app_rcu.h"
#include "../key_module/key_handle.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
//  PHILIPS  protocol
#define T 444//444.44 us
u16 waveform_start_bit_1st;
u16 waveform_start_bit_2nd;
u16 waveform_stop_bit_1st;
u16 waveform_stop_bit_2nd;

u16 waveform_logic_0_1st;
u16 waveform_logic_0_2nd;
u16 waveform_logic_1_1st;
u16 waveform_logic_1_2nd;

u16 waveform_repeat_1st;
u16 waveform_repeat_2nd;
u16 waveform_repeat_3rd;
u16 waveform_repeat_4th;

extern u8 ir_hw_initialed;
/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
#if (TL_SZ119_TVSSU) 
extern unsigned char isIRCodeTxing(void);
#endif
extern void ir_dispatch(u8 type, u8 syscode, u8 ircode);
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  ir driver init philips
 * @param  none
 * @return none
 * @retval void
 */
void ir_driver_init_philips(void)
{
//only pwm0 support fifo mode
    pwm_n_revert(PWM0_ID);    //if use PWMx_N, user must set "pwm_n_revert" before gpio_set_func(pwmx_N).
    gpio_set_func(GPIO_PB3, AS_PWM0_N);
    pwm_set_mode(PWM0_ID, PWM_IR_DMA_FIFO_MODE);
    pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning
#if (!TL_SZ119_TVSSU) 
    pwm_set_cycle_and_duty(PWM0_ID, PWM_CARRIER_CYCLE_TICK,  PWM_CARRIER_HIGH_TICK );     //config carrier: 38k, 1/3 duty
    pwm_set_dma_address(&T_dmaData_buf);
#endif
//logic_0, 1T carrier,  1T low
    //逻辑零的低电平时间
    waveform_logic_0_1st = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, (T+4) * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    //逻辑0的高电平时间
    waveform_logic_0_2nd = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, (T+25) * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);

//logic_1, 1T carrier,  1T low
    //逻辑1的低电平时间
    waveform_logic_1_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, (T+10) * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    //逻辑1的高电平时间
    waveform_logic_1_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, (T+55) * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);

//start bit, 9000 us carrier,  4500 us low
    waveform_start_bit_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, (6*T) * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_start_bit_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, (2*T) * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);

//stop bit,  560 us carrier, 500 us low
    waveform_stop_bit_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_stop_bit_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 500 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);

//repeat signal  first part,  9000 us carrier, 2250 us low
    waveform_repeat_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 9000 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_repeat_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 2250 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
//repeat signal  second part,  560 us carrier, 99190 us low(110 ms - 9000us - 2250us - 560us = 99190 us)
    waveform_repeat_3rd = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_repeat_4th = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 99190 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);

//add fifo stop irq, when all waveform send over, this irq will triggers
    //enable system irq PWM
    reg_irq_mask |= FLD_IRQ_SW_PWM_EN;

    //enable pwm0 fifo stop irq
    reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //clear irq status
    ir_send_ctrl.last_cmd = 0xff; //must
}

/******************************************************************
 * @brief  ir driver send philips
 * @param  repeat data
 * @return none
 * @retval void
 */
void ir_driver_send_philips(u8 repeat, u8 data)
{
    _attribute_data_retention_ static bool toggle_bit =0;

    if(ir_send_ctrl.last_cmd != data)
    {
        if(ir_sending_check())
        {
            return;
        };

        ir_send_ctrl.last_cmd = data;

        //// set waveform input in sequence //////
        T_dmaData_buf.data_num = 0;

        //LEADER sysbol , 10T(8+2) total
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_2nd;

        //one symbol (included in leader)
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_2nd;

        // 3 mode bits  000 , 6T
        T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_2nd;
        T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_2nd;
        T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_2nd;

        //toggle bit(trailer), 2T H + 2T L == 4T
        if(toggle_bit){
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_1_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_1_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_1_2nd;
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_1_2nd;
        }
        else{
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_2nd;
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_2nd;
        }
        toggle_bit = !toggle_bit;

        //custom , 1 byte, which TV is all 000, (control field)
        for(int i=0;i<8;i++){
            //waveform for logic_0
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num++] = waveform_logic_0_2nd;
        }

        //data in , 1 byte  (infomation field)
        for(int i=7;i>=0;i--){ // do bit swap
            if(data & BIT(i)){
                //waveform for logic_1
                T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_1st;
                T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_2nd;
            }
            else{
                //waveform for logic_0
                T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
                T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;
            }
        }

        //backup frame for repeat
        for(int k=0 ;k<T_dmaData_buf.data_num;k++)
        {
            T_FirstdmaData_buf.data[k]=T_dmaData_buf.data[k];
        }
        T_FirstdmaData_buf.data_num = T_dmaData_buf.data_num;

        T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;

        if(repeat){  //need repeat
            ir_send_ctrl.repeat_enable = 1;  //need repeat signal
        }
        else{  //no need repeat
            ir_send_ctrl.repeat_enable = 0;  //no need repeat signal
        }

        reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
        reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask

        ir_send_ctrl.is_sending = IR_SENDING_DATA;

        ir_send_ctrl.sending_start_time = clock_time();

        pwm_start_dma_ir_sending();

    }
}

/******************************************************************
 * @brief  ir dispatch philips
 * @param  type repeat ircode
 * @return none
 * @retval void
 */
void ir_dispatch_philips(u8 type, u8 repeat ,u8 ircode)
{
    printf("--->>> ircode == 0x%02X\r\n",ircode);
#if (TL_SZ119_TVSSU)
    unsigned char busy = isIRCodeTxing();
    if(busy) {
        //extern unsigned char irRepeatCount;
        // extern DMASTRUCTDATA dma;
        //omni_TVSSU_printf("Co SSU busy:%d,repeat:%X\r\n",busy,irRepeatCount);
        return;
    }
#endif

    if(!ir_hw_initialed){
        ir_hw_initialed = 1;
        ir_driver_init_philips();
    }
#if (TL_SZ119_TVSSU)     
    pwm_set_cycle_and_duty(PWM0_ID, PWM_CARRIER_CYCLE_TICK,  PWM_CARRIER_HIGH_TICK );   //config carrier: 38k, 1/3 duty
    pwm_set_dma_address(&T_dmaData_buf);
#endif
    if(type == TYPE_IR_SEND){
        ir_driver_send_philips(repeat, ircode);
    }
    else if(type == TYPE_IR_RELEASE){
        ir_send_release();
    }
}

/******************************************************************
 * @brief  ir timer stop code
 * @param  int
 * @return none
 * @retval void
 */
int ir_timer_stop_code(void)
{
    if(ir_send_ctrl.is_sending)
    {
        ir_dispatch(TYPE_IR_RELEASE, 0, 0);  //release
        if(batt_poor_flag == 0x01)
            process_batt_poor();
        if(tick_key_source == 0x02)
        {
            tick_key_source = 0x00;
            app_rcu_enter_pair();
        }
    }
    return -1; 
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
