#ifndef __CLI_H
#define __CLI_H

#include "stdint.h"
#include "stdbool.h"
#include "uart.h"

#include "ir_module\ir_module.h"

typedef enum
{
    CMD_CONFIG_IR_NEC       = 0x81,
    CMD_SEND_NEC_RAW        = 0x82,
    CMD_STOP_KEY            = 0x83,
    CMD_SEND_NEC_KEY        = 0x84,
    CMD_SEND_IR_WAVE   = 0x90,
    CMD_SET_WAVE_FRAMES    = 0x91,

    CMD_READ_STATUS         = 0x00,
    CMD_READ_IR_NEC         = 0x01,
    CMD_READ_VERSION        = 0x02,
    CMD_READ_IR_SPECIAL     = 0x10,
}cli_cmd_t;

typedef struct  
{
    uint8_t cmd;
    uint8_t len;
    uint8_t params[64];
}cli_packet_t;

void cli_init();

void cli_task();

#endif
