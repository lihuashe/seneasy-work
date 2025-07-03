 


#ifndef HOST_INTERFACE_H
#define HOST_INTERFACE_H


//=====================================================================================================================
// DEFINE
//=====================================================================================================================



//=====================================================================================================================
// DEFINE STRUCT
//=====================================================================================================================
 
uint32_t  rom_host_interface_get_random(void);
void  rom_host_interface_get_random_array(uint8_t rand[16]);
void  rom_host_interface_get_chip_id(uint8_t chip_id[16]);
void  rom_host_interface_timer_start(stack_timer_source_t * timer);
void  rom_host_interface_timer_stop(stack_timer_source_t * timer);
void  rom_host_interface_timer_restart(stack_timer_source_t * timer);
void  rom_host_interface_timer_delete(stack_timer_source_t * ptimer);
void rom_host_interface_start_encryption (uint16_t conn_handle, uint8_t *rand, uint16_t ediv, uint8_t *ltk );


void rom_host_interface_long_term_key_request_reply ( uint16_t conn_handle, uint8_t *ltk );

void rom_host_interface_long_term_key_request_negative_reply ( uint16_t conn_handle );

void rom_host_interface_set_hci_event_parameter_max_length(uint16_t u16MaxLength);
uint16_t rom_host_interface_get_hci_event_parameter_max_length(void);
bool rom_host_interface_process_acl_data(uint16_t Handle, uint8_t Packet_Boundary_Flag, uint8_t Broadcast_Flag, uint16_t Data_Total_Length, uint8_t* Data);
bool rom_host_interface_process_event(uint8_t Event_Code, uint8_t Parameter_Total_Length, uint8_t* Event_Parameter);
void rom_host_interface_get_own_identify_address(uint8_t pu8OwnIdentifyAddr[6],uint8_t * pu8OwnIdentifyAddrType);
#endif  /* __STACK_PORTING_H__ */

