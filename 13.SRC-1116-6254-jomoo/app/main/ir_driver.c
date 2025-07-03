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
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
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


/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  ir driver init philips
 * @param  none
 * @return none
 * @retval void
 */
void rc_ir_init(void)
{
//only pwm0 support fifo mode
    // pwm_n_revert(PWM0_ID);    //if use PWMx_N, user must set "pwm_n_revert" before gpio_set_func(pwmx_N).
    gpio_set_func(GPIO_PB4, PWM0_N);
    pwm_set_mode(PWM0_ID, PWM_IR_DMA_FIFO_MODE);
    pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning
    pwm_set_cycle_and_duty(PWM0_ID, PWM_CARRIER_CYCLE_TICK,  PWM_CARRIER_CYCLE_TICK );     //config carrier: 38k, 1/3 duty

    pwm_set_dma_address(&T_dmaData_buf);

//logic_0, 560 us carrier,  560 us low
    waveform_logic_0_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 680 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_logic_0_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 480 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);

//logic_1, 560 us carrier,  1690 us low
    waveform_logic_1_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 680 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_logic_1_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 1440 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);


//start bit, 9000 us carrier,  4500 us low
    waveform_start_bit_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 4000 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_start_bit_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 8000 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
//stop bit,  560 us carrier, 500 us low
    waveform_stop_bit_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 680 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_stop_bit_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 42000 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);



//repeat signal  first part,  9000 us carrier, 2250 us low
    waveform_repeat_1st = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 9000 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    waveform_repeat_2nd = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 2250 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);

//repeat signal  second part,  560 us carrier, 99190 us low(110 ms - 9000us - 2250us - 560us = 99190 us)
    // waveform_repeat_3rd = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
    // waveform_repeat_4th = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 96190 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);


//add fifo stop irq, when all waveform send over, this irq will triggers
    //enable system irq PWM
    reg_irq_mask |= FLD_IRQ_SW_PWM_EN;

    //enable pwm0 fifo stop irq
    reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //clear irq status
//    reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;

}

void ir_send(u8 *data)
{
    if(ir_sending_check())
    {
        return;
    }

//// set waveform input in sequence //////
    T_dmaData_buf.data_num = 0;

    //waveform for start bit
    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_1st;
    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_2nd;

    for(int j = 0;j<6;j++){
        // printf("data[0] = %x\n\r",data[j]);
        for(int i=0;i<8;i++){
            if(data[j] & BIT((7-i))){
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
    }
    //waveform for stop bit
    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_stop_bit_1st;
    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_stop_bit_2nd;

    //waveform for start bit
    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_1st;
    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_2nd;

    for(int j = 0;j<6;j++){
        // printf("data[0] = %x\n\r",data[j]);
        for(int i=0;i<8;i++){
            if(data[j] & BIT((7-i))){
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
    }
    //waveform for stop bit
    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_stop_bit_1st;
    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_stop_bit_2nd;

    T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;

    reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
    reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask

    ir_send_ctrl.is_sending = IR_SENDING_DATA;

    ir_send_ctrl.sending_start_time = clock_time();

    pwm_start_dma_ir_sending();

}
void ir_send_release(void)
{
    u8 r = irq_disable();
    ir_send_ctrl.last_cmd = 0xff;

    ir_send_ctrl.is_sending = IR_SENDING_NONE;
    reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear irq status
    reg_pwm_irq_mask &= ~FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //disable irq mask

    irq_restore(r);

}

int ir_is_sending(void)
{
    if(ir_send_ctrl.is_sending && clock_time_exceed(ir_send_ctrl.sending_start_time, 300*1000))
    {
        ir_send_ctrl.is_sending = IR_SENDING_NONE;

        pwm_stop_dma_ir_sending();
    }
    return ir_send_ctrl.is_sending;
}

int ir_sending_check(void)
{
    u8 r = irq_disable();
    if(ir_is_sending()){
        irq_restore(r);
        return 1;
    }
    irq_restore(r);
    return 0;
}
void rc_ir_irq_prc(void)
{
    if(reg_pwm_irq_sta & FLD_IRQ_PWM0_IR_DMA_FIFO_DONE){
        reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //clear irq status
        //a stop come
        ir_send_release();
    }
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
