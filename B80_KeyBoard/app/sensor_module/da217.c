/**
 * @file da217.c
 * @author zhangkaihua
 * @brief 
 * @version 0.1
 * @date 2022-05-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "da217.h"
#include "fmath.h"
#include "da217_driver.h"

#if (GSENSOR_DA217_ENABLE)
//static bool is_sensor_int = false;

_attribute_data_retention_ u32 tick_sensor_refresh = 0;
_attribute_data_retention_ u32 tick_sensor_no_active = 0;
_attribute_data_retention_ u32 sensor_no_active_cnt = 0;
_attribute_data_retention_ u8 sensor_state;
_attribute_data_retention_ u8 pre_sensor_state;
_attribute_data_retention_ u8 new_state_debounce_cnt = 0;

static void _sensor_i2c_init()
{
    // 切换I2C总线引脚到sensor 引脚
    i2c_gpio_set(DA217_I2C_SDA, DA217_I2C_SCL);  	//SDA/CK : PA3/PA4
    i2c_master_init(DA217_I2C_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)));    // I2C clk
}

static u8 _sensor_read_reg(u8 reg_addr)
{
    // 读取sensor寄存器数据
    return i2c_read_byte(reg_addr, 1);
}

static void _sensor_write_reg(u8 reg_addr, u8 reg_value)
{
    // 写入sensor寄存器数据
    i2c_write_byte(reg_addr, 1, reg_value);
}

static void _sensor_read_regs(u8 reg_addr, u8 * read_buff, u8 len) 
{
    // 读取sensor寄存器数据
    i2c_read_series(reg_addr, 1, read_buff, len);
}

static void _sensor_write_regs(u8 reg_addr, u8 * write_buff, u8 len)
{
    // 读取sensor寄存器数据
    i2c_write_series(reg_addr, 1, write_buff, len);
}

// #define log_str(x)      u_printf("(%s:%d)"  x "\r\n" , __FUNCTION__, __LINE__)
// #define log(x, s,...)   u_printf("(%s:%d)"  x "\r\n" , __FUNCTION__, __LINE__, ## s)

sensor_result_t sensor_init_normal()
{
    _sensor_i2c_init();

    u8 chip_id = _sensor_read_reg(REG_WHO_AM_I);
    if (chip_id != DA217_CHIP_ID) {
        // log("sensor not presetn, id = 0x%02x", chip_id);
        printf("(%s:%d) sensor not present, id = 0x%02x \r\n" , __FUNCTION__, __LINE__, chip_id);
        return SENSOR_RESULT_NO_PRESENT;
    } else {
        printf("(%s:%d) sensor init sucess, id = 0x%02x \r\n" , __FUNCTION__, __LINE__, chip_id);
    }
    tick_sensor_refresh = clock_time(); 

    u8 bus_config = 0x66;
    _sensor_write_reg(REG_SPI_CONFIG, bus_config);
    WaitMs(20);

    static sensor_config_t sensor_config = {
        .resolution_range.hp_en = 0,
        .resolution_range.wdt_en = 0,
        .resolution_range.wdt_time = 0,
        .resolution_range.resolution = SENSOR_RESOLUTION_12b,
        .resolution_range.full_scale = SENSOR_FULLSCALE_2G,
        .odr_axis.x_disable = 0,
        .odr_axis.y_disable = 0,
        .odr_axis.z_disable = 0,
        .odr_axis.odr = SENSOR_ODR_31HZ25,
        .mode_bw.power_off = 0,
        .mode_bw.bw = SENSOR_BW_250HZ,
        .mode_bw.auto_sleep = 0,
        .swap_polarity.x_pol = 0,
        .swap_polarity.y_pol = 0,
        .swap_polarity.z_pol = 0,
        .swap_polarity.xy_swap = 0,
        .fifo_ctrl.mode = SENSOR_FIFO_MODE_BYPASS,
        .fifo_ctrl.watermark = 0
    };
    _sensor_write_regs(REG_SENSOR_CONFIG, (u8 *)&sensor_config, sizeof(sensor_config_t));

    sensor_int_set_t sensor_int_set = {
        .s1.tile = 0,
        .s1.watermark = 0,
        .s1.fifo_full = 0,
        .s1.sign_motion = 0,
        .s1.step = 0,
        .s2.int_source = 1,   // 0: oversample data; 1: unfiltered data; 2: filter data
        .s2.s_tap = 0,
        .s2.d_tap = 0,
        .s2.orient = 0,
        .s2.x_active = 1,
        .s2.y_active = 1,
        .s2.z_active = 1,
        .s3.dnd_enable = 0,
        .s3.dnd_time = SENSOR_DND_TIME_1S,
        .s3.new_data = 0,
        .s3.freefall = 0,
    };
    _sensor_write_regs(REG_SENSOR_INT_SET, (u8 *)&sensor_int_set, sizeof(sensor_int_set_t));

    sensor_int_map_t sensor_int_map = {
        .map1.int1_sign_motion = 0,
        .map1.int1_orient = 0,
        .map1.int1_s_tap = 0,
        .map1.int1_d_tap = 0,
        .map1.int1_tilt = 0,
        .map1.int1_active = 1,
        .map1.int1_step = 0,
        .map1.int1_freefall = 0,
        .map2.int1_fifo_full = 0,
        .map2.int1_watermark = 0,
        .map2.int1_new_data = 0,
        .map2.int2_new_data = 0,
        .map2.int2_watermark = 0,
        .map2.int2_fifo_full = 0,
        .map3.int2_sign_motion = 0,
        .map3.int2_orient = 0,
        .map3.int2_s_tap = 0,
        .map3.int2_d_tap = 0,
        .map3.int2_tilt = 0,
        .map3.int2_active = 0,
        .map3.int2_step = 0,
        .map3.int2_freefall = 0,
    };
    _sensor_write_regs(REG_SENSOR_INT_MAP, (u8 *)&sensor_int_map, sizeof(sensor_int_map_t));

    sensor_int_config_t sensor_int_config = {
        .config.int_reset = 1,
        .config.int2_od = 0,    // 0: push-pull; 1: od
        .config.int2_level = 0, // 0: active high; 1: active low
        .config.int1_od = 0,
        .config.int1_level = 1,
        .latch.int2_latch = SENSOR_LATCH_TEMP_LATCHED_1S,
        .latch.int1_latch = SENSOR_LATCH_TEMP_LATCHED_500MS,
    };
    _sensor_write_regs(REG_SENSOR_INT_CONFIG, (u8 *)&sensor_int_config, sizeof(sensor_int_config_t));

    // 配置active中断
    sensor_func_active_t sensor_func_active = {
        .active.active_duration = 1, // Active duration time = (Active_dur + 1)* ODR_period
        .active.inactive_duration = 1, // inactive duration time = (Inactive_dur + 1)* ODR_period
        .active.threshold = 20 // Threshold of active interrupt=Active_th*K(mg) K = 3.91(2g range),K =7.81(4g range),K=15.625(8g range),K=31.25(16g range).
    };
    _sensor_write_regs(REG_SENSOR_ACTIVE, (u8 *)&sensor_func_active, sizeof(sensor_func_active_t));

	// /***step1. set pin as gpio and enable input********/
	// gpio_set_func(DA217_INT1, AS_GPIO);           //enable GPIO func
	// gpio_set_input_en(DA217_INT1, 1);             //enable input
	// gpio_set_output_en(DA217_INT1, 0);            //disable output
	// /***step2.      set the polarity and open pullup ***/
	// gpio_set_interrupt_pol(DA217_INT1, POL_FALLING);    //rising edge
	// /***step3.      set irq enable  ***/
	// reg_irq_src = FLD_IRQ_GPIO_RISC1_EN; //clean irq status
	// reg_irq_mask |= FLD_IRQ_GPIO_RISC1_EN;
	// gpio_en_interrupt_risc1(DA217_INT1, 1);

    sensor_state = 0;
    pre_sensor_state = 0;
    new_state_debounce_cnt = 0;
   
    cpu_set_gpio_wakeup(DA217_INT1, Level_Low, 1);

    return SENSOR_RESULT_SUCESS;
}

