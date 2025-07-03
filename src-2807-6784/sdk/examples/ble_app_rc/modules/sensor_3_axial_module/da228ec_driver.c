/**
**********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     da228ec_driver.c
* @brief    DA228EC加速度传感器驱动实现
* @details  本文件实现了DA228EC加速度传感器的驱动功能，包括初始化、数据读取、配置等
* @author   lihuashe
* @date     2025-6-23
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "da228ec_driver.h"
#include "board_demo.h"
#include "i2c.h"  // 假设存在I2C驱动
#include "gpio.h"


#define APP_LOG_DOMAIN      "module"
#define APP_LOG_LEVEL       APP_MODULE_LOG_LEVEL
#include "co_debug.h"

#if (DA228EC_DRIVER_EN)
/*============================================================================*
 *                              Definitions
 *============================================================================*/
#define DA228EC_DEFAULT_ODR	                DA228EC_ODR_125HZ     // 默认输出频率125Hz
#define DA228EC_DEFAULT_RANGE	            DA228EC_RANGE_4G      // 默认量程±4g

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
static uint8_t da228ec_last_odr = DA228EC_DEFAULT_ODR;
static uint8_t da228ec_last_range = DA228EC_DEFAULT_RANGE;
static bool da228ec_interrupt_status = false;
/*============================================================================*
 *                              Function Implementations
 *============================================================================*/


bool da2288ec_i2c_read_reg(uint8_t reg_addr, uint8_t *data)
{
    i2c_master_write(XH_I2C, DA228EC_ADDR, &reg_addr, 1, NULL);
    return i2c_master_read(XH_I2C, DA228EC_ADDR, data, 1, NULL);
}

bool da2288ec_i2c_write_reg(uint8_t reg_addr, uint8_t data)
{
    uint8_t tx_buf[2] = {reg_addr, data};

    return i2c_master_write(XH_I2C, DA228EC_ADDR, tx_buf, 2, NULL);
}

bool da2288ec_i2c_write_reg_nbyte(uint8_t reg_addr, uint8_t *data, uint8_t data_len)
{
    uint8_t tx_buf[2] = {reg_addr};
    memcpy(&tx_buf[1], data, data_len);

    return i2c_master_write(XH_I2C, DA228EC_ADDR, tx_buf, data_len + 1, NULL);
}


/**
 * @brief 初始化I2C跟中断
 * @param  
 */
void da228ec_init(void) 
{
    APP_LOG_INF("--->>> da228ec_init\r\n");

    // 查找 SCL 和 SDA 引脚
    int scl_pin = 0;
    int sda_pin = 1;
    // 确保引脚查找成功
    co_assert(scl_pin >= 0 && sda_pin >= 0);

    // 配置 SCL 和 SDA 引脚为 GPIO 模式
    pinmux_config(scl_pin, PINMUX_GPIO_MODE_CFG);
    pinmux_config(sda_pin, PINMUX_GPIO_MODE_CFG);

    // 设置 SCL 为输出，SDA 为输入
    gpio_set_direction(BITMASK(scl_pin), GPIO_OUTPUT);
    gpio_set_direction(BITMASK(sda_pin), GPIO_INPUT);

    // 等待 SDA 线释放（如果被拉低）
    int unlock_timeout = 200;
    while ((gpio_read(BITMASK(sda_pin)) == 0) && (--unlock_timeout > 0))
    {
        gpio_toggle(BITMASK(scl_pin));
        co_delay_10us(10);
    }

    // 恢复 SCL 和 SDA 引脚的 I2C 功能
    pinmux_config(scl_pin, PINMUX_I2C0_MST_SCK_CFG);
    pinmux_config(sda_pin, PINMUX_I2C0_MST_SDA_CFG);

    i2c_open(XH_I2C, I2C_MODE_MASTER, 400000);


    // 读取芯片ID验证
    uint8_t chip_id;
    if (!da228ec_check_id(&chip_id)) {
        // 芯片ID验证失败处理
        APP_LOG_INF("--->>> scl_pin %d  sda_pin %d \r\n", scl_pin, sda_pin);
        return;
    }
    
    // 设置默认量程和输出频率
    da228ec_set_range((DA228EC_RANGE_DEF)DA228EC_DEFAULT_RANGE);
    da228ec_set_odr((DA228EC_ODR_DEF)DA228EC_DEFAULT_ODR);
    
    // 设置正常模式
    da228ec_set_mode(DA228EC_MODE_NORMAL);

    da228ec_config_init();
}

