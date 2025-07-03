/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     vendor_service.c
* @brief    Vendor service source file.
* @details  Interfaces to access Vendor service.
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/

#include "smartrcu_handle.h"
#include <string.h>
#include <trace.h>
#include "crc16btx.h"
#include "board.h"
#include "ir_learn.h"
#include "rcu_application.h"
#include "smartrcu_service.h"
#include "mp_test.h"
#include "ir_app_config.h"
#include "ftl.h"

T_SMARTRCU_GLOBAL_DATA smartrcu_global_data;
T_SMARTRCU_WAVE_PACKET_DATA smartrcu_wave_packet_data;
T_SMARTRCU_IR_KEY_DATA smartrcu_ir_key_data;
T_SMARTRCU_IR_CODE_DISCRIBE smartrcu_ir_code_discribe;
extern void smartrcu_ir_learn_mag(uint8_t ir_learn_type);
uint8_t smartrcu_ir_buf[SMARTRCU_WAVE_MAX_SIZE];

void smartrcu_data_init(void)
{
    APP_PRINT_INFO0("[smartrcu_data_init] smartrcu_data_init");
    memset(&smartrcu_global_data, 0, sizeof(smartrcu_global_data));
    if(ftl_load(&smartrcu_global_data,4,12) == 0){
        APP_PRINT_INFO1("[smartrcu_data_init] smartrcu_global_data.send_smartrcu_ir_state = %d",smartrcu_global_data.send_smartrcu_ir_state);
    }
    else{
        smartrcu_global_data.key_send_data[INDEX_POWER].key_code = KEY_POWER;
        smartrcu_global_data.key_send_data[INDEX_POWER].key_code_type = SEND_IR_KEY;
        smartrcu_global_data.key_send_data[INDEX_NETFLIX].key_code = KEY_NETFLIX;
        smartrcu_global_data.key_send_data[INDEX_NETFLIX].key_code_type = SEND_IR_KEY;
        smartrcu_global_data.key_send_data[INDEX_YouTube].key_code = KEY_YouTube;
        smartrcu_global_data.key_send_data[INDEX_YouTube].key_code_type = SEND_IR_KEY;
        APP_PRINT_INFO0("[smartrcu_data_init] ftl_load data fail");
        if(ftl_save(&smartrcu_global_data,4,12) == 0){
            APP_PRINT_INFO0("[smartrcu_command_handle] ftl_save suc");
        }
        else{
            APP_PRINT_INFO0("[smartrcu_command_handle] ftl_save fail");
        }
    }
    ir_learn_module_init();
}



