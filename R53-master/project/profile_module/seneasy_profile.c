/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     seneasy_profile.c
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
#include "seneasy_profile.h"
#include "stack/ble/ble.h"
#include "dis_profile.h"
#include "../led_module/ltr_sensor.h"
#include "app_rcu.h"
#include "../led_module/aw21036_driver.h"
#include "../led_module/bct3236_driver.h"
#include "../led_module/backlight_handle.h"
#include "../key_module/key_handle.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
extern u8  device_in_connection_state;
extern u8 user_pair_wait_terminate;
extern u32 latest_user_event_tick;
u16 char_desc_smart_rcu_ccc = 0;
_attribute_data_retention_ u8 seneasy_get_Lux_flag = 0x00;
_attribute_data_retention_ u8 seneasy_rcu_data_ccc = 0x00;
u8 seneasy_key_flag = 0;
#if (FUNCTION_CALIBRATION_BL)
_attribute_data_retention_ u8 seneasy_bl_a_float = 0x01;
_attribute_data_retention_ u16 seneasy_bl_a_data = 100;
_attribute_data_retention_ u8 seneasy_bl_b_float = 0x01;
_attribute_data_retention_ u16 seneasy_bl_b_data = 0;
_attribute_data_retention_ u8 seneasy_test_bl_data [10]  = {0x44, 0x55, 0x66, 0x77, 0x01, 0x64, 0x00, 0x01, 0x00, 0x00};
#endif
/*********************************************************************************************
 *  Smart RCU Serivce 		8e8f2421-f12d-4462-a7fa-bd2cfa07eae3
 *  1. CHAR - data read		00000001-f12d-4462-a7fa-bd2cfa07eae3
 * 	2. CHAR - data write    00000002-f12d-4462-a7fa-bd2cfa07eae3
 *  3. CHAR - data notify   00000003-f12d-4462-a7fa-bd2cfa07eae3
 * 	Client Characteristic Configuration
 * ******************************************************************************************/
const u8 uuid_service_seneasy_rcu[16] = {0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x21,0x24,0x8f,0x8e};
const u8 uuid_char_seneasy_rcu_rd[16] = {0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x01,0x00,0x00,0x00};
const u8 uuid_char_seneasy_rcu_wr[16] = {0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x02,0x00,0x00,0x00};
// 1. Smart RCU data read
const u8 char_decl_seneasy_rcu_rd[19] = {
	CHAR_PROP_READ,
	U16_LO(SENEASY_RCU_RD_DP_H), U16_HI(SENEASY_RCU_RD_DP_H),
	0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x01,0x00,0x00,0x00
};
// 2. Smart RCU data write
const u8 char_decl_seneasy_rcu_wd[19] = {
	CHAR_PROP_WRITE | CHAR_PROP_NOTIFY,
	U16_LO(SENEASY_RCU_WR_DP_H), U16_HI(SENEASY_RCU_WR_DP_H),
	0xe3,0xea,0x07,0xfa,0x2c,0xbd,0xfa,0xa7,0x62,0x44,0x2d,0xf1,0x02,0x00,0x00,0x00
};
#if (TL_SZ119_TVSSU)
    extern void tv_ssu_factory_reset(void);
#endif
extern void advertise_tick_retime(void);
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  seneasy_rcu_svc_attr_write_cb
 * @param  p
 * @return int
 * @retval void
 */
int seneasy_rcu_svc_attr_write_cb(void *p) 
{
    u8 sendBuff[2] = {0};
    u8 sendBuff_Temp[9] = {0};
    rf_packet_att_data_t *pw = p;
    if ((pw->l2cap - 3) == 1 && pw->dat[0] == 0xF5) {
       // app_erase_pair();
        sendBuff[0] = 0x01;
        sendBuff[1] = 0x00;
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, sendBuff, 2);
        WaitMs(625);
        sendBuff[0] = 0x00;
        sendBuff[1] = 0x00;
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, sendBuff, 2);
        WaitMs(100);

        u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
        if(bond_number)   //at least 1 bonding device exist
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("--->>> [work_debug] seneasy_rcu_svc -> bls_smp_eraseAllParingInformation\n");
            #endif
            bls_smp_eraseAllParingInformation();
        }

#if (TL_SZ119_TVSSU)   
        tv_ssu_factory_reset();