/**
 * @brief 中断IO初始化
 * @param  
 */
void da228ec_int_gpio_init(void)
{
    // 配置 SCL 和 SDA 引脚为 GPIO 模式
    pinmux_config(DA228EC_INT_PIN, PINMUX_GPIO_MODE_CFG);

    // 设置GPIO方向为输入
    gpio_set_direction(DA228EC_INT_PIN, GPIO_INPUT);

    // 设置GPIO中断回调
    gpio_set_interrupt_callback(da228ec_interrupt_handle);

    // 设置GPIO中断类型为上升沿触发
    gpio_set_interrupt(DA228EC_INT_PIN, GPIO_RISING_EDGE);

}


/**
 * @brief 初始化传感器寄存器配置
 * @param  
 */
void da228ec_config_init(void) 
{
    // 初始化代码示例
    sensor_conf_da228ec_t conf = {
        .resolution_range_da228ec = {.full_scale = DA228EC_RANGE_4G},
        .mode_odr_da228ec = {.odr = DA228EC_ODR_125HZ},
        .mode_axis_da228ec = {.auto_sleep = DA228EC_FUN_ENABLE, .power_off = 0},
        .swap_polarity_da228ec = {.xy_swap = 0, .z_pol = 0, .y_pol = 0, .x_pol = 0}
    };

    sensor_int_da228ec_set_t int_set = {
        .s1_da228ec = {.x_active = DA228EC_FUN_ENABLE, .y_active = DA228EC_FUN_ENABLE, .z_active = DA228EC_FUN_ENABLE, .int_source = 0},
        .s2_da228ec = {.new_data = DA228EC_FUN_ENABLE}
    };

    sensor_int_da228ec_map_t int_map = {
        .map1_da228ec = {.int1_active   = DA228EC_FUN_ENABLE},
        .map2_da228ec = {.int1_new_data = DA228EC_FUN_ENABLE}
    };

    sensor_int_conf_da228ec_t int_conf = {
        .config_da228ec = {.int1_level = 0, .int1_od = DA228EC_FUN_ENABLE, .int_reset = 0},
        .latch_da228ec = {.int1_latch = 0x00}
    };

    sensor_func_act_da228ec_t func_act = {
        .active_dur_da228ec = {.active_duration = 0x00, .inactive_duration = 0x00, .active_ths = 0x14}
    };

    // 写入配置寄存器
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_RANGE, (uint8_t *)&(conf.resolution_range_da228ec), 1);
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_ODR_AXIS, (uint8_t *)&(conf.mode_odr_da228ec), 1);
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_MODE_BW, (uint8_t *)&(conf.mode_axis_da228ec), 1);
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_SWAP_POLARITY, (uint8_t *)(&conf.swap_polarity_da228ec), 1);

    // 写入中断设置寄存器
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_INT_SET1, (uint8_t *)(&int_set.s1_da228ec), 1);
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_INT_SET2, (uint8_t *)(&int_set.s2_da228ec), 1);

    // 写入中断映射寄存器
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_INT_MAP1, (uint8_t *)(&int_map.map1_da228ec), 1);
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_INT_MAP2, (uint8_t *)(&int_map.map2_da228ec), 1);

    // 写入中断配置寄存器
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_INT_CONFIG, (uint8_t *)(&int_conf.config_da228ec), 1);
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_INT_LATCH, (uint8_t *)(&int_conf.latch_da228ec), 1);

    // 写入活动功能寄存器
    uint8_t active_dur_reg = (func_act.active_dur_da228ec.active_duration << 4) | func_act.active_dur_da228ec.inactive_duration;
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_ACTIVE_DUR, &active_dur_reg, 1);
    da2288ec_i2c_write_reg_nbyte(DA228EC_REG_ACTIVE_THS, (uint8_t *)(&func_act.active_dur_da228ec.active_ths), 1);
}



