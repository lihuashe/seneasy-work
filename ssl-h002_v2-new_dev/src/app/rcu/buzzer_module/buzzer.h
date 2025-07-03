#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <stdint.h>
#include <stdbool.h>
#include <app_section.h>
#include "swtimer.h"
#include "app_task.h"

#define BUZZER_REPEAT_RUN_TIME  10 /* 循环开启蜂鸣器运行时长10ms */

#define BUZZER_TIMER_NUM		TIM4
#define PWM_TIMER_NUM           BUZZER_TIMER_NUM
#define PWM_OUT_PIN_PINMUX      TIM_PWM4
#define PWM_OUT_PIN             P3_3

typedef enum{
	PLAY_NONE = 1,
	PLAY_BEGIN,
	PLAYING,
}ENUM_PLAY_SSTATUS;

typedef enum{
	MUSIC_KEY_CLICKED = 1,
	MUSIC_POWER_ON,
	MUSIC_LOW_POWER,
	MUSIC_WORK_TIME,
	MUSIC_PAIR,
}ENUM_MUSIC_MAP;

typedef struct{
	uint32_t tone;  //音调计数值 = 1000000/音调频率
	uint32_t period; //音调持续时间
}Tone;

typedef struct{
	uint8_t tone_num;
	Tone tone_map[6];
}MusicBuzzer;




extern MusicBuzzer *music;
extern const MusicBuzzer tone_key_clicked;
extern const MusicBuzzer tone_power_on;
extern const MusicBuzzer tone_low_power;
extern const MusicBuzzer tone_work_time;
extern const MusicBuzzer tone_off;
extern MusicBuzzer tone_xhz;

void pwm_init_driver(void);
void play_one_music(const MusicBuzzer *one_music);
void play_music_map(uint8_t music_map);
void buzzer_open(void);
void buzzer_stop(void);
void buzzer_timer_cb(TimerHandle_t pxTimer) DATA_RAM_FUNCTION;
void buzzer_repeat_timer_cb(TimerHandle_t pxTimer) DATA_RAM_FUNCTION;

#endif  /* _BUZZER_H_ */
