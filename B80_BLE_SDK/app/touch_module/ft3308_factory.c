/*********************************************************************************************************
 * @file      ft3308_factory.c
 *
 * @details   This file contains the implementation of various tests for the FT3308 touch screen controller.
 *            It includes functions to read raw data, control block data, short circuit data, noise data,
 *            and touch difference data. The tests can be performed in either full or half mode.
 *
 * @note      These functions are designed to be used in factory mode for testing purposes.
 *            The functions assume that the FT3308 device is properly connected and configured for I2C communication.
 *            The test results are stored in the provided buffers and can be used for further analysis or debugging.
 *
 * @author    huzhuohuan
 * @date      2024-10-22
 * @version   1.0
 ********************************************************************************************************/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "ft3308_factory.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
static u16 raw_wo_pressure[MAX_CHANNEL_NUM] = {0};
static u16 touch_diff[MAX_CHANNEL_NUM] = {0};
static bool bTestResult = 1;
static bool half_flag = false;

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/

/**
 * @brief      Provides a delay for a specified number of milliseconds.
 *
 * This function implements a blocking delay by calling the WaitMs function,
 * which is typically provided by the hardware abstraction layer or the operating system.
 * The function pauses the execution of the current thread for the given number of milliseconds.
 *
 * @param[in]  msec   The number of milliseconds to delay.
 *
 */
void sys_delay_ms(u32 msec)
{
    WaitMs(msec);
}

/**
 * @brief      Writes data to an I2C slave device.
 *
 * @param[in]  i2c_addr    The I2C address of the slave device.
 * @param[in]  reg_addr    The address of the register to write to.
 * @param[in]  reg_data    A pointer to the data to be written.
 * @param[in]  length      The number of bytes to write.
 *
 * @return     0 if successful, non-zero if an error occurred.
 */
s8 I2cPlatformWrite(u8 i2c_addr, u8 reg_addr, const u8 *reg_data, u16 length)
{
    i2c_master_init(FT3308_SLAVE_DMA_ADDR, FT3308_SLAVE_DIVCLOCK);

    i2c_write_byte(reg_addr, FT3308_SLAVE_ADD_LED, reg_data[0]);

    return 0;
}

/**
 * @brief      Reads data from an I2C slave device.
 *
 * @param[in]  i2c_addr    The I2C address of the slave device.
 * @param[in]  reg_addr    The address of the register to read from.
 * @param[out] reg_data    A pointer to a buffer where the read data will be stored.
 * @param[in]  length      The number of bytes to read.
 *
 * @return     0 if successful, non-zero if an error occurred.
 */
s8 I2cPlatformRead(u8 i2c_addr, u8 reg_addr, u8 *reg_data, u16 length)
{
    i2c_master_init(i2c_addr, FT3308_SLAVE_DIVCLOCK);

    i2c_read_series(reg_addr, FT3308_SLAVE_ADD_LED, (unsigned char *)reg_data, length);

    return 0;
}

/**
 * @brief      Writes data to the touch panel device via I2C.
 *
 * @param[in]  wbuf    A pointer to the buffer containing the data to be written.
 * @param[in]  wlen    The length of the data in the buffer.
 *
 * @return     0 if the write operation is successful, non-zero if all attempts fail.
 */
int touch_write(u8 *wbuf, u16 wlen)
{
    int ret = 0;
    int i = 3;
    if (wlen == 0 || wbuf == NULL)
    {
        printf("touch_write invalid\r\n");
        return ret;
    }
    for (i = 0; i < 3; i++)
    {
        ret = I2cPlatformWrite(FT3308_I2C_DEV_ADDR, wbuf[0], &wbuf[1], wlen - 1);
        if (ret != 0)
        {
            printf("touch_write twi write fail,ret:%d\r\n", ret);
        }
        else
            break;
    }

    return ret;
}