/**
 * @brief 校验芯片id
 * @param chip_id 
 * @return 
 */
bool da228ec_check_id(uint8_t *chip_id) 
{
    int retries = 3;

    while (retries > 0)
    {
        // 通过 I2C 读取芯片 ID 寄存器
        if (da2288ec_i2c_read_reg(DA228EC_REG_CHIPID, chip_id) == DA228EC_I2C_OK)
        {
            //APP_LOG_INF("init i2c succus chip_id %d\r\n", *chip_id);
            // 验证芯片 ID 是否正确（假设为 0x13）
            return (*chip_id == 0x13);
        }
        retries--;
    }

    // 如果 3 次读取都失败，返回 false
    return false;
}

/**
 * @brief 读取加速度数据
 * @param data 
 */
void da228ec_read_acceleration(DA228EC_Data_t *data) 
{
    DA228EC_Data_Msb_Lsb_t msb_lsb[3];

    // 读取高字节和低字节
    for (uint8_t i = 0; i < sizeof(msb_lsb); i++)
    {
        da2288ec_i2c_read_reg(DA228EC_REG_ACC_X_LSB + i, (uint8_t *)(&msb_lsb[0].lsb) + i);//获取x->z 轴
    }

    // 组合数据并左对齐
    for (uint8_t i = 0; i < sizeof(msb_lsb)/sizeof(msb_lsb[0]); i++)
    {
        *((uint16_t *)(&(data->x)) + i) = (int16_t)((msb_lsb[i].msb << 4) | (msb_lsb[i].lsb >> 4));

        // 处理二进制补码（如果需要）
        if (*((uint16_t *)(&(data->x)) + i) & 0x800) // 检查符号位
        { 
            *((uint16_t *)(&(data->x)) + i) |= 0xFFFFF000; // 扩展符号位
        }
    }
    
}

/**
 * @brief 
 * @param range 量程
 */
void da228ec_set_range(DA228EC_RANGE_DEF range) 
{
    uint8_t reg_val;
    
    // 读取当前量程寄存器值
    if (da2288ec_i2c_read_reg(DA228EC_REG_RANGE, &reg_val) != DA228EC_I2C_OK) {
        return;
    }
    
    // 清除量程位并写入新值
    reg_val &= ~DA228EC_MSK_RANGE;
    reg_val |= (uint8_t)range;
    
    // 写回量程寄存器
    if (da2288ec_i2c_write_reg(DA228EC_REG_RANGE, reg_val) != DA228EC_I2C_OK) {
        return;
    }
    
    da228ec_last_range = (uint8_t)range;
}

/**
 * @brief 设置回报率
 * @param odr 
 */
void da228ec_set_odr(DA228EC_ODR_DEF odr) 
{
    uint8_t reg_val;
    
    // 读取当前输出频率寄存器值
    if (da2288ec_i2c_read_reg(DA228EC_REG_ODR_AXIS, &reg_val) != DA228EC_I2C_OK) {
        return;
    }
    
    // 清除输出频率位并写入新值
    reg_val &= ~DA228EC_MSK_ODR;
    reg_val |= (uint8_t)odr;
    
    // 写回输出频率寄存器
    if (da2288ec_i2c_write_reg(DA228EC_REG_ODR_AXIS, reg_val) != DA228EC_I2C_OK) {
        return;
    }
    
    da228ec_last_odr = (uint8_t)odr;
}

/**
 * @brief 模式设置
 * @param mode 正常、挂起
 */
