/********************************************************************************************************
 * @file	da217_driver.c
 *
 * @details This file contains code for calculating pitch Angle and roll Angle using IIC
 *          communication to obtain gyroscope data for initial configuration and accelerometer data.
 *
 * @note    Please ensure to replace the data set with real-time sensor data in actual implementation.
 *
 * @author  huzhuohuan
 * @date    2024-06-02
 * @version 1.0
 ********************************************************************************************************/

#include "ft3308_driver.h"
#include "../key_module/key_driver.h"
#include "../keyboard_rcu.h"
#include "ft3308_handle.h"
#if (TOUCH_ENABLE)
Ft3308_driver ft3308;
bool ft_touch_sleep_flag = false;
bool flit_min_interval_flag = false;
bool Mouse_move_rf_send_flag = false;
bool two_keybroad_flag = false;
volatile u32 ft_touch_refresh_time = 0;
volatile u32 check_touch_time = 0;
Mouse_state mouse_state = MOUSE_NULL;
_attribute_data_retention_ Mouse_gear Mouse_move_gear = MOVE_GEAR_ONE;

/**
 * @brief    Interrupt handler for touch events.
 * @details  This function is called when a touch interrupt is triggered. It checks the source of the interrupt,
 *           clears the relevant interrupt flag, and performs actions based on the state of the interrupt trigger pin.
 *           If the pin remains low for a certain period, it sets flags and updates timestamps for touch events.
 *
 * @note     The function is marked with _attribute_ram_code_sec_noinline_ to indicate it should be placed in RAM and not be inlined.
 */
void touch_irq_handler(void)
{
    if ((reg_irq_src & FLD_IRQ_GPIO_EN) == FLD_IRQ_GPIO_EN)
    {
        reg_irq_src = FLD_IRQ_GPIO_EN; // clear the relevant irq

        rcu_wakeup_touch = 0x02;
        ft_touch_sleep_flag = true;
        ft_touch_refresh_time = clock_time();
        rf_24G_deep_tick = ft_touch_refresh_time;
        check_touch_time = ft_touch_refresh_time;
        if (ft3308.touch_press.touch_press == 0 && ft3308.touch_press.touch_time == 0)
            ft3308.touch_press.touch_time = ft_touch_refresh_time;
    }
}

/**
 * @brief    Processes mouse swipe data and executes corresponding actions.
 * @details  Determines the direction of the swipe based on the swipe_value parameter,
 *           updates the mouse state, prints the appropriate message, and reports the gesture.
 *
 * @param    swipe_value The swipe data of the mouse, which determines the direction of the swipe,
 *                        a value of type uint8_t.
 * 
 * @param    fast_mode   If true, the mouse movement will be executed in fast mode.
 */
void Mouse_move_rf_send_data(u8 swipe_value , bool fast_mode)
{
    if (swipe_value == SWIPE_UP)
    {
        mouse_state = MOUSE_ROLL;
        printf("--->>> Roll up\r");
        get_data_report_gesture(SWIPE_UP, fast_mode);
    }
    else if (swipe_value == SWIPE_DOWN)
    {
        mouse_state = MOUSE_ROLL;
        printf("--->>> Roll down\r");
        get_data_report_gesture(SWIPE_DOWN, fast_mode);
    }
    else if (swipe_value == SWIPE_LEFT)
    {
        mouse_state = MOUSE_ROLL;
        printf("--->>> Roll left\r");
        get_data_report_gesture(SWIPE_LEFT, fast_mode);
    }
    else if (swipe_value == SWIPE_RIGHT)
    {
        mouse_state = MOUSE_ROLL;
        printf("--->>> Roll right\r");
        get_data_report_gesture(SWIPE_RIGHT, fast_mode);
    }
    else if (swipe_value == SWIPE_ZOOM_IN)
    {
        mouse_state = MOUSE_ZOOM;
        printf("--->>> Zoom in\r");
        get_data_report_gesture(SWIPE_ZOOM_IN, fast_mode);
    }
    else if (swipe_value == SWIPE_ZOOM_OUT)
    {
        mouse_state = MOUSE_ZOOM;
        printf("--->>> Zoom out\r");
        get_data_report_gesture(SWIPE_ZOOM_OUT, fast_mode);
    }
}

/**
 * @brief    Set the mouse movement gear.
 * @details   This allows the user to switch between different movement distance settings.
 */
