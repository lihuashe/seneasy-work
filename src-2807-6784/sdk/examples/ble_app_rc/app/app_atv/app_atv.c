/*******************************************************************************************
*                    Copyright (c) 2021, XIHAO Technology Co., Ltd.
* @file        app_atv.c
* @author      Reborn    
* @version     V1.0    
* @date        2020-12-30
* @brief       atv 语音操作文件
*********************************************************************************************/


#include "peripheral.h"
#include "app_led.h"
#include "app_keyboard.h"
#include "atv_service.h"
#include "hid_service.h"
#include "ringbuffer.h"
#include "app_audio.h"
#include "app_atv.h"

#define APP_LOG_DOMAIN      "atv"
#define APP_LOG_LEVEL       APP_ATV_LOG_LEVEL
#include "co_debug.h"

#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)

/*atv cmd*/
#define AUDIO_GOOGLE_CMD_CAP                0x0A
#define AUDIO_GOOGLE_CMD_OPEN               0x0C
#define AUDIO_GOOGLE_CMD_CLOSE              0x0D
#define AUDIO_GOOGLE_CMD_EXTEND             0x0E
#define ATV_MIC_CHAR_CTL_AUDIO_STOP         0x00
#define ATV_MIC_CHAR_CTL_AUDIO_START        0x04
#define ATV_MIC_CHAR_CTL_SEARCH             0x08
#define ATV_MIC_CHAR_CTL_DPAD_SELECT        0x07
#define ATV_MIC_CHAR_CTL_SYNC               0x0A
#define ATV_MIC_CHAR_CTL_CAPS_RESP          0x0B
#define ATV_MIC_CHAR_CTL_MIC_OPEN_ERROR     0x0C

/*atv version*/
#define	GOOGLE_VOICE_VERSION_0P4 	0x0004
#define	GOOGLE_VOICE_VERSION_1P0 	0x0100


/*atv cmd param*/
#define CODEC_USED_8K                       0x0001
#define CODEC_USED_16K                      0x0002
#define CODEC_USED_OPUS                     0x0004      //not support

#define CODEC_SUPPORTED_8K                  0x01        //ADPCM,8khz/16bit
#define CODEC_SUPPORTED_16K                 0x02        //ADPCM,16khz/16bit(default)
#define CODEC_SUPPORTED_8K16K               0x03        //ADPCM (8khz/16bit & 16khz/16bit)
#define CODEC_SUPPORTED_8KOPUS              0x05        //Opus and ADPCM 8khz/16bit
#define CODEC_SUPPORTED_8K16KOPUS           0x07        //Opus and ADPCM (8khz/16bit & 16khz/16bit)

#define AUDIO_FRAME_SIZE					120

typedef enum {
    REASON_MICOPEN                          = 0x00,
    REASON_PTT                              = 0x01,
    REASON_HTT                              = 0x03,
}GOOGLE_VOICE_REASON_E;


/**/
#define FIRST_SYNC                          BIT(0)
#define SAMPLING_CHANGE                     BIT(1)
#define PACKET_LOSS                         BIT(2)
#define SYNC_PACKET                         BIT(3)
typedef enum {
    REASON_MICCLOSE                         = 0x00,
    REASON_RELEASE_HTT                      = 0x02,
    REASON_UPCOMING_AUDIO_START             = 0x04,
    REASON_TIMEOUT                          = 0x08,
    REASON_DISABLE_CCC                      = 0x10,
    REASON_OTHERS                           = 0x80,
}AudioStop_TypeDef;

typedef enum {
    PLAYBACK_MODE                           = 0x00,
    CAPTURE_MODE                            = 0x01,
}MicOpenMode_TypeDef;


typedef enum {
     //google voice v0.4e
    ERROR_INVALIED_CODEC                    = 0x0F01,
    //google voice v1.0
    ERROR_MIC_ALREADY_OPEN                  = 0x0F01,
    ERROR_RCU_NOT_ACTIVE                    = 0x0F02,
    ERROR_CCC_NOT_ENABLED                   = 0x0F03,
    ERROR_ONGOING_PTT_HTT                   = 0x0F04,
    ERROR_INTERNAL_ERROR                    = 0x0FFF,
}MicOpenError_TypeDef;

/***************************google event*****************************/
#define FLAG_GOOGLE_AUDIO_IDLE				0

/*google audio event define*/
#define FLAG_NOTIFY_AUDIO_DATA              BIT(5) //notify audio data(0: disable , 1: enable)
#define FLAG_AUDIO_CLOSE                    BIT(4) //

//google voice v0.4e
#define FLAG_GOOGLE_OPEN_ERROR              BIT(0) //mic_open_error.(CHAR_CTL:0x0c,error_code(2))
#define FLAG_GOOGLE_AUDIO_START             BIT(1) //auido_start.(CHAR_CTL:0x04)
#define FLAG_GOOGLE_AUDIO_SYNC              BIT(2) //auido_sync.(CHAR_CTL:0x0A,frame number(2))

//google voice v1.0
#define FLAG_GOOGLE_AUDIO_FIRST_SYNC        BIT(3)             	//used for send sync cmd before start to send audio data


typedef struct {
	unsigned short google_voice_ver;		/*remote google version*/
	unsigned char used_assistant_model;		/*assistant model ->GOOGLE_VOICE_REASON_E*/
	unsigned short atv_char_ctl_ccc;
	unsigned short atv_char_rx_ccc;
	
	unsigned int google_audio_event;
	unsigned short google_audio_event_code;
	unsigned char audio_start_reason;
	unsigned char audio_stop_reason;
	unsigned char mic_open_mode;
	unsigned short google_voice_codec_used;

    //v0.4
    unsigned char app_audio_sync_serial;
    //adpcm parameter
    int adpcm_predict;
    int adpcm_predict_idx;
    unsigned short adpcm_sequence_num;
	unsigned char stream_id;
}ATV_EVENT_CNT_T;



