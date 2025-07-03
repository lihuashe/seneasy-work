/********************************************************************************************************
 * @file     AAA_app.c
 *
 * @brief    This is the source file for KMD SDK
 *
 * @author	 KMD GROUP
 * @date         01,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#include "AAA_project_config.h"


#define  SIMU_DATA_DEBUG_ENABLE 0

#define  HOPING_FREQ    0

#define  RF_FAST_TX_SETTING_ENABLE  1
#define  RF_FAST_RX_SETTING_ENABLE  1

#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)

volatile u32 tick_rf_fast_setting=0;
volatile int rf_fast_setting_flag=0;//0 init 1:time out  2:enable fast set
#endif

//t_mouse_inf mouse_inf,last_mouse_inf; 

u8 usb_device_status;
u8 need_enter_suspend_flag=0;

volatile int rx_fail=0;
volatile u8	chn_mask = 0x80;
volatile u8 last_chn_mask=0;
u8 deep_flag = POWER_ON_ANA_AAA;

// u32 rx_tick=0;
//u32 rx_mouse_cnt=0;
//u32 mouse_to_pc_cnt=0;
//u8 has_write_cfg=0;
//u8 debug_report_rate=0;
//u32 tick_pc_breath=0;


#if (AES_METHOD == 0)
	#define ID_LEN 4 
#elif (AES_METHOD == 1)
	#define ID_LEN 16 
#endif
typedef void (*callback_rx_func)(u8 *);
u8 * raw_pkt=NULL;
callback_rx_func p_post = NULL;
u8 bin_crc[4];
u32 pair_accesscode=0x39517695;

USB_FIFO_DATA_S  usb_fifo_aaa;
RF_RX_FIFO_DATA_S rf_rx_fifo_aaa;
u8 mouse_not_release=0;
volatile u32 dongle_id=0;

volatile u32 kb_tick=0;
volatile u32 ms_tick=0;
 s16 x_smoother;
 s16 y_smoother;

// u8 allow_suspend = 0;
//u32 tick_allow_suspend = 0;
//u32 tick_host_start_power_on=0;
//u32 tick_host_start_power_off=0;
u32 tick_suspend_interval;
static mouse_data_t  mouse_last_data={0,0,0,0};	
static u8 consume_last_data[2]={0,0};	
static u8 sys_last_data=0;	
static u8 normal_last_data[8]={0,0,0,0,0,0,0,0};

callback_rx_func p_keyboard_data_handle_post = NULL;
volatile u8 to_usb_keyboard_data[8];

callback_rx_func p_mouse_data_handle_post = NULL;
volatile u8 to_usb_mouse_data[8];


//u8 need_wake_up_host_flag=0;
//u32 tick_resume_host;
u8 allow_send_to_usb_flag=1;
int	custom_binding_idx = 0;

u8 encKey[16]={
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10};
u8 pub_key[16] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
};

rf_packet_t	ack_rf_dat ;

#if (HOPING_FREQ==1)
volatile u32 tick_fh = 0;
frqHopping_data_t  frq_hopping_data = 
{
	FRQ_HOPPING_MODE_NORMAL,
	8,
	2,
	0,
	120000,
	0
};
volatile u8		ll_chn_sel;
volatile u8		ll_chn_pkt[16] = {0};
volatile int		device_packet_received;
#endif	
volatile unsigned char  rf_rx_buff[PKT_BUFF_SIZE*2] __attribute__((aligned(4)));
int		rf_rx_wptr;
volatile u8		host_channel = 0;

AES_KEY key_enc[2];

u32 power_on_tick=0;
volatile u8 rf_pair_enable=1;
u8 pair_success_flag;
custom_cfg_t user_config;

u32 ota_program_offset;
//int ota_program_bootAddr;

//u32 rx_cnt_aaa=0;
//u16 rf_state_aaa=0;
//s8 manual_paring_enable;
//s8 golden_dongle_enable;
unsigned short crc16_user (unsigned char *pD, int len)
{
	unsigned short 		crc16_poly_user[2] = {0, 0xa001}; 
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

#if OTA_ENABLE_AAA
int notify_rsp_buf2hci(u8 *data,u16 length);
void notify_rsp_update ();

//bool fw_start_addr_offset=0;	//0-FW_ADDR:0X00000  1-FW_ADDR:0x10000
#define FW_SECTOR_LENGTH		0X10000	//64k
#define USER_FW_ADDR			(FW_SECTOR_LENGTH-ota_program_offset)
#define START_UP_FLAG			(0x544c4e4b)
#define MCU_RAM_START_ADDR		(0x840000)  // for 825x: 0x840000, don't change.

static u32 notify_rsp_tick;


u32 blt_ota_start_tick;

u8 usb_start_flag = 0;
u8 ui_ota_is_working = 0;

typedef	struct {
	u32		size;
	u16		num;
	u8		wptr;
	u8		rptr;
	u8*		p;
}	my_fifo_t;

void my_fifo_reset (my_fifo_t *f)
{
	
	f->wptr = 0;
	f->rptr = 0;
}

void my_fifo_init (my_fifo_t *f, int s, u8 n, u8 *p)
{
	f->size = s;
	f->num = n;
	f->wptr = 0;
	f->rptr = 0;
	f->p = p;
}

u8* my_fifo_wptr (my_fifo_t *f)
{
	if (((f->wptr - f->rptr) & 255) < f->num)
	{
		return f->p + (f->wptr & (f->num-1)) * f->size;
	}
	return 0;
}

void my_fifo_next (my_fifo_t *f)
{
	f->wptr++;
}

int my_fifo_push (my_fifo_t *f, u8 *p, int n)
{
	if (((f->wptr - f->rptr) & 255) >= f->num)
	{
		return -1;
	}

	if (n >= f->size)
	{
		return -1;
	}
	u8 *pd = f->p + (f->wptr++ & (f->num-1)) * f->size;
	//*pd++ = n & 0xff;
	//*pd++ = (n >> 8) & 0xff;
	memcpy (pd, p, n);
	return 0;
}

u8 my_fifo_number(my_fifo_t *f)
{
	u8 num =(f->wptr - f->rptr);
	return num;
}

void my_fifo_pop (my_fifo_t *f)
{
	f->rptr++;
}

u8 * my_fifo_get (my_fifo_t *f)
{
	if (f->rptr != f->wptr)
	{
		u8 *p = f->p + (f->rptr & (f->num-1)) * f->size;
		return p;
	}
	return 0;
}

extern my_fifo_t usb_to_pc_data;
#define		MYFIFO_INIT(name,size,n)		u8 name##_b[size * n]={0};my_fifo_t name = {size,n,0,0, name##_b}
MYFIFO_INIT(usb_to_pc_data, 80, 4);

int usb_ota_resut(u8 result)
{
	usb_data_t p;
	int count=0;
	u32 tick=0;
	tick=clock_time();
	int ret=0;
	p.report_id=OTA_REPORT_ID;
	p.opcode=0x02;
	p.length=3;
	p.dat[0]=0x06;
	p.dat[1]=0xff;
	p.dat[2]=result;
    while(1)
	{
		usb_handle_irq();
		if(clock_time_exceed(tick, 50000))
		{
			tick=clock_time();
			if(count==0)
			{
				ret=notify_rsp_buf2hci(&p.report_id,USB_OTA_LENGTH);
				if(ret)
				{
					count=1;
				}
			}
			else
			{
				break;
			}
		}
		
	}
	//app_debug_ota_result(result);
	printf("---usb_ota_result=0x%01x.\r\n",result);
	
	analog_write(SYS_DEEP_ANA_REG, OTA_STOP_AAA);
	
	start_reboot();
	
	return ret;
}

void usb_update_loop()
{
	//static u32 dbg_m_loop;
	//dbg_m_loop ++;
	if((usb_start_flag == 1) && clock_time_exceed(blt_ota_start_tick , 120000000))
	{
		usb_ota_resut(OTA_TIMEOUT);
	}

	//proc_host();
	
	notify_rsp_update();  //
}

void load_next_firmware(u32 addr_load, u32 ramcode_size)
{
	//func_test();	// just for compile error indication when over flow, but make no sense now.
	irq_disable();	// must, can't enter irq, because cstartup will be changed.

	// copy ramcode of next firmware.
	#if 0
		boot_load_memcpy4((u8 *)MCU_RAM_START_ADDR, (u8 *)addr_load, ramcode_size);	// have been sure that ramcode_size is 4 bytes align.
	#else
		flash_read_page(addr_load, ramcode_size, (u8 *)MCU_RAM_START_ADDR); // if use flash read, all flash API need to be located at "_attribute_no_over_written_code_"
	#endif

	printf("---addr_load=0x%03x,ramcode_size=%d.\n",addr_load,ramcode_size);
	//while(1);
#if 0 //no need restart
	// jump to next image
	WRITE_REG8(0x602, 0x88);	// reboot from RAM, and the IO setting will remain unchanged, such as GPIO's function of DP.
    while(1){// have been reboot before, just make sure not go ahead.
        //static volatile u32 boot_load_err;boot_load_err++; // can not use global variable here, incase there is some delay for reboot.
    };
#endif
}

u8 run_app_code()
{	
	irq_disable();  // must, can't enter irq, because cstartup will be changed.

	u32 startup_flag = 0;
	
	flash_read_page(8, sizeof(startup_flag), (u8 *)&startup_flag);
	
	if(START_UP_FLAG == startup_flag)
	{
		//fw_start_addr_offset = 1;
		ota_program_offset=FW_SECTOR_LENGTH;
	}

	//ota_program_offset = ota_program_bootAddr = USER_OTA_ADDR;

	printf("***********************************************************\r\n");
	printf("****** firmware_addr = 0x%03x, ota_addr = 0x%03x ******\r\n",USER_FW_ADDR,ota_program_offset);
	printf("***********************************************************\r\n");

	//crc32_init(0x04C11DB7, AA_crc_table);  //1600us

	u32 bin_size = 0;
	flash_read_page(USER_FW_ADDR + 0x18, 4, (u8 *)&bin_size);
	flash_read_page(USER_FW_ADDR + (bin_size - 4), 4, (u8 *)&output_dev_info.bin_crc);
	//flash_read_page(offset + 2, 4, (u8 *)&output_dev_info.fw_version);

	printf("---bin_crc=0x%04x.\r\n",output_dev_info.bin_crc);

	return 1;
}

void notify_rsp_buf_init()
{
    //memset(notify_rsp_buf, 0, sizeof(notify_rsp_buf));
    //notify_rsp_buf_wptr = notify_rsp_buf_rptr = 0;
    my_fifo_reset(&usb_to_pc_data);
}

int notify_rsp_buf2hci(u8 *data,u16 length)
{
	if(reg_irq_src & FLD_IRQ_USB_PWDN_EN)
	{
		//user_resume_host();
		reg_usb_ep_ctrl(USB_EDP_SPP_IN) = 0;
		return 1;
	}

	if (!reg_usb_host_conn)
	{
		reg_usb_ep_ctrl(USB_EDP_SPP_IN) = 0;
		return 1;
	}
	
	if(usbhw_is_ep_busy(USB_EDP_SPP_IN))
	{
		return 0;
	}

	reg_usb_ep_ptr(USB_EDP_SPP_IN) = 0;

	//printf("tx=0x");
	for(u8 i=0;i<length;i++)
	{
		//printf("%01x",data[i]);
		reg_usb_ep_dat(USB_EDP_SPP_IN) = data[i];
	}
	//printf("\r\n");	
	
	usbhw_data_ep_ack(USB_EDP_SPP_IN);

	return 1;
}

void app_enter_ota_mode(void)
{
#if MODULE_WATCHDOG_ENABLE
	wd_stop();
#endif

	ui_ota_is_working = 1;

#if (BLT_APP_LED_ENABLE && BLE_OTA_LED_DEBUG)
	gpio_write(PIN_BLE_LED, 1);
#endif

	printf("---start ota\r\n");

#if (APP_FLASH_LOCK_ENABLE)
	flash_unlock_init();
	WaitMs(15);
#endif

	blt_ota_start_tick = clock_time();  //mark time
	usb_start_flag = 1;  //mark time
	
	//bls_ota_setTimeout(120 * 1000 * 1000); //set OTA timeout  15 seconds
}
void app_debug_ota_result(int result)
{
    irq_disable();
    //flash_erase_sector(CFG_DEVICE_MODE_ADDR);
#if MODULE_WATCHDOG_ENABLE
    wd_stop();
#endif

#if(BLE_OTA_LED_DEBUG && BLT_APP_LED_ENABLE)
    gpio_set_output_en(PIN_BLE_LED, 1);
    if (result == OTA_SUCCESS)  //OTA success
    {
        gpio_write(PIN_BLE_LED, 1);
        sleep_us(2000000);  //led on for 2 second
        gpio_write(PIN_BLE_LED, 0);

        printf("ota success\r\n");
    }
    else   //OTA fail
    {
        for (int i = 0; i < 4; i++)
        {
            gpio_write(PIN_BLE_LED, 1);
            sleep_us(250000);
            gpio_write(PIN_BLE_LED, 0);
            sleep_us(250000);
        }

        printf("ota fail=%x\r\n",result);
    }
#endif
}

u8 usb_ota_write(usb_data_t *p_usb)
{
	static	u16 ota_index=0;
	static u16 start_index=0;
	static u32 flash_write_addr=0;
	static u8 first_data_buf[16];
	static u32 fw_size=0;
	static u8 ota_error_flag=0;
	ota_data_st *pd=(ota_data_st*)&p_usb->dat[0];

	if((pd->cmd== CMD_OTA_START)&&(p_usb->length==2))		
	{
		blt_ota_start_tick = clock_time();  //mark time
		usb_start_flag = 1;  //mark time
		notify_rsp_buf_init();
	#if MODULE_WATCHDOG_ENABLE
		wd_stop();
	#endif
		ota_index=0;
		
		flash_write_addr=0;
		start_index=0;
		fw_size=0;
		//fw_check_value=0;
		//fw_cal_crc=0xffffffff;
		ota_error_flag=OTA_SUCCESS;
		flash_erase_sector(ota_program_offset);
		app_enter_ota_mode();
		
		printf("******** usb_ota_start at=0x%03x ********\r\n",ota_program_offset);
		return ota_error_flag;
	}
	else if(ota_error_flag==OTA_SUCCESS)
	{
		if((pd->cmd == CMD_OTA_END)&&(p_usb->length==6))		
		{
			printf("---usb_ota_end\r\n");
			u32 *telink_mark=(u32*)&first_data_buf[8];
			if(telink_mark[0]!=0x544c4e4b)
			{
				ota_error_flag=OTA_FIRMWARE_MARK_ERR;
				return ota_error_flag;
			}
			u32 real_bin_size=0;
			//real_bin_size=fw_size-4;
			real_bin_size=fw_size;
			//printf("---real_bin_size=%d_%d.fw_size=%d.\n",real_bin_size,(start_index*16),fw_size);

			if((real_bin_size%16)==0) //16�ı���
			{
				printf("---1 real_bin_size=%d_%d.\n",real_bin_size,((start_index+1)*16));
				
				if(real_bin_size!=((start_index+1)*16)) //
				{
					ota_error_flag=OTA_FW_SIZE_ERR;
					return ota_error_flag;
				}
			}
			else
			{
				printf("---2 real_bin_size=%d_%d.\n",(real_bin_size-(real_bin_size%16)),((start_index+1)*16));
				
				if((real_bin_size-(real_bin_size%16))!=(start_index*16))
				{
					ota_error_flag=OTA_FW_SIZE_ERR;
					return ota_error_flag;
				}
			}

			printf("******** write first packet to addr:0x%03x ********\n",ota_program_offset);
			flash_write_page(ota_program_offset,16,first_data_buf);

			u8 read_flash_buf[16];  
			flash_read_page(ota_program_offset,16, read_flash_buf);

			if(memcmp(read_flash_buf, first_data_buf, 16))
			{  //do not equal
				flash_erase_sector(ota_program_offset);
				ota_error_flag=OTA_WRITE_FLASH_ERR;
				return ota_error_flag;
			}

			u32 flag = 0;

			u8 ret=flash_fw_check(0xffffffff,ota_program_offset); 

			if(ret==0)
			{
				extern u32 fw_crc_init;
				printf("---fw_crc_init=0x%04x\r\n",fw_crc_init);

				printf("---usb ota success,erase Firmware\r\n");
				flash_write_page(USER_FW_ADDR + 0x08, 4, (u8 *)&flag);	//Invalid flag
				usb_ota_resut(OTA_SUCCESS);
			}
			else
			{
				printf("---usb ota fail,erase OTA\r\n");
				flash_erase_sector(ota_program_offset);
				ota_error_flag=OTA_FW_CHECK_ERR;
				return ota_error_flag;
				
			}
		}
		else 
		{
			if((p_usb->length%20)!=0)
			{
				ota_error_flag=OTA_PDU_LEN_ERR;
				return ota_error_flag;
			}
			u8 cnt=p_usb->length/20;
			
			for(u8 i=0;i<cnt;i++)
			{
				pd=(ota_data_st*)&p_usb->dat[20*i];
				//printf("pd_crc=%02x.user_crc=0x%02x.\n",pd->crc,crc16_user((u8*)&pd->cmd,18));
				if(crc16_user((u8*)&pd->cmd,18)==pd->crc)
				{
					
					if(pd->cmd==0x0000)//first_data
		 			{
						memcpy(first_data_buf,pd->buf,16);
						start_index=0;
		 			}
					else
					{
						if((start_index+1)!=pd->cmd)
						{
							ota_error_flag=OTA_DATA_PACKET_SEQ_ERR;
							return ota_error_flag;
						}
						start_index=pd->cmd;
						//printf("---start_index=%d.\n",start_index);
						if(pd->cmd==0x0001)//second 
						{
							fw_size=pd->buf[8] | (pd->buf[9] <<8) |(pd->buf[10]<<16) | (pd->buf[11]<<24);
							printf("---fw_size=%d.\n",fw_size);
							//if((fw_size)>ota_program_bootAddr)//128k
							if((fw_size)>FW_SECTOR_LENGTH)//128k
							{
								ota_error_flag=OTA_FW_SIZE_ERR;
								return ota_error_flag;
							}
						}
						
						if((fw_size)<flash_write_addr)
						{
							ota_error_flag=OTA_FW_SIZE_ERR;
							return ota_error_flag;
						}
						
						if(ota_error_flag==OTA_SUCCESS)
						{
							if((flash_write_addr%4096)==0)
					   		{
								flash_erase_sector(flash_write_addr+ota_program_offset);
					   		}
					   		flash_write_page(flash_write_addr+ota_program_offset,16,pd->buf);
						
						}
						
					}
					
					flash_write_addr+=16;
				}
				else
				{
					ota_error_flag=OTA_DATA_CRC_ERR;
					return ota_error_flag;
				}
			}
		}
	}
	
	return ota_error_flag;
}

void usb_data_handle(usb_data_t *p,u16 length)
{
	if((p->report_id==OTA_REPORT_ID)&&(length==USB_OTA_LENGTH))
	{
		if((p->opcode==0x01)&&(p->length==0))//get firmversion
		{
			p->length=8;
			memcpy(&p->dat[0], (u8 *)&output_dev_info.fw_version, 4);
			memcpy(&p->dat[4], (u8 *)&output_dev_info.bin_crc, 4);
		}
		else if(p->opcode==0x02)
		{
			u8 ota_error_flag=usb_ota_write(p);
			
			if(ota_error_flag)
			{
				usb_ota_resut(ota_error_flag);
			}
		}
	
		if(notify_rsp_buf2hci(&p->report_id, length)==0)
		{
			notify_rsp_tick = clock_time()|1;
			my_fifo_push(&usb_to_pc_data, &p->report_id, length);
		}
	}
}


void app_hid_set_report_handle(u8 data_request,u8 report_id,u16 length)
{
	static u16 ep0_out_data_len=0;
	static u8  ep0_out_data_buf[USB_OTA_LENGTH]={0};
	static u16 ep0_out_index=0;
	u8 i;

	if (data_request)
	{
		if(ep0_out_data_len < USB_OTA_LENGTH)
		{
			for(i=0;i<8;i++)
			{
				ep0_out_data_buf[ep0_out_index]=usbhw_read_ctrl_ep_data();
				
				ep0_out_index++;
			}

			ep0_out_data_len += 8;

			if(ep0_out_data_len == USB_OTA_LENGTH)
			{
				ep0_out_index = 0;
				ep0_out_data_len = 0;
				
				//printf("rx=0x");
				//for(i=0;i<length;i++)
				{
					//printf("%01x",ep0_out_data_buf[i]);
				}
				//printf("\r\n");

				usb_data_t *p=(usb_data_t *)&ep0_out_data_buf[0];
				usb_data_handle(p,length);
			}
		}
	}
	else
	{
		ep0_out_data_len=0;
		ep0_out_index=0;
		
		printf("set_report_cmd=0x%01x,length=%d.\r\n",report_id,length);
	}
}

void notify_rsp_update ()
{
    if(notify_rsp_tick&&clock_time_exceed(notify_rsp_tick, 600))
	{
		u8*p=my_fifo_get(&usb_to_pc_data);
		
		if(p)
		{
			int length=p[0]+(p[1]<<8);
			
			if(notify_rsp_buf2hci(&p[2],length))
			{
				my_fifo_pop(&usb_to_pc_data);
				
				notify_rsp_tick = 0;
			}
		}
		else
		{
			notify_rsp_tick = 0;
		}	
	}
}
#endif

u8 update_channel_mask(u8 mask, u8 chn, u8 *chn_pkt)
{
#if (HOPING_FREQ==1)	
	static int ll_chn_sel_chg, ll_chn_hold;
	if (clock_time_exceed(tick_fh, frq_hopping_data.fre_hp_always_time_us))  {
		chn_pkt[ll_chn_sel_chg] = 0;
		chn_pkt[!ll_chn_sel_chg] = frq_hopping_data.frq_hp_chn_pkt_rcvd_max;
	}
	if (ll_chn_hold) {
		ll_chn_hold--;
		chn_pkt[0] = chn_pkt[1] = 0;
	}
	int diff = chn_pkt[ll_chn_sel] - chn_pkt[!ll_chn_sel];
	int hit_th = diff > frq_hopping_data.frq_hp_hit_diff_num;
	if (chn_pkt[ll_chn_sel] >= frq_hopping_data.frq_hp_chn_pkt_rcvd_max || hit_th) {
		int dual_chn[2];
		dual_chn[0] = mask & 0x0f;
		dual_chn[1] = mask >> 4;
		if (hit_th) { //change channel
			ll_chn_hold = 32;//32ms
			chn = dual_chn[!ll_chn_sel];
			for (int i=0; i<8; i++) {
				chn = LL_NEXT_CHANNEL (chn);
				if ((ll_chn_sel && chn != dual_chn[1])) {
					mask = (mask & 0xf0) | chn;
					break;
				}
				else if (!ll_chn_sel && chn != dual_chn[0]) {
					mask = (mask & 0x0f) | (chn << 4);
					break;
				}
			}
			tick_fh = clock_time ();
			ll_chn_sel_chg = !ll_chn_sel;		 //remember latest channel change
		}
		chn_pkt[0] = chn_pkt[1] = 0;
	}
#endif
return mask;
}

#define FRE_OFFSET 	0
const unsigned char rf_chn[16] =
{
    FRE_OFFSET + 5, FRE_OFFSET + 9, FRE_OFFSET + 13, FRE_OFFSET + 17,
    FRE_OFFSET + 22, FRE_OFFSET + 26, FRE_OFFSET + 30, FRE_OFFSET + 35,
    FRE_OFFSET + 40, FRE_OFFSET + 45, FRE_OFFSET + 50, FRE_OFFSET + 55,
    FRE_OFFSET + 60, FRE_OFFSET + 65, FRE_OFFSET + 70, FRE_OFFSET + 76,
};
	
_attribute_ram_code_sec_ void rf_set_channel_aaa(signed char chn, unsigned short set)
{
	rf_set_channel (rf_chn[chn],0);
	//rf_set_channel (chn, set);
}

void user_rf_ack_send(int rf_length)
{
	ack_rf_dat.dma_len = rf_length+1;
	ack_rf_dat.rf_len = rf_length;
    PIN_DEBUG_RF_TX_LEVEL(1);

	rf_set_tx_pipe(0);
	rf_set_tx_rx_off();
	rf_start_stx((void *)&ack_rf_dat.dma_len, clock_time() );
}

//rf_set_channel(59,0);

void clear_pair_enable_flag()
{
	printf("---clear_pair_enable_flag\n");
	rf_pair_enable=0;
	pair_success_flag=0;
	//if(user_config.paring_type!=GOLDEN_DONGLE_PARING_TYPE)
	{
		//rf_rx_acc_code_enable(0x06);//mouse&keboard
	}
}

_attribute_ram_code_sec_ void check_first_normal_data()
{
	
#if (GOLD_TEST_DONGLE==0)
	static u32 first_rx_normal_data_tick=0;
	if(first_rx_normal_data_tick==0)
	{
	    first_rx_normal_data_tick=clock_time()|1;
	}
	else if(rf_pair_enable&&clock_time_exceed(first_rx_normal_data_tick,1000000))
	{
	    clear_pair_enable_flag();
	}
#endif
}

u8 push_usb_fifo_aaa(u8 type,u8 *buf,u8 len)
{
	USB_DATA_S *p=(USB_DATA_S*)usb_fifo_aaa.fifo[usb_fifo_aaa.wptr&(USB_FIFO_NUM-1)];
	p->type=type;
	memcpy(p->buf,buf,len);
	usb_fifo_aaa.wptr++;
	int fifo_use=(usb_fifo_aaa.wptr-usb_fifo_aaa.rptr)&(USB_FIFO_NUM*2-1);
	if(fifo_use>USB_FIFO_NUM)
	{
		usb_fifo_aaa.rptr++;//overlap older data
	}
	return 1;
}

static inline u32 rf_access_code_24to32 (u32 code)
{
	u32 r = (code & 0x00ffff00)>>8;
	u32 p = code & 0xff;
	u32 t = 0;
	for (int i=0; i<8; i++) {
		t = t << 2;
		t |= ((p & BIT(i)) ? 1 : 2);
		t = t << 2;
		t |= (r & 0x03);
		r = r >>2;
	}
	return t;
}

void set_pair_access_code(u32 code)
{
    write_reg32(0x800408, ((code & 0xffffff00) | 0x71));
    write_reg8(0x80040c, code);
}

void set_data_access_code(u32 code)
{
	write_reg32(0x800408, ((code & 0xffffff00) | 0x77));
	write_reg8(0x80040c, code);
}

static inline u32 rf_get_access_code1 (void)
{
	return read_reg8 (0x800414) | (read_reg32(0x800410) & 0xffffff00);
}

#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)

#if RF_FAST_RX_SETTING_ENABLE
_attribute_ram_code_sec_ void rf_rx_fast_settle_init()
{
	//RX_SETTLE_TIME_44US)
	write_reg8(0x1284,(read_reg8(0x1284)&0x87)|(0x6<<3));

	write_reg8(0x12b0,0x00);
	write_reg8(0x12b1,read_reg8(0x12b1)&0xfe);
	write_reg8(0x12b2,0x10);
	write_reg8(0x12b3,read_reg8(0x12b3)&0xfe);
	write_reg8(0x12b4,0x10);
	write_reg8(0x12b5,read_reg8(0x12b5)&0xfe);
	write_reg8(0x12b6,0x32);
	write_reg8(0x12b7,read_reg8(0x12b7)&0xfe);
	write_reg8(0x12b8,0x58);
	write_reg8(0x12b9,read_reg8(0x12b9)&0xfe);
	write_reg8(0x12ba,0x58 );
	write_reg8(0x12bb,read_reg8(0x12bb)&0xfe);
}

_attribute_ram_code_sec_ void rf_rx_fast_settle_en(void)
{
	write_reg8(0x1229,read_reg8(0x1229)|0x01);
}

_attribute_ram_code_sec_ void rf_rx_fast_settle_dis(void)
{
	write_reg8(0x1229,read_reg8(0x1229)&0xfe);
}

#endif
#if RF_FAST_TX_SETTING_ENABLE
_attribute_ram_code_sec_ void rf_tx_fast_settle_init()
{
	//TX_SETTLE_TIME_53US)
	write_reg8(0x1284,(read_reg8(0x1284)&0xf8)|0x02);

	write_reg8(0x12a4,0x00);
	write_reg8(0x12a5,read_reg8(0x12a5)&0xfe);
	write_reg8(0x12a6,0x10);
	write_reg8(0x12a7,read_reg8(0x12a7)&0xfe);
	write_reg8(0x12a8,0x62);
	write_reg8(0x12a9,read_reg8(0x12a9)&0xfe);
	write_reg8(0x12aa,0x64);
	write_reg8(0x12ab,read_reg8(0x12ab)&0xfe);
	write_reg8(0x12ac,0x6a);
	write_reg8(0x12ad,read_reg8(0x12ad)&0xfe);
	write_reg8(0x12ae,0x6a);
	write_reg8(0x12af,read_reg8(0x12af)&0xfe);
	write_reg8(0x12bc,0x62);
	write_reg8(0x12bd,read_reg8(0x12bd)&0xfe);
}

_attribute_ram_code_sec_ void rf_tx_fast_settle_en(void)
{
	write_reg8(0x1229,read_reg8(0x1229)|0x02);
}

_attribute_ram_code_sec_ void rf_tx_fast_settle_dis(void)
{
	write_reg8(0x1229,read_reg8(0x1229)&0xfd);
}
#endif

_attribute_ram_code_sec_ void check_rf_fast_setting_time()
{
#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)
	if(clock_time_exceed(tick_rf_fast_setting, 2000000))
	{
		tick_rf_fast_setting=clock_time();
		rf_fast_setting_flag=1;
	}
#endif
}

_attribute_ram_code_sec_ void check_rf_fast_setting_flag()
{
#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)
	if(rf_fast_setting_flag==1)//timeout
	{
		rf_fast_setting_flag=0;
	}
	else
	{
		rf_fast_setting_flag=2;
	}
#endif
}

_attribute_ram_code_sec_ void rf_set_tx_rx_setting()
{
#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)
	if(rf_fast_setting_flag<2)
	{
		#if RF_FAST_TX_SETTING_ENABLE
			write_reg8(0x0f04, 0x70);//tx settle time: Default 150us, minimum 113us(0x70+1)
			rf_tx_fast_settle_dis();
		#endif
		
		#if RF_FAST_RX_SETTING_ENABLE
			write_reg8(0x0f0c, 0x54); //rx settle time: Default 150us, minimum 85us(0x54+1)
			rf_rx_fast_settle_dis();
		#endif
	}
	else //if(rf_ca_cnt==1)
	{
		#if RF_FAST_TX_SETTING_ENABLE
			write_reg8(0x0f04, 65);//tx settle time: Default 150us, minimum 113us(0x70+1)
			rf_tx_fast_settle_en();
		#endif

		#if RF_FAST_RX_SETTING_ENABLE
			write_reg8(0x0f0c, 48); //rx settle time: Default 150us, minimum 85us(0x54+1)
			rf_rx_fast_settle_en();
		#endif
	}
#endif
}
#endif

void rf_user_init()
{
	rf_mode_init();
	rf_set_pri_2M_mode();

	rf_rx_buffer_set((u8*)rf_rx_buff,PKT_BUFF_SIZE,0);

	write_reg8(0x405, (read_reg8(0x405)&0xf8) |5); //access_byte_num[2:0]
	write_reg8(0x420, 35);

	rf_set_power_level_index(RF_POWER_P8p99dBm);

	// write_reg32(0x800408,0x39517671);  //for 8366
	// write_reg8(0x80040c,0x95);
	set_pair_access_code(pair_accesscode);

	// write_reg32(0x800410,0xd3f03577);  //for 8366
	// write_reg8(0x800414,0xe7);

	//write_reg32(0x800418,0xc0300c03); //for 8366

	write_reg16(0xf06, 0);//rx wait time
	write_reg8(0x0f0c, 0x54); //rx settle time: Default 150us, minimum 85us(0x54+1)

	write_reg16(0xf0e, 0);//tx wait time
	write_reg8(0x0f04, 0x70);//tx settle time: Default 150us, minimum 113us(0x70+1)

	write_reg8(0xf10, 0);// wait time on NAK

	write_reg8(0x402,0x42);

#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)	
	rf_fast_setting_flag=0;
	tick_rf_fast_setting=clock_time()|1;
	#if RF_FAST_RX_SETTING_ENABLE	//must fist  call rx fast settle init
		rf_rx_fast_settle_init();
		rf_rx_fast_settle_dis();
	#endif
	
	#if RF_FAST_TX_SETTING_ENABLE	
		rf_tx_fast_settle_init();
		rf_tx_fast_settle_dis();
	#endif
#endif

	rf_set_tx_rx_off();
	rf_rx_acc_code_enable(0X01);//
	rf_tx_acc_code_select(0);

	irq_disable();
	rf_irq_clr_src(0xffff);
	irq_enable_type(FLD_IRQ_ZB_RT_EN);
	rf_irq_disable(FLD_RF_IRQ_ALL);
	rf_irq_enable(FLD_RF_IRQ_TX|FLD_RF_IRQ_RX);
}

_attribute_ram_code_sec_ void rf_tx_irq_handle()
{

	rf_set_tx_rx_off();		//add for save

#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)	
	check_rf_fast_setting_flag();
	rf_set_tx_rx_setting();
#endif	
	rf_set_channel_aaa (host_channel, RF_CHN_TABLE);
	rf_set_rxmode ();
}

u8 check_from_ram(pair_data_t *p)
{
	static u16 buf[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	if(memcmp(buf, &p->did, ID_LEN))
	{
		memcpy(buf, &p->did, ID_LEN);
		
		return 0;
	}
	
	return 1;
}

int set_device_id_in_firmware (u8 *p)
{
    rf_packet_t *p_rf_data_pkt = (rf_packet_t *)p;
    pair_data_t *p_pair_data=(pair_data_t *)&p_rf_data_pkt->dat[0];

	u8 dec[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
#if (AES_METHOD == 0)	
	memcpy(&dec[0], &p_pair_data->did, ID_LEN);
#elif (AES_METHOD == 1)
	if(p_pair_data->cmd&BIT(7)) //AES ENABLE
	{
		memcpy(encKey, pub_key, 16);
		aes_decrypt(encKey, (u8 *)&p_pair_data->did, &dec[0]);
		memcpy((u8*)&p_pair_data->did, dec, ID_LEN);
	}
#endif

	u8 device_index=0;//0 mouse 1 keyboard
	
    if((p_pair_data->did&0x03)==2)
    {
		device_index=1;
    }

    if(memcmp((u8*)&key_enc[device_index].did, dec, ID_LEN)==0)
    {
    	pair_success_flag = 1;
		return 1;
    }
	else if(custom_binding_idx < PARING_MAX_NUM)
	{
		memcpy((u8*)&key_enc[device_index].did,dec,ID_LEN);
		u32 adr =0;
		adr=(ID_SAVE_STORAGE_ADDR + ID_LEN * custom_binding_idx);
		
	#if HW_IS_FLASH
		u8 tmp_buf[16];
		flash_write_page(adr,ID_LEN,(u8*)&dec[0]);
		flash_read_page(adr, ID_LEN, tmp_buf);	//Read the stored content back out
		custom_binding_idx++;
	#else
		rf_set_tx_rx_off();
		unsigned char r = irq_disable();
		u32 temp;
		for(u8 i=0;i<4;i++)
		{
			otp_set_active_mode();
			otp_write(adr, 1,(u32*)&dec[0]);
			otp_read(adr, 1, (u32*)&temp);
			if(memcmp(&dec[0], &temp, 4)==0)
			{
				custom_binding_idx++;
				break;
			}
			else
			{
				custom_binding_idx++;
				if(custom_binding_idx >= PARING_MAX_NUM)
				{
					break;
				}
				adr=(ID_SAVE_STORAGE_ADDR + ID_LEN * custom_binding_idx);
			}
		}
		irq_restore(r);
		rf_set_channel_aaa (host_channel, RF_CHN_TABLE);
		rf_set_rxmode ();
	#endif
	
	#if HW_IS_FLASH 
		//When the offset exceeds 3000, or the stored data is incorrect，Store from the start
		if(((ID_LEN*custom_binding_idx)>3000)||(memcmp(tmp_buf, dec, ID_LEN)!=0))
		{
			custom_binding_idx=0;
			
			flash_erase_sector(ID_SAVE_STORAGE_ADDR);
	
			if((key_enc[0].did!=U32_MAX)&&(key_enc[0].did!=0))
			{
				flash_write_page(ID_SAVE_STORAGE_ADDR+custom_binding_idx*ID_LEN,ID_LEN,(u8*)&key_enc[0].did);
	
				custom_binding_idx++;
			}
	
			if((key_enc[1].did!=U32_MAX)&&(key_enc[1].did!=0))
			{
				flash_write_page(ID_SAVE_STORAGE_ADDR+custom_binding_idx*ID_LEN,ID_LEN,(u8*)&key_enc[1].did);
	
				custom_binding_idx++;
			}
		}
	#endif

		return 1;
	}

	return 0;
}


_attribute_ram_code_sec_ u8 rf_pair_response(rf_packet_t *p_rf_rx_data)
{
	static u8 rx_id_same_cnt=0;

	pair_data_t *p_pair_data=(pair_data_t *)&p_rf_rx_data->dat[0];
    pair_ack_data_t *p_pair_ack_data=(pair_ack_data_t*)&ack_rf_dat.dat[0];
    p_pair_ack_data->chn=chn_mask;
#if PAIR_CHECK_RSSI_ENABLE_AAA
	static u32 rx_rssi = 0;
	static u8 rssi_last;
	u8 *p_rf_rx_data_start=(u8 *)&p_rf_rx_data->dma_len;
	u8 rssi_now=GET_RX_RSSI(p_rf_rx_data_start);

	if(!rx_rssi){
		rx_rssi = rssi_now;
	}

	if ( abs_aaa(rssi_last - rssi_now) < 17 ){
		rx_rssi = ( ( rx_rssi << 1) + rssi_now ) / 3;
	}
	rssi_last = rssi_now;
    if(rx_rssi>(46-user_config.rssi_threshold))
#endif
	{
		if (check_from_ram(p_pair_data)) //check ram
		{
			rx_id_same_cnt++;
			
			if (rx_id_same_cnt>=2)
			{
				p_pair_ack_data->gid = dongle_id;
				p_pair_ack_data->cmd = PAIR_ACK_CMD;

				memcpy(&p_pair_ack_data->did, &p_pair_data->did,ID_LEN);

				if(pair_success_flag)
				{
					user_rf_ack_send(27);
				}

				p_post = (void *)set_device_id_in_firmware;

				return 1;
			}
    	} 
		else
		{
		    rx_id_same_cnt = 0;
			pair_success_flag = 0;
        }
    }
		
	return 0;
}


u8 get_next_channel_with_mask(u32 mask,u8 chn)
{
	PIN_DEBUG_RF_CHN_NEXT_TOGGLE;
#if (HOPING_FREQ==0)
    //printf("next_channel=%d.\n",(chn+3)%15);
   	return (chn+3)%15;
#else
	int chn_high = (mask >> 4) & 0x0f;
	if (mask & LL_CHANNEL_SEARCH_FLAG) {
		return LL_NEXT_CHANNEL (chn);
	}
	else if (chn_high != chn) {
		ll_chn_sel = 1;
		return chn_high;
	}
	else {
		ll_chn_sel = 0;
		return mask & 0x0f;
	}
#endif
}
void soft_tick_poll()
{
	static u32 tick;
	static u8 flag=0;
	
#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)	
	check_rf_fast_setting_time();
#endif

	if(clock_time_exceed(tick,1000))
	{
		tick=clock_time();

		rx_fail++;

		if(rx_fail>18)
		{
			if(rf_pair_enable==0)
			{
				rx_fail=14;
				set_data_access_code(dongle_id);
			}
			else
			{
				rx_fail=17;
				if(flag)
				{
					flag=0;
					set_pair_access_code(pair_accesscode);
				}
				else
				{
					flag=1;
					set_data_access_code(dongle_id);
				}
			}
			
			host_channel = get_next_channel_with_mask(chn_mask, host_channel);
			//get_next_channel_with_mask(0, 0);
			rf_set_tx_rx_off();

		#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)
			if(rf_fast_setting_flag==2)
			{
				rf_fast_setting_flag=1;
				rf_set_tx_rx_setting();
				user_rf_ack_send(1);
				return ;
			}
			else
			{
				rf_fast_setting_flag=1;
				rf_set_tx_rx_setting();
			}
		#endif	

			rf_set_channel_aaa (host_channel, RF_CHN_TABLE);
			rf_set_rxmode();
		}
		
	#if (HOPING_FREQ==1)
		chn_mask = update_channel_mask(chn_mask, host_channel, (u8*)&ll_chn_pkt[0]);
	#endif
	}
}

_attribute_ram_code_sec_ void irq_host_timer1 (void)
{

}

void call_mouse_data_handle(u8 *buf)
{
    ms_tick=clock_time();
    mouse_data_t *ms_dat=(mouse_data_t*)&buf[0];

    check_first_normal_data();

    if(ms_dat->x||ms_dat->y)
    {
		x_smoother=ms_dat->x;
		y_smoother=ms_dat->y;
		adaptive_smoother();
		ms_dat->x=x_smoother;
		ms_dat->y=y_smoother;
    }
	if(memcmp(&mouse_last_data.btn,&ms_dat->btn,sizeof(mouse_data_t))||(ms_dat->x!=0)||(ms_dat->y!=0)||(ms_dat->wheel!=0))
	{
		memcpy(&mouse_last_data.btn, &ms_dat->btn,sizeof(mouse_data_t));
		push_usb_fifo_aaa(MOUSE_DATA_TYPE,&ms_dat->btn,sizeof(mouse_data_t));
	}
}

void call_keyboard_data_handle(u8 *buf)
{
	check_first_normal_data();
	kb_data_handle(buf);
}

#if (AES_METHOD == 1)
volatile u8  aes_dec[59];
#endif
_attribute_ram_code_sec_ u8 rf_km_data_repsonse(rf_packet_t *p_rf_rx_data,u8 hw_crc_ok)
{
	u8 ret=0;
	static u8 slave_seq_no[2]={0xff,0xff};
	u8 *p_km_data_temp=(u8 *)&p_rf_rx_data->dat[0];
	km_ack_data_t *p_km_ack_data=(km_ack_data_t*)&ack_rf_dat.dat[0];
	p_km_ack_data->chn=chn_mask;
	
	if(hw_crc_ok)
	{
		ret=1;
	} 
#if DATA_3_CHOOSE_1_ENABLE
	else 
	{
		u16 crc_c=0;
		u16 crc_s[3]={0,0,0};
	    u8 *p_src1;
		u8 *p_src2; 
		u8 *p_src3;
		u8 crc_position=sizeof(km_3_c_1_data_t)-2;
		p_src1=&p_km_data_temp[0];
		p_src2=p_src1+sizeof(km_3_c_1_data_t);
		p_src3=p_src2+sizeof(km_3_c_1_data_t);
		crc_s[0]=p_src1[crc_position]+(p_src1[crc_position+1]<<8);
		crc_s[1]=p_src2[crc_position]+(p_src2[crc_position+1]<<8);
		crc_s[2]=p_src3[crc_position]+(p_src3[crc_position+1]<<8);
		for(u8 i=0;i<3;i++)
        {
			crc_c=crc16_user(p_km_data_temp, sizeof(km_3_c_1_data_t)-2);
			if((crc_c==crc_s[0])||(crc_c==crc_s[1])||(crc_c==crc_s[2]))
			{
				ret=1;
				break;
			}
			else
			{
				p_km_data_temp+=sizeof(km_3_c_1_data_t);
			}
        }
	}
#endif

    if(ret==1)
	{
		km_3_c_1_data_t *km_ptr=(km_3_c_1_data_t*)&p_km_data_temp[0];
		p_km_ack_data->seq_no=km_ptr->seq_no;
		p_km_ack_data->host_led_status=(host_keyboard_status&0x1f)|((usb_device_status&0x7)<<5);
		u8 device_index=0;//0 mouse 1 keyboard
		
	    if((km_ptr->cmd & 0x0f) == KB_CMD)
		{
		    device_index = 1;
		}

	#if (AES_METHOD == 1)
		if (((km_ptr->cmd >>   7) & 1) == 1) {
			//packet have aes
			//aes decrypt
			//copy cmd pn_no to aes_dec
			memcpy(aes_dec, (u8 *)&km_ptr->cmd, 2);
			//set private_key
			memcpy(encKey, &key_enc[device_index].did, ID_LEN);
			//decryption data
			aes_decrypt(encKey, (u8 *)&km_ptr->pn_no, &aes_dec[2]);
			km_ptr = (km_3_c_1_data_t*)&aes_dec[0];
		}
	#endif
	
	#if CHECK_DEVICE_ID_ENABLE
	    if(key_enc[device_index].did!=km_ptr->did)
	    {
			return 0;
	    }
	#endif
	
		if((km_ptr->cmd & 0x0f) == MOUSE_CMD)
		{
			p_km_ack_data->cmd=MOUSE_ACK_CMD;
			
			user_rf_ack_send(1);
		}
		else if((km_ptr->cmd & 0x0f) == KB_CMD)
		{
			p_km_ack_data->cmd=KB_ACK_CMD;
			
			user_rf_ack_send(7);
		}
		else
		{
			return 0;
		}

		if(km_ptr->seq_no==slave_seq_no[device_index])
		{
			return 1;
		}

		slave_seq_no[device_index]=km_ptr->seq_no;
		
		if((km_ptr->cmd & 0x0f) == MOUSE_CMD)
		{
			if(p_mouse_data_handle_post==NULL)
			{
				memcpy((u8 *)to_usb_mouse_data, &km_ptr->km_dat[0], 6);
				p_mouse_data_handle_post=(void *)call_mouse_data_handle;
			}
		
			return 1;
		}
		else if((km_ptr->cmd & 0x0f) == KB_CMD)
		{	
			if(p_keyboard_data_handle_post==NULL)
			{
				memcpy((u8 *)to_usb_keyboard_data, &km_ptr->km_dat[0], 6);
				p_keyboard_data_handle_post=(void *)call_keyboard_data_handle;
			}
		
			return 1;
		}
	}
	
	return 0;
}


_attribute_ram_code_sec_ void rf_rx_irq_handle()
{
	u8 need_ack=0;
	u8 hw_crc_ok=0;

	raw_pkt = (u8 *) (rf_rx_buff + rf_rx_wptr * PKT_BUFF_SIZE);
	rf_rx_wptr = (rf_rx_wptr + 1) & 1;
	rf_rx_buffer_set((u8*)(rf_rx_buff + rf_rx_wptr * PKT_BUFF_SIZE),PKT_BUFF_SIZE,0);
	rf_packet_t *p_rf_rx_data=(rf_packet_t*)raw_pkt;

    if(RF_TPLL_PACKET_CRC_OK(raw_pkt)&&RF_TPLL_PACKET_LENGTH_OK(raw_pkt))
    {
		PIN_DEBUG_RF_RX_CRC_OK_TOGGLE;
		hw_crc_ok=1;
    }

    if(rf_pair_enable)
    {
		if(hw_crc_ok)
		{
			pair_data_t *pair_data_ptr=(pair_data_t*)&p_rf_rx_data->dat[0];

		#if (AES_METHOD == 1)
			if (pair_data_ptr->cmd == (PAIR_CMD|BIT(7))) 
		#else
			if (pair_data_ptr->cmd == PAIR_CMD) 
		#endif
			{
				need_ack = rf_pair_response(p_rf_rx_data);
            }
			else 
			{
				need_ack = rf_km_data_repsonse(p_rf_rx_data, hw_crc_ok);
			}
		}
    } 
	else 
	{
		need_ack = rf_km_data_repsonse(p_rf_rx_data, hw_crc_ok);
	}


	if(need_ack)
	{
		PIN_DEBUG_RF_RX_DATA_OK_TOGGLE;
	#if (HOPING_FREQ==1)
		device_packet_received=1;
		ll_chn_pkt[ll_chn_sel]++;
		tick_fh = clock_time ();
		host_channel = get_next_channel_with_mask (chn_mask, host_channel);
	#endif
		rx_fail=0;
	}
    raw_pkt[0] = 1;
    //device_packet_received++;
}

static unsigned short tsync_crc16(unsigned short crc, unsigned char *pD, int len)
{
	#define	poly			0x8408
	int i,j;
    for(j=len; j>0; j--)
    {
        unsigned int ds = *pD++;
        for(i=0; i<8; i++)
        {
        	int bit = (crc ^ ds ) & 1;
        	crc = crc >> 1;
        	if (bit)
        	{
				crc = crc ^ poly;
			}
			ds = ds >> 1;
        }
    }
	return crc;
}

static inline unsigned int tsync_access_code_16to32 (unsigned short code)
{
	unsigned int r = 0;
	for (int i=0; i<16; i+=2) {
		r = r << 4;
		int c = (code >> i) & 3;
		r |= (c<<2) | ((~c) & 3);
	}
	return r;
}


void custom_init()
{
#if HW_IS_FLASH
	flash_read_page(CFG_MAC_ADDR,sizeof(custom_cfg_t),(u8*)&user_config.mac_addr);
	if (user_config.mac_addr == U32_MAX)
    {
        generateRandomNum(4, (u8 *) &user_config.mac_addr);
        flash_write_page(CFG_MAC_ADDR, 4, (u8 *)&user_config.mac_addr);
		user_config.access_code_type = 0;
    }
#else
	otp_read(CFG_MAC_ADDR, sizeof(custom_cfg_t)/4, (u32*)&user_config.mac_addr);
#endif
	printf("---user_cfg.dev_mac=0x%04x.\r\n",user_config.mac_addr);

	if(user_config.access_code_type!=U8_MAX)
	{
		dongle_id = user_config.mac_addr;
	}
	else
	{
		int c = tsync_crc16 (0xffff, (u8*)&user_config.mac_addr, 4);
		dongle_id = tsync_access_code_16to32 (c);
	}
	printf("---dongle_id=0x%04x.\n",dongle_id);
	
    if(user_config.rf_id!=0xffff)
    {
		pair_accesscode=rf_access_code_16to32(user_config.rf_id);
    }
	
	if(user_config.rssi_threshold==U8_MAX)
	{
		user_config.rssi_threshold=0;
	}

	if((user_config.paring_limit_t==0)||(user_config.paring_limit_t==U8_MAX))
	{
		user_config.paring_limit_t=60;
	}
	
#if EMI_TEST_FUN_ENABLE_AAA
	if((user_config.tx_power_emi==0)||(user_config.tx_power_emi==U8_MAX))
	{
		user_config.tx_power_emi=DEFAULT_EMI_TX_POWER;
	}
#endif

#if (AES_METHOD==1)
	for (u8 i = 0; i < 16; i++)
	{
   	    if (user_config.encKey[i]!= 0xff)
    	{
    		memcpy(pub_key, &user_config.encKey[0], 16);
			
    		break;
    	}
	}
#endif

#if USB_DESCRIPTOR_MY_SELF
	custom_set_usb_cfg_params(&user_config);
#endif

    AES_KEY tmp_key;
	u8 index = 0;

    for (u8 i = 0; i < PARING_MAX_NUM; i++)
    {
	#if HW_IS_FLASH
	    flash_read_page(ID_SAVE_STORAGE_ADDR + ID_LEN*i, ID_LEN, (u8*)&tmp_key.did);
	#else
	    otp_read(ID_SAVE_STORAGE_ADDR+ID_LEN*i, ID_LEN/4, (u32*)&tmp_key.did);
	#endif
        if (tmp_key.did == U32_MAX)
        {
			break;
        }
		else
		{
			index = (tmp_key.did  & 0x03) - 1;
			memcpy(&key_enc[index].did, &tmp_key.did, ID_LEN);
			custom_binding_idx++;
		}
    }
	
#if HW_IS_FLASH //When the offset exceeds 3000, Store from the start
	if((ID_LEN*custom_binding_idx)>3000)
	{
		custom_binding_idx=0;
		
		flash_erase_sector(ID_SAVE_STORAGE_ADDR);

		if((key_enc[0].did!=U32_MAX)&&(key_enc[0].did!=0))
		{
			flash_write_page(ID_SAVE_STORAGE_ADDR+custom_binding_idx*ID_LEN,ID_LEN,(u8*)&key_enc[0].did);

			custom_binding_idx++;
		}

		if((key_enc[1].did!=U32_MAX)&&(key_enc[1].did!=0))
		{
			flash_write_page(ID_SAVE_STORAGE_ADDR+custom_binding_idx*ID_LEN,ID_LEN,(u8*)&key_enc[1].did);

			custom_binding_idx++;
		}
	}
#endif

	printf("---custom_binding_idx=%d.\n",custom_binding_idx);
    printf("---mouse_id=0x%04x.\n",key_enc[0].did);
    printf("---kb_id=0x%04x.\n",key_enc[1].did);
}
void usb_custom_init()
{
	//enable USB manual interrupt(in auto interrupt mode,USB device would be USB printer device)
	usb_init_interrupt();
	
#if USB_DESCRIPTOR_MY_SELF
	#if OTA_ENABLE_AAA
		usb_register_set_report(app_hid_set_report_handle);
	
		write_reg8(0x10e,(1<<USB_EDP_MOUSE)|(1<<USB_EDP_KEYBOARD_IN)|(1<<USB_EDP_SPP_IN));

		notify_rsp_buf_init();
	#else
		write_reg8(0x10e,(1<<USB_EDP_MOUSE)|(1<<USB_EDP_KEYBOARD_IN));
	#endif
#endif


	//deepsleep_dp_dm_gpio_low_wake_enable(); // TODO: check B80 have not this interface

	//enable USB DP pull up 1.5k
	usb_set_pin_en();
	
}
void timer1_init_aaa()
{
#if (MY_CHANNEL_ENABLE==0)
	timer1_set_mode(TIMER_MODE_SYSCLK,0,8* CLOCK_SYS_CLOCK_1MS);
	timer_start(TIMER1);
#endif
}

#define FW_OFFSET_IN_FLASH      0x0000 

void user_init()
{

	rf_user_init();

   	usb_custom_init();
	pm_set_suspend_power_cfg(PM_POWER_USB, 1);

   	irq_enable();

	power_on_tick=clock_time()|1;
}

const u16 consumer_list[]={
    0x221,		//0xa3 C_WWW_SEARCH		
    0x223,		//0xa4 C_WWW_HOME
    0x224,		//0xa5 C_WWW_BACKWARD
    0x225,		//0xa6 C_WWW_FORWARD
    0x226,		//0xa7 C_WWW_STOP
    0x227,		//0xa8 C_WWW_REFRESH
    0x22A,		//0xa9 C_MY_FAVORITE
    0x183,		//0xaa C_MEDIA_SELECT
    	
    0x18A,		//0xab C_EMAIL
    0x192,		//0xac C_CALCULATOR
    0x194,		//0xad C_MY_COMPUTER
    0xB5,		//0xae C_NEXT_TRACK
    0xB6,		//0xaf C_PRE_TRACK
    0xB7,		//0xb0 C_STOP
    0xCD,		//0xb1 C_PLAY_PAUSE
    0xE2,		//0xb2 C_MUTE	
    	
    0xE9,		//0xb3 C_VOL_INC
    0xEA,		//0xb4 C_VOL_DEC	
    0x00,		//0xb5 telinkԶ
    0x22D,		//0xb6 USAGE ZOOM IN
    0x22E,		//0xb7	 USAGE ZOOM OUT 
    0x236,		//0xb8	 USAGE PAN LEFT
    0x237,		//0xb9	 USAGE PAN RIGHT
    0x30B,		//0xba	C_BRIGHT_INC	
    	
    0x30A,		//0xbb	C_BRIGHT_DEC
    0xB8,		//0Xbc	 c_rject
    0x30,		//0Xbd	C_POWER 		
    0x19E,		//0Xbe	C_TERMINAL_LOCK 
};


volatile u8 has_consume_key_press=0;
volatile u8 has_normal_key_press=0;
volatile u8 has_system_key_press=0;

void special_key_wake_up_pc()
{
#if 1 
	/* for example  fn conusmer key  wake up window pc
	The disadvantage is that an extra L is sent_ ctrl
	the user decides whether to need this function*/
	if(usb_device_status==USB_DEVICE_CHECK_PC_SLEEP)
    {
		if(need_enter_suspend_flag)
		{
		  	u8 x[8]={0,0,0,0,0,0,0,0};
		#if 0
			x[0]=1;//l_ctrl
		  	push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, x, 8);
		  	x[0]=0;
		  	push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, x, 8);
		#else
			x[1]=0;
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, x, 6);
		#endif
		}
    }
