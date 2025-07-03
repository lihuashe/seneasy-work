#include "buzzer.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include <app_section.h>
#include "swtimer.h"
#include "os_timer.h"
#include "trace.h"





extern TimerHandle_t buzzer_repeat_timer;

MusicBuzzer *music = 0;
static uint32_t music_ticks;
static uint8_t tone_index;
static ENUM_PLAY_SSTATUS status = PLAY_BEGIN;
const MusicBuzzer tone_key_clicked = {2, {{440, 80}, {587, 80}, {1912, 60}, {784, 60}, {3030, 60}, {3080, 60}}};
// const MusicBuzzer tone_power_on = {3, {{1516, 100}, {1702, 100}, {1912, 100}, {784, 60}, {3030, 60}, {3080, 60}}};

const MusicBuzzer tone_power_on = {
    6, // 音符数量
    {
        { 523, 100 }, // C5 (523 Hz)
        { 587, 100 }, // D5 (587 Hz)
        { 659, 80 },  // E5 (659 Hz)
        { 784, 60 },  // G5 (784 Hz)
        { 880, 60 },  // A5 (880 Hz)
        { 988, 60 }   // B5 (988 Hz)
    }
};

const MusicBuzzer tone_low_power = {1, {{1274, 100}, {100000, 20}, {1912, 60}, {100000, 20}, {3030, 60}, {100000, 20}}};
const MusicBuzzer tone_work_time = {3, {{1274, 100}, {954, 100}, {1912, 100}}};
const MusicBuzzer tone_pair_time = {5, {{1016, 100}, {1252, 60}, {1512, 100}, {1984, 60}, {2230, 100}, {3080, 60}}};
const MusicBuzzer tone_off = {1, {{100000, 0}}};
MusicBuzzer tone_xhz = {1, {{2000, 1000}}};


/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
*/
void board_pwm_init(void)
{
    Pad_Config(PWM_OUT_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    /* Normal mode */
    Pinmux_Config(PWM_OUT_PIN, PWM_OUT_PIN_PINMUX);
}

/**
  * @brief  Initialize tim peripheral.
  * @param  No parameter.
  * @return void
*/
void driver_pwm_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_InitStruct;

    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_PWM_En   = PWM_ENABLE;
    TIM_InitStruct.TIM_Mode     = TIM_Mode_UserDefine;
    TIM_InitStruct.TIM_Period   = 800 * 10 * 4;         //周期：370 us  频率2.7KHZ
    TIM_InitStruct.TIM_PWM_High_Count   = 400 * 10 * 4; //高电平185us
    TIM_InitStruct.TIM_PWM_Low_Count    = 400 * 10 * 4; //低电平185us
    TIM_TimeBaseInit(BUZZER_TIMER_NUM, &TIM_InitStruct);

    TIM_Cmd(BUZZER_TIMER_NUM, DISABLE);
}

/**
 * @brief 播放指定的一首音乐
 * @param one_music 
 */
void play_one_music(const MusicBuzzer *one_music)
{
    tone_xhz = (MusicBuzzer)*one_music;
    music = &tone_xhz;
    buzzer_open();
}




/**
 * @brief 播放乐曲
 * @param music_map 
 */
void play_music_map(uint8_t music_map)
{
    switch (music_map)
    {
        case MUSIC_KEY_CLICKED:
            music = (MusicBuzzer *)&tone_key_clicked;
            break;

        case MUSIC_POWER_ON:
            music = (MusicBuzzer *)&tone_power_on;
            break;

        case MUSIC_LOW_POWER:
            music = (MusicBuzzer *)&tone_low_power;
            break;

        case MUSIC_WORK_TIME:
            music = (MusicBuzzer *)&tone_work_time;
            break;

        case MUSIC_PAIR:
            music = (MusicBuzzer *)&tone_pair_time;
            break;

        default:
            break;
    }

    buzzer_open();
}


/**
 * @brief 停止蜂鸣器音乐
 * @param  
 */
void buzzer_stop(void)
{
    os_timer_stop(&buzzer_repeat_timer);
    TIM_Cmd(BUZZER_TIMER_NUM, DISABLE);
}

/**
 * @brief 开启蜂鸣器
 * @param  
 */
void buzzer_open(void)
{
    os_timer_restart(&buzzer_repeat_timer, BUZZER_REPEAT_RUN_TIME);
    TIM_Cmd(BUZZER_TIMER_NUM, ENABLE);
}

void buzzer_repeat_timer_cb(TimerHandle_t pxTimer) DATA_RAM_FUNCTION;  //重复开启蜂鸣器回调
void buzzer_repeat_timer_cb(TimerHandle_t pxTimer)
{
    if(music){
		switch (status)
		{
			case PLAY_BEGIN:
                //PRINT("PLAY_BEGIN\n");
				music_ticks = 0; 
				tone_index = 0;
                TIM_ChangePeriod(BUZZER_TIMER_NUM,(40 * 1000000 / (uint32_t)(music->tone_map[tone_index].tone)));
				TIM_PWMChangeFreqAndDuty(BUZZER_TIMER_NUM, (40 * 1000000 / (uint32_t)(music->tone_map[tone_index].tone))/10, (40 * 1000000 / (uint32_t)(music->tone_map[tone_index].tone))/10 * 9); //占空比10%
				status = PLAYING;
				break;
			case PLAYING:
                //PRINT("PLAYING\n");
				music_ticks += BUZZER_REPEAT_RUN_TIME;
				if(music_ticks >= music->tone_map[tone_index].period) //播完一音调
				{
					music_ticks = 0;
					tone_index++;
					
					if(tone_index >= music->tone_num)
                    { //全部音调播完
						
						buzzer_stop();
						status = PLAY_BEGIN;
						music = 0;
						tone_index = 0;
					}else
                    {	
						TIM_ChangePeriod(BUZZER_TIMER_NUM,(40 * 1000000 / (uint32_t)(music->tone_map[tone_index].tone)));
				        TIM_PWMChangeFreqAndDuty(BUZZER_TIMER_NUM, (40 * 1000000 / (uint32_t)(music->tone_map[tone_index].tone))/10, (40 * 1000000 / (uint32_t)(music->tone_map[tone_index].tone))/10 * 9); //占空比10%
					}
				}
				break;
			default:
				break;
		}		
	}
}

void pwm_init_driver(void) //初始化PWM
{
    board_pwm_init();
    driver_pwm_init();
}

void buzzer_enter_dlps_config(void) //休眠时失能引脚
{
    Pad_Config(PWM_OUT_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
}
