#ifndef __OTA_SLAVER_H__
#define __OTA_SLAVER_H__
#include "driver.h"

#define OTA_FLASH_START_ADD_FIRM_0 0x00000
#define OTA_FLASH_START_ADD_FIRM 0x10000
#define OTA_FIRMWARE_FLASH_ADDRESS 0x1F030
#define OTA_MAC_FLASH_ADDRESS 0x1F000

#define FLASH_SECTION_SIZE 128
#define OTA_FLASH_SECTION_SIZE 48
#define READ_FLASH_SIZE 16
#define DRIVE_ADDR_OFFSER 8

#define OTA_SEND_DMA_LEN READ_FLASH_SIZE + 6
#define OTA_SEND_RF_LEN OTA_SEND_DMA_LEN - 1

#define DEVICE_DRIVER_ADDRESS 0x08
#define OTA_ERASE_LEN OTA_FLASH_SECTION_SIZE * 2

#define OTA_ERASE_SEATOR 4096
#define OTA_ERASE_BYTE_SIZE OTA_ERASE_SEATOR / 16

#define OTA_FREQ_INTERVAL 3000
#define OTA_ACK_RETRANMISSION 0x40

// OTA 软件号
#define OTA_SOFTWARE_ID 0x11166030

// 全局变量声明
extern bool ota_slaver_start;

extern volatile u8 ota_enable_begin;
extern volatile u16 address_offset;
extern volatile u32 ota_send_neef;

extern volatile u32 ota_start_time;

// Send_data_structure
typedef struct
{
    u32 OTA_DMA_LEN;
    u8 OTA_RF_LEN;
    u32 SOFTWARE_ID;
    u32 OTA_MAC_ADDR;
    u16 OTA_LEN_CHECK;

} SEND_OTA_DATE;

// OTA 发送状态
typedef enum
{
    OTA_WAKE_STATE_IDLE = 0,
    OTA_WAKE_STATE_SEND_DATA,
    OTA_WAKE_STATE_SEND_FAIL,
} OTA_WAKE_STATE;
extern OTA_WAKE_STATE Ota_wake_state;

typedef struct
{
    u32 OTA_MCK_ADDR;
    u8 OTA_DATA_PACKET[READ_FLASH_SIZE];
    u16 OTA_LEN_CHECK;
} ACK_OTA_DATE;

#define SEND_OTA_DATE_SIZE sizeof(SEND_OTA_DATE);
extern SEND_OTA_DATE Send_ota_date;

extern void OTA_LOOP(void);
void RAED_OTA_FIRMWARE_DATE(int address_offset);
void OTA_BREACH_LED_SHOW(void);
void Channel_modulation(void);
extern void OTA_INIT(void);
void OTA_FIRMWARE_ADDRESS_CORREAT(void);
void OTA_FIRMWARE_ERASE_SECTOR(void);
_attribute_ram_code_sec_ void OTA_RF_TX(void);
_attribute_ram_code_sec_ void OTA_SLAVER_START(void);
_attribute_ram_code_sec_ void OTA_ACK_FAIL_EVENT(void);
_attribute_ram_code_sec_ void OTA_FIRMWARE_DATE_ACK_END(void);
_attribute_ram_code_sec_ void WIRTE_OTA_FIRMWARE_DATE(ACK_OTA_DATE *ota_ack_date);
extern _attribute_ram_code_sec_ u8 OTA_RF_RX(rf_packet_t *p_rf_rx_data);
extern _attribute_ram_code_sec_ void OTA_UPDATE_IRP_HANDLE(u8 * raw_pkt ,rf_packet_t *p_rf_rx_data);
extern void OTA_fACTORY_ENABLE_EVENT();
#endif