/*
 * Name: touch_write
 * Brief: The function is used to read values from touch chip with I2C BUS
 *  interface.
 *
 *  The standard procedure of reading values from touch chip consists of the
 *  two steps:
 *    1. Send I2C writ packet to transfer register addresses or commands
 *       following a stop condition.
 *    2. Send I2C read packet to read values from touch chip.
 *  Sometimes, only read packet is sent, now step 1(write packet) is ignored
 *  or skipped with invalid parameters of wbuf and wlen.
 *
 * Input: @wbuf: the write buffer.
 *        @wlen: the size in byte of write buffer.
 *
 * Output: @rbuf: the buffer to save reading values
 *         @rlen: the size in byte that you want to read
 *
 * Return: return 0 if success, otherwise return error code.
 */
int touch_read(u8 *wbuf, u16 wlen, u8 *rbuf, u16 rlen)
{
    int ret = 0;
    int i = 3;

    if (wbuf == NULL || rbuf == NULL || wlen == 0 || rlen == 0)
    {
        printf("touch_read  INVALID");
        return ret;
    }

    for (i = 0; i < 3; i++)
    {
        ret = I2cPlatformRead(FT3308_I2C_DEV_ADDR, wbuf[0], rbuf, rlen);
        if (ret)
        {
            printf("touch_read twi read fail,ret:%d\r\n", ret);
        }
        else
            break;
    }

    return ret;
}

/*
 * Name: touch_write_reg
 * Brief: Variety of touch_write() function. The function is used to write
 *  a byte value to corresponding register address.
 *
 *
 * Input: @addr: the register address of touch chip.
 *        @value: the value that you want to write to the register address.
 * Output:
 * Return: return 0 if success, otherwise return error code.
 */
int touch_write_reg(u8 addr, u8 value)
{
    u8 wbuf[2] = {0};

    wbuf[0] = addr;
    wbuf[1] = value;
    return touch_write(&wbuf[0], 2);
}

/*
 * Name: touch_read_reg
 * Brief: Variety of touch_read() function. The function is used to read
 *  a byte value from corresponding register address.
 *
 *
 * Input: @addr: the register address of touch chip.
 *
 * Output: @value: the value read from the register address.
 *
 * Return: return 0 if success, otherwise return error code.
 */
int touch_read_reg(u8 addr, u8 *value)
{
    return touch_read(&addr, 1, value, 1);
}

/**
 * @brief      Initiates a scanning process by writing to and reading from a touch device register.
 *
 * This function attempts to start a scan by writing a specific value to a register
 * of the touch device and then repeatedly reads the register until a certain
 * condition is met or a maximum number of retries is exceeded.
 *
 * @return     0 if the scanning process is successfully initiated, -1 if it fails.
 */
static int StartScan(void)
{
    int ret = 0;
    int i = 0;
    u8 val = 0;

    ret = touch_write_reg(0x08, 0x01);
    if (ret < 0)
    {
        printf("write reg08 fails\n");
        return ret;
    }

    for (i = 0; i < 400; i++)
    {
        sys_delay_ms(10);
        val = 0xFF;
        ret = touch_read_reg(0x08, &val);
        if ((val == 0) && (ret == 0))
            return 0;
        // else
        //     printf("read reg%x,val:%x,ret:%d,retry:%d\n", 0x08, val, ret, i);
    }

    return -1;
}

/**
 * @brief      Attempts to enter the factory mode of a touch screen device.
 *
 * This function tries to enter the factory mode by setting a specific value in the
 * device's register 0x00. It first checks if the device is already in factory mode.
 * If not, it repeatedly writes the value 0x40 to the register and checks if the
 * device has entered factory mode by reading back the register's value.
 *
 * @return     Returns 0 on success, or -1 if the operation fails after the maximum number of attempts.
 */
static int fts_enter_factory_mode(void)
{
    int ret = 0;
    int i = 0;
    u8 val = 0xFF;

    ret = touch_read_reg(0x00, &val);
    if (val == 0x40)
    {
        printf("already in factory mode\n");
        return 0;
    }

    for (i = 0; i < 20; i++)
    {
        val = 0xFF;
        ret = touch_write_reg(0x00, 0x40);
        if (ret < 0)
        {
            printf("write reg0x00 fails\n");
            return ret;
        }

        ret = touch_read_reg(0x00, &val);
        if (val == 0x40)
        {
            printf("enter into factory mode\n");
            sys_delay_ms(200);
            return 0;
        }
        sys_delay_ms(20);
    }

    printf("reg00:%x\n", val);
    return -1;
}

