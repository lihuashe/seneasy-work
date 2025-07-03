/*******************************************************************************
 * @note Copyright (C) 2023 Shanghai Panchip Microelectronics Co., Ltd. All rights reserved.
 *
 * @file display.c
 * @brief
 *
 * @history - V0.2, 2023-08-04
*******************************************************************************/
#include "oled.h"
//#include "radio.h"
#include "display.h"
#include "string.h"

#define X0 0
#define X1 8
#define X2 16
#define X3 24
#define X4 32
#define X5 40
#define X6 48
#define X7 56
#define X8 64
#define X9 72
#define X10 80
#define X11 88
#define X12 96
#define X13 104
#define X14 112
#define X15 120

#define Y0 0
#define Y1 16
#define Y2 28
#define Y3 40
#define FONT_SIZE 12


//8px*8px
const uint8_t icon_up[] = {
	0xf7, 0xe3, 0xd5, 0xb6, 0x77, 0xf7, 0xf7, 0xf7
};

//8px*8px
const uint8_t icon_down[] = {
0xf7, 0xf7, 0xf7, 0xf7, 0xb6, 0xd5, 0xe3, 0xf7
};

//8px*8px
const uint8_t icon_back[] = {
	0xff, 0xef, 0xdf, 0xbf, 0x00, 0xbf, 0xdf, 0xef
};

//8px*8px
const uint8_t icon_next[] = {
	0xf7, 0xfb, 0xfd, 0xfe, 0x00, 0xfe, 0xfd, 0xfb
};

//8px*8px
const uint8_t icon_return[] = {
    0xc3, 0xfd, 0xfe, 0xde, 0xbe, 0x01, 0xbf, 0xdf
};

//8px*8px
const uint8_t icon_circle[] = {
	0xc3, 0x99, 0x3c, 0x7e, 0x7e, 0x3c, 0x99, 0xc3
};

//8px*8px
const uint8_t icon_circle_dot[] = {
    0xc3, 0x99, 0x3c, 0x66, 0x66, 0x3c, 0x99, 0xc3
};