#endif
}

u32 need_enter_suspend_tick;
u8 kb_data_handle(u8 *buf)
{

	u16 consume_key=0;
	u8 system_key=0;
	u8 normal_key[8]={0,0,0,0,0,0,0,0};
	u8 nk_offset=2;
	u8 len=0;

	for(u8 i=0;i<6;i++)
	{
	if(buf[i]!=0)
		len++;
	else
		break;
	}

	if(len==0)//release
	{
		has_normal_key_press=0;
		has_consume_key_press=0;
		has_system_key_press=0;
		if(need_enter_suspend_tick==0)
		{
		special_key_wake_up_pc();//for fn
		}
	}
	else
	{
		for(u8 i=0;i<len;i++)
		{
			if((buf[i]>=0xa0)&&(buf[i]<=0xa2))
			{
				system_key=1<<(buf[i]-0xa0);
				has_system_key_press=1;
			}
			else if((buf[i]>=0xa3)&&(buf[i]<=0xbe))
			{
				consume_key=consumer_list[buf[i]-0xa3];
				has_consume_key_press=1;
			}
			else if((buf[i]>=0xe0)&&(buf[i]<=0xe7))
			{
				normal_key[0] |= (1 << (buf[i]-0xe0));
				has_normal_key_press=1;
			}
			else
			{
				normal_key[nk_offset++]=buf[i];
				has_normal_key_press=1;
			}
		}
	}

	kb_tick=clock_time();	
	if(memcmp(normal_last_data, normal_key,8))
	{
		memcpy(normal_last_data, normal_key,8);
		push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, normal_key, 8);
	}
	if(memcmp(consume_last_data, (u8*)&consume_key,2))
	{
		memcpy(consume_last_data, (u8*)&consume_key,2);
		special_key_wake_up_pc();//for fn
		push_usb_fifo_aaa(CONSUME_DATA_TYPE, (u8*)&consume_key, 2);
	}
	if(sys_last_data!=system_key)
	{
		sys_last_data=system_key;
		special_key_wake_up_pc();//for fn
		push_usb_fifo_aaa(SYSTEM_DATA_TYPE, &system_key, 1);
	}

	return 1;
}