sensor_result_t sensor_init_deepRetn()
{
    _sensor_i2c_init();
	// /***step1. set pin as gpio and enable input********/
	// gpio_set_func(DA217_INT1, AS_GPIO);           //enable GPIO func
	// gpio_set_input_en(DA217_INT1, 1);             //enable input
	// gpio_set_output_en(DA217_INT1, 0);            //disable output
	// /***step2.      set the polarity and open pullup ***/
	// gpio_set_interrupt_pol(DA217_INT1, POL_FALLING);    //rising edge
	// /***step3.      set irq enable  ***/
	// reg_irq_src = FLD_IRQ_GPIO_RISC1_EN; //clean irq status
	// reg_irq_mask |= FLD_IRQ_GPIO_RISC1_EN;
	// gpio_en_interrupt_risc1(DA217_INT1, 1);
    
    cpu_set_gpio_wakeup(DA217_INT1, Level_Low, 1);

    return SENSOR_RESULT_SUCESS;
}

void sensor_get_attitude(sensor_data_t * a, s16 * pitch, s16 * roll)
{
    s16 ax,ay,az;
    ax = a->x >> 4;
    ay = a->y >> 4;
    az = a->z >> 4;
    *pitch = fast_arctan2(az, ay);
    *roll = fast_arctan2(az, ax);
}

