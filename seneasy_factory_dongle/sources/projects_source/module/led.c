/**********************************************************************************************************************
 * @file     led.c
 * @author   Maxscend AE Team
 * @version  V1.0
 * @date     2023/01/11
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "type_def.h"
#include "hw_crg.h"
#include "hw_pmu.h"
#include "hw_sys_ctrl.h"
#include "hw_gpio.h"
#include "hw_uart.h"
#include "hw_dma.h"
#include "hw_timer.h"
#include "hw_stim.h"

#include "hal_timer.h"
#include "utility.h"

#include "app_cfg.h"

#include "led.h"

//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static led_event_callback_t led_stop_event_callback;
static stLedParam_t gstLedParam;

/**********************************************************************************************************************
 * @brief  led timer interrupt init
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
static void led_timer_int_init( void )
{
    rom_hw_pmu_set_wakeup_source(14, LUT_TRIG_ID_OTHER, LUT_TRIG_ID_STIM1_CH2, LUT_ACT_PD_SYS_ON|LUT_ACT_DCXO_HCLK_ON|LUT_ACT_CP_IRQ_EN);

    rom_hw_sys_ctrl_enable_peri_int(SYS_CTRL_CP, LED_TIMER_IRQ);
    NVIC_ClearPendingIRQ(LED_TIMER_IRQ);
    NVIC_SetPriority(LED_TIMER_IRQ, 0x03);
    NVIC_EnableIRQ(LED_TIMER_IRQ);
}

/**********************************************************************************************************************
 * @brief  led timer interrupt enable
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
static void led_timer_int_enable( void )
{
    if(gstLedParam.enLedCurrentState != EN_LED_STATE_OFF)
    {
        rom_hw_stim_enable_interrupt(LED_TIMER, LED_TIMER_INT_MSK);
        rom_hw_stim_set_compare(LED_TIMER, LED_TIMER_CH, TIME_MS_TO_32K_CNT(LED_TIMER_PERIOD));
        NVIC_SetPendingIRQ(LED_TIMER_IRQ);
        rom_hw_stim_enable_wakeup(STIM1, STIM_CH2_INT_MATCH_WAKEUP);
    }
}

/**********************************************************************************************************************
 * @brief  led timer interrupt disable
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
static void led_timer_int_disable( void )
{
    if(gstLedParam.enLedCurrentState == EN_LED_STATE_OFF)
    {
        rom_hw_stim_disable_interrupt(LED_TIMER, LED_TIMER_INT_MSK);
        rom_hw_stim_disable_wakeup(LED_TIMER, STIM_CH2_INT_MATCH_WAKEUP);
        // PRINTF("LED_TIMER disable\n");
    } 
}

/**********************************************************************************************************************
 * @brief  led gpio init
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
static void led_gpio_init( void )
{
    rom_hw_gpio_set_pin_input_output(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum), GPIO_MODE_OUTPUT);
    rom_hw_gpio_set_pin_output_level(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum), !gstLedParam.enLedOnLevel);
    rom_hw_gpio_set_pin_pull_mode(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum), GPIO_PULL_UP);
}

/**********************************************************************************************************************
 * @brief  led gpio enable
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
static void led_gpio_enable( void )
{
    rom_hw_gpio_set_pin_pull_mode(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum), GPIO_PULL_DOWN);
    rom_hw_gpio_set_pin_drive_strength(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum), GPIO_DRV_STRENGTH_LEVEL3);
    rom_hw_gpio_set_pin_output_level(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum), gstLedParam.enLedOnLevel);
}

/**********************************************************************************************************************
 * @brief  LED timer irq handler 
 * @param  None. 
 * @output
 * @return None. 
 *********************************************************************************************************************/
