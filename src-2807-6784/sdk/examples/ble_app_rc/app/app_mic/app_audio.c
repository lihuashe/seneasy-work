#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "peripheral.h"
#include "app_audio.h"
#include "cc.h"
#include "app_led.h"
#include "app_keyboard.h"
#include "app_atv.h"

#define APP_LOG_DOMAIN      "audio"
#define APP_LOG_LEVEL       APP_AUDIO_LOG_LEVEL
#include "co_debug.h"

#if (BLE_AUDIO_ENABLE)
#include "radio.h"
#include "audio.h"
#include "i2s.h"    
#include "app_atv.h"
#include "ringbuffer.h"


static void i2s_rx_dma_handler(dma_status_t status, uint32_t cur_src_addr, uint32_t cur_dst_addr, uint32_t xfer_size);
void audio_raw_data_in(uint8_t *pdata, uint16_t len);

#define MIC_OPEN_DELAY			20
static unsigned char voice_data_delay = 0;

bool g_mic_status = false;

unsigned char audio_encode_data[512];
unsigned char audio_pcm_data[512];
	
/// i2s config
#pragma pack(4)
int16_t   i2s_dma_buffer[I2S_DMA_BUFFER_SIZE];
#pragma pack()

dma_fifo_t i2s_dma_rx_fifo;
static XH_DMA_CH_Type *i2s_dma_rx = NULL;
DMA_DECLARE_LLIP(i2s_dma_block_llip_rx, I2S_BLOCK_NUM);

const i2s_dma_config_t i2s_dma_rx_cfg = {
    .use_fifo       = true,
    .buffer         = i2s_dma_buffer,
    .buffer_len     = sizeof(i2s_dma_buffer),
    .block_llip     = i2s_dma_block_llip_rx,
    .block_num      = I2S_BLOCK_NUM,
    .callback       = i2s_rx_dma_handler,
};


const i2s_config_t i2s_cfg = {
    .sample_rate  = I2S_SAMPLE_16K,
    .sample_width = I2S_BITWIDTH_16BIT,
    .ws_width     = I2S_BITWIDTH_16BIT,
    .workmode     = I2S_WORKMODE_SLAVE,
    .pcmmode      = I2S_PCMMODE_MONO, //I2S_PCMMODE_STEREO, //I2S_PCMMODE_MONO,
};


ring_buf_t pcm_ring_buf;
ring_buf_t encode_ring_buf;
unsigned char audio_pcm_buff[AUDIO_PCM_LEN] = {0};
unsigned char audio_encode_buff[AUDIO_ENCODE_LEN] = {0};
unsigned char audio_cb_status = 0;

/**
 * @brief i2s_dma_handler()
 *
 * @param[in] status  
 * @param[in] cur_src_addr  
 * @param[in] cur_dst_addr  
 * @param[in] xfer_size  
 *
 * @return 
 **/
static void i2s_rx_dma_handler(dma_status_t status, uint32_t cur_src_addr, uint32_t cur_dst_addr, uint32_t xfer_size)
{
    uint8_t pcm_frame[PCM_FRAME_SIZE+1] = {0};
 
    if(!g_mic_status)
        return;
   
	dma_fifo_set_in_pointer(&i2s_dma_rx_fifo, cur_dst_addr);
	while(dma_fifo_len(&i2s_dma_rx_fifo) >= PCM_FRAME_SIZE)
	{
		dma_fifo_out(&i2s_dma_rx_fifo, pcm_frame, PCM_FRAME_SIZE);           
		audio_raw_data_in(pcm_frame, PCM_FRAME_SIZE);
	}
}

static void _audio_codec_start(void)
{
    memset(&i2s_dma_buffer, 0, sizeof(i2s_dma_buffer));

    i2s_rx_config(&i2s_cfg);

    /// dma setting
    dma_fifo_init(&i2s_dma_rx_fifo, i2s_dma_buffer, sizeof(i2s_dma_buffer));
    dma_init();
    
    i2s_dma_rx = i2s_rx_dma_config((XH_DMA_CH_Type *)NULL, &i2s_dma_rx_cfg);

    /// start dm rx
    dma_start_with_lli(i2s_dma_rx);

    /// start i2s
    i2s_rx_start(&i2s_cfg);
}

static void _audio_codec_stop(void)
{
    /// close i2s
    i2s_rx_stop();

    /// stop dma
    dma_stop(i2s_dma_rx);
    dma_release(i2s_dma_rx);
    i2s_dma_rx = NULL;
}


