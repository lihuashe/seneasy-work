/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bct3236_driver.c
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
#include "bct3236_driver.h"
#include "led_driver.h"
#include "ltr_sensor.h"
#include "backlight_handle.h"
#include "../../sdk/drivers/8278/i2c.h"
#include "../../sdk/drivers/8278/gpio.h"
#include "../app_rcu.h"
#include "aw21036_driver.h"
#include "../profile_module/dis_profile.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
s16 flagbreathe=0,openallcnt=0;
u8 backlight_rcu_ledflag = 0;
s16 backlight_rcu_ledcnt = 0;
/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  bct3236_init
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_init(void)
{
#if (LED_BCT3236_ENABLE)
    printf("bct3236_init !!!\r\n");
    gpio_set_func(GPIO_PC4,  AS_GPIO);
    gpio_set_output_en(GPIO_PC   4, 1);
    gpio_write(GPIO_PC4,1);

    user_i2c_gpio_set(I2C_GPIO_GROUP_C2C3);
	i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

	//i2c_write_byte(0x4F, 1, 0x00);
    i2c_write_byte(0x00, 1, 0x01);

    sensor_init_error_cnt = 0;
    if(led_display_flag)
        ALS_Read_Status();
    /*
    while (1)
    {
        bct3236_breath();
        TimeCtrl();
    }
    */
#endif
}