ATV_EVENT_CNT_T g_atv_event_cnt = {
	.google_voice_ver = GOOGLE_VOICE_VERSION_0P4,
	.used_assistant_model = REASON_MICOPEN,
	.atv_char_ctl_ccc = 1,
	.atv_char_rx_ccc = 1,
};


static co_timer_t atv_timeout_timer;
static co_timer_t atv_audio_send_timer;

static co_timer_t atv_event_timer;

static void mic_to_adpcm_split (signed short *ps, int len, signed short *pds, int start);
static void atv_timeout_handle(co_timer_t *timer, void *param);

extern ring_buf_t encode_ring_buf;
extern ring_buf_t pcm_ring_buf;

unsigned char atvv_transmit_flag = ATVV_VOICE_STOP;

void atv_task_audio(co_timer_t *timer, void *param);

	
void atv_event_send(unsigned int event_id, unsigned short event_code)
{
	co_timer_set(&atv_event_timer,10,TIMER_ONE_SHOT,
		atv_task_audio, NULL);
}

/**
 * @brief   initialize voice control parameters
 * @param   none
 * @return  none
 */
void app_audio_parameter_init()
{

    //v0.4
    g_atv_event_cnt.app_audio_sync_serial = 0;

    //adpcm parameter
    g_atv_event_cnt.adpcm_predict      = 0;
    g_atv_event_cnt.adpcm_predict_idx  = 1;

    g_atv_event_cnt.adpcm_sequence_num = 0;
}

/**
 * @brief   initial settings before sending voice.
 * @param   none
 * @return  none
 */
void google_mic_enable()
{
    app_audio_parameter_init();
    ui_enable_mic(1,GOOGLE_AUDIO_TYPE);
	
    g_atv_event_cnt.google_audio_event |= FLAG_NOTIFY_AUDIO_DATA;
    g_atv_event_cnt.google_audio_event |= FLAG_GOOGLE_AUDIO_FIRST_SYNC;
	APP_LOG_INF("g_atv_event_cnt.google_audio_event = %d\r\n",g_atv_event_cnt.google_audio_event);
	atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
}


/**
 * @brief   set FLAG_GOOGLE_SEARCH bit for send search key(on-request)
 * @param   none
 * @return  none
 */
void google_voice_on_request()
{
    APP_LOG_INF("sendData = ATV_MIC_CHAR_CTL_SEARCH\n");
	unsigned char sendData[2] = {0};

	sendData[0] = ATV_MIC_CHAR_CTL_SEARCH;
	atv_ntf_cmd(sendData,1);

//	unsigned char key_buf[8] = {0,0,0,0,0,0,0,0};
//	key_buf[2] = 0x45;
//	hid_gatt_report_notify(KEYBOARD_HANDLE, key_buf, 8);
//	key_buf[2] = 0;
//	hid_gatt_report_notify(KEYBOARD_HANDLE, key_buf, 8);
	sendData[0] = 0x21;
	sendData[1] = 0x02;
	atv_ntf_key(sendData,2);

	sendData[0] = 0x00;
	sendData[1] = 0x00;
	atv_ntf_key(sendData,2);
}

/**
 * @brief   set FLAG_GOOGLE_DPAD_SELECT bit for send DPAD cmd.(ver 0.4e 3.2.3)
 * @param   none
 * @return  none
 */
void google_voice_dpad_select()
{
    if(g_atv_event_cnt.google_voice_ver == GOOGLE_VOICE_VERSION_0P4)
	{
        unsigned char sendData = ATV_MIC_CHAR_CTL_DPAD_SELECT;
		atv_ntf_cmd(&sendData,1);
    }
}

/**
 * @brief       this function is used to start the voice.
 * @param[in]   start_reason:audio start reason
 * @return      none
 */
unsigned char app_audio_key_start(unsigned char start_reason){

    APP_LOG_INF("app_audio_key_start\n");

    if(!g_atv_event_cnt.atv_char_rx_ccc)
	{
		APP_LOG_INF("ATVV_CHAR_AUDIO ccc is disabled\n");
		return 1;                        //ATVV_CHAR_AUDIO ccc is disabled
	}
	
    if(g_atv_event_cnt.google_voice_ver != GOOGLE_VOICE_VERSION_1P0) 
	{
		APP_LOG_INF("google voice version error\n");
		return 2;                		//google voice version error
	}
	
    //if(start_reason > used_assistant_model) return 3;    //Unavailable model
    //if(!enable_rsp_gaps_rsp)    return 3;

    unsigned char sendBuff[20]  = {0};

    switch(start_reason)
	{
        case REASON_PTT:
        {
            APP_LOG_INF("audio_start_reason = REASON_PTT\n");
            g_atv_event_cnt.audio_start_reason = REASON_PTT;
        }
        case REASON_HTT:
        {
            if(start_reason == REASON_HTT)
			{
                g_atv_event_cnt.audio_start_reason = REASON_HTT;
                printf("audio_start_reason = REASON_HTT\n");
            }
			
            g_atv_event_cnt.stream_id++;    //increase stream_id
			
            if(g_atv_event_cnt.stream_id > 0x80) 
			{
				g_atv_event_cnt.stream_id = 1;    //stream_id range 0x01~0x80
			}
			
            g_atv_event_cnt.google_voice_codec_used = CODEC_USED_16K;
			
//            google_voice_codec_used = CODEC_USED_8K;

            //init audio parameter
            sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
            sendBuff[1] = g_atv_event_cnt.audio_start_reason;
            sendBuff[2] = U16_LO(g_atv_event_cnt.google_voice_codec_used);
            sendBuff[3] = g_atv_event_cnt.stream_id;

            atv_ntf_cmd(sendBuff,4);

            google_mic_enable();

            break;
        }
        case REASON_MICOPEN:
        {
            APP_LOG_INF("audio_start_reason = REASON_MICOPEN\n");
            //send google voice CHAR_CTL search
            google_voice_on_request();
            break;
        }
        default:
        {
            return 4;    //not supported
        }
    }
    return 0;
}