void pull_usb_data()
{
	int success=1;

	if(usb_fifo_aaa.wptr!=usb_fifo_aaa.rptr)
	{
		USB_DATA_S *p=(USB_DATA_S*)usb_fifo_aaa.fifo[usb_fifo_aaa.rptr&(USB_FIFO_NUM-1)];
        //mouse_data_t *now_ms=(mouse_data_t *)&p->buf[0];
		if(p->type==MOUSE_DATA_TYPE)
		{
			//if(memcmp_aaa(&mouse_last_data.btn, p->buf,sizeof(mouse_data_t))||(now_ms->x!=0)||(now_ms->y!=0)||(now_ms->wheel!=0))
			{
				//memcpy(&mouse_last_data.btn, p->buf,sizeof(mouse_data_t));
				success=usb_mouse_hid_report_aaa(1,p->buf, sizeof(mouse_data_t));
			}
			
     		if((mouse_last_data.btn==0)&&(mouse_last_data.wheel==0))
			{
				mouse_not_release=0;
			}
			else
			{
				mouse_not_release=1;
			}
		}
		else if(p->type==NORMAL_KB_DATA_TYPE)
		{
			//if(memcmp_aaa(normal_last_data, p->buf,8))
			{
				//memcpy(normal_last_data, p->buf,8);
				success=usb_keyboard_hid_report_aaa(p->buf);
			}
		}
		else if(p->type==CONSUME_DATA_TYPE)
		{
			//if(memcmp_aaa(consume_last_data, p->buf,2))
			{
				//memcpy(consume_last_data, p->buf,2);
				success=usb_mouse_hid_report_aaa(2,p->buf, 2);
			}
		}
		else if(p->type==SYSTEM_DATA_TYPE)
		{
			//if(sys_last_data!=p->buf[0])
			{
				//sys_last_data=p->buf[0];
				success=usb_mouse_hid_report_aaa(3,p->buf, 1);
			}
		}
		if(success)
		{
			usb_fifo_aaa.rptr++;
		}
	}
    
    {
		u8 release_data[8] = {0,0,0,0,0,0,0,0};
		if(mouse_not_release&&clock_time_exceed(ms_tick,2000000))
		{
			if(usb_mouse_hid_report_aaa(1,release_data, 6))
			{
				mouse_not_release=0;
			}
		}
		else if(has_consume_key_press&&clock_time_exceed(kb_tick,2000000))
		{
			if(usb_mouse_hid_report_aaa(2,release_data, 2))
			{
				has_consume_key_press=0;
			}
		}
		else if(has_system_key_press&&clock_time_exceed(kb_tick,2000000))
		{
			if(usb_mouse_hid_report_aaa(3,release_data, 1))
			{
				has_system_key_press=0;
			}
		}
		
		if(has_normal_key_press&&clock_time_exceed(kb_tick,2000000))
		{
			if(usb_keyboard_hid_report_aaa(release_data))
			{
				has_normal_key_press=0;
			}
		}
	}
}