//16px*68px
const uint8_t arror[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xe7, 0xe0, 0x03,0xff, 0xe7, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

//64px*64px
const uint8_t panchip_icon[]= {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x08, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x3f, 0x7f, 0xff, 0xf7, 0xff, 
	0xff, 0xfe, 0x0c, 0x1f, 0xbf, 0xdf, 0xf3, 0xff, 0xff, 0xfe, 0x1e, 0x1f, 0x9f, 0xef, 0xf9, 0xff, 
	0xff, 0xfc, 0x1e, 0x0f, 0x9f, 0xe7, 0xf9, 0xff, 0xff, 0xfc, 0x3f, 0x0f, 0x9f, 0xe7, 0xf9, 0xff, 
	0xff, 0xfc, 0x3f, 0x0f, 0x3f, 0xc7, 0xf9, 0xff, 0xff, 0xf8, 0x3f, 0xfe, 0x7f, 0xcf, 0xf1, 0xff, 
	0xff, 0xf8, 0x7f, 0xf1, 0xff, 0x9f, 0xf3, 0xff, 0xff, 0xf0, 0x7f, 0xc7, 0xff, 0x3f, 0xe3, 0xff, 
	0xff, 0xf0, 0xfe, 0x7f, 0xfc, 0x7f, 0xc7, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf9, 0xff, 0x8f, 0xff, 
	0xff, 0xe1, 0xff, 0xff, 0xc7, 0xfe, 0x1f, 0xff, 0xff, 0xe1, 0xff, 0xff, 0x1f, 0xfc, 0x7f, 0xff, 
	0xff, 0xc1, 0xff, 0xf8, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xcf, 0xff, 0xc3, 0xff, 0xff, 
	0xff, 0xc3, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 
	0xff, 0x87, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x07, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 
	0xff, 0x0f, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 
	0xfe, 0x1f, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0x3f, 0xef, 0xff, 0xff, 0xff, 
	0xfc, 0x1f, 0xff, 0xff, 0x07, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 
	0xfc, 0x3f, 0xff, 0xff, 0x83, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 
	0xf8, 0x7f, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xf8, 0x7b, 0xb4, 0x2d, 0x43, 0xff, 0xff, 0xff, 0xfb, 0xb1, 0x95, 0xed, 0x5d, 0xff, 0xff, 0xff, 
	0xfb, 0xa5, 0x85, 0xe1, 0x59, 0xff, 0xff, 0xff, 0xf8, 0x60, 0xa5, 0xed, 0x43, 0xff, 0xff, 0xff, 
	0xfb, 0xce, 0xb4, 0x2d, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const OptionName_t common_menu[Options_Max] = {
    {"Tx/Rx Mode",
     {"Tx 1-Packet", "Tx 1000-Packet", "Tx Continuous", "Rx Continuous", "Tx Carrier Wave", "Sleep",
      ""}},
    {"Chip Mode", {"297L", 
"FS01", "FS32",
    "BLE", ""}},
    {"Tx Power Level",
     {"0dBm", "1dBm", "2dBm", "3dBm", "4dBm", "5dBm", "6dBm", "7dBm", "8dBm", "9dBm", ""}},
    {"Data Rate", {"1Mbps", "2Mbps", "250Kbps", ""}},

    {"Frequency Channel",
     {"2400MHz", "2402MHz", "2404MHz", "2406MHz", "2408MHz", "2410MHz", "2412MHz", "2414MHz",
      "2416MHz", "2418MHz", "2420MHz", "2422MHz", "2424MHz", "2426MHz", "2428MHz", "2430MHz",
      "2432MHz", "2434MHz", "2436MHz", "2438MHz", "2440MHz", "2442MHz", "2444MHz", "2446MHz",
      "2448MHz", "2450MHz", "2452MHz", "2454MHz", "2456MHz", "2458MHz", "2460MHz", "2462MHz",
      "2464MHz", "2466MHz", "2468MHz", "2470MHz", "2472MHz", "2474MHz", "2476MHz", "2478MHz",
      "2480MHz", "2482MHz", "2484MHz", "2485MHz", ""}},

    {"Work Mode", {"Normal", "Enhance", ""}},
    {"CRC Select", {"Disable", "CRC-8", "CRC-16", "CRC-24", ""}},
    {"Dynamic Payload", {"Disable", "Enable", ""}},
    {"Tx No-Ack", {"Disable", "Enable", ""}},
    {"Whiten/Scramble", {"Disable", "Enable", ""}},
    {"Payload Length", {"32", "64", "128", ""}},

};

const OptionName_t ble_menu[BLE_Options_Max] = {
    {"Tx/Rx Mode",
     {"Tx 1-Packet", "Tx 1000-Packet", "Tx Continuous", "Rx Continuous", "Tx Carrier Wave", "Sleep",
      ""}},
    {"Chip Mode", {"297L", 
"FS01", "FS32",
    "BLE", ""}},

    {"Tx Power Level",
     {"0dBm", "1dBm", "2dBm", "3dBm", "4dBm", "5dBm", "6dBm", "7dBm", "8dBm", "9dBm", ""}},
    {"Data Rate", {"1Mbps", "2Mbps", "250Kbps", ""}},
    {"Channel",
     {"BLE_CH_37", "BLE_CH_38", "BLE_CH_39", "2400MHz", "2402MHz", "2404MHz", "2406MHz", "2408MHz",
      "2410MHz",   "2412MHz",   "2414MHz",   "2416MHz", "2418MHz", "2420MHz", "2422MHz", "2424MHz",
      "2426MHz",   "2428MHz",   "2430MHz",   "2432MHz", "2434MHz", "2436MHz", "2438MHz", "2440MHz",
      "2442MHz",   "2444MHz",   "2446MHz",   "2448MHz", "2450MHz", "2452MHz", "2454MHz", "2456MHz",
      "2458MHz",   "2460MHz",   "2462MHz",   "2464MHz", "2466MHz", "2468MHz", "2470MHz", "2472MHz",
      "2474MHz",   "2476MHz",   "2478MHz",   "2480MHz", "2482MHz", "2484MHz", "2485MHz", ""}},
    {"S2/S8 Mode", {"Disable", "S2", "S8", ""}},

};

void OLED_FILL(int i, int j, int w, int h, uint8_t *data)
{
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            OLED_DrawPoint(i + x, j + y, (~data[(x + y * w) / 8] & (0x80 >> (x % 8))));
        }
    }
}

void show_header(uint8_t * icon1, uint8_t * icon2, uint8_t* icon3, uint8_t * icon4)
{
    OLED_ShowString(X0+4, Y0, "PAN216x EVB GUI V1.0", 12);
    for(int i = 0; i < 128; i ++)
    {
        OLED_DrawPoint(i, 14, 1);
        OLED_DrawPoint(i, 54, 1);
    }
    if (icon1)
        OLED_FILL(X0, 56, 8, 8, icon1);
    if (icon2)
        OLED_FILL(40, 56, 8, 8, icon2);
    if (icon3)
        OLED_FILL(80, 56, 8, 8, icon3);
    if (icon4)
        OLED_FILL(119, 56, 8, 8, icon4);
}

void OLED_PaintFrame(int x, int y, int w, int h)
{
    for (int i = 0; i < w; i++)
    {
        OLED_DrawPoint(x + i, y, 1);
        OLED_DrawPoint(x + i, y + h, 1);
    }
    for (int i = 0; i < h; i++)
    {
        OLED_DrawPoint(x, y + i, 1);
        OLED_DrawPoint(x + w, y + i, 1);
    }
}


