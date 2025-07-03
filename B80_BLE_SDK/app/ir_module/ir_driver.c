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
#include "../charge_module/lp4081b_driver.h"
#include "../led_module/pwm_handle.h"
#include "../led_module/led_driver.h"
#include "../keyboard_rcu.h"

#if(IR_MODULE_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#define PWM_PIN         GPIO_PB4
#define AS_PWMx         PWM0
#define PWM_ID			PWM0_ID

#define T                               444//444.44 us
#define IR_DMA_CARRIER_FREQ				37900//PHILIPS: 38000  NEC:37900
#define IR_DMA_MAX_TICK					(CLOCK_SYS_CLOCK_HZ/IR_DMA_CARRIER_FREQ)
#define IR_DMA_CMP_TICK					(IR_DMA_MAX_TICK/3)//PHILIPS: IR_DMA_MAX_TICK/2  NEC:IR_DMA_MAX_TICK/3

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

u8 ir_hw_initialed = 0;
u16 waveform_wait_to_repeat;

void ir_driver_send_PHILIPS(u8 repeat, u8 data);
void ir_driver_send_NEC(u8 addr1, u8 addr2, u8 ircode);
int ir_is_sending(void);
int ir_sending_check(void);
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  ir_driver_init
 * @param  none
 * @return none
 * @retval void
 */
void ir_driver_init(void)
{
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
	gpio_set_func(PWM_PIN, AS_PWMx);
	pwm_set_mode(PWM_ID, PWM_IR_DMA_FIFO_MODE);
	pwm_set_cycle_and_duty(PWM_ID, IR_DMA_MAX_TICK, IR_DMA_CMP_TICK);
    pwm_set_dma_address(&T_dmaData_buf);

    //NEC
	waveform_logic_0_1st    = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560  * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_logic_0_2nd    = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 560  * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_logic_1_1st    = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560  * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_logic_1_2nd    = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 1690 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);

	waveform_start_bit_1st  = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 9000 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_start_bit_2nd  = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 4500 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_stop_bit_1st   = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560  * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_stop_bit_2nd   = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 500  * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);

	waveform_repeat_1st     = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 9000 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_repeat_2nd     = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 2250 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_repeat_3rd     = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560  * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
	waveform_repeat_4th     = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 99190* CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);

#if 0 //PHILIPS
    waveform_logic_0_1st    = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, (T + 4)  * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_logic_0_2nd    = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, (T + 25) * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_logic_1_1st    = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, (T + 10) * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_logic_1_2nd    = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, (T + 55) * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);

    waveform_start_bit_1st  = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, (6 * T) * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_start_bit_2nd  = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, (2 * T) * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_stop_bit_1st   = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_stop_bit_2nd   = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 500 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);

    waveform_repeat_1st     = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 9000 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_repeat_2nd     = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 2250 * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_repeat_3rd     = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 560  * CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
    waveform_repeat_4th     = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, 99190* CLOCK_SYS_CLOCK_1US / IR_DMA_MAX_TICK);
#endif

	pwm_set_interrupt_enable(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE);
	irq_set_mask(FLD_IRQ_SW_PWM_EN);
	irq_enable();
}

/******************************************************************
 * @brief  rc_ir_irq_prc
 * @param  none
 * @return none
 * @retval void
 */
void rc_ir_irq_prc(void)
{
    if(reg_pwm_irq_sta & FLD_IRQ_PWM0_IR_DMA_FIFO_DONE){
        reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //clear irq status
        if(ir_send_ctrl.repeat_enable){

            /* PHILIPS */
            #if 0
            if(ir_send_ctrl.is_sending == IR_SENDING_DATA){
                ir_send_ctrl.is_sending = IR_SENDING_REPEAT;

                T_dmaData_buf.data_num = 0;

                u32 tick_2_repeat_sysClockTimer16M = 107 * CLOCK_16M_SYS_TIMER_CLK_1MS - (clock_time() - ir_send_ctrl.sending_start_time);
                u32 tick_2_repeat_sysTimer = (tick_2_repeat_sysClockTimer16M * CLOCK_SYS_CLOCK_1US >> 4);

                waveform_wait_to_repeat = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, tick_2_repeat_sysTimer / IR_DMA_MAX_TICK);

                T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_wait_to_repeat;
                T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;

                pwm_start_dma_ir_sending();
            }
            else if(ir_send_ctrl.is_sending == IR_SENDING_REPEAT){

                T_dmaData_buf.data_num = 0;
                for (int j = 0; j < T_FirstdmaData_buf.data_num; j++) {
                    T_dmaData_buf.data[T_dmaData_buf.data_num ++] = T_FirstdmaData_buf.data[j];
                }
                T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_wait_to_repeat;
                T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;
                pwm_start_dma_ir_sending();
            }
            #endif

            /* NEC */
			if(ir_send_ctrl.is_sending == IR_SENDING_DATA){
				ir_send_ctrl.is_sending = IR_SENDING_REPEAT;

				T_dmaData_buf.data_num = 0;

				u32 tick_2_repeat_sysClockTimer16M = 108*CLOCK_16M_SYS_TIMER_CLK_1MS - (clock_time() - ir_send_ctrl.sending_start_time);
				u32 tick_2_repeat_sysTimer = (tick_2_repeat_sysClockTimer16M * CLOCK_SYS_CLOCK_1US>>4);

				waveform_wait_to_repeat = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, tick_2_repeat_sysTimer / IR_DMA_MAX_TICK);

				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_wait_to_repeat;

				for(int i=0; i<5; i++){
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_1st;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_2nd;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_3rd;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_4th;
				}

				T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;
				pwm_start_dma_ir_sending();
			}
			else if(ir_send_ctrl.is_sending == IR_SENDING_REPEAT){

				T_dmaData_buf.data_num = 0;
				for(int i=0; i<5; i++){
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_1st;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_2nd;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_3rd;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_4th;
				}

				T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;
				pwm_start_dma_ir_sending();
			}            
        }
        else{
            ir_send_release();
        }
    }
}

