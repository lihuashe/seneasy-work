#include "buzzer.h"
#include "rtl876x_tim.h"
#include "rtl876x_enh_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include <app_section.h>
#include "swtimer.h"
#include "os_timer.h"
#include "trace.h"
#include "voltage_driver.h"
#include "common_macros.h"

static Voltage_Map now_voltage;


/**
 * @brief 这是高压部分的正反向电路gpio控制初始化
 * @param  
 */
void voltage_gpio_init(void)
{
    
    Pad_Config(HV_HIV_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(HV_LIV_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    
    Pad_Config(DC24V_OR_DC14V_POWER_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(HV_POWER_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LV_POWER_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);

    Pad_Config(OC1_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(OC2_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(OC3_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(OC4_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    /* Normal mode */
    Pinmux_Config(HV_HIV_Pin, DWGPIO);
    Pinmux_Config(HV_LIV_Pin, DWGPIO);

    Pinmux_Config(DC24V_OR_DC14V_POWER_Pin, DWGPIO);
    Pinmux_Config(HV_POWER_Pin, DWGPIO);
    Pinmux_Config(LV_POWER_Pin, DWGPIO);
    
    Pinmux_Config(OC1_Pin, DWGPIO);
    Pinmux_Config(OC2_Pin, DWGPIO);
    Pinmux_Config(OC3_Pin, DWGPIO);
    Pinmux_Config(OC4_Pin, DWGPIO);

    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);


    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    // 配置引脚为模式，启用上拉电阻
    
    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(DC24V_OR_DC14V_POWER_Pin) | GPIO_GetPin(HV_POWER_Pin) | GPIO_GetPin(LV_POWER_Pin) | GPIO_GetPin(OC1_Pin) 
    | GPIO_GetPin(OC2_Pin) | GPIO_GetPin(OC3_Pin) | GPIO_GetPin(OC4_Pin) | GPIO_GetPin(HV_HIV_Pin) | GPIO_GetPin(HV_LIV_Pin); 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; // 设置为输出模式
    GPIO_InitStruct.GPIO_ITCmd = DISABLE; // 禁用中断
    GPIO_InitStruct.GPIO_ControlMode = GPIO_SOFTWARE_MODE; 
    GPIO_Init(&GPIO_InitStruct);

    OC1_Pin_OPEN();
    OC2_Pin_OPEN();
    OC3_Pin_OPEN();
    OC4_Pin_OPEN();
}

/**
 * @brief 低压部分复用设置
 * @param  
 */
void lv_pad_pwm_pn(void)
{
    Pad_Config(LV_HIV_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LV_LIV_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    
    Pinmux_Config(LV_HIV_Pin, TIM_PWM2);

}

/**
 * @brief 低压部分使用互补pwm输出
 * @param  
 */
void lv_pwm_init(void)
{
    lv_pad_pwm_pn();

    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_InitStruct;

    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
    TIM_InitStruct.TIM_PWM_En   = PWM_ENABLE;
    TIM_InitStruct.TIM_Period   = 2000 * 10 * 4;         //周期us,40mhz
    TIM_InitStruct.TIM_PWM_High_Count   = 1000 * 10 * 4; //高电平
    TIM_InitStruct.TIM_PWM_Low_Count    = 1000 * 10 * 4; //低电平
    // TIM_InitStruct.PWM_Stop_State_P     = PWM_STOP_AT_LOW;
    // TIM_InitStruct.PWM_Stop_State_N     = PWM_STOP_AT_HIGH;
    // TIM_InitStruct.PWMDeadZone_En       = DEADZONE_ENABLE;  //enable to use pwn p/n output
    // TIM_InitStruct.PWM_Deazone_Size     = 15;
    TIM_TimeBaseInit(LV_PWM_TIMER_NUM, &TIM_InitStruct);

    TIM_Cmd(LV_PWM_TIMER_NUM , DISABLE);
}

/**
 * @brief 这是电压输出的定时器周期初始化
 * @param  
 */
void voltage_init(void)
{
    voltage_gpio_init();
    lv_pwm_init();

    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_InitStruct;

    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_Mode   = TIM_Mode_UserDefine;
    TIM_InitStruct.TIM_Period =  600 * 1000 * 10 * 4;//40次计数为1us
    TIM_TimeBaseInit(VOLTAGE_PWM_TIMER_NUM, &TIM_InitStruct);

    /*  Enable TIMER IRQ  */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = VOLTAGE_TIMER_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_ClearINT(VOLTAGE_PWM_TIMER_NUM);
    TIM_INTConfig(VOLTAGE_PWM_TIMER_NUM, ENABLE);
    TIM_Cmd(VOLTAGE_PWM_TIMER_NUM,  ENABLE);
}


/**
 * @brief 这是电压脉冲周期中断处理
 * @param  
 */
void VOLTAGE_INT_Handler(void)
{
    TIM_ClearINT(VOLTAGE_PWM_TIMER_NUM);
    TIM_Cmd(VOLTAGE_PWM_TIMER_NUM, DISABLE);
    
    T_IO_MSG voltage_msg;
    voltage_msg.type = IO_MSG_TYPE_TIMER;
    voltage_msg.subtype = IO_MSG_SUBTYPE_VOLTAGE;

    if (false == app_send_msg_to_apptask(&voltage_msg))
    {
        PRINT("send voltage message failed!\n");
    }

    TIM_Cmd(VOLTAGE_PWM_TIMER_NUM, ENABLE);
}

/*============================================================================*
 *                           Must Functions 
 *============================================================================*/

/**
 * @brief 这是一个解耦合接口，移植要实现这个函数
 * @param timer_num 哪一个定时器
 * @param period    周期是多少
 */
static void voltage_timer_change_peroid(void *timer_num, uint32_t period)
{  
    TIM_TypeDef *timer_num_t = timer_num;

    if (period == 0)
    {
        TIM_ChangePeriod(timer_num_t, 5); 
    }
    else
    {
        TIM_ChangePeriod(timer_num_t, period); 
    }
    
}

/**
 * @brief 这是一个解耦合的开关定时器的函数
 * @param timer_num 哪一个定时器
 * @param cmd 开or关
 */
static void voltage_timer_open_off(void *timer_num, uint8_t cmd)
{
    TIM_TypeDef *timer_num_t = timer_num;
    TIM_Cmd(timer_num_t, (FunctionalState)cmd);
}


/**
 * @brief 关闭中断
 * @param  
 * @return 
 */
static uint32_t voltage_lock(void)
{
    return os_lock();
}


/**
 * @brief 打开中断
 * @param lock_num 锁上的序号
 * @return 
 */
static void voltage_unlock(uint32_t lock_num)
{
    os_unlock(lock_num);
}


/*============================================================================*
 *                           Normal Functions 
 *============================================================================*/
/**
 * @brief 开启高压必须关断低压，防止倒灌
 * @param  
 */
void open_voltage_1800v(void)
{
    uint32_t hv_s = voltage_lock();//进入临界

    SET_VOLTAGE_Pin(DC24V_OR_DC14V_POWER_Pin, 0);

    LV_POWER_OFF();
    VOLTAGE_DELAY_US(100);
    HV_POWER_OPEN();
    
    voltage_unlock(hv_s);
}

/**
 * @brief 开启低压必须关断高压，防止倒灌
 * @param  
 */
void open_voltage_24v(void)
{
    HV_POWER_OFF();
    VOLTAGE_DELAY_US(500);
    LV_POWER_OPEN();

}


/**
 * @brief 关闭高压输出
 * @param  
 */
void hv_io_off(void)
{
    voltage_timer_open_off(VOLTAGE_PWM_TIMER_NUM, DISABLE);//失能电压控制时钟
    HV_POWER_OFF();
    SET_VOLTAGE_Pin(HV_HIV_Pin, 0);
    SET_VOLTAGE_Pin(HV_HIV_Pin, 0);
}


/**
 * @brief 关闭低压脉冲输出
 * @param  
 */
void lv_pwm_off(void)
{
    LV_POWER_OFF();
    voltage_timer_open_off(LV_PWM_TIMER_NUM , DISABLE);
}

/**
 * @brief 关闭高中低压
 * @param  
 */
void out_voltage_off(void)
{
    hv_io_off();
    lv_pwm_off();
}

/**
 * @brief 开启oc1-oc4
 * @param  
 */
void oc1_oc4_open(void)
{
    OC1_Pin_OPEN();
    OC2_Pin_OPEN();
    OC3_Pin_OPEN();
    OC4_Pin_OPEN();
}

/**
 * @brief 关闭oc1-oc4
 * @param  
 */
void oc1_oc4_off(void)
{
    OC1_Pin_OFF();
    OC2_Pin_OFF();
    OC3_Pin_OFF();
    OC4_Pin_OFF();
}

/**
 * @brief 通过oc1-oc4有节奏的开关形成有规律脉冲
 * @param  
 */
void oc1_4_open_off_rhythmic_pulse(Oc_Open_Off_Cycles *oc_cycles) 
{
    
    if (oc_cycles == NULL) 
    {
        return ; // 检查输入指针是否有效
    }

    oc1_oc4_off();//先全部关闭引脚使能

    static uint8_t count_cycles = 0;
    static uint8_t beat_ratio = 0;

    Oc_Open_Off_Define *oc_ctrl = oc_cycles->oc_ctrl;
    // 遍历每个OC控制定义
    
    uint8_t oc1 = oc_ctrl[count_cycles].oc1;
    uint8_t oc2 = oc_ctrl[count_cycles].oc2;
    uint8_t oc3 = oc_ctrl[count_cycles].oc3;
    uint8_t oc4 = oc_ctrl[count_cycles].oc4;
    

    // 依据beat_ratio执行次数
    if (beat_ratio >= oc_ctrl[count_cycles].beat_ratio) 
    {

        if (count_cycles < ARRAY_LENGTH(oc_cycles->oc_ctrl) - 1)
        {
            count_cycles++;
        }
        else
        {
            count_cycles = 0;
        }
        beat_ratio = 0;
    }

    if (oc1) OC1_Pin_OPEN();
    if (oc2) OC2_Pin_OPEN();
    if (oc3) OC3_Pin_OPEN();
    if (oc4) OC4_Pin_OPEN();

    beat_ratio++;      
}

void mode_set_oc1_4_start(void)
{
    for (uint8_t i = 0; i < ARRAY_LENGTH(oc1_4_map); i++)
    {
        if (now_voltage.mode == oc1_4_map[i].mode && now_voltage.gear == oc1_4_map[i].gear)
        {
            oc1_4_open_off_rhythmic_pulse((Oc_Open_Off_Cycles *)&oc1_4_map[i]);
        }
    }
}


/**
 * @brief 生成有规律的循环脉冲
 * @param pulse_count 起始脉冲数
 * @param loop_count 循环了多少次
 * @return 此次执行的脉冲数
 */
uint8_t voltage_loop_change_pulse_count(uint8_t pulse_count, uint8_t *loop_count)
{
    uint8_t loop_change[5] = {pulse_count, pulse_count - 1, pulse_count - 2, pulse_count -3};

    if (*loop_count < 18) 
    {
        return loop_change[(*loop_count % 6) / 2];
    } 
    else 
    {
        if (*loop_count >= 35)
        {
            *loop_count = 0;
            return loop_change[3];
        }
        return loop_change[1 + (*loop_count % 6) / 2];
    }

}


/**
 * @brief 开启高压循环
 * @param flag 
 * @param hv_pulse_count 
 * @param hv_pulse_space 
 */
void voltage_hv_pulse_do(bool *flag, uint8_t hv_pulse_count, uint32_t hv_pulse_space)
{
    for (uint8_t i = 0; i < hv_pulse_count; i++)
    {
        uint32_t hv_s = voltage_lock();//进入临界

        if (*flag == false)
        {
            *flag = true;
            SET_VOLTAGE_Pin(HV_LIV_Pin, 1);
            VOLTAGE_DELAY_US(HV_LEVEL_KEEP_TIME);

            SET_VOLTAGE_Pin(HV_LIV_Pin, 0);
            VOLTAGE_DELAY_US(LEVEL_DEAD_TIME);
        }
        else
        {
            *flag = false;
            SET_VOLTAGE_Pin(HV_HIV_Pin, 1);
            VOLTAGE_DELAY_US(HV_LEVEL_KEEP_TIME);

            SET_VOLTAGE_Pin(HV_HIV_Pin, 0);
            VOLTAGE_DELAY_US(LEVEL_DEAD_TIME);
        }
        voltage_unlock(hv_s);

        VOLTAGE_DELAY_US(hv_pulse_space);
    }
}

/**
 * @brief 这是发生档位变化的处理函数
 * @param mode 现在的模式
 * @param gear 现在的档位
 */
void voltage_mode_gear_change(uint8_t mode, uint8_t gear)
{
    for (uint8_t i = 0; i < sizeof(voltage_map)/sizeof(Voltage_Map); i++)
    {
        if (voltage_map[i].mode == mode && voltage_map[i].gear == gear)
        {
            now_voltage = voltage_map[i];
        }
    }

    voltage_mode_gear_do();
}


/**
 * @brief 定时器循环设置
 * @param loop_cycles 循环数组
 * @param cycles_len 数组长度
 * @param arr_on     数组元素位置标记
 * @param loop_count 循环次数
 * @param hv_pulse_count 高压脉冲次数
 * @return 
 */
bool timer_change_set_cycles(void *loop_cycles, uint8_t cycles_len, uint8_t *arr_on, uint8_t *loop_count, uint8_t hv_pulse_count)
{
    Cycles_Time_Count *cycles = (Cycles_Time_Count *)loop_cycles;
    
    IF_DO(*arr_on >= cycles_len, return false);

    for (uint8_t i = *arr_on; i < cycles_len; i++)
    {
        if (cycles[i].cycles_time != 0)
        {
            *arr_on = i;
            voltage_timer_open_off(VOLTAGE_PWM_TIMER_NUM, DISABLE);
            //PRINT("cycles[i].cycles_time:%d\n", cycles[i].cycles_time);
            voltage_timer_change_peroid(VOLTAGE_PWM_TIMER_NUM, (cycles[i].cycles_time - hv_pulse_count) * VOLTAGE_TIMER_MS_COUNT);
            voltage_timer_open_off(VOLTAGE_PWM_TIMER_NUM, ENABLE);
            break;
        }
    }

    if (*arr_on < (cycles_len - 1) && *loop_count >= cycles[*arr_on].cycles_count)
    {
        (*arr_on)++;
        *loop_count = 0;
    }
    
    if (*arr_on == (cycles_len - 1) && *loop_count >= cycles[*arr_on].cycles_count)
    {
        *arr_on = 0;
        *loop_count = 1;
        return true;
    }
    (*loop_count)++;
    
    return false;
}

/**
 * @brief 输出电压的档位对应的脉冲和周期的处理
 * @param  
 */
void voltage_mode_gear_do(void)
{   
    static bool hv_or_lv = false;
    static uint8_t last_mode = SCALP_MODE;
    static uint8_t hv_lv_tick = 0;
    static uint8_t hv_lv_count_tick = 1;
    static uint8_t lv_tick = 0;
    static uint8_t lv_count_tick = 1;
    static uint8_t hv_lv_switching = HV_MODE;

    voltage_timer_open_off(VOLTAGE_PWM_TIMER_NUM, ENABLE);//使能电压控制时钟

    if (last_mode != now_voltage.mode)
    {
        hv_or_lv = false;
        hv_lv_tick = 0;
        hv_lv_count_tick = 1;
        lv_tick = 0;
        lv_count_tick = 1;
        hv_lv_switching = HV_MODE;
        last_mode = now_voltage.mode;
    }

    oc1_oc4_open();
    mode_set_oc1_4_start();

    if (now_voltage.hv_lv_mode == HV_MODE)
    { 
        open_voltage_1800v();
 
        voltage_hv_pulse_do(&hv_or_lv, now_voltage.hv_pulse_count, CHANGE_DELAY_US);

        //定时器处理部分
        static uint8_t hv_lv_tick = 0;
        static uint8_t hv_lv_count_tick = 1;

        timer_change_set_cycles((void *)&now_voltage.hv_loop_cycles, ARRAY_LENGTH(now_voltage.hv_loop_cycles), &hv_lv_tick, &hv_lv_count_tick, now_voltage.hv_pulse_count);
    }
    else if (now_voltage.hv_lv_mode == HV_AND_LV_MODE)
    {
       
        //高压部分
        if (hv_lv_switching == HV_MODE)
        {
            //PRINT("hv_lv_switching == HV_MODE\n");
            open_voltage_1800v();
                
            if (now_voltage.mode == FACE_MESO_MODE)
            {
                voltage_hv_pulse_do(&hv_or_lv, now_voltage.hv_pulse_count, CHANGE_DELAY_US);

                //定时器处理部分

                bool status = timer_change_set_cycles((void *)&now_voltage.hv_loop_cycles, ARRAY_LENGTH(now_voltage.hv_loop_cycles), &hv_lv_tick, &hv_lv_count_tick, now_voltage.hv_pulse_count);

                if (status == true)
                {
                    voltage_timer_change_peroid(VOLTAGE_PWM_TIMER_NUM, 15 * VOLTAGE_TIMER_MS_COUNT);
                    hv_lv_switching = LV_MODE;
                }
            }
            else if (now_voltage.mode == SCALP_MESO_MODE)
            {
                static uint8_t save_loop_count = 0;
                uint8_t now_pulse_count = voltage_loop_change_pulse_count(now_voltage.hv_pulse_count, &save_loop_count);
                save_loop_count++;

                voltage_hv_pulse_do(&hv_or_lv, now_pulse_count, CHANGE_DELAY_US);
                //PRINT("now_pulse_count:%d\n", now_pulse_count);
                //定时器处理部分
                static uint8_t hv_lv_tick = 0;
                static uint8_t hv_lv_count_tick = 1;

                bool status = timer_change_set_cycles((void *)&now_voltage.hv_loop_cycles, ARRAY_LENGTH(now_voltage.hv_loop_cycles), &hv_lv_tick, &hv_lv_count_tick, now_pulse_count);

                if (status == true)
                {
                    save_loop_count = 0;
                    voltage_timer_change_peroid(VOLTAGE_PWM_TIMER_NUM, 15 * VOLTAGE_TIMER_MS_COUNT);
                    hv_lv_switching = LV_MODE;
                }
            }
            
        }
        else if (hv_lv_switching == LV_MODE)//中低压处理
        {
            oc1_oc4_open();
            //PRINT("hv_lv_switching == LV_MODE\n");

            uint32_t lv_s = voltage_lock();//进入临界
            
            open_voltage_24v();

            voltage_unlock(lv_s);

            if (lv_tick == 0 && now_voltage.lv_loop_cycles[lv_tick].cycles_time != 0)
            {              
                //SET_VOLTAGE_Pin(DC24V_OR_DC14V_POWER_Pin, 1);
                TIM_ChangePeriod(LV_PWM_TIMER_NUM, 2000 * 4 * 10);
				TIM_PWMChangeFreqAndDuty(LV_PWM_TIMER_NUM, 1000 * 4 * 10, 1000 * 4 * 10); //占空比10%
                voltage_timer_open_off(LV_PWM_TIMER_NUM , ENABLE);
            }
            else
            {
                //SET_VOLTAGE_Pin(DC24V_OR_DC14V_POWER_Pin, 0);
                TIM_ChangePeriod(LV_PWM_TIMER_NUM, 600 * 4 * 10);
				TIM_PWMChangeFreqAndDuty(LV_PWM_TIMER_NUM, 300 * 4 * 10, 300 * 4 * 10); //占空比10%
                voltage_timer_open_off(LV_PWM_TIMER_NUM , ENABLE);
                
                VOLTAGE_DELAY_US(3000);
                
                voltage_timer_open_off(LV_PWM_TIMER_NUM , DISABLE);
            }
            
            
            bool status = timer_change_set_cycles((void *)&now_voltage.lv_loop_cycles, ARRAY_LENGTH(now_voltage.lv_loop_cycles), &lv_tick, &lv_count_tick, now_voltage.hv_pulse_count);

            if (status == true)
            {
                voltage_timer_change_peroid(VOLTAGE_PWM_TIMER_NUM, (now_voltage.hv_loop_cycles[0].cycles_time - now_voltage.hv_pulse_count) * VOLTAGE_TIMER_MS_COUNT);
                voltage_timer_open_off(LV_PWM_TIMER_NUM , DISABLE);
                hv_lv_switching = HV_MODE;
                LV_POWER_OFF();
            }    
        }
    }
    else if (now_voltage.hv_lv_mode == LV_MODE)
    {
        oc1_oc4_open();
        uint32_t lv_s = voltage_lock();//进入临界
        
        open_voltage_24v();
        
        if (now_voltage.lv_loop_cycles[lv_tick].cycles_time != 0)
        {
            if (lv_tick == 0)
            {
                //SET_VOLTAGE_Pin(DC24V_OR_DC14V_POWER_Pin, 1);
                TIM_ChangePeriod(LV_PWM_TIMER_NUM, 2000 * 4 * 10);
				TIM_PWMChangeFreqAndDuty(LV_PWM_TIMER_NUM, 1000 * 4 * 10, 1000 * 4 * 10); //占空比10%
                VOLTAGE_DELAY_US(10);
            }
            else
            {
                //SET_VOLTAGE_Pin(DC24V_OR_DC14V_POWER_Pin, 0);
                TIM_ChangePeriod(LV_PWM_TIMER_NUM, 600 * 4 * 10);
				TIM_PWMChangeFreqAndDuty(LV_PWM_TIMER_NUM, 300 * 4 * 10, 300 * 4 * 10); //占空比10%
                VOLTAGE_DELAY_US(10);     
            }
            voltage_timer_open_off(LV_PWM_TIMER_NUM , ENABLE); 
        }

        voltage_unlock(lv_s);

        timer_change_set_cycles((void *)&now_voltage.lv_loop_cycles, ARRAY_LENGTH(now_voltage.lv_loop_cycles), &lv_tick, &lv_count_tick, now_voltage.hv_pulse_count);
    }
}



/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/
