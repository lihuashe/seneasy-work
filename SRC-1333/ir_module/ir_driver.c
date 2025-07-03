/**
 * @attention
 * 
 *  Copyright 2024 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
 * @file     ir_driver.c
 * 
 * @author   �ſ���(zhangkaihua@seneasy.com)
 * 
 * @brief    
 * 
 * @date     2024-11-19
 * 
*/

#include "ir_driver.h"

volatile union 
{
    struct 
    {
        uint8_t waveform        :1;        // 1,���ڷ��Ͳ���;0,����
        uint8_t resv1           :1;
        uint8_t resv2           :1;
        uint8_t resv3           :1;
        uint8_t resv4           :1;
        uint8_t resv5           :1;
        uint8_t resv6           :1;
        uint8_t resv7           :1;
    };
}ir_flags @ 0x70;   // ����Bank SRAM


ir_waveform_t ir_wave;
ir_data_t ir_data;
ir_code_state_t ir_code;
ir_change_key_t ir_change_key;
ir_send_state_t ir_send_state;

inline void ir_output_waveform(uint8_t code)
{   
    uint16_t pulse_h, pulse_l;
    uint8_t i;
    i = code;

    i = i << 1;
    pulse_h = ir_data.waveform_spec[i];
    i++;
    pulse_l = ir_data.waveform_spec[i];

    pulse_l = pulse_h + pulse_l - 0x20;
    TIM1ARRH = (uint8_t)(pulse_l >> 8 & 0x00FF);
    TIM1ARRL = (uint8_t)(pulse_l & 0x00FF);
    
    TIM1CCR1H = (uint8_t)(pulse_h >> 8 & 0x00FF); 
    TIM1CCR1L = (uint8_t)(pulse_h & 0x00FF);

    if (pulse_h)
    {
        TIM2CCMR1 = 0b01101000; // CH3 pwm1 ģʽ
        ir_flags.waveform = 1;
    }
    else
    {
        TIM2CCMR1 = 0b01001000; // CH3 ǿ����Ч(OC3REF = 0)
        ir_flags.waveform = 0;
    }

    T1CEN = 1;
}

inline void ir_output_delay(uint16_t delay)
{
    TIM2CCMR1 = 0b01001000; // CH3 ǿ����Ч(OC3REF = 0)
    TIM1CCR1H = (uint8_t)(delay >> 8 & 0x00FF);
    TIM1CCR1L = (uint8_t)(delay & 0x00FF);

    TIM1ARRH = (uint8_t)(delay >> 8 & 0x00FF);
    TIM1ARRL = (uint8_t)(delay & 0x00FF);  

    T1CEN = 1;
}

inline void ir_output_logic(uint8_t index)
{
    uint16_t pulse_h, pulse_l;
    
    if ((index & 0xF0) == 0xF0)
    {
        pulse_h = ir_wave.pulse[ir_wave.pulse_idx++];
    }
    else
    {
        pulse_h = ir_wave.dict[(uint8_t)(index >> 4)];
    }

    if ((index & 0x0F) == 0x0F)
    {
        pulse_l = ir_wave.pulse[ir_wave.pulse_idx++];
    }
    else
    {
        pulse_l = ir_wave.dict[(uint8_t)(index & 0x0F)];
    }

    if (pulse_l > 0x03)
    {
        pulse_l = pulse_h + pulse_l - 0x03;
    }
    else
    {
        pulse_l = pulse_h + pulse_l;
    }
    TIM1ARRH = (uint8_t)(pulse_l >> 8 & 0x00FF);
    TIM1ARRL = (uint8_t)(pulse_l & 0x00FF);
    
    TIM1CCR1H = (uint8_t)(pulse_h >> 8 & 0x00FF); 
    TIM1CCR1L = (uint8_t)(pulse_h & 0x00FF);

    if (pulse_h)
    {
        TIM2CCMR1 = 0b01101000; // CH3 pwm1 ģʽ
        ir_flags.waveform = 1;
    }
    else
    {
        TIM2CCMR1 = 0b01001000; // CH3 ǿ����Ч(OC3REF = 0)
        ir_flags.waveform = 0;
    }

    T1CEN = 1;
}