void smartrcu_command_handle(uint8_t *p_smartrcu_fifo_data)
{
    uint8_t smartrcu_fifo_data[40] = {0};
    uint8_t report_data[10] = {0};
    uint8_t smartrcu_fifo_command = 0,key_code = 0,key_type = 0;
    uint8_t restort_factory_cmd = 0;
    uint8_t ir_data[600] = {0};
    uint32_t crc_sum = 0;
    uint16_t crc_first_wave = 0,crc_inve = 0;
    memcpy(&smartrcu_fifo_data, p_smartrcu_fifo_data, sizeof(smartrcu_fifo_data));
    // APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_fifo_data: 0x %b",
    //                         TRACE_BINARY(sizeof(smartrcu_fifo_data), &smartrcu_fifo_data));  
    smartrcu_fifo_command = smartrcu_fifo_data[0];                                     
    switch (smartrcu_fifo_command)
    {
    case COMMAOND_IR_CODE_DISCRIBE:
        smartrcu_ir_code_discribe.data_len = smartrcu_fifo_data[1];
        smartrcu_ir_code_discribe.code_group_number = smartrcu_fifo_data[2] | (smartrcu_fifo_data[3] << 8);
        smartrcu_ir_key_data.freq = smartrcu_fifo_data[4];
        smartrcu_global_data.inve_code_flag = smartrcu_fifo_data[5];
        smartrcu_ir_code_discribe.first_wave_len = smartrcu_fifo_data[6] | (smartrcu_fifo_data[7] << 8);
        smartrcu_ir_code_discribe.first_wave_crc = smartrcu_fifo_data[8] | (smartrcu_fifo_data[9] << 8);

        APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_wave_packet_data.first_wave_len = %d",smartrcu_ir_code_discribe.first_wave_len);
        APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_wave_packet_data.first_wave_len = %x",smartrcu_ir_code_discribe.first_wave_len);

        if(smartrcu_ir_code_discribe.first_wave_len%9 == 0){
            smartrcu_wave_packet_data.packet_number = smartrcu_ir_code_discribe.first_wave_len/9;
        }
        else{
            smartrcu_wave_packet_data.packet_number = smartrcu_ir_code_discribe.first_wave_len/9 + 1;
        }

        if(smartrcu_global_data.inve_code_flag != 0){
            smartrcu_ir_code_discribe.inve_code_len = smartrcu_fifo_data[10] | (smartrcu_fifo_data[11] << 8);
            smartrcu_ir_code_discribe.inve_code_crc = smartrcu_fifo_data[12] | (smartrcu_fifo_data[13] << 8);
            if(smartrcu_ir_code_discribe.inve_code_len%9 == 0){
                smartrcu_wave_packet_data.packet_inve_number = smartrcu_ir_code_discribe.inve_code_len/9;
            }
            else{
                smartrcu_wave_packet_data.packet_inve_number = smartrcu_ir_code_discribe.inve_code_len/9 + 1;
            }           
            APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_wave_packet_data.packet_inve_number = %d",smartrcu_wave_packet_data.packet_inve_number); 
        }
        APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_ir_code_discribe: 0x %b",
                TRACE_BINARY(sizeof(smartrcu_ir_code_discribe), &smartrcu_ir_code_discribe));
        break;
    case COMMAOND_IR_CODE_FIRST:
        if(smartrcu_fifo_data[1] <= smartrcu_wave_packet_data.packet_number){
            smartrcu_wave_packet_data.smartrcu_wave_data[smartrcu_fifo_data[1]].data_number = smartrcu_fifo_data[1];
            for(uint8_t i = 0;i < 18; i++){
                smartrcu_wave_packet_data.smartrcu_wave_data[smartrcu_fifo_data[1]].data[i] = smartrcu_fifo_data[i+2];
            } 
            // APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_wave_data: 0x %b",
            //     TRACE_BINARY(sizeof(smartrcu_wave_packet_data.smartrcu_wave_data[smartrcu_fifo_data[1]]), &smartrcu_wave_packet_data.smartrcu_wave_data[smartrcu_fifo_data[1]]));
        }
        break;
    case COMMAOND_IR_INVERSE_CODE:
        if(smartrcu_fifo_data[1] <= smartrcu_wave_packet_data.packet_inve_number){
            smartrcu_wave_packet_data.smartrcu_inve_data[smartrcu_fifo_data[1]].data_number = smartrcu_fifo_data[1];
            for(uint8_t i = 0;i < 18; i++){
                smartrcu_wave_packet_data.smartrcu_inve_data[smartrcu_fifo_data[1]].data[i] = smartrcu_fifo_data[i+2];
            }  
            // APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_inve_data: 0x %b",
            //     TRACE_BINARY(sizeof(smartrcu_wave_packet_data.smartrcu_inve_data[smartrcu_fifo_data[1]]), &smartrcu_wave_packet_data.smartrcu_inve_data[smartrcu_fifo_data[1]]));
        }     
        break;
    case COMMAOND_IR_CODE_SEND_END:
        crc_sum = 0;
        memset(&smartrcu_ir_key_data.ir_buffer, 0, sizeof(smartrcu_ir_key_data.ir_buffer));
        for(uint16_t i = 0;i < smartrcu_wave_packet_data.packet_number; i++){
            for(uint16_t j = 0;j < 18;j ++){
                ir_data[i*18+j] = smartrcu_wave_packet_data.smartrcu_wave_data[i].data[j];
           }
        }
        smartrcu_ir_key_data.ir_buff_index = smartrcu_ir_code_discribe.first_wave_len;
        DBG_DIRECT("smartrcu_command_handle smartrcu_ir_key_data.ir_buff_index = %d",smartrcu_ir_key_data.ir_buff_index);         
        for(uint16_t k = 0;k < smartrcu_ir_key_data.ir_buff_index;k ++){
            smartrcu_ir_key_data.ir_buffer[k] = (ir_data[k*2] | (ir_data[k*2+1] << 8))*10;
            crc_sum = crc_sum + smartrcu_ir_key_data.ir_buffer[k]; 
            // DBG_DIRECT("smartrcu_command_handle smartrcu_ir_key_data.ir_buffer[%d] = %d  crc_sum = %d",k,smartrcu_ir_key_data.ir_buffer[k],crc_sum);   
        }       
        crc_first_wave = (crc_sum/10)%65536;

        APP_PRINT_INFO2("[smartrcu_command_handle]crc_first_wave = %x,smartrcu_ir_code_discribe.first_wave_crc = %x",crc_first_wave,smartrcu_ir_code_discribe.first_wave_crc);
        if(crc_first_wave == smartrcu_ir_code_discribe.first_wave_crc){
            smartrcu_ir_learn_mag(LEARN_DATA);
            smartrcu_global_data.send_smartrcu_ir_state = 1;
            if(ftl_save(&smartrcu_global_data,4,12) == 0){
                // APP_PRINT_INFO0("[smartrcu_command_handle] ftl_save suc");
            }
            else{
                // APP_PRINT_INFO0("[smartrcu_command_handle] ftl_save fail");
            }
            smartrcu_wave_send_ir(smartrcu_ir_key_data.ir_key_index);
            report_data[0] = COMMAOND_REPORT_RICIEVE_DATA_STATE;
            report_data[1] = 0;
            server_send_data(0, app_global_data.smartrcu_srv_id, BLE_SERVICE_CHAR_SMARTRCU_WRITE_DATA_INDEX, \
                            report_data,2, GATT_PDU_TYPE_NOTIFICATION);
        }
        else{
            report_data[0] = COMMAOND_REPORT_RICIEVE_DATA_STATE;
            report_data[1] = 0x02;
            server_send_data(0, app_global_data.smartrcu_srv_id, BLE_SERVICE_CHAR_SMARTRCU_WRITE_DATA_INDEX, \
                            report_data,2, GATT_PDU_TYPE_NOTIFICATION);
        }

        if(smartrcu_global_data.inve_code_flag != 0){
            memset(&smartrcu_ir_key_data.inve_ir_buffrt, 0, sizeof(smartrcu_ir_key_data.inve_ir_buffrt));
            crc_sum = 0;
            for(uint16_t i = 0;i < smartrcu_wave_packet_data.packet_inve_number; i++){
                for(uint16_t j = 0;j < 18;j ++){
                    ir_data[i*18+j] = smartrcu_wave_packet_data.smartrcu_inve_data[i].data[j];
                }
            }
            smartrcu_ir_key_data.inve_ir_buff_index = smartrcu_ir_code_discribe.inve_code_len;
            uint16_t inve_code_crc = smartrcu_ir_code_discribe.inve_code_crc;

            for(uint16_t k = 0;k < smartrcu_ir_key_data.inve_ir_buff_index;k ++){
                smartrcu_ir_key_data.inve_ir_buffrt[k] = (ir_data[k*2] | (ir_data[k*2+1] << 8))*10;
                crc_sum = crc_sum + smartrcu_ir_key_data.inve_ir_buffrt[k]; 
            }  

            crc_inve = (crc_sum/10)%65536;
            APP_PRINT_INFO2("[smartrcu_command_handle] crc_inve = %x,inve_code_crc = %x",crc_inve,inve_code_crc);
            if(crc_inve == inve_code_crc){
                smartrcu_key_index_inservion_convert(smartrcu_ir_key_data.ir_key_index);

                smartrcu_ir_learn_mag(INVE_LEARN_DATA);
                smartrcu_wave_send_ir(smartrcu_ir_key_data.ir_key_index);
                report_data[0] = COMMAOND_REPORT_RICIEVE_DATA_STATE;
                report_data[1] = 0;
                server_send_data(0, app_global_data.smartrcu_srv_id, BLE_SERVICE_CHAR_SMARTRCU_WRITE_DATA_INDEX, \
                                report_data,2, GATT_PDU_TYPE_NOTIFICATION);
            }
            else{
                report_data[0] = COMMAOND_REPORT_RICIEVE_DATA_STATE;
                report_data[1] = 0x04;
                server_send_data(0, app_global_data.smartrcu_srv_id, BLE_SERVICE_CHAR_SMARTRCU_WRITE_DATA_INDEX, \
                                report_data,2, GATT_PDU_TYPE_NOTIFICATION);
            }

        }
        app_set_latency_status(LATENCY_SMARTRCU_PROC_BIT, LANTENCY_ON);
        break;
    case COMMAOND_IR_KEY_INDEX:
        smartrcu_key_index_convert(smartrcu_fifo_data[1]);
        app_set_latency_status(LATENCY_SMARTRCU_PROC_BIT, LANTENCY_OFF);
        break;        
    case COMMAOND_IR_BLE_CHANGE:
        key_code = smartrcu_fifo_data[1];
        key_type = smartrcu_fifo_data[2];
        if(key_code == KEY_POWER){
            smartrcu_global_data.key_send_data[INDEX_POWER].key_code_type = key_type;
        }
        else if(key_code == KEY_NETFLIX){
            smartrcu_global_data.key_send_data[INDEX_NETFLIX].key_code_type = key_type;
        }
        else if(key_code == KEY_YouTube){
            smartrcu_global_data.key_send_data[INDEX_YouTube].key_code_type = key_type;
        }
        if(ftl_save(&smartrcu_global_data,4,12) == 0){
            APP_PRINT_INFO0("[smartrcu_command_handle] COMMAOND_IR_BLE_CHANGE ftl_save suc");
        }
        else{
            APP_PRINT_INFO0("[smartrcu_command_handle] COMMAOND_IR_BLE_CHANGE ftl_save fail");
        }
        break;
    case COMMAOND_READ_RCU_KEY_STATE:
        key_code = smartrcu_fifo_data[1];
        report_data[0] = COMMAOND_REPORT_RCU_KEY_STATE;
        report_data[1] = key_code;
        if(ftl_load(&smartrcu_global_data,4,12) == 0){
            APP_PRINT_INFO0("[smartrcu_command_handle] COMMAOND_READ_RCU_KEY_STATE ftl_load suc");
        }
        else{
            APP_PRINT_INFO0("[smartrcu_command_handle] COMMAOND_READ_RCU_KEY_STATE ftl_load fail");
        }
        switch (key_code)
        {
            case KEY_NETFLIX:
                report_data[2] = smartrcu_global_data.key_send_data[INDEX_NETFLIX].key_code_type;
                break;
            case KEY_YouTube:
                report_data[2] = smartrcu_global_data.key_send_data[INDEX_YouTube].key_code_type;
                break;
            default:
                break;
        }
        server_send_data(0, app_global_data.smartrcu_srv_id, BLE_SERVICE_CHAR_SMARTRCU_WRITE_DATA_INDEX, \
                report_data,3, GATT_PDU_TYPE_NOTIFICATION);
        break;
    case COMMAOND_READ_KEY_NUMBER:

        break;
    case COMMAOND_READ_ALL_STUDY_KEY_NUMBER:

        break;
    case COMMAOND_READ_IR_CODE_DISCRIBE:

        break;
    case COMMAOND_READ_IR_CODE_DATA:

        break;
    case COMMAOND_READ_IR_INVERSE_CODE_DATA:

        break;
    case COMMAOND_READ_CODE_STATE:

        break;
    case COMMAOND_RESTORE_FACTORY_SETTING:
        restort_factory_cmd = smartrcu_fifo_data[1];
        switch (restort_factory_cmd)
        {
        case CLEAR_ALL_INFO:
            mp_test_clear_bond_info();
            smartrcu_data_reset();
            break;
        case CLEAR_PAIR_INFO:
            mp_test_clear_bond_info();
            break;
        case CLEAR_IR_INFO:
            smartrcu_data_reset();
            break;
        
        default:
            break;
        }

        break;

    default:

        break;
    }
}