void Mouse_move_gear_set(void)
{
    if (Mouse_move_gear == MOVE_GEAR_TWO)
        Mouse_move_gear = MOVE_GEAR_THREE;
    else if (Mouse_move_gear == MOVE_GEAR_THREE)
        Mouse_move_gear = MOVE_GEAR_ONE;
    else if (Mouse_move_gear == MOVE_GEAR_ONE)
        Mouse_move_gear = MOVE_GEAR_TWO;
}

/**
 * @brief      Reads data from a register in the FT3308.
 * @param[in]  addr     - The address of the register to read from.
 * @param[out] data_m   - Pointer to the buffer to store the read data.
 * @param[in]  len      - The number of bytes to read.
 * @return     0 if successful, non-zero if error.
 */
s8 FT3308_register_read(u8 addr, u8 *data_m, u8 len)
{
    i2c_master_init(FT3308_SLAVE_DMA_ADDR, FT3308_SLAVE_DIVCLOCK);

    i2c_read_series(addr, FT3308_SLAVE_ADD_LED, (unsigned char *)data_m, len);

    return *data_m;
}

/**
 * @brief      Writes a byte of data to a register in the FT3308.
 * @param[in]  addr    - The address of the register to write to.
 * @param[in]  data_m  - The data byte to write.
 * @return     0 if successful, non-zero if error.
 */
s8 FT3308_register_write_byte(u8 addr, u8 data_m)
{
    static s8 res;
    i2c_master_init(FT3308_SLAVE_DMA_ADDR, FT3308_SLAVE_DIVCLOCK);

    i2c_write_byte(addr, FT3308_SLAVE_ADD_LED, data_m);

    return res;
}

/**
 * @brief    Configure the GPIO pin of the FT3308 touch chip.
 */
void FT3308_gpio_config(void)
{
#if (!TOUCH_INT_MONITOR_EN)
    gpio_set_output_en(FT3308_CHIP_DRIVER_PIN, 1);
    gpio_set_input_en(FT3308_CHIP_DRIVER_PIN, 0);
    gpio_setup_up_down_resistor(FT3308_CHIP_DRIVER_PIN, PM_PIN_PULLDOWN_100K);
    gpio_set_func(FT3308_CHIP_DRIVER_PIN, AS_GPIO);
    gpio_write(FT3308_CHIP_DRIVER_PIN, LOW_LEVEL_VOLTAGE);

    gpio_set_output_en(FT3308_RESETER, 1);
    gpio_set_input_en(FT3308_RESETER, 0);
    gpio_setup_up_down_resistor(FT3308_RESETER, PM_PIN_PULLUP_10K);
    gpio_set_func(FT3308_RESETER, AS_GPIO);
    gpio_write(FT3308_RESETER, HIGH_LEVEL_VOLTAGE);

    gpio_set_func(INT_TRIGGER_PIN, AS_GPIO);
    gpio_set_output_en(INT_TRIGGER_PIN, 0);
    gpio_set_input_en(INT_TRIGGER_PIN, 1);
    gpio_setup_up_down_resistor(INT_TRIGGER_PIN, PM_PIN_PULLUP_10K);
#endif

#if FT3308_INT_TIRGGIR_ENABLE
    gpio_set_interrupt(INT_TRIGGER_PIN, POL_FALLING);
    cpu_set_gpio_wakeup(INT_TRIGGER_PIN, LOW_LEVEL_VOLTAGE, 1);
#endif
}

/**
 * @brief    Initializes the FT3308 touch chip configuration.
 * @details  Writes a specific byte value to a specified register address to initialize the FT3308 chip.
 */
void FT3308_sleep_config(void)
{
    FT3308_register_write_byte(0xA5, 0x01);
}

/**
 * @brief    Puts the FT3308 touch chip into work mode.
 * @details  Writes a specific byte value to a specified register address to change the chip's mode.
 *           This function sets the chip to work mode, allowing it to operate normally and respond to touch inputs.
 */
void FT3308_into_work_mode_config(void)
{
    FT3308_register_write_byte(0x00, 0x00);
}

/**
 * @brief    Enables the FT3308 touch chip.
 * @details  Sets the FT3308 chip driver pin to high voltage to enable the chip.
 */
void FT3308_chip_enble(void)
{
    gpio_write(FT3308_CHIP_DRIVER_PIN, HIGH_LEVEL_VOLTAGE);
}

