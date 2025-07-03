/*******************************************************************************
 * @note Copyright (C) 2023 Shanghai Panchip Microelectronics Co., Ltd. All rights reserved.
 *
 * @file radio.c
 * @brief
 *
 * @history - V0.2, 2023-08-04
*******************************************************************************/
#ifndef __FRAME_H_
#define __FRAME_H_

#include "stdint.h"
#include "PanSeries.h"


typedef enum
{
		RF_SEND_POWER_STOP,
		RF_SEND_MAIN_FUN,
		RF_SEND_BABY_FUN,
		RF_SEND_SUB_FUN,
		RF_SEND_AIR_SET,
		RF_SEND_NETWORK_CONFIG,
		RF_SEND_LIGHT,
		RF_SEND_WIFI,
		RF_SEND_VOICE,
		RF_SEND_SMELL,
		RF_SEND_BLUETOOTH_MUSIC,
		RF_SEND_FILTER,
} RfSendType;

/* ??????? -----------------------------------------------*/
typedef struct
{
		/* ???? */
		uint8_t main_mode;      // ????
		uint8_t fan_speed;      // ??
		uint8_t air_direction;  // ??
		uint8_t air_type;       // ??
		uint8_t time_setting;   // ????
		uint8_t remain_hours;   // ????
		uint8_t remain_minutes; // ????

		/* ????? */
		uint8_t light_ctrl; // ????
		uint8_t wifi_ctrl;  // WiFi??
		uint8_t voice_ctrl; // ????

		/* ???? */
		uint8_t model_type; // ????
		uint8_t reserved;   // ??
} __attribute__((packed)) RemoteControlData_A7535;

typedef struct
{
		/* ???? */
		uint8_t main_mode;      // ????
		uint8_t fan_speed;      // ??
		uint8_t air_direction;  // ??
		uint8_t air_type;       // ??
		uint8_t time_setting;   // ????
		uint8_t remain_hours;   // ????
		uint8_t remain_minutes; // ????

		/* ????? */
		uint8_t light_ctrl; // ????
		uint8_t wifi_ctrl;  // WiFi??
		uint8_t voice_ctrl; // ????

		/* ???? */
		uint8_t model_type;      // ????
		uint8_t smell;           // ??
		uint8_t bluetooth_music; // ????
		uint8_t filter;          // ????
		uint8_t reserved[4];     // ??
} __attribute__((packed)) RemoteControlData_XN297L, RemoteControlData;


typedef struct
{
		uint8_t preamble_fixed[6];
		uint8_t id[4];          // id
		uint8_t unique_id[4];   // unique id
		uint8_t placeholder[6]; // ???

		uint8_t model_type; // ????
		uint8_t reserved;   // ??
		uint8_t checksum;   // ???
} __attribute__((packed)) PairPayload_A7535;
typedef struct
{
		uint8_t preamble_fixed[6];
		uint8_t id[4];          // id
		uint8_t unique_id[4];   // unique id
		uint8_t placeholder[6]; // ???

		uint8_t model_type;  // ????
		uint8_t reserved[7]; // ??
		uint8_t checksum;    // ???
} __attribute__((packed)) PairPayload_XN297L, PairPayload;
/* ????? -----------------------------------------------*/

typedef struct
{
		uint8_t preamble_fixed[6];
		uint8_t unique_id[4]; // unique id
		RemoteControlData_A7535 ctrl_data;
		uint8_t checksum; // ???
} __attribute__((packed)) CtrlPayload_A7535;

typedef struct
{
		uint8_t preamble_fixed[6];
		uint8_t unique_id[4]; // unique id
		RemoteControlData_XN297L ctrl_data;
		uint8_t checksum;
} __attribute__((packed)) CtrlPayload_XN297L, CtrlPayload;

#endif