static Context_t* this = NULL;


void context_init(Context_t* context)
{
    this = context;
}

void context_set_option()
{
    //this->chosen_index[this->current_option] = this->current_index;
}

void context_set_page(enum Page page)
{
    this->current_page = page;
}

static void show_menu(void);
static void show_options(void);
static void show_tx(void);
static void show_rx(void);
static void show_carrier_wave(void);
static void show_sleep(void);
static void show_tx_refresh_counter(void);
static void show_rx_refresh_counter(void);
void context_update_gui(void)
{
    if (this->current_page == MENU)
    {
        show_menu();
    }
    else if (this->current_page == OPTIONS)
    {
        show_options();
    }
    else if (this->current_page == TX)
    {
        show_tx();
    }
    else if (this->current_page == RX)
    {
        show_rx();
    }
    else if (this->current_page == CW)
    {
        show_carrier_wave();
    }
    else if (this->current_page == SLEEP)
    {
        show_sleep();
    }
}


void context_update_test_counter()
{
    if (this->current_page == TX)
    {
        show_tx_refresh_counter();
    }
    else if (this->current_page == RX)
    {
        show_rx_refresh_counter();
    }
    else if (this->current_page == CW)
    {
        show_carrier_wave();
    }
    else if (this->current_page == SLEEP)
    {
        show_sleep();
    }
}

static void show_menu(void)
{
    int index = this->current_option;
    int prev, next = 0;
    if (index == 0)
        prev = this->current_menu_size - 1;
    else
        prev = index - 1;
    if (index == this->current_menu_size - 1)
        next = 0;
    else
        next = index + 1;
    OLED_Clear();
    show_header((uint8_t *)icon_return, (uint8_t*)icon_up, (uint8_t *)icon_down, (uint8_t *)icon_next);
    OLED_ShowNum(X0, 16, prev, 2, 12);
    OLED_ShowNum(X0, 28, index, 2, 12);
    OLED_ShowNum(X0, 40, next, 2, 12);
    OLED_ShowString(X2, 16, this->current_menu[prev].name, 12);
    OLED_ShowString(X2, 28, this->current_menu[index].name, 12);
    OLED_ShowString(X2, 40, this->current_menu[next].name, 12);
    OLED_Reverse(X0, 40, 128, 12);
    OLED_Refresh_Gram();
}


static void show_options(void)
{
    OLED_Clear();
    show_header((uint8_t *)icon_return, (uint8_t*)icon_up, (uint8_t *)icon_down, (uint8_t *)icon_next);
    OLED_ShowString(X0, 16, this->current_menu[this->current_option].name, FONT_SIZE);
    OLED_ShowString(X0 + 6 * strlen(this->current_menu[this->current_option].name), Y1 ,":", FONT_SIZE);
    OLED_ShowString(X3, Y2, this->current_menu[this->current_option].options[this->current_index], FONT_SIZE);
    OLED_Refresh_Gram();
}

void show_welcome(void)
{
    OLED_Clear();
    OLED_FILL(70, 16, 64, 64, (uint8_t*)panchip_icon);
    OLED_ShowString(0, 16, "PAN216x-EVB", FONT_SIZE);
    OLED_ShowString(0, 40, "V1.0 ", FONT_SIZE);
    OLED_Refresh_Gram();
}


static void show_tx(void)
{
    OLED_Clear();
    OLED_ShowString(80, 52, "clr", FONT_SIZE);
    if (this->test_info.is_on == 0)
    {
        show_header((uint8_t *)icon_return, NULL, NULL, (uint8_t *)icon_circle);
    }
    else
    {
        show_header((uint8_t *)icon_return, NULL, NULL, (uint8_t *)icon_circle_dot);
    }

    char s[30] = "";
    strcat(s, this->current_menu[Channel].options[this->chosen_index[Channel]]);
    strcat(s, " ");
    strcat(s, this->current_menu[TxPower].options[this->chosen_index[TxPower]]);
    strcat(s, " ");
    strcat(s, this->current_menu[Datarate].options[this->chosen_index[Datarate]]);
    strcat(s, " ");
    while(strlen(s) < 21)
    {
        strcat(s, " ");
    }
    OLED_ShowString(X0, Y1, s, FONT_SIZE);
    OLED_ShowString(X0, Y2,"tx:", FONT_SIZE);
    OLED_ShowString(60, Y2, "crcerr:", FONT_SIZE);
    OLED_ShowString(0, Y3, "ack:", FONT_SIZE);
    OLED_ShowString(68, Y3, "RSSI:-", FONT_SIZE);
    OLED_ShowNum(20, Y2, this->test_info.tx_cnt, 4, FONT_SIZE);
    OLED_ShowNum(104, Y2, this->test_info.crc_error_cnt, 4, FONT_SIZE);
    OLED_ShowNum(24, Y3, this->test_info.rx_cnt, 4, FONT_SIZE);
    OLED_ShowNum(104, Y3, this->test_info.rssi, 4, FONT_SIZE);
    OLED_Refresh_Gram();
}



