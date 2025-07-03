/*******************************************************************************
 * @note Copyright (C) 2023 Shanghai Panchip Microelectronics Co., Ltd. All rights reserved.
 *
 * @file radio.c
 * @brief
 *
 * @history - V0.2, 2023-08-04
*******************************************************************************/
#ifndef __DISPLAY_H_
#define __DISPLAY_H_
#include "stdint.h"
#include "oled.h"

// Page->Option->Index


enum Page
{
    MENU,
    OPTIONS,
    TX,
    RX,
    CW,
    SLEEP,
};

typedef struct
{
    char *name;
    char *options[50];
} OptionName_t;

enum Option
{
    TxRxMode = 0,
    ChipMode = 1,
    TxPower = 2,
    Datarate = 3,
    Channel = 4,
    WorkMode = 5,
    S2S8Mode = 5,
    BLE_Options_Max = 6,
    CRC = 6,
    EnDPL,
    EnTxNoAck,
    EnWhiten,
    PayloadLenth,
    Options_Max,
};

typedef struct{
    int tx_cnt;
    int rx_cnt;
    int crc_error_cnt;
    int is_on;
    int rssi;
}TestInfo_t;


typedef struct
{
    enum Page current_page;
    enum Option current_option;
    uint8_t current_index;
    uint8_t chosen_index[Options_Max];
    TestInfo_t test_info;
    OptionName_t *current_menu;
    uint8_t current_menu_size;
} Context_t;

extern const OptionName_t common_menu[];
extern const OptionName_t ble_menu[];

void context_init(Context_t* context);
void context_set_option(void);
void context_update_gui(void);
void context_update_test_counter(void);
void show_welcome(void);
void show_disconnect(void);
#endif
