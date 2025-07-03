/*******************************************************************************
 * @note Copyright (C) 2023 Shanghai Panchip Microelectronics Co., Ltd. All rights reserved.
 *
 * @file oled.c
 * @brief
 *
 * @history - V0.2, 2023-08-04
*******************************************************************************/
#include "oled.h"
#include "oledfont.h"
#include "pan216.h"
#define OLED_DC(n)              (P22 = n)
#define OLED_CS(n)              (P20 = n)
#define OLED_RST(n)             (P26 = n)
#define OLED_SCLK(n)			(P21 = n)
#define OLED_SDIN(n)			(P23 = n)

#define Disable     0
#define Enable      1
#define OLED_CMD        		Disable
#define OLED_DATA       		Enable

/* memory */
static uint8_t OLED_GRAM[128][8];

/**
 * @brief write one byte in SSD1306
 * @param[in] <dat> data or order
 * @param[in] <cmd> 1:data 0:order
 * @return result
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    uint8_t i;
    OLED_DC(cmd);

    OLED_CS(Disable);
    for (i = 0; i < 8; i++)
    {
        OLED_SCLK(Disable);
        if (dat & 0x80)
            OLED_SDIN(Enable);
        else
            OLED_SDIN(Disable);
        OLED_SCLK(Enable);
        dat <<= 1;
    }
    OLED_CS(Enable);
    OLED_DC(Enable);
}

/**
 * @brief Refresh OLED after wright memory
 * @param[in] <none>
 * @return none
 */
void OLED_Refresh_Gram(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);
        OLED_WR_Byte(0x00, OLED_CMD);
        OLED_WR_Byte(0x10, OLED_CMD);
        for (n = 0; n < 128; n++)
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
    }
}


void OLED_Refresh_GramPart(void)
{
    uint8_t i, n;
    i = 0;
    OLED_WR_Byte(0xb0 + i, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0x10, OLED_CMD);
    for (n = 0; n < 128; n++)
        OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
}

/**
 * @brief turn on the OLED
 * @param[in] <none>
 * @return none
 */
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD);
    OLED_WR_Byte(0X14, OLED_CMD);
    OLED_WR_Byte(0XAF, OLED_CMD);
}

/**
 * @brief turn off the OLED
 * @param[in] <none>
 * @return none
 */
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD);
    OLED_WR_Byte(0X10, OLED_CMD);
    OLED_WR_Byte(0XAE, OLED_CMD);
}

/**
 * @brief clear the OLED configuration to initialize
 * @param[in] <none>
 * @return none
 after clear the OLED will be black
 */
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
        for (n = 0; n < 128; n++)
            OLED_GRAM[n][i] = 0x00;
    OLED_Refresh_Gram();
}

/**
 * @brief weight memory
 * @param[in] <x> abscissa axis
 * @param[in] <y> vertical axis
 * @param[in] <t> 1:point 0:no point
 * @return none
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t pos, bx, temp = 0;
    if (x > 127 || y > 63)
        return;
    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
    if (t)
        OLED_GRAM[x][pos] |= temp;
    else
        OLED_GRAM[x][pos] &= ~temp;
}

void OLED_ReversePoint(uint8_t x, uint8_t y)
{
    uint8_t pos, bx, temp = 0;
    if (x > 127 || y > 63)
        return;
    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
//    uint8_t t;
    if (OLED_GRAM[x][pos] & temp)
        OLED_GRAM[x][pos] &= ~temp;
    else
        OLED_GRAM[x][pos] |= temp;

}
/**
 * @brief paint  area
 * @param[in] <x1> abscissa axis of left
 * @param[in] <y1> vertical axis of left
 * @param[in] <x2> abscissa axis of right
 * @param[in] <y2> vertical axis of right
 * @param[in] <dot> 1:point 0:no point
 * @return none
 */
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;
    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)
            OLED_DrawPoint(x, y, dot);
    }
    OLED_Refresh_Gram();
}

