/*
MLOG_ID_RANGE: 0x34A0~0x34FF
MLOG_ID_USED:  0x34A0~0x34A7
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "llc_llp_interface.h"


#include "llc_spec_5_1.h"

#include "llc_adv_channel_pdu.h"
#include "llc_data_channel_pdu.h"
#include "llc_acl_data.h"
#include "llc_hci_command.h"
#include "llc_scheduler.h"

#include "llc.h"
#include "llc_data.h"
#include "patch_llc_white_list.h"
#include "llc_ctrl_procedure.h"

#include "llc_connection_role.h"

#include "llc_master.h"
#include "llc_legacy_adv.h"

#include "llc_legacy_initiator.h"

#include "llc_ext_adv.h"
#include "llc_ext_scan.h"
#include "llc_ext_initiator.h"

#include "hci.h"


#include "ble_impl.h"


#include "mlog.h"

#include "llc_tool.h"

#include "controller_interface.h"

#include "extended_controller_interface.h"

void rom_controller_command_HCI_LE_Set_Advertising_Set_Random_Address(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    uint8_t Advertising_Handle;
//    uint8_t Advertising_Random_Address[6];
//}__attribute__((packed))
//stHciCommandParamLESetAdvertisingSetRandomAddress_t;
    
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if ((NULL == Parameters)||(sizeof(stHciCommandParamLESetAdvertisingSetRandomAddress_t) != Parameter_Total_Length))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Advertising_Set_Random_Address,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stHciCommandParamLESetAdvertisingSetRandomAddress_t* pstParam = (stHciCommandParamLESetAdvertisingSetRandomAddress_t*)Parameters;
    
    stReturnParameters.Status = rom_llc_ext_adv_set_advertising_set_random_address(pstParam->Advertising_Handle, pstParam->Advertising_Random_Address);
    
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Advertising_Set_Random_Address,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_HCI_LE_Set_Extended_Advertising_Parameters(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    uint8_t Advertising_Handle;
//    uint16_t Advertising_Event_Properties;
//    uint8_t Primary_Advertising_Interval_Min[3];
//    uint8_t Primary_Advertising_Interval_Max[3];
//    uint8_t Primary_Advertising_Channel_Map;
//    uint8_t Own_Address_Type;
//    uint8_t Peer_Address_Type;
//    uint8_t Peer_Address[6];
//    uint8_t Advertising_Filter_Policy;
//    int8_t Advertising_Tx_Power;
//    uint8_t Primary_Advertising_PHY;
//    uint8_t Secondary_Advertising_Max_Skip;
//    uint8_t Secondary_Advertising_PHY;
//    uint8_t Advertising_SID;
//    uint8_t Scan_Request_Notification_Enable;
//}__attribute__((packed))
//stHciCommandParamLESetExtendedAdvertisingParameters_t;
//typedef struct
//{
//    uint8_t Status;
//    int8_t Selected_Tx_Power;
//}__attribute__((packed))
//stHciReturnParametersLESetExtendedAdvertisingParameters_t;

    stHciReturnParametersLESetExtendedAdvertisingParameters_t stReturnParameters = {0};
    if ((NULL == Parameters)||(sizeof(stHciCommandParamLESetExtendedAdvertisingParameters_t) != Parameter_Total_Length))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stHciCommandParamLESetExtendedAdvertisingParameters_t* pstParam = (stHciCommandParamLESetExtendedAdvertisingParameters_t*)Parameters;
    
    uint32_t u32PrimaryAdvertisingIntervalMin = (pstParam->Primary_Advertising_Interval_Min[0])
                                                    |(((uint32_t)pstParam->Primary_Advertising_Interval_Min[1])<<8)
                                                        |(((uint32_t)pstParam->Primary_Advertising_Interval_Min[2])<<16);
    uint32_t u32PrimaryAdvertisingIntervalMax = (pstParam->Primary_Advertising_Interval_Max[0])
                                                    |(((uint32_t)pstParam->Primary_Advertising_Interval_Max[1])<<8)
                                                        |(((uint32_t)pstParam->Primary_Advertising_Interval_Max[2])<<16);
    
    stLlcSpec51HciPhyOptions_t stPhyOptions;
    stPhyOptions.u16Value = 0;
    stPhyOptions.bitField.CodedTxPrefer = LLC_SPEC_5_1_PHY_OPTION_CODED_TX_PREFER_NO_PREFER;
    
    uint8_t Secondary_Advertising_Channel_Map[5];
    rom_ble_impl_get_default_channel_map(Secondary_Advertising_Channel_Map);
    
    stReturnParameters.Status = rom_llc_ext_adv_set_parameters(pstParam->Advertising_Handle,
        pstParam-> Advertising_Event_Properties,
        u32PrimaryAdvertisingIntervalMin,
        u32PrimaryAdvertisingIntervalMax,
        pstParam-> Primary_Advertising_Channel_Map,
        Secondary_Advertising_Channel_Map,
        pstParam-> Own_Address_Type,
        pstParam-> Peer_Address_Type,
        pstParam-> Peer_Address,
        pstParam-> Advertising_Filter_Policy,
        pstParam-> Advertising_Tx_Power,
        pstParam-> Primary_Advertising_PHY,
        pstParam-> Secondary_Advertising_Max_Skip,
        pstParam-> Secondary_Advertising_PHY,
        stPhyOptions.u16Value,
        pstParam-> Advertising_SID,
        pstParam-> Scan_Request_Notification_Enable,
        &stReturnParameters.Selected_Tx_Power);
    
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);

}


void rom_controller_command_VENDOR_Set_Extended_Advertising_Parameters(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    uint8_t Advertising_Handle;
//    uint16_t Advertising_Event_Properties;
//    uint8_t Primary_Advertising_Interval_Min[3];
//    uint8_t Primary_Advertising_Interval_Max[3];
//    uint8_t Primary_Advertising_Channel_Map;
//    uint8_t Secondary_Advertising_Channel_Map[5];
//    uint8_t Own_Address_Type;
//    uint8_t Peer_Address_Type;
//    uint8_t Peer_Address[6];
//    uint8_t Advertising_Filter_Policy;
//    int8_t Advertising_Tx_Power;
//    uint8_t Primary_Advertising_PHY;
//    uint8_t Secondary_Advertising_Max_Skip;
//    uint8_t Secondary_Advertising_PHY;
//    uint16_t PHY_Options;
//    uint8_t Advertising_SID;
//    uint8_t Scan_Request_Notification_Enable;
//}__attribute__((packed))
//stHciCommandParamVendorSetExtendedAdvertisingParameters_t;
//typedef struct
//{
//    uint8_t Status;
//    int8_t Selected_Tx_Power;
//}__attribute__((packed))
//stHciReturnParametersVendorSetExtendedAdvertisingParameters_t;

    stHciReturnParametersVendorSetExtendedAdvertisingParameters_t stReturnParameters = {0};
    if ((NULL == Parameters)||(sizeof(stHciCommandParamVendorSetExtendedAdvertisingParameters_t) != Parameter_Total_Length))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Advertising_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stHciCommandParamVendorSetExtendedAdvertisingParameters_t* pstParam = (stHciCommandParamVendorSetExtendedAdvertisingParameters_t*)Parameters;
    
    uint32_t u32PrimaryAdvertisingIntervalMin = (pstParam->Primary_Advertising_Interval_Min[0])
                                                    |(((uint32_t)pstParam->Primary_Advertising_Interval_Min[1])<<8)
                                                        |(((uint32_t)pstParam->Primary_Advertising_Interval_Min[2])<<16);
    uint32_t u32PrimaryAdvertisingIntervalMax = (pstParam->Primary_Advertising_Interval_Max[0])
                                                    |(((uint32_t)pstParam->Primary_Advertising_Interval_Max[1])<<8)
                                                        |(((uint32_t)pstParam->Primary_Advertising_Interval_Max[2])<<16);
    
    stReturnParameters.Status = rom_llc_ext_adv_set_parameters(pstParam->Advertising_Handle,
        pstParam-> Advertising_Event_Properties,
        u32PrimaryAdvertisingIntervalMin,
        u32PrimaryAdvertisingIntervalMax,
        pstParam-> Primary_Advertising_Channel_Map,
        pstParam-> Secondary_Advertising_Channel_Map,
        pstParam-> Own_Address_Type,
        pstParam-> Peer_Address_Type,
        pstParam-> Peer_Address,
        pstParam-> Advertising_Filter_Policy,
        pstParam-> Advertising_Tx_Power,
        pstParam-> Primary_Advertising_PHY,
        pstParam-> Secondary_Advertising_Max_Skip,
        pstParam-> Secondary_Advertising_PHY,
        pstParam->PHY_Options,
        pstParam-> Advertising_SID,
        pstParam-> Scan_Request_Notification_Enable,
        &stReturnParameters.Selected_Tx_Power);
    
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Advertising_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);

}

void rom_controller_command_HCI_LE_Set_Extended_Advertising_Data(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
/*
Advertising_Handle,             Size: 1 octet
Operation,                      Size: 1 octet
Fragment_Preference,            Size: 1 octet
Advertising_Data_Length,        Size: 1 octet
Advertising_Data                Size: Advertising_Data_Length octets
*/
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if ((NULL == Parameters)||(Parameter_Total_Length < 4))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Data,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    uint8_t Advertising_Handle = Parameters[0];
    uint8_t Operation = Parameters[1];
    uint8_t Fragment_Preference = Parameters[2];
    uint8_t Advertising_Data_Length = Parameters[3];
    uint8_t *Advertising_Data = &Parameters[4];
    
    if (Parameter_Total_Length < 4 + Advertising_Data_Length)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Data,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stReturnParameters.Status = rom_llc_ext_adv_set_advertising_data(Advertising_Handle,
        Operation,
        Fragment_Preference,
        Advertising_Data_Length,
        Advertising_Data);
    
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Data,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_HCI_LE_Set_Extended_Scan_Response_Data(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
/*
Advertising_Handle,             Size: 1 octet
Operation,                      Size: 1 octet
Fragment_Preference,            Size: 1 octet
Scan_Response_Data_Length,      Size: 1 octet
Scan_Response_Data              Size: Advertising_Data_Length octets
*/
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if ((NULL == Parameters)||(Parameter_Total_Length < 4))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Response_Data,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    uint8_t Advertising_Handle = Parameters[0];
    uint8_t Operation = Parameters[1];
    uint8_t Fragment_Preference = Parameters[2];
    uint8_t Scan_Response_Data_Length = Parameters[3];
    uint8_t *Scan_Response_Data = &Parameters[4];
    
    if (Parameter_Total_Length < 4 + Scan_Response_Data_Length)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Response_Data,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stReturnParameters.Status = rom_llc_ext_adv_set_scan_response_data(Advertising_Handle,
        Operation,
        Fragment_Preference,
        Scan_Response_Data_Length,
        Scan_Response_Data);
    
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Response_Data,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_HCI_LE_Set_Extended_Advertising_Enable(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    
    stReturnParameters.Status = rom_llc_ext_adv_set_enable(Parameter_Total_Length, Parameters);
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Enable,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_HCI_LE_Read_Maximum_Advertising_Data_Length(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    ////no param
//}__attribute__((packed))
//stHciCommandParamLEReadMaximumAdvertisingDataLength_t;
//typedef struct
//{
//    uint8_t Status;
//    uint16_t Maximum_Advertising_Data_Length;
//}__attribute__((packed))
//stHciReturnParametersLEReadMaximumAdvertisingDataLength_t;
    
    stHciReturnParametersLEReadMaximumAdvertisingDataLength_t stReturnParameters = {0};
    
    if (0 != Parameter_Total_Length)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Read_Maximum_Advertising_Data_Length,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    //do not use "&stReturnParameters.Maximum_Advertising_Data_Length" to avoid align-fault
    uint16_t Maximum_Advertising_Data_Length = 0;
    stReturnParameters.Status = rom_llc_ext_adv_read_maximum_advertising_data_length(&Maximum_Advertising_Data_Length);
    stReturnParameters.Maximum_Advertising_Data_Length = Maximum_Advertising_Data_Length;
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Read_Maximum_Advertising_Data_Length,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_HCI_LE_Read_Number_of_Supported_Advertising_Sets(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    ////no param
//}__attribute__((packed))
//stHciCommandParamLEReadNumberofSupportedAdvertisingSets_t;
//typedef struct
//{
//    uint8_t Status;
//    uint8_t Num_Supported_Advertising_Sets;
//}__attribute__((packed))
//stHciReturnParametersLEReadNumberofSupportedAdvertisingSets_t;
    
    stHciReturnParametersLEReadNumberofSupportedAdvertisingSets_t stReturnParameters = {0};
    
    if (0 != Parameter_Total_Length)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Read_Number_of_Supported_Advertising_Sets,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stReturnParameters.Status = rom_llc_ext_adv_read_number_of_supported_advertising_sets(&stReturnParameters.Num_Supported_Advertising_Sets);
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Read_Number_of_Supported_Advertising_Sets,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_HCI_LE_Remove_Advertising_Set(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    uint8_t Advertising_Handle;
//}__attribute__((packed))
//stHciCommandParamLERemoveAdvertisingSet_t;
    
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if ((NULL == Parameters)||(sizeof(stHciCommandParamLERemoveAdvertisingSet_t)!= Parameter_Total_Length))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Remove_Advertising_Set,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stHciCommandParamLERemoveAdvertisingSet_t* pstParam = (stHciCommandParamLERemoveAdvertisingSet_t*) Parameters;
    
    stReturnParameters.Status = rom_llc_ext_adv_remove_advertising_set(pstParam->Advertising_Handle);
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Remove_Advertising_Set,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_HCI_LE_Clear_Advertising_Sets(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    ////no param
//}__attribute__((packed))
//stHciCommandParamLEClearAdvertisingSets_t;
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if (0 != Parameter_Total_Length)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Clear_Advertising_Sets,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stReturnParameters.Status = rom_llc_ext_adv_clear_advertising_sets();
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Clear_Advertising_Sets,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_event_HCI_LE_Advertising_Set_Terminated(uint8_t Status, uint8_t Advertising_Handle, uint16_t Connection_Handle, uint8_t Num_Completed_Extended_Advertising_Events)
{
//typedef struct
//{
//    uint8_t Subevent_Code;
//    uint8_t Status;
//    uint8_t Advertising_Handle;
//    uint16_t Connection_Handle;
//    uint8_t Num_Completed_Extended_Advertising_Events;
//}__attribute__((packed))
//stHciEventParamLEAdvertisingSetTerminated_t;
    
    stHciEventParamLEAdvertisingSetTerminated_t stEventParameter;
    stEventParameter.Subevent_Code = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Advertising_Set_Terminated;
    stEventParameter.Status = Status;
    stEventParameter.Advertising_Handle = Advertising_Handle;
    stEventParameter.Connection_Handle = Connection_Handle;
    stEventParameter.Num_Completed_Extended_Advertising_Events = Num_Completed_Extended_Advertising_Events;

    rom_hci_controller_report_event(LLC_SPEC_5_1_HCI_EVENT_CODE_LE_Meta_event, sizeof(stEventParameter), (uint8_t*)&stEventParameter);
}

void rom_controller_event_HCI_LE_Scan_Request_Received(uint8_t Advertising_Handle, uint8_t Scanner_Address_Type, uint8_t Scanner_Address[6])
{
//typedef struct
//{
//    uint8_t Subevent_Code;
//    uint8_t Advertising_Handle;
//    uint8_t Scanner_Address_Type;
//    uint8_t Scanner_Address[6];
//}__attribute__((packed))
//stHciEventParamLEScanRequestReceived_t;
    
    stHciEventParamLEScanRequestReceived_t stEventParameter;
    
    stEventParameter.Subevent_Code = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Scan_Request_Received;
    stEventParameter.Advertising_Handle = Advertising_Handle;
    stEventParameter.Scanner_Address_Type = Scanner_Address_Type;
    if (NULL == Scanner_Address)
    {
        memset(stEventParameter.Scanner_Address, 0, 6);
    }
    else
    {
        memcpy(stEventParameter.Scanner_Address, Scanner_Address, 6);
    }
    
    rom_hci_controller_report_event(LLC_SPEC_5_1_HCI_EVENT_CODE_LE_Meta_event, sizeof(stEventParameter), (uint8_t*)&stEventParameter);
}



void rom_controller_command_HCI_LE_Set_Extended_Scan_Parameters(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    /*
    Own_Address_Type,                       Size: 1 octet
    Scanning_Filter_Policy,                 Size: 1 octet
    Scanning_PHYs,                          Size: 1 octet
    Scan_Type[i],                           Size: 1 octet * bits set in Scanning_PHYs
    Scan_Interval[i]                        Size: 2 octets * bits set in Scanning_PHYs
    Scan_Window[i]                          Size: 2 octets * bits set in Scanning_PHYs
    */
    #define SINGLE_SCAN_PHY_PARAM_LENGTH   (1 + 2 + 2)
    
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if ((Parameter_Total_Length < 3)||(NULL == Parameters))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    uint8_t Scanning_PHYs = Parameters[2];
    if (Scanning_PHYs > 0x7)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    const uint8_t pu8ScanningPhysNumTable[8] =
    {
        0,//0 : None
        1,//1 : 1M
        1,//2 : 2M
        2,//3 : 1M|2M
        1,//4 : Coded
        2,//5 : 1M|Coded
        2,//6 : 2M|Coded
        3,//7 : 1M|2M|Coded
    };
    uint8_t u8ScanningPhysNum = pu8ScanningPhysNumTable[Scanning_PHYs & 0x7];
    if (0 == u8ScanningPhysNum)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    if (3 + (SINGLE_SCAN_PHY_PARAM_LENGTH * u8ScanningPhysNum) != Parameter_Total_Length)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stLlcExtScanSetParameters_t stParameters = {0};
    stParameters.Own_Address_Type = Parameters[0];
    stParameters.Scanning_Filter_Policy = Parameters[1];
    
    uint32_t u32Offset = 3;
    if (Scanning_PHYs & 0x01)
    {
        stParameters.Scan_PHY_Enabled_1M = true;
        stParameters.Scan_Type_1M = Parameters[u32Offset + 0];
        stParameters.Scan_Interval_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32Offset + 1]);
        stParameters.Scan_Window_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32Offset + 3]);
        stParameters.Scan_Map_1M = LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_37|LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_38|LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_39;
        
        u32Offset += SINGLE_SCAN_PHY_PARAM_LENGTH;
    }
    else
    {
        stParameters.Scan_PHY_Enabled_1M = false;
    }
    if (Scanning_PHYs & 0x04)
    {
        stParameters.Scan_PHY_Enabled_Coded = true;
        stParameters.Scan_Type_Coded = Parameters[u32Offset + 0];
        stParameters.Scan_Interval_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32Offset + 1]);
        stParameters.Scan_Window_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32Offset + 3]);
        stParameters.Scan_Map_Coded = LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_37|LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_38|LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_39;
        
        u32Offset += SINGLE_SCAN_PHY_PARAM_LENGTH;
    }
    else
    {
        stParameters.Scan_PHY_Enabled_Coded = false;
    }
    
    stReturnParameters.Status = rom_llc_ext_scan_set_parameters(&stParameters);
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_VENDOR_Set_Extended_Scan_Parameters(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    /*
    Own_Address_Type,                       Size: 1 octet
    Scanning_Filter_Policy,                 Size: 1 octet
    Scanning_PHYs,                          Size: 1 octet
    Scan_Type[i],                           Size: 1 octet * bits set in Scanning_PHYs
    Scan_Interval[i]                        Size: 2 octets * bits set in Scanning_PHYs
    Scan_Window[i]                          Size: 2 octets * bits set in Scanning_PHYs
    Scan_Map[i]                             Size: 1 octet * bits set in Scanning_PHYs
    */
    #define SINGLE_SCAN_PHY_PARAM_WITH_MAP_LENGTH   (1 + 2 + 2 + 1)
    
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if ((Parameter_Total_Length < 3)||(NULL == Parameters))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    uint8_t Scanning_PHYs = Parameters[2];
    if (Scanning_PHYs > 0x7)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    const uint8_t pu8ScanningPhysNumTable[8] =
    {
        0,//0 : None
        1,//1 : 1M
        1,//2 : 2M
        2,//3 : 1M|2M
        1,//4 : Coded
        2,//5 : 1M|Coded
        2,//6 : 2M|Coded
        3,//7 : 1M|2M|Coded
    };
    uint8_t u8ScanningPhysNum = pu8ScanningPhysNumTable[Scanning_PHYs & 0x7];
    if (0 == u8ScanningPhysNum)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    if (3 + (SINGLE_SCAN_PHY_PARAM_WITH_MAP_LENGTH * u8ScanningPhysNum) != Parameter_Total_Length)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    stLlcExtScanSetParameters_t stParameters = {0};
    stParameters.Own_Address_Type = Parameters[0];
    stParameters.Scanning_Filter_Policy = Parameters[1];
    
    uint32_t u32Offset = 3;
    if (Scanning_PHYs & 0x01)
    {
        stParameters.Scan_PHY_Enabled_1M = true;
        stParameters.Scan_Type_1M = Parameters[u32Offset + 0];
        stParameters.Scan_Interval_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32Offset + 1]);
        stParameters.Scan_Window_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32Offset + 3]);
        stParameters.Scan_Map_1M = Parameters[u32Offset + 5];
        
        u32Offset += SINGLE_SCAN_PHY_PARAM_WITH_MAP_LENGTH;
    }
    else
    {
        stParameters.Scan_PHY_Enabled_1M = false;
    }
    if (Scanning_PHYs & 0x04)
    {
        stParameters.Scan_PHY_Enabled_Coded = true;
        stParameters.Scan_Type_Coded = Parameters[u32Offset + 0];
        stParameters.Scan_Interval_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32Offset + 1]);
        stParameters.Scan_Window_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32Offset + 3]);
        stParameters.Scan_Map_Coded = Parameters[u32Offset + 5];
        
        u32Offset += SINGLE_SCAN_PHY_PARAM_WITH_MAP_LENGTH;
    }
    else
    {
        stParameters.Scan_PHY_Enabled_Coded = false;
    }
    
    stReturnParameters.Status = rom_llc_ext_scan_set_parameters(&stParameters);
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Scan_Parameters,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}

