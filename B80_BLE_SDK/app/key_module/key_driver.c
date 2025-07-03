/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_driver.c
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
#include "key_driver.h"
#include "../application/keyboard/keyboard.h"
#include "stack/ble/ble.h"
#include "../led_module/led_driver.h"
#include "../keyboard_rcu.h"
#include "../app.h"
#include "key_handle.h"
#include "../sensor_module/da217_driver.h"
#include "../factory_module/factory_profile.h"
#include "../touch_module/ft3308_driver.h"

#if (UI_KEYBOARD_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
int key_not_released;
static u32 keyScanTick = 0;
extern u32 scan_pin_need;
u8 key_power_flag = 0;
_attribute_data_retention_ u32 keyScanLongPressTick = 0;
_attribute_data_retention_ u32 LongPressTick = 0;
_attribute_data_retention_ APP_Accessibility_Shortcut_Mode  Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_NONE;
_attribute_data_retention_ u8 Accessibility_Shortcut_Flag = 0;

#if(GHOST_KEY_FILTER_ENABLE)
u8 front_dec_num = 0;
u8 back_dec_num = 0;
#endif
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  keyboard gpio wakeup init
 * @param  none
 * @return none
 * @retval void
 */
void key_wakeup_init(void)
{
	u32 pin[] = KB_DRIVE_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
	{
		cpu_set_gpio_wakeup (pin[i], Level_High,1);  //drive pin pad high wakeup deepsleep
	}
	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);
}

/******************************************************************
 * @brief  this function is used to process keyboard matrix status change.
 * @param  none
 * @return none
 * @retval void
 */
void key_change_proc(void)
{
	#if (PM_DEEPSLEEP_ENABLE)
		extern u32 latest_user_event_tick;
		latest_user_event_tick = clock_time();  //record latest key change time
	#endif

	if((rcu_wakeup_touch == 0x00) || (rcu_wakeup_touch == 0x01))
	{
		rcu_wakeup_touch = 0x02;
		FT3308_chip_rsin();
	}
	key_not_released = 1;
	keyScanLongPressTick = 0;
    LongPressTick = 0;

	if(kb_event.cnt != 0)
	{
		if(app_led_2_blink_timer == 0)
		{
			if(app_rcu_batt_mv <= VBAT_VOL_LOW)
			{
				app_led_2_blink_time_ms = 250;
				app_led_2_blink_timer = clock_time() | 1;
			}			
		}
	}

	if (kb_event.cnt == 3){
		key_handle_pressed_3_event();
	}
	else if (kb_event.cnt == 2){
		key_handle_pressed_2_event();
	}
	else if(kb_event.cnt == 1){
		key_handle_pressed_1_event();
	}
	else   //kb_event.cnt == 0,  key release
	{
		key_handle_release();
	}
}

#if(GHOST_KEY_FILTER_ENABLE)
/******************************************************************
 * @brief  This function is used to handle stuck key filter events.
 * @param  none
 * @return none
 * @retval void
 */
void stuck_key_filt_event(void)
{
	static bool key_driver_front_init_flag = false;
	static bool key_driver_back_init_flag = false;
	if(Face_up_flag == true && key_driver_front_init_flag == false)
	{
		key_driver_back_init_flag = false;
		key_driver_front_init_flag = true;
		back_dec_num = KEY_BACK_FILTER_NUM;
		front_dec_num = KEY_FILTER_NUM_NULL;
		// printf("--->>> back view\r\n");
	}
	else if (Face_up_flag == false && key_driver_back_init_flag == false)
	{
		key_driver_front_init_flag = false;
		key_driver_back_init_flag = true;
		back_dec_num = KEY_FILTER_NUM_NULL;
		front_dec_num = KEY_FRONT_FILTER_NUM;
		// printf("--->>> front view\r\n");
	}
}
#endif

/******************************************************************
 * @brief  this function is used to detect if key pressed or released.
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return none
 * @retval void
 */
void proc_keyboard (u8 e, u8 *p, int n)
{
	if(clock_time_exceed(keyScanTick, 8000)){
		keyScanTick = clock_time();
	}
	else{
		return;
	}

#if(GHOST_KEY_FILTER_ENABLE)
#if(FACTORY_ENABLE)
	if(factory_mode_flag == 0)
	stuck_key_filt_event();
	else
	{
		back_dec_num = 0;
		front_dec_num = 0;
	}
#else
	stuck_key_filt_event();
#endif
#endif

	kb_event.keycode[0] = 0;
	int det_key = kb_scan_key (0, 1);
			
	if (det_key){
		key_change_proc();
	}

    if((keyScanLongPressTick == 0) && (Accessibility_Shortcut_Mode != APP_ACCESSIBILITY_SHORTCUT_NONE))
    {
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_NONE;
    }

    if(keyScanLongPressTick && clock_time_exceed(keyScanLongPressTick, LongPressTick))
    {
        keyScanLongPressTick = 0;
        key_handle_LongPress(Accessibility_Shortcut_Mode);
    }
}


#define KB_COL_NUM   1
const GPIO_PinTypeDef gpio_column[KB_COL_NUM]={GPIO_PA5};
/**
 * @brief       init col gpio for keyboard
 * @param[in]   col_pin	- 
 * @param[in]   col_pull- 
 * @return      none
 * @note        
 */
void keyboard_col_gpio_init(GPIO_PinTypeDef col_pin,GPIO_PullTypeDef col_pull)
{
	gpio_set_func(col_pin,AS_GPIO);			//IO设为普通IO
	gpio_set_output_en(col_pin,0);	 		//输出使能关掉
	gpio_set_input_en(col_pin,1);			//使能输入
	gpio_setup_up_down_resistor(col_pin,col_pull);//设置上拉1M电阻
	gpio_write(col_pin,0);					//输出设为0
	cpu_set_gpio_wakeup(col_pin, Level_Low, 1);
}

/**
 * @brief       init all col gpio for keyboard
 * @param[in]   ks_col	- 
 * @param[in]   col_cnt	- 
 * @param[in]   col_pull- 
 * @return      none
 * @note        
 */
void keyboard_set_col(const GPIO_PinTypeDef* ks_col,unsigned char col_cnt, GPIO_PullTypeDef col_pull)
{
	unsigned char i_cnt = 0;
	for(i_cnt = 0;i_cnt < col_cnt;i_cnt++){
		keyboard_col_gpio_init(ks_col[i_cnt],col_pull);
	}
}

/**
 * @brief       init keyboard gpio
 * @param[in]   void- 
 * @return      none
 * @note        
 */
void keyscan_gpio_init(void)
{
	keyboard_set_col(gpio_column,KB_COL_NUM,PM_PIN_PULLUP_1M);
}

/**
 * @brief       scan the keyboard and get the key value
 * @param[in]   void- 
 * @return      
 * @note        
 */
void keyscan_scan_func(void)
{
	unsigned char col_j;
	for(col_j=0;col_j<KB_COL_NUM;col_j++){
		if(gpio_read(gpio_column[col_j])==0){//有按键按下
			if(key_power_flag == 0){
				key_power_flag = 1;
				key_handle_pressed_power();
			}
		}
		else
		{
			if(key_power_flag == 1){
				key_power_flag = 0;
				key_handle_release_power();
			}
		}
	}
}

#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
