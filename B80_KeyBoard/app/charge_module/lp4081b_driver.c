/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     lp4081b_driver.c
* @brief    
* @details
* @author   chenrongrong
* @date     2024-05-31
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "lp4081b_driver.h"
#include "i2c.h"
#include "gpio.h"
#include "../keyboard_rcu.h"
#include "../led_module/led_driver.h"
#include "../led_module/pwm_handle.h"
#include "../key_module/key_driver.h"

#if(CHARGE_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
LP4081_REG_Typedef lp4081_reg;
bool app_rcu_charge_flag = false;
bool app_rcu_fully_charged = false;
u8 app_rcu_charge_First = 0x00;
u16 app_rcu_charge_batt = 0;
u8 user_led_blink_flag = 0x00;
u8 user_led_on_flag = 0x00;
_attribute_data_retention_ u32 app_rcu_charge_check_timer = 0;
extern u8 my_batVal[1];
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
static void read_reg_num(void);

/******************************************************************
 * @brief  lp4081_init
 * @param  none
 * @return none
 * @retval void
 */
void lp4081_init(void)
{
    i2c_gpio_set(GPIO_PD1,GPIO_PD0);
    i2c_master_init(LP40801B_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*100000)));

    lp4081_reg.intr_msk.byte = 0x00;   /* addr 02H */
    lp4081_reg.ctrl_0.byte = 0x24;     /* addr 03H */
    lp4081_reg.ctrl_1.byte = 0x44;     /* addr 04H */
    lp4081_reg.ctrl_2.byte = 0x9E;     /* addr 05H */
    lp4081_reg.ctrl_3.byte = 0x0B;     /* addr 06H */

    i2c_write_byte(LP4081_REG_MSK,1,lp4081_reg.intr_msk.byte);
    i2c_write_byte(LP4081_REG_CTRL0,1,lp4081_reg.ctrl_0.byte);
    i2c_write_byte(LP4081_REG_CTRL1,1,lp4081_reg.ctrl_1.byte);
    i2c_write_byte(LP4081_REG_CTRL2,1,lp4081_reg.ctrl_2.byte);
    i2c_write_byte(LP4081_REG_CTRL3,1,lp4081_reg.ctrl_3.byte);

    //printf("lp4081_init\n");
}

/******************************************************************
 * @brief  lp4081_ntc_set
 * @param  none
 * @return none
 * @retval void
 */
void lp4081_ntc_set(void)
{
    i2c_gpio_set(GPIO_PD1,GPIO_PD0);
    i2c_master_init(LP40801B_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*100000)));
    lp4081_reg.ctrl_3.byte = 0x2B;     /* addr 06H */
    i2c_write_byte(LP4081_REG_CTRL3,1,lp4081_reg.ctrl_3.byte);
    printf("lp4081_ntc_set--->>> \r\n");
}

/******************************************************************
 * @brief  enter_auto_mode
 * @param  none
 * @return none
 * @retval void
 */
void enter_auto_mode(void)
{
    i2c_master_init(LP40801B_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*100000)));
    lp4081_reg.ctrl_0.byte = 0x26;     /* addr 03H */
    i2c_write_byte(LP4081_REG_CTRL0,1,lp4081_reg.ctrl_0.byte);
    read_reg_num();
    printf("enter_auto_mode\n");
}

/******************************************************************
 * @brief  enter_ship_mode
 * @param  none
 * @return none
 * @retval void
 */
void enter_ship_mode(void)
{
    enter_auto_mode();   /* should be set to auto mode before enter ship mode */
    WaitMs(10);
    i2c_master_init(LP40801B_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*100000)));
    lp4081_reg.ctrl_0.byte = 0x27;     /* addr 03H */
    i2c_write_byte(LP4081_REG_CTRL0,1,lp4081_reg.ctrl_0.byte);
    read_reg_num();
    printf("enter_ship_mode\n");
}

/******************************************************************
 * @brief  read_temperature
 * @param  none
 * @return  0: < 0°
 *          1：0-10°
 *          2: 10-45°
 *          3：> 45°
 * @retval int
 */
int read_temperature(void)
{
    int ret;
    unsigned char temperature;
    i2c_master_init(LP40801B_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*100000)));
    temperature = i2c_read_byte(LP4081_REG_STUTAS1, 1);
    switch (temperature)
    {
    case 0x01:
        ret = 0;
        printf("Current temperature is < 0°\r\n");
        break;
    case 0x02:
        ret = 1;
        printf("Current temperature is < 0-10°\r\n");
        break;
    case 0x03:
        ret = 2;
        printf("Current temperature is < 10-45°\r\n");
        break;
    case 0x04:
        ret = 3;
        printf("Current temperature is > 45°\r\n");
        break;
    default:
        ret = -1;
        printf("[ERR] invalid value\r\n");
        break;
    }

    return ret;
}

