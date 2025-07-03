/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     optical_sensor.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "ltr_sensor.h"
#include "led_driver.h"
#include "aw21036_driver.h"
#include "bct3236_driver.h"
#include "../../sdk/drivers/8278/i2c.h"
#include "../../sdk/drivers/8278/gpio.h"
#include "backlight_handle.h"
#include "stack/ble/ble.h"
#include "../profile_module/seneasy_profile.h"
#include "../app_rcu.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
u8 Read_Part_ID      = 0x00;
u8 Read_Dart_Offset  = 0x00;
u8 Read_DATA_LOW     = 0x00;
u8 Read_DATA_HIGH    = 0x00;
u16 ASL_ADC_Data     = 0x0000;
u8 Read_DATA_STATUS  = 0x00;
u8 ASL_Status_Data   = 0x00;
u8 ASL_Status_Valid  = 0x00;
u8 ASL_Status_Gain_Range = 0x00;
u16 ASL_Lux_Calc = 700;
u16 ASL_GAIN_Value = 1;
u16 ASL_INT_Value  = 1;
u16 ASL_WFAC_Value = 20;
u8 asl_status_error_cnt = 0;
u8 factory_Read_ASL = 0;
u16 Notify_Sy_Lux = 0;
u32 ASL_Test_BL_Lux = 0;
/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void ALS_Integration_Time(void);
static void ALS_Enable(void);

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  ltr sensor init
 * @param  none
 * @return none
 * @retval void
 */
void ltr_sensor_init(void)
{
    //I2C pin set
    gpio_set_func(GPIO_PD7,  AS_GPIO);
    gpio_set_output_en(GPIO_PD7, 1);
    gpio_write(GPIO_PD7,0);

    //I2C pin set
    user_i2c_gpio_set(I2C_GPIO_GROUP_C2C3);
	i2c_master_init(LTR_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );

    Read_Part_ID = i2c_read_byte(REG_PART_ID, 1);
    //printf("--->>> LTR_Sensor Read_Part_ID == 0x%02x\r\n",Read_Part_ID);

    ALS_Integration_Time();
    ALS_Enable();
    //ALS_Dark_Offset();

	if(Read_Part_ID != 0x1C)
	{
		sensor_init_error_cnt ++;
		if(sensor_init_error_cnt <= 10)
			sensor_init_timer = clock_time() | 1;
		else
        {
            printf("--->>> LTR SENSOR ERROR !!!\r\n");
            sensor_init_error_cnt = 0;
            aw21036_init();
            bct3236_init();
        }
	}
	else
	{
		//printf("--->>> LTR_Sensor_Chip_id: 0x%02x, init_error_cnt: %d\r\n",Read_Part_ID,sensor_init_error_cnt);
		sensor_init_error_cnt = 0;
        ALS_Read_Status_Init(); 
	}
}

/******************************************************************
 * @brief  ALS Integration Time
 * @param  none
 * @return none
 * @retval void
 */
static void ALS_Integration_Time(void)
{
    //printf("--->>> [work_debug] ALS_Integration_Time >>>\r\n");
    // Set ALS integration time 50msec (with default ALS measurement rate 400ms)
    //i2c_write_byte(REG_ALS_INT_TIME, 1, 0xA2);  // ALS_INT_TIME register
                                                // Integration time = 50msec
                                                // For Integration time = 100msec, Command = 0xA6
                                                // For Integration time = 200msec, Command = 0xAA
                                                // For Integration time = 400msec, Command = 0xAE

    // Set ALS measurement rate 100msec (with default ALS measurement time 100ms)
    i2c_write_byte(REG_ALS_INT_TIME, 1, 0xA4);  // ALS_INT_TIME register
                                                // measurement rate = 100msec
                                                // measurement rate = 200msec, Command = 0xA5
                                                // measurement rate = 400msec, Command = 0xA6
                                                // measurement rate = 800msec, Command = 0xA7                                    
    ASL_INT_Value = 1;
}

/******************************************************************
 * @brief  ALS Enable
 * @param  none
 * @return none
 * @retval void
 */
