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
#include "../key_module/key_handle.h"

#if (GSENSOR_DA217_ENABLE)
//static bool is_sensor_int = false;

_attribute_data_retention_ u32 tick_sensor_refresh = 0;
_attribute_data_retention_ u32 tick_sensor_no_active = 0;
_attribute_data_retention_ u32 sensor_no_active_cnt = 0;
_attribute_data_retention_ u8 sensor_state;
_attribute_data_retention_ u8 pre_sensor_state;
_attribute_data_retention_ u8 new_state_debounce_cnt = 0;
_attribute_data_retention_ bool int_wake_up_level = Level_Low;
_attribute_data_retention_ u8 sensor_chip_flag = 1; //0: NG; 1: MIR3DA267/217/260B; 2: MSA311

static void _sensor_i2c_init()
{
    // 切换I2C总线引脚到sensor 引脚
    //i2c_gpio_set(DA217_I2C_SDA, DA217_I2C_SCL);  	//SDA/CK : PA3/PA4
    //i2c_master_init(DA217_I2C_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)));    // I2C clk
    sensor_chip_flag = sensor_chip_id_set_init();
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

static void sensor_da217_init(void)
{
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
}

static void sensor_da267_init(void)
{
    static sensor_conf_da267_t sensor_config = {

        .resolution_range_da267.hp_en = 0,
        .resolution_range_da267.wdt_en = 0,
        .resolution_range_da267.wdt_time = 0,
        .resolution_range_da267.resolution = SENSOR_RESOLUTION_12b,
        .resolution_range_da267.full_scale = SENSOR_FULLSCALE_2G,

        .mode_odr_da267.perf_mode = LOW_POWER_MODE,
        .mode_odr_da267.perf_level = PERF_LEVEL_0,
        .mode_odr_da267.odr = SENSOR_ODR_100HZ,

        .mode_axis_da267.power_off = 0,
        .mode_axis_da267.self_test = 0,
        .mode_axis_da267.x_disable = 0,
        .mode_axis_da267.y_disable = 0,
        .mode_axis_da267.z_disable = 0,
        .mode_axis_da267.auto_sleep = 0,

        .swap_polarity_da267.x_pol = 0,
        .swap_polarity_da267.y_pol = 0,
        .swap_polarity_da267.z_pol = 0,
        .swap_polarity_da267.xy_swap = 0,

        .fifo_ctrl_da267.mode = SENSOR_FIFO_MODE_BYPASS,
        .fifo_ctrl_da267.fifi_clr = 0,
        .fifo_ctrl_da267.watermark = 0
    };
    _sensor_write_regs(REG_SENSOR_CONFIG, (u8 *)&sensor_config, sizeof(sensor_conf_da267_t));

    static sensor_int_da267_set_t sensor_int_set = {
        .s1_da267.tile = 0,
        .s1_da267.watermark = 0,
        .s1_da267.fifo_full = 0,
        .s1_da267.sign_motion = 0,
        .s1_da267.step = 0,

        .s2_da267.int_source = 1,   // 0: oversample data; 1: unfiltered data; 2: filter data
        .s2_da267.s_tap = 0,
        .s2_da267.d_tap = 0,
        .s2_da267.orient = 0,
        .s2_da267.x_active = 1,
        .s2_da267.y_active = 1,
        .s2_da267.z_active = 1,

        .s3_da267.dnd_enable = 0,
        .s3_da267.dnd_time = SENSOR_DND_TIME_1S,
        .s3_da267.new_data = 0,
        .s3_da267.freefall = 0,
        .s3_da267.active_sel = 0,
    };
    _sensor_write_regs(REG_SENSOR_INT_SET, (u8 *)&sensor_int_set, sizeof(sensor_int_da267_set_t));

    sensor_int_da267_map_t sensor_int_map = {
        .map1_da267.int1_sign_motion = 0,
        .map1_da267.int1_orient = 0,
        .map1_da267.int1_s_tap = 0,
        .map1_da267.int1_d_tap = 0,
        .map1_da267.int1_tilt = 0,
        .map1_da267.int1_active = 1,
        .map1_da267.int1_step = 0,
        .map1_da267.int1_freefall = 0,

        .map2_da267.int1_fifo_full = 0,
        .map2_da267.int1_watermark = 0,
        .map2_da267.int1_new_data = 0,

        .map3_da267.int2_sign_motion = 0,
        .map3_da267.int2_orient = 0,
        .map3_da267.int2_s_tap = 0,
        .map3_da267.int2_d_tap = 0,
        .map3_da267.int2_tilt = 0,
        .map3_da267.int2_active = 0,
        .map3_da267.int2_step = 0,
        
        .map3_da267.int2_freefall = 0,
        .map4_da267.int2_new_data = 0,
        .map4_da267.int2_watermark = 0,
        .map4_da267.int2_fifo_full = 0,
    };
    _sensor_write_regs(REG_SENSOR_INT_MAP, (u8 *)&sensor_int_map, sizeof(sensor_int_da267_map_t));

    sensor_int_conf_da267_t sensor_int_config = {
        .config_da267.int_reset = 1,
        .config_da267.int2_od = 0,    // 0: push-pull; 1: od
        .config_da267.int2_level = 0, // 0: active high; 1: active low
        .config_da267.int1_od = 0,
        .config_da267.int1_level = 1,
        .latch_da267.int2_latch = SENSOR_LATCH_TEMP_LATCHED_1S,
        .latch_da267.int1_latch = SENSOR_LATCH_TEMP_LATCHED_500MS,
    };
    _sensor_write_regs(REG_SENSOR_INT_CONFIG, (u8 *)&sensor_int_config, sizeof(sensor_int_conf_da267_t));

    // 配置active中断
    sensor_func_act_da267_t sensor_func_active = {
        .active_dur_da267.active_duration = 1, // Active duration time = (Active_dur + 1)* ODR_period
        .active_dur_da267.inactive_duration = 1, // inactive duration time = (Inactive_dur + 1)* ODR_period
        .active_dur_da267.active_x_ths = 30, // Threshold of active interrupt=Active_th*K(mg) K = 3.91(2g range),K =7.81(4g range),K=15.625(8g range),K=31.25(16g range).
        .active_dur_da267.active_y_ths = 30, // Threshold of active interrupt=Active_th*K(mg) K = 3.91(2g range),K =7.81(4g range),K=15.625(8g range),K=31.25(16g range).
        .active_dur_da267.active_z_ths = 30, // Threshold of active interrupt=Active_th*K(mg) K = 3.91(2g range),K =7.81(4g range),K=15.625(8g range),K=31.25(16g range).
    };
    _sensor_write_regs(REG_DA267_ACTIVE, (u8 *)&sensor_func_active, sizeof(sensor_func_act_da267_t)); 
}

static void sensor_da260b_init(void)
{
    static sensor_conf_da260b_t sensor_config = {
        // 0x0F
        .resolution_range_da260b.full_scale = SENSOR_FULLSCALE_2G,

        // 0x10
        .mode_odr_da260b.odr = SENSOR_ODR_31HZ25,

        // 0x11
        .mode_axis_da260b.power_off = 0,
        .mode_axis_da260b.bandwitdth = SENSOR_BW_500HZ_2, 
        .mode_axis_da260b.auto_sleep = 0,

        // 0x12
        .swap_polarity_da260b.x_pol = 0,
        .swap_polarity_da260b.y_pol = 0,
        .swap_polarity_da260b.z_pol = 0,
        .swap_polarity_da260b.xy_swap = 0,
    };
    _sensor_write_regs(REG_SENSOR_CONFIG, (u8 *)&sensor_config, sizeof(sensor_conf_da260b_t));

    static sensor_int_da260b_set_t sensor_int_set = {
        .s2_da260b.int_source = 0,   // 0: oversample data; 1: unfiltered data; 2: filter data
        .s2_da260b.x_active = 1,
        .s2_da260b.y_active = 1,
        .s2_da260b.z_active = 0,

        .s3_da260b.new_data = 0,
    };
    _sensor_write_regs(REG_INT_SET1, (u8 *)&sensor_int_set, sizeof(sensor_int_da260b_set_t));

    sensor_int_da260b_map_t sensor_int_map = {
        .map1_da260b.int1_active = 1,

        .map2_da260b.int1_new_data = 0,
    };
    _sensor_write_regs(REG_SENSOR_INT_MAP, (u8 *)&sensor_int_map, sizeof(sensor_int_da260b_map_t));

    sensor_int_conf_da260b_t sensor_int_config = {
        .config_da260b.int_reset = 1,
        .config_da260b.int1_od = 0,
        .config_da260b.int1_level = 1,

        .latch_da260b.int1_latch = SENSOR_LATCH_TEMP_LATCHED_500MS,
    };
    _sensor_write_regs(REG_SENSOR_INT_CONFIG, (u8 *)&sensor_int_config, sizeof(sensor_int_conf_da260b_t));

    // 配置active中断
    sensor_func_act_da260b_t sensor_func_active = {
        .active_dur_da260b.active_duration = 1, // Active duration time = (Active_dur + 1)* ODR_period
        .active_dur_da260b.inactive_duration = 1, // inactive duration time = (Inactive_dur + 1)* ODR_period
        
        .active_dur_da260b.active_ths = 20, // Threshold of active interrupt=Active_th*K(mg) K = 3.91(2g range),K =7.81(4g range),K=15.625(8g range),K=31.25(16g range).
    };
    _sensor_write_regs(REG_SENSOR_ACTIVE, (u8 *)&sensor_func_active, sizeof(sensor_func_act_da260b_t)); 
}

u8 sensor_ic_da267_check(void)
{
    // 1.在初始化时先把sensor的工程模式打开：
    u8 w_buf[3] = {0x83, 0x69, 0xBD};

    foreach(i, 3)
        _sensor_write_regs(0x7f, &w_buf[i], sizeof(w_buf[i]));
   
    // 2.读取C0 寄存器：
    u8 data_m = 0;
    data_m = _sensor_read_reg(0xC0);

    // 3.如下相关寄存做兼容判断：
    u8 asic_18 = 0;
    asic_18 = data_m & 0x38;
    
    // printf("--->>> ic check asic_18 :%1x\r\n", asic_18);

    if (asic_18 == 0x18)
    {
        asic_18 = DA260B_IC; 
        int_wake_up_level = Level_High;
    }
    else if((asic_18 == 0x20) || (asic_18 == 0x28))
    {
        asic_18 = DA267_IC; 
        int_wake_up_level = Level_Low;
    }
    else
    {
        asic_18 = DA217_IC;
        int_wake_up_level = Level_Low;
    }
        

    return asic_18;
}

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

#if DA267_INIT_EN
    u8 sensor_ic_init_chose = sensor_ic_da267_check();
    if (sensor_ic_init_chose == DA217_IC)
    {
        printf("--->>> sensor_da217_init\r\n");
        sensor_da217_init();
    }
    else if (sensor_ic_init_chose == DA267_IC)
    {
        printf("--->>> sensor_da267_init\r\n");
        sensor_da267_init();
    }
    else if (sensor_ic_init_chose == DA260B_IC)
    {
        printf("--->>> sensor_da260b_init\r\n");
        sensor_da260b_init();
    }
#else
    sensor_da217_init();
#endif

    sensor_state = 0;
    pre_sensor_state = 0;
    new_state_debounce_cnt = 0;
   
    cpu_set_gpio_wakeup(DA217_INT1, int_wake_up_level, 1);

    return SENSOR_RESULT_SUCESS;
}

