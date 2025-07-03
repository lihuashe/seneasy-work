/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     factory_profile.c
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
#include "factory_profile.h"
#include "../../stack/ble/ble.h"
#include "../../stack/ble/host/attr/att.h"
#include "../rf_ble_module/app_att.h"
#include "../led_module/led_driver.h"
#include "../app.h"
#include "../keyboard_rcu.h"
#include "../pa_module/gsr2501_driver.h"
#include "../charge_module/lp4081b_driver.h"
#include "../touch_module/ft3308_upgrade.h"

#if(FACTORY_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
/*********************************************************************************************
 *  Factory RCU Serivce     8e8f2421-f12d-4462-a7fa-bd2cfa07eae3
 *  1. CHAR - data read		00000001-f12d-4462-a7fa-bd2cfa07eae3
 * 	2. CHAR - data write    00000002-f12d-4462-a7fa-bd2cfa07eae3
 *  3. CHAR - data notify   00000003-f12d-4462-a7fa-bd2cfa07eae3
 * 	Client Characteristic Configuration
 * ******************************************************************************************/
const u8 uuid_service_factory_rcu[16] = {0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x21,0x24,0x8f,0x8e};
const u8 uuid_char_factory_rcu_rd[16] = {0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x01,0x00,0x00,0x00};
const u8 uuid_char_factory_rcu_wr[16] = {0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x02,0x00,0x00,0x00};
// 1. Factory data read
const u8 char_decl_factory_rcu_rd[19] = {
	CHAR_PROP_READ,
	U16_LO(FACTORY_RCU_RD_DP_H), U16_HI(FACTORY_RCU_RD_DP_H),
	0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x01,0x00,0x00,0x00
};
// 2. Factory data write
const u8 char_decl_factory_rcu_wd[19] = {
	CHAR_PROP_WRITE | CHAR_PROP_NOTIFY,
	U16_LO(FACTORY_RCU_WR_DP_H), U16_HI(FACTORY_RCU_WR_DP_H),
	0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x02,0x00,0x00,0x00
};
u16 char_desc_factory_rcu_ccc = 0;
u8 factory_rcu_data_ccc = 0x00;
u8 factory_mode_flag = 0x00;
u8 factory_report_ram_flag = 0x00;
u8 raw_buff[20] = {0};
u8 factory_Gsensor_Flag = 0x01;
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  factory_rcu_svc_attr_write_cb
 * @param  p
 * @return int
 * @retval void
 */
int factory_rcu_svc_attr_write_cb(void *p) 
{
    rf_packet_att_data_t *pw = p;
    if ((pw->l2capLen - 3) == 1 && pw->dat[0] == 0xF3) {

        printf("--->>> Single Tone start_reboot!\r\n");
        app_rcu_flash_unlock_init();//UNLOCK
        u8 rcu_mode_flash[2] = {0x02, 0x02};
        flash_erase_sector(CFG_BLE_24G_ADDR);
        flash_write_page (CFG_BLE_24G_ADDR, 2, rcu_mode_flash);
        app_rcu_flash_lock_init();  //LOCK
        start_reboot();
    } 
    
    return 1;
}

/******************************************************************
 * @brief  factory_rcu_svc_attr_read_cb
 * @param  p
 * @return int
 * @retval void
 */
int factory_rcu_svc_attr_read_cb(void *p) 
{
    return 1;
}

/******************************************************************
 * @brief  factory_rcu_ccc_update_cb
 * @param  p
 * @return int
 * @retval void
 */
int factory_rcu_ccc_update_cb(void *p) 
{
    printf("FACTORY_RCU_WR_CCC_H\n");
    factory_rcu_data_ccc = 0x01;
    return 0;
}

/******************************************************************
 * @brief  factory_rcu_enter_single_tone
 * @param  none
 * @return none
 * @retval void
 */
void factory_rcu_enter_single_tone(void)
{
    printf("--->>> [work_debug] factory_rcu_enter_single_tone\n");
    led_1_driver_init();
    gpio_write(GPIO_LED_1, !LED_ON_LEVAL); 
    led_2_driver_init();
    gpio_write(GPIO_LED_2, LED_ON_LEVAL);   
    pa_set_mode(MODE_TRANSMIT);
    rf_emi_single_tone(RF_POWER_N0p22dBm, 42);//-0.2dBm
    WaitMs(7000);
    rf_emi_stop();  
    printf("--->>> [work_debug] Exit Single Tone Finish\r\n");      
    gpio_write(GPIO_LED_2, !LED_ON_LEVAL); 
    printf("--->>> 2.4G start_reboot!\r\n");
    start_reboot(); 
}

