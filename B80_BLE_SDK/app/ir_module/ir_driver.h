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
#include "../app_config.h"
#include "../../tl_common.h"
#include "../../sdk/chip/B80/drivers/pwm.h"
#include "../../sdk/chip/B80/drivers/irq.h"

#if(IR_MODULE_ENABLE)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define TYPE_IR_SEND            1
#define TYPE_IR_RELEASE         2

#define IR_SENDING_NONE         0
#define IR_SENDING_DATA         1
#define IR_SENDING_REPEAT       2
#define IR_SENDING_STOP         3

#define PWM_IR_MAX_NUM          300     //user can define this max number according to application
typedef struct{
    unsigned int    dma_len;            // dma len
    unsigned short  data[PWM_IR_MAX_NUM];
    unsigned int    data_num;
}pwm_dma_data_t;
pwm_dma_data_t T_dmaData_buf;
pwm_dma_data_t T_FirstdmaData_buf;
extern u8 ir_hw_initialed;

typedef struct{
    u8 is_sending;
    u8 repeat_enable;
    u8 last_cmd;
    u8 rsvd;
    u8 is_full_sequence;
    u32 sending_start_time;
}ir_send_ctrl_t;
ir_send_ctrl_t ir_send_ctrl;

//16 KEY
#define IR_POWER               0x18
#define IR_OK                  0x55
#define IR_UP                  0x46 
#define IR_DOWN                0x16 
#define IR_LEFT                0x47 
#define IR_RIGHT               0x15 
#define IR_BACK                0x0A
#define IR_DB_WINDOWS          0x0F
#define IR_SEL_BUTTON          0x1F 
#define IR_FLAME               0x40 
#define IR_FREEZE_SCREEN       0x12 
#define IR_DISABLE_TOUCH       0x0E
#define IR_VOLUME_UP           0x14 
#define IR_VOLUME_DOWN         0x10 
#define IR_INPUT_SELECT        0x0D 
#define IR_SETTING             0x25

/*============================================================================*
 *                          Functions
 *============================================================================*/
void ir_driver_init(void);
void rc_ir_irq_prc(void);
void ir_send_release(void);
void ir_dispatch(u8 type, u8 repeat ,u8 ircode);

#endif
#endif /* IR_DRIVER_H_ */
