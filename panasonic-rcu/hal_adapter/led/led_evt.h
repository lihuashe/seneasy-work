/**
 * @file led_evt.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APP_LED_MODULE_LED_EVT_H_
#define APP_LED_MODULE_LED_EVT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "led_driver.h"

/*============================================================================*
 *                              LED Event Config
 *============================================================================*/
#define LED_EVT_MAX_NUM 10
#define LED_EVT_GRADIENT_EN 1



#define LED_EVT_FOREVER 0xFF
#define LED_EVT_ITEM(onTime_ms, offTime_ms, repeatCount, priority)  {onTime_ms, offTime_ms, repeatCount, priority}

/**
 * @brief	Configure the parameters for led event
 */
typedef struct{
	unsigned short onTime_ms;
	unsigned short offTime_ms;

	unsigned char  repeatCount;  //"LED_EVT_FOREVER" special 
	unsigned char  priority;     //0x00 < 0x01 < 0x02 < 0x04 < 0x08 < 0x10 < 0x20 < 0x40 < 0x80
} LedConfig;


typedef struct{
	unsigned short onTime_ms;
	unsigned short offTime_ms;

	unsigned char  id;  
	unsigned char  repeatCount;  // "LED_EVT_FOREVER" special 
	unsigned char  priority;     //0x00 < 0x01 < 0x02 < 0x04 < 0x08 < 0x10 < 0x20 < 0x40 < 0x80
	unsigned int startTick;
} LedEvent;

/**
 * @brief	the status of led event
 */
typedef struct{
	LedEvent arr[LED_EVT_MAX_NUM];
	unsigned char len;

} LedEventManager;


/**
 * @brief		This function is used to manage led tasks
 * @param[in]	none
 * @return      none
 */
extern void led_evt_proc(void);

int led_evt_setup(unsigned char id, LedConfig led_cfg);
int led_evt_setup_gradient(unsigned char id, LedConfig led_cfg);
int led_evt_reset(unsigned char id);
int led_evt_is_busy();

#ifdef __cplusplus
}
#endif


#endif 