static void ALS_Enable(void)
{
    //printf("--->>> [work_debug] ALS_Enable >>>\r\n");
    i2c_write_byte(REG_ALS_CONFIG, 1, 0x00);    // ALS_CONFIG register
    i2c_write_byte(REG_MAIN_CONTR, 1, 0x10);    // MAIN_CONTR register
    i2c_write_byte(REG_ALS_CONTR,  1, 0x29);    // ALS_CONTR register
                                                // For Dynamic Range x1  , Command = 0x21 
                                                // For Dynamic Range x4  , Command = 0x25
                                                // For Dynamic Range x16 , Command = 0x29
                                                // For Dynamic Range x64 , Command = 0x2D
}

/******************************************************************
 * @brief  ALS Enable
 * @param  none
 * @return none
 * @retval void
 */
void ALS_Disable(void)
{
    i2c_master_init(LTR_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    i2c_write_byte(REG_ALS_CONTR,  1, 0x20);
}

#if 0
/******************************************************************
 * @brief  ALS Dark Offset
 * @param  none
 * @return none
 * @retval void
 */
static void ALS_Dark_Offset(void)
{
    i2c_write_byte(REG_DARK_CONTR, 1, 0x1C);        // For selecting Dark Offset register
    i2c_write_byte(REG_DARK_CONTR, 1, 0x1D);        // To confirm Dark Offset register selection

    Read_Dart_Offset = i2c_read_byte(REG_DARK_CONFIG, 1);
    Read_Dart_Offset = (Read_Dart_Offset >> 5) & 7; // Obtain dark offset references in 0xB9<7:5>
    printf("--->>> [work_debug] Read_Dart_Offset: %d\r\n",Read_Dart_Offset);
    i2c_write_byte(0x81, 1, 0x01);                  // For IC Reset
}
#endif

/******************************************************************
 * @brief  ALS Read Data
 * @param  none
 * @return lux
 * @retval void
 */
u16 ALS_Read_Data(void)
{
    //u8 sendBuff[2] = {0};
    i2c_master_init(LTR_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    Read_DATA_LOW  = i2c_read_byte(REG_ALS_DATA_LSB, 1); // ALS ADC low  byte address
    Read_DATA_HIGH = i2c_read_byte(REG_ALS_DATA_MSB, 1); // ALS ADC high byte address
    ASL_ADC_Data = (Read_DATA_HIGH << 8) | Read_DATA_LOW;// Combining lower and upper bytes to give 16-bit ALS ADC data  
                                                         // (Direct conversion to illuminance in lux).
    if(((backlight_hidden_Flag == 1) || (backlight_normal_Flag == 1)) && (factory_Read_ASL == 0))
    {
        u16 Test_ASL_Lux_Calc = 0;
        Test_ASL_Lux_Calc = (((144 * ASL_ADC_Data) / (ASL_GAIN_Value * ASL_INT_Value)) * ASL_WFAC_Value) / 1000;
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] ASL_ADC_DATA: %d, Test_ASL_Lux_Calc: %d Lux\r\n",ASL_ADC_Data,Test_ASL_Lux_Calc);
        #endif
#if (FUNCTION_CALIBRATION_BL)
        if(seneasy_bl_a_float == 0x01)
        {
            if(seneasy_bl_b_float == 0x01)
                ASL_Test_BL_Lux = ((ASL_Lux_Calc * seneasy_bl_a_data) + seneasy_bl_b_data) / 100;
            else if(seneasy_bl_b_float == 0x00)
                ASL_Test_BL_Lux = ((ASL_Lux_Calc * seneasy_bl_a_data) - seneasy_bl_b_data) / 100;
            ASL_Lux_Calc = ASL_Test_BL_Lux;
        }
#endif
        Notify_Sy_Lux = Test_ASL_Lux_Calc;
    }
    else
    {
        ASL_Lux_Calc = (((144 * ASL_ADC_Data) / (ASL_GAIN_Value * ASL_INT_Value)) * ASL_WFAC_Value) / 1000;
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] ASL_ADC_DATA: %d, ASL_Lux_Calc: %d Lux\r\n",ASL_ADC_Data,ASL_Lux_Calc);
        #endif
#if (FUNCTION_CALIBRATION_BL)
        if(seneasy_bl_a_float == 0x01)
        {
            if(seneasy_bl_b_float == 0x01)
                ASL_Test_BL_Lux = ((ASL_Lux_Calc * seneasy_bl_a_data) + seneasy_bl_b_data) / 100;
            else if(seneasy_bl_b_float == 0x00)
                ASL_Test_BL_Lux = ((ASL_Lux_Calc * seneasy_bl_a_data) - seneasy_bl_b_data) / 100;
            ASL_Lux_Calc = ASL_Test_BL_Lux;
        }
#endif
        Notify_Sy_Lux = ASL_Lux_Calc;
    }

    if(seneasy_get_Lux_flag == 0x01)
    {
        u8 sendBuff[2] = {0};
        seneasy_get_Lux_flag = 0x00;
        sendBuff[0] = U16_LO(Notify_Sy_Lux);
        sendBuff[1] = U16_HI(Notify_Sy_Lux);
        bls_att_pushNotifyData (SENEASY_RCU_WR_DP_H, sendBuff, 2);        
    }

    if(ASL_ADC_Data >= 0xFFFF)
        ASL_Lux_Calc = 700;
    return ASL_Lux_Calc;
}

