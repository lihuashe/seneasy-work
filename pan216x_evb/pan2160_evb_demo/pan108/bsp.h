
#ifndef _BSP_H
#define _BSP_H

#include "base_types.h"

extern volatile uint32_t g_SystemTick;
#include "PanSeries.h"
#include "pan_gpio.h"

/* PIN definitions */
// LED
#define LED2_PIN P10
#define LED2_PORT P1
#define LED2_NUM 0

#define LED3_PIN P11
#define LED3_PORT P1
#define LED3_NUM 1

// TX_RX_SWITCH
#define TX_RX_SELECT P30
#define TX_SELECT 0
#define RX_SELECT 1

// TRIGGER
#define TRIGGER_PIN       P30

//KEY
#define KEY1_PIN P31
#define KEY2_PIN P01
#define KEY3_PIN P16
#define KEY4_PIN P17

#define KEY1_DOWN (P31 == 0)
#define KEY2_DOWN (P01 == 0)
#define KEY3_DOWN (P16 == 0)
#define KEY4_DOWN (P17 == 0)

#define KEY1_UP (P31 == 1)
#define KEY2_UP (P01 == 1)
#define KEY3_UP (P16 == 1)
#define KEY4_UP (P17 == 1)

//OLED
#define OLED_DC_PIN       P22
#define OLED_CS_PIN       P20
#define OLED_RST_PIN      P26
#define OLED_SCLK_PIN P21
#define OLED_SDIN_PIN P23
// SPI
#define SCK_PIN           P40
#define MOSI_PIN          P43
#define SDA_PIN           P43
#define CS_PIN            P41
#define MISO_PIN          P42
#define IRQ_PIN           P45

#define BSP_DELAY_2M()    __NOP();

#define BSP_DELAY_1M() \
    __NOP();           \
    __NOP();           \
    __NOP();           \
    __NOP();           \
    __NOP();           \
    __NOP();           \
    __NOP();           \
    __NOP();           \
    __NOP();           \
    __NOP();           \
    __NOP();

#define BSP_DELAY_700K() \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();

#define BSP_DELAY_400K() \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();

#define BSP_DELAY_200K() \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();             \
    __NOP();

void BSP_ClockInit(void);
void BSP_GpioInit(void);
void BSP_UartInit(void);
void BSP_Timer0Init(void);
void BSP_TriggerGpioInit(void);
void BSP_Led2Toggle(void);
void BSP_Led3Toggle(void);
void BSP_KeyInit(void);
void BSP_OLEDInit(void);
void BSP_IICInit(void);
void BSP_3LineSPIInit(void);
void BSP_4LineSPIInit(void);
void SYS_ConfigMFP(uint8_t PinID, uint32_t PinFunction);
void GPIO_SetModeByPin(uint8_t PinID, GPIO_ModeDef Mode);
void BSP_DelayMs(const uint32_t ms);
void BSP_DelayUs(const uint32_t us);

#endif