/**
 * @brief      Tries to enter the work mode of a touch screen device.
 *
 * This function attempts to enter the work mode by writing a specific value to
 * the touch screen device's register 0x00. It first checks if the device is already
 * in work mode. If not, it writes the value 0x00 to the register and then repeatedly
 * checks if the device has entered work mode by reading back the register's value.
 *
 * @return     0 if the device successfully enters work mode, -1 if it fails.
 */
int fts_enter_work_mode(void)
{
    int ret = 0;
    int i = 0;
    u8 val = 0xFF;

    ret = touch_read_reg(0x00, &val);
    if (val == 0x00)
    {
        printf("already in factory mode\n");
        return 0;
    }

    for (i = 0; i < 20; i++)
    {
        ret = touch_write_reg(0x00, 0x00);
        if (ret < 0)
        {
            printf("write reg0x00 fails\n");
            return ret;
        }
        val = 0xFF;
        ret = touch_read_reg(0x00, &val);
        if (val == 0x00)
        {
            printf("enter into work mode\n");
            sys_delay_ms(20);
            return 0;
        }
        sys_delay_ms(20);
    }

    return -1;
}

/**
 * @brief      Retrieves the raw data from the touch screen.
 *
 * This function initiates a scan, writes to a specific register to prepare for
 * reading raw data, and then reads the raw data from the touch screen.
 *
 * @param[out] raw     A pointer to store the raw data.
 *
 * @return     0 on success, negative error code on failure.
 */
static int fts_get_rawdata(u16 *raw)
{
    int ret = 0;
    int i = 0;
    u8 raw_addr = 0x35;
    u8 tmp_data[MAX_CHANNEL_NUM * 2] = {0};

    ret = StartScan();
    if (ret < 0)
    {
        printf("star scan fails\n");
        return ret;
    }

    ret = touch_write_reg(0x34, 00);
    if (ret < 0)
    {
        printf("write reg34 fails\n");
        return ret;
    }

    ret = touch_read(&raw_addr, 1, tmp_data, MAX_CHANNEL_NUM * 2);
    if (ret < 0)
    {
        printf("read raw fails\n");
        return ret;
    }

    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        raw[i] = (tmp_data[i * 2] << 8) + tmp_data[i * 2 + 1];
    }

    return 0;
}

/**
 * @brief      Retrieves the control block (CB) data from the touch screen.
 *
 * This function initiates a scan, writes to a specific register to prepare for
 * reading CB data, and then reads the CB data from the touch screen.
 *
 * @param[out] cb      A pointer to store the CB data.
 * @param[in]  mode    The mode in which to operate (not used in this function).
 *
 * @return     0 on success, negative error code on failure.
 */
static int fts_get_cb(u16 *cb, u8 mode)
{
    int ret = 0;
    int i = 0;
    u8 cb_addr = 0x39;
    u8 tmp_data[MAX_CHANNEL_NUM * 2] = {0};

    ret = StartScan();
    if (ret < 0)
    {
        printf("star scan fails\n");
        return ret;
    }

    ret = touch_write_reg(0x33, 00);
    if (ret < 0)
    {
        printf("write reg33 fails\n");
        return ret;
    }

    ret = touch_read(&cb_addr, 1, tmp_data, MAX_CHANNEL_NUM * 2);
    if (ret < 0)
    {
        printf("read cb fails\n");
        return ret;
    }

    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        cb[i] = (tmp_data[i * 2] << 8) + tmp_data[i * 2 + 1];
    }

    return 0;
}

/**
 * @brief      Retrieves the raw base data from the touch screen.
 *
 * This function performs multiple scans, retrieves the raw data, and calculates
 * the average raw data to be used as the base.
 *
 * @return     0 on success, negative error code on failure.
 */