/**
 * @brief       this function is used to stop the voice.
 * @param[in]   reason:Audio stop reason
 * @return      none
 */
unsigned char app_audio_key_stop(unsigned char reason)
{


    g_atv_event_cnt.audio_stop_reason = reason;
    g_atv_event_cnt.google_audio_event |= FLAG_AUDIO_CLOSE;
	atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);

    ui_enable_mic(0,GOOGLE_AUDIO_TYPE);
    return 0;
}

/**
 * @brief   this function is used to stop the voice by HTT.
 * @param   none
 * @return  0: ready to stop voice  1:voice start reason is not HTT
 */
unsigned char app_audio_key_stop_by_htt(void)
{
    if(g_atv_event_cnt.audio_start_reason != REASON_HTT) 
	{
		APP_LOG_INF("voice start reason is not HTT\r\n");
		return 1;
	}
	
    app_audio_key_stop(REASON_RELEASE_HTT);

    return 0;
}

/**
 * @brief   according to the version of google voice, start the corresponding process
 * @param   none
 * @return  none
 */
void google_voice_start(void)
{
	if(g_atv_event_cnt.google_audio_event & FLAG_NOTIFY_AUDIO_DATA)
	{
		APP_LOG_INF("google_audio_event:%d\r\n",g_atv_event_cnt.google_audio_event);
		return;
	}
	
    if(g_atv_event_cnt.google_voice_ver == GOOGLE_VOICE_VERSION_1P0)
	{
        app_audio_key_start(g_atv_event_cnt.used_assistant_model);
    }
	else
	{
        google_voice_on_request();
    }
}

/**
 * @brief   this function is used to stop the voice.
 * @param   none
 * @return  none
 */
void google_voice_stop(void)
{
    if(g_atv_event_cnt.audio_start_reason == REASON_HTT)
	{
        app_audio_key_stop_by_htt();
	}
}

void google_key_release(void)
{
	APP_LOG_INF("g_atv_event_cnt.audio_start_reason = %x,g_atv_event_cnt.google_audio_event = %x\r\n,",g_atv_event_cnt.audio_start_reason,g_atv_event_cnt.google_audio_event);
	if((g_atv_event_cnt.audio_start_reason == REASON_HTT) && (g_atv_event_cnt.google_audio_event & FLAG_NOTIFY_AUDIO_DATA)){
		APP_LOG_INF("key release_audio_start_reason:0x%x\n",g_atv_event_cnt.audio_start_reason);
		g_atv_event_cnt.google_audio_event |= FLAG_AUDIO_CLOSE;
		g_atv_event_cnt.audio_stop_reason = REASON_RELEASE_HTT;
		g_atv_event_cnt.audio_start_reason = 0;
		ui_enable_mic(0,GOOGLE_AUDIO_TYPE);
		atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
	}
}




/**
 * @brief   notify CAPS_RESP packet.
 * @param   none
 * @return  none
 */
void google_get_rsp(void)
{
    unsigned char sendBuff[9] = {0};
    if(g_atv_event_cnt.google_voice_ver == GOOGLE_VOICE_VERSION_1P0)
	{
        sendBuff[0] = ATV_MIC_CHAR_CTL_CAPS_RESP;
        sendBuff[1] = U16_HI(GOOGLE_VOICE_VERSION_1P0);     //major version
        sendBuff[2] = U16_LO(GOOGLE_VOICE_VERSION_1P0);     //minor version
        sendBuff[3] = CODEC_SUPPORTED_16K;				//codecs supported (1)
        sendBuff[4] = g_atv_event_cnt.used_assistant_model;  //set assistant interaction model.  on-request,ptt,htt
        sendBuff[5] = 0x00;                                 //audio frame size (2)
        sendBuff[6] = AUDIO_FRAME_SIZE;              		//audio frame size (2)
        sendBuff[7] = ((AUDIO_FRAME_SIZE+3) > 23)?1:0;      //extra configuration (1)
        sendBuff[8] = 0x00;                                 //reserved(1)
	}
	else
	{
		sendBuff[0] = ATV_MIC_CHAR_CTL_CAPS_RESP;
		sendBuff[1] = U16_HI(GOOGLE_VOICE_VERSION_0P4);     //major version
		sendBuff[2] = U16_LO(GOOGLE_VOICE_VERSION_0P4);     //minor version
		sendBuff[3] = 0x00;                                 //codecs_supported high
		sendBuff[4] = CODEC_SUPPORTED_16K;                  //CODEC_SUPPORTED_8K16K;
		sendBuff[5] = 0x00;                                 //frame lengths high
		sendBuff[6] = 0x86;                                 //frame lengths low -- 134Byts
		sendBuff[7] = 0x00;                                 //packet lengths high
		sendBuff[8] = 0x14;                                 //packet lengths low -- 20 Bytes
    }
	
	atv_ntf_cmd(sendBuff,9);
}