/******************************************************************
 * @brief  ALS Read Status
 * @param  none
 * @return status
 * @retval void
 */
u8 ALS_Read_Status(void)
{       
    i2c_master_init(LTR_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    Read_DATA_STATUS = i2c_read_byte(REG_ALS_STATUS, 1); // ALS_STATUS register address
    //printf("--->>> [work_debug] Read_DATA_STATUS: 0x%02x\r\n",Read_DATA_STATUS);
    ASL_Status_Valid = Read_DATA_STATUS & 0x40;
    if(ASL_Status_Valid)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] ALS Data is invalid - ERROR\r\n");
        #endif
        //Sensor Error
        ASL_Lux_Calc = 700;
        //return 2;
    }

    ASL_Status_Gain_Range = Read_DATA_STATUS & 0x38;
    switch (ASL_Status_Gain_Range)
    {
    case 0x00:
        ASL_GAIN_Value = 1;
        break;
    case 0x08:
        ASL_GAIN_Value = 4;
        break;
    case 0x10:
        ASL_GAIN_Value = 16;
        break;
    case 0x18:
        ASL_GAIN_Value = 64;
        break;
    case 0x20:
        ASL_GAIN_Value = 128;
        break;    
    default:
        break;
    }
    //printf("--->>> [work_debug] ASL Gain: X%d\r\n",ASL_GAIN_Value);

    ASL_Status_Data = Read_DATA_STATUS & 0x01;
    if(ASL_Status_Data == 0x00)
    {
        if(asl_status_error_cnt < 20)
        {
            asl_status_error_cnt ++;
            ltr_sensor_timer = clock_time() | 1; 
        }
        else
        {
            //printf("--->>> [work_debug] ASL_Status_Data: %d ,ERROR !!!!!!\r\n",ASL_Status_Data);
            //Sensor Error
            ASL_Lux_Calc = 700;
            asl_status_error_cnt = 0;
            if(led_display_flag == 0x01)
                app_backlight_key_handle(led_display_keycode);
            else if(led_display_flag == 0x02)
                pressed_123_key();
            led_display_flag = 0x00;
        }
    }
    else if(ASL_Status_Data == 0x01)
    {
        //printf("--->>> [work_debug] ASL_Status_Data: %d ,New Data, error_cnt: %d \r\n",ASL_Status_Data,asl_status_error_cnt);
        ALS_Read_Data();
        asl_status_error_cnt = 0;
        if(led_display_flag == 0x01)
            app_backlight_key_handle(led_display_keycode);
        else if(led_display_flag == 0x02)
            pressed_123_key();
        led_display_flag = 0x00;
    }

    return ASL_Status_Data;
}

/******************************************************************
 * @brief  ALS Read Status Init
 * @param  none
 * @return status
 * @retval void
 */
