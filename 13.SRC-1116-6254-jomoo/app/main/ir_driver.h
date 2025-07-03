/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ir_driver.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef IR_DRIVER_H_
#define IR_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "AAA_project_config.h"

#define IR_SENDING_NONE          0
#define IR_SENDING_DATA          1
#define IR_SENDING_REPEAT        2
#define IR_SENDING_STOP          3

#define PWM_IR_MAX_NUM    300     //user can define this max number according to application
typedef struct{
    unsigned int dma_len;        // dma len
    unsigned short data[PWM_IR_MAX_NUM];
    unsigned int   data_num;
}pwm_dma_data_t;

pwm_dma_data_t T_dmaData_buf;

typedef struct{
    u8 is_sending;
    u8 repeat_enable;
    u8 last_cmd;
    u8 rsvd;
    u8 is_full_sequence;
    u32 sending_start_time;
}ir_send_ctrl_t;

ir_send_ctrl_t ir_send_ctrl;

#define IR_CARRIER_FREQ                    38000      // 1 frame -> 1/38k -> 1000/38 = 26 us
#define PWM_CARRIER_CYCLE_TICK            ( CLOCK_SYS_CLOCK_HZ/IR_CARRIER_FREQ )  //16M: 421 tick, f = 16000000/421 = 38004,T = 421/16=26.3125 us
#define PWM_CARRIER_HIGH_TICK             ( PWM_CARRIER_CYCLE_TICK/3 )   // 1/3 duty

#define PWM_CARRIER_HALF_CYCLE_TICK       ( PWM_CARRIER_CYCLE_TICK>>1 )
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define TYPE_IR_SEND               1
#define TYPE_IR_RELEASE            2
/*============================================================================*
 *                          Functions
 *============================================================================*/
void rc_ir_init(void);
void ir_send(u8 *data);
void ir_send_release(void);
int ir_sending_check(void);
int ir_is_sending(void);
void rc_ir_irq_prc(void);
#endif /* IR_DRIVER_H_ */
