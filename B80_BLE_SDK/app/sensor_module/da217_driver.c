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

#include "da217_driver.h"
#if (GSENSOR_ENABLE)
// Structure declaration
Gsensor_AccData Acc_data;

bool Face_up_flag = true;

// Pre-calculated table of arctan values, represented in units of 10000 radians
const s32 atan_table[16] = {17453, 10294, 5461, 2909, 1470, 737, 369, 184, 92, 46, 23, 12, 6, 3, 2, 1};

/**
 * @brief      Calculate the integer approximation of arctan(y/x) using the CORDIC algorithm.
 * @param[in]  y      - The y-coordinate.
 * @param[in]  x      - The x-coordinate.
 * @return     The angle in radians, represented as a fixed-point integer.
 */
s32 cordic_arctan(s32 y, s32 x)
{
    s32 angle = 0;
    s32 x_temp;
    for (int i = 0; i < 16; i++)
    {
        if (y > 0)
        {
            x_temp = x + (y >> i);
            y = y - (x >> i);
            x = x_temp;
            angle += atan_table[i];
        }
        else
        {
            x_temp = x - (y >> i);
            y = y + (x >> i);
            x = x_temp;
            angle -= atan_table[i];
        }
    }
    return angle;
}

/**
 * @brief      Calculate the integer square root of a number.
 * @param[in]  x   - The number for which to calculate the square root.
 * @return     The integer square root of the input number.
 */
s32 integer_sqrt(s32 x)
{
    s32 res = 0;
    s32 bit = 1UL << 30;

    while (bit > x)
        bit >>= 2;

    while (bit != 0)
    {
        if (x >= res + bit)
        {
            x -= res + bit;
            res = (res >> 1) + bit;
        }
        else
        {
            res >>= 1;
        }
        bit >>= 2;
    }
    return res;
}

/**
 * @brief      Convert radians to degrees.
 * @param[in]  rad  - The angle in radians.
 * @return     The angle converted to degrees.
 */
s32 rad_to_deg(s32 rad)
{
    return rad * 180 / 17453; // 17453 是 10000 * pi/180 的近似值
}

/**
 * @brief      Ensure that an angle is within the range of 0 to 359 degrees.
 * @param[in]  angle  - The angle to constrain.
 * @return     The constrained angle within the range of 0 to 359 degrees.
 */
s32 constrain_angle(s32 angle)
{
    while (angle < 0)
        angle += 360;
    while (angle >= 360)
        angle -= 360;
    return angle;
}

/**
 * @brief      Reads data from a register in the G-sensor.
 * @param[in]  addr     - The address of the register to read from.
 * @param[out] data_m   - Pointer to the buffer to store the read data.
 * @param[in]  len      - The number of bytes to read.
 * @return     0 if successful, non-zero if error.
 */
s8 Gsensor_register_read(u8 addr, u8 *data_m, u8 len)
{
    i2c_master_init(GSENSOR_SLAVE_DMA_ADDR, GSENSOR_SLAVE_DIVCLOCK);

    i2c_read_series(addr, GSENSOR_SLAVE_ADD_LED, (unsigned char *)data_m, len);

    return *data_m;
}

/**
 * @brief      Writes a byte of data to a register in the G-sensor.
 * @param[in]  addr    - The address of the register to write to.
 * @param[in]  data_m  - The data byte to write.
 * @return     0 if successful, non-zero if error.
 */
s8 Gsensor_register_write_byte(u8 addr, u8 data_m)
{
    static s8 res;
    i2c_master_init(GSENSOR_SLAVE_DMA_ADDR, GSENSOR_SLAVE_DIVCLOCK);

    i2c_write_byte(addr, GSENSOR_SLAVE_ADD_LED, data_m);

    return res;
}

/**
 * @brief      Initializes the G-sensor.
 * @return     0 if successful, non-zero if error.
 */