#endif
        aw21036_all_off();
        ALS_Disable();
        aw21036_Disable();    
        cpu_sleep_wakeup(DEEPSLEEP_MODE,PM_WAKEUP_PAD,0);
    } 
    
    if ((pw->l2cap - 3) == 1 && pw->dat[0] == 0xF4) {
       // app_erase_pair();
        sendBuff[0] = 0x01;
        sendBuff[1] = 0x01;
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, sendBuff, 2);
    }

    //短暂关闭背光及指示灯功能
    if ((pw->l2cap - 3) == 5 && (pw->dat[0] == 0xC0) &&\
        (pw->dat[1] == 0x01) && (pw->dat[2] == 0x02) &&\
        (pw->dat[3] == 0x03)) {
            if((pw->dat[4] >= 1) && (pw->dat[4] <= 60))
            {
                br_test_close_led_settime = pw->dat[4] * 1000 * 1000;
                br_test_close_led_timer = clock_time() | 1;
                BR_TEST_CLOSE_FLAG = true;
                backlight_task = 0;
                aw21036_all_off();
                ALS_Disable();
                aw21036_Disable();

                sendBuff[0] = 0xC0;
                sendBuff[1] = 0x01;
                bls_att_pushNotifyData (SENEASY_RCU_WR_DP_H, sendBuff, 2);
            }
    }

    //读取光感数值
    if ((pw->l2cap - 3) == 5 && (pw->dat[0] == 0xC1) &&\
        (pw->dat[1] == 0x01) && (pw->dat[2] == 0x02) &&\
        (pw->dat[3] == 0x03) && (pw->dat[4] == 0x00)) {
            latest_user_event_tick = clock_time();
            aw21036_init_TEST();
            ltr_sensor_init_TEST(); 
            WaitMs(100);
            asl_status_error_cnt = 0;
            factory_Read_ASL = 1;
            ALS_Read_Status_TEST();
            printf("--->>> [seneasy_profile] Notify_Sy_Lux: %d Lux\r\n",Notify_Sy_Lux);
            sendBuff[0] = U16_LO(Notify_Sy_Lux);
            sendBuff[1] = U16_HI(Notify_Sy_Lux);
            bls_att_pushNotifyData (SENEASY_RCU_WR_DP_H, sendBuff, 2);
            app_dcdc_delay_disable();
    }

    //设置LED duty
    if ((pw->l2cap - 3) == 5 && (pw->dat[0] == 0xC2) &&\
        (pw->dat[1] == 0x01) && (pw->dat[2] == 0x02)) {
            latest_user_event_tick = clock_time();
            if((pw->dat[3] >= 1) && (pw->dat[3] <= 60) && (pw->dat[4] >= 0) && (pw->dat[4] <= 100))
            {
                br_test_led_duty_settime = pw->dat[3] * 1000 * 1000;
                br_test_led_duty_timer = clock_time() | 1;
                BR_TEST_LED_DUTY_FLAG = true;

                //I2C pin set
                gpio_set_func(GPIO_PD7,  AS_GPIO);
                gpio_set_output_en(GPIO_PD7, 1);
                gpio_write(GPIO_PD7,0);
                WaitMs(10);
                aw21036_init();
                LED_Test_SW_Duty(pw->dat[4]);

                sendBuff[0] = 0xC2;
                sendBuff[1] = 0x01;
                bls_att_pushNotifyData (SENEASY_RCU_WR_DP_H, sendBuff, 2);
            }
    }

    //设置BL Control
    if ((pw->l2cap - 3) == 5 && (pw->dat[0] == 0xC3)) {
        latest_user_event_tick = clock_time();
        if((pw->dat[3] >= 1) && (pw->dat[3] <= 60) && (pw->dat[4] >= 0) && (pw->dat[4] <= 100))
        {
            /*
                Mode Control
                01:Each LED Control
                02:Group LED Control           
            */
            //Each LED Control
            if(pw->dat[1] == 0x01)
            {
                //I2C pin set
                gpio_set_func(GPIO_PD7,  AS_GPIO);
                gpio_set_output_en(GPIO_PD7, 1);
                gpio_write(GPIO_PD7,0);
                WaitMs(10);
                aw21036_init();
                LED_Test_SW_Duty_EachLed(pw->dat[4], pw->dat[2]);
            }
            //Group LED Control
            if(pw->dat[1] == 0x02)
            {
                //I2C pin set
                gpio_set_func(GPIO_PD7,  AS_GPIO);
                gpio_set_output_en(GPIO_PD7, 1);
                gpio_write(GPIO_PD7,0);
                WaitMs(10);
                aw21036_init();
                LED_Test_SW_Duty_GroupLed(pw->dat[4], pw->dat[2]);
            }

            br_test_led_duty_settime = pw->dat[3] * 1000 * 1000;
            br_test_led_duty_timer = clock_time() | 1;
            BR_TEST_LED_DUTY_FLAG = true;

            sendBuff[0] = 0xC3;
            sendBuff[1] = 0x01;
            bls_att_pushNotifyData (SENEASY_RCU_WR_DP_H, sendBuff, 2);           
        }
    }