static void show_tx_refresh_counter(void)
{
    OLED_ShowNum(20, Y2, this->test_info.tx_cnt, 4, FONT_SIZE);
    OLED_ShowNum(104, Y2, this->test_info.crc_error_cnt, 4, FONT_SIZE);
    OLED_ShowNum(24, Y3, this->test_info.rx_cnt, 4, FONT_SIZE);
    OLED_ShowNum(104, Y3, this->test_info.rssi, 4, FONT_SIZE);
    OLED_Refresh_Gram();
}

static void show_rx_refresh_counter(void)
{
    OLED_ShowNum(20, Y2, this->test_info.rx_cnt, 4, FONT_SIZE);
    OLED_ShowNum(96, Y3, this->test_info.crc_error_cnt, 4, FONT_SIZE);
    OLED_ShowNum(96, Y2, this->test_info.rssi, 4, FONT_SIZE);
    OLED_Refresh_Gram();
}

static void show_carrier_wave(void)
{
    OLED_Clear();
    if (this->test_info.is_on== 0)
    {
        show_header((uint8_t *)icon_return, NULL, NULL, (uint8_t *)icon_circle);
        OLED_ShowString(X0, Y2, "Carrier Wave Stopped", FONT_SIZE);
    }
    else
    {
        show_header((uint8_t *)icon_return, NULL, NULL, (uint8_t *)icon_circle_dot);
        OLED_ShowString(X0, Y2, "Sending Carrier Wave", FONT_SIZE);
    }
    char s[30] = "";
    strcat(s, this->current_menu[Channel].options[this->chosen_index[Channel]]);
    strcat(s, " ");
    strcat(s, this->current_menu[TxPower].options[this->chosen_index[TxPower]]);
    strcat(s, " ");
    while(strlen(s) < 21)
    {
        strcat(s, " ");
    }
    OLED_ShowString(X0, Y1, s, FONT_SIZE);
    OLED_Refresh_Gram();
}

static void show_rx(void)
{
    OLED_Clear();
    OLED_ShowString(80, 52, "clr", FONT_SIZE);
    if (this->test_info.is_on == 0)
    {
        show_header((uint8_t *)icon_return, NULL, NULL, (uint8_t *)icon_circle);
    }
    else
    {
        show_header((uint8_t *)icon_return, NULL, NULL, (uint8_t *)icon_circle_dot);
    }


    char s[30] = "";
    strcat(s, this->current_menu[Channel].options[this->chosen_index[Channel]]);
    strcat(s, " ");
    strcat(s, this->current_menu[Datarate].options[this->chosen_index[Datarate]]);
    strcat(s, " ");
    while(strlen(s) < 21)
    {
        strcat(s, " ");
    }
    OLED_ShowString(X0, Y1, s, FONT_SIZE);
    OLED_ShowString(X0, Y2,"rx:", FONT_SIZE);
    OLED_ShowString(56, Y2, "RSSI:-", FONT_SIZE);
    OLED_ShowString(0, Y3, "crcerr:", FONT_SIZE);
    OLED_ShowNum(20, Y2, this->test_info.rx_cnt, 4, FONT_SIZE);
    OLED_ShowNum(96, Y2, this->test_info.rssi, 4, FONT_SIZE);
    OLED_ShowNum(96, Y3, this->test_info.crc_error_cnt, 4, FONT_SIZE);
    OLED_Refresh_Gram();
}

static void show_sleep(void)
{
    OLED_Clear();
    if (this->test_info.is_on== 0)
    {
        show_header((uint8_t *)icon_return, NULL, NULL, (uint8_t *)icon_circle);
        OLED_ShowString(X0, Y2, "Sleep Mode Stopped", FONT_SIZE);
    }
    else
    {
        show_header((uint8_t *)icon_return, NULL, NULL, (uint8_t *)icon_circle_dot);
        OLED_ShowString(X0, Y2, "Sleeping Now      ", FONT_SIZE);
    }
    
    OLED_Refresh_Gram();
}

void show_disconnect(void)
{
    OLED_Clear();
    show_header(NULL, NULL, NULL, NULL);
    OLED_ShowString(X0, Y2, "Chip Disconnected", FONT_SIZE);
    OLED_Refresh_Gram();
}