/******************************************************************
 * @brief  ir_dispatch
 * @param  type repeat ircode
 * @return none
 * @retval void
 */
void ir_dispatch(u8 type, u8 repeat ,u8 ircode)
{
    if(!ir_hw_initialed){
        ir_hw_initialed = 1;
        ir_driver_init();
    }
    if(type == TYPE_IR_SEND){
        /* PHILIPS */
        //ir_driver_send_PHILIPS(repeat, ircode);

        /* NEC */
        u8 addr1 = 0x01;
	    u8 addr2 = 0xFE;
        ir_driver_send_NEC(addr1, addr2, ircode);
    }
    else if(type == TYPE_IR_RELEASE){
        ir_send_release();
    }
}

/******************************************************************
 * @brief  ir_driver_send_PHILIPS
 * @param  repeat data
 * @return none
 * @retval void
 */
void ir_driver_send_PHILIPS(u8 repeat, u8 data)
{
    static bool toggle_bit =0;
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
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;

        //toggle bit(trailer), 2T H + 2T L == 4T
        if(toggle_bit){
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_2nd;
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_2nd;
        }
        else{
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;
        }
        toggle_bit = !toggle_bit;

        //custom , 1 byte, which TV is all 000, (control field)
        for(int i=0; i<8; i++){
            //waveform for logic_0
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
            T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;
        }

        //data in , 1 byte  (infomation field)
        for(int i=7; i>=0; i--){ // do bit swap
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
        for(int k=0; k < T_dmaData_buf.data_num; k++)
        {
            T_FirstdmaData_buf.data[k] = T_dmaData_buf.data[k];
        }
        T_FirstdmaData_buf.data_num = T_dmaData_buf.data_num;
        T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;

        if(repeat){  //need repeat
            ir_send_ctrl.repeat_enable = 1;  //need repeat signal
        }
        else{  //no need repeat
            ir_send_ctrl.repeat_enable = 0;  //no need repeat signal
        }

        reg_pwm_irq_sta   = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
        reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask
        ir_send_ctrl.is_sending = IR_SENDING_DATA;
        ir_send_ctrl.sending_start_time = clock_time();

        pwm_start_dma_ir_sending();
    }
}

/******************************************************************
 * @brief  ir_driver_send_NEC
 * @param  repeat data
 * @return none
 * @retval void
 */
void ir_driver_send_NEC(u8 addr1, u8 addr2, u8 cmd)
{
    if(ir_send_ctrl.last_cmd != cmd)
    {
        if(ir_sending_check())
        {
            return;
        };
        ir_send_ctrl.last_cmd = cmd;

        //// set waveform input in sequence //////
        T_dmaData_buf.data_num = 0;

        //LEADER sysbol , 10T(8+2) total
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_1st;
        T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_2nd;

		//add data
		u32 data = (~cmd) << 24 | cmd << 16 | addr2 << 8 | addr1;
		for(int i=0; i<32; i++){
			if(data & BIT(i)){
				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_1st;
				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_2nd;
			}
			else{
				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;
			}
		}

		T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_stop_bit_1st;
		T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_stop_bit_2nd;

        T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;

        if(1){  //need repeat
            ir_send_ctrl.repeat_enable = 1;  //need repeat signal
        }
        else{  //no need repeat
            ir_send_ctrl.repeat_enable = 0;  //no need repeat signal
        }

        reg_pwm_irq_sta   = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
        reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask
        ir_send_ctrl.is_sending = IR_SENDING_DATA;
        ir_send_ctrl.sending_start_time = clock_time();

        pwm_start_dma_ir_sending();
    }
}

/******************************************************************
 * @brief  ir_send_release
 * @param  none
 * @return none
 * @retval void
 */
void ir_send_release(void)
{
    u8 r = irq_disable();
    ir_send_ctrl.last_cmd = 0xff;

    /* NEC */
	if(ir_send_ctrl.is_sending != IR_SENDING_NONE){
		pwm_stop_dma_ir_sending();
	}

    ir_send_ctrl.is_sending = IR_SENDING_NONE;
    reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear irq status
    reg_pwm_irq_mask &= ~FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //disable irq mask
    irq_restore(r);

#if (CHARGE_ENABLE)
    if(user_led_blink_flag == 0x01)
        led_pwm_init();
#endif
    if(app_led_2_blink_timer == 0)
        gpio_write(GPIO_LED_2, !LED_ON_LEVAL);
}

/******************************************************************
 * @brief  ir_is_sending / ir_sending_check
 * @param  none
 * @return none
 * @retval void
 */
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

#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