#if (FUNCTION_CALIBRATION_BL)
    //校准光感
    if ((pw->l2cap - 3) == 9 && (pw->dat[0] == 0xC4)) {
        latest_user_event_tick = clock_time();
        if((pw->dat[1] == 0x01) && (pw->dat[5] == 0x02))
        {
            seneasy_test_bl_data[0] = 0x44;
            seneasy_test_bl_data[1] = 0x55;
            seneasy_test_bl_data[2] = 0x66;
            seneasy_test_bl_data[3] = 0x77;

            seneasy_test_bl_data[4] = pw->dat[2];
            seneasy_test_bl_data[5] = pw->dat[3];
            seneasy_test_bl_data[6] = pw->dat[4];
            seneasy_test_bl_data[7] = pw->dat[6];
            seneasy_test_bl_data[8] = pw->dat[7];
            seneasy_test_bl_data[9] = pw->dat[8];
            seneasy_rcu_write_test_bl_addr(seneasy_test_bl_data);
            
            sendBuff_Temp[0] = 0xC5;
            for(int i = 1; i < 9; i++)
                sendBuff_Temp[i] = pw->dat[i];
            bls_att_pushNotifyData (SENEASY_RCU_WR_DP_H, sendBuff_Temp, 9);           
        }
    }
    //读取校准光感
    if ((pw->l2cap - 3) == 5 && (pw->dat[0] == 0xC5) &&\
        (pw->dat[1] == 0x01) && (pw->dat[2] == 0x02) &&\
        (pw->dat[3] == 0x03) && (pw->dat[4] == 0x00)) {
            read_flash_rcu_test_bl();
            sendBuff_Temp[0] = 0xC5;
            sendBuff_Temp[1] = 0x01;
            sendBuff_Temp[2] = seneasy_test_bl_data[4];
            sendBuff_Temp[3] = seneasy_test_bl_data[5];
            sendBuff_Temp[4] = seneasy_test_bl_data[6];
            sendBuff_Temp[5] = 0x02;
            sendBuff_Temp[6] = seneasy_test_bl_data[7];
            sendBuff_Temp[7] = seneasy_test_bl_data[8];
            sendBuff_Temp[8] = seneasy_test_bl_data[9];
            bls_att_pushNotifyData (SENEASY_RCU_WR_DP_H, sendBuff_Temp, 9);   
    }
#endif

    if ((pw->l2cap - 3) == 1 && pw->dat[0] == 0xB4) {
        memcpy(my_SerialNumtrs, pw->dat+4, 4);
        sendBuff[0] = 0x00;
        sendBuff[1] = 0x0D;
        seneasy_rcu_read_fw_addr(my_SerialNumtrs);
        bls_att_pushNotifyData (DeviceInformation_serialNum_DP_H,(u8 *)&my_SerialNumtrs, 4);
    }

    if ((pw->l2cap - 3) == 5 && pw->dat[0] == 0xB5) {
        memset(my_SerialNumtrs, 0, 30);
        memcpy(my_SerialNumtrs, pw->dat+1, 4);

        if ((strcmp((char*)my_SerialNumtrs,"EU01") != 0) && (strcmp((char*)my_SerialNumtrs,"LM01") != 0)) 
             strncpy((char*)my_SerialNumtrs,"EU01",4); 
        sendBuff[0] = 0x00;
        sendBuff[1] = 0x0D;
        seneasy_rcu_write_fw_addr(my_SerialNumtrs);
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, sendBuff, 2);
    }

    return 1;
}

/******************************************************************
 * @brief  seneasy_rcu_svc_attr_read_cb
 * @param  p
 * @return int
 * @retval void
 */
int seneasy_rcu_svc_attr_read_cb(void *p) 
{
    return 1;
}

/******************************************************************
 * @brief  seneasy_rcu_ccc_update_cb
 * @param  p
 * @return int
 * @retval void
 */
int seneasy_rcu_ccc_update_cb(void *p) 
{
    #if (!CLOSE_CODE_PRINTF)
    printf("SENEASY_RCU_WR_CCC_H\n");
    #endif
    seneasy_rcu_data_ccc = 0x01;
    return 0;
}

/******************************************************************
 * @brief  seneasy_rcu_write_fw_addr
 * @param  u8 *fw_addr
 * @return void
 * @retval void
 */
