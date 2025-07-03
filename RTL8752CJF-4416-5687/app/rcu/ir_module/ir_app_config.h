/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file         ir_app_config.h
* @brief
* @details
* @author       yuyin_zhang
* @date         2017-11-28
* @version      v1.0
* *********************************************************************************************************
*/

#ifndef __IR_APP_CONFIG_H_
#define __IR_APP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "smartrcu_handle.h"
/*============================================================================*
 *                          IR Common config
 *============================================================================*/
typedef enum
{
    IR_ATTR_INVALID = 0,
    IR_ATTR_OFFSET,
    IR_ATTR_FREQ,
    IR_ATTR_DUTY,
} IR_ATTR;

bool is_ir_send_working(void);
bool is_ir_learn_working(void);
bool ir_learn_get_attr(IR_ATTR irAttr, void *attrValue, uint32_t key_index);

/*============================================================================*
 *                          IR Send config
 *============================================================================*/
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)

#define IR_SEND_ADDR                0x1d02  /*IR Send Local Address*/
#define IR_SEND_FREQUENCY           38    /*ir send frequency*/

#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)

#define IR_SEND_ADDR                0x7F  /*IR Send Local Address*/
#define IR_SEND_FREQUENCY           38    /*ir send frequency*/

#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)

#define IR_SEND_ADDR                0x4040  /*IR Send Local Address*/
#define IR_SEND_FREQUENCY           38    /*ir send frequency*/

#endif

#define IR_TX_FIFO_THR_LEVEL            2

typedef enum
{
    IR_SEND_IDLE,
    IR_SEND_CAMMAND = 1,
    IR_SEND_REPEAT_CODE,

    IR_SEND_NOT_COMPLETED,
    IR_SEND_COMPLETED,

    IR_SEND_KEY_PRESS,
    IR_SEND_KEY_RELEASE,
} _IR_SEND_STATE_;

typedef struct
{
    uint8_t irSendStage;            /*ir send cammand or repeate*/
    uint8_t isIrSendComplete;       /*ir send send complete flag*/
    uint8_t irSendKeyState;         /*ir send key state, press or release*/
    float   irSendFrequency;        /*ir send frequency*/
    uint32_t irLearnDataOffset;      /*ir learn data offset*/
    uint8_t irSendkeyData;
} IR_SendStruct;

bool ir_send_msg_proc(uint16_t msg_sub_type);
void ir_send_key_press_handle(uint32_t key_index_1, uint32_t key_index_2, uint32_t key_length);
void ir_send_key_release_handle(void);
bool ir_send_check_dlps(void);
void ir_send_exit(void);

/*============================================================================*
 *                          IR Learn config
 *============================================================================*/

#define  INVALID_VALUE                  0xff
#define  INVALID_KEY                    0xff
#define  INVALID_OFFSET                 0xff
#define  INVALID_FREQUENCY              0x00
#define IR_LEARN_TIMEOUT                (20*1000)  /* 20s */
typedef enum
{
    IR_LEARN_IDLE = 0,
    IR_LEARN_INIT,
    IR_LEARN_KEY_PRESS,
    IR_LEARN_KEY_RELEASE,
} IR_LEARN_STATE;

typedef struct
{
    bool isConfigUpdate;            /*whether ir learn config updata*/
    uint8_t irLearnKey;             /*ir learn specific key*/
    uint8_t irLearnStage;           /*ir learn stage*/

} IR_LearnStruct;

typedef struct
{
    bool        isValid;
    float       frequency;
    uint32_t    key_value;
    uint32_t    offset;
} IR_LearnStorgeInfo;


#if SCENARIO_SWITCH_EN
#define IR_SCENARIO_EN   1
#endif

/**============================================
*  voice module <Interface> for external
===============================================*/

extern void ir_learn_exit(void);
extern void ir_learn_module_init(void);
extern bool ir_learn_press_handle(uint32_t key_index_1, uint32_t key_index_2,
                                  uint32_t key_length);
extern bool ir_learn_key_release_handle(void);
extern bool ir_learn_check_dlps(void);
extern bool ir_learn_msg_proc(uint16_t msg_sub_type);
extern void ir_learn_init_timer(void);
extern bool ir_learn_reset_ftl_learning_data(void);

#ifdef __cplusplus
}
#endif

#endif /*__IR_APP_CONFIG_H_*/


/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