/******************************************************************
 * @brief  bct3236_all_on
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_all_on(void)
{
#if (LED_BCT3236_ENABLE)
    printf("bct3236_all_on !!!\r\n");
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    for(int i=1; i<36; i++)
        i2c_write_byte(i, 1, 0xFF);
    
    for(int i=1; i<36; i++)
        i2c_write_byte(i+37, 1, 0x01);
    
	i2c_write_byte(0x25, 1, 0x00);
    i2c_write_byte(0x4A, 1, 0x00);	
#endif
}

/******************************************************************
 * @brief  bct3236_breath
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_breath(void)
{
#if (LED_BCT3236_ENABLE)
    printf("bct3236_breath !!!\r\n");
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    for(int i=1; i<36; i++)
        i2c_write_byte(i, 1, openallcnt);

    for(int i=1; i<36; i++)
        i2c_write_byte(i+37, 1, 0x01);

    i2c_write_byte(0x4A, 1, 0x00);

#endif
}

void TimeCtrl(){
		if(!flagbreathe){
			openallcnt++;
			if(openallcnt>=0xFF){flagbreathe=1;openallcnt=0xFF;}
		}
		else{
			openallcnt--;
			if(openallcnt<=2){flagbreathe=0;openallcnt=0;}
		}
}

/******************************************************************
 * @brief  bct3236_breath_NormalKey_00 / 01
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_breath_NormalKey_00(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_POWER,       1, openallcnt);
    i2c_write_byte(LED_AMBILIGHT,   1, openallcnt);
#if (PCB_LED_VER == PCB_LED_INIT)
    i2c_write_byte(LED_SOURCE,      1, openallcnt);
#elif (PCB_LED_VER == PCB_LED_POINT)
    if ((strcmp((char*)rcu_project_name,"EU01") == 0) ||\
        (strcmp((char*)rcu_project_name,"RU01") == 0))
        i2c_write_byte(LED_SOURCE,  1, openallcnt);
#endif
    i2c_write_byte(LED_VOICE,       1, openallcnt);
    i2c_write_byte(LED_MENU,        1, openallcnt);
    i2c_write_byte(LED_BACK,        1, openallcnt);
    i2c_write_byte(LED_HOME,        1, openallcnt);
    i2c_write_byte(LED_VOL_L,       1, openallcnt);
    i2c_write_byte(LED_VOL_R,       1, openallcnt);
    i2c_write_byte(LED_123,         1, openallcnt);
    i2c_write_byte(LED_CHN_L,       1, openallcnt);
    i2c_write_byte(LED_CHN_R,       1, openallcnt);
    i2c_write_byte(LED_OTT_1,       1, openallcnt);
    i2c_write_byte(LED_OTT_2,       1, openallcnt);
    i2c_write_byte(LED_OTT_3,       1, openallcnt);
    i2c_write_byte(LED_OTT_4,       1, openallcnt);
    i2c_write_byte(LED_OTT_5,       1, openallcnt);
    i2c_write_byte(LED_OTT_6,       1, openallcnt);

    i2c_write_byte(LED_POWER + 37,  1, 0x01);
    i2c_write_byte(LED_AMBILIGHT + 37, 1, 0x01);
#if (PCB_LED_VER == PCB_LED_INIT)
    i2c_write_byte(LED_SOURCE + 37, 1, 0x01);
#elif (PCB_LED_VER == PCB_LED_POINT)
    if ((strcmp((char*)rcu_project_name,"EU01") == 0) ||\
        (strcmp((char*)rcu_project_name,"RU01") == 0))
        i2c_write_byte(LED_SOURCE + 37, 1, 0x01);
#endif
    i2c_write_byte(LED_VOICE + 37,  1, 0x01);
    i2c_write_byte(LED_MENU + 37,   1, 0x01);
    i2c_write_byte(LED_BACK + 37,   1, 0x01);
    i2c_write_byte(LED_HOME + 37,   1, 0x01);
    i2c_write_byte(LED_VOL_L + 37,  1, 0x01);
    i2c_write_byte(LED_VOL_R + 37,  1, 0x01);
    i2c_write_byte(LED_123 + 37,    1, 0x01);
    i2c_write_byte(LED_CHN_L + 37,  1, 0x01);
    i2c_write_byte(LED_CHN_R + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_1 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_2 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_3 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_4 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_5 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_6 + 37,  1, 0x01);

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}
void bct3236_breath_NormalKey_01(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_AMBILIGHT,   1, openallcnt);
    i2c_write_byte(LED_VOL_L,       1, openallcnt);
    i2c_write_byte(LED_VOL_R,       1, openallcnt);
    i2c_write_byte(LED_CHN_L,       1, openallcnt);
    i2c_write_byte(LED_CHN_R,       1, openallcnt);

    i2c_write_byte(LED_AMBILIGHT + 37, 1, 0x01);
    i2c_write_byte(LED_VOL_L + 37,  1, 0x01);
    i2c_write_byte(LED_VOL_R + 37,  1, 0x01);
    i2c_write_byte(LED_CHN_L + 37,  1, 0x01);
    i2c_write_byte(LED_CHN_R + 37,  1, 0x01);

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}

/******************************************************************
 * @brief  bct3236_breath_NormalKey_00_OFF / 01
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_breath_NormalKey_00_OFF(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_POWER,       1, 0x00);
    i2c_write_byte(LED_AMBILIGHT,   1, 0x00);
#if (PCB_LED_VER == PCB_LED_INIT)
    i2c_write_byte(LED_SOURCE,      1, 0x00);
#elif (PCB_LED_VER == PCB_LED_POINT)
    if ((strcmp((char*)rcu_project_name,"EU01") == 0) ||\
        (strcmp((char*)rcu_project_name,"RU01") == 0))
        i2c_write_byte(LED_SOURCE,  1, 0x00);
#endif
    i2c_write_byte(LED_VOICE,       1, 0x00);
    i2c_write_byte(LED_MENU,        1, 0x00);
    i2c_write_byte(LED_BACK,        1, 0x00);
    i2c_write_byte(LED_HOME,        1, 0x00);
    i2c_write_byte(LED_VOL_L,       1, 0x00);
    i2c_write_byte(LED_VOL_R,       1, 0x00);
    i2c_write_byte(LED_123,         1, 0x00);
    i2c_write_byte(LED_CHN_L,       1, 0x00);
    i2c_write_byte(LED_CHN_R,       1, 0x00);
    i2c_write_byte(LED_OTT_1,       1, 0x00);
    i2c_write_byte(LED_OTT_2,       1, 0x00);
    i2c_write_byte(LED_OTT_3,       1, 0x00);
    i2c_write_byte(LED_OTT_4,       1, 0x00);
    i2c_write_byte(LED_OTT_5,       1, 0x00);
    i2c_write_byte(LED_OTT_6,       1, 0x00);

    i2c_write_byte(LED_POWER + 37,  1, 0x00);
    i2c_write_byte(LED_AMBILIGHT + 37, 1, 0x00);
#if (PCB_LED_VER == PCB_LED_INIT)
    i2c_write_byte(LED_SOURCE + 37, 1, 0x00);
#elif (PCB_LED_VER == PCB_LED_POINT)
    if ((strcmp((char*)rcu_project_name,"EU01") == 0) ||\
        (strcmp((char*)rcu_project_name,"RU01") == 0))
        i2c_write_byte(LED_SOURCE + 37, 1, 0x00);
#endif
    i2c_write_byte(LED_VOICE + 37,  1, 0x00);
    i2c_write_byte(LED_MENU + 37,   1, 0x00);
    i2c_write_byte(LED_BACK + 37,   1, 0x00);
    i2c_write_byte(LED_HOME + 37,   1, 0x00);
    i2c_write_byte(LED_VOL_L + 37,  1, 0x00);
    i2c_write_byte(LED_VOL_R + 37,  1, 0x00);
    i2c_write_byte(LED_123 + 37,    1, 0x00);
    i2c_write_byte(LED_CHN_L + 37,  1, 0x00);
    i2c_write_byte(LED_CHN_R + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_1 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_2 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_3 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_4 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_5 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_6 + 37,  1, 0x00);

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}
void bct3236_breath_NormalKey_01_OFF(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_AMBILIGHT,   1, 0x00);
    i2c_write_byte(LED_VOL_L,       1, 0x00);
    i2c_write_byte(LED_VOL_R,       1, 0x00);
    i2c_write_byte(LED_CHN_L,       1, 0x00);
    i2c_write_byte(LED_CHN_R,       1, 0x00);

    i2c_write_byte(LED_AMBILIGHT + 37, 1, 0x00);
    i2c_write_byte(LED_VOL_L + 37,  1, 0x00);
    i2c_write_byte(LED_VOL_R + 37,  1, 0x00);
    i2c_write_byte(LED_CHN_L + 37,  1, 0x00);
    i2c_write_byte(LED_CHN_R + 37,  1, 0x00);
    i2c_write_byte(0x4A, 1, 0x00);
#endif
}

/******************************************************************
 * @brief  bct3236_breath_HiddenKey
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_breath_HiddenKey(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_NUM_1,       1, openallcnt);
    i2c_write_byte(LED_NUM_2,       1, openallcnt);
    i2c_write_byte(LED_NUM_3,       1, openallcnt);
    i2c_write_byte(LED_NUM_4,       1, openallcnt);
    i2c_write_byte(LED_NUM_5,       1, openallcnt);
    i2c_write_byte(LED_NUM_6,       1, openallcnt);
    i2c_write_byte(LED_NUM_7,       1, openallcnt);
    i2c_write_byte(LED_NUM_8,       1, openallcnt);
    i2c_write_byte(LED_NUM_9,       1, openallcnt);
    i2c_write_byte(LED_NUM_0,       1, openallcnt);
    i2c_write_byte(LED_REWIND,      1, openallcnt);
    i2c_write_byte(LED_PAUSE,       1, openallcnt);
    i2c_write_byte(LED_PLAY,        1, openallcnt);
    i2c_write_byte(LED_FFWD,        1, openallcnt);
#if (PCB_LED_VER == PCB_LED_POINT)
    if (strcmp((char*)rcu_project_name,"LM01") == 0)
        i2c_write_byte(LED_SOURCE,  1, openallcnt);
#endif

    i2c_write_byte(LED_NUM_1 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_2 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_3 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_4 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_5 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_6 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_7 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_8 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_9 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_0 + 37,  1, 0x01);
    i2c_write_byte(LED_REWIND + 37, 1, 0x01);
    i2c_write_byte(LED_PAUSE + 37,  1, 0x01);
    i2c_write_byte(LED_PLAY + 37,   1, 0x01);
    i2c_write_byte(LED_FFWD + 37,   1, 0x01);
#if (PCB_LED_VER == PCB_LED_POINT)
    if (strcmp((char*)rcu_project_name,"LM01") == 0)
        i2c_write_byte(LED_SOURCE + 37,  1, 0x01);
#endif

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}

/******************************************************************
 * @brief  bct3236_breath_HiddenKey_OFF
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_breath_HiddenKey_OFF(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_NUM_1,       1, 0x00);
    i2c_write_byte(LED_NUM_2,       1, 0x00);
    i2c_write_byte(LED_NUM_3,       1, 0x00);
    i2c_write_byte(LED_NUM_4,       1, 0x00);
    i2c_write_byte(LED_NUM_5,       1, 0x00);
    i2c_write_byte(LED_NUM_6,       1, 0x00);
    i2c_write_byte(LED_NUM_7,       1, 0x00);
    i2c_write_byte(LED_NUM_8,       1, 0x00);
    i2c_write_byte(LED_NUM_9,       1, 0x00);
    i2c_write_byte(LED_NUM_0,       1, 0x00);
    i2c_write_byte(LED_REWIND,      1, 0x00);
    i2c_write_byte(LED_PAUSE,       1, 0x00);
    i2c_write_byte(LED_PLAY,        1, 0x00);
    i2c_write_byte(LED_FFWD,        1, 0x00);
#if (PCB_LED_VER == PCB_LED_POINT)
    if (strcmp((char*)rcu_project_name,"LM01") == 0)
        i2c_write_byte(LED_SOURCE,  1, 0x00);
#endif

    i2c_write_byte(LED_NUM_1 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_2 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_3 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_4 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_5 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_6 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_7 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_8 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_9 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_0 + 37,  1, 0x00);
    i2c_write_byte(LED_REWIND + 37, 1, 0x00);
    i2c_write_byte(LED_PAUSE + 37,  1, 0x00);
    i2c_write_byte(LED_PLAY + 37,   1, 0x00);
    i2c_write_byte(LED_FFWD + 37,   1, 0x00);
#if (PCB_LED_VER == PCB_LED_POINT)
    if (strcmp((char*)rcu_project_name,"LM01") == 0)
        i2c_write_byte(LED_SOURCE + 37,  1, 0x00);
#endif

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}

/******************************************************************
 * @brief  bct3236_breath_allKey_00 / 01
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_breath_AllKey_00(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_POWER,       1, openallcnt);
    i2c_write_byte(LED_AMBILIGHT,   1, openallcnt);
    i2c_write_byte(LED_SOURCE,      1, openallcnt);
    i2c_write_byte(LED_VOICE,       1, openallcnt);
    i2c_write_byte(LED_MENU,        1, openallcnt);
    i2c_write_byte(LED_BACK,        1, openallcnt);
    i2c_write_byte(LED_HOME,        1, openallcnt);
    i2c_write_byte(LED_VOL_L,       1, openallcnt);
    i2c_write_byte(LED_VOL_R,       1, openallcnt);
    i2c_write_byte(LED_123,         1, openallcnt);
    i2c_write_byte(LED_CHN_L,       1, openallcnt);
    i2c_write_byte(LED_CHN_R,       1, openallcnt);
    i2c_write_byte(LED_OTT_1,       1, openallcnt);
    i2c_write_byte(LED_OTT_2,       1, openallcnt);
    i2c_write_byte(LED_OTT_3,       1, openallcnt);
    i2c_write_byte(LED_OTT_4,       1, openallcnt);
    i2c_write_byte(LED_OTT_5,       1, openallcnt);
    i2c_write_byte(LED_OTT_6,       1, openallcnt);
    i2c_write_byte(LED_NUM_1,       1, openallcnt);
    i2c_write_byte(LED_NUM_2,       1, openallcnt);
    i2c_write_byte(LED_NUM_3,       1, openallcnt);
    i2c_write_byte(LED_NUM_4,       1, openallcnt);
    i2c_write_byte(LED_NUM_5,       1, openallcnt);
    i2c_write_byte(LED_NUM_6,       1, openallcnt);
    i2c_write_byte(LED_NUM_7,       1, openallcnt);
    i2c_write_byte(LED_NUM_8,       1, openallcnt);
    i2c_write_byte(LED_NUM_9,       1, openallcnt);
    i2c_write_byte(LED_NUM_0,       1, openallcnt);
    i2c_write_byte(LED_REWIND,      1, openallcnt);
    i2c_write_byte(LED_PAUSE,       1, openallcnt);
    i2c_write_byte(LED_PLAY,        1, openallcnt);
    i2c_write_byte(LED_FFWD,        1, openallcnt);

    i2c_write_byte(LED_POWER + 37,  1, 0x01);
    i2c_write_byte(LED_AMBILIGHT + 37, 1, 0x01);
    i2c_write_byte(LED_SOURCE + 37, 1, 0x01);
    i2c_write_byte(LED_VOICE + 37,  1, 0x01);
    i2c_write_byte(LED_MENU + 37,   1, 0x01);
    i2c_write_byte(LED_BACK + 37,   1, 0x01);
    i2c_write_byte(LED_HOME + 37,   1, 0x01);
    i2c_write_byte(LED_VOL_L + 37,  1, 0x01);
    i2c_write_byte(LED_VOL_R + 37,  1, 0x01);
    i2c_write_byte(LED_123 + 37,    1, 0x01);
    i2c_write_byte(LED_CHN_L + 37,  1, 0x01);
    i2c_write_byte(LED_CHN_R + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_1 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_2 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_3 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_4 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_5 + 37,  1, 0x01);
    i2c_write_byte(LED_OTT_6 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_1 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_2 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_3 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_4 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_5 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_6 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_7 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_8 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_9 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_0 + 37,  1, 0x01);
    i2c_write_byte(LED_REWIND + 37, 1, 0x01);
    i2c_write_byte(LED_PAUSE + 37,  1, 0x01);
    i2c_write_byte(LED_PLAY + 37,   1, 0x01);
    i2c_write_byte(LED_FFWD + 37,   1, 0x01);

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}
void bct3236_breath_AllKey_01(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_AMBILIGHT,   1, openallcnt);
    i2c_write_byte(LED_VOL_L,       1, openallcnt);
    i2c_write_byte(LED_VOL_R,       1, openallcnt);
    i2c_write_byte(LED_CHN_L,       1, openallcnt);
    i2c_write_byte(LED_CHN_R,       1, openallcnt);
    i2c_write_byte(LED_NUM_1,       1, openallcnt);
    i2c_write_byte(LED_NUM_2,       1, openallcnt);
    i2c_write_byte(LED_NUM_3,       1, openallcnt);
    i2c_write_byte(LED_NUM_4,       1, openallcnt);
    i2c_write_byte(LED_NUM_5,       1, openallcnt);
    i2c_write_byte(LED_NUM_6,       1, openallcnt);
    i2c_write_byte(LED_NUM_7,       1, openallcnt);
    i2c_write_byte(LED_NUM_8,       1, openallcnt);
    i2c_write_byte(LED_NUM_9,       1, openallcnt);
    i2c_write_byte(LED_NUM_0,       1, openallcnt);
    i2c_write_byte(LED_REWIND,      1, openallcnt);
    i2c_write_byte(LED_PAUSE,       1, openallcnt);
    i2c_write_byte(LED_PLAY,        1, openallcnt);
    i2c_write_byte(LED_FFWD,        1, openallcnt);

    i2c_write_byte(LED_AMBILIGHT + 37, 1, 0x01);
    i2c_write_byte(LED_VOL_L + 37,  1, 0x01);
    i2c_write_byte(LED_VOL_R + 37,  1, 0x01);
    i2c_write_byte(LED_CHN_L + 37,  1, 0x01);
    i2c_write_byte(LED_CHN_R + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_1 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_2 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_3 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_4 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_5 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_6 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_7 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_8 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_9 + 37,  1, 0x01);
    i2c_write_byte(LED_NUM_0 + 37,  1, 0x01);
    i2c_write_byte(LED_REWIND + 37, 1, 0x01);
    i2c_write_byte(LED_PAUSE + 37,  1, 0x01);
    i2c_write_byte(LED_PLAY + 37,   1, 0x01);
    i2c_write_byte(LED_FFWD + 37,   1, 0x01);

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}

/******************************************************************
 * @brief  bct3236_breath_allKey_00_OFF / 01
 * @param  none
 * @return none
 * @retval void
 */