void da228ec_set_mode(DA228EC_MODE_DEF mode) 
{
    uint8_t reg_val;
    
    // 读取当前模式寄存器值
    if (da2288ec_i2c_read_reg(DA228EC_REG_MODE_BW, &reg_val) != DA228EC_I2C_OK) {
        return;
    }
    
    // 清除模式位并写入新值
    reg_val &= ~DA228EC_MSK_MODE;
    reg_val |= (uint8_t)(mode << 7);
    
    // 写回模式寄存器
    if (da2288ec_i2c_write_reg(DA228EC_REG_MODE_BW, reg_val) != DA228EC_I2C_OK) {
        return;
    }
}

/**
 * @brief 使能中断
 * @param interrupt 
 */
void da228ec_enable_interrupt(DA228EC_INTERRUPT_DEF interrupt) 
{
    uint8_t reg_val;
    
    // 根据中断类型选择对应的寄存器
    if (interrupt == DA228EC_INT_ACTIVE) {
        if (da2288ec_i2c_read_reg(DA228EC_REG_INT_MAP1, &reg_val) != DA228EC_I2C_OK) {
            return;
        }
        reg_val |= 0x01;  // 使能活动中断
    } else {
        if (da2288ec_i2c_read_reg(DA228EC_REG_INT_MAP2, &reg_val) != DA228EC_I2C_OK) {
            return;
        }
        reg_val |= 0x01;  // 使能新数据中断
    }
    
    // 写回中断映射寄存器
    if (interrupt == DA228EC_INT_ACTIVE) {
        da2288ec_i2c_write_reg(DA228EC_REG_INT_MAP1, reg_val);
    } else {
        da2288ec_i2c_write_reg(DA228EC_REG_INT_MAP2, reg_val);
    }
}

/**
 * @brief 失能中断
 * @param interrupt 
 */
void da228ec_disable_interrupt(DA228EC_INTERRUPT_DEF interrupt) 
{
    uint8_t reg_val;
    
    // 根据中断类型选择对应的寄存器
    if (interrupt == DA228EC_INT_ACTIVE) {
        if (da2288ec_i2c_read_reg(DA228EC_REG_INT_MAP1, &reg_val) != DA228EC_I2C_OK) {
            return;
        }
        reg_val &= ~0x01;  // 禁用活动中断
    } else {
        if (da2288ec_i2c_read_reg(DA228EC_REG_INT_MAP2, &reg_val) != DA228EC_I2C_OK) {
            return;
        }
        reg_val &= ~0x01;  // 禁用新数据中断
    }
    
    // 写回中断映射寄存器
    if (interrupt == DA228EC_INT_ACTIVE) {
        da2288ec_i2c_write_reg(DA228EC_REG_INT_MAP1, reg_val);
    } else {
        da2288ec_i2c_write_reg(DA228EC_REG_INT_MAP2, reg_val);
    }
}

/**
 * @brief 检查是否为新数据
 * @param  
 * @return 
 */
bool da228ec_check_new_data(void) 
{
    uint8_t reg_val;
    
    // 读取新数据标志寄存器
    if (da2288ec_i2c_read_reg(DA228EC_REG_NEWDATA_FLAG, &reg_val) != DA228EC_I2C_OK) {
        return false;
    }
    
    // 检查新数据标志位
    return (reg_val & 0x01) != 0;
}

/**
 * @brief 中断处理
 * @param  
 */
void da228ec_interrupt_handle(uint32_t pin_mask)
{
    APP_LOG_INF("da228ec_interrupt_handle\r\n");
    if (pin_mask == DA228EC_INT_PIN)
        if (!da228ec_interrupt_status)
            da228ec_interrupt_status = true;
}


/**
 * @brief 获取中断状态
 * @param  
 * @return 
 */
bool da228ec_get_interrupt_status(void)
{
    return da228ec_interrupt_status;
}

/**
 * @brief 清除中断
 * @param  
 */
void da228ec_clear_interrupt(void)
{
    da228ec_interrupt_status = false;
}


#endif
/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/