s8 Gsensor_init(void)
{
    s8 res = 0;
    u8 data_m = 0;

    // Retry 3 times to read chip ID
    res = Gsensor_register_read(NSA_REG_WHO_AM_I, &data_m, 1);
    if (data_m != 0x13)
    {
        res = Gsensor_register_read(NSA_REG_WHO_AM_I, &data_m, 1);
        if (data_m != 0x13)
        {
            res = Gsensor_register_read(NSA_REG_WHO_AM_I, &data_m, 1);
            if (data_m != 0x13)
            {
                printf("Gsensor read chip id  error= %01x-----\r\n", data_m);
                return -1;
            }
        }
    }

    // Write configuration settings to registers
    res |= Gsensor_register_write_byte(NSA_REG_SPI_I2C, 0x24);
    sleep_ms(20);

    res |= Gsensor_register_write_byte(NSA_REG_G_RANGE, 0x00);          // +/-2G, 14-bit resolution
    res |= Gsensor_register_write_byte(NSA_REG_POWERMODE_BW, 0x34);     // Normal mode
    res |= Gsensor_register_write_byte(NSA_REG_ODR_AXIS_DISABLE, 0x06); // ODR = 62.5Hz

    // Enter engineering mode for further configurations
    res |= Gsensor_register_write_byte(NSA_REG_ENGINEERING_MODE, 0x83);
    res |= Gsensor_register_write_byte(NSA_REG_ENGINEERING_MODE, 0x69);
    res |= Gsensor_register_write_byte(NSA_REG_ENGINEERING_MODE, 0xBD);

    sleep_ms(50);

    // Reduce power consumption
    // Gsensor_register_write_byte(NSA_REG_SENS_COMP, 0x00);

    return res;
}

/**
 * @brief      Enables or disables the G-sensor chip.
 * @param[in]  enable - Flag indicating whether to enable (1) or disable (0) the chip.
 * @return     0 if successful, non-zero if error.
 */
s8 Gsensor_set_enable(u8 enable)
{
    s8 res = 0;

    if (enable)
        res = Gsensor_register_write_byte(NSA_REG_POWERMODE_BW, 0x30); // Enable chip
    else
        res = Gsensor_register_write_byte(NSA_REG_POWERMODE_BW, 0x80); // Disable chip

    return res;
}

/**
 * @brief      Reads the three-axis accelerometer data from the G-sensor.
 * @param[out] x - Pointer to store the X-axis acceleration.
 * @param[out] y - Pointer to store the Y-axis acceleration.
 * @param[out] z - Pointer to store the Z-axis acceleration.
 * @return     0 if successful, non-zero if error.
 */
s8 Gsensor_read_data(s16 *x, s16 *y, s16 *z)
{
    u8 tmp_data[6] = {0};

    Gsensor_register_read(NSA_REG_ACC_X_LSB, tmp_data, 6);

    *x = ((s16)(tmp_data[1] << 8 | tmp_data[0])) >> 4;

    *y = ((s16)(tmp_data[3] << 8 | tmp_data[2])) >> 4;

    *z = ((s16)(tmp_data[5] << 8 | tmp_data[4])) >> 4;

#if ANGLE_CONVERSION_ENABLED

    // Calculate total acceleration
    s32 sqrt_ax_az = integer_sqrt((s32)(*x * *x) + (s32)(*z * *z));
    s32 sqrt_ay_az = integer_sqrt((s32)(*y * *y) + (s32)(*z * *z));

    // Calculate pitch angle (in degrees)
    s32 pitch = cordic_arctan(*y, sqrt_ax_az);

    // Calculate roll angle (in degrees)
    s32 roll = cordic_arctan(*x, sqrt_ay_az);

    // Convert angles from radians to degrees
    pitch = rad_to_deg(pitch);
    roll = rad_to_deg(roll);

    // Print pitch and roll angles

    printf("Pitch (degrees): %d\n", pitch);
    printf("Roll (degrees): %d\n", roll);

#endif

    // Device orientation variable
    Face_up_flag = (*z >= 0);

    // printf("--->>> Face_up_flag = %d\n", Face_up_flag);
    // printf("--->>> Accelerometer data: acc_x = %d, acc_y = %d, acc_z = %d\n", *x, *y, *z);

    // FOVA debugging
    // printf("%d,%d,%d\r\n", *x, *y, *z);
    
    return 0;
}

