/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ota_handle.c
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
#include "ota_handle.h"
#include "flash_fw_check.h"
#include "ota.h"
#include "../app.h"
#include "../keyboard_rcu.h"

#if(OTA_DFU_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#define FW_SECTOR_LENGTH		MULTI_BOOT_ADDR_0x40000
#define USER_FW_ADDR			(FW_SECTOR_LENGTH - ota_program_offset)
#define START_UP_FLAG			(0x544c4e4b)

#define xstr(s) str(s)
#define str(s) #s
#define FW_VERSION_STR      xstr(VERSION)

OUTPUT_DEV_INFO_AAA output_dev_info =
{
    0,//u32 bin_crc;
    (u32) FW_VERSION_STR, //u32 fw version
};

u32 ota_program_offset;
u32 ota_24g_address = 0;
u32 flash_write_addr = 0;
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  ota_fw_init
 * @param  none
 * @return none
 * @retval void
 */
u8 ota_fw_init(void)
{	
	irq_disable();  // must, can't enter irq, because cstartup will be changed.
	u32 startup_flag = 0;
	flash_read_page(8, sizeof(startup_flag), (u8 *)&startup_flag);
	
	if(START_UP_FLAG == startup_flag)
		ota_program_offset = FW_SECTOR_LENGTH;

	u32 bin_size = 0;
	flash_read_page(USER_FW_ADDR + 0x18, 4, (u8 *)&bin_size);
	flash_read_page(USER_FW_ADDR + (bin_size - 4), 4, (u8 *)&output_dev_info.bin_crc);

	printf("***********************************************************\r\n");
	printf("***** firmware_addr: 0x%04x, ota_addr: 0x%04x *****\r\n",USER_FW_ADDR, ota_program_offset);
    printf("***** bin_size: %d, bin_crc = 0x%04x *******************\r\n",bin_size,output_dev_info.bin_crc);
    printf("***** fw_version: %s\r\n",output_dev_info.fw_version);
    printf("***********************************************************\r\n");

	return 1;
}