void LED_TIMER_STIM_IRQ_HANDLER(void)
{
    uint16_t u16Flag;
    uint32_t u32Count;
    
    /* Get stim1 channel 2 interrupt flag and clear it. */
    rom_hw_stim_get_interrupt_flag(LED_TIMER, &u16Flag);
    rom_hw_stim_clear_interrupt_flag(LED_TIMER, (u16Flag & LED_TIMER_INT_MSK));

    rom_hw_stim_get_count(LED_TIMER, &u32Count);

    u32Count += TIME_MS_TO_32K_CNT(LED_TIMER_PERIOD);
    rom_hw_stim_set_compare(LED_TIMER, LED_TIMER_CH, u32Count);
    

    if(EN_LED_STATE_OFF == gstLedParam.enLedCurrentState)
    {
        rom_hw_gpio_set_pin_output_level(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum), !gstLedParam.enLedOnLevel);
        led_timer_int_disable();
        led_stop_event_callback( TIMEROUT_STOP, NULL, NULL );
    }
    
    else if(EN_LED_STATE_ON == gstLedParam.enLedCurrentState)
    {
        rom_hw_gpio_set_pin_output_level(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum), gstLedParam.enLedOnLevel);
    }
    
    else if(EN_LED_STATE_BLINK_WITH_TIMEOUT == gstLedParam.enLedCurrentState)
    {   
        gstLedParam.u16LedBlinkTimes--;
        gstLedParam.u32LedBlinkCount++;
        if( gstLedParam.u16LedBlinkFreq ==  gstLedParam.u32LedBlinkCount )
        {
            gstLedParam.u32LedBlinkCount = 0;
            rom_hw_gpio_toggle_pin_output_level(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum));
        }

        if( !gstLedParam.u16LedBlinkTimes )
        {
            if( EN_LED_STATE_OFF == gstLedParam.enLedLastState )
            {
                gstLedParam.enLedLastState = gstLedParam.enLedCurrentState;
                gstLedParam.enLedCurrentState = EN_LED_STATE_OFF;
            }
            else
            {
                gstLedParam.enLedCurrentState = gstLedParam.enLedLastState;
                gstLedParam.enLedLastState = EN_LED_STATE_BLINK_WITH_TIMEOUT;
            }
        }
    }
    
    else if(EN_LED_STATE_BLINK == gstLedParam.enLedCurrentState)
    {
        gstLedParam.u32LedBlinkCount++;
        if( gstLedParam.u16LedBlinkFreq ==  gstLedParam.u32LedBlinkCount )
        {
            gstLedParam.u32LedBlinkCount = 0;
            rom_hw_gpio_toggle_pin_output_level(GPION(gstLedParam.enLedPinNum), GPIO_PIN_N(gstLedParam.enLedPinNum));
        }
        
    }
    
    else if(EN_LED_STATE_ON_WITH_TIMEOUT == gstLedParam.enLedCurrentState)
    {
        gstLedParam.u32LedOnTimeTimes--;
        if( !(gstLedParam.u32LedOnTimeTimes) )
        {
            gstLedParam.enLedLastState = gstLedParam.enLedCurrentState;
            gstLedParam.enLedCurrentState = EN_LED_STATE_OFF;
        }
    }
}

/**********************************************************************************************************************
 * @brief  led init 
 * @param  led_event_callback_t: led event callback 
 * @output
 * @return EN_ERR_STA_T. 
 *********************************************************************************************************************/
EN_ERR_STA_T led_init( led_event_callback_t led_event_callback_cb )
{
    if( (NULL == led_event_callback_cb) )
    {
        return ERR_PARA_ERR;
    }
    
    led_stop_event_callback = led_event_callback_cb;
    
    gstLedParam.enLedPinNum = LED_PIN_NUM;
    gstLedParam.enLedOnLevel = LED_STATE_ON_LEVEL;
    gstLedParam.enLedCurrentState = EN_LED_STATE_OFF;

    //GPIO init
    led_gpio_init();

    // Config stim1 ch2 peripheral int
    led_timer_int_init();

    return ERR_STA_OK;
}

/**********************************************************************************************************************
 * @brief  led on 
 * @param  None. 
 * @output
 * @return EN_ERR_STA_T. 
 *********************************************************************************************************************/
EN_ERR_STA_T led_on( void )
{
    if( EN_LED_STATE_DISABLE == gstLedParam.enLedCurrentState )
    {
        return ERR_NOT_ALLOWED;
    }
    else
    {
        if( (EN_LED_STATE_ON_WITH_TIMEOUT == gstLedParam.enLedCurrentState) ||
            (EN_LED_STATE_OFF == gstLedParam.enLedCurrentState)    )
        {
            gstLedParam.enLedLastState = gstLedParam.enLedCurrentState;
            gstLedParam.enLedCurrentState = EN_LED_STATE_ON; 
        }
        else
        {
            return ERR_NOT_ALLOWED;
        }
    }
 

    //LED gpio enable
    led_gpio_enable( );
    //LED interrupt enable
    led_timer_int_enable();
    
    return ERR_STA_OK;
}

