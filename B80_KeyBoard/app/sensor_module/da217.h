/**
 * @file da217.h
 * @author zhangkaihua
 * @brief 
 * @version 0.1
 * @date 2022-05-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __DA217_H__
#define __DA217_H__

#include "app_config.h"
#include "driver.h"
#include "../../tl_common.h"

#if (GSENSOR_DA217_ENABLE)

#define DA217_REG_NUM  39
typedef enum da217_register_maping_t{
    REG_SPI_CONFIG      = 0x00,
    REG_WHO_AM_I,

    REG_SENSOR_DATA     = 0x02,
    REG_ACC_X_LSB       = REG_SENSOR_DATA,
    REG_ACC_X_MSB,             
    REG_ACC_Y_LSB,             
    REG_ACC_Y_MSB,             
    REG_ACC_Z_LSB,             
    REG_ACC_Z_MSB,

    REG_SENSOR_STATUS    = 0x08,
    REG_FIFO_STATUS     = REG_SENSOR_STATUS,
    REG_MOTION_FLAG,
    REG_NEW_DATA_FLAG,
    REG_TAP_ACTIVE_STATUS,
    REG_ORIENT_STATUS,
    REG_STEPS_MSB,
    REG_STEPS_LSB,

    REG_SENSOR_CONFIG  = 0x0F,
    REG_RESOLUTION_RANGE= REG_SENSOR_CONFIG,
    REG_ODR_AXIS,
    REG_MODE_BW,
    REG_SWAP_POLARITY,
    REG_FIFO_CTRL       = 0x14,

    REG_SENSOR_INT_SET  = 0x15,
    REG_INT_SET0        = REG_SENSOR_INT_SET,
    REG_INT_SET1,
    REG_INT_SET2,

    REG_SENSOR_INT_MAP  = 0x19,
    REG_INT_MAP0        = REG_SENSOR_INT_MAP,
    REG_INT_MAP1,
    REG_INT_MAP2,

    REG_SENSOR_INT_CONFIG   = 0x20,
    REG_INT_CONFIG      = REG_SENSOR_INT_CONFIG,
    REG_INT_LATCH,

    REG_SENSOR_FREEFALL = 0x22,
    REG_FREEFALL_DUR    = REG_SENSOR_FREEFALL,
    REG_FREEFALL_THS,
    REG_FREEFALL_HYST,

    REG_SENSOR_ACTIVE   = 0x27,
    REG_ACTIVE_DUR      = REG_SENSOR_ACTIVE,
    REG_ACTIVE_THS,

    REG_SENSOR_TAP      = 0x2A,
    REG_TAP_DUR         = REG_SENSOR_TAP,
    REG_TAP_THS,

    REG_ORIENT_HYSY,
    REG_Z_BLOCK,
    REG_RESET_STEP,
    REG_STEP_FILTER     = 0x33,
    REG_SIGN_MOTION_THS,
}da217_register_maping_t;

typedef union word_def_t
{   
    struct 
    {
        u8 LSB;
        u8 MSB;
    }bytes;
    s16 word;
}word_def_t;

typedef struct sensor_data_t {
    // 0x02-0x07
    struct acc {
        s16 x;
        s16 y;
        s16 z;
    };    
}sensor_data_t;

typedef struct sensor_status_t {
    // 0x08 [R]
    struct fifo_status {
        u8 entries:6;
        u8 full:1;
        u8 watermark:1;
    }fifo_status;

    // 0x09 [R]
    struct motion_flag {
        u8 freefall:1;
        u8 tilt:1;
        u8 active:1;
        u8 sign_motion:1;
        u8 d_tap:1;
        u8 s_tap:1;
        u8 orient:1;
        u8 step:1;
    }motion_flag;

    // 0x0A
    u8 new_data:1;
    u8 :7;

    // 0x0B
    struct tap_active_flag {
        u8 active_first_z:1;
        u8 active_first_y:1;
        u8 active_first_x:1;
        u8 sign_active:1;
        u8 tap_first_z:1;
        u8 tap_first_y:1;
        u8 tap_first_x:1;
        u8 sign_tap:1;
    }tap_active_flag;

    // 0x0C
    struct orient_status {
        u8 step_status:2;
        u8 :2;
        u8 xy_orient:2;
        u8 z_orient:1;
        u8 :1;
    }orient_status;
}sensor_status_t;

typedef struct sensor_config_t {
    // 0x0F
    struct resolution_range {
        u8 full_scale:2;
        u8 resolution:2;
        u8 :1;
        u8 wdt_time:1;
        u8 wdt_en:1;
        u8 hp_en:1;
    }resolution_range;

    // 0x10
    struct odr_axis {
        u8 odr:4;
        u8 :1;
        u8 z_disable:1;
        u8 y_disable:1;
        u8 x_disable:1;
    }odr_axis;

    // 0x11
    struct mode_bw {
        u8 auto_sleep:1;
        u8 bw:2;
        u8 :4;
        u8 power_off:1;
    }mode_bw;

    // 0x12
    struct swap_polarity {
        u8 xy_swap:1;
        u8 z_pol:1;
        u8 y_pol:1;
        u8 x_pol:1;
        u8 :4;
    }swap_polarity;

    // 0x14
    struct fifo_ctrl {
        u8 watermark:5;
        u8 :1;
        u8 mode:2;
    }fifo_ctrl;
}sensor_config_t;

typedef struct sensor_int_set_t {
    struct s1 {
        u8 step:1;
        u8 sign_motion:1;
        u8 fifo_full:1;
        u8 watermark:1;
        u8 tile:1;
        u8 :3;
    }s1;

    struct s2 {
        u8 x_active:1;
        u8 y_active:1;
        u8 z_active:1;
        u8 orient:1;
        u8 d_tap:1;
        u8 s_tap:1;
        u8 int_source:2;
    }s2;

    struct s3 {
        u8 :3;
        u8 freefall:1;
        u8 new_data:1;
        u8 dnd_time:2;
        u8 dnd_enable:1;    // do not disturb
    }s3;
}sensor_int_set_t;

typedef struct sensor_int_map_t {
    struct map1 {
        u8 int1_freefall:1;
        u8 int1_step:1;
        u8 int1_active:1;
        u8 int1_tilt:1;
        u8 int1_d_tap:1;
        u8 int1_s_tap:1;
        u8 int1_orient:1;
        u8 int1_sign_motion:1;
    }map1;

    struct map2 {
        u8 int1_new_data:1;
        u8 int1_watermark:1;
        u8 int1_fifo_full:1;
        u8 :2;
        u8 int2_fifo_full:1;
        u8 int2_watermark:1;
        u8 int2_new_data:1;
    }map2;

    struct map3 {
        u8 int2_freefall:1;   
        u8 int2_step:1;
        u8 int2_active:1;
        u8 int2_tilt:1;
        u8 int2_d_tap:1;
        u8 int2_s_tap:1;
        u8 int2_orient:1;
        u8 int2_sign_motion:1;
    }map3;
}sensor_int_map_t;

typedef struct sensor_int_config_t {
    // 0x20
    struct config {
        u8 int1_level:1;
        u8 int1_od:1;
        u8 int2_level:1;
        u8 int2_od:1;
        u8 :3;
        u8 int_reset:1;
    }config;

    // 0x21:
    struct latch {
        u8 int1_latch:4;
        u8 int2_latch:4;
    }latch;
}sensor_int_config_t;

typedef struct sensor_func_freefall_t {
    // 0x22-0x24
    struct freefall {
        u8 duration;
        u8 threshold;

        u8 hysteresis:2;
        u8 mode:1;
        u8 :5;
    }freefall;
}sensor_func_freefall_t;

typedef struct sensor_func_active_t {
    // 0x27-0x28
    struct active {
        u8 active_duration:4;
        u8 inactive_duration:4;

        u8 threshold;
    }active;
}sensor_func_active_t;

typedef struct sensor_func_tap_t {
    // 0x2A-0x2B
    struct tap {
        u8 tap_duration:3;
        u8 :3;
        u8 shock_duration:1;
        u8 quiet_duration:1;

        u8 threshold:5;
        u8 tilt_time:3;
    }tap;
}sensor_func_tap_t;

typedef struct sensor_func_orient_t {
    // 0x2C
    u8 mode:2;
    u8 block:2;
    u8 hystersis:3;
    u8 :1;
}sensor_func_orient_t;

typedef struct sensor_func_z_block_t {
    u8 z_blocking:4;    
    u8 tilt_threshold:4;
}sensor_func_z_block_t;

typedef struct sensor_step_reset_t {
    // 0x2E
    u8 :7;
    u8 reset_steps:1;
}sensor_step_reset_t;

typedef struct sensor_step_enable_t {
    // 0x33
    u8 :7;
    u8 step_enable:1;
}sensor_step_enable_t;

/*

typedef union da217_registers_t
{
    u8 regs[DA217_REG_NUM];

    struct reg_map_t
    {
        // 0x00
        u8 spi_config;

        // 0x01
        u8 chip_id;

        // 0x02-0x07
        struct data {
            s16 x;
            s16 y;
            s16 z;
        };
        
        // 0x08
        struct fifo_status {
            u8 watermark:1;
            u8 full:1;
            u8 entries:6;
        };

        // 0x09
        struct motion_flag {
            u8 step:1;
            u8 orient:1;
            u8 s_tap:1;
            u8 d_tap:1;
            u8 sign_motion:1;
            u8 active:1;
            u8 tilt:1;
            u8 freefall:1;
        };

        // 0x0A
        u8 :7;
        u8 new_data:1;

        // 0x0B
        struct tap_active_flag {
            u8 sign_tap:1;
            u8 tap_first_x:1;
            u8 tap_first_y:1;
            u8 tap_first_z:1;
            u8 sign_active:1;
            u8 active_first_x:1;
            u8 active_first_y:1;
            u8 active_first_z:1;
        };

        // 0x0C
        struct orient_status {
            u8 :1;
            u8 z_orient:1;
            u8 xy_orient:2;
            u8 :2;
            u8 step_status:2;
        };

        // 0x0D-0x0E
        u8 step_msb;
        u8 step_lsb;

        // 0x0F
        struct resolution_range {
            u8 hp_en:1;
            u8 wdt_en:1;
            u8 wdt_time:1;
            u8 :1;
            u8 resolution:2;
            u8 full_scale:2;
        };

        // 0x10
        struct odr_axis {
            u8 x_disable:1;
            u8 y_disable:1;
            u8 z_disable:1;
            u8 :1;
            u8 odr:4;
        };

        // 0x11
        struct mode_bw {
            u8 power_off:1;
            u8 :4;
            u8 bw:2;
            u8 auto_sleep:1;
        };

        // 0x12
        struct swap_polarity {
            u8 :4;
            u8 x_pol:1;
            u8 y_pol:1;
            u8 z_pol:1;
            u8 xy_swap:1;
        };

        // 0x14
        struct fifo_ctrl {
            u8 mode:2;
            u8 :1;
            u8 watermark:5;
        };

        // 0x15-0x17
        struct int_set {
            u8 :3;
            u8 tile:1;
            u8 watermark:1;
            u8 fifo_full:1;
            u8 sign_motion:1;
            u8 step:1;

            u8 int_source:2;
            u8 s_tap:1;
            u8 d_tap:1;
            u8 orient:1;
            u8 z_active:1;
            u8 y_active:1;
            u8 x_active:1;

            u8 temp_disable:1;
            u8 temp_disable_time:2;
            u8 new_data:1;
            u8 freefall:1;
            u8 :3;
        };

        // 0x19-0x1B
        struct int_map {
            u8 int1_sign_motion:1;
            u8 int1_orient:1;
            u8 int1_s_tap:1;
            u8 int1_d_tap:1;
            u8 int1_tilt:1;
            u8 int1_active:1;
            u8 int1_step:1;
            u8 int1_freefall:1;

            u8 int2_new_data:1;
            u8 int2_watermark:1;
            u8 int2_fifo_full:1;
            u8 :2;
            u8 int1_fifo_full:1;
            u8 int1_watermark:1;
            u8 int1_new_data:1;

            u8 int2_sign_motion:1;
            u8 int2_orient:1;
            u8 int2_s_tap:1;
            u8 int2_d_tap:1;
            u8 int2_tilt:1;
            u8 int2_active:1;
            u8 int2_step:1;
            u8 int2_freefall:1;
        };

        // 0x20
        struct int_config {
            u8 int_reset:1;
            u8 :3;
            u8 int2_od:1;
            u8 int2_level:1;
            u8 int1_od:1;
            u8 int1_level:1;
        };

        // 0x21:
        struct int_latch {
            u8 int2_latch:4;
            u8 int1_latch:4;
        };

        // 0x22-0x24
        struct freefall {
            u8 duration;
            u8 threshold;
            u8 mode:1;
            u8 hysteresis:7;
        };

        // 0x27-0x28
        struct active {
            u8 inactive_duration:4;
            u8 active_duration:4;
            u8 threshold;
        };

        // 0x2A-0x2B
        struct tap {
            u8 quiet_duration:1;
            u8 shock_duration:1;
            u8 :3;
            u8 tap_duration:3;

            u8 tilt_time:3;
            u8 threshold:5;
        };

        // 0x2C
        struct orient {
            u8 hystersis:3;
            u8 block:2;
            u8 mode:2;
        };

        // 0x2D
        struct z_block {
            u8 tilt_threshold:4;
            u8 z_blocking:4;
        };

        // 0x2E
        u8 reset_steps:1;
        u8 :7;

        // 0x33
        u8 step_enable:1;
        u8 :7;

        // 0x34
        u8 sign_motion_threshold;
    }map;
}da217_registers_t;
*/