void rom_controller_command_HCI_LE_Set_Extended_Scan_Enable(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    uint8_t Enable;
//    uint8_t Filter_Duplicates;
//    uint16_t Duration;
//    uint16_t Period;
//}__attribute__((packed))
//stHciCommandParamLESetExtendedScanEnable_t;
    
    if ((sizeof(stHciCommandParamLESetExtendedScanEnable_t) != Parameter_Total_Length) || (NULL == Parameters))
    {
        rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    stHciCommandParamLESetExtendedScanEnable_t* pstParam = (stHciCommandParamLESetExtendedScanEnable_t*)Parameters;
    rom_llc_ext_scan_set_enable(pstParam->Enable, pstParam->Filter_Duplicates, pstParam->Duration, pstParam->Period);
}



void rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(uint8_t Status)
{
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    stReturnParameters.Status = Status;
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Enable,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}


void rom_controller_event_HCI_LE_Extended_Advertising_Report(uint8_t Parameter_Total_Length, uint8_t* Event_Parameter)
{
    rom_hci_controller_report_event(LLC_SPEC_5_1_HCI_EVENT_CODE_LE_Meta_event, Parameter_Total_Length, Event_Parameter);
}


void rom_controller_event_HCI_LE_Scan_Timeout(void)
{
//typedef struct
//{
//    uint8_t Subevent_Code;
//}__attribute__((packed))
//stHciEventParamLEScanTimeout_t;
    
    stHciEventParamLEScanTimeout_t stEventParameter;
    stEventParameter.Subevent_Code = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Scan_Timeout;

    rom_hci_controller_report_event(LLC_SPEC_5_1_HCI_EVENT_CODE_LE_Meta_event, sizeof(stEventParameter), (uint8_t*)&stEventParameter);
}


void rom_controller_command_HCI_LE_Extended_Create_Connection(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    /*
    Initiator_Filter_Policy                 Size: 1 octet
    Own_Address_Type,                       Size: 1 octet
    Peer_Address_Type,                      Size: 1 octet
    Peer_Address,                           Size: 6 octets
    Initiating_PHYs,                        Size: 1 octet
    Scan_Interval[i]                        Size: 2 octets * bits set in Initiating_PHYs
    Scan_Window[i]                          Size: 2 octets * bits set in Initiating_PHYs
    Conn_Interval_Min[i],                   Size: 2 octets * bits set in Initiating_PHYs
    Conn_Interval_Max[i],                   Size: 2 octets * bits set in Initiating_PHYs
    Conn_Latency[i],                        Size: 2 octets * bits set in Initiating_PHYs
    Supervision_Timeout[i],                 Size: 2 octets * bits set in Initiating_PHYs
    Minimum_CE_Length[i],                   Size: 2 octets * bits set in Initiating_PHYs
    Maximum_CE_Length[i],                   Size: 2 octets * bits set in Initiating_PHYs
    */
    #define HCI_EXT_INIT_PARAM_WITHOUT_PHY_LENGTH   (1+1+1+6+1)
    #define SINGLE_HCI_EXT_INIT_PHY_PARAM_LENGTH   (2+2+2+2+2+2+2+2)
    
    if ((NULL == Parameters)||(Parameter_Total_Length < HCI_EXT_INIT_PARAM_WITHOUT_PHY_LENGTH))
    {
        rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_HCI_LE_Extended_Create_Connection, LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    uint8_t Initiating_PHYs = Parameters[9];
    if (Initiating_PHYs > 0x7)
    {
        rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_HCI_LE_Extended_Create_Connection, LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    const uint8_t pu8InitiatingPhysNumTable[8] =
    {
        0,//0 : None
        1,//1 : 1M
        1,//2 : 2M
        2,//3 : 1M|2M
        1,//4 : Coded
        2,//5 : 1M|Coded
        2,//6 : 2M|Coded
        3,//7 : 1M|2M|Coded
    };
    uint8_t u8InitiatingPhysNum = pu8InitiatingPhysNumTable[Initiating_PHYs & 0x7];
    if (0 == u8InitiatingPhysNum)
    {
        rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_HCI_LE_Extended_Create_Connection, LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    if ((HCI_EXT_INIT_PARAM_WITHOUT_PHY_LENGTH) + u8InitiatingPhysNum * SINGLE_HCI_EXT_INIT_PHY_PARAM_LENGTH != Parameter_Total_Length)
    {
        rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_HCI_LE_Extended_Create_Connection, LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    stLlcExtInitiatorSetParameters_t stParameters = {0};
    stParameters.Initiator_Filter_Policy = Parameters[0];
    stParameters.Own_Address_Type = Parameters[1];
    stParameters.Peer_Address_Type = Parameters[2];
    memcpy(stParameters.Peer_Address, &Parameters[3], 6);
    
    uint32_t u32PhyOffset = HCI_EXT_INIT_PARAM_WITHOUT_PHY_LENGTH;
    if (Initiating_PHYs & (1 << 0))
    {
        stParameters.Scan_PHY_Enabled_1M = true;
        stParameters.Scan_Interval_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 0]);
        stParameters.Scan_Window_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 2]);
        stParameters.Scan_Map_1M = LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_37|LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_38|LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_39;
        
        stParameters.Connection_Parameters_Provided_1M = true;
        stParameters.Conn_Interval_Min_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 4]);
        stParameters.Conn_Interval_Max_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 6]);
        stParameters.Conn_Latency_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 8]);
        stParameters.Supervision_Timeout_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 10]);
        
        u32PhyOffset += SINGLE_HCI_EXT_INIT_PHY_PARAM_LENGTH;
    }
    else
    {
        stParameters.Scan_PHY_Enabled_1M = false;
        stParameters.Connection_Parameters_Provided_1M = false;
    }
    
    if (Initiating_PHYs & (1 << 1))
    {
        stParameters.Connection_Parameters_Provided_2M = true;
        stParameters.Conn_Interval_Min_2M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 4]);
        stParameters.Conn_Interval_Max_2M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 6]);
        stParameters.Conn_Latency_2M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 8]);
        stParameters.Supervision_Timeout_2M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 10]);
        
        u32PhyOffset += SINGLE_HCI_EXT_INIT_PHY_PARAM_LENGTH;
    }
    else
    {
        stParameters.Connection_Parameters_Provided_2M = false;
    }
    
    if (Initiating_PHYs & (1 << 2))
    {
        stParameters.Scan_PHY_Enabled_Coded = true;
        stParameters.Scan_Interval_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 0]);
        stParameters.Scan_Window_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 2]);
        stParameters.Scan_Map_Coded = LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_37|LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_38|LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_39;
        
        stParameters.Connection_Parameters_Provided_Coded = true;
        stParameters.Conn_Interval_Min_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 4]);
        stParameters.Conn_Interval_Max_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 6]);
        stParameters.Conn_Latency_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 8]);
        stParameters.Supervision_Timeout_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 10]);
        
        u32PhyOffset += SINGLE_HCI_EXT_INIT_PHY_PARAM_LENGTH;
    }
    else
    {
        stParameters.Scan_PHY_Enabled_Coded = false;
        stParameters.Connection_Parameters_Provided_Coded = false;
    }
    
    uint8_t Status = rom_llc_ext_initiator_create_connection(&stParameters);
    
    rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_HCI_LE_Extended_Create_Connection, Status);
    
}