/**********************************************************************************************************************
 * @brief  led on with timeout 
 * @param  u32TimeMs: the time of led on, uint: ms.
 * @output
 * @return EN_ERR_STA_T. 
 *********************************************************************************************************************/
EN_ERR_STA_T led_on_with_timeout( uint32_t u32TimeMs)
{ 
    
    if( gstLedParam.enLedCurrentState == EN_LED_STATE_DISABLE )
    {
        return ERR_NOT_ALLOWED;
    }
    else
    {
        if(EN_LED_STATE_OFF == gstLedParam.enLedCurrentState)
        {
            gstLedParam.enLedLastState = gstLedParam.enLedCurrentState;
            gstLedParam.enLedCurrentState = EN_LED_STATE_ON_WITH_TIMEOUT;

            gstLedParam.u32LedOnTimeTimes = u32TimeMs / LED_TIMER_PERIOD;
        }
        else
        {
            return ERR_NOT_ALLOWED;
        }
    }

    //LED gpio enable
    led_gpio_enable();
    //LED interrupt enable
    led_timer_int_enable();
    
    return ERR_STA_OK;
}

/**********************************************************************************************************************
 * @brief  led blink 
 * @param  u16Freq: the frequency of led blink
 * @output
 * @return EN_ERR_STA_T. 
 *********************************************************************************************************************/
EN_ERR_STA_T led_blink(uint16_t u16Freq)
{
    if( !u16Freq )
    {
        return ERR_PARA_ERR;
    }

    if( EN_LED_STATE_DISABLE == gstLedParam.enLedCurrentState )
    {
        return ERR_NOT_ALLOWED;
    }
    else
    {
        if( EN_LED_STATE_BLINK_WITH_TIMEOUT != gstLedParam.enLedCurrentState )
        {
            gstLedParam.enLedLastState = gstLedParam.enLedCurrentState;
            gstLedParam.enLedCurrentState = EN_LED_STATE_BLINK;

            gstLedParam.u32LedBlinkCount= 0;
            gstLedParam.u16LedBlinkFreq = (1000 / u16Freq / LED_TIMER_PERIOD);  
        }       
        else
        {
            return ERR_NOT_ALLOWED;
        }
    }


    //LED gpio enable
    led_gpio_enable(  );
    //LED interrupt enable
    led_timer_int_enable();
    
    return ERR_STA_OK; 
}

/**********************************************************************************************************************
 * @brief  led blink with timeout
 * @param  u16Freq: the frequency of led blink
           u16FlashTimes: the time of led blink
 * @output
 * @return EN_ERR_STA_T. 
 *********************************************************************************************************************/
EN_ERR_STA_T led_blink_with_timeout(uint16_t u16Freq, uint16_t u16FlashTimes)
{
     if( !u16Freq || !u16FlashTimes )
    {
        return ERR_PARA_ERR;
    }


    if( gstLedParam.enLedCurrentState == EN_LED_STATE_DISABLE)
    {
        return ERR_NOT_ALLOWED;
    }
    else
    {
        gstLedParam.enLedLastState = gstLedParam.enLedCurrentState;
        gstLedParam.enLedCurrentState = EN_LED_STATE_BLINK_WITH_TIMEOUT;

        gstLedParam.u32LedBlinkCount= 0;
        gstLedParam.u16LedBlinkFreq = (1000 / u16Freq / LED_TIMER_PERIOD);  
        gstLedParam.u16LedBlinkTimes = u16FlashTimes / LED_TIMER_PERIOD; 
    }
    
    //LED gpio enable
    led_gpio_enable( );
    //LED interrupt enable
    led_timer_int_enable();
    
    return ERR_STA_OK;
}

/**********************************************************************************************************************
 * @brief  led off
 * @param  None.
 * @output
 * @return EN_ERR_STA_T. 
 *********************************************************************************************************************/
EN_ERR_STA_T led_off( void )
{
    if( EN_LED_STATE_DISABLE == gstLedParam.enLedCurrentState )
    {
        return ERR_NOT_ALLOWED;
    }
    else
    {
        gstLedParam.enLedLastState = gstLedParam.enLedCurrentState;
        gstLedParam.enLedCurrentState = EN_LED_STATE_OFF;

        led_stop_event_callback( MANUAL_STOP, NULL, NULL);
    }

    return ERR_STA_OK;
}