_attribute_data_retention_ u8 factory_buf_aaa[20];
void report_factory_status(u8 factory_status, u8 *raw_buf)
{
	device_status = STATE_NORMAL;
	my_fifo_reset(&fifo_km);
	p_km_data->cmd = FACTORY_TEST_CMD;
	u8 temp_buff[10] = {0};
    memset(factory_buf_aaa, 0, sizeof(factory_buf_aaa));

    temp_buff[0] = 0xAA;
    temp_buff[1] = 0x00;

    if(app_tx_send_data_timer)
        temp_buff[1] = 0x99; 
    
    if(factory_status == 0x00)
        memcpy(factory_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);
    else
        memcpy(factory_buf_aaa, raw_buf, 10); 
    //printf("factory_buf_aaa: %02x %02x %02x %02x %02x %02x\r\n",factory_buf_aaa[0],factory_buf_aaa[1],factory_buf_aaa[2],factory_buf_aaa[3],factory_buf_aaa[4],factory_buf_aaa[5]);  

	#if (IS_SINGLE_GPIO_CHANGE_MODE)
		sing_gpio_mode_poll();
	#endif
		push_report_fifo();

	if ((device_status == STATE_NORMAL)) {
		reset_idle_status();
		my_fifo_push(&fifo_km, factory_buf_aaa, 6);
	}		
}

void recv_factory_data(u8 *factory_buf)
{
    u8 fw_ver_in_tp = 0;
    if((factory_buf[0] == 0x55) && (factory_buf[1] == 0xAA))
    {
        raw_buff[0] = 0xAA;//Sync byte
        raw_buff[1] = 0x01;//Object
        raw_buff[2] = 0x01;//Type
        raw_buff[3] = factory_buf[5];//Code
        raw_buff[4] = 0x01;//Ack
        report_factory_status(0x01,raw_buff);   

        factory_report_ram_flag = raw_buff[3];
        switch (factory_report_ram_flag)
        {
        case CMD_REQ_ONOFF_GSENSOR:
            factory_Gsensor_Flag = factory_buf[6]; 
            break;
        case CMD_REQ_DATA_GSENSOR:
            raw_buff[4] = Face_up_flag;//GensorData
            break; 
        case CMD_REQ_ONOFF_LED:
            if(factory_buf[6] == 0x00)
            {
                if(factory_buf[7] == 0x01)
                    gpio_write(GPIO_LED_1, LED_ON_LEVAL); 
                else
                    gpio_write(GPIO_LED_1, !LED_ON_LEVAL); 
            }
            if(factory_buf[6] == 0x01)
            {
                if(factory_buf[7] == 0x01)
                    gpio_write(GPIO_LED_2, LED_ON_LEVAL); 
                else
                    gpio_write(GPIO_LED_2, !LED_ON_LEVAL); 
            }   
            break;  
        case CMD_REQ_RCU_BATTERY:
        #if (CHARGE_ENABLE)
            app_rcu_batt_mv = get_battery_voltage();
   		    printf("--->>> [work_debug] app_rcu_batt_mv: %d mV\r\n",app_rcu_batt_mv);
            raw_buff[5] = (u8) (app_rcu_batt_mv & 0xFF);
            raw_buff[4] = (u8) ((app_rcu_batt_mv >> 8) & 0xFF);
        #endif
            break;
        case CMD_REQ_TOUCH_IC_VERSION:
        #if (TOUCH_ENABLE)
            fts_read_reg(0xA6, &fw_ver_in_tp);
            printf("fw version in tp:%02x\r\n", fw_ver_in_tp);
            raw_buff[4] = 0x00;
            raw_buff[5] = fw_ver_in_tp;
        #endif
            break;
        case CMD_REQ_SEND_TX_DATA:
            if(factory_buf[6])
                app_tx_send_data_timer_ms = factory_buf[6] * 1000;
            break;
        default:
            break;
        }
        
        report_factory_status(0x01,raw_buff);     
    }
    else if((factory_buf[0] == 0xAA) && (factory_buf[1] == 0x55))
    {
        switch (factory_report_ram_flag)
        {
        case CMD_REQ_SINGLETONE_MODE:
            printf("--->>> Single Tone start_reboot!\r\n");
            app_rcu_flash_unlock_init();//UNLOCK
            u8 rcu_mode_flash[2] = {0x02, 0x02};
            flash_erase_sector(CFG_BLE_24G_ADDR);
            flash_write_page (CFG_BLE_24G_ADDR, 2, rcu_mode_flash);
            app_rcu_flash_lock_init();  //LOCK
            start_reboot();
            break;
        case CMD_REQ_EXIT_FACTORY:
            printf("Exit Factory Mode!!!\r\n");
            app_factory_mode_timer = 0;
            gpio_write(GPIO_LED_2,!LED_ON_LEVAL);
            factory_mode_flag = 0;
            factory_Gsensor_Flag = 0x01;
            rcu_wakeup_touch = 0x00;
            rf_24G_deep_time_ms = 1000;
            rf_24G_deep_tick = clock_time();
            break;    
        case CMD_REQ_ENTER_SHIP_MODE:
            #if (CHARGE_ENABLE)
            enter_ship_mode();
            #endif
            break;    
        case CMD_REQ_SEND_TX_DATA:
            app_tx_send_data_timer = clock_time();
            break;
        default:
            break;
        }
        raw_buff[0] = 0xAA;
        raw_buff[1] = 0x00;
        factory_report_ram_flag = 0x00;
	    report_factory_status(0x00,raw_buff);
    }
}

#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