static int fts_get_raw_base(void)
{
    int ret = 0;
    int i = 0;
    int j = 0;
    u16 raw[MAX_CHANNEL_NUM] = {0};

    memset(raw_wo_pressure, 0, MAX_CHANNEL_NUM * sizeof(u16));

    for (i = 0; i < 3; i++)
        StartScan();

    for (i = 0; i < 3; i++)
    {
        ret = fts_get_rawdata(raw);
        if (ret < 0)
        {
            printf("get raw(base) fails\n");
            return ret;
        }

        for (j = 0; j < MAX_CHANNEL_NUM; j++)
        {
            raw[j] = raw[j] / 3;
            raw_wo_pressure[j] += raw[j];
        }
    }

    return 0;
}

/**
 * @brief      Performs a raw data test on the provided buffer.
 *
 * This function tests the raw data in the buffer to verify its integrity or
 * to perform some quality checks. The test is optionally executed in half
 * mode, which might involve processing only a portion of the data or using
 * specific test criteria for half data.
 *
 * @param[in]  buf        Pointer to the buffer containing the raw data to be tested.
 * @param[in]  is_half    is_half 0 indicates a finished test and 1 indicates a semi-finished test
 * @param[out] bTestResult Pointer to a boolean variable where the test result will be stored.
 *                        True indicates the test passed, while false indicates failure.
 *
 * @return     The number of bytes processed during the test. In case of an error or
 *             special condition, a negative value may be returned.
 */
int TestItem_RawDataTest(u8 *buf, bool is_half, bool *bTestResult)
{
    int ret = 0;
    int i = 0;
    u16 raw[MAX_CHANNEL_NUM] = {0};

    /*rawdata type*/
    buf[0] = 0x01;

    for (i = 0; i < 3; i++)
        StartScan();

    ret = fts_get_rawdata(raw);
    if (ret < 0)
    {
        printf("get raw fails\n");
        return ret;
    }

#if (COMFIRM_FACTORY_CHECK)
    u16 RawMin[MAX_CHANNEL_NUM] = RAW_MIN;
    u16 RawMax[MAX_CHANNEL_NUM] = RAW_MAX;
    u16 HalfRawMin[MAX_CHANNEL_NUM] = HALF_RAW_MIN;
    u16 HalfRawMax[MAX_CHANNEL_NUM] = HALF_RAW_MAX;
    u16 *min;
    u16 *max;
    *bTestResult = true;
    /*compare*/
    if (is_half)
    {
        min = HalfRawMin;
        max = HalfRawMax;
    }
    else
    {
        min = RawMin;
        max = RawMax;
    }
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        if ((raw[i] < min[i]) || (raw[i] > max[i]))
        {
            printf("ch:%d, raw:%d,min:%d,max:%d\n", i, raw[i], min[i], max[i]);
            *bTestResult = false;
        }
    }
    buf[1] = (*bTestResult) ? 0x01 : 0x02;
#endif

    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        buf[2 + i * 2 + 1] = raw[i];
        buf[2 + i * 2] = (raw[i] >> 8);
    }

    printf("raw:");
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
        printf("%d ", raw[i]);
    printf("\n\n");

    return 0;
}

/**
 * @brief      Performs a control block (CB) test on the provided buffer.
 *
 * This function executes a test on the raw data buffer to verify the integrity
 * of the control block information. The test can be performed in full or half
 * mode, depending on the value of the is_half parameter.
 *
 * @param[in]  buf        Pointer to the buffer containing the raw data to be tested.
 * @param[in]  is_half    bool value indicating whether to perform a half test.
 *                        0 indicates a full test, while 1 indicates a half test.
 * @param[out] bTestResult Pointer to a boolean variable where the test result will be stored.
 *                        True indicates the test passed, while false indicates failure.
 *
 * @return     The number of bytes processed during the test. In case of an error or
 *             special condition, a negative value may be returned.
 */
