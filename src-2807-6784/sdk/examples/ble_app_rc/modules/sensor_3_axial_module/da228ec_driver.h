/**
**********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     da228ec_driver.h
* @brief    DA228EC加速度传感器驱动
* @details  本文件提供了DA228EC加速度传感器的驱动接口，包括初始化、数据读取、配置等功能
* @author   lihuashe
* @date     2024-12-20
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _DA228EC_DRIVER_H_
#define _DA228EC_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>

/*============================================================================*
 *                              Definitions
 *============================================================================*/
#define DA228EC_DRIVER_EN                   1
#define DA228EC_ADDR			            0x27    //实际I2C地址需要左移1位
#define DA228EC_INT_PIN                     10
/*============================================================================*
 *                              Register Definitions
 *============================================================================*/
#define DA228EC_REG_CHIPID			        0x01    // 芯片ID寄存器
#define DA228EC_REG_ACC_X_LSB			    0x02    // X轴加速度低8位
#define DA228EC_REG_ACC_X_MSB			    0x03    // X轴加速度高8位
#define DA228EC_REG_ACC_Y_LSB			    0x04    // Y轴加速度低8位
#define DA228EC_REG_ACC_Y_MSB			    0x05    // Y轴加速度高8位
#define DA228EC_REG_ACC_Z_LSB			    0x06    // Z轴加速度低8位
#define DA228EC_REG_ACC_Z_MSB			    0x07    // Z轴加速度高8位
#define DA228EC_REG_MOTION_FLAG		        0x09    // 运动标志寄存器
#define DA228EC_REG_NEWDATA_FLAG		    0x0A    // 新数据标志寄存器
#define DA228EC_REG_ACTIVE_STATUS	        0x0B    // 活动状态寄存器
#define DA228EC_REG_RANGE			        0x0F    // 量程寄存器
#define DA228EC_REG_ODR_AXIS			    0x10    // 输出频率寄存器
#define DA228EC_REG_MODE_BW			        0x11    // 模式和带宽寄存器
#define DA228EC_REG_SWAP_POLARITY			0x12    // xyz数据交换自定义寄存器
#define DA228EC_REG_INT_SET1			    0x16    // 中断设置1寄存器
#define DA228EC_REG_INT_SET2			    0x17    // 中断设置2寄存器
#define DA228EC_REG_INT_MAP1			    0x19    // 中断映射1寄存器
#define DA228EC_REG_INT_MAP2			    0x1A    // 中断映射2寄存器
#define DA228EC_REG_INT_CONFIG			    0x20    // 中断配置寄存器
#define DA228EC_REG_INT_LATCH			    0x21    // 中断保持寄存器
#define DA228EC_REG_ACTIVE_DUR			    0x27    // 活动持续时间寄存器
#define DA228EC_REG_ACTIVE_THS			    0x28    // 活动阈值寄存器

/*============================================================================*
 *                              Macros
 *============================================================================*/
#define DA228EC_MSK_RANGE			       0x03    // 量程设置掩码
#define DA228EC_MSK_ODR			           0x0F    // 输出频率掩码
#define DA228EC_MSK_MODE			       0x80    // 模式掩码

/*============================================================================*
 *                              Enumerations
 *============================================================================*/
typedef enum {
    DA228EC_RANGE_2G  = 0,  // ±2g
    DA228EC_RANGE_4G  = 1,  // ±4g
    DA228EC_RANGE_8G  = 2   // ±8g
} DA228EC_RANGE_DEF;

typedef enum {
    DA228EC_ODR_1HZ     = 0,  // 1Hz
    DA228EC_ODR_1_95HZ  = 1,  // 1.95Hz
    DA228EC_ODR_3_9HZ   = 2,  // 3.9Hz
    DA228EC_ODR_7_81HZ  = 3,  // 7.81Hz
    DA228EC_ODR_15_63HZ = 4,  // 15.63Hz
    DA228EC_ODR_31_25HZ = 5,  // 31.25Hz
    DA228EC_ODR_62_5HZ  = 6,  // 62.5Hz
    DA228EC_ODR_125HZ   = 7,  // 125Hz
    DA228EC_ODR_250HZ   = 8,  // 250Hz
    DA228EC_ODR_500HZ   = 9,  // 500Hz
    DA228EC_ODR_1000HZ  = 10  // 1000Hz
} DA228EC_ODR_DEF;