u8 state_of_charge(void)
{
    u8 ret;
    i2c_master_init(LP40801B_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*100000)));
    lp4081_reg.status.byte = i2c_read_byte(LP4081_REG_STUTAS0, 1);
    ret = lp4081_reg.status.fields.chg;
    //printf("is_charging 0x%02x,%02x , ",lp4081_reg.status.byte,ret);
    if(ret != 0)
    {
        app_rcu_charge_flag = true;
        if(ret == 3)
            app_rcu_fully_charged = true;
        else
            app_rcu_fully_charged = false;
        if(user_led_blink_flag == 0x00)
        {
            printf("app_rcu_charge_flag is TRUE\r\n");
            led_pwm_init();
            lp4081_ntc_set();
        }
        read_temperature();
        app_rcu_charge_check_timer = clock_time() | 1; 
    }
    else
    {
        app_rcu_charge_flag = false;
        app_rcu_fully_charged = false;
        //printf("app_rcu_charge_flag is FALSE\r\n");
        read_temperature();
        led_1_driver_init();
        gpio_write(GPIO_LED_1,!LED_ON_LEVAL);
        app_rcu_charge_check_timer = 0;
        user_led_blink_flag = 0x00;
    }
    return ret;
}

static void read_reg_num(void)
{
    unsigned char read_data[LP4081_REG_NUM] = {0};
    int i;
    for(i=0;i<7;i++)
    {
        read_data[i] = i2c_read_byte(LP4081_REG_STUTAS0 + i, 1);
        printf("--->>> read_reg_num[%d]  == 0x%02x\r\n",i,read_data[i]);
    }
}

/*=========================================================================================*
 *                             Interrupt Functions
 * The interrupt pin IRQb pulls to low for 256us when an interrupt event happens in the chip.
 * The interrupt events can be masked by the control bits in the Interrupt Mask Register.
 *=========================================================================================*/

#define IRQ_PIN   GPIO_PA7
/**
 * @brief       irqb_col_gpio_init
 * @param[in]   col_pin	- 
 * @param[in]   col_pull- 
 * @return      none
 * @note        
 */
void irqb_col_gpio_init(GPIO_PinTypeDef col_pin)
{
	gpio_set_func(col_pin,AS_GPIO);			//IO设为普通IO
	gpio_set_output_en(col_pin,0);	 		//输出使能关掉
	gpio_set_input_en(col_pin,1);			//使能输入
    gpio_setup_up_down_resistor(col_pin,PM_PIN_PULLUP_1M);//设置上拉1M电阻
	gpio_write(col_pin,0);					//输出设为0
	cpu_set_gpio_wakeup(col_pin, Level_Low, 1);
}


/**
 * @brief       init irqb_gpio_init
 * @param[in]   void- 
 * @return      none
 * @note        
 */
void irqb_gpio_init(void)
{
	irqb_col_gpio_init(IRQ_PIN);
    //printf("--->>> [work_debug] irqb_gpio_init\r\n");
    state_of_charge();
}

/**
 * @brief       init irqb_scan_func
 * @param[in]   void- 
 * @return      none
 * @note        
 */
void irqb_scan_func(void)
{
    if(gpio_read(IRQ_PIN) == 0){
        rf_24G_deep_tick = clock_time();
        if(key_power_flag == 1)
            return;
        if(state_of_charge() == 0){
            app_rcu_charge_flag = false;
        }
    }
}

/******************************************************************
 * @brief  app_check_usb_typeC
 * @param  none
 * @return none
 * @retval void
 */