/**
 * @brief    Resets the FT3308 touch chip.
 * @details  This function performs a reset sequence for the FT3308 touch chip by toggling the reset pin.
 */
void FT3308_chip_rsin(void)
{
    gpio_write(FT3308_RESETER, LOW_LEVEL_VOLTAGE);

    WaitMs(10);

    gpio_write(FT3308_RESETER, HIGH_LEVEL_VOLTAGE);

    // WaitMs(100);
}

/**
 * @brief    Initializes the I2C communication for the FT3308 touch chip.
 * @details  This function sets up the GPIO configuration, resets the chip, and initializes the chip's configuration.
 */
void FT3308_iic_init(void)
{
    FT3308_gpio_config();

#if (!TOUCH_INT_MONITOR_EN)
    FT3308_chip_rsin();
#endif

    FT3308_into_work_mode_config();
}

/**
 * @brief Calculates the coordinate offset between the current touch data and the stored touch data for the FT3308 touch chip.
 * @details This function determines the difference in x and y coordinates between the current touch position and the previously stored position.
 */
void FT3308_coordinate_offset()
{
    if (ft3308.ts_now_data.x < ft3308.ts_store.x)
    {
        ft3308.Ts_move.move_x = ft3308.ts_now_data.x - ft3308.ts_store.x;
    }
    else if (ft3308.ts_now_data.x > ft3308.ts_store.x)
    {
        ft3308.Ts_move.move_x = ft3308.ts_now_data.x - ft3308.ts_store.x;
    }
    else
    {
        ft3308.Ts_move.move_x = 0;
    }
    if (ft3308.ts_now_data.y > ft3308.ts_store.y)
    {
        ft3308.Ts_move.move_y = ft3308.ts_store.y - ft3308.ts_now_data.y;
    }
    else if (ft3308.ts_now_data.y < ft3308.ts_store.y)
    {
        ft3308.Ts_move.move_y = ft3308.ts_store.y - ft3308.ts_now_data.y;
    }
    else
    {
        ft3308.Ts_move.move_y = 0;
    }
}

/**
 * @brief Adjusts the movement speed based on the magnitude of the coordinate offsets for the FT3308 touch chip.
 * @details This function modifies the movement speed by adjusting the x and y coordinate offsets stored in the ft3308.Ts_move structure.
 */
void FT3308_move_speed_change()
{
    // printf("--->>> real move x,y : %d , %d\r\n", ft3308.Ts_move.move_x, ft3308.Ts_move.move_y);  
    if (((abs(ft3308.Ts_move.move_x) >= 10) && (abs(ft3308.Ts_move.move_x) < 15)) ||
        (((abs(ft3308.Ts_move.move_y) >= 10)) && ((abs(ft3308.Ts_move.move_y) < 15))))
    {
        ft3308.Ts_move.move_x = ((ft3308.Ts_move.move_x * 15) / 10);
        ft3308.Ts_move.move_y = ((ft3308.Ts_move.move_y * 15) / 10);
    }
    else if ((abs(ft3308.Ts_move.move_x) >= 15) || ((abs(ft3308.Ts_move.move_y) >= 15)))
    {
        ft3308.Ts_move.move_x = (ft3308.Ts_move.move_x * 2);
        ft3308.Ts_move.move_y = (ft3308.Ts_move.move_y * 2);
    }
}

/**
 * @brief    Triggers a scan for the FT3308 touch chip.
 * @details  This function checks the interrupt trigger pin and reads data from the FT3308 chip if an interrupt is detected.
 */
