#ifndef __KEY_SCAN_H
#define __KEY_SCAN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "board_config.h"
#include "peripheral.h"

typedef enum
{
    PMU_PULL_UP_RES_4K = 0,
    PMU_PULL_UP_RES_10K,
    PMU_PULL_UP_RES_300K,
    PMU_PUL_UPL_RES_2M,
}pmu_pullup_resistor_t;





typedef struct 
{
    uint8_t num;
    uint8_t val[KEY_COL_NUM];
} KEY_SCAN_DATA_T;

typedef struct{
	unsigned char cnt;
	unsigned char ctrl_key;
	unsigned char keycode[KEY_COL_NUM];
	//unsigned char padding[2];	//  for  32 bit padding,  if KB_RETURN_KEY_MAX change,  this should be changed
}KB_DATA_T;


/// Event
typedef enum
{
    KEY_SCAN_EVENT_PRESSED,
    KEY_SCAN_EVENT_RELEASED,
} KEY_SCAN_EVENT_T;


/**
 * @brief event callback
 *
 * @param[in] event  Event type
 * @param[in] data  @ref kpp_sim_data_t
 *
 * <pre>
 * data[0] bit-0 is col-0,row-0
 * data[0] bit-1 is col-0,row-1
 * data[0] bit-2 is col-0,row-2
 * ...
 * data[0] bit-0 is col-0,row-0
 * data[1] bit-0 is col-1,row-0
 * data[2] bit-1 is col-2,row-1
 * ...
 * </pre>
 *
 * @return None
 **/
typedef void (*key_scan_event_callback_t)(KEY_SCAN_EVENT_T event, const KB_DATA_T data);

/// Keyboad config struct
typedef struct
{
    uint32_t row_pin_mask;              // row pin bitmask
    uint32_t col_pin_mask;              // col pin bitmask
    key_scan_event_callback_t callback; // key scan event callback
} KEY_SCAN_CONFIG_T;

typedef struct
{
    KEY_SCAN_CONFIG_T config;	/**按键配置*/
    bool is_pressed;			/**按键gpio按下状态*/
    bool is_report_pressed;		/**按键上报按下状态*/
    uint8_t release_count;		/**检测到几次松手才算真正的松手释放,控制次数查看 KEY_SCAN_RELEASE_COUNT*/
    KEY_SCAN_DATA_T data;		/**按键数据*/
	KB_DATA_T kb_event;			/**按键映射*/
    co_timer_t scan_timer;		/**按键扫描计时器*/
    co_timer_t debounce_timer;	/**按键消抖计时器*/
}KEY_SCAN_ENV_T;



typedef struct {
    unsigned char  page;    //HID usage page
    unsigned short hid_val; //HID key value
    unsigned char  ir_val;  //IR key value
} key_val_map_t;

typedef struct {
    unsigned char state;    //key press or release state
    unsigned char row;      //the row index of key pressed
    unsigned char col;      //the column index of key pressed
    key_val_map_t val_map;
} app_key_t;

void app_key_init(key_scan_event_callback_t keyscan_event_cb);
void key_scan(app_key_t *key);


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief kpp gpio IRQ handler, PUT THIS TO GPIO IRQ HANDLER
 *
 * @param[in] pin_mask  pin mask
 *
 * @return None
 **/
void app_key_scan_gpio_handler(uint32_t pin_mask);
void set_stuck_key_scan_timer(void);

__STATIC_INLINE void pmu_pull_up_resistor_set(uint32_t pin_mask, pmu_pullup_resistor_t pu_res);


#ifdef __cplusplus
}
#endif

#endif //__KEY_SCAN_H