void tim1_isr()
{
    // Timer1 ch1 ƥ��
    if (T1CC1IF)
    {
        T1CC1IF = 1;
        TIM2CCMR1 = 0b01001000; // CH3 ǿ����Ч(OC3REF = 0)
    }
    // Timer1 �����¼��ж�
    if (T1UIF)
    {
        T1UIF = 1;

        if (ir_data.state == IR_STATE_COMMAND)
        {
            if (ir_data.command_idx < ir_data.command_len)
            {
                ir_data.code = ir_data.command[ir_data.command_idx];
                if (ir_data.code == 0x80)
                {
                    if (ir_data.data_idx < ir_data.data_len)
                    {
                        uint8_t d = ir_data.data[(uint8_t)(ir_data.data_idx >> 3)];
                        uint8_t mask = 0x01 << (ir_data.data_idx & 0x07);
                        if (d & mask) 
                        {
                            ir_data.code = 1;
                        }
                        else
                        {
                            ir_data.code = 0;
                        }
                        ir_data.data_idx++;

                        // ���һ�����ݷ���,�������ݽ׶�
                        if(ir_data.data_idx == ir_data.data_len)
                        {
                            ir_data.command_idx++;
                        }
                    }
                }
                else
                {
                    ir_data.command_idx++;
                }

                ir_output_waveform(ir_data.code);
            }
            else
            {
                if (ir_data.pending_stop) 
                {
                    ir_data.state = IR_STATE_IDLE;
                    return;
                }

                ir_data.state = IR_STATE_REPEAT;
            }
        }

        if(ir_data.state == IR_STATE_REPEAT) 
        {
            if ((ir_data.repeat_idx == 0) && (ir_data.pending_stop == true))
            {
                ir_data.state = IR_STATE_IDLE;
                if (ir_change_key == IR_CHANGE_COMMAND)
                {
                    ir_change_key = IR_CHANGE_SUCCESS;
                }
                return ;
            }

            ir_data.code = ir_data.repeat[ir_data.repeat_idx++];
            if (ir_data.repeat_idx >= ir_data.repeat_len) 
            {
                ir_data.repeat_idx = 0;
            }
            ir_output_waveform(ir_data.code);
            ir_send_state = IR_NEC_SEND_FINISH;
        }

        if (ir_wave.state == IR_STATE_COMMAND)
        {
            if (ir_wave.command_idx < ir_wave.command_len)
            {
                ir_output_logic(ir_wave.command[ir_wave.command_idx]);
                ir_wave.command_idx++;
            }
            else
            {
                if (ir_wave.frames != 0)
                {
                    ir_wave.state = IR_STATE_DELAY;
                    ir_output_delay(ir_wave.delay);
                    return;

                }
                else
                {
                    ir_wave.state = IR_STATE_IDLE;
                    ir_send_state = IR_UNIVERSAL_SEND_FINISH;
                }
            }
        }

        if (ir_wave.state == IR_STATE_DELAY)
        {
            if (ir_wave.frames != 0)
            {
                ir_wave.frames--;

                // ��������
                ir_wave.state = IR_STATE_COMMAND;
                ir_wave.command_idx = 0;
                ir_wave.pulse_idx = 0;
                ir_start_send();
            }
            else
            {
                ir_wave.state = IR_STATE_IDLE;
                ir_send_state = IR_UNIVERSAL_SEND_FINISH;
            }
        }
    }
}

/**
 * @brief
 *
 */