void FT3308_scan_trigger(void)
{
#if FT3308_INT_TIRGGIR_ENABLE
    u8 buf[] = {0};
    FT3308_register_read(0x00, buf, FT3308_READ_DATA_LEN);
    // printf("[FT3308_BUF] ");
    // for (int i = 0; i < FT3308_READ_DATA_LEN; i++)
    //     printf("%01x ", buf[i]);
    // printf("\r\n");

    ft3308.ts_now_data.x = ((((buf[FTS_TOUCH_Y_H_POS] & 0x0F) << 8) + (buf[FTS_TOUCH_Y_L_POS] & 0xFF)) << Mouse_move_gear);
    ft3308.ts_now_data.y = ((((buf[FTS_TOUCH_X_H_POS] & 0x0F) << 8) + (buf[FTS_TOUCH_X_L_POS] & 0xFF)) << Mouse_move_gear);
    ft3308.ts_now_data.x1 = ((((buf[FTS_TOUCH_Y_H_1_POS] & 0x0F) << 8) + (buf[FTS_TOUCH_Y_L_1_POS] & 0xFF)) << Mouse_move_gear);
    ft3308.ts_now_data.y1 = ((((buf[FTS_TOUCH_X_H_1_POS] & 0x0F) << 8) + (buf[FTS_TOUCH_X_L_1_POS] & 0xFF)) << Mouse_move_gear);
    ft3308.ts_now_data.two_keys = (buf[FTS_TOUCH_POINT_NUM] & 0xFF);

    if (check_touch_time && !clock_time_exceed(check_touch_time, 1 * 1000) && !clock_time_exceed(ft3308.touch_press.touch_time, 24 * 1000))
    {

        Shape_ret_data check_ret_shape = SHAPE_RET_DATA_INIT;
        check_ret_shape = move_shape_map(ft3308.ts_now_data.x, ft3308.ts_now_data.y, ft3308.ts_now_data.x1, ft3308.ts_now_data.y1);

        if (ft3308.ts_now_data.two_keys == 0x02)
        {
            two_keybroad_flag = true;
        }

        if (check_ret_shape.interval == WRONG_INTERVAL)
        {
            return;
        }
    }

    if (check_touch_time && !clock_time_exceed(check_touch_time, 12 * 1000) && clock_time_exceed(ft3308.touch_press.touch_time, 24 * 1000))
    {

        if ((ft3308.ts_store.x != ft3308.ts_now_data.x) || (ft3308.ts_store.y != ft3308.ts_now_data.y))
        {
            if (ft3308.ts_store.x == 0 && ft3308.ts_store.y == 0)
            {
                ft3308.ts_store.x = ft3308.ts_now_data.x;
                ft3308.ts_store.y = ft3308.ts_now_data.y;
                ft3308.ts_store.x1 = ft3308.ts_now_data.x1;
                ft3308.ts_store.y1 = ft3308.ts_now_data.y1;
            }

            if (ft3308.ts_store.key_num == 0)
            {
                ft3308.ts_store.key_num = ft3308.ts_now_data.two_keys;
            }

            FT3308_coordinate_offset();

            FT3308_move_speed_change();
        }

        if ((ft3308.ts_store.x != ft3308.ts_now_data.x) || (ft3308.ts_store.y != ft3308.ts_now_data.y) ||
            (ft3308.ts_store.x1 != ft3308.ts_now_data.x1) || (ft3308.ts_store.y1 != ft3308.ts_now_data.y1))
        {
            Shape_ret_data ret_shape = SHAPE_RET_DATA_INIT;

            ft3308.Ts_move.data_filt++;

            if (ft3308.ts_store.key_num != ft3308.ts_now_data.two_keys)
            {
                ft3308.ts_store.key_num = ft3308.ts_now_data.two_keys;
                memset(&ft3308, FT3308_DATA_INIT, sizeof(ft3308));
            }

            ret_shape = move_shape_map(ft3308.ts_now_data.x, ft3308.ts_now_data.y, ft3308.ts_now_data.x1, ft3308.ts_now_data.y1);
            
            if (flit_min_interval_flag == false && ret_shape.interval == WRONG_INTERVAL)
            {
                flit_min_interval_flag = true;
            }

            if (ft3308.ts_now_data.two_keys == 0x01)
            {
                ft3308.Ts_move.touch_key_one_num++;
            }
            if (ft3308.Ts_move.touch_key_one_num > 20)
            {
                ft3308.Ts_move.touch_key_one_num = 0;
                flit_min_interval_flag = false;
            }
            if (ft3308.ts_now_data.two_keys == 0x02)
            {
                ft3308.Ts_move.touch_key_one_num = 0;
            }

            if (Mouse_move_rf_send_flag == true && ft3308.Ts_move.touch_key_one_num == 10 && mouse_state != MOUSE_NULL)
            {
                position_data_clear();
                flit_min_interval_flag = false;
                Mouse_move_rf_send_flag = false;
                mouse_state = MOUSE_NULL;
            #if (FT3308_OPEN_TC_REL_ENABLE)
                if (report_gesture_flag == 0x01)
                {
                    report_gesture_flag = 0x00;
                    report_release_flag = 0x01;
                    get_data_report_release();
                }
            #endif
            }

            if (ret_shape.type != WRONG_DATA)
            {
                Mouse_move_rf_send_flag = true;

                // printf("--->>> ret_shape.fast_mode: %d\r\n", ret_shape.fast_mode);
                Mouse_move_rf_send_data(ret_shape.type,ret_shape.fast_mode);
                
                position_data_clear();
            }
            else if (ft3308.Ts_move.data_filt >= 1)
            {
                if ((ft3308.ts_now_data.two_keys == 0x01) && mouse_state != MOUSE_ROLL && mouse_state != MOUSE_ZOOM)
                {
                    mouse_state = MOUSE_MOVE;
                    if (((ft3308.ts_now_data.two_keys == 0x01) && (flit_min_interval_flag == false)) &&
                        ((abs(ft3308.Ts_move.move_x) <= 127) && (abs(ft3308.Ts_move.move_y) <= 127)))
                    {
                        get_mouse_report_move_press(0x00, ft3308.Ts_move.move_x, ft3308.Ts_move.move_y);
                        // printf("--->>> x,y : %d, %d\r\n", ft3308.Ts_move.move_x, ft3308.Ts_move.move_y);
                    }
                }
            }
        }

        ft3308.ts_store.x = ft3308.ts_now_data.x;
        ft3308.ts_store.y = ft3308.ts_now_data.y;
        ft3308.ts_store.x1 = ft3308.ts_now_data.x1;
        ft3308.ts_store.y1 = ft3308.ts_now_data.y1;
        ft3308.touch_press.touch_press = true;
    }
    else if ((ft3308.touch_press.touch_time && !clock_time_exceed(ft3308.touch_press.touch_time, 200 * 1000)) &&
             (check_touch_time && clock_time_exceed(check_touch_time, 24 * 1000)))
    {
        // clean struct data
        memset(&ft3308, FT3308_DATA_INIT, sizeof(ft3308));

        ft3308.Ts_move.touch_press = MOUSE_DOWN;

        if (mouse_state == MOUSE_NULL)
        {
            if (two_keybroad_flag == false)
            {
                get_mouse_report_move_press(0x01, ft3308.Ts_move.move_x, ft3308.Ts_move.move_y);
                printf("--->>> mouse_down \r\n");
            }
            else
            {
                two_keybroad_flag = false;
            }
        }
    }

    if (check_touch_time && clock_time_exceed(check_touch_time, 100 * 1000))
    {
        // clean struct data
        memset(&ft3308, FT3308_DATA_INIT, sizeof(ft3308));
        position_data_clear();
        check_touch_time = 0;
        flit_min_interval_flag = false;
        two_keybroad_flag = false;
        /****************************************************************/
        // The 2.4G send interface was added
        //  * @param ft3308.Ts_move.touch_press
        //  * @param ft3308.Ts_move.move_x
        //  * @param ft3308.Ts_move.move_y
        /****************************************************************/
        if (mouse_state != MOUSE_ROLL && mouse_state != MOUSE_ZOOM)
        {
            get_mouse_report_move_press(0x00, ft3308.Ts_move.move_x, ft3308.Ts_move.move_y);
            printf("--->>> mouse_realse \r\n");
        }
        mouse_state = MOUSE_NULL;
    }

    #if(FT3308_OPEN_TC_REL_ENABLE)
    if (check_touch_time && clock_time_exceed(check_touch_time, 48 * 1000))
    {
        if (report_gesture_flag == 0x01)
        {
            report_gesture_flag = 0x00;
            report_release_flag = 0x01;
            get_data_report_release();
            printf("--->>> zoom release\r\n");
        }
    }
    #endif

    // clone touch store data
    if (check_touch_time && clock_time_exceed(check_touch_time, 12 * 1000))
    {
        ft3308.ts_store.x = 0;
        ft3308.ts_store.y = 0;
        ft3308.ts_store.x1 = 0;
        ft3308.ts_store.y1 = 0;
    }

    if (ft_touch_refresh_time && clock_time_exceed(ft_touch_refresh_time, 3 * 1000 * 1000))
    {
        ft_touch_refresh_time = 0;
        ft_touch_sleep_flag = false;
        // keyScanLongPressTick = 0;
        printf("--->>> touch into sleep state \r\n");
    }
#endif
}

/**
 * @brief    Main loop for FT3308 touch chip operation.
 * @details  This function continuously scans for touch events if interrupt triggering is enabled.
 */
void FT3308_loop(void)
{
#if FT3308_INT_TIRGGIR_ENABLE
    FT3308_scan_trigger();
#endif
}
#endif