typedef enum sensor_error_t{
    SENSOR_RESULT_SUCESS = 0,
    SENSOR_RESULT_NO_PRESENT = -1,
}sensor_result_t;


typedef enum sensor_resolution_t {
    SENSOR_RESOLUTION_14b = 0,
    SENSOR_RESOLUTION_12b = 1,
    SENSOR_RESOLUTION_10b = 2,
    SENSOR_RESOLUTION_8b  = 3,
}sensor_resolution_t;

typedef enum sensor_fullscale_t {
    SENSOR_FULLSCALE_2G  = 0,
    SENSOR_FULLSCALE_4G  = 1,
    SENSOR_FULLSCALE_8G  = 2,
    SENSOR_FULLSCALE_16G = 3,
}sensor_fullscale_t;

typedef enum sensor_odr_t {
    SENSOR_ODR_1HZ     = 0,
    SENSOR_ODR_1HZ95     = 1,
    SENSOR_ODR_3HZ90     = 2,
    SENSOR_ODR_7HZ81     = 3,
    SENSOR_ODR_15HZ63    = 4,
    SENSOR_ODR_31HZ25    = 5,
    SENSOR_ODR_62HZ50    = 6,
    SENSOR_ODR_125HZ     = 7,
    SENSOR_ODR_250HZ     = 8,
    SENSOR_ODR_500HZ     = 9,
}sensor_odr_t;