void seneasy_rcu_write_fw_addr(u8 *fw_addr) 
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] seneasy_rcu_write_fw_addr]\r\n");
    for(u8 i=0; i<4; i++)
    printf(":%02X", fw_addr[i]);
    printf("\n");
    #endif
    flash_erase_sector(FLASH_TPV_RCU_FW_ADDR);
    flash_write_page (FLASH_TPV_RCU_FW_ADDR, 4, fw_addr);

#if TPV_RCU_PROJECT_SEL
    read_flash_rcu_fw();
#endif
}

/******************************************************************
 * @brief  seneasy_rcu_read_fw_addr
 * @param  u8 *fw_addr
 * @return void
 * @retval void
 */
void seneasy_rcu_read_fw_addr(u8 *fw_addr) 
{
    flash_read_page (FLASH_TPV_RCU_FW_ADDR, 4, fw_addr);
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] seneasy_rcu_read_fw_addr]\r\n");
    for(u8 i=0; i<4; i++)
    printf(":%02X", fw_addr[i]);
    printf("\n");
    #endif
}

/******************************************************************
 * @brief  seneasy_rcu_key_handle
 * @param  none
 * @return none
 * @retval void
 */
void seneasy_rcu_key_handle(u8 notify_key)
{
    u8 sendBuff[6] = {0};

    if(seneasy_key_flag == 0x01)
    {
        if(notify_key == KEY_NUM_1)
            notify_key = 0x28;        
    }
    //上报光感值及按键值
    sendBuff[0] = 0xE0;
    sendBuff[1] = U16_LO(Notify_Sy_Lux);
    sendBuff[2] = U16_HI(Notify_Sy_Lux);
    sendBuff[3] = 0xE1;
    sendBuff[4] = notify_key;
    sendBuff[5] = 0x00;
    bls_att_pushNotifyData (SENEASY_RCU_WR_DP_H, sendBuff, 6);     
    seneasy_key_flag = 0x00; 
}

#if (FUNCTION_CALIBRATION_BL)
/******************************************************************
 * @brief  seneasy_rcu_write_test_bl_addr
 * @param  u8 *fw_addr
 * @return void
 * @retval void
 */
void seneasy_rcu_write_test_bl_addr(u8 *fw_addr) 
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] seneasy_rcu_write_fw_addr]\r\n");
    for(u8 i=0; i<4; i++)
    printf(":%02X", fw_addr[i]);
    printf("\n");
    #endif
    flash_erase_sector(FLASH_RCU_TEST_BL_ADDR);
    flash_write_page (FLASH_RCU_TEST_BL_ADDR, 10, fw_addr);

#if TPV_RCU_PROJECT_SEL
    read_flash_rcu_test_bl();
#endif
}

/******************************************************************
 * @brief  read_flash_rcu_test_bl
 * @param  none
 * @return none
 * @retval void
 */
void read_flash_rcu_test_bl(void)
{
    memset(seneasy_test_bl_data, 0, 10);
	flash_read_page(FLASH_RCU_TEST_BL_ADDR, 10, seneasy_test_bl_data);
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] read_flash_rcu_test_bl = [");
    for (int i=0; i<6; i++)
    {
        printf("%02x ",seneasy_test_bl_data[i]);
    }
    printf("]\r\n");
    #endif
    if((seneasy_test_bl_data[0] == 0x44) && (seneasy_test_bl_data[1] == 0x55) && (seneasy_test_bl_data[2] == 0x66) && (seneasy_test_bl_data[3] == 0x77))
    {
        seneasy_bl_a_float = seneasy_test_bl_data[4];
        seneasy_bl_b_float = seneasy_test_bl_data[7];
        seneasy_bl_a_data = (seneasy_test_bl_data[6] << 8) | seneasy_test_bl_data[5];
        seneasy_bl_b_data = (seneasy_test_bl_data[9] << 8) | seneasy_test_bl_data[8];        
    }
    else
    {
        seneasy_bl_a_float = 0x01;
        seneasy_bl_a_data  = 100;
        seneasy_bl_b_float = 0x01;
        seneasy_bl_b_data  = 0;
    }

    #if (!CLOSE_CODE_PRINTF)
    if(seneasy_bl_a_float == 0x01)
        printf("--->>> [work_Debug] seneasy_bl_a_data = +%d\r\n",seneasy_bl_a_data);
    else
        printf("--->>> [work_Debug] seneasy_bl_a_data = -%d\r\n",seneasy_bl_a_data);
    if(seneasy_bl_b_float == 0x01)
        printf("--->>> [work_Debug] seneasy_bl_b_data = +%d\r\n",seneasy_bl_b_data);
    else
        printf("--->>> [work_Debug] seneasy_bl_b_data = -%d\r\n",seneasy_bl_b_data);
    #endif
}
#endif

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
