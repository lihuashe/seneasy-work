/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ft3308_driver.h
* @brief    
* @details
* @author   huzhuohuan
* @date     2024-09-13
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __FT3308_DRIVER_H__
#define __FT3308_DRIVER_H__

#include "app_config.h"
#include "driver.h"
#include "../../tl_common.h"

#if (TOUCH_ENABLE)
#define FT3308_I2C_SDA_PIN              GPIO_PD1
#define FT3308_I2C_SCL_PIN              GPIO_PD0
#define FT3308_CHIP_DRIVER_PIN          GPIO_PC2
#define FT3308_RESETER                  GPIO_PD7
#define INT_TRIGGER_PIN                 GPIO_PA0
#define FT3308_SLAVE_DMA_ADDR           0x70
#define FT3308_SLAVE_ADD_LED            1
#define FT3308_SLAVE_DIVCLOCK           ((unsigned char)(CLOCK_SYS_CLOCK_HZ / (4 * 400000)))

#define FUNCTION_ENABLE                 1
#define HIGH_LEVEL_VOLTAGE              1
#define LOW_LEVEL_VOLTAGE               0

/*
 * Gesture function enable
 * default: disable
 */
#define FT3308_FUNCTION_ENABLE          0
#define FT3308_INT_TIRGGIR_ENABLE       1
#define FT3308_OPEN_TC_REL_ENABLE       0

/*****************************************************************************
 * Private constant and macro definitions using #define
 *****************************************************************************/
#define HOST_MCU_DRIVER_VERSION "FocalTech MCU V1.0"

#define INTERVAL_READ_REG               200 /* unit:ms */
#define FT3308_DATA_INIT                0
#define FT3308_READ_DATA_LEN            14

#define FTS_CMD_READ_ID                 0x90

/* chip id */
#define FTS_CHIP_IDH                    0x62
#define FTS_CHIP_IDL                    0x16

/* register address */
#define FTS_REG_CHIP_ID                 0xA3
#define FTS_REG_CHIP_ID2                0x9F
#define FTS_REG_FW_VER                  0xA6
#define FTS_REG_UPGRADE                 0xFC

#define FTS_FACE_DETECT_POS             1
#define FTS_TOUCH_X_H_POS               3
#define FTS_TOUCH_X_L_POS               4
#define FTS_TOUCH_Y_H_POS               5
#define FTS_TOUCH_Y_L_POS               6
#define FTS_TOUCH_X_H_1_POS             9
#define FTS_TOUCH_X_L_1_POS             10
#define FTS_TOUCH_Y_H_1_POS             11
#define FTS_TOUCH_Y_L_1_POS             12
#define FTS_TOUCH_PRE_POS               7
#define FTS_TOUCH_AREA_POS              8
#define FTS_TOUCH_EVENT_POS             3
#define FTS_TOUCH_ID_POS                5
#define FTS_TOUCH_POINT_NUM             2
#define FTS_TOUCH_XY_POS                7
#define FTS_TOUCH_MISC                  8

#define MOUSE_LARGEN                    0x48
#define MOUSE_SHRINK                    0x49

typedef enum {
    SWIPE_UP,
    SWIPE_DOWN,          
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_ZOOM_IN,
    SWIPE_ZOOM_OUT,
} Mouse_gesture;

// Mobile gear selection
typedef enum {
    MOVE_GEAR_ONE,      // For computers, with a movement range of 0 ~ 35 cm
    MOVE_GEAR_TWO,      // For TVs, with a movement range of 0 ~ 80 cm
    MOVE_GEAR_THREE,    // For larger displays, with a movement range of 0 ~ 150 cm
} Mouse_gear;

typedef enum {
    MOUSE_UP,
    MOUSE_DOWN ,          
    MOUSE_DOUBLE_CLICK,
} MouseEvent;

typedef enum {
    MOUSE_NULL,
    MOUSE_MOVE,
    MOUSE_ROLL,
    MOUSE_ZOOM,
}Mouse_state;
extern Mouse_state mouse_state;

/*touch event info*/
typedef struct TS_EVENT
{
    s16 x;    /*x coordinate */
    s16 y;    /*y coordinate */
    s16 x1;
    s16 y1;   /* y coordinate */
    u8 flag;  /* touch event flag: 0 -- down; 1-- up; 2 -- contact */
    u8 two_keys;
    u8 zoom;
    u8 key_num;
}Ts_event;

typedef struct TS_MOVE_STATE
{
    s16 up_move_num;
    s16 up_move_total_num;
    s16 down_move_num;
    s16 down_move_total_num;
    s16 left_move_num;
    s16 left_move_total_num;
    s16 right_move_num;
    s16 right_move_total_num;

}Ts_move_state;

typedef struct TS_MOVE
{
    Ts_move_state ts_move_state; 
    s16  move_x;
    s16  move_y;
    u32  data_filt;
    u8   touch_press;
    u32  touch_key_one_num;
}Ts_move;

typedef struct TOUCH_PRESS
{
    bool touch_press;
    u32  touch_time;
}Touch_press;

typedef struct FT3308_DRIVER
{
    Ts_event ts_now_data;
    Ts_event ts_store;
    Ts_move Ts_move;
    Touch_press touch_press;
}Ft3308_driver;

/*****************************************************************************
 * Peripheral interface transfer parameters
 *****************************************************************************/
extern Ft3308_driver ft3308;
extern bool ft_touch_sleep_flag;
/*****************************************************************************/

extern void FT3308_iic_init(void);
extern void FT3308_loop(void);
extern void FT3308_sleep_config(void);
extern void Mouse_move_gear_set(void);
extern void touch_irq_handler(void);
extern void FT3308_chip_rsin(void);
#endif
#endif