void rom_controller_command_VENDOR_Extended_Create_Connection(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    /*
    Initiator_Filter_Policy                 Size: 1 octet
    Own_Address_Type,                       Size: 1 octet
    Peer_Address_Type,                      Size: 1 octet
    Peer_Address,                           Size: 6 octets
    Initiating_PHYs,                        Size: 1 octet
    Scan_Interval[i]                        Size: 2 octets * bits set in Initiating_PHYs
    Scan_Window[i]                          Size: 2 octets * bits set in Initiating_PHYs
    Scan_Map[i]                             Size: 1 octet * bits set in Initiating_PHYs
    Conn_Interval_Min[i],                   Size: 2 octets * bits set in Initiating_PHYs
    Conn_Interval_Max[i],                   Size: 2 octets * bits set in Initiating_PHYs
    Conn_Latency[i],                        Size: 2 octets * bits set in Initiating_PHYs
    Supervision_Timeout[i],                 Size: 2 octets * bits set in Initiating_PHYs
    Minimum_CE_Length[i],                   Size: 2 octets * bits set in Initiating_PHYs
    Maximum_CE_Length[i],                   Size: 2 octets * bits set in Initiating_PHYs
    */
    #define VENDOR_EXT_INIT_PARAM_WITHOUT_PHY_LENGTH   (1+1+1+6+1)
    #define SINGLE_VENDOR_EXT_INIT_PHY_PARAM_WITH_MAP_LENGTH   (2+2+1+2+2+2+2+2+2)
    
    if ((NULL == Parameters)||(Parameter_Total_Length < VENDOR_EXT_INIT_PARAM_WITHOUT_PHY_LENGTH))
    {
        rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_VENDOR_Extended_Create_Connection, LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    uint8_t Initiating_PHYs = Parameters[9];
    if (Initiating_PHYs > 0x7)
    {
        rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_VENDOR_Extended_Create_Connection, LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    const uint8_t pu8InitiatingPhysNumTable[8] =
    {
        0,//0 : None
        1,//1 : 1M
        1,//2 : 2M
        2,//3 : 1M|2M
        1,//4 : Coded
        2,//5 : 1M|Coded
        2,//6 : 2M|Coded
        3,//7 : 1M|2M|Coded
    };
    uint8_t u8InitiatingPhysNum = pu8InitiatingPhysNumTable[Initiating_PHYs & 0x7];
    if (0 == u8InitiatingPhysNum)
    {
        rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_VENDOR_Extended_Create_Connection, LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    if ((VENDOR_EXT_INIT_PARAM_WITHOUT_PHY_LENGTH) + u8InitiatingPhysNum * SINGLE_VENDOR_EXT_INIT_PHY_PARAM_WITH_MAP_LENGTH != Parameter_Total_Length)
    {
        rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_VENDOR_Extended_Create_Connection, LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    stLlcExtInitiatorSetParameters_t stParameters = {0};
    stParameters.Initiator_Filter_Policy = Parameters[0];
    stParameters.Own_Address_Type = Parameters[1];
    stParameters.Peer_Address_Type = Parameters[2];
    memcpy(stParameters.Peer_Address, &Parameters[3], 6);
    
    uint32_t u32PhyOffset = VENDOR_EXT_INIT_PARAM_WITHOUT_PHY_LENGTH;
    if (Initiating_PHYs & (1 << 0))
    {
        stParameters.Scan_PHY_Enabled_1M = true;
        stParameters.Scan_Interval_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 0]);
        stParameters.Scan_Window_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 2]);
        stParameters.Scan_Map_1M = Parameters[u32PhyOffset + 4];
        
        stParameters.Connection_Parameters_Provided_1M = true;
        stParameters.Conn_Interval_Min_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 5]);
        stParameters.Conn_Interval_Max_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 7]);
        stParameters.Conn_Latency_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 9]);
        stParameters.Supervision_Timeout_1M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 11]);
        
        u32PhyOffset += SINGLE_VENDOR_EXT_INIT_PHY_PARAM_WITH_MAP_LENGTH;
    }
    else
    {
        stParameters.Scan_PHY_Enabled_1M = false;
        stParameters.Connection_Parameters_Provided_1M = false;
    }
    
    if (Initiating_PHYs & (1 << 1))
    {
        stParameters.Connection_Parameters_Provided_2M = true;
        stParameters.Conn_Interval_Min_2M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 5]);
        stParameters.Conn_Interval_Max_2M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 7]);
        stParameters.Conn_Latency_2M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 9]);
        stParameters.Supervision_Timeout_2M = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 11]);
        
        u32PhyOffset += SINGLE_VENDOR_EXT_INIT_PHY_PARAM_WITH_MAP_LENGTH;
    }
    else
    {
        stParameters.Connection_Parameters_Provided_2M = false;
    }
    
    if (Initiating_PHYs & (1 << 2))
    {
        stParameters.Scan_PHY_Enabled_Coded = true;
        stParameters.Scan_Interval_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 0]);
        stParameters.Scan_Window_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 2]);
        stParameters.Scan_Map_Coded = Parameters[u32PhyOffset + 4];
        
        stParameters.Connection_Parameters_Provided_Coded = true;
        stParameters.Conn_Interval_Min_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 5]);
        stParameters.Conn_Interval_Max_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 7]);
        stParameters.Conn_Latency_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 9]);
        stParameters.Supervision_Timeout_Coded = rom_llc_tool_read_little_endian_uint16(&Parameters[u32PhyOffset + 11]);
        
        u32PhyOffset += SINGLE_VENDOR_EXT_INIT_PHY_PARAM_WITH_MAP_LENGTH;
    }
    else
    {
        stParameters.Scan_PHY_Enabled_Coded = false;
        stParameters.Connection_Parameters_Provided_Coded = false;
    }
    
    uint8_t Status = rom_llc_ext_initiator_create_connection(&stParameters);
    
    rom_controller_event_HCI_Command_Status(LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS, 
            LLC_SPEC_5_1_HCI_OCF_VENDOR_Extended_Create_Connection, Status);
}


