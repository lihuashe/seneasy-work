#ifndef HCI_TRANSPORT_LAYER_H
#define HCI_TRANSPORT_LAYER_H

bool hci_transport_layer_init(void);

void hci_transport_layer_on_hci_ready(void);

uint16_t hci_transport_layer_get_event_parameter_max_length(void);

bool hci_transport_layer_report_event(uint8_t Event_Code, uint8_t Parameter_Total_Length, uint8_t* Event_Parameter);

bool hci_transport_layer_report_acl_data(uint16_t Handle, uint8_t Packet_Boundary_Flag, uint8_t Broadcast_Flag, uint16_t Data_Total_Length, uint8_t* Data);

#endif