int getByte(char byte,int bit)
{
    if(bit < 0 || bit > 7)
        return 0;
    return (byte & (0b1 << bit)) > 0 ? 1: 0;
}
void app_check_charge_batt(void)
{
    app_rcu_charge_batt = get_battery_voltage();
    state_of_charge();
    printf("--->>> [work_debug] Charge_Flag: %d, Charge_BATT: %d mV\r\n",app_rcu_charge_flag,app_rcu_charge_batt);
    if(app_rcu_charge_flag)
    {
        app_rcu_charge_check_timer = clock_time() | 1;
        printf("--->>> [work_debug] app_rcu_fully_charged: %d\r\n",app_rcu_fully_charged);
        if(app_rcu_fully_charged)
        {
            printf("--->>> [work_debug] Charge Finish !!!\r\n",app_rcu_charge_flag);
            user_led_blink_flag = 0x00;
            if(user_led_on_flag == 0x00)
            {
                led_1_driver_init();
                gpio_write(GPIO_LED_1,LED_ON_LEVAL);
            }
            user_led_on_flag = 0x01;
        }
        else
        {
            if(user_led_on_flag == 0x01)
            {
                printf("--->>> [work_debug] Charge Finish 222 !!!\r\n",app_rcu_charge_flag);
                return;
            }      
            user_led_on_flag = 0x00;
            if(user_led_blink_flag == 0x00)
                led_pwm_init(); 
            user_led_blink_flag = 0x01;
        }
    }
    else
    {
        app_rcu_charge_check_timer = 0;
        user_led_blink_flag = 0;
        user_led_on_flag = 0;
        led_1_driver_init();
        gpio_write(GPIO_LED_1,!LED_ON_LEVAL);
    }
}


#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#define ADC_SAMPLE_NUM		8
volatile unsigned int adc_dat_buf[ADC_SAMPLE_NUM];  //size must 16 byte aligned(16/32/64...)
_attribute_data_reload_ u8 adc_hw_initialized = 0;   //note: can not be retention variable
 u16 batt_vol_mv;
void adc_vbat_detect_init(void)
{
    // DFIFO Mode
    /******power off sar adc********/
    adc_power_on_sar_adc(0);

    gpio_set_output_en(GPIO_VBAT_DETECT, 1);
    //gpio_write(GPIO_VBAT_DETECT, 1);
    gpio_set_input_en(GPIO_VBAT_DETECT,1);
    gpio_set_output_en(GPIO_VBAT_DETECT, 0);

    /******set adc sample clk as 4MHz******/
    adc_set_sample_clk(5); //adc sample clk= 24M/(1+5)=4M

    //set misc channel en,  and adc state machine state cnt 2( "set" stage and "capture" state for misc channel)
    adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);      //set total length for sampling state machine and channel

    //set "capture state" length for misc channel: 240
    //set "set state" length for misc channel: 10
    //adc state machine  period  = 24M/250 = 96K, T = 10.4 uS
    adc_set_state_length(240, 10);      //set R_max_mc,R_max_c,R_max_s

#if 1  //optimize, for saving time
    //set misc channel use differential_mode,
    //set misc channel resolution 14 bit,  misc channel differential mode
    //notice that: in differential_mode MSB is sign bit, rest are data,  here BIT(13) is sign bit
    analog_write (anareg_adc_res_m, RES14 | FLD_ADC_EN_DIFF_CHN_M);
    adc_set_ain_chn_misc(ADC_INPUT_PCHN, GND);
#else
////set misc channel use differential_mode,
    adc_set_ain_channel_differential_mode(ADC_INPUT_PCHN, GND);

    //set misc channel resolution 14 bit
    //notice that: in differential_mode MSB is sign bit, rest are data,  here BIT(13) is sign bit
    adc_set_resolution(RES14);
#endif

    //set misc channel vref 1.2V
    adc_set_ref_voltage(ADC_VREF_1P2V);

    //set misc t_sample 6 cycle of adc clock:  6 * 1/4M
#if 1   //optimize, for saving time
    adc_set_tsample_cycle_chn_misc(SAMPLING_CYCLES_6);      //Number of ADC clock cycles in sampling phase
#else
    adc_set_tsample_cycle(SAMPLING_CYCLES_6);       //Number of ADC clock cycles in sampling phase
#endif

    //set Analog input pre-scal.ing 1/8
    adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);

    /******power on sar adc********/
    //note: this setting must be set after all other settings
    adc_power_on_sar_adc(1);
}