void audio_google_cap_handle(unsigned char* dat)
{
	unsigned short voice_ver = (dat[1]<<8) + dat[2];
	if(ENABLE_GOOGLE_VOICE_1P0 && voice_ver == GOOGLE_VOICE_VERSION_1P0)
	{
		APP_LOG_INF("GOOGLE_VOICE_VERSION_1P0\n");
		
		g_atv_event_cnt.google_voice_ver = GOOGLE_VOICE_VERSION_1P0;

		APP_LOG_INF("supported_assistant_models:0x%x\n",dat[5]);
		g_atv_event_cnt.used_assistant_model = dat[5];//REASON_MICOPEN; //
	}
	else
	{
		APP_LOG_INF("GOOGLE_VOICE_VERSION_0P4\n");
		g_atv_event_cnt.google_voice_ver = GOOGLE_VOICE_VERSION_0P4;
	}
	
	google_get_rsp();
}

/**
 * @brief       google voice 0.4 processing function.
 * @param[in]   pw:received RF data
 * @return      none
 */
void google_voice_v0p4(unsigned char *dat, int len)
{
    unsigned char sendBuff[20]  = {0};

    unsigned char cmd = dat[0];
	
    if(cmd == AUDIO_GOOGLE_CMD_OPEN)
    {
        //v0.4e proc
        unsigned short ATV_codec_used = 2;
        APP_LOG_INF("received pkt len:%d\n",len);

        if(len == 3)
		{
            ATV_codec_used = (dat[1]<<8) + dat[2];
        }
		else
		{
            ATV_codec_used = 2;
        }

        APP_LOG_INF("received v0.4 mic open:%d\n",ATV_codec_used);

        switch(ATV_codec_used)
		{
            case CODEC_USED_8K:
            {
                APP_LOG_INF("ADPCM, 8khz/16bit\n");

                g_atv_event_cnt.google_voice_codec_used = CODEC_USED_8K; //8k
				
                sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
                
				atv_ntf_cmd(sendBuff,1);

                google_mic_enable();

                break;
            }
            case CODEC_USED_16K:
            {
                APP_LOG_INF("ADPCM, 16khz/16bit\n");

                g_atv_event_cnt.google_voice_codec_used = CODEC_USED_16K;

 			    sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
				
                atv_ntf_cmd(sendBuff,1);

                google_mic_enable();

                break;
            }
            case CODEC_USED_OPUS:
                //printf("Opus(future),current sdk do Not support\n");
            default:
            {
                APP_LOG_INF("only support 8k voice, default ADPCM, 8khz/16bit\n");

                g_atv_event_cnt.google_voice_codec_used = CODEC_USED_16K; //8k

                sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
                atv_ntf_cmd(sendBuff,1);

                google_mic_enable();
                break;
            }
        }

    }
	else if(cmd == AUDIO_GOOGLE_CMD_CLOSE)
	{
        APP_LOG_INF("AUDIO_GOOGLE_CMD_CLOSE\n");



        //send audio close cmd
        g_atv_event_cnt.google_audio_event |= FLAG_AUDIO_CLOSE;
		atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);

        ui_enable_mic(0,GOOGLE_AUDIO_TYPE);
    }
}

/**
 * @brief       verify stream id.
 * @param[in]   received stream id from host
 * @return      0: OK  1: audio_start_reason is not REASON_MICOPEN
 *              2: stream id out of range  3: the received id is inconsistent with the current id
 */
unsigned char verify_stream_id(unsigned char id)
{

    switch(id)
	{
        case 0x00:        //  Check if start reason is on-request.
        {
            if(g_atv_event_cnt.audio_start_reason != REASON_MICOPEN)  
			{
				return 1;
			}
            break;
        }
        case 0xff:        //any ongoing audio stream.
        {
            break;
        }
        default:        //when start reason is htt or ptt,check the stream_id.
        {
            if(id > 0x80) 
			{
				return 2;    //audio stream id range: 0x01 ~ 0x80
			}

            if(id != g_atv_event_cnt.stream_id) 
			{
				return 3;
			}
            break;
        }
    }

    return 0;
}

/**
 * @brief       google voice 1.0 processing function.
 * @param[in]   received RF data
 * @return      none
 */