int TestItem_CbTest(u8 *buf, bool is_half, bool *bTestResult)
{
    int ret = 0;
    int i = 0;
    u16 cb[MAX_CHANNEL_NUM] = {0};

    buf[0] = 0x02;

    ret = fts_get_cb(cb, 0);
    if (ret < 0)
    {
        printf("get cb fails\n");
        return ret;
    }

#if (COMFIRM_FACTORY_CHECK)
    u16 CbMin[MAX_CHANNEL_NUM] = CB_MIN;
    u16 CbMax[MAX_CHANNEL_NUM] = CB_MAX;
    u16 HalfCbMin[MAX_CHANNEL_NUM] = HALF_CB_MIN;
    u16 HalfCbMax[MAX_CHANNEL_NUM] = HALF_CB_MAX;
    u16 *min;
    u16 *max;
    *bTestResult = true;
    /*compare*/
    if (is_half)
    {
        min = HalfCbMin;
        max = HalfCbMax;
    }
    else
    {
        min = CbMin;
        max = CbMax;
    }
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        if ((cb[i] < min[i]) || (cb[i] > max[i]))
        {
            printf("ch:%d, cb:%d,min:%d,max:%d\n", i, cb[i], min[i], max[i]);
            *bTestResult = false;
        }
    }
    buf[1] = (*bTestResult) ? 0x01 : 0x02;
#endif
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        buf[2 + i * 2 + 1] = cb[i];
        buf[2 + i * 2] = (cb[i] >> 8);
    }

    printf("cb:");
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
        printf("%d ", cb[i]);
    printf("\n\n");
    return 0;
}

/**
 * @brief      Performs a short test on the provided buffer.
 *
 * This function tests the provided buffer for short conditions, which may indicate
 * errors or anomalies in the data. The test can be executed in full or half mode,
 * depending on the value of the is_half parameter.
 *
 * @param[in]  buf        Pointer to the buffer containing the data to be tested.
 * @param[in]  is_half    Boolean flag indicating whether to perform a half test.
 *                        0 (false) indicates a full test, while 1 (true) indicates a half test.
 * @param[out] bTestResult Pointer to a boolean variable where the test result will be stored.
 *                        True indicates the test passed, while false indicates failure.
 *
 * @return     The number of bytes processed during the test. In case of an error or
 *             special condition, a negative value may be returned.
 */
int TestItem_ShortTest(u8 *buf, bool is_half, bool *bTestResult)
{
    int ret = 0;
    int i = 0;
    u8 reg0d_val = 0;
    u16 short_data[MAX_CHANNEL_NUM] = {0};

    buf[0] = 0x03;

    ret = touch_read_reg(0x0D, &reg0d_val);
    if (ret < 0)
    {
        printf("read reg0D fails");
        return ret;
    }

    ret = touch_write_reg(0x0D, 0x0F);
    if (ret < 0)
    {
        printf("write reg0D fails");
        return ret;
    }

    ret = touch_write_reg(0xAE, 2);
    if (ret < 0)
    {
        printf("write regAE fails");
        return ret;
    }
    sys_delay_ms(200);

    ret = fts_get_cb(short_data, 2);
    if (ret < 0)
    {
        printf("get cb fails");
        goto err_short;
    }

#if (COMFIRM_FACTORY_CHECK)
    u16 ShortMin[MAX_CHANNEL_NUM] = SHORT_MIN;
    u16 ShortMax[MAX_CHANNEL_NUM] = SHORT_MAX;
    u16 HalfShortMin[MAX_CHANNEL_NUM] = HALF_SHORT_MIN;
    u16 HalfShortMax[MAX_CHANNEL_NUM] = HALF_SHORT_MAX;
    u16 *min;
    u16 *max;
    *bTestResult = true;
    /*compare*/
    if (is_half)
    {
        min = HalfShortMin;
        max = HalfShortMax;
    }
    else
    {
        min = ShortMin;
        max = ShortMax;
    }
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        if ((short_data[i] < min[i]) || (short_data[i] > max[i]))
        {
            printf("ch:%d, short:%d,min:%d,max:%d", i, short_data[i], min[i], max[i]);
            *bTestResult = false;
        }
    }
    buf[1] = (*bTestResult) ? 0x01 : 0x02;
