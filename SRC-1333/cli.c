/**
 * @attention
 * 
 *  Copyright 2024 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
 * @file     cli.c
 * 
 * @author   张凯华(zhangkaihua@seneasy.com)
 * 
 * @brief    
 * 
 * @date     2024-11-20
 * 
*/

#include "cli.h"

cli_packet_t response;

void cli_init()
{

}

inline uint8_t cli_ack(bool success)
{
    response.cmd = rx_data[2];
    response.len = 1;
    response.params[0] = success;
    return 3;
}

inline uint8_t cli_get_status()
{
    response.cmd = 0x00;
    response.len = 1;
    if ((ir_data.state == IR_STATE_IDLE || ir_data.state == IR_STATE_REPEAT)&& ir_wave.state == IR_STATE_IDLE)
    {
        response.params[0] = 0;
    }
    else
    {
        response.params[0] = 1;
    }
    return 3;
}

inline uint8_t cli_get_version()
{
    response.cmd = 0x02;
    response.len = 4;
    response.params[0] = 2;
    response.params[1] = 7;
    response.params[2] = 12;
    response.params[3] = 19;
    return 6;
}

/**
 * @brief    TEST IR WAVE DATA, NEC6212
 * 
*/

void cli_task()
{
    uint8_t ack_payload_len = 0;
    uint8_t cmd_payload_len = 0;
    uint16_t u16;
    bool ret = 1;   //0：成功写入，1：写入失败

    cmd_payload_len = uart_read_len();
    if (ir_change_key == IR_CHANGE_SUCCESS)
    {
        ir_change_key = IR_CHANGE_IDLE;
        cmd_payload_len = rx_data_len;
    }

    if (cmd_payload_len != 0) 
    {
        switch (rx_data[2])
        {
            case CMD_CONFIG_IR_NEC:
                ir_protocol_config_nec();
                ack_payload_len = cli_ack(0);
                break;
            case CMD_SEND_NEC_RAW:
                ret = ir_protocol_send_nec_raw(4);
                ack_payload_len = cli_ack(ret);
                break;
            case CMD_SEND_NEC_KEY:
                ret = ir_protocol_send_nec(rx_data[4], rx_data[5], rx_data[6]);
                ack_payload_len = cli_ack(ret);
                break;
            case CMD_STOP_KEY:
                ir_protocol_stop();
                ack_payload_len = cli_ack(0);
                break;
            case CMD_SEND_IR_WAVE:
                ret = ir_protocol_send_wave(cmd_payload_len);
                ack_payload_len = cli_ack(ret);
                break;
            case CMD_SET_WAVE_FRAMES:
                u16 = rx_data[6];
                u16 = (u16 << 8 & 0xFF00) |  rx_data[5];
                ret = ir_protocol_set_wave_frames(rx_data[4], u16);
                ack_payload_len = cli_ack(ret);
                break;
            
            case CMD_READ_STATUS:
                ack_payload_len = cli_get_status();
                break;  
            case CMD_READ_VERSION:
                ack_payload_len = cli_get_version();
                break;
            default:
                break;
        }

        cmd_payload_len = 0;
    }

    if (ack_payload_len != 0) 
    {
        uart_send_packet((uint8_t *)&response, ack_payload_len);
    }

    if (ir_send_state == IR_NEC_SEND_FINISH)
    {
        send_nec_finish();
        ir_send_state = 0;
    }
    else if (ir_send_state == IR_UNIVERSAL_SEND_FINISH)
    {
        send_unicersal_finish();
        ir_send_state = 0;
    }
    
}