void google_voice_v1p0(unsigned char* dat)
{
    //AUDIO_START 
	if(dat[0] == AUDIO_GOOGLE_CMD_OPEN)
	{
        APP_LOG_INF("AUDIO_GOOGLE_CMD_OPEN\n");
		
        //check ccc
        if(!g_atv_event_cnt.atv_char_rx_ccc)
		{
            APP_LOG_INF("atv_char_rx_ccc is disabled\n");

            //notifications are disabled while audio data transfer is in progress
            g_atv_event_cnt.google_audio_event |= FLAG_GOOGLE_OPEN_ERROR;
            g_atv_event_cnt.google_audio_event_code = ERROR_CCC_NOT_ENABLED;
			atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
            return;
        }

        unsigned char mic_mode = dat[1];
		
		//audio repeat turn on
        if(g_atv_event_cnt.google_audio_event & FLAG_NOTIFY_AUDIO_DATA)
		{    
			//ongoing audio stream
            APP_LOG_INF("ongoing audio stream_audio_start_reason:%d\n",g_atv_event_cnt.audio_start_reason);
			
            switch(g_atv_event_cnt.audio_start_reason)
			{
                case REASON_PTT:
                case REASON_HTT:
                {
                    //send open error.microphone is open because of ongoing PTT/HTT interaction
                    g_atv_event_cnt.google_audio_event_code = ERROR_ONGOING_PTT_HTT;
                    g_atv_event_cnt.google_audio_event |= FLAG_GOOGLE_OPEN_ERROR;
					atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
                    break;
                }
                case REASON_MICOPEN:
                default:
                {
                    //send audio stop and restart audio or ignore cmd
                    //Prevent repeated triggers(2020.12.15)

                    g_atv_event_cnt.google_audio_event |= FLAG_AUDIO_CLOSE;
                    g_atv_event_cnt.audio_stop_reason = REASON_UPCOMING_AUDIO_START;
                    g_atv_event_cnt.audio_start_reason = REASON_MICOPEN;
					atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);

                    ui_enable_mic(0,GOOGLE_AUDIO_TYPE);
                    break;
                }
            }
        } //open audio
		else
		{
            APP_LOG_INF("new audio. mic mode:0x%x \n",mic_mode);
			
            //start audio
            switch(mic_mode)
			{
                case PLAYBACK_MODE:
                {
                    g_atv_event_cnt.mic_open_mode = PLAYBACK_MODE;
                    break;
                }
                case CAPTURE_MODE:
//                  mic_open_mode = CAPTURE_MODE;
//                  break;
                default:
                {
                    g_atv_event_cnt.mic_open_mode = CAPTURE_MODE;
                    break;
                }
            }
			
            //SEND AUDIO START
            g_atv_event_cnt.audio_start_reason = REASON_MICOPEN;
            g_atv_event_cnt.google_voice_codec_used = CODEC_USED_16K;

            unsigned char sendBuff[4] = {0};
			
            //init audio parameter
            sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
            sendBuff[1] = g_atv_event_cnt.audio_start_reason;
            sendBuff[2] = U16_LO(g_atv_event_cnt.google_voice_codec_used);
            sendBuff[3] = 0x00;        //an audio stream which was initiated by the MIC_OPEN command
			
			atv_ntf_cmd(sendBuff,4);
			
            google_mic_enable();
        }
    }//AUDIO_STOP
	else if(dat[0] == AUDIO_GOOGLE_CMD_CLOSE)
	{
        APP_LOG_INF("AUDIO_GOOGLE_CMD_CLOSE\n");
		
//        unsigned char close_stream_id = dat[1];

//      if(verify_stream_id(close_stream_id)) 
//		{
//			return;
//		}
		
        //close ongoing audio stream


        g_atv_event_cnt.google_audio_event |= FLAG_AUDIO_CLOSE;
        g_atv_event_cnt.audio_stop_reason = REASON_MICCLOSE;
		atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);

        ui_enable_mic(0,GOOGLE_AUDIO_TYPE);
		APP_LOG_INF("AUDIO_GOOGLE_CMD_CLOSEEND\n");

    }//MIC_EXTEND
	else if(dat[0] == AUDIO_GOOGLE_CMD_EXTEND)
	{
        APP_LOG_INF("AUDIO_GOOGLE_CMD_EXTEND\n");
        unsigned char close_stream_id = dat[1];

        if(verify_stream_id(close_stream_id))
		{
			return;
		}
		
		//复位定时器
		co_timer_set(&atv_timeout_timer, APP_AUDIO_GOOGLE_TIMEOUT, TIMER_ONE_SHOT,
				 atv_timeout_handle, NULL);
    }
}


void atvv_char_tx_handle(unsigned char* dat, int len)
{
	APP_LOG_INF("atv_tx_dat[0]:%x\r\n",dat[0]);
	
	unsigned char google_command = dat[0];

	if (google_command == AUDIO_GOOGLE_CMD_CAP)
	{
		g_board_status.app_audio_type = GOOGLE_AUDIO_TYPE;
		audio_google_cap_handle(dat);
	}
	else
	{
        if(g_atv_event_cnt.google_voice_ver == GOOGLE_VOICE_VERSION_1P0)
		{
            google_voice_v1p0(dat);
        }
		else
		{
            google_voice_v0p4(dat,len);
        }
	}
	
	return;
}





/**
 * @brief   google command process for google voice v0.4.
 * @param   none
 * @return  none
 */
void google_cmd_v0p4_proc()
{
//	APP_LOG_INF("g_atv_event_cnt.google_audio_event = %d",g_atv_event_cnt.google_audio_event);
//	APP_LOG_INF("flag = %d",g_atv_event_cnt.google_audio_event & FLAG_AUDIO_CLOSE);
    if(g_atv_event_cnt.google_audio_event & FLAG_AUDIO_CLOSE)
	{
        //audio stop. google voice auido end
        APP_LOG_INF("google_cmd_v0p4_proc_close\r\n");

        unsigned char sendData = ATV_MIC_CHAR_CTL_AUDIO_STOP;
		atv_ntf_cmd(&sendData,1);

        g_atv_event_cnt.google_audio_event = FLAG_GOOGLE_AUDIO_IDLE;
		atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
        ui_enable_mic(0,GOOGLE_AUDIO_TYPE);
    }

    if(g_atv_event_cnt.google_audio_event & FLAG_GOOGLE_OPEN_ERROR)
	{
		APP_LOG_INF("FLAG_GOOGLE_OPEN_ERROR");
        //mic open error.
        //Timeout1.This is a timeout on the Android TV device.
		
        unsigned char sendBuff[3] = {0};
        sendBuff[0] = ATV_MIC_CHAR_CTL_MIC_OPEN_ERROR;
        sendBuff[1] = U16_HI(ERROR_INVALIED_CODEC);
        sendBuff[2] = U16_LO(ERROR_INVALIED_CODEC);
	
		atv_ntf_cmd(sendBuff,3);
		g_atv_event_cnt.google_audio_event = FLAG_GOOGLE_AUDIO_IDLE;
  
		atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
		
		ui_enable_mic(0,GOOGLE_AUDIO_TYPE); 
    }
}



/**
 * @brief   google command process for google voice v1.0.
 * @param   none
 * @return  none
 */
