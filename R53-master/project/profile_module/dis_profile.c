/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     dis_profile.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "dis_profile.h"
#include "stack/ble/ble.h"
#include "version.h"
#include "seneasy_profile.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
_attribute_data_retention_ u8 my_deviceName   [5]  = {'A','P','H','R','C'};
_attribute_data_retention_ u8 my_FWRevtrs     [21] = {'E','U','0','1','-','R','5','3','E','B','S','Y','T','E','v','.','A','A','.','b','b'};
_attribute_data_retention_ u8 my_SWRevtrs     [21] = {'E','U','0','1','-','R','5','3','E','B','S','Y','T','E','v','.','A','A','.','b','b'};
_attribute_data_retention_ u8 my_HWRevtrs     [4]  = {'V','1','.','0'};
_attribute_data_retention_ u8 my_SerialNumtrs [4]  = {'E','U','0','1'};
#if (!CLOSE_CODE_PRINTF)
static u8 my_ManuNametrs  [7] = {'S','e','n','e','a','s','y'};
#endif
static const u8 my_ModelNumtrs [6] = {'T','e','l','i','n','k'};
_attribute_data_retention_ u8 rcu_project_name [5] = {'\0'};

const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;
const u16 my_PnPUUID        = CHARACTERISTIC_UUID_PNP_ID;
const u16 my_SysIDUUID      = CHARACTERISTIC_UUID_SYSTEM_ID;
const u16 my_ModelNUMUUID   = CHARACTERISTIC_UUID_MODEL_NUM_STRING;
const u16 my_SerialNUMUUID  = CHARACTERISTIC_UUID_SERIAL_NUM_STRING;
const u16 my_FWRevUUID      = CHARACTERISTIC_UUID_FW_REVISION_STRING;
const u16 my_HWRevUUID      = CHARACTERISTIC_UUID_HW_REVISION_STRING;
const u16 my_SWRevUUID      = CHARACTERISTIC_UUID_SW_REVISION_STRING;
const u16 my_ManuNameUUID   = CHARACTERISTIC_UUID_MANU_NAME_STRING;

/*============================================================================*
 *                     device Information  attribute values
 *============================================================================*/
const u8 my_PnCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(DeviceInformation_pnpID_DP_H), U16_HI(DeviceInformation_pnpID_DP_H),
    U16_LO(CHARACTERISTIC_UUID_PNP_ID), U16_HI(CHARACTERISTIC_UUID_PNP_ID)
};
const u8 my_ManuNameCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(DeviceInformation_manuName_DP_H), U16_HI(DeviceInformation_manuName_DP_H),
    U16_LO(CHARACTERISTIC_UUID_MANU_NAME_STRING), U16_HI(CHARACTERISTIC_UUID_MANU_NAME_STRING)
};
const u8 my_SerialNUMCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY,
    U16_LO(DeviceInformation_serialNum_DP_H), U16_HI(DeviceInformation_serialNum_DP_H),
    U16_LO(CHARACTERISTIC_UUID_SERIAL_NUM_STRING), U16_HI(CHARACTERISTIC_UUID_SERIAL_NUM_STRING)
};
const u8 my_FWRevCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(DeviceInformation_firmwareRev_DP_H), U16_HI(DeviceInformation_firmwareRev_DP_H),
    U16_LO(CHARACTERISTIC_UUID_FW_REVISION_STRING), U16_HI(CHARACTERISTIC_UUID_FW_REVISION_STRING)
};
const u8 my_HWRevCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(DeviceInformation_hardwareRev_DP_H), U16_HI(DeviceInformation_hardwareRev_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HW_REVISION_STRING), U16_HI(CHARACTERISTIC_UUID_HW_REVISION_STRING)
};
const u8 my_SWRevCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(DeviceInformation_softwareRev_DP_H), U16_HI(DeviceInformation_softwareRev_DP_H),
    U16_LO(CHARACTERISTIC_UUID_SW_REVISION_STRING), U16_HI(CHARACTERISTIC_UUID_SW_REVISION_STRING)
};

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  init_device_info
 * @param  none
 * @return none
 * @retval void
 */
void init_device_info (void)
{
    strncpy((char*)my_FWRevtrs, (char*)my_SerialNumtrs, 4);
    strncpy((char*)my_SWRevtrs, (char*)my_SerialNumtrs, 4);
    strncpy((char*)my_FWRevtrs + 4, "-", 1);
    strncpy((char*)my_SWRevtrs + 4, "-", 1);

    strncpy((char*)my_FWRevtrs + 5, FW_VERSION_STR,16);
    strncpy((char*)my_SWRevtrs + 5, FW_VERSION_STR,16);
#if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] MANUFACTURER_NAME: %s\n",my_ManuNametrs);
    printf("--->>> [work_debug] SERIAL_NUMBER_STR: %s\n",my_SerialNumtrs);
    printf("--->>> [work_debug] FIRMWARE_REVISION: %s\n",my_FWRevtrs);
    printf("--->>> [work_debug] SOFTWARE_REVISION: %s\n",my_SWRevtrs);
    u8 my_FWBuild[20] = {0};
    strncpy((char*)my_FWBuild, FW_VERSION_STR + 17,sizeof(FW_VERSION_STR) - 17);
    printf("--->>> [work_debug] BUILD_SW_REVISION: %s\n",my_FWBuild);
#endif
}

/******************************************************************
 * @brief  read flash rcu fw
 * @param  none
 * @return none
 * @retval void
 */
void read_flash_rcu_fw (void)
{
    memset(my_SerialNumtrs, 0, 30);
	flash_read_page(FLASH_TPV_RCU_FW_ADDR, 4, my_SerialNumtrs);
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] READ FLASH TPV_RCU_PROJECT = [");
    for (int i=0; i<4; i++)
    {
        printf("%02x ",my_SerialNumtrs[i]);
    }
    printf("]\r\n");
    #endif
    //支持 EU01/LM01 项目
    if ((strcmp((char*)my_SerialNumtrs,"EU01") != 0) && (strcmp((char*)my_SerialNumtrs,"LM01") != 0))
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Default_Project = [EU01]\r\n");
        #endif
        strncpy((char*)my_SerialNumtrs,"EU01",4); 
        seneasy_rcu_write_fw_addr(my_SerialNumtrs); 
    }
    //printf("--->>> [work_debug] TPV_RCU_PROJECT = [%s]\r\n",my_SerialNumtrs);

    for (int i=0; i<4; i++)
        rcu_project_name[i] = my_SerialNumtrs[i];
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] rcu_project_name= [%s]\r\n",rcu_project_name);
    #endif
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