void bct3236_breath_AllKey_00_OFF(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_POWER,       1, 0x00);
    i2c_write_byte(LED_AMBILIGHT,   1, 0x00);
    i2c_write_byte(LED_SOURCE,      1, 0x00);
    i2c_write_byte(LED_VOICE,       1, 0x00);
    i2c_write_byte(LED_MENU,        1, 0x00);
    i2c_write_byte(LED_BACK,        1, 0x00);
    i2c_write_byte(LED_HOME,        1, 0x00);
    i2c_write_byte(LED_VOL_L,       1, 0x00);
    i2c_write_byte(LED_VOL_R,       1, 0x00);
    i2c_write_byte(LED_123,         1, 0x00);
    i2c_write_byte(LED_CHN_L,       1, 0x00);
    i2c_write_byte(LED_CHN_R,       1, 0x00);
    i2c_write_byte(LED_OTT_1,       1, 0x00);
    i2c_write_byte(LED_OTT_2,       1, 0x00);
    i2c_write_byte(LED_OTT_3,       1, 0x00);
    i2c_write_byte(LED_OTT_4,       1, 0x00);
    i2c_write_byte(LED_OTT_5,       1, 0x00);
    i2c_write_byte(LED_OTT_6,       1, 0x00);
    i2c_write_byte(LED_NUM_1,       1, 0x00);
    i2c_write_byte(LED_NUM_2,       1, 0x00);
    i2c_write_byte(LED_NUM_3,       1, 0x00);
    i2c_write_byte(LED_NUM_4,       1, 0x00);
    i2c_write_byte(LED_NUM_5,       1, 0x00);
    i2c_write_byte(LED_NUM_6,       1, 0x00);
    i2c_write_byte(LED_NUM_7,       1, 0x00);
    i2c_write_byte(LED_NUM_8,       1, 0x00);
    i2c_write_byte(LED_NUM_9,       1, 0x00);
    i2c_write_byte(LED_NUM_0,       1, 0x00);
    i2c_write_byte(LED_REWIND,      1, 0x00);
    i2c_write_byte(LED_PAUSE,       1, 0x00);
    i2c_write_byte(LED_PLAY,        1, 0x00);
    i2c_write_byte(LED_FFWD,        1, 0x00);

    i2c_write_byte(LED_POWER + 37,  1, 0x00);
    i2c_write_byte(LED_AMBILIGHT + 37, 1, 0x00);
    i2c_write_byte(LED_SOURCE + 37, 1, 0x00);
    i2c_write_byte(LED_VOICE + 37,  1, 0x00);
    i2c_write_byte(LED_MENU + 37,   1, 0x00);
    i2c_write_byte(LED_BACK + 37,   1, 0x00);
    i2c_write_byte(LED_HOME + 37,   1, 0x00);
    i2c_write_byte(LED_VOL_L + 37,  1, 0x00);
    i2c_write_byte(LED_VOL_R + 37,  1, 0x00);
    i2c_write_byte(LED_123 + 37,    1, 0x00);
    i2c_write_byte(LED_CHN_L + 37,  1, 0x00);
    i2c_write_byte(LED_CHN_R + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_1 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_2 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_3 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_4 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_5 + 37,  1, 0x00);
    i2c_write_byte(LED_OTT_6 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_1 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_2 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_3 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_4 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_5 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_6 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_7 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_8 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_9 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_0 + 37,  1, 0x00);
    i2c_write_byte(LED_REWIND + 37, 1, 0x00);
    i2c_write_byte(LED_PAUSE + 37,  1, 0x00);
    i2c_write_byte(LED_PLAY + 37,   1, 0x00);
    i2c_write_byte(LED_FFWD + 37,   1, 0x00);

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}
void bct3236_breath_AllKey_01_OFF(void)
{
#if (LED_BCT3236_ENABLE)
    i2c_master_init(BCT3236_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*400000)) );

    i2c_write_byte(0x00, 1, 0x01);
    i2c_write_byte(0x25, 1, 0x00);

    i2c_write_byte(LED_AMBILIGHT,   1, 0x00);
    i2c_write_byte(LED_VOL_L,       1, 0x00);
    i2c_write_byte(LED_VOL_R,       1, 0x00);
    i2c_write_byte(LED_CHN_L,       1, 0x00);
    i2c_write_byte(LED_CHN_R,       1, 0x00);
    i2c_write_byte(LED_NUM_1,       1, 0x00);
    i2c_write_byte(LED_NUM_2,       1, 0x00);
    i2c_write_byte(LED_NUM_3,       1, 0x00);
    i2c_write_byte(LED_NUM_4,       1, 0x00);
    i2c_write_byte(LED_NUM_5,       1, 0x00);
    i2c_write_byte(LED_NUM_6,       1, 0x00);
    i2c_write_byte(LED_NUM_7,       1, 0x00);
    i2c_write_byte(LED_NUM_8,       1, 0x00);
    i2c_write_byte(LED_NUM_9,       1, 0x00);
    i2c_write_byte(LED_NUM_0,       1, 0x00);
    i2c_write_byte(LED_REWIND,      1, 0x00);
    i2c_write_byte(LED_PAUSE,       1, 0x00);
    i2c_write_byte(LED_PLAY,        1, 0x00);
    i2c_write_byte(LED_FFWD,        1, 0x00);
    
    i2c_write_byte(LED_AMBILIGHT + 37, 1, 0x01);
    i2c_write_byte(LED_VOL_L + 37,  1, 0x01);
    i2c_write_byte(LED_VOL_R + 37,  1, 0x01);
    i2c_write_byte(LED_CHN_L + 37,  1, 0x00);
    i2c_write_byte(LED_CHN_R + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_1 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_2 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_3 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_4 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_5 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_6 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_7 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_8 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_9 + 37,  1, 0x00);
    i2c_write_byte(LED_NUM_0 + 37,  1, 0x00);
    i2c_write_byte(LED_REWIND + 37, 1, 0x00);
    i2c_write_byte(LED_PAUSE + 37,  1, 0x00);
    i2c_write_byte(LED_PLAY + 37,   1, 0x00);
    i2c_write_byte(LED_FFWD + 37,   1, 0x00);

    i2c_write_byte(0x4A, 1, 0x00);
#endif
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