#endif

    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        buf[2 + i * 2 + 1] = short_data[i];
        buf[2 + i * 2] = (short_data[i] >> 8);
    }

    printf("short_data:");
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
        printf("%d ", short_data[i]);
    printf("\n\n");

    ret = 0;

err_short:
    ret = touch_write_reg(0x0D, reg0d_val);
    if (ret < 0)
    {
        printf("restore regAE fails\n");
    }

    ret = touch_write_reg(0xAE, 0x00);
    if (ret < 0)
    {
        printf("write 00 to regAE fails\n");
    }
    sys_delay_ms(200);

    fts_get_raw_base();

    return ret;
}

/**
 * @brief      Performs a noise test on the provided buffer.
 *
 * This function tests the buffer for noise or interference. It analyzes the data
 * for any unwanted signals or disturbances that could affect the quality or
 * integrity of the data. The test can be performed in full or half mode,
 * depending on the value of the is_half parameter.
 *
 * @param[in]  buf        Pointer to the buffer containing the data to be tested.
 * @param[in]  is_half    bool value indicating whether to perform a half test.
 *                        0 indicates a full test, while 1 indicates a half test.
 * @param[out] bTestResult Pointer to a boolean variable where the test result will be stored.
 *                        True indicates the test passed, while false indicates failure.
 *
 * @return     The number of bytes processed during the test. In case of an error or
 *
 */
int TestItem_NoiseTest(u8 *buf, bool is_half, bool *bTestResult)
{
    int ret = 0;
    u8 regae_val = 0;
    int i = 0;
    int m = 0;
    int j = 0;
    u16 raw[MAX_CHANNEL_NUM] = {0};
    u16 ave_raw[MAX_CHANNEL_NUM] = {0};
    u16 max_raw[MAX_CHANNEL_NUM] = {0};
    u16 noise[MAX_CHANNEL_NUM] = {0};

    buf[0] = 0x04;

    ret = touch_read_reg(0xAE, &regae_val);
    if (ret < 0)
    {
        printf("read regAE fails");
        return ret;
    }

    ret = touch_write_reg(0xAE, 0x00);
    if (ret < 0)
    {
        printf("write 00 to regAE fails\n");
        goto err_noise;
    }
    sys_delay_ms(20);

    for (i = 0; i < READ_NOISE_DATA_RETRIES; i++)
    {
        for (m = 0; m < 3; m++)
            StartScan();

        sys_delay_ms(20);
        ret = fts_get_rawdata(raw);
        if (ret < 0)
        {
            printf("get raw fails\n");
            goto err_noise;
        }
        for (j = 0; j < MAX_CHANNEL_NUM; j++)
        {
            ave_raw[j] += raw[j] / READ_NOISE_DATA_RETRIES;
            if (max_raw[j] < raw[j])
                max_raw[j] = raw[j];
        }
    }

    for (i = 0; i < MAX_CHANNEL_NUM; i++)
        noise[i] = max_raw[i] - ave_raw[i];

#if (COMFIRM_FACTORY_CHECK)
    u16 NoiseMax[MAX_CHANNEL_NUM] = NOISE_MAX;
    u16 HalfNoiseMax[MAX_CHANNEL_NUM] = HALF_NOISE_MAX;
    u16 *max;
    *bTestResult = true;
    /*compare*/
    if (is_half)
    {
        max = HalfNoiseMax;
    }
    else
    {
        max = NoiseMax;
    }

    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        if (noise[i] > max[i])
        {
            printf("ch:%d, noise:%d,max:%d\n", i, noise[i], max[i]);
            *bTestResult = false;
        }
    }
    buf[1] = (*bTestResult) ? 0x01 : 0x02;
#endif

    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        buf[2 + i * 2 + 1] = noise[i];
        buf[2 + i * 2] = (noise[i] >> 8);
    }

    printf("noise:");
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
        printf("%d ", noise[i]);
    printf("\n\n");