u8 ALS_Read_Status_Init(void)
{       
    i2c_master_init(LTR_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    Read_DATA_STATUS = i2c_read_byte(REG_ALS_STATUS, 1); // ALS_STATUS register address
    //printf("--->>> [work_debug] ALS_Read_Status_Init ---- Read_DATA_STATUS: 0x%02x\r\n",Read_DATA_STATUS);
    ASL_Status_Valid = Read_DATA_STATUS & 0x40;
    if(ASL_Status_Valid)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] ALS Data is invalid - ERROR\r\n");
        #endif
        //Sensor Error
        ASL_Lux_Calc = 700;
        //return 2;
    }

    ASL_Status_Gain_Range = Read_DATA_STATUS & 0x38;
    switch (ASL_Status_Gain_Range)
    {
    case 0x00:
        ASL_GAIN_Value = 1;
        break;
    case 0x08:
        ASL_GAIN_Value = 4;
        break;
    case 0x10:
        ASL_GAIN_Value = 16;
        break;
    case 0x18:
        ASL_GAIN_Value = 64;
        break;
    case 0x20:
        ASL_GAIN_Value = 128;
        break;    
    default:
        break;
    }
    //printf("--->>> [work_debug] ASL Gain: X%d\r\n",ASL_GAIN_Value);

    ASL_Status_Data = Read_DATA_STATUS & 0x01;
    if(ASL_Status_Data == 0x00)
    {
        if(asl_status_error_cnt < 20)
        {
            asl_status_error_cnt ++;
            ASL_SENSOR_INIT_timer = clock_time() | 1; 
        }
        else
        {
            //printf("--->>> [work_debug] ASL_Status_Data: %d ,ERROR !!!!!!\r\n",ASL_Status_Data);
            //Sensor Error
            ASL_Lux_Calc = 700;
            led_display_flag = 0x00;
        }
    }
    else if(ASL_Status_Data == 0x01)
    {
        //printf("--->>> [work_debug] ASL_Status_Data_Init: %d ,New Data, error_cnt: %d \r\n",ASL_Status_Data,asl_status_error_cnt);
        asl_status_error_cnt = 0;
        aw21036_init();
        bct3236_init();
    }
    return ASL_Status_Data;
}


void ltr_sensor_init_TEST(void)
{
    //I2C pin set
    user_i2c_gpio_set(I2C_GPIO_GROUP_C2C3);
	i2c_master_init(LTR_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    Read_Part_ID = i2c_read_byte(REG_PART_ID, 1);
    ALS_Integration_Time();
    ALS_Enable();
}

u8 ALS_Read_Status_TEST(void)
{       
    i2c_master_init(LTR_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    Read_DATA_STATUS = i2c_read_byte(REG_ALS_STATUS, 1); // ALS_STATUS register address
    ASL_Status_Valid = Read_DATA_STATUS & 0x40;
    if(ASL_Status_Valid)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] ALS Data is invalid - ERROR\r\n");
        #endif
        //Sensor Error
        ASL_Lux_Calc = 700;
        //return 2;
    }
    ASL_Status_Gain_Range = Read_DATA_STATUS & 0x38;
    switch (ASL_Status_Gain_Range)
    {
    case 0x00:
        ASL_GAIN_Value = 1;
        break;
    case 0x08:
        ASL_GAIN_Value = 4;
        break;
    case 0x10:
        ASL_GAIN_Value = 16;
        break;
    case 0x18:
        ASL_GAIN_Value = 64;
        break;
    case 0x20:
        ASL_GAIN_Value = 128;
        break;    
    default:
        break;
    }
    ASL_Status_Data = Read_DATA_STATUS & 0x01;
    if(ASL_Status_Data == 0x00)
    {
        asl_status_error_cnt ++;
        if(asl_status_error_cnt < 10)
        {
            WaitMs(10);
            ALS_Read_Status();
        }
        else
        {
            //Sensor Error
            ASL_Lux_Calc = 700;
        }
    }
    else if(ASL_Status_Data == 0x01)
    {
        //printf("--->>> [work_debug] ASL_Status_Data: %d ,New Data, error_cnt: %d \r\n",ASL_Status_Data,asl_status_error_cnt);
        ALS_Read_Data();
    }
    return ASL_Status_Data;
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
