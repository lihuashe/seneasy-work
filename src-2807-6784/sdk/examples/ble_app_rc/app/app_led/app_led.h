/**
 ***************************************************************************************************
 *
 * @file app_led.h
 *
 * @brief LED on, start, blink
 *
 * Copyright (C) Eker 2020
 *
 *
 ***************************************************************************************************
 */
#ifndef __APP_LED_H__
#define __APP_LED_H__

#include "board_config.h"

#define	 LED_SINGLE_STATE	 0xFFFFFFFF

#define  LED_PERIOD   		10  /*50ms*/
#define  LED_DEBUG     		0




typedef struct{
	unsigned int onTime_ms;
	unsigned int offTime_ms;
	unsigned int  repeatCount;   //0xffffffff special for long on(offTime_ms=0)/long off(onTime_ms=0)
	unsigned char  priority;     //0x00 < 0x01 < 0x02 < 0x04 < 0x08 < 0x10 < 0x20 < 0x40 < 0x80
    unsigned char cfg_index;
	unsigned char led_index;
} led_cfg_t;

typedef struct {
	unsigned char  isOn;
	unsigned char  polar;
	unsigned int  repeatCount;
	unsigned char  priority;


	unsigned int onTime_ms;
	unsigned int offTime_ms;

	unsigned char led_type;
	unsigned int startTick;
	unsigned int cur_timer;

	//reborn add
	unsigned char along_on; //3¤áá
    unsigned char cfg_index;
	unsigned char led_port;
	unsigned int on_leval;
}device_led_t;



extern const led_cfg_t led_cfg[];






/*
 * GLOBAL FUNCTIONS
 ***************************************************************************************************
 */
int device_led_setup(led_cfg_t led_cfg);
void led_module_init(void);
void device_led_on_off(unsigned char on, unsigned char led_index);
void led_module_reset(void);
void led_module_deinit(void);
unsigned char device_all_led_busy(void);
void reset_designate_led_index(unsigned char led_index);
void device_LED_DC(unsigned char on);
#endif /* __APP_LED_H__ */
