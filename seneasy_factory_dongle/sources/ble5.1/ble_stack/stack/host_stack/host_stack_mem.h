/**********************************************************************************************************************
 * @file     host_stack_mem.h
 * @version  V1.0
 * @date     2022/01/03
 * @history
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/


#ifndef __HOST_STACK_MEM_H__
#define __HOST_STACK_MEM_H__

//=====================================================================================================================
// DEFINE STRUCT
//=====================================================================================================================
typedef struct st_ll_buffer_record
{

    uint8_t max_conn_num;
    uint8_t max_total_acl_data_num;
    uint8_t max_short_acl_data_num;    
    uint8_t max_long_acl_data_num;
    uint16_t max_short_acl_data_size;
    uint16_t max_long_acl_data_size;
     
} ble_mem_cfg_t;


typedef struct st_stack_acl_data
{ 
    uint8_t *pu8AclData ;
    uint32_t u32MaxBufSize;
    uint16_t u16DataBufSize;
    uint16_t acl_size;
    uint16_t conn_handle;
    uint8_t bMemUsed; 
    uint8_t b_rx_pdu; 
    struct st_stack_acl_data* next; 
}st_stack_acl_data_t;



//memory configure record
extern ble_mem_cfg_t g_ble_mem_cfg;

 

uint8_t * rom_host_get_avail_data_buffer( uint16_t u16BufLen);
uint8_t   rom_host_get_avail_data_buffer_num(void);
uint8_t rom_host_get_avail_long_data_buffer_num_after_alloc ( uint16_t u16NeedLen );
bool rom_l2cap_tx_buffer_commit( uint16_t con_handle, uint16_t CID, uint8_t *p_data, uint16_t data_size);
void rom_host_free_data_buffer ( uint8_t* pdata );
uint8_t rom_host_add_rx_packet_from_llc(uint16_t con_handle,uint8_t* indata,uint16_t len,uint8_t start01_or_continue00);
uint8_t* rom_host_get_rx_packet_from_list(void* pConnContext , uint16_t *outlen);
uint16_t rom_ble_stack_get_max_acl_buf_size(void);
bool rom_host_stack_mem_acl_data_mem_init ( uint8_t *buffer, uint16_t offered_size );
bool rom_host_stack_mem_ctrl_init ( uint8_t *buffer, uint16_t offered_size );
void rom_ble_host_stack_mem_init ( uint8_t u8MaxConNum,uint8_t u8MaxShortAclBufNum,uint16_t u16MaxShortAclBufSize,uint8_t u8MaxLongAclBufNum,uint16_t u16MaxLongAclBufSize  );
void rom_ble_host_stack_timer_init ( void* ptimer_node, uint8_t u8TimerNodeNum,uint32_t u32PointLen);
bool  rom_host_tx_acl_data_to_llc ( void  );
void  rom_clear_tx_acl_data_on_disconn_event ( uint16_t conn_handle );
bool rom_l2cap_buffer_for_sm_tx_is_valid ( void );
bool rom_host_stack_check_has_enough_buffer_to_rsp ( void* pConnContext );
uint8_t *rom_l2cap_get_avail_data_buffer_for_sm_tx ( void );
void rom_host_get_avail_data_buffer_num_ext ( uint8_t *pu8LongBufferCnt, uint8_t *pu8ShortBufferCnt);
#endif  /* __HOST_STACK_MEM_H__ */