void app_audio_open_callback(void* param)
{	
	APP_LOG_INF_FUNC();
	
    // audio adc setting
    audio_config_t config;
	
    
    config.pga_gain      = AUDIO_MIC_IN_PGA_GAIN_24DB;		// 调节语音增益，值越大底噪越大
    config.adc_gain      = AUDIO_ADC_GAIN_0DB;						//AUDIO_ADC_GAIN_0DB;

	audio_cb_status = 0;
	
    audio_set_config(&config);

    //codec config and start
    _audio_codec_start();
	
    //co_timer_set(AUDIO_DATA_DELAY_SEND_TID, 500, TIMER_ONE_SHOT, audio_data_delay_timer_handler, NULL);
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void mic_start_transfer(void)
{
	APP_LOG_INF_FUNC();
	
    if(g_mic_status)
    {
        APP_LOG_INF("voice already open\n");
        return;
    }
	
    g_mic_status = true;


    // analog mic input config ---start
    // the number of input pins is decided by the hardware designment
    
    uint8_t pin_2 = 2;
    uint8_t pin_3 = 3;
    
	pcm_ring_buf.buf = audio_pcm_buff;
	encode_ring_buf.buf = audio_encode_buff;

	if (encode_ring_buf.buf == NULL || pcm_ring_buf.buf == NULL)
    {
        APP_LOG_INF("encode init err\r\n");
    }
	
    ring_buf_create(&pcm_ring_buf,pcm_ring_buf.buf,AUDIO_PCM_LEN);
	ring_buf_create(&encode_ring_buf,encode_ring_buf.buf,AUDIO_ENCODE_LEN);	
	
    // 1. audio adc pin setting
    pmu_pin_mode_set(BITMASK(pin_2), PMU_PIN_MODE_FLOAT);
    pmu_pin_mode_set(BITMASK(pin_3), PMU_PIN_MODE_FLOAT);
    REGW0(&XH_PMU->GPIO_IE_CTRL, BITMASK((pin_2)));
    REGW0(&XH_PMU->GPIO_IE_CTRL, BITMASK((pin_3)));
    pmu_pin_driven_current_set(BITMASK(pin_2), PMU_PIN_DRIVEN_CURRENT_2MA);
    pmu_pin_driven_current_set(BITMASK(pin_3), PMU_PIN_DRIVEN_CURRENT_2MA);
    
    // analog mic input config ---end 

	audio_cb_status = 1;
		
	voice_data_delay = 0;
	
    /// start audio adc
    audio_open(app_audio_open_callback);
}


void mic_stop_transfer(void)
{
    if(!g_mic_status) 
	{
        APP_LOG_INF("voice already close\n");
        return;
    }

    g_mic_status = false;

	APP_LOG_INF("is true = %d\r\n",(pcm_ring_buf.buf != NULL));		
	
	if (pcm_ring_buf.buf != NULL)
	{
		//os_free(pcm_ring_buf.buf);	
		APP_LOG_INF("clear pcm_ring_buf.buf\r\n");
		pcm_ring_buf.buf = NULL;	
	}		

	if (encode_ring_buf.buf != NULL)
	{
		//os_free(encode_ring_buf.buf);
		APP_LOG_INF("clear encode_ring_buf.buf\r\n");
		encode_ring_buf.buf = NULL;
	}
	
	if (!audio_cb_status)	
    {
		/// close i2s
		i2s_rx_stop();

		/// stop dma
		dma_stop(i2s_dma_rx);
		dma_release(i2s_dma_rx);
		i2s_dma_rx = NULL;
	}
	audio_cb_status = 0;
	voice_data_delay = 0;
    audio_close();
	APP_LOG_INF("mic_stop_transfer<<<\r\n");
}

void ui_enable_mic (int en, unsigned char ctl_type)
{
#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
	device_led_setup(led_cfg[en ? LED_VOICE_PRESS : LED_VOICE_RELEASE]);
#endif

	APP_LOG_INF("ui_enable_mic\r\n");
	if (g_board_status.ui_mic_enable != en)
	{
		//开启超时定时器
		if (ctl_type == GOOGLE_AUDIO_TYPE)
		{
			#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
			atv_ui_enable_mic(en);
			#endif
		}
		else if (ctl_type == OTHER_AUDIO_TYPE)
		{
			
		}	
		
		if(en)
		{  //audio on
			APP_LOG_INF("audio on\r\n");			
			mic_start_transfer();
		}
		else
		{  
			//audio off
			APP_LOG_INF("audio off\r\n");
			mic_stop_transfer();
		}
		g_board_status.ui_mic_enable = en;
		g_board_status.ui_mic_enable_type = ctl_type;
	}
}


void disconnect_mic_close(void)
{
	if (g_board_status.ui_mic_enable_type == GOOGLE_AUDIO_TYPE)
	{
		#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
		ui_enable_mic(0,GOOGLE_AUDIO_TYPE);
		atv_disconnect_mic_close();
		#endif
	}
	else if (g_board_status.ui_mic_enable_type == OTHER_AUDIO_TYPE)
	{
		;
	}	
}

/**pcm数据输入*/
void audio_raw_data_in(uint8_t *pdata, uint16_t len)
{
    //语音数据入队
	if(voice_data_delay >= MIC_OPEN_DELAY)
	{
		ring_buf_put(&pcm_ring_buf,(unsigned char*)pdata,len);		
	}
	else
	{
		voice_data_delay++;
	}
}

void app_audio_init(void)
{
	#if ((BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)&& (BLE_APP_AUDIO_TYPE & OTHER_AUDIO_TYPE))
	g_board_status.app_audio_type = OTHER_AUDIO_TYPE;
	#elif (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
	g_board_status.app_audio_type = GOOGLE_AUDIO_TYPE;
	#elif (BLE_APP_AUDIO_TYPE & OTHER_AUDIO_TYPE)
	g_board_status.app_audio_type = OTHER_AUDIO_TYPE;
	#endif
	
	#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)	
	atv_audio_init();	
	#endif

}
#endif
