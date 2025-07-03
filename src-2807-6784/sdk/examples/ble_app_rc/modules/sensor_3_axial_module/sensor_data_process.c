

#include "sensor_data_process.h"
#include "da228ec_driver.h"
#include "app_mode.h"
#include "math.h"

#define APP_LOG_DOMAIN      "module"
#define APP_LOG_LEVEL       APP_MODULE_LOG_LEVEL
#include "co_debug.h"


/**
 * @brief 计算俯仰角度（单位：度）
 * @param sensor 加速度数据
 * @return 俯仰角度（单位：度）
 */
float calculate_pitch(SENSOR_3_Axial_Data_t *sensor) 
{
    // 确保加速度计数据正确解析
    float x = sensor->x; // 假设数据范围是 -1g 到 1g
    float y = sensor->y;
    float z = sensor->z;

    // 计算俯仰角度（单位：弧度）
    float pitch_rad = atan2(x, sqrt(y * y + z * z));
    // 转换为度
    float pitch_deg = pitch_rad * (180.0 / PI);
    return pitch_deg;
}

/**
 * @brief 计算横滚角度（单位：度）
 * @param sensor 加速度数据
 * @return 横滚角度（单位：度）
 */
float calculate_roll(SENSOR_3_Axial_Data_t *sensor) 
{
    // 确保加速度计数据正确解析
    float y = sensor->y;
    float z = sensor->z;

    // 计算横滚角度（单位：弧度）
    float roll_rad = atan2(y, z);
    // 转换为度
    float roll_deg = roll_rad * (180.0 / PI);
    return roll_deg;
}

// 示例：在主函数或其他事件循环中调用此函数
void sensor_event_handler(void) 
{
    static uint32_t tick_sensor_refresh = 0;
    static bool is_backlight_on = false;
    static uint32_t backlight_timer = 0;
    static float last_pitch_angle = 0;
    DA228EC_Data_t sensor;
    float pitch_angle, roll_angle;

    // 定时读取传感器数据
    if (remote_clock_exceed(tick_sensor_refresh, 5 * 100 * 1000) || da228ec_get_interrupt_status()) // 500ms刷新一次
    { 
        if (da228ec_get_interrupt_status())
        {
            da228ec_clear_interrupt();
        }
        tick_sensor_refresh = remote_clock();
        da228ec_read_acceleration(&sensor);

        // 计算俯仰角度和横滚角度
        pitch_angle = calculate_pitch((SENSOR_3_Axial_Data_t *)&sensor);
        roll_angle = calculate_roll((SENSOR_3_Axial_Data_t *)&sensor);

        // 检测是否需要开启背光（基于俯仰角度）
        if (!is_backlight_on && pitch_angle >= 30.0 && last_pitch_angle <= 10.0) 
        {
            is_backlight_on = true;
            backlight_timer = remote_clock();
            // 调用背光开启函数
            //turn_on_backlight();
            APP_LOG_INF("turn_on_backlight\n");
        }
        last_pitch_angle = pitch_angle;

        APP_LOG_INF("Backlight turned on. Pitch angle: %.2f°, Roll angle: %.2f°\n", pitch_angle, roll_angle);
    }
    da228ec_read_acceleration(&sensor);

    // 计算俯仰角度和横滚角度
    pitch_angle = calculate_pitch((SENSOR_3_Axial_Data_t *)&sensor);
    roll_angle = calculate_roll((SENSOR_3_Axial_Data_t *)&sensor);
    APP_LOG_INF("Backlight turned on. Pitch angle: %.2f°, Roll angle: %.2f°\n", pitch_angle, roll_angle);

    // 检查背光是否需要关闭
    if (is_backlight_on && remote_clock_exceed(backlight_timer, 4 * 1000 * 1000)) { // 4秒后关闭
        is_backlight_on = false;
        // 调用背光关闭函数
        //turn_off_backlight();
        APP_LOG_INF("Backlight turned off\n");
    }

}

void turn_on_backlight(void) {
    // 实现背光开启逻辑
}

void turn_off_backlight(void) {
    // 实现背光关闭逻辑
}