/**
 * @brief      Opens the freefall interrupt of the G-sensor.
 * @param[in]  th - Threshold value for the freefall interrupt.
 * @return     0 if successful, non-zero if error.
 */
s8 Gsensor_open_freefall_interrupt(u8 th)
{
    s8 res = 0;

    // Set freefall duration (default: 20ms)
    res = Gsensor_register_write_byte(NSA_REG_FREEFALL_DURATION, 0x09);

    // Set freefall threshold (default: 375mg)
    res = Gsensor_register_write_byte(NSA_REG_FREEFALL_THRESHOLD, th);

    // Set freefall hysteresis (default: 125mg)
    res = Gsensor_register_write_byte(NSA_REG_FREEFALL_HYST, 0x02);

    // Map freefall interrupt to INT2
    res |= Gsensor_register_write_byte(NSA_REG_INTERRUPT_MAPPING3, 0x01);

    // Enable freefall interrupt
    res = Gsensor_register_write_byte(NSA_REG_INTERRUPT_SETTINGS2, 0x08);

    return res;
}

/**
 * @brief      Closes the freefall interrupt of the G-sensor.
 * @return     0 if successful, non-zero if error.
 */
s8 Gsensor_close_freefall_interrupt(void)
{
    s8 res = 0;

    res = Gsensor_register_write_byte(NSA_REG_INTERRUPT_SETTINGS2, 0x00);

    res = Gsensor_register_write_byte(NSA_REG_INTERRUPT_MAPPING3, 0x00);

    return res;
}

/**
 * @brief      Opens the active interrupt of the G-sensor.
 * @param[in]  th - Threshold value for the active interrupt.
 * @return     0 if successful, non-zero if error.
 */
s8 Gsensor_open_active_interrupt(u8 th)
{
    s8 res = 0;
    u8 date = 0;

    res = Gsensor_register_write_byte(NSA_REG_INTERRUPT_SETTINGS1, 0x87);

    res = Gsensor_register_write_byte(NSA_REG_ACTIVE_DURATION, 0x00);

    res = Gsensor_register_write_byte(NSA_REG_ACTIVE_THRESHOLD, th);

    res = Gsensor_register_write_byte(NSA_REG_INTERRUPT_MAPPING1, 0x04);

    Gsensor_register_read(NSA_REG_ACTIVE_THRESHOLD, &date, 1);
    //printf("Active threshold is %01x\n", date);

    return res;
}

/**
 * @brief      This function disables the active interrupt of the G-sensor
 * @return     0 if successful, other values if error
 */
s8 Gsensor_close_active_interrupt(void)
{
    s8 res = 0;

    res = Gsensor_register_write_byte(NSA_REG_INTERRUPT_SETTINGS1, 0x00);

    res = Gsensor_register_write_byte(NSA_REG_INTERRUPT_MAPPING1, 0x00);

    return res;
}

/*
 * Note:
 * slave device id 0x5C(write) 0x5D(read), contorl transmission direction by LSB
 * i2c clock 200K, only master need set i2c clock
 */
void Gsensor_iic_init(void)
{

    i2c_gpio_set(I2C_SDA_PIN, I2C_SCL_PIN);

    i2c_master_init(GSENSOR_SLAVE_DMA_ADDR, GSENSOR_SLAVE_DIVCLOCK);

    Gsensor_init();

    Gsensor_open_active_interrupt(0x30);
}

/**
 * @brief      Periodically reads data from the G-sensor.
 * @details    Reads accelerometer data every 500 milliseconds (5 * 100 * 1000 microseconds).
 */
void Gsensor_loop(void)
{
    static u32 Gsensor_loop_time;

    if (clock_time_exceed(Gsensor_loop_time, 5 * 100 * 1000))
    {
        Gsensor_loop_time = clock_time();
        Gsensor_read_data(&Acc_data.acc_x, &Acc_data.acc_y, &Acc_data.acc_z);
    }
}
#else
bool Face_up_flag = true;
#endif
