#ifndef _APP_FUNCTION_H_
#define _APP_FUNCTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <trace.h>
#include <key_handle.h>
#include "buzzer.h"
#include "lp4081h_driver.h"
#include "mem_types.h"
#include "usb_chg.h"      

#define  POWER_AUTO_OFF_TIME 5 * 60 * 1000 /* 10min自动关机 */


typedef enum
{
    SCALP_MODE    = 0x00,
    FACE_MODE,
    FACE_MESO_MODE,
    SCALP_MESO_MODE
    
}ENUM_BEAUTY_COMB_MODE_DEF;

typedef struct
{
    uint8_t  mode;
    uint8_t  gear;
    bool     reverse_gear_flag;
}Beauty_Comb_Mode; 

extern Beauty_Comb_Mode g_beauty_comb;
extern bool power_is_on;
extern uint8_t paired_flag;


extern void power_on_off_handle(uint8_t key_value) DATA_RAM_FUNCTION;

extern void power_and_gear_pair_ble_handle(uint8_t key_value) DATA_RAM_FUNCTION;

extern void beauty_comb_mode_switch_handle(uint8_t key_value)  DATA_RAM_FUNCTION;

extern void beauty_comb_meso_mode_on_off_handle(uint8_t key_value) DATA_RAM_FUNCTION;

extern void beauty_comb_mode_gear_add_sub_handle(uint8_t key_value) DATA_RAM_FUNCTION;

void power_auto_off_timer_cb(TimerHandle_t pxTimer) DATA_RAM_FUNCTION;


#ifdef __cplusplus
}
#endif

#endif