void ir_config_carrier(uint16_t frequence)
{
    uint16_t period = (32000000 + (frequence - 1)) / frequence;

    /** 1. ����Timer1 + Timer2ʱ��Դ */
    PCKEN |= 0b00000110; // ʹ��Timer1,Timer2ģ��ʱ��
    CKOCON = 0b00100000; // Timer1��Ƶʱ��ռ�ձȵ���λ4ns�ӳ�
    TCKSRC = 0b00110011; // Timer1,Timer2ʱ��ԴΪHIRC��2��Ƶ

    /** 2. ����Timer2����ģʽ
     * TIM1CR1
     *   b7   - ���������Զ�Ԥװ�� = 1,enable
     *   b6:5 - ����������ģʽ = 00, ���ض���ģʽ
     *   b4   - �������������� = 0, ����   
     *   b3   - ������ģʽ = 0,�ر�
     *   b2   - �����¼�Դ = 1, ����������/����
     *   b1   - ������������¼����� = 0, ʹ��
     *   b0   - TIM2��ʱ�� = 0, �ر�
     */
    TIM2CR1 = 0b10000000; // �����Զ�װ�أ�ʹ�ܼ�����

    TIM2IER  = 0b00000000; // ����Ҫ�ж�

    TIM2SR1 = 0b00000000;
    TIM2SR2 = 0b00000000;

    TIM2EGR = 0b00000000;

    /** 3. ����Timer1 CH3 PWMģʽ1 
     * TIM1CCMR3
     *   b7   - T1OC3CE ͨ��3����ο��ź���ETR������� = 0, �ر�
     *   b6:4 - T1OC3M ͨ��3����Ƚ�ģʽ��OC3REF���� = 110, PWM1ģʽ;
     *   b3   - T1OC3PE ͨ��3����Ƚ�ֵ���Զ�Ԥװ�� = 1, ʹ��
     *   b2   - T1OC3FE ͨ��3����źſ���ʹ�� = 0, �ر�
     *   b1:0 - T1CC3S ͨ��3ģʽ = 00,���
    */
    TIM2CCMR1 = 0b00000000;
    TIM2CCMR2 = 0b00000000;
    TIM2CCMR3 = 0b00000000;
    
    /** 4. ����Timer1 ���
     * TIM2CCR1
     *  b7:6   - N/A 
     *  b5   - T2CC2P  ͨ��2������� = 1, �͵�ƽ
     *  b4   - T2CC2E  ͨ��2������Ź��� = 1, ʹ��
     *  b3:2   - N/A 
     *  b1   - T2CC1P  ͨ��1������� = 1, �͵�ƽ
     *  b0   - T2CC1E  ͨ��1������Ź��� = 1, ʹ��
     */
    TIM2CCER1 = 0b00000001; // ͨ��1ʹ�����������������͵�ƽ��Ч

    /** TIM2CCR2
     *  b7:2   - N/A 
     *  b1   - T2CC3P  ͨ��1�������
     *  b0   - T2CC3E  ͨ��1������Ź���
     */
    TIM2CCER2 = 0b00000000;

    TIM2CNTRH = 0;
    TIM2CNTRL = 0;

    TIM2ARRH = (uint8_t)(period >> 8 & 0x00FF); // �Զ�װ�ظ�8λ03H
    TIM2ARRL = (uint8_t)(period & 0x00FF); // �Զ�װ�ص�8λe8H

    uint16_t duty = (period + 2) / 3;
    TIM2CCR1H = (uint8_t)(duty >> 8 & 0x00FF);    // PWM���� = 1/3
    TIM2CCR1L = (uint8_t)(duty & 0x00FF);

    TIM2PSCR = 0b00000000; // ����Ƶ
}

void ir_config_timer(uint16_t clk_khz)
{
    TIM1CR1 = 0b00001000;    // ������ģʽ
    TIM1CR2 = 0b10000000;    

    TIM1SMCR = 0;

    TIM1ETR = 0;

    TIM1SR1 = 0;
    TIM1SR2 = 0;

    TIM1CCMR1 = 0b01100000;
    TIM1CCMR2 = 0;
    TIM1CCMR3 = 0;
    TIM1CCMR4 = 0;

    TIM1CCER1 = 0;
    TIM1CCER2 = 0;

    // Timer1 CK_CNT = 2*HIRC/clk_khz = 32000 / clk_khz 
    uint16_t psc = (32000 + (clk_khz -1) ) / clk_khz;
    TIM1PSCRH = (uint8_t)(psc >> 8 & 0x00FF);
    TIM1PSCRL = (uint8_t)(psc & 0x00FF);    

    TIM1CNTRH = 0;
    TIM1CNTRL = 0;

    TIM1ARRH = 0xFF;
    TIM1ARRL = 0xFF;

    TIM1CCR1H = 0xFF;
    TIM1CCR1L = 0xFF;

    TIM1RCR = 0;

    TIM1IER = 0b00000011;
}

void ir_start_send()
{
    if (ir_data.state == IR_STATE_COMMAND)
    {
        ir_data.code = ir_data.command[ir_data.command_idx];
        ir_data.command_idx++;
        ir_output_waveform(ir_data.code);
        return ;
    }

    if (ir_wave.state == IR_STATE_COMMAND)
    {
        ir_wave.command_idx++;
        ir_output_logic(ir_wave.command[0]);
    }
}

void ir_stop_send()
{
    ir_data.pending_stop = true;
}