typedef enum  sensor_power_mode_t{
    SENSOR_POWER_NORMAL = 0,
    SENSOR_POWER_SUSPEND = 1,
}sensor_power_mode_t;

typedef enum  sensor_bandwidth_t{
    SENSOR_BW_500HZ = 0,
    SENSOR_BW_250HZ = 1,
    SENSOR_BW_100HZ = 2,
    SENSOR_BW_500HZ_2 = 3,
}sensor_bandwidth_t;

typedef enum sensor_fifo_mode_t {
    SENSOR_FIFO_MODE_BYPASS   = 0,
    SENSOR_FIFO_MODE_FIFO     = 1,
    SENSOR_FIFO_MODE_STREAM   = 2,
    SENSOR_FIFO_MODE_TRIGGER  = 3,
}sensor_fifo_mode_t;

typedef enum sensor_dnd_time_t {
    SENSOR_DND_TIME_100MS = 0,
    SENSOR_DND_TIME_1S    = 1,
    SENSOR_DND_TIME_2S    = 2,
    SENSOR_DND_TIME_4S    = 3,
}sensor_dnd_time_t;

typedef enum sensor_latch_time_t {
    SENSOR_LATCH_NO_LATCHED         = 0,
    SENSOR_LATCH_TEMP_LATCHED_250MS = 1,
    SENSOR_LATCH_TEMP_LATCHED_500MS = 2,
    SENSOR_LATCH_TEMP_LATCHED_1S    = 3,
    SENSOR_LATCH_TEMP_LATCHED_2S    = 4,
    SENSOR_LATCH_TEMP_LATCHED_4S    = 5,
    SENSOR_LATCH_TEMP_LATCHED_8S    = 6,
    SENSOR_LATCH_LATCHED            = 7,
    SENSOR_LATCH_TEMP_LATCHED_1MS   = 10,
    SENSOR_LATCH_TEMP_LATCHED_2MS   = 11,
    SENSOR_LATCH_TEMP_LATCHED_25MS  = 12,
    SENSOR_LATCH_TEMP_LATCHED_50MS  = 13,
    SENSOR_LATCH_TEMP_LATCHED_100MS = 14,
}sensor_latch_time_t;


#define DA217_I2C_SCL   GPIO_PD0
#define DA217_I2C_SDA   GPIO_PD1
#define DA217_I2C_ADDR  0x4E
#define DA217_INT1      GPIO_PA4

#define DA217_CHIP_ID   0x13
#define SENSOR_NO_MOTION_THRESHOLD      0x30
#define SENSOR_ANY_MOTION_THRESHOLD     0x10

sensor_result_t sensor_init_normal();

sensor_result_t sensor_init_deepRetn();

void sensor_event_handler(void);

void sensor_init_FaceUp(void);

bool sensor_is_pickup();


sensor_result_t sensor_power_mode(sensor_power_mode_t mode);

sensor_result_t sensor_read_data(s16 * x, s16 * y, s16 * z);

void sensor_irq_handler();
bool sensor_factory_check();
#endif
#endif
