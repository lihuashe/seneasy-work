#include "PanSeries.h"
#include "pan_gpio.h"
#include "pan216_ext.h"
#include "bsp.h"
#include "oled.h"
#include "rf_frame.h"

void data_1mbps_to_500kbps(const uint8_t *data, uint8_t data_len, uint8_t *playload, uint8_t playload_len);
void decode_data_1mbps_to_500kbps(const uint16_t *encoded, uint8_t encoded_len, uint8_t *decoded, uint8_t decoded_len);
void a7535_format(RfSendType t, CtrlPayload_A7535 *controlData);
void xn297l_format(RfSendType t, CtrlPayload_XN297L *controlData);

static int count = 0;
u8 TxData[23] = {0xAA, 0xAA, 0xAA, 0xBB, 0xCC, 0xDD, 0x0d, 0x6c, 0x4a, 0x79, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x04, 0xff, 0xff, 0x04, 0xff, 0x3a}; 
u8 RxBuf[46] = {};
u8 a7535_TxBuf[] = {0x42, 0x15, 0xc9, 0xeb, 0x05, 0x00, 0x00, 0xc0, 0x0e, 0xff, 0xe2, 0x09, 0xa1, 0x08, 0x11, 0x00, 0x00, 0x03, 0x02, 0xe0, 0xe3, 0x81, 0xfd, 0x99, 0xe1, 0x6a};
u8 a7535_decodeBuf[23] = {};
u8 xn297l_TxBuf[30] = {0x18, 0xAA, 0xAA, 0xAA, 0xBB, 0xCC, 0xDD, 0x7E, 0x00, 0x00, 0x00, 0x04, 0x00, 0x03, 0x01, 0x06, 0x00, 0x1E, 0xFF, 0xFF, 0xFF, 0x1B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB};

u8 channel_table[] = {35, 45, 55};
u8 channel_index = 0;
u8 channel_count = 0;
u8 now_channel = 0;
u8 is_pair = 0;
RFConfig_t *now_rfconfig = NULL;
	