void patch_controller_command_HCI_LE_Create_Connection_Cancel(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
//typedef struct
//{
//    ////no param
//}__attribute__((packed))
//stHciCommandParamLECreateConnectionCancel_t;
    
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if (sizeof(stHciCommandParamLECreateConnectionCancel_t) != Parameter_Total_Length)
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Create_Connection_Cancel, 
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    
    bool bLegacyInitiatorEnabled = rom_llc_legacy_initiator_is_enabled();
    bool bExtInitiatorEnabled = rom_llc_ext_initiator_is_enabled();
    
    if(bLegacyInitiatorEnabled && bExtInitiatorEnabled)
    {
        uint8_t u8LegacyInitiatorStatus = rom_llc_legacy_initiator_create_connection_cancel();
        uint8_t u8ExtInitiatorStatus = rom_llc_ext_initiator_create_connection_cancel();
        
        if ((LLC_SPEC_5_1_ERROR_CODE_SUCCESS == u8LegacyInitiatorStatus) || (LLC_SPEC_5_1_ERROR_CODE_SUCCESS == u8ExtInitiatorStatus))
        {
            stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
            rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Create_Connection_Cancel, 
                sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
            return;
        }
        else
        {
            stReturnParameters.Status = u8LegacyInitiatorStatus;
            rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Create_Connection_Cancel, 
                sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
            return;
        }
    }
    else if (bLegacyInitiatorEnabled)
    {
        stReturnParameters.Status = rom_llc_legacy_initiator_create_connection_cancel();
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Create_Connection_Cancel, 
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    else if (bExtInitiatorEnabled)
    {
        stReturnParameters.Status = rom_llc_ext_initiator_create_connection_cancel();
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Create_Connection_Cancel, 
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    else
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Create_Connection_Cancel, 
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
}


void patch_controller_command_HCI_LE_Set_Host_Channel_Classification(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    stHciReturnParametersStatusOnly_t stReturnParameters = {0};
    if((NULL == Parameters) || (sizeof(stHciCommandParamLESetHostChannelClassification_t) != Parameter_Total_Length))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Host_Channel_Classification,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }

    stHciCommandParamLESetHostChannelClassification_t* pstParam = (stHciCommandParamLESetHostChannelClassification_t*)Parameters;
    
    if (rom_llc_tool_is_array_all_zero(pstParam->Channel_Map, 4) && (0 == (0x1F & pstParam->Channel_Map[4])))
    {
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Host_Channel_Classification,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
        return;
    }
    
    rom_ble_impl_set_default_channel_map(pstParam->Channel_Map);
    rom_llc_connection_role_update_channel_map_for_all_master(pstParam->Channel_Map);
    rom_llc_ext_adv_update_secondary_advertising_channel_map_for_all_extended_adv(pstParam->Channel_Map);
    
    stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    rom_controller_event_HCI_Command_Complete(LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS, LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Host_Channel_Classification,
            sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
}


bool extended_controller_interface_on_recv_le_control_command(EN_LLC_SPEC_5_1_HCI_OCF_T OCF, uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    switch(OCF)
    {
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Advertising_Set_Random_Address:
            rom_controller_command_HCI_LE_Set_Advertising_Set_Random_Address(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Parameters:
            rom_controller_command_HCI_LE_Set_Extended_Advertising_Parameters(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Data:
            rom_controller_command_HCI_LE_Set_Extended_Advertising_Data(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Response_Data:
            rom_controller_command_HCI_LE_Set_Extended_Scan_Response_Data(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Enable:
            rom_controller_command_HCI_LE_Set_Extended_Advertising_Enable(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Read_Maximum_Advertising_Data_Length:
            rom_controller_command_HCI_LE_Read_Maximum_Advertising_Data_Length(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Read_Number_of_Supported_Advertising_Sets:
            rom_controller_command_HCI_LE_Read_Number_of_Supported_Advertising_Sets(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Remove_Advertising_Set:
            rom_controller_command_HCI_LE_Remove_Advertising_Set(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Clear_Advertising_Sets:
            rom_controller_command_HCI_LE_Clear_Advertising_Sets(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Parameters:
            rom_controller_command_HCI_LE_Set_Extended_Scan_Parameters(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Enable:
            rom_controller_command_HCI_LE_Set_Extended_Scan_Enable(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Create_Connection_Cancel:
            patch_controller_command_HCI_LE_Create_Connection_Cancel(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Extended_Create_Connection:
            rom_controller_command_HCI_LE_Extended_Create_Connection(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Host_Channel_Classification:
            patch_controller_command_HCI_LE_Set_Host_Channel_Classification(Parameter_Total_Length, Parameters);
            return true;
        
        default:
            return false;
        
    }
}

bool extended_controller_interface_on_recv_vendor_command(EN_LLC_SPEC_5_1_HCI_OCF_T OCF, uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    switch(OCF)
    {
        case LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Advertising_Parameters:
            rom_controller_command_VENDOR_Set_Extended_Advertising_Parameters(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Scan_Parameters:
            rom_controller_command_VENDOR_Set_Extended_Scan_Parameters(Parameter_Total_Length, Parameters);
            return true;
        
        case LLC_SPEC_5_1_HCI_OCF_VENDOR_Extended_Create_Connection:
            rom_controller_command_VENDOR_Extended_Create_Connection(Parameter_Total_Length, Parameters);
            return true;
        
        default:
            return false;
    }
}

bool extended_controller_interface_on_recv_command(uint8_t OGF, uint16_t OCF, uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    if (!rom_ble_impl_is_command_supported(OGF, OCF))
    {
        return false;
    }
    
    switch(OGF)
    {
        case LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS:
            return extended_controller_interface_on_recv_le_control_command(OCF, Parameter_Total_Length, Parameters);
        
        case LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS:
            return extended_controller_interface_on_recv_vendor_command(OCF, Parameter_Total_Length, Parameters);
        
        default:
            return false;
    }
}


void extended_controller_interface_command_task_process(void* pParam)
{
    MLOG0I(0x34a0, "extended_controller_interface_command_task_process");
    if (NULL == pParam)
    {
        MLOG0I(0x34a1, "extended_controller_interface_command_task_process - pParam NULL");
        ////todo : unexpected situation
        return;
    }
    
    //run in llc task
    
    stLlcHciCommand_t* pstCommand = pParam;
    MLOG1616I(0x34a2, "extended_controller_interface_command_task_process - OGF:0x%02x OCF:0x%04x", pstCommand->u8OGF, pstCommand->u16OCF);
    bool bCommandSupported = extended_controller_interface_on_recv_command(pstCommand->u8OGF, pstCommand->u16OCF, pstCommand->u8ParameterTotalLength, pstCommand->pu8ParametersBuffer);
    rom_llc_hci_command_free(pstCommand);
    
    if (!bCommandSupported)
    {
        stHciReturnParametersStatusOnly_t stReturnParameters = {0};
        stReturnParameters.Status = LLC_SPEC_5_1_ERROR_CODE_UNKNOWN_HCI_COMMAND;
        
        rom_controller_event_HCI_Command_Complete(pstCommand->u8OGF, pstCommand->u16OCF, sizeof(stReturnParameters), (uint8_t*)&stReturnParameters);
    }
}


bool extended_controller_interface_check_if_consume_command_packet(uint8_t OGF, uint16_t OCF)
{
    switch(OGF)
    {
        case LLC_SPEC_5_1_HCI_OGF_LE_CONTROLLER_COMMANDS:
        {
            switch(OCF)
            {
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Advertising_Set_Random_Address:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Parameters:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Data:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Response_Data:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Advertising_Enable:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Read_Maximum_Advertising_Data_Length:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Read_Number_of_Supported_Advertising_Sets:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Remove_Advertising_Set:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Clear_Advertising_Sets:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Parameters:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Extended_Scan_Enable:
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Extended_Create_Connection:
                
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Create_Connection_Cancel:
                
                case LLC_SPEC_5_1_HCI_OCF_HCI_LE_Set_Host_Channel_Classification:
                    return true;
                
                default:
                    break;
            }
        }
            break;
        
        case LLC_SPEC_5_1_HCI_OGF_VENDOR_COMMANDS:
        {
            switch(OCF)
            {
                case LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Advertising_Parameters:
                case LLC_SPEC_5_1_HCI_OCF_VENDOR_Set_Extended_Scan_Parameters:
                case LLC_SPEC_5_1_HCI_OCF_VENDOR_Extended_Create_Connection:
                    return true;
                
                default:
                    break;
            }
        }
            break;
        
        default:
            break;
    }
    
    return false;
}

bool extended_controller_interface_consume_command_packet(uint8_t OGF, uint16_t OCF, uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    ////send to llc task
    MLOG1616I(0x34a3, "extended_controller_interface_consume_command_packet - OGF:0x%02x OCF:0x%04x", OGF, OCF);
    
    if (Parameter_Total_Length > rom_llc_hci_command_get_parameters_size())
    {
        MLOG1616E(0x34a4, "extended_controller_interface_consume_command_packet - length error, Parameter_Total_Length:%u max:%u", Parameter_Total_Length, rom_llc_hci_command_get_parameters_size());
        return false;
    }
    
    stLlcHciCommand_t* pstCommand = rom_llc_hci_command_alloc();
    if (NULL == pstCommand)
    {
        MLOG0W(0x34a5, "extended_controller_interface_consume_command_packet - alloc NULL");
        return false;
    }
    
    if ((NULL == pstCommand->pu8ParametersBuffer)||(Parameter_Total_Length > pstCommand->u16ParametersBufferSize))
    {
        ////todo : unexpected situation;
        rom_llc_hci_command_free(pstCommand);
        MLOG0E(0x34a6, "extended_controller_interface_consume_command_packet - buffer error");
        return false;
    }
    
    pstCommand->u8OGF = OGF;
    pstCommand->u16OCF = OCF;
    pstCommand->u8ParameterTotalLength = Parameter_Total_Length;
    memcpy(pstCommand->pu8ParametersBuffer, Parameters, Parameter_Total_Length);
    bool bSuccess = rom_llc_send_msg_to_llc_task(extended_controller_interface_command_task_process, pstCommand);
    if (!bSuccess)
    {
        rom_llc_hci_command_free(pstCommand);
        MLOG0W(0x34a7, "extended_controller_interface_consume_command_packet - send failed");
        return false;
    }
    return true;
}


bool extended_controller_interface_process_command_packet(uint8_t OGF, uint16_t OCF, uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    if (extended_controller_interface_check_if_consume_command_packet(OGF, OCF))
    {
        return extended_controller_interface_consume_command_packet(OGF, OCF, Parameter_Total_Length, Parameters);
    }
    else
    {
        return rom_controller_interface_process_command_packet(OGF, OCF, Parameter_Total_Length, Parameters);
    }
}


void extended_controller_interface(void)
{
    rom_hci_set_host_send_command_packet_callback(extended_controller_interface_process_command_packet);
}
