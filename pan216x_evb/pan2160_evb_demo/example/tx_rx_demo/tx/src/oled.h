/*******************************************************************************
 * @note Copyright (C) 2023 Shanghai Panchip Microelectronics Co., Ltd. All rights reserved.
 *
 * @file radio.c
 * @brief
 *
 * @history - V0.2, 2023-08-04
*******************************************************************************/
#ifndef __OLED_H_
#define __OLED_H_

#include "stdint.h"
#include "PanSeries.h"


typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,char *p,uint8_t size);
void OLED_ShowStringRev(uint8_t x, uint8_t y, char *p, uint8_t size);
void OLED_Reverse(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

#endif