#if (!EASY_RF)
const RFConfig_t a7535_gRfConfig = {
    .Channel = 78,      // 0 ~ 83 -> 2400MHz ~ 2483MHz
    .TxPower = PAN216_TXPWR_0dBm,       // 0 ~ 9
    .DataRate = PAN216_DR_1Mbps,        // 1Mbps, 2Mbps, 250Kbps
    .ChipMode = PAN216_CHIPMODE_FS01,   //
    .EnWhite = false,                   // false, true
    .Crc = PAN216_CRC_off,              // disable, 1byte, 2byte, 3byte
    .TxLen = 46,                        // 1 ~ 128
    .RxLen = 46,                        // 1 ~ 128
    .TxAddrWidth = PAN216_WIDTH_4BYTES, // 2 ~ 5
    .TxAddr = {0xAA, 0xBB, 0xCC, 0xDD},
    .RxAddrWidth = PAN216_WIDTH_4BYTES, // 2 ~ 5
    .RxAddr =
        {
            {true, {0xF3, 0xF3, 0xF0, 0xF0}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
        },
    .Endian = PAN216_ENDIAN_BIG, // big, little
    .crcSkipAddr = false,        // false, true
    .EnRxPlLenLimit = false,     // false, true

    // Do not edit the following fields
    .WorkMode = PAN216_WORKMODE_NORMAL, // Normal
    .EnDPL = false,
    .EnTxNoAck = true,
    .EnManuPid = false,
    .TRxDelayTimeUs = 0,
    .RxTimeoutUs = 30000,
    .AutoDelayUs = 0,
    .AutoMaxCnt = 0,
    .TxMode = PAN216_TX_MODE_SINGLE,
    .RxMode = PAN216_RX_MODE_CONTINOUS,
};

const RFConfig_t xn297l_gRfConfig = {
    .Channel = 35,                      // 0 ~ 83 -> 2400MHz ~ 2483MHz
    .TxPower = PAN216_TXPWR_0dBm,       // 0 ~ 9
    .DataRate = PAN216_DR_1Mbps,        // 1Mbps, 2Mbps, 250Kbps
    .ChipMode = PAN216_CHIPMODE_XN297,  //
    .EnWhite = true,                    // false, true
    .Crc = PAN216_CRC_2byte,            // disable, 1byte, 2byte, 3byte
    .TxLen = 30,                        // 1 ~ 128
    .RxLen = 30,                        // 1 ~ 128
    .TxAddrWidth = PAN216_WIDTH_5BYTES, // 2 ~ 5
    .TxAddr = {0xC4, 0xCC, 0xCC, 0xCC, 0xC1},
    .RxAddrWidth = PAN216_WIDTH_5BYTES, // 2 ~ 5
    .RxAddr =
        {
            {true, {0xC4, 0xCC, 0xCC, 0xCC, 0xC1}},
            {true, {0xA1, 0xCC, 0xCC, 0xCC, 0xC1}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
        },
    .Endian = PAN216_ENDIAN_BIG, // big, little
    .crcSkipAddr = false,        // false, true
    .EnRxPlLenLimit = false,     // false, true

    // Do not edit the following fields
    .WorkMode = PAN216_WORKMODE_ENHANCE, // Normal
    .EnDPL = false,
    .EnTxNoAck = true,
    .EnManuPid = false,
    .TRxDelayTimeUs = 0,
    .RxTimeoutUs = 0,
    .AutoDelayUs = 0,
    .AutoMaxCnt = 0,
    .TxMode = PAN216_TX_MODE_SINGLE,
    .RxMode = PAN216_RX_MODE_CONTINOUS,
};

#endif

				
	
int main(void)
{
    BSP_ClockInit();
    BSP_UartInit();
    BSP_GpioInit();
    BSP_KeyInit();
		BSP_OLEDInit();
		OLED_Init();
    SYS_TEST("CPU @ %dHz\n", SystemCoreClock);

#if (INTERFACE_MODE == USE_SPI_3LINE)
    SYS_TEST("SPI-3LINE\n");
#elif (INTERFACE_MODE == USE_I2C)
    SYS_TEST("IIC\n");
#endif
#if INTERFACE_MODE == USE_SPI_4LINE
    SYS_TEST("SPI-4LINE\n");
#endif

    RF_BusInit();
    PAN216_Err_t ret = PAN216_Init();
    if (ret != PAN216_OK)
        goto err_init;
#if (!EASY_RF)
    ret = PAN216_SetUpConfig((RFConfig_t *)&a7535_gRfConfig);
		now_rfconfig = (RFConfig_t *)&a7535_gRfConfig;
    if (ret != PAN216_OK)
        goto err_setup;
#endif
		SYS_TEST("rf_init_ok\n");
    PAN216_EnableInterfaceMuxIRQ(true);
    PAN216_ConfigIT(RF_IT_RX_IRQ | RF_IT_CRC_ERR_IRQ);
    PAN216_ClearIRQFlags(0xFF);
    PAN216_PrepareTRxWithAck(PAN216_RX, false);
    PAN216_RxStart();
		OLED_Clear();
		OLED_ShowString(0, 16, "A7535", 12);
		OLED_Refresh_Gram();
		BSP_DelayMs(500);

    while (1)
    {
				u8 flag = 0;
			
				while (!IRQ_DETECTED && KEY1_UP && KEY2_UP && KEY3_UP && KEY4_UP)
				{
					if (KEY1_UP && KEY2_UP && KEY3_UP && KEY4_UP)
					{
						BSP_DelayMs(5);
					}
					else
					{
						BSP_DelayMs(1);
					}
						
				}
					
				
				if (KEY1_DOWN)
        {
						OLED_Clear();
						OLED_ShowString(0, 16, "A7535", 12);
						OLED_Refresh_Gram();
					  BSP_DelayMs(500);
						
						now_rfconfig = (RFConfig_t *)&a7535_gRfConfig;
						now_channel = 78;
						ret = PAN216_SetUpConfig((RFConfig_t *)now_rfconfig);
						if (ret != PAN216_OK)
								goto err_setup;
						
						PAN216_EnableInterfaceMuxIRQ(true);
						PAN216_ConfigIT(RF_IT_RX_IRQ | RF_IT_CRC_ERR_IRQ);
						PAN216_ClearIRQFlags(0xFF);
						PAN216_PrepareTRxWithAck(PAN216_RX, false);
						PAN216_RxStart();
						BSP_DelayMs(50);

        }
        else if (KEY2_DOWN)
        {
						OLED_Clear();
						OLED_ShowString(0, 16, "XN297L 35", 12);
						OLED_Refresh_Gram();
					  BSP_DelayMs(500);
					
						now_rfconfig = (RFConfig_t *)&xn297l_gRfConfig;
						now_channel = 35;
						ret = PAN216_SetUpConfig((RFConfig_t *)now_rfconfig);
						if (ret != PAN216_OK)
								goto err_setup;		
						
						PAN216_EnableInterfaceMuxIRQ(true);
						PAN216_ConfigIT(RF_IT_RX_IRQ | RF_IT_CRC_ERR_IRQ);
						PAN216_ClearIRQFlags(0xFF);
						PAN216_PrepareTRxWithAck(PAN216_RX, false);
						PAN216_RxStart();
						BSP_DelayMs(50);
        }	
				else if (KEY3_DOWN)
        {
						OLED_Clear();
						OLED_ShowString(0, 16, "XN297L 45", 12);
						OLED_Refresh_Gram();
					  BSP_DelayMs(500);
					
						PAN216_SetChannel((now_channel = channel_table[1]));
						BSP_DelayMs(10);
        }
				else if (KEY4_DOWN)
        {
						OLED_Clear();
						OLED_ShowString(0, 16, "XN297L 55", 12);
						OLED_Refresh_Gram();
					  BSP_DelayMs(500);
					
						PAN216_SetChannel((now_channel = channel_table[2]));
						BSP_DelayMs(10);
        }

				flag = PAN216_GetIRQFlags();
				
				while (KEY1_UP && KEY2_UP && (flag & RF_IT_RX_IRQ))
				{		
						if (flag & RF_IT_RX_IRQ)
						{
								u8 RecvLen = PAN216_GetRecvLen();
								RfSendType temp;
							
								if (PAN216_GetRxPipeNum())
									is_pair = 1;
								else
									is_pair = 0;
								
								PAN216_ReadFifo(TRX_FIFO, RxBuf, RecvLen);
								int rssi = PAN216_GetRssiDBm();
								PAN216_ClearIRQFlags(RF_IT_RX_IRQ);
								printf("+RecvLen = %d, cnt = %d, now_channel = %d\n", RecvLen, ++count, now_channel);

								if (now_rfconfig == &a7535_gRfConfig)
								{
									decode_data_1mbps_to_500kbps((uint16_t *)RxBuf, sizeof(a7535_decodeBuf), a7535_decodeBuf, sizeof(a7535_decodeBuf));
									
									CtrlPayload_A7535 a7535_frame;
									memcpy(&a7535_frame.unique_id, &a7535_decodeBuf, sizeof(a7535_frame));
									a7535_format(temp, &a7535_frame);
									printf("\r\n");
									printHex(a7535_decodeBuf, sizeof(a7535_decodeBuf)-6);
								}
								else
								{
									CtrlPayload_XN297L xn297l_frame;
									memcpy(&xn297l_frame, &RxBuf[1], sizeof(xn297l_frame));
									xn297l_format(temp, &xn297l_frame);
									printHex(RxBuf, RecvLen);									
								}
							  
								printf("RSSI=%ddBm\r\n", rssi);
								BSP_Led3Toggle();
								break;
						}
				}
					
    }

err_init:
    printf("PAN216_Init failed.\r\n");
		LED2_PIN = 1;
    while (1)
        ;
#if (!EASY_RF)
err_setup:
    printf("PAN216_SetUpConfig failed.\r\n");
    while (1)
        ;
#endif
}


uint16_t double_bits(uint8_t input) 
{
    uint16_t result = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        if (input & 0x80)
        {
            result |= (1 << (15 - i * 2));
            result |= (1 << (14 - i * 2));
        }
        else
        {
            result &= ~(0 << (15 - i * 2));
            result &= ~(0 << (14 - i * 2));
        }
        input = input << 1;    
    }

    return ((result & 0xFF00) >> 8) | ((result & 0x00FF) << 8);
}


uint8_t decode_double_bits(uint16_t encoded)
{
    uint8_t result = 0;
    for (int i = 0; i < 8; i++)
    {
        uint16_t two_bits = (encoded >> (14 - i * 2)) & 0x3; // 0x3 = 0b11

        if (two_bits == 0x3)       // 11 -> 1
        {
            result |= (1 << (7 - i));
        }
        else if (two_bits == 0x0)  // 00 -> 0
        {
            // result
        }
    }
    return ((result & 0xF0) >> 4) | ((result & 0x0F) << 4);
}

void data_1mbps_to_500kbps(const uint8_t *data, uint8_t data_len, uint8_t *playload, uint8_t playload_len)
{
	if (playload_len < data_len * 2)
		return ;
	
	uint16_t *playload_p = (uint16_t *)playload;
    
	for(int i = data_len - 1; i >= 0; i--)
	{
		playload_p[i] = double_bits(data[i]);
	}
}
	

void decode_data_1mbps_to_500kbps(const uint16_t *encoded, uint8_t encoded_len, uint8_t *decoded, uint8_t decoded_len)
{
    if (decoded_len < encoded_len)
        return;

    for (int i = 0; i < encoded_len; i++)
    {
        decoded[i] = decode_double_bits(encoded[i]);
    }
}
	

/* ???? -----------------------------------------------*/

void a7535_format(RfSendType t, CtrlPayload_A7535 *controlData)
{
    // ????0xFF
    controlData->ctrl_data.reserved = 0xFF;

    printf("\n");
    printf("===================================\n");
		printf("unique_id      : 0x%02x 0x%02x 0x%02x 0x%02x\n", controlData->unique_id[0], controlData->unique_id[1], controlData->unique_id[2], controlData->unique_id[3]);
    printf("main_mode      : 0x%02x\n", controlData->ctrl_data.main_mode);
    printf("fan_speed      : 0x%02x\n", controlData->ctrl_data.fan_speed);
    printf("air_direction  : 0x%02x\n", controlData->ctrl_data.air_direction);
    printf("air_type       : 0x%02x\n", controlData->ctrl_data.air_type);
    printf("time_setting   : 0x%02x\n", controlData->ctrl_data.time_setting);
    printf("remain_hours   : 0x%02x\n", controlData->ctrl_data.remain_hours);
    printf("remain_minutes : 0x%02x\n", controlData->ctrl_data.remain_minutes);
    printf("light_ctrl     : 0x%02x\n", controlData->ctrl_data.light_ctrl);
    printf("wifi_ctrl      : 0x%02x\n", controlData->ctrl_data.wifi_ctrl);
    printf("voice_ctrl     : 0x%02x\n", controlData->ctrl_data.voice_ctrl);
    printf("model_type     : 0x%02x\n", controlData->ctrl_data.model_type);
    printf("===================================\n");

}

void xn297l_format(RfSendType t, CtrlPayload_XN297L *controlData)
{	
    if (!is_pair)
    {
        printf("\n");
        printf("===================================\n");
        printf("unique_id      : 0x%02x 0x%02x 0x%02x 0x%02x\n", controlData->unique_id[0], controlData->unique_id[1], controlData->unique_id[2], controlData->unique_id[3]);
        printf("main_mode      : 0x%02x\n", controlData->ctrl_data.main_mode);
        printf("fan_speed      : 0x%02x\n", controlData->ctrl_data.fan_speed);
        printf("air_direction  : 0x%02x\n", controlData->ctrl_data.air_direction);
        printf("air_type       : 0x%02x\n", controlData->ctrl_data.air_type);
        printf("time_setting   : 0x%02x\n", controlData->ctrl_data.time_setting);
        printf("remain_hours   : 0x%02x\n", controlData->ctrl_data.remain_hours);
        printf("remain_minutes : 0x%02x\n", controlData->ctrl_data.remain_minutes);
        printf("light_ctrl     : 0x%02x\n", controlData->ctrl_data.light_ctrl);
        printf("wifi_ctrl      : 0x%02x\n", controlData->ctrl_data.wifi_ctrl);
        printf("voice_ctrl     : 0x%02x\n", controlData->ctrl_data.voice_ctrl);
        printf("model_type     : 0x%02x\n", controlData->ctrl_data.model_type);
        printf("smell          : 0x%02x\n", controlData->ctrl_data.smell);
        printf("bluetooth_music: 0x%02x\n", controlData->ctrl_data.bluetooth_music);
        printf("filter         : 0x%02x\n", controlData->ctrl_data.filter);
        printf("===================================\n");
    }
    else
    {
        PairPayload_XN297L *pair_data = (PairPayload_XN297L *)controlData;
        printf("\n");
        printf("================配对===============\n");
        
        printf("ID: ");
        for (int i = 0; i < sizeof(pair_data->id); i++) {
            printf("%02X ", pair_data->id[i]);
        }
        printf("\n");
    
        printf("unique_ID: ");
        for (int i = 0; i < sizeof(pair_data->unique_id); i++) {
            printf("%02X ", pair_data->unique_id[i]);
        }
        printf("\n");
    
        printf("placeholder: ");
        for (int i = 0; i < sizeof(pair_data->placeholder); i++) {
            printf("%02X ", pair_data->placeholder[i]);
        }
        printf("\n");
    
        printf("model_type: %02X\n", pair_data->model_type);
    
        printf("reserved: ");
        for (int i = 0; i < sizeof(pair_data->reserved); i++) {
            printf("%02X ", pair_data->reserved[i]);
        }
        printf("\n");
    
    
        printf("===================================\n");
    
	}
    
}