static inline void check_kb_led_status()
{
#if USER_DEBUG
	static u8 last_led_status=0xff;
	if(last_led_status!=host_keyboard_status)
	{
		last_led_status=host_keyboard_status;
		gpio_write(D1_LED_PIN, last_led_status&0x01);
		gpio_write(D2_LED_PIN, last_led_status&0x02);
		gpio_write(D3_LED_PIN, last_led_status&0x04);
	}
#endif	
}

void mcu_enter_suspend()
{
	if(clock_time_exceed (tick_suspend_interval, 40000))
	{
		u8 r=irq_disable();
		
		cpu_sleep_wakeup (SUSPEND_MODE, PM_WAKEUP_TIMER|PM_WAKEUP_CORE_USB, clock_time()+200*CLOCK_16M_SYS_TIMER_CLK_1MS);
		
		rf_user_init();

		timer1_init_aaa();

		irq_restore(r);
		tick_suspend_interval = clock_time()|1;
		irq_enable();	   
	}
}

static inline void check_pair_timeout()
{
	if(power_on_tick&&clock_time_exceed(power_on_tick, user_config.paring_limit_t*1000000))
	{
		power_on_tick=0;
		clear_pair_enable_flag();
	}
}

void usb_host_status_check(void)
{
	static u8 last_status=0xff;
	
	if(reg_usb_host_conn & BIT(7)) 
	{
	#if 0
		if((reg_usb_mdev & BIT(2)) && (reg_irq_src & FLD_IRQ_USB_PWDN_EN)) 
		{
			usb_device_status = USB_DEVICE_CHECK_PC_SLEEP;
		} 
		else 
		{
			usb_device_status = USB_DEVICE_CONNECT_PC;
		}
	#else
		if((reg_irq_src & FLD_IRQ_USB_PWDN_EN)) 
		{
			if(reg_usb_mdev & BIT(2))
			{
				usb_device_status = USB_DEVICE_CHECK_PC_SLEEP;
			}
			else
			{
				usb_device_status = USB_DEVICE_UNPLUG;
			}
		} 
		else 
		{
			
			usb_device_status = USB_DEVICE_CONNECT_PC;
		}
	#endif
	} 
	else 
	{
		usb_device_status = USB_DEVICE_DISCONECT_PC;
	}
	
	if(last_status!=usb_device_status)
	{
		printf("---usb_device_status=%d.\r\n",usb_device_status);
		
        if((usb_device_status==USB_DEVICE_CONNECT_PC))// fist press key then restart pc
		{
			//led_status_out(host_keyboard_status);
			allow_send_to_usb_flag=1;
			if(last_status==USB_DEVICE_DISCONECT_PC)
			{
				//usb_io_printf("pc_power_on");
				if(has_normal_key_press)
				{
					push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, normal_last_data, 8);
				}
			}
			need_enter_suspend_flag=0;
		}
		else if(usb_device_status==USB_DEVICE_CHECK_PC_SLEEP)
		{
			need_enter_suspend_flag=1;
			need_enter_suspend_tick = clock_time()|1;
			//led_status_out(0);
		}
		else if(usb_device_status==USB_DEVICE_DISCONECT_PC)
		{
			need_enter_suspend_flag=1;
			need_enter_suspend_tick = clock_time()|1;
			//led_status_out(0);
		}
		last_status=usb_device_status;
	}
	
	if((usb_device_status==USB_DEVICE_CHECK_PC_SLEEP)&&(need_enter_suspend_flag))
	{
		mcu_enter_suspend();
	}
}