u8 ota_dfu_write(u8 ota_cmd, u8 *ota_buf)
{
	static u16 start_index=0;
	static u8 first_data_buf[4];
	static u32 fw_size=0;
	static u8 ota_error_flag=0;

	if(ota_cmd == 0x00)		
	{
		flash_write_addr=0;
		start_index=0;
		fw_size=0;

		ota_error_flag=OTA_SUCCESS;

        app_rcu_flash_unlock_init();//UNLOCK
		flash_erase_sector(ota_program_offset);

		fw_size = ota_buf[24] | (ota_buf[25]<<8) | (ota_buf[26]<< 16) | (ota_buf[27]<<24);

		printf("********** DFU OTA START --- fw_size: %d **********\r\n",fw_size);
        if((fw_size) > FW_SECTOR_LENGTH)
        {
            ota_error_flag=OTA_FW_SIZE_ERR;
            return ota_error_flag;
        }
		printf("********** usb_ota_start at=0x%08x **********\r\n",ota_program_offset);
	}
	if(ota_error_flag == OTA_SUCCESS)
	{
		if(ota_cmd == 0x02)		
		{
			printf("********** DFU OTA END **********\n");
			first_data_buf[3] = 0xFF;
			first_data_buf[2] = 0xFF;
			first_data_buf[1] = 0xFF;
			first_data_buf[0] = 0xFF;			
			flash_write_page(ota_program_offset+8, 4, first_data_buf);

			first_data_buf[3] = 0x54;
			first_data_buf[2] = 0x4C;
			first_data_buf[1] = 0x4e;
			first_data_buf[0] = 0x4b;
			flash_write_page(ota_program_offset+8, 4, first_data_buf);
			printf("********** write first packet to addr:0x%04x **********\n",ota_program_offset);

			u8 read_flash_buf[16];  
			flash_read_page(ota_program_offset+8, 4, read_flash_buf);
			printf("read_flash_buf: %01x,%01x,%01x,%01x\r\n",read_flash_buf[0], read_flash_buf[1],read_flash_buf[2], read_flash_buf[3]);
			if(memcmp(read_flash_buf, first_data_buf, 4))
			{
                //do not equal
				flash_erase_sector(ota_program_offset);
				ota_error_flag = OTA_WRITE_FLASH_ERR;
				return ota_error_flag;
			}

			u32 flag = 0;
			u8 ret = flash_fw_check(0xffffffff,ota_program_offset); 

			extern u32 fw_crc_init;
			printf("********** fw_crc_init = 0x%04x\r\n",fw_crc_init);
			flash_read_page(ota_program_offset + (fw_size - 4), 4, (u8 *)&output_dev_info.bin_crc);
			printf("***** fw_size: %d, bin_crc = 0x%04x *******************\r\n",fw_size,output_dev_info.bin_crc);

			if(ret==0)
			{
				extern u32 fw_crc_init;
				printf("********** fw_crc_init = 0x%04x\r\n",fw_crc_init);

				u8 fw_name[64];
				flash_read_page(ota_program_offset + (fw_size - 4 - 64), 64, (u8 *)&fw_name);
				u8 search_ota[8] = "V_SY_RCU";
				int found = 0;
				printf("search_ota: %s\r\n", search_ota);
				for(int i = 0; i <= (64 - 8); i++) {
					if(memcmp(&fw_name[i], search_ota, 8) == 0) {
						found = 1;
						break;
					}
				}
				printf("--->>> found: %d\r\n", found);
				if (found) {
					printf("********** dfu ota success, erase Firmware **********\r\n");
					flash_write_page(USER_FW_ADDR + 0x08, 4, (u8 *)&flag);	//Invalid flag
					app_ota_dfu_download_timer = 0;	
					gpio_write(GPIO_LED_1, LED_ON_LEVAL);
					WaitMs(3000);										
					start_reboot();
				} else {
					printf("********** usb ota fail,erase OTA **********\r\n");
					flash_erase_sector(ota_program_offset);
					ota_error_flag=OTA_FW_CHECK_ERR;
					app_ota_dfu_download_timer = 0;	
					gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
					start_reboot();
					return ota_error_flag;
				}			
			}
			else
			{
				printf("********** usb ota fail,erase OTA **********\r\n");
				flash_erase_sector(ota_program_offset);
				ota_error_flag=OTA_FW_CHECK_ERR;
				app_ota_dfu_download_timer = 0;	
				gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
				start_reboot();
				return ota_error_flag;
			}
		}
		else 
		{
            printf("[ota_dfu_write] flash_write_addr: 0x%04x\r\n",flash_write_addr + ota_program_offset);
            //for(u16 i = 0; i < 32; i++)
            //    printf("%01x ", ota_buf[i]);
            //printf("\r\n");
            {
                if((fw_size) < flash_write_addr)
                {
					rcu_ota_dfu_flag = 0x03;
                    ota_error_flag=OTA_SUCCESS;
                    return ota_error_flag;
                }

                if(ota_error_flag==OTA_SUCCESS)
                {
                    if((flash_write_addr % 4096)==0)
                    {
                        flash_erase_sector(flash_write_addr + ota_program_offset);
                    }
                    flash_write_page(flash_write_addr + ota_program_offset, 16+16, ota_buf);
                }
            }
            /*
            else
            {
                ota_error_flag=OTA_DATA_CRC_ERR;
                return ota_error_flag;
            }
            */
		}
	}
	
	return ota_error_flag;
}

unsigned short crc16_user (unsigned char *pD, int len)
{
	unsigned short crc16_poly_user[2] = {0, 0xa001}; 
	unsigned short crc = 0xffff;
	int i,j;

	for(j=len; j>0; j--)
	{
		unsigned char ds = *pD++;
		for(i=0; i<8; i++)
		{
			crc = (crc >> 1) ^ crc16_poly_user[(crc ^ ds ) & 1];
			ds = ds >> 1;
		}
	}
	
	return crc;
}


void report_ota_status(u8 ota_status)
{
	device_status = STATE_NORMAL;
	my_fifo_reset(&fifo_km);
	p_km_data->cmd = D24G_OTA_CMD;
	u8 temp_buff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	temp_buff[0] = 0xFF;
	temp_buff[1] = ota_status;

	temp_buff[5] = (flash_write_addr >> 24) & 0xFF;
	temp_buff[4] = (flash_write_addr >> 16) & 0xFF;
	temp_buff[3] = (flash_write_addr >> 8) & 0xFF;
	temp_buff[2] = flash_write_addr & 0xFF;

	memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
	memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);   

	#if (IS_SINGLE_GPIO_CHANGE_MODE)
		sing_gpio_mode_poll();
	#endif
		push_report_fifo();

	if ((device_status == STATE_NORMAL)) {
		reset_idle_status();
		my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
	}		
}

#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