u16 get_battery_voltage(void){    //mv
    u16 temp;
    int i,j;

    //when MCU powered up or wakeup from deep/deep with retention, adc need be initialized
    if(!adc_hw_initialized){
        adc_hw_initialized = 1;
        adc_vbat_detect_init();
    }

    adc_reset_adc_module();
#if (!MANNUAL_MODE_GET_ADC_SAMPLE_RESULT)
    u32 t0 = clock_time();

#if (DBG_ADC_SAMPLE_DAT)
    adc_dat_buf = (int *)&adc_dat_raw[ADC_SAMPLE_NUM*adc_index];
#else
    u16 adc_sample[ADC_SAMPLE_NUM] = {0};
    u32 adc_result;
#endif
    for(i=0;i<ADC_SAMPLE_NUM;i++){       //dfifo data clear
        adc_dat_buf[i] = 0;
    }
    while(!clock_time_exceed(t0, 25));  //wait at least 2 sample cycle(f = 96K, T = 10.4us)

    //dfifo setting will lose in suspend/deep, so we need config it every time
    adc_config_misc_channel_buf((u16 *)adc_dat_buf, ADC_SAMPLE_NUM<<2);  //size: ADC_SAMPLE_NUM*4
    dfifo_enable_dfifo2();


//////////////// get adc sample data and sort these data ////////////////
    for(i=0;i<ADC_SAMPLE_NUM;i++){
        while(!adc_dat_buf[i]);


        if(adc_dat_buf[i] & BIT(13)){  //14 bit resolution, BIT(13) is sign bit, 1 means negative voltage in differential_mode
            adc_sample[i] = 0;
        }
        else{
            adc_sample[i] = ((u16)adc_dat_buf[i] & 0x1FFF);  //BIT(12..0) is valid adc result
        }

#if (DBG_ADC_SAMPLE_DAT) //debug
        if(adc_sample[i] < adc_dat_min){
            adc_dat_min = adc_sample[i];
        }
        if(adc_sample[i] > adc_dat_max){
            adc_dat_max = adc_sample[i];
        }
#endif

        //insert sort
        if(i){
            if(adc_sample[i] < adc_sample[i-1]){
                temp = adc_sample[i];
                adc_sample[i] = adc_sample[i-1];
                for(j=i-1;j>=0 && adc_sample[j] > temp;j--){
                    adc_sample[j+1] = adc_sample[j];
                }
                adc_sample[j+1] = temp;
            }
        }
    }
//////////////////////////////////////////////////////////////////////////////
    dfifo_disable_dfifo2();   //misc channel data dfifo disable

///// get average value from raw data(abandon some small and big data ), then filter with history data //////
#if (ADC_SAMPLE_NUM == 4)      //use middle 2 data (index: 1,2)
    u32 adc_average = (adc_sample[1] + adc_sample[2])/2;
#elif(ADC_SAMPLE_NUM == 8)     //use middle 4 data (index: 2,3,4,5)
    u32 adc_average = (adc_sample[2] + adc_sample[3] + adc_sample[4] + adc_sample[5])/4;
#endif

#else
    unsigned char adc_misc_data_L;
    unsigned char adc_misc_data_H;
    unsigned short adc_misc_data;
    u32 adc_result;
    u32 adc_average;
    analog_write(adc_data_sample_control,analog_read(adc_data_sample_control) | NOT_SAMPLE_ADC_DATA);
    adc_misc_data_L = analog_read(areg_adc_misc_l);
    adc_misc_data_H = analog_read(areg_adc_misc_h);
    analog_write(adc_data_sample_control,analog_read(adc_data_sample_control) & (~NOT_SAMPLE_ADC_DATA));

    adc_misc_data = (adc_misc_data_H<<8 | adc_misc_data_L);

    if(adc_misc_data & BIT(13)){
        adc_misc_data=0;
    }
    else{
        adc_misc_data  = (adc_misc_data * adc_vref_cfg.adc_vref)>>10;
    }
    adc_average = adc_misc_data;

#endif

#if 1
    adc_result = adc_average;
#else      //history data filter
    if(adc_first_flg){
        adc_result = adc_average;
        adc_first_flg = 0;
    }
    else{
        adc_result = ( (adc_result*3) + adc_average + 2 )>>2;  //filter
    }
#endif

//////////////// adc sample data convert to voltage(mv) ////////////////
    //                          (Vref, 1/8 scaler)   (BIT<12~0> valid data)
    //             =  adc_result * Vref * 8 / 0x2000
    //           =  adc_result * Vref >>10
    extern unsigned short g_adc_vref;
    batt_vol_mv  = (adc_result * g_adc_vref)>>10;
    batt_vol_mv = ((u32)(batt_vol_mv*(1000+100+1000)))/1000;  //检测到的电压 再按分压得到实际电压

    if(batt_vol_mv > 4200)
        my_batVal[0] = 100;
    else if (batt_vol_mv > 4100)
        my_batVal[0] = 90;
    else if (batt_vol_mv > 4000)
        my_batVal[0] = 80;
    else if (batt_vol_mv > 3900)
        my_batVal[0] = 70;
    else if (batt_vol_mv > 3800)
        my_batVal[0] = 50;
    else if (batt_vol_mv > 3700)
        my_batVal[0] = 30;
    else if (batt_vol_mv > 3600)
        my_batVal[0] = 20;
    else
        my_batVal[0] = 0; 

    return batt_vol_mv;
}


#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
