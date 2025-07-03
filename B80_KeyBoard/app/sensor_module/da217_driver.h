#ifndef __DA217_DRIVER_H__
#define __DA217_DRIVER_H__
#include "app_config.h"
#include "driver.h"
#include "../../tl_common.h"

#if (GSENSOR_ENABLE)
#define I2C_SDA_PIN                     GPIO_PD1
#define I2C_SCL_PIN                     GPIO_PD0
#define GSENSOR_SLAVE_DMA_ADDR          0x4F 
#define GSENSOR_SLAVE_ADD_LED           1
#define ANGLE_MULTIPLIER                1000
#define M_PI                            3.14159265358979323846
#define RAD_TO_DEG                      57.295779513082320876798154814105
#define GSENSOR_SLAVE_DIVCLOCK          ((unsigned char)(CLOCK_SYS_CLOCK_HZ / (4 * 400000)))

///////////////////////// Feature Configuration/////////////////////////////////
/**
 *  @brief  Feature select in BLE Sample project
 */
#define ANGLE_CONVERSION_ENABLED 0

/*******************************************************************************
Macro definitions - Register define for Gsensor asic
********************************************************************************/

#define NSA_REG_SPI_I2C                 0x00
#define NSA_REG_WHO_AM_I                0x01
#define NSA_REG_ACC_X_LSB               0x02
#define NSA_REG_ACC_X_MSB               0x03
#define NSA_REG_ACC_Y_LSB               0x04
#define NSA_REG_ACC_Y_MSB               0x05
#define NSA_REG_ACC_Z_LSB               0x06
#define NSA_REG_ACC_Z_MSB               0x07 
#define NSA_REG_MOTION_FLAG             0x09 
#define NSA_REG_G_RANGE                 0x0f
#define NSA_REG_ODR_AXIS_DISABLE        0x10
#define NSA_REG_POWERMODE_BW            0x11
#define NSA_REG_SWAP_POLARITY           0x12
#define NSA_REG_FIFO_CTRL               0x14
#define NSA_REG_INTERRUPT_SETTINGS0     0x15
#define NSA_REG_INTERRUPT_SETTINGS1     0x16
#define NSA_REG_INTERRUPT_SETTINGS2     0x17
#define NSA_REG_INTERRUPT_MAPPING1      0x19
#define NSA_REG_INTERRUPT_MAPPING2      0x1a
#define NSA_REG_INTERRUPT_MAPPING3      0x1b
#define NSA_REG_INT_PIN_CONFIG          0x20
#define NSA_REG_INT_LATCH               0x21
#define NSA_REG_FREEFALL_DURATION       0x22
#define NSA_REG_FREEFALL_THRESHOLD      0x23
#define NSA_REG_FREEFALL_HYST           0x24
#define NSA_REG_ACTIVE_DURATION         0x27
#define NSA_REG_ACTIVE_THRESHOLD        0x28
#define NSA_REG_TAP_DURATION            0x2A
#define NSA_REG_TAP_THRESHOLD           0x2B
#define NSA_REG_ENGINEERING_MODE        0x7f
#define NSA_REG_SENS_COMP               0x8c
#define NSA_REG_MEMS_OPTION             0x8f
#define NSA_REG_CHIP_INFO               0xc0
#define Gsensor_abs(x)          (((x) > 0) ? (x) : (-(x)))

typedef struct Gsensor_AccData_tag{
   s16 acc_x;                                  
   s16 acc_y;
   s16 acc_z;
}Gsensor_AccData;
extern Gsensor_AccData Acc_data;

// Function external call declaration
extern bool Face_up_flag;

extern void Gsensor_loop(void);
extern void Gsensor_iic_init(void);
extern s8 Gsensor_read_data(s16 *x, s16 *y, s16 *z);
#else
extern bool Face_up_flag;
#endif
#endif