sensor_result_t sensor_init_deepRetn()
{
    _sensor_i2c_init();
    if(sensor_chip_flag == 1)
        sensor_ic_da267_check();
    else if(sensor_chip_flag == 2)
        int_wake_up_level = Level_High;

    cpu_set_gpio_wakeup(DA217_INT1, int_wake_up_level, 1);

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
        if(sensor_chip_flag == 1)
            i2c_master_init(DA217_I2C_ADDR, ((unsigned char)(CLOCK_SYS_CLOCK_HZ / (4 * 400000))));
        if(sensor_chip_flag == 2)
            i2c_master_init(MSA_I2C_ADDR, ((unsigned char)(CLOCK_SYS_CLOCK_HZ / (4 * 400000))));

        _sensor_read_regs(REG_SENSOR_DATA, (u8 *)&tmp_data, 6);

        sensor_data_t acc;
        acc.x = ((s16)(tmp_data[1] << 8 | tmp_data[0])) >> 4;

        acc.y = ((s16)(tmp_data[3] << 8 | tmp_data[2])) >> 4;

        acc.z = ((s16)(tmp_data[5] << 8 | tmp_data[4])) >> 4;

        Face_up_flag = (acc.z < 0);
        //printf("--->>> Face_up_flag = %d\n", Face_up_flag);
        //get_data_report_device_fwversion_Gsensor(tmp_data[0], tmp_data[1], tmp_data[2], tmp_data[3], tmp_data[4], tmp_data[5]);
    }
}