typedef enum {
    DA228EC_MODE_NORMAL  = 0,  // 正常模式
    DA228EC_MODE_SUSPEND = 1   // 挂起模式
} DA228EC_MODE_DEF;

typedef enum {
    DA228EC_INT_ACTIVE   = 0x01,  // 活动中断
    DA228EC_INT_NEWDATA  = 0x02   // 新数据中断
} DA228EC_INTERRUPT_DEF;

typedef enum {  
    DA228EC_I2C_ERR = 0,  
    DA228EC_I2C_OK  = 1,  
} DA228EC_I2CYES_DEF;

typedef enum {  
    DA228EC_FUN_DISABLE = 0,  
    DA228EC_FUN_ENABLE  = 1,  
} DA228EC_REG_USED_DEF;

/*============================================================================*
 *                              Data Structures
 *============================================================================*/
typedef struct sensor_conf_da228ec_t {
    // 0x0F
    struct {
        uint8_t full_scale:2;
        uint8_t :6;
    }resolution_range_da228ec;

    // 0x10
    struct {
        uint8_t odr:4;
        uint8_t :4;
    }mode_odr_da228ec;

    // 0x11
    struct {
        uint8_t auto_sleep:1;
        uint8_t bandwitdth:2;
        uint8_t :4;
        uint8_t power_off:1;
    }mode_axis_da228ec;

    // 0x12
    struct {
        uint8_t xy_swap:1;
        uint8_t z_pol:1;
        uint8_t y_pol:1;
        uint8_t x_pol:1;
        uint8_t :4;
    }swap_polarity_da228ec;
}sensor_conf_da228ec_t;

typedef struct sensor_int_da228ec_set_t {
    // 0x16
    struct {
        uint8_t x_active:1;
        uint8_t y_active:1;
        uint8_t z_active:1;
        uint8_t :4;
        uint8_t int_source:1;
    }s1_da228ec;

    // 0x17
    struct {
        uint8_t :4;
        uint8_t new_data:1;
        uint8_t :3;
    }s2_da228ec;
}sensor_int_da228ec_set_t;

typedef struct sensor_int_da228ec_map_t {
    // 0x19
    struct {
        uint8_t :2;
        uint8_t int1_active:1;
        uint8_t :5;
    }map1_da228ec;

    // 0x1A
    struct {
        uint8_t int1_new_data:1;
        uint8_t :7;
    }map2_da228ec;
}sensor_int_da228ec_map_t;

typedef struct sensor_int_conf_da228ec_t {
    // 0x20
    struct {
        uint8_t int1_level:1;
        uint8_t int1_od:1;
        uint8_t :5;
        uint8_t int_reset:1;
    }config_da228ec;

    // 0x21:
    struct {
        uint8_t int1_latch:4;
        uint8_t :4;
    }latch_da228ec;
}sensor_int_conf_da228ec_t;

typedef struct sensor_func_act_da228ec_t {
    // 0x27 - 0x28
    struct {
        uint8_t active_duration:4;
        uint8_t inactive_duration:4;

        uint8_t active_ths;
    }active_dur_da228ec;
}sensor_func_act_da228ec_t;

typedef struct {
    int16_t x;  // X轴加速度
    int16_t y;  // Y轴加速度
    int16_t z;  // Z轴加速度
} DA228EC_Data_t;

typedef struct {
    uint8_t lsb;
    uint8_t msb;    
} DA228EC_Data_Msb_Lsb_t;

/*============================================================================*
 *                              Function Prototypes
 *============================================================================*/
void da228ec_init(void);
static bool da228ec_check_id(uint8_t *chip_id);
void da228ec_config_init(void);
void da228ec_read_acceleration(DA228EC_Data_t *data);
void da228ec_set_range(DA228EC_RANGE_DEF range);
void da228ec_set_odr(DA228EC_ODR_DEF odr);
void da228ec_set_mode(DA228EC_MODE_DEF mode);
void da228ec_enable_interrupt(DA228EC_INTERRUPT_DEF interrupt);
void da228ec_disable_interrupt(DA228EC_INTERRUPT_DEF interrupt);
bool da228ec_check_new_data(void);
void da228ec_interrupt_handle(uint32_t pin_mask);
bool da228ec_get_interrupt_status(void);
void da228ec_clear_interrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* _DA228EC_DRIVER_H_ */

/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/