err_noise:

    ret = touch_write_reg(0xAE, regae_val);
    if (ret < 0)
    {
        printf("write %d to regAE fails\n", regae_val);
    }

    return 0;
}

/**
 * @brief      Performs a touch difference test and stores the result.
 *
 * This function retrieves raw data from the touch screen, calculates the
 * difference between the maximum raw data and the original raw data without pressure,
 * and checks if the differences are within the predefined minimum and maximum values.
 *
 * @param[in]  buf     A buffer to store test data and results.
 * @param[out] bTestResult  A pointer to a boolean indicating the test result.
 *
 * @return     0 on success, negative error code on failure.
 */
int TestItem_TouchDiffTest(u8 *buf, bool *bTestResult)
{
    int ret = 0;
    int i = 0;
    int j = 0;
    u16 raw[MAX_CHANNEL_NUM] = {0};
    u16 *diff = touch_diff;
    u16 diff_min[MAX_CHANNEL_NUM] = TOUCH_DIFF_MIN;
    u16 diff_max[MAX_CHANNEL_NUM] = TOUCH_DIFF_MAX;

    /*rawdata type*/
    buf[0] = 0x05;

    for (i = 0; i < RAW_NUM_P; i++)
    {
        ret = fts_get_rawdata(raw);
        if (ret < 0)
        {
            printf("get raw fails\n");
            return ret;
        }

        for (j = 0; j < MAX_CHANNEL_NUM; j++)
        {
            if (diff[j] < raw[j])
                diff[j] = raw[j];
        }
    }

    printf("\n\nmax raw:");
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
        printf("%d ", diff[i]);
    printf("\n\n");
    printf("ori raw:");
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
        printf("%d ", raw_wo_pressure[i]);
    printf("\n\n");

    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        if (diff[i] >= raw_wo_pressure[i])
            diff[i] = diff[i] - raw_wo_pressure[i];
        else
            diff[i] = raw_wo_pressure[i] - diff[i];
    }

    *bTestResult = true;
    /*compare*/
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        if ((diff[i] < diff_min[i]) || (diff[i] > diff_max[i]))
        {
            printf("ch:%d, diff:%d,min:%d,max:%d\n", i, diff[i], diff_min[i], diff_max[i]);
            *bTestResult = false;
        }
    }

    buf[1] = (*bTestResult) ? 0x01 : 0x02;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        buf[2 + i * 2 + 1] = diff[i];
        buf[2 + i * 2] = (diff[i] >> 8);
    }

    printf("diff:");
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
        printf("%d ", diff[i]);
    printf("\n\n");

    return 0;
}

/**
 * @brief      Performs a series of tests in factory mode for half product testing.
 *
 * This function enters factory mode, performs raw data, CB, short, and noise tests,
 * and records the results. It then exits factory mode.
 *
 * @param[in]  buf     A buffer to store test data and results.
 */
void half_product_test(u8 *buf)
{
    int ret = 0;
    bool tmp_result = false;
    u32 result = 0;
    u8 *tmp_buf;

    ret = fts_enter_factory_mode();
    if (ret < 0)
    {
        printf("enter factory mode fails\n");
        return;
    }

    result |= (1 << 0);
    tmp_buf = buf;
    ret = TestItem_RawDataTest(tmp_buf, true, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("raw test fails\n");
        result &= (~(1 << 0));
    }

    result |= (1 << 1);
    tmp_buf = buf + 74;
    ret = TestItem_CbTest(tmp_buf, true, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("cb test fails\n");
        result &= (~(1 << 1));
    }

    result |= (1 << 2);
    tmp_buf = buf + 74 * 2;
    ret = TestItem_ShortTest(tmp_buf, true, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("short test fails\n");
        result &= (~(1 << 2));
    }

    result |= (1 << 3);
    tmp_buf = buf + 74 * 3;
    ret = TestItem_NoiseTest(tmp_buf, true, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("noise test fails\n");
        result &= (~(1 << 3));
    }

    ret = fts_enter_work_mode();
    if (ret < 0)
    {
        printf("enter work mode fails\n");
    }
}

