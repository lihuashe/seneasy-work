/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     SMARTRCU_service.h
  * @brief    Head file for using SMARTRCU Service Remote Controller.
  * @details  SMARTRCU service data structs and external functions declaration.
  * @author   Chenjie Jin
  * @date     2018-5-7
  * @version  v1.1
  * *************************************************************************************
  */

#ifndef _SMARTRCU_HANDLE_H_
#define _SMARTRCU_HANDLE_H_
#include "profile_server.h"
#include "board.h"
#include "ir_app_config.h"
#include "key_handle.h"
#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */
/* smart rcu command define */
#define MAX_SMARTRCU_KEY  4

#define KEY_POWER       0x21
#define KEY_NETFLIX     0x63
#define KEY_YouTube     0x64


typedef enum
{
    INDEX_POWER,
    INDEX_NETFLIX,
    INDEX_YouTube               
} T_SMARTRCU_KEY_INDEX_DEF;

typedef enum
{
    COMMAOND_IR_CODE_DISCRIBE          = 0x01,
    COMMAOND_IR_CODE_FIRST             = 0x02,
    COMMAOND_IR_INVERSE_CODE           = 0x03,
    COMMAOND_IR_CODE_SEND_END          = 0x04,
    COMMAOND_REPORT_RICIEVE_DATA_STATE = 0x05,
    COMMAOND_IR_KEY_INDEX              = 0x10,
    COMMAOND_IR_BLE_CHANGE             = 0x11,
    COMMAOND_READ_RCU_KEY_STATE        = 0x12,
    COMMAOND_REPORT_RCU_KEY_STATE      = 0x13,
    COMMAOND_READ_KEY_NUMBER           = 0xA0,
    COMMAOND_READ_ALL_STUDY_KEY_NUMBER = 0xA1,
    COMMAOND_READ_IR_CODE_DISCRIBE     = 0xA2,
    COMMAOND_READ_IR_CODE_DATA         = 0xA3,
    COMMAOND_READ_IR_INVERSE_CODE_DATA = 0xA4,
    COMMAOND_READ_CODE_STATE           = 0xA5,
    COMMAOND_RESTORE_FACTORY_SETTING   = 0xB0,
} T_SMARTRCU_COMMAND_TYPE;

typedef enum
{
    SEND_BLE_KEY = 0x01,
    SEND_IR_KEY  = 0x02,
    SEND_ALL_KEY = 0x03
} T_SMARTRCU_KEY_CONTROL_TYPE;

typedef struct
{
    uint8_t key_code;
    uint8_t key_code_type;
} T_SMARTRCU_KEY_SEND_STATE;

typedef enum
{
    WAVE_DATA_LEN_ERROR    = 0x01,
    WAVE_DATA_CRC_ERROR    = 0x02,
    INVERSE_DATA_LEN_ERROR = 0x04,
    INVERSE_DATA_CRC_ERROR = 0x08, 
    KEY_NOT_SUPPORT_ERROR  = 0x10
} T_SMARTRCU_DATA_MASK;

typedef enum
{
    CLEAR_ALL_INFO   = 0x01,
    CLEAR_PAIR_INFO  = 0x02,
    CLEAR_IR_INFO    = 0x03
} T_SMARTRCU_RESET_TYPE;
typedef struct
{
    uint8_t  data_number; 
    uint8_t  data[18];
} T_SMARTRCU_WAVE_DATA;

typedef struct
{
    uint8_t packet_number;
    uint8_t packet_inve_number;
    T_SMARTRCU_WAVE_DATA  smartrcu_wave_data[40]; 
    T_SMARTRCU_WAVE_DATA  smartrcu_inve_data[40];
} T_SMARTRCU_WAVE_PACKET_DATA;

typedef struct
{
    uint8_t  data_len;
    uint16_t code_group_number;
    uint16_t  first_wave_len;
    uint16_t  first_wave_crc;
    uint16_t  inve_code_len;
    uint16_t  inve_code_crc;
} T_SMARTRCU_IR_CODE_DISCRIBE;

typedef struct
{
    uint8_t send_smartrcu_ir_state;
    uint8_t inve_code_flag;
    T_SMARTRCU_KEY_SEND_STATE key_send_data[MAX_SMARTRCU_KEY];
} T_SMARTRCU_GLOBAL_DATA;

typedef struct
{
    uint8_t ir_key_index;
    uint8_t inve_ir_key_index;   
    uint8_t freq;
    uint16_t ir_buff_index; 
    uint32_t ir_buffer[IR_SEND_WAVE_MAX_SIZE];
    uint16_t inve_ir_buff_index;   
    uint32_t inve_ir_buffrt[IR_SEND_WAVE_MAX_SIZE];
}T_SMARTRCU_IR_KEY_DATA;

extern T_SMARTRCU_IR_KEY_DATA smartrcu_ir_key_data;
extern T_SMARTRCU_GLOBAL_DATA smartrcu_global_data;

void smartrcu_command_handle(uint8_t *p_smartrcu_fifo_data);
void smartrcu_data_init(void);
void smartrcu_key_index_convert(uint8_t key_index);
void smartrcu_key_index_inservion_convert(uint8_t key_index);
void smartrcu_data_reset(void);
void smartrcu_wave_send_ir(uint8_t key_index);
#endif