void main_loop (void)
{

	usb_handle_irq();//must first 

#if OTA_ENABLE_AAA
	usb_update_loop();
#endif

	usb_host_status_check();

#if USER_DEBUG
	test_button();
#endif

#if EMI_TEST_FUN_ENABLE_AAA
	usb_host_cmd_proc();

	if(emi_flg) 
	{
		return;
	}
#endif

#if KM_DATA_HANDLE_CALL_BACK_ENABLE
	if(p_keyboard_data_handle_post)
	{
		(*p_keyboard_data_handle_post)((u8 *)to_usb_keyboard_data);	//Run the data handler function
		p_keyboard_data_handle_post=NULL;
	}

	if(p_mouse_data_handle_post)
	{
		(*p_mouse_data_handle_post)((u8 *)to_usb_mouse_data);	//Run the data handler function
		p_mouse_data_handle_post=NULL;
	}
#endif

	if (p_post)
    {
        (*p_post)(raw_pkt);
        p_post = NULL;
    }

	pull_usb_data();
	check_kb_led_status();
    check_pair_timeout();

	soft_tick_poll();
	if(need_enter_suspend_tick&&clock_time_exceed(need_enter_suspend_tick, 3*1000*1000))
		{
			need_enter_suspend_tick =0;
		}
		
}
  int sm_sum_x, sm_pre_x, sm_sum_y, sm_pre_y;