/**
 * @brief      Performs a series of tests without pressure for product testing.
 *
 * This function enters factory mode, performs raw data, CB, short, and noise tests
 * without pressure, and records the results. It then exits factory mode.
 *
 * @param[in]  buf     A buffer to store test data and results.
 */
void product_wo_pressure_test(u8 *buf)
{
    int ret = 0;
    bool tmp_result = false;
    u32 result = 0;
    u8 *tmp_buf;

    ret = fts_enter_factory_mode();
    if (ret < 0)
    {
        printf("enter factory mode fails\n");
        return;
    }

    result |= (1 << 0);
    tmp_buf = buf;
    ret = TestItem_RawDataTest(tmp_buf, false, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("raw test fails\n");
        result &= (~(1 << 0));
    }

    result |= (1 << 1);
    tmp_buf = buf + 74;
    ret = TestItem_CbTest(tmp_buf, false, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("cb test fails\n");
        result &= (~(1 << 1));
    }

    result |= (1 << 2);
    tmp_buf = buf + 74 * 2;
    ret = TestItem_ShortTest(tmp_buf, false, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("short test fails\n");
        result &= (~(1 << 2));
    }

    result |= (1 << 3);
    tmp_buf = buf + 74 * 3;
    ret = TestItem_NoiseTest(tmp_buf, false, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("noise test fails\n");
        result &= (~(1 << 2));
    }
}

/**
 * @brief      Performs a touch difference test with pressure for product testing.
 *
 * This function performs a touch difference test with pressure and records the result.
 *
 * @param[in]  buf     A buffer to store test data and results.
 */
void product_pressure_test(u8 *buf)
{
    int ret = 0;
    bool tmp_result = false;
    u32 result = 0;
    u8 *tmp_buf;

    result |= (1 << 0);
    tmp_buf = buf;
    ret = TestItem_TouchDiffTest(tmp_buf, &tmp_result);
    if ((ret != 0) || (tmp_result == false))
    {
        printf("touch diff test fails\n");
        result &= (~(1 << 0));
    }
}

/**
 * @brief      Reads the raw data from the touch screen.
 *
 * This function enters factory mode and performs a raw data test.
 *
 * @param[out] buf     A buffer to store the raw data test result.
 */
void touch_read_raw_data(u8 *buf)
{
    fts_enter_factory_mode();

    TestItem_RawDataTest(buf, half_flag, &bTestResult);
}

/**
 * @brief      Reads the control block (CB) data from the touch screen.
 *
 * This function enters factory mode and performs a CB data test.
 *
 * @param[out] buf     A buffer to store the CB data test result.
 */
void touch_read_cb_data(u8 *buf)
{
    fts_enter_factory_mode();

    TestItem_CbTest(buf, half_flag, &bTestResult);
}

/**
 * @brief      Reads the short circuit data from the touch screen.
 *
 * This function enters factory mode and performs a short circuit test.
 *
 * @param[out] buf     A buffer to store the short circuit test result.
 */
void touch_read_short_data(u8 *buf)
{
    fts_enter_factory_mode();

    TestItem_ShortTest(buf, half_flag, &bTestResult);
}

/**
 * @brief      Reads the noise data from the touch screen.
 *
 * This function enters factory mode and performs a noise test.
 *
 * @param[out] buf     A buffer to store the noise test result.
 */
void touch_read_noize_data(u8 *buf)
{
    fts_enter_factory_mode();

    TestItem_NoiseTest(buf, half_flag, &bTestResult);
}

/**
 * @brief      Reads the touch difference data from the touch screen.
 *
 * This function enters factory mode and performs a touch difference test.
 *
 * @param[out] buf     A buffer to store the touch difference test result.
 */
void touch_read_diff_data(u8 *buf)
{
    fts_enter_factory_mode();

    TestItem_TouchDiffTest(buf, &bTestResult);
}
