/**
**********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     usb_chg.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-20
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _USB_CHG_H_
#define _USB_CHG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <board.h>

#define IO_MSG_SUBTYPE_USB_CHG      0xFE
#define BAT_STATUS_CHECK_TIME       3 * 60 * 1000 //3分钟检测一次

#define POWER_ON            P0_0
#define ADC_BAT_VET_C       P0_2
#define USB_DET             P2_3
#define USB_DET_IRQ         GPIO19_IRQn
#define USB_DET_Handler     GPIO19_Handler

#define POWER_ON_OPEN() GPIO_SetBits(GPIO_GetPin(POWER_ON))
#define POWER_ON_OFF()  GPIO_ResetBits(GPIO_GetPin(POWER_ON))

#define ADC_BAT_VET_C_OPEN() GPIO_SetBits(GPIO_GetPin(ADC_BAT_VET_C))
#define ADC_BAT_VET_C_OFF()  GPIO_ResetBits(GPIO_GetPin(ADC_BAT_VET_C))

static bool bat_chg_done;
static bool usb_chg_tatus;

void power_on_io_init(void);
void usb_chg_gpio_init(void);
void usb_chg_task_init(void);
void delete_usb_chg_task(void);
void all_sys_function_off(void);
void usb_chg_task(void *p_param);
void bat_get_chg_status(void);
void usb_set_chg_mode(void);
void bat_chg_status_check_timer_cb(TimerHandle_t pxTimer);
bool bat_chg_doing(void);

#ifdef __cplusplus
}
#endif

#endif


/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