void smartrcu_key_index_convert(uint8_t key_index)
{
    switch (key_index)
    {
    case KEY_POWER:
        ir_learn_reset_ftl_learning_data();
        smartrcu_ir_key_data.ir_key_index = MM_POWER;   
        break;
    case KEY_NETFLIX:
        smartrcu_ir_key_data.ir_key_index = MM_NETFLIX;
        break;
    case KEY_YouTube:
        smartrcu_ir_key_data.ir_key_index = MM_YouTube;
        break;
    
    default:
        break;
    }
    APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_ir_key_data.ir_key_index = %d",smartrcu_ir_key_data.ir_key_index);  
}

void smartrcu_key_index_inservion_convert(uint8_t key_index)
{
    switch (key_index)
    {
    case MM_POWER:
        smartrcu_ir_key_data.inve_ir_key_index = MM_POWER_INCERSE;   
        break;
    case MM_NETFLIX:
        smartrcu_ir_key_data.inve_ir_key_index = MM_NETFLIX_INCERSE;
        break;
    case MM_YouTube:
        smartrcu_ir_key_data.inve_ir_key_index = MM_YouTube_INCERSE;
        break;
    
    default:
        break;
    }
    APP_PRINT_INFO1("[smartrcu_command_handle] smartrcu_ir_key_data.ir_key_index = %d",smartrcu_ir_key_data.ir_key_index);  
}
void smartrcu_wave_send_ir(uint8_t key_index)
{ 
    switch (key_index)
    {
    case MM_POWER:
        smartrcu_global_data.key_send_data[INDEX_POWER].key_code_type = SEND_IR_KEY;  
        break;
    case MM_NETFLIX:
        smartrcu_global_data.key_send_data[INDEX_NETFLIX].key_code_type = SEND_IR_KEY;  
        break;
    case MM_YouTube:
        smartrcu_global_data.key_send_data[INDEX_YouTube].key_code_type = SEND_IR_KEY;  
        break;
    
    default:
        break;
    } 
}
void smartrcu_data_reset(void)
{
    memset(&smartrcu_global_data, 0, sizeof(smartrcu_global_data));
    smartrcu_global_data.key_send_data[INDEX_POWER].key_code = KEY_POWER;
    smartrcu_global_data.key_send_data[INDEX_POWER].key_code_type = SEND_IR_KEY;
    smartrcu_global_data.key_send_data[INDEX_NETFLIX].key_code = KEY_NETFLIX;
    smartrcu_global_data.key_send_data[INDEX_NETFLIX].key_code_type = SEND_IR_KEY;
    smartrcu_global_data.key_send_data[INDEX_YouTube].key_code = KEY_YouTube;
    smartrcu_global_data.key_send_data[INDEX_YouTube].key_code_type = SEND_IR_KEY;
    if(ftl_save(&smartrcu_global_data,4,12) == 0){
        APP_PRINT_INFO0("[smartrcu_data_reset] ftl_save suc");
    }
    else{
        APP_PRINT_INFO0("[smartrcu_data_reset] ftl_save fail");
    }
    ir_learn_reset_ftl_learning_data();
}