void google_cmd_v1p0_proc()
{
    if(!g_atv_event_cnt.atv_char_rx_ccc && (g_atv_event_cnt.google_audio_event & FLAG_NOTIFY_AUDIO_DATA))
	{
        //notifications are disabled while audio data transfer is in progress
        g_atv_event_cnt.google_audio_event |= FLAG_AUDIO_CLOSE;
        g_atv_event_cnt.audio_stop_reason = REASON_DISABLE_CCC;
		atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
    }

    //audio stop
    if(g_atv_event_cnt.google_audio_event & FLAG_AUDIO_CLOSE)
	{
        //audio stop. google voice auido end
        //printf("google_cmd_v1p0_proc_close\r\n");

        //if(REASON_RELEASE_HTT == g_atv_event_cnt.audio_stop_reason)
		{
            unsigned char sendBuff[2] = {0};
            sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_STOP;
            sendBuff[1] = g_atv_event_cnt.audio_stop_reason;
			atv_ntf_cmd(sendBuff,2);
			
            APP_LOG_INF("ATV_MIC_CHAR_CTL_AUDIO_STOP:0x%x\n",g_atv_event_cnt.audio_stop_reason);
            g_atv_event_cnt.google_audio_event = FLAG_GOOGLE_AUDIO_IDLE;
//            ui_enable_mic(0,GOOGLE_AUDIO_TYPE);

			if(g_atv_event_cnt.audio_stop_reason == REASON_UPCOMING_AUDIO_START)
			{
				APP_LOG_INF("REASON_UPCOMING_AUDIO_START\r\n");
				g_atv_event_cnt.google_audio_event |= FLAG_GOOGLE_AUDIO_START;
				atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);				
			}
			else
			{
				APP_LOG_INF("REASON_UPCOMING_AUDIO_STOP\r\n");
				atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);		
				ui_enable_mic(0,GOOGLE_AUDIO_TYPE);
			}
        }
    }

    //audio start
    if(g_atv_event_cnt.google_audio_event & FLAG_GOOGLE_AUDIO_START)
	{
        //audio start. triggered by an upcoming AUDIO_START command;
        g_atv_event_cnt.google_voice_codec_used = CODEC_USED_16K;

        unsigned char sendBuff[4] = {0};
        sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
        sendBuff[1] = g_atv_event_cnt.audio_start_reason;
        sendBuff[2] = U16_LO(g_atv_event_cnt.google_voice_codec_used);
        sendBuff[3] = 0x00;        //an audio stream which was initiated by the MIC_OPEN command
		atv_ntf_cmd(sendBuff,4);
		
		g_atv_event_cnt.google_audio_event &= ~FLAG_GOOGLE_AUDIO_START;
		//start send audio data
		google_mic_enable();
		APP_LOG_INF("FLAG_GOOGLE_AUDIO_START\r\n");
    }

    //mic open error
    if(g_atv_event_cnt.google_audio_event & FLAG_GOOGLE_OPEN_ERROR)
	{
        //mic open error.
        //Timeout1.This is a timeout on the Android TV device.
        APP_LOG_INF("g_atv_event_cnt.google_audio_event & FLAG_GOOGLE_OPEN_ERROR:%x",g_atv_event_cnt.google_audio_event_code);

        unsigned char sendBuff[3] = {0};
        sendBuff[0] = ATV_MIC_CHAR_CTL_MIC_OPEN_ERROR;
        sendBuff[1] = U16_HI(g_atv_event_cnt.google_audio_event_code);
        sendBuff[2] = U16_LO(g_atv_event_cnt.google_audio_event_code);
		atv_ntf_cmd(sendBuff,3);
		
		g_atv_event_cnt.google_audio_event = FLAG_GOOGLE_AUDIO_IDLE;
		 
		atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
		
		ui_enable_mic(0,GOOGLE_AUDIO_TYPE); 
    }
}



/**
 * @brief   notify audio data process for google voice v0.4.
 * @param   none
 * @return  none
 */
void google_voice_data_notify_v0p4_proc()
{
	unsigned char temp_data[134];
	uint8_t pkg_num;
    xh_ble_tx_buffer_count_get(&pkg_num);
	
	if ((pkg_num > 0) && (ring_buf_len(&encode_ring_buf) >=  134))
	{
		//encode_queue_pop(temp_data,134);
		ring_buf_get(&encode_ring_buf,temp_data,134);
		
		atv_ntf_audio(&temp_data[0],134);	
		g_atv_event_cnt.app_audio_sync_serial ++;
		
////		//APP_LOG_INF("t4:%d\r\n",gatt_get_tx_buf_nb(0));
////		if(g_atv_event_cnt.app_audio_sync_serial > 10)
////		{
////			g_atv_event_cnt.app_audio_sync_serial =0;
////			//audio sync
////			unsigned char p[2];
////			int p_len = show_encode_data(p,2);
////	      	unsigned short frame_no;
////	      	if(p_len >= 2){
////	      	    frame_no = (p[0] <<8) + p[1];
////	      	}else{
////	      	    frame_no = g_atv_event_cnt.adpcm_sequence_num;
////	      	}
		
////	      	unsigned char sendBuff[3] = {0};
////	      	sendBuff[0] = 0x0A;
////	      	sendBuff[1] = U16_HI(frame_no);
////	      	sendBuff[2] = U16_LO(frame_no);
////			atv_ntf_cmd(sendBuff,3);
////		}
	}
	else
	{
		APP_LOG_INF("pkg_num:%d\r\n",pkg_num);
	}
}


unsigned char  previous_sampling_rate = U16_LO(CODEC_USED_16K);    //default sampling rate:16k
unsigned short previous_seq_no = 0xffff;
/**
 * @brief   notify audio data process for google voice v1.0
 * @param   none
 * @return  none
 */