void iir_smoother()
{
    sm_sum_x = sm_sum_x - sm_pre_x + x_smoother;
    sm_pre_x = sm_sum_x / 2;
    x_smoother = sm_pre_x;

    sm_sum_y = sm_sum_y - sm_pre_y + y_smoother;
    sm_pre_y = sm_sum_y / 2;
    y_smoother = sm_pre_y;
}

static inline void iir_smoother_clear(void)
{
    sm_sum_x = 0;
    sm_pre_x = 0;

    sm_sum_y = 0;
    sm_pre_y = 0;
}
#define sm_dyn_pth1  6
#define  sm_dyn_pth2 4
u8 adaptive_smoother()
{
     static u8 asm_flg = 0;
     static u32 sm_last_smoother_tick = 0;

    //auto clear asm sum when no data for a long time
    if (asm_flg && clock_time_exceed(sm_last_smoother_tick, 100000))
    {
        asm_flg = 0;
        iir_smoother_clear();
    }
    if (!asm_flg)
    {
        if ((abs(x_smoother) > sm_dyn_pth1) || (abs(y_smoother) > sm_dyn_pth1))
        {
            asm_flg = 1;
            iir_smoother();
        }
        else
        {
            asm_flg = 0;
            iir_smoother_clear();
        }
    }
    else
    {
        if ((abs(x_smoother) < sm_dyn_pth2) && (abs(y_smoother) < sm_dyn_pth2))
        {
            asm_flg = 0;
            iir_smoother_clear();
        }
        else
        {
            asm_flg = 1;
            iir_smoother();
        }
    }

    if (asm_flg)
    {
        sm_last_smoother_tick = clock_time();
    }

    return asm_flg;
}