/**
 * @brief show a char on OLED
 * @param[in] <x> abscissa axis
 * @param[in] <y> vertical axis
 * @param[in] <char> char
 * @param[in] <size> size of word 12 0r 16 or 24
 * @param[in] <mode> show mode  0:with shadow 1:without shadow
 * @return result
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';
    for (t = 0; t < csize; t++)
    {
        if (size == 12)
            temp = asc2_1206[chr][t];
        else if (size == 16)
            temp = asc2_1608[chr][t];
        else if (size == 24)
            temp = asc2_2412[chr][t];
        else
            return;
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
                OLED_DrawPoint(x, y, mode);
            else
                OLED_DrawPoint(x, y, !mode);
            temp <<= 1;
            y++;
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**
 * @brief calculate m^n
 * @param[in] <m>
 * @param[in] <n>
 * @return result
 */
uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}

/**
 * @brief show number on OLED
 * @param[in] <x> abscissa axis
 * @param[in] <y> vertical axis
 * @param[in] <num> number (0~4294967295)
 * @param[in] <len> length of number
 * @param[in] <size> size of word 16 0r 12
 * @return result
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size / 2) * t, y, '0', size, 1);
                continue;
            }
            else
                enshow = 1;
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
    }
}

/**
 * @brief show string on OLED
 * @param[in] <x> abscissa axis
 * @param[in] <y> vertical axis
 * @param[in] <p> string
 * @param[in] <size> size of word 16 0r 12
 * @return result
 */
void OLED_ShowString(uint8_t x, uint8_t y, char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))
    {
        if (x > (128 - (size / 2)))
        {
            x = 0;
            y += size;
        }
        if (y > (64 - size))
        {
            y = x = 0;
            OLED_Clear();
        }
        OLED_ShowChar(x, y, *p, size, 1);
        x += size / 2;
        p++;
    }
}

void OLED_Reverse(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t i, j;
    for (i = x; i < x + width; i++)
    {
        for (j = (y - height); j < y; j++)
        {
            OLED_ReversePoint(i, j);
        }
    }
}

void OLED_ShowStringRev(uint8_t x, uint8_t y, char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))
    {
        if (x > (128 - (size / 2)))
        {
            x = 0;
            y += size;
        }
        if (y > (64 - size))
        {
            y = x = 0;
            OLED_Clear();
        }
        OLED_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}
/**
 * @brief do basic configuration to initialize
 * @param[in] <none>
 * @return none
 */
void OLED_Init(void)
{
    OLED_CS(Enable);
    OLED_RST(Enable);
    BSP_DelayUs(10);
    OLED_RST(Disable);
    BSP_DelayUs(100);
    OLED_RST(Enable);
    BSP_DelayUs(10);

    OLED_WR_Byte(0xAE, OLED_CMD);
    OLED_WR_Byte(0xD5, OLED_CMD);
    OLED_WR_Byte(80, OLED_CMD);
    OLED_WR_Byte(0xA8, OLED_CMD);
    OLED_WR_Byte(0X3F, OLED_CMD);
    OLED_WR_Byte(0xD3, OLED_CMD);
    OLED_WR_Byte(0X00, OLED_CMD);
    OLED_WR_Byte(0x40, OLED_CMD);

    OLED_WR_Byte(0x8D, OLED_CMD);
    OLED_WR_Byte(0x14, OLED_CMD);
    OLED_WR_Byte(0x20, OLED_CMD);
    OLED_WR_Byte(0x02, OLED_CMD);
    OLED_WR_Byte(0xA1, OLED_CMD);
    OLED_WR_Byte(0xC0, OLED_CMD);
    OLED_WR_Byte(0xDA, OLED_CMD);
    OLED_WR_Byte(0x12, OLED_CMD);

    OLED_WR_Byte(0x81, OLED_CMD);
    OLED_WR_Byte(0xEF, OLED_CMD);
    OLED_WR_Byte(0xD9, OLED_CMD);
    OLED_WR_Byte(0xf1, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD);
    OLED_WR_Byte(0x30, OLED_CMD);

    OLED_WR_Byte(0xA4, OLED_CMD);
    OLED_WR_Byte(0xA6, OLED_CMD);
    OLED_WR_Byte(0xAF, OLED_CMD);
    OLED_Clear();
}