void google_voice_data_notify_v1p0_proc()
{
	unsigned char temp_data[134] = {0};
    
	uint8_t pkg_num;
    xh_ble_tx_buffer_count_get(&pkg_num);
	
	if ((pkg_num > 2) && (ring_buf_len(&encode_ring_buf) >= 126))
	{
		//encode_queue_pop(temp_data,126);
		unsigned char len = 0;
		len = ring_buf_get(&encode_ring_buf,temp_data,126);
		
        unsigned short current_seq_no = (temp_data[0]<<8) + temp_data[1];
        unsigned char current_sampling_rate = temp_data[2];

        unsigned char  send_sync_cmd_flag = 0;

        //check seq no and sampling rate before notify audio data
        if(current_sampling_rate != previous_sampling_rate)
		{
            send_sync_cmd_flag |= SAMPLING_CHANGE;
        }

        if((unsigned short)(current_seq_no - previous_seq_no) != 1)
		{
            send_sync_cmd_flag |= PACKET_LOSS;
        }

        g_atv_event_cnt.app_audio_sync_serial ++;
        if(g_atv_event_cnt.app_audio_sync_serial >= 20)
		{
            //app_audio_sync_serial = 0;
            send_sync_cmd_flag |= SYNC_PACKET;
        }
        if(g_atv_event_cnt.google_audio_event & FLAG_GOOGLE_AUDIO_FIRST_SYNC)
		{
            APP_LOG_INF("FLAG_GOOGLE_AUDIO_FIRST_SYNC\n");
            g_atv_event_cnt.google_audio_event &= ~FLAG_GOOGLE_AUDIO_FIRST_SYNC;
            //send_sync_cmd_flag |= FIRST_SYNC;
            //need_buffer_count = 1;
            previous_sampling_rate = U16_LO(CODEC_USED_16K);    //default sampling rate:16k
            previous_seq_no = 0xffff;
            g_atv_event_cnt.app_audio_sync_serial = 0;
            APP_LOG_INF("start rate:0x%x",current_sampling_rate);
        }
        //check sync data
        if(send_sync_cmd_flag)
		{
            unsigned char sendBuff[7] = {0};
            //send sync cmd
            unsigned int notify_seq_no = current_seq_no * (120/AUDIO_FRAME_SIZE);
            //APP_LOG_INF("notify_seq_no:%d",notify_seq_no);

            sendBuff[0] = ATV_MIC_CHAR_CTL_SYNC;
            sendBuff[1] = current_sampling_rate;            //The audio codec that will be used for the audio stream after this sync point.
            sendBuff[2] = (notify_seq_no >> 8) & 0xFF;      //Sequence number of the audio frame
            sendBuff[3] = notify_seq_no & 0xFF;             //Sequence number of the audio frame

            sendBuff[4] = temp_data[3];                     //Predicted ADPCM value.
            sendBuff[5] = temp_data[4];                     //Predicted ADPCM value.
            sendBuff[6] = temp_data[5];                     //step index (1) Index in ADPCM step size table.
			
			atv_ntf_cmd(sendBuff,7);
			APP_LOG_INF("send audio sync\r\n");
		}		
		atv_ntf_audio(&temp_data[6],AUDIO_FRAME_SIZE);
		
		g_atv_event_cnt.app_audio_sync_serial = 0;

		previous_sampling_rate = current_sampling_rate;
		previous_seq_no = current_seq_no;
    }
	else
	{
		APP_LOG_INF("pkg_num:%d\r\n",pkg_num);
	}
}

static void atv_timeout_handle(co_timer_t *timer, void *param)
{
	APP_LOG_INF("app_audio_transfer_timer timeout\n");
   //app_audio_disable();
	
	if(g_atv_event_cnt.google_audio_event & FLAG_NOTIFY_AUDIO_DATA)
	{
		g_atv_event_cnt.audio_stop_reason = REASON_TIMEOUT;
		g_atv_event_cnt.google_audio_event |= FLAG_AUDIO_CLOSE;
		atv_event_send(g_atv_event_cnt.google_audio_event,g_atv_event_cnt.google_audio_event_code);
	}
	
	ui_enable_mic(0,GOOGLE_AUDIO_TYPE); 	

}

void audio_data_send(co_timer_t *timer, void *param)
{
	co_timer_del(&atv_audio_send_timer);

	if (g_atv_event_cnt.google_audio_event & FLAG_NOTIFY_AUDIO_DATA)
	{
		//压缩处理
		if (ring_buf_len(&pcm_ring_buf) >= 512)
		{	
			//send voice data
			if(g_atv_event_cnt.google_voice_ver == GOOGLE_VOICE_VERSION_1P0)
			{
				unsigned char len = 0;

				ring_buf_get(&pcm_ring_buf,(unsigned char*)audio_pcm_data,480);

				mic_to_adpcm_split((signed short*)audio_pcm_data,240,(short*)audio_encode_data,g_atv_event_cnt.google_voice_ver);

				len = ring_buf_put(&encode_ring_buf,(unsigned char*)audio_encode_data,126);
				
				google_voice_data_notify_v1p0_proc();			
			}
			else
			{
				ring_buf_get(&pcm_ring_buf,(unsigned char*)audio_pcm_data, 512);
				mic_to_adpcm_split((signed short*)audio_pcm_data,256,(short*)audio_encode_data,1);
				ring_buf_put(&encode_ring_buf,(unsigned char*)audio_encode_data,134); //134
							
				//google voice version error
				google_voice_data_notify_v0p4_proc();
			}
		}
	}
	
	co_timer_set(&atv_audio_send_timer, 3, TIMER_ONE_SHOT,
	audio_data_send, NULL);
}

void atv_task_audio(co_timer_t *timer, void *param)
{
	co_timer_del(&atv_event_timer);

    if(g_atv_event_cnt.google_voice_ver == GOOGLE_VOICE_VERSION_1P0)
	{
//		APP_LOG_INF("GOOGLE_VOICE_VERSION_1P0");
        google_cmd_v1p0_proc();
    }
	else
	{
        google_cmd_v0p4_proc();
    }
}

