/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ota_handle.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef OTA_HANDLE_H_
#define OTA_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../tl_common.h"

#if(OTA_DFU_ENABLE)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define DFU_OTA_PDU_LENGTH      0x400 //1024
#define NOTIFY_DG_BLE_OTA		0x01
#define NOTIFY_DG_DFU_OTA		0x02
#define NOTIFY_RCU_DFU_OTA		0x03

typedef struct
{
    u32 bin_crc;
    u32 fw_version;
} OUTPUT_DEV_INFO_AAA;
extern OUTPUT_DEV_INFO_AAA output_dev_info;

typedef enum
{
	SUCCESS_OTA=0,
	BIN_CRC_ERROR_OTA=1,
	DATA_CRC_ERROR_OTA=2,
	FW_SIZE_ERROR_OTA=3,
	LOST_DATA_OTA=4,
}OTA_ERROR;

#define USB_OTA_LENGTH  64

typedef struct{
	u8	report_id;
	u8 	opcode;
	u16	length;					
	u8	dat[DFU_OTA_PDU_LENGTH];
}usb_data_t;

typedef struct
{
	u16 cmd;
	u8 buf[DFU_OTA_PDU_LENGTH];
	u16 crc;
}ota_data_st;

typedef struct
{	
	u8 cmd;//data type
	u8 seq_no;
	u8 pno_no;	
	u8 tick_0;
	u8 tick_1;
	u8 chn;
	u8 buf[40];
} ota_24G_data_t;

extern u32 ota_24g_address;
extern u32 flash_write_addr;
/*============================================================================*
 *                          Functions
 *============================================================================*/
u8 ota_fw_init(void);
u8 ota_dfu_write(u8 ota_cmd, u8 *ota_buf);
unsigned short crc16_user (unsigned char *pD, int len);

void report_ota_status(u8 ota_status);
#endif
#endif /* PWM_HANDLE_H_ */
