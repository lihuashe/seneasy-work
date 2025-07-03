
#include "key_scan.h"

#define APP_LOG_DOMAIN      "kscan"
#define APP_LOG_LEVEL       APP_KEY_LOG_LEVEL
#include "co_debug.h"

/// keyboard pin max num
#define KEY_SCAN_PIN_NUM_MAX                        31

#define KEY_SCAN_RELEASE_COUNT            			1

#define KEY_SCAN_COL_OUTPUT_KEEP_TIME           	100  //unit: us °′?üé¨?ü?óê±

#define KEY_SCAN_DEBOUNCE_TIME					  5
#define KEY_SCAN_INTERVAL                		  20//20 30  50   //Keys scan interval, unit:ms

 
static KEY_SCAN_ENV_T s_key_scan_env;
static const unsigned char kb_map_normal[KEY_ROW_NUM][KEY_COL_NUM] = KB_MAP_NORMAL;


/**
 * @brief key_scan_is_ghost_key()
 *
 * @param[in] data  
 *
 * @return 
 **/
static bool is_ghost_key(const KEY_SCAN_DATA_T data)
{
    int i, j;
    uint8_t ghost;

	//在一个矩阵内的三个按键按下造成的鬼键处理
    for(i=0; i<KEY_ROW_NUM; ++i)//sizeof(data.val)
    {
        if(data.val[i] == 0)
            continue;

        for(j=i+1; j<KEY_ROW_NUM; ++j)//sizeof(data.val)
        {
            if(data.val[j] == 0)
                continue;

            ghost = data.val[i] & data.val[j];

            if(ghost && (ghost & (ghost - 1)))
            {
				APP_LOG_ERR("ghost key press\r\n");
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief key_pressed()
 *
 * @return 
 **/
static void key_pressed(const KEY_SCAN_DATA_T data, KB_DATA_T kb_data)
{
    if(memcmp(s_key_scan_env.data.val, data.val, KEY_COL_NUM))
    {
        s_key_scan_env.is_report_pressed = true;

		memcpy(s_key_scan_env.data.val, data.val, KEY_COL_NUM);
		memcpy(&s_key_scan_env.kb_event, &kb_data, sizeof(KB_DATA_T));

        if(s_key_scan_env.config.callback && !is_ghost_key(data))
		{
            s_key_scan_env.config.callback(KEY_SCAN_EVENT_PRESSED, s_key_scan_env.kb_event);
		}
    }
}

/**
 * @brief key_released()
 *
 * @return 
 **/
static void key_released(void)
{
    if(s_key_scan_env.is_report_pressed)
    {
        s_key_scan_env.is_report_pressed = false;

        memset(&s_key_scan_env.data, 0, sizeof(KEY_SCAN_DATA_T));
		memset(&s_key_scan_env.kb_event, 0, sizeof(KB_DATA_T));

        if(s_key_scan_env.config.callback)
            s_key_scan_env.config.callback(KEY_SCAN_EVENT_RELEASED, s_key_scan_env.kb_event);
    }
}

/**
 * @brief key_scan_row()
 *
 * @return 
 **/
static void key_scan_row(void)
{
    uint32_t col, col_mask, col_index;
    uint32_t row, row_mask, row_index, row_value;
    bool pressed = false;
    KEY_SCAN_DATA_T data = {0};
	KB_DATA_T	kb_data = {0};
	uint32_t row_pin_mask = s_key_scan_env.config.row_pin_mask;
    uint32_t col_pin_mask = s_key_scan_env.config.col_pin_mask;

    // Disable IRQ
    gpio_set_interrupt(row_pin_mask, GPIO_TRIGGER_DISABLE);

    // All colume to HIGH
	pmu_pin_mode_set(col_pin_mask, PMU_PIN_MODE_PU);
    gpio_write(col_pin_mask, GPIO_HIGH);
    co_delay_us(KEY_SCAN_COL_OUTPUT_KEEP_TIME);
    pmu_pin_mode_set(col_pin_mask, PMU_PIN_MODE_OD);

    // Scan
    col_index = 0;
	
		//gnd scan
	#if (KEY_SCAN_GND_EN)
		unsigned char scan_gnd_index = KEY_COL_NUM-1;
		// Read value
		row_value = (~gpio_read(row_pin_mask)) & row_pin_mask;

		// Check ROW
		row_index = 0;
		for(row=0; row<KEY_SCAN_PIN_NUM_MAX; ++row)
		{
			row_mask = 1u << row;
			if(row_pin_mask & row_mask)
			{
				if(row_value & row_mask)
				{
					data.num++;
					data.val[scan_gnd_index] |= 1u << row_index;	 
					kb_data.keycode[kb_data.cnt] = kb_map_normal[row_index][scan_gnd_index];
	//						APP_LOG_INF("\n kb_data.keycode[kb_data.cnt]=%d",kb_data.keycode[kb_data.cnt]);						
					kb_data.cnt++;

				}

				++row_index;
				if(row_index >= KEY_ROW_NUM)
					break;
			}
		}

		if(data.val[scan_gnd_index])
		{
			pressed = true;
		}
	#endif
	
    for(col=0; col<KEY_SCAN_PIN_NUM_MAX; ++col)
    {
        col_mask = 1u << col;
        if(col_pin_mask & col_mask)
        {
            // Only current colume output LOW
            gpio_write(col_mask, GPIO_LOW);
            co_delay_us(KEY_SCAN_COL_OUTPUT_KEEP_TIME);

            // Read value
            row_value = (~gpio_read(row_pin_mask)) & row_pin_mask;

            // Current colume to HIGH
            pmu_pin_mode_set(col_pin_mask, PMU_PIN_MODE_PU);
            gpio_write(col_pin_mask, GPIO_HIGH);

            // Check ROW
            row_index = 0;
            for(row=0; row<KEY_SCAN_PIN_NUM_MAX; ++row)
            {
                row_mask = 1u << row;
                if(row_pin_mask & row_mask)
                {
					#if (KEY_SCAN_GND_EN)
					if (data.val[scan_gnd_index] & (1u << row_index))
					{
						++row_index;
						if(row_index >= KEY_ROW_NUM)
						{
							break;
						}
						continue;
					}
					#endif
				
                    if(row_value & row_mask)
                    {
                        data.num++;
                        data.val[col_index] |= 1u << row_index;	 
						kb_data.keycode[kb_data.cnt] = kb_map_normal[row_index][col_index];
//						APP_LOG_INF("\n kb_data.keycode[kb_data.cnt]=%d",kb_data.keycode[kb_data.cnt]);						
						kb_data.cnt++;
	
                    }

                    ++row_index;
                    if(row_index >= KEY_ROW_NUM)
                        break;
                }
            }

            if(data.val[col_index])
			{
                pressed = true;
			}
			
            // restore OD
            pmu_pin_mode_set(col_pin_mask, PMU_PIN_MODE_OD);

            ++col_index;
			#if (KEY_SCAN_GND_EN)
			if(col_index >= KEY_COL_NUM-1)
			#else
            if(col_index >= KEY_COL_NUM)
			#endif
			{
                break;
			}
        }
    }
	

	
    // All colume to LOW
    gpio_write(col_pin_mask, GPIO_LOW);
    co_delay_us(KEY_SCAN_COL_OUTPUT_KEEP_TIME+50);

    // Enable IRQ
    gpio_set_interrupt(row_pin_mask, GPIO_BOTH_EDGE);

    if(pressed)
	{
        key_pressed(data,kb_data);
	}
}

/**
 * @brief kpp_scan_timer_handler()
 *
 * @param[in] id  
 * @param[in] param  
 *
 * @return 
 **/
static void key_scan_timer_handler(co_timer_t *timer, void *param)
{
    uint32_t cur_row_level_mask = gpio_read(s_key_scan_env.config.row_pin_mask);

    if(cur_row_level_mask == s_key_scan_env.config.row_pin_mask)
        ++s_key_scan_env.release_count;
    else
        s_key_scan_env.release_count = 0;

    if(s_key_scan_env.release_count < KEY_SCAN_RELEASE_COUNT)
    {
        key_scan_row();
    }
    else
    {
        CO_DISABLE_IRQ();
        s_key_scan_env.is_pressed = false;
        CO_RESTORE_IRQ();

        s_key_scan_env.release_count = 0;

            pmu_lowpower_allow(PMU_LP_KPP);
        co_timer_del(&s_key_scan_env.scan_timer);
//		APP_LOG_INF("del scan_timer\n");

        pmu_wakeup_pin_set(s_key_scan_env.config.row_pin_mask, PMU_PIN_WAKEUP_LOW_LEVEL);

        key_released();
    }
}

/**
 * @brief kpp_sim_debounce_timer_handler()
 *
 * @param[in] id  
 * @param[in] param  
 *
 * @return 
 **/
static void key_scan_debounce_timer_handler(co_timer_t *timer, void *param)
{
    uint32_t cur_row_level_mask = gpio_read(s_key_scan_env.config.row_pin_mask);

    if(cur_row_level_mask != s_key_scan_env.config.row_pin_mask)
    {
        CO_DISABLE_IRQ();
        s_key_scan_env.is_pressed = true;
        CO_RESTORE_IRQ();

        co_timer_set(&s_key_scan_env.scan_timer, KEY_SCAN_INTERVAL,
                TIMER_REPEAT, key_scan_timer_handler, NULL);

        key_scan_row();

        pmu_wakeup_pin_set(s_key_scan_env.config.row_pin_mask, PMU_PIN_WAKEUP_DISABLE);
    }

    pmu_lowpower_allow(PMU_LP_KPP);
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief encoder gpio IRQ handler, PUT THIS TO GPIO IRQ HANDLER
 *
 * @param[in] pin_mask  pin mask
 *
 * @return None
 **/
void app_key_scan_gpio_handler(uint32_t pin_mask)
{
    // depress
//	APP_LOG_INF("app_key_scan_gpio_handler\n");
//	APP_LOG_INF("s_key_scan_env.is_pressed = %d，g_keyboard_ctl.stuck_flag = %d\n",s_key_scan_env.is_pressed,g_keyboard_ctl.stuck_flag);
    if(!s_key_scan_env.is_pressed)
    {
        uint32_t cur_row_level_mask = gpio_read(s_key_scan_env.config.row_pin_mask);

        if(cur_row_level_mask != s_key_scan_env.config.row_pin_mask)
        {
            co_timer_set(&s_key_scan_env.debounce_timer, KEY_SCAN_DEBOUNCE_TIME,
                    TIMER_ONE_SHOT, key_scan_debounce_timer_handler, NULL);

            pmu_lowpower_prevent(PMU_LP_KPP);
        }
    }
}


static void key_wakeup_handler(uint32_t pin_mask)
{
	pmu_wakeup_pin_set(s_key_scan_env.config.row_pin_mask, PMU_PIN_WAKEUP_LOW_LEVEL);
	pmu_pull_up_resistor_set(s_key_scan_env.config.row_pin_mask, PMU_PULL_UP_RES_300K);
	app_key_scan_gpio_handler(pin_mask);
}


/**
* @brief Mux configuration of pins, which used to keyboard.
**/
static void key_pinmux_init(void)
{
    // column
#if (PIN_KEY_COL_0<32)
    pinmux_config(PIN_KEY_COL_0,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_COL_1<32)
    pinmux_config(PIN_KEY_COL_1,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_COL_2<32)
    pinmux_config(PIN_KEY_COL_2,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_COL_3<32)
    pinmux_config(PIN_KEY_COL_3,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_COL_4<32)
    pinmux_config(PIN_KEY_COL_4,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_COL_5<32)
    pinmux_config(PIN_KEY_COL_5,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_COL_6<32)
    pinmux_config(PIN_KEY_COL_6,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_COL_7<32)
    pinmux_config(PIN_KEY_COL_7,  PINMUX_GPIO_MODE_CFG);
#endif

    // row
#if (PIN_KEY_ROW_0<32)
    pinmux_config(PIN_KEY_ROW_0,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_ROW_1<32)
    pinmux_config(PIN_KEY_ROW_1,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_ROW_2<32)
    pinmux_config(PIN_KEY_ROW_2,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_ROW_3<32)
    pinmux_config(PIN_KEY_ROW_3,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_ROW_4<32)
    pinmux_config(PIN_KEY_ROW_4,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_ROW_5<32)
    pinmux_config(PIN_KEY_ROW_5,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_ROW_6<32)
    pinmux_config(PIN_KEY_ROW_6,  PINMUX_GPIO_MODE_CFG);
#endif
#if (PIN_KEY_ROW_7<32)
    pinmux_config(PIN_KEY_ROW_7,  PINMUX_GPIO_MODE_CFG);
#endif

	// Row (pull-up)
    pmu_pin_mode_set(s_key_scan_env.config.row_pin_mask, PMU_PIN_MODE_PU);
    gpio_set_direction(s_key_scan_env.config.row_pin_mask, GPIO_INPUT);
    gpio_set_interrupt(s_key_scan_env.config.row_pin_mask, GPIO_BOTH_EDGE);
	
	pmu_pull_up_resistor_set(s_key_scan_env.config.row_pin_mask,PMU_PULL_UP_RES_300K);

    // Colume (open-drain)
    pmu_pin_mode_set(s_key_scan_env.config.col_pin_mask, PMU_PIN_MODE_OD);
    gpio_set_direction(s_key_scan_env.config.col_pin_mask, GPIO_OUTPUT);
    gpio_write(s_key_scan_env.config.col_pin_mask, GPIO_LOW);
}

/**
* @brief Get the row and column pins mask and quantity.
**/
static void row_col_init(void)
{
	uint8_t pin_key_row[8] = {PIN_KEY_ROW_0, PIN_KEY_ROW_1, PIN_KEY_ROW_2, PIN_KEY_ROW_3, 
                              PIN_KEY_ROW_4, PIN_KEY_ROW_5, PIN_KEY_ROW_6, PIN_KEY_ROW_7};
    uint8_t pin_key_col[8] = {PIN_KEY_COL_0, PIN_KEY_COL_1, PIN_KEY_COL_2, PIN_KEY_COL_3, 
                              PIN_KEY_COL_4, PIN_KEY_COL_5, PIN_KEY_COL_6, PIN_KEY_COL_7};
    
    for(uint8_t i=0; i<8; i++)
    {
        if(pin_key_row[i] < 32)
        {
            s_key_scan_env.config.row_pin_mask |= BIT_MASK(pin_key_row[i]);
        }
        
        if(pin_key_col[i] < 32)
        {
            s_key_scan_env.config.col_pin_mask |= BIT_MASK(pin_key_col[i]);
        }
    }
}

void app_key_init(key_scan_event_callback_t keyscan_event_cb)
{
	memset(&s_key_scan_env,0,sizeof(s_key_scan_env));
	
	s_key_scan_env.config.callback = keyscan_event_cb;
	
	row_col_init();
    key_pinmux_init(); 
	
	// wakeup pin
    pmu_wakeup_pin_set(s_key_scan_env.config.row_pin_mask, PMU_PIN_WAKEUP_LOW_LEVEL);
    gpio_set_interrupt_callback(key_wakeup_handler);
    pmu_wakeup_pin_register_callback(key_wakeup_handler, NULL);
}

void set_stuck_key_scan_timer(void)
{
	co_timer_del(&s_key_scan_env.scan_timer);
	pmu_pull_up_resistor_set(s_key_scan_env.config.row_pin_mask,PMU_PULL_UP_RES_300K);
	co_timer_set(&s_key_scan_env.scan_timer, 500,
		TIMER_REPEAT, key_scan_timer_handler, NULL);
}

__STATIC_INLINE void pmu_pull_up_resistor_set(uint32_t pin_mask, pmu_pullup_resistor_t pu_res)
{
  if(pu_res&0x01)
  {    
    XH_PMU->GPIO_DRV_CTRL_2 |= pin_mask;
  }
  else
  {
    XH_PMU->GPIO_DRV_CTRL_2 &= ~pin_mask;
  }
  
  if(pu_res&0x02)
  {    
    XH_PMU->GPIO_POL_1 |= pin_mask;
  }
  else
  {
    XH_PMU->GPIO_POL_1 &= ~pin_mask;
  }
}