static const signed char idxtbl[] = { -1, -1, -1, -1, 2, 4, 6, 8, -1, -1, -1, -1, 2, 4, 6, 8};
static const unsigned short steptbl[] = {
     7,  8,  9,  10,  11,  12,  13,  14,  16,  17,
     19,  21,  23,  25,  28,  31,  34,  37,  41,  45,
     50,  55,  60,  66,  73,  80,  88,  97,  107, 118,
     130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
     337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
     876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
     2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
     5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
     15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767   };

#define STREAM_ID    0x00
void mic_to_adpcm_split (signed short *ps, int len, signed short *pds, int start)
{
    int i, j;
    unsigned short code=0;
    unsigned short code16=0;
    static signed short *pd;
    code = 0;

    pd = pds;

    if (start == GOOGLE_VOICE_VERSION_1P0){    //google voice v1.0
        *pd++ = ((g_atv_event_cnt.adpcm_sequence_num>>8)&0x00ff)|((g_atv_event_cnt.adpcm_sequence_num<<8)&0xff00);
        *pd++ = (g_atv_event_cnt.google_voice_codec_used&0x00ff)|((g_atv_event_cnt.adpcm_predict&0xff00));
        *pd++ = ((g_atv_event_cnt.adpcm_predict)&0x00ff)|((g_atv_event_cnt.adpcm_predict_idx<<8)&0xff00);
    }else{                //google voice v0.4
        //printf("ver 0.4_len:%d\n",len);
        //Seq# 2bytes; Id: 1bytes; Prev.pred: 2bytes; index: 1bytes
        *pd++ = ((g_atv_event_cnt.adpcm_sequence_num>>8)&0x00ff)|((g_atv_event_cnt.adpcm_sequence_num<<8)&0xff00);
        *pd++ = (STREAM_ID)|((g_atv_event_cnt.adpcm_predict&0xff00));
        *pd++ = ((g_atv_event_cnt.adpcm_predict)&0x00ff)|((g_atv_event_cnt.adpcm_predict_idx<<8)&0xff00);
    }
    g_atv_event_cnt.adpcm_sequence_num ++;
	
    for(i=0 ; i<len; i++){//unit sample

        signed short di;
        if(g_atv_event_cnt.google_voice_codec_used == CODEC_USED_16K){
            di = ps[i];
        }else{
            di = ps[i*2];
        }

        int step = steptbl[g_atv_event_cnt.adpcm_predict_idx];
        int diff = di - g_atv_event_cnt.adpcm_predict;

        if(diff >= 0){
            code = 0;
        }else{
            diff = -diff;
            code = 8;
        }

        int diffq = step >> 3;

        for(j=4; j>0; j=j>>1){
            if(diff >= step){
                diff = diff - step;
                diffq = diffq + step;
                code = code + j;
            }
            step = step >> 1;
        }

        code16 = (code16 >> 4) | (code << 12);
        if((i&3) == 3){
            code16 = ((code16&0x0f)<<4)|((code16&0xf0)>>4) | ((code16&0x0f00)<<4)|((code16&0xf000)>>4);
            *pd++ = code16;
        }

        if(code >= 8) {
            g_atv_event_cnt.adpcm_predict = g_atv_event_cnt.adpcm_predict - diffq;
        }else {
            g_atv_event_cnt.adpcm_predict = g_atv_event_cnt.adpcm_predict + diffq;
        }

        if(g_atv_event_cnt.adpcm_predict > 32767){
            g_atv_event_cnt.adpcm_predict = 32767;
        }else if (g_atv_event_cnt.adpcm_predict < -32768) {
            g_atv_event_cnt.adpcm_predict = -32768;
        }
		
        g_atv_event_cnt.adpcm_predict_idx = g_atv_event_cnt.adpcm_predict_idx + idxtbl[code&15];

        if(g_atv_event_cnt.adpcm_predict_idx < 0) {
            g_atv_event_cnt.adpcm_predict_idx = 0;
        }else if(g_atv_event_cnt.adpcm_predict_idx > 88){
            g_atv_event_cnt.adpcm_predict_idx = 88;
        }
    }
}

void atv_ui_enable_mic (int en)
{
	if(en)
	{ 
		//开启超时定时器
		APP_LOG_INF("atv_ui_enable_mic open");
		co_timer_set(&atv_timeout_timer, APP_AUDIO_GOOGLE_TIMEOUT, TIMER_ONE_SHOT,
		atv_timeout_handle, NULL);
		
		co_timer_set(&atv_audio_send_timer, 3, TIMER_ONE_SHOT,
		audio_data_send, NULL);
		
//		co_timer_set(&atv_event_timer,10,TIMER_ONE_SHOT,
//		atv_task_audio, NULL);
		
		atvv_transmit_flag = ATVV_VOICE_START;
	}
	else
	{  
		APP_LOG_INF("atv_ui_enable_mic close");		
		co_timer_del(&atv_timeout_timer);
		
		co_timer_del(&atv_audio_send_timer);
		co_timer_del(&atv_event_timer);
		
		atvv_transmit_flag = ATVV_VOICE_STOP;
		g_atv_event_cnt.google_audio_event = FLAG_GOOGLE_AUDIO_IDLE;

	}
}

void atv_disconnect_mic_close(void)
{
	g_atv_event_cnt.google_audio_event = FLAG_GOOGLE_AUDIO_IDLE;
}


void atv_audio_init(void)
{
	//注册atv处理回调
	atvv_transmit_flag = ATVV_VOICE_STOP;
}

#endif
