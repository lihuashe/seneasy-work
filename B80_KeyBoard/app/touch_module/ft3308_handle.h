/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ft3308_handle.h
* @brief    
* @details
* @author   huzhuohuan
* @date     2024-08-27
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __FT3308_HANDLE__
#define __FT3308_HANDLE__

#include "ft3308_driver.h"

// Defines a constant to represent an invalid or wrong data indicator.
#define WRONG_DATA            0xFF
#define WRONG_INTERVAL        0xFFFF

// Defines a constant to represent a specific roll state, possibly a threshold or condition value.
#define ROLL_STATE            0x02

// Minimum interval Threshold Specifies the minimum interval for filtering data
#define MIN_INTERVAL          200

// Effective data length
#define EFFE_DATA_LENGTH      4
#define READ_STORE_LENGTH     (EFFE_DATA_LENGTH - 1)

// The larger the fast and fast sliding sensitivity threshold is, the smaller it is
#define FASH_MOVE_SENSITIVE   48

// touch interrupt enable
#define TOUCH_INT_MONITOR_EN  1

// Initializes a FUNC_DATA structure with default values. 
// This macro is used to set default states for run_one, distance_flag, and store_point.
#define FUNC_DATA_INIT        \
{                             \
    .run_one = false,         \
    .distance_flag = false,   \
    .store_point = 0          \
}                             \

#define SHAPE_RET_DATA_INIT   \
{                             \
    .type = WRONG_DATA,       \
    .interval = 0,            \
    .speed = {0, 0},          \
    .fast_mode = false        \
}                             \

enum
{
    LOW_SPEED_MODE,
    HIGH_SPEED_MODE
};

// Declares a structure to store seat position and related data.
typedef struct SEAT_STORE
{
    // Arrays to store the x and y coordinates of two points for each seat position.
    s16 x1[EFFE_DATA_LENGTH];
    s16 y1[EFFE_DATA_LENGTH];
    s16 x2[EFFE_DATA_LENGTH];
    s16 y2[EFFE_DATA_LENGTH];
    
    // Array to store the point interval for each seat position.
    u32 point_interval[EFFE_DATA_LENGTH];
    
    // The current seat number being recorded.
    u8  seat_num;
    
    // Counters for up, down, left, and right movements.
    u8  up_num[3];
    u8  down_num[3];
    u8  left_num[3];
    u8  right_num[3];

    u8  arctan_coordinate_one;
    u8  arctan_coordinate_two;
} Seat_store;

// Declares an external Seat_store variable to be defined elsewhere.
extern Seat_store seat_st;

// Declares a structure to hold functional data.
typedef struct FUNC_DATA
{
    // A flag to indicate if a certain operation has been run once.
    bool run_one;
    
    // A flag to indicate the state of distance recording.
    bool distance_flag;
    
    // A variable to store a point related to distance data.
    u32 store_point;
} Func_data;

// Declares an external Func_data variable to be defined elsewhere.
extern Func_data func_data;


// Declares a structure to hold shape return data.
typedef struct SHAPE_RET_DATA 
{
    // An 8-bit unsigned integer to store the type of shape.
    u8 type; 

    // An integer to store the interval in milliseconds.
    int interval; 

    // An speed Feedback the speed of coordinates 1 and 2
    u8 speed[2];

    // An fast_mode Indicates fast movement
    bool fast_mode;
} Shape_ret_data;

// Declares an external Shape_ret_data variable to be defined elsewhere.
extern Shape_ret_data Shape_data;

/*****************************************************************************
 * Peripheral interface transfer parameters
 *****************************************************************************/
extern Shape_ret_data move_shape_map(s16 x1, s16 y1, s16 x2, s16 y2);
extern void position_data_clear(void);
#endif 