void sensor_init_FaceUp(void)
{  
    u8 tmp_data[6] = {0};

    if(sensor_chip_flag == 1)
        i2c_master_init(DA217_I2C_ADDR, ((unsigned char)(CLOCK_SYS_CLOCK_HZ / (4 * 400000))));
    if(sensor_chip_flag == 2)
        i2c_master_init(MSA_I2C_ADDR, ((unsigned char)(CLOCK_SYS_CLOCK_HZ / (4 * 400000))));
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

/******************************************************************
 * @brief  sensor_chip_id_set_init
 * @param  none
 * @return none
 * @retval void
 */
int sensor_chip_id_set_init(void)
{
    i2c_gpio_set(DA217_I2C_SDA, DA217_I2C_SCL);
    i2c_master_init(DA217_I2C_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)));
    u8 chip_id = _sensor_read_reg(REG_WHO_AM_I);
    if (chip_id == DA217_CHIP_ID) {
        printf("[G_Sensor: MIR3DA267/217/260B], chip_id = 0x%02x\r\n",chip_id);
        return 1;
    } else {
        printf("[G_Sensor: NG / Retry], id = 0x%02x \r\n",chip_id);
    }    

    i2c_master_init(MSA_I2C_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)));
    chip_id = _sensor_read_reg(REG_WHO_AM_I);
    if (chip_id == MSA311_CHIP_ID) {
        printf("[G_Sensor: MSA311], chip_id = 0x%02x\r\n",chip_id);
        return 2;
    } else {
        printf("[G_Sensor: NG!!!], id = 0x%02x \r\n",chip_id);
        return 0;
    }     
}

#endif