void sensor_irq_handler()
{
	// /************* gpio irq risc1 *************/
	// if(reg_irq_src & FLD_IRQ_GPIO_RISC1_EN){
	// 	reg_irq_src = FLD_IRQ_GPIO_RISC1_EN;        // clear irq_gpio irq flag
    //     // latest_user_event_tick = clock_time();
    //     // is_device_no_active = false;
    //     is_sensor_int = 1;
    //     // 唤醒后重新恢复传感器灵敏度为0x10
    //     // _sensor_write_reg(0x2e, SENSOR_ANY_MOTION_THRESHOLD);   //any_mot_threshold = SENSOR_ANY_MOTION_THRESHOLD
    //     // printf("zfm into sensor_irq is_sensor_int=[%d] ...\r\n",is_sensor_int);
	// }
}

/**
 * @brief g-sensor 处理主线程
 * 
 */
void sensor_event_handler(void)
{
    if (clock_time_exceed(tick_sensor_refresh, 5 * 100 * 1000))
    {
        tick_sensor_refresh = clock_time();     
        u8 tmp_data[6] = {0};

        i2c_master_init(DA217_I2C_ADDR, ((unsigned char)(CLOCK_SYS_CLOCK_HZ / (4 * 400000))));
        _sensor_read_regs(REG_SENSOR_DATA, (u8 *)&tmp_data, 6);

        sensor_data_t acc;
        acc.x = ((s16)(tmp_data[1] << 8 | tmp_data[0])) >> 4;

        acc.y = ((s16)(tmp_data[3] << 8 | tmp_data[2])) >> 4;

        acc.z = ((s16)(tmp_data[5] << 8 | tmp_data[4])) >> 4;

        Face_up_flag = (acc.z < 0);
        //printf("--->>> Face_up_flag = %d\n", Face_up_flag);
    }

#if 0
    if (clock_time_exceed(tick_sensor_refresh, 32 * 1000) || is_sensor_int) {
    // if (is_sensor_int) {
        tick_sensor_refresh = clock_time();     
        is_sensor_int = false;

        static sensor_status_t status;
        _sensor_read_regs(REG_SENSOR_STATUS, (u8 *)&status, sizeof(sensor_status_t));
        // array_printf((u8 *)&status, sizeof(sensor_status_t));
        if (status.new_data) {

            sensor_data_t acc;
            s16 pitch,roll;
            _sensor_read_regs(REG_SENSOR_DATA, (u8 *)&acc, sizeof(sensor_data_t));

            sensor_get_attitude(&acc, &pitch, &roll);
            // 正面朝上>30°,左右不超过25°,视为抬起状态
            if (pitch > 30 && (roll < 25 && roll > -25)) {
                if (sensor_state != 1) {
                    new_state_debounce_cnt++;
                    if (new_state_debounce_cnt > 5) {
                        sensor_state = 1;
                        new_state_debounce_cnt = 0;
                    }
                }
            }
            // 平置归位
            if (pitch < 15 && pitch > -15) {
                if (sensor_state != 0) {
                    new_state_debounce_cnt++;
                    if (new_state_debounce_cnt > 5) {
                        sensor_state = 0;
                        new_state_debounce_cnt = 0;
                    }
                }
            }

           // printf("(%s:%d) x = %d, y = %d, z = %d " , 
           //    __FUNCTION__, __LINE__, acc.x >> 4, acc.y >> 4, acc.z >> 4);
           //printf("attitude:pitch=%d,roll=%d, state = %d\r\n\r\n", pitch, roll, sensor_state);
        }
    }
#endif
}

void sensor_init_FaceUp(void)
{  
    u8 tmp_data[6] = {0};

    i2c_master_init(DA217_I2C_ADDR, ((unsigned char)(CLOCK_SYS_CLOCK_HZ / (4 * 400000))));
    _sensor_read_regs(REG_SENSOR_DATA, (u8 *)&tmp_data, 6);

    sensor_data_t acc;
    acc.x = ((s16)(tmp_data[1] << 8 | tmp_data[0])) >> 4;
    acc.y = ((s16)(tmp_data[3] << 8 | tmp_data[2])) >> 4;
    acc.z = ((s16)(tmp_data[5] << 8 | tmp_data[4])) >> 4;

    Face_up_flag = (acc.z < 0);
    printf("--->>> [sensor_init_FaceUp] Face_up_flag = %d\n", Face_up_flag);
}

bool sensor_is_pickup()
{
    if (pre_sensor_state != sensor_state) {
        pre_sensor_state = sensor_state;
        if (sensor_state) {
            return true;
        }    
    }

    return false;
}

bool sensor_factory_check()
{
    //产测用 校验传感器已经是否正常 读三次ID都读不出认为硬件有问题
    _sensor_i2c_init();
    u8 chip_id =0;
    for (int i = 0 ; i <=2 ; i ++)
    {
        chip_id = _sensor_read_reg(REG_WHO_AM_I);
        if (chip_id == DA217_CHIP_ID) {
            return TRUE ;
        }   
    } 
    return  FALSE ;
}

#endif
