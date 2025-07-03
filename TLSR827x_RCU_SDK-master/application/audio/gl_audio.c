/******************************************************************************
 * @file     gl_audio.c
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

#include     "tl_common.h"
#include     "drivers.h"
#include    "audio_config.h"
#include    "adpcm.h"
#include     "gl_audio.h"
#include     "tl_audio.h"

#if (TL_AUDIO_MODE == TL_AUDIO_RCU_ADPCM_GATT_GOOGLE)

#include     "stack/ble/ble.h"

extern void ui_enable_mic(int en);

extern u8        buffer_mic_pkt_wptr;
extern u8        buffer_mic_pkt_rptr;

u8 audio_start_reason = 0;        //on-request(0x00),PTT(0x01),HTT(0x03) etc.
u8 audio_stop_reason = 0;


u8 app_audio_send_index = 0;
u8 app_audio_sync_serial = 0;            //Used for google voice v0.4 sync cmd

u16 mic_open_error_code = 0;

_attribute_data_retention_ u8 mic_open_mode = CAPTURE_MODE;        //0x00:playback mode   0x01:capture mode

_attribute_data_retention_ u8 google_voice_ver = 4;    //4: ver 0.4e  10:ver 1.0
_attribute_data_retention_ u8 used_assistant_model = REASON_MICOPEN;
_attribute_data_retention_ u8 get_cap_received = false;

//Google voice v1.0 control parameter
_attribute_data_retention_ u8 app_mtu_size = 23;
_attribute_data_retention_ u8 auido_frame_size = 20;      //Byte. this variable is updated after sending CAPS_RESP
_attribute_data_retention_ u8 stream_id = 0;              //The identifier of an audio stream to extend.
_attribute_data_retention_ u8 flag_active_mic_open = 0;   //Enable/disable mic open command

_attribute_data_retention_ u16 google_voice_ctl = 0;

_attribute_data_retention_ u16 atv_char_ctl_ccc = 0;
_attribute_data_retention_ u16 atv_char_rx_ccc = 0;

_attribute_data_retention_ u16 google_voice_codec_used = CODEC_USED_16K;
_attribute_data_retention_ u16 google_voice_packet_length = VOICE_V0P4_ADPCM_PACKET_LEN;
_attribute_data_retention_ u16 google_voice_pcm_sample_packet = VOICE_V0P4_ADPCM_UNIT_SIZE;

_attribute_data_retention_ u32 app_audio_start_delay = 0;
_attribute_data_retention_ u32 app_active_remote_timer = 0;     //Wait mic open cmd after send START_SEARCH
_attribute_data_retention_ u32 app_audio_transfer_timer = 0;    //Used for google voice timeout

extern int device_in_connection_state;

/**
 * @brief   initialize packet parameters according to the version of google voice
 * @param   none
 * @return  none
 */
void google_voice_para_init(){

    if(google_voice_ver == 4){      //ver0.4e
        google_voice_packet_length = VOICE_V0P4_ADPCM_PACKET_LEN;   //128+6+2
        google_voice_pcm_sample_packet = VOICE_V0P4_ADPCM_UNIT_SIZE;    //

    }else if(google_voice_ver == 10){   //ver1.0
        google_voice_packet_length = VOICE_V1P0_ADPCM_PACKET_LEN;   //
        google_voice_pcm_sample_packet = VOICE_V1P0_ADPCM_UNIT_SIZE;    //

        google_voice_codec_used = CODEC_USED_16K;
    }else{//set to ver0.4e
        google_voice_ver = 4;

        google_voice_packet_length = VOICE_V0P4_ADPCM_PACKET_LEN;   //128+6+2
        google_voice_pcm_sample_packet = VOICE_V0P4_ADPCM_UNIT_SIZE;    //
    }
}

/**
 * @brief   initialize voice control parameters
 * @param   none
 * @return  none
 */
void app_audio_parameter_init(){

    app_audio_send_index = 0;

    //v0.4
    app_audio_sync_serial = 0;
    //app_audio_send_index = 0;

    //adpcm parameter
    adpcm_predict      = 0;
    adpcm_predict_idx  = 1;
    //adpcm_predict_idx  = 0;

    adpcm_sequence_num = 0;

    //adpcm buffer
    buffer_mic_pkt_wptr = 0;
    buffer_mic_pkt_rptr = 0;


//    previous_sampling_rate = U16_LO(CODEC_USED_16K);    //default sampling rate:16k
//    previous_seq_no = 0xffff;
}

/**
 * @brief       this function is used to save the assistant model.
 * @param[in]   model:the model type
 * @return      none
 */
void set_assistant_model(u8 model){
    used_assistant_model = model;
}

/**
 * @brief   this function is used to read the assistant model.
 * @param   none
 * @return  the model type
 */
u8 read_assistant_model(){
    return used_assistant_model;
}

/**
 * @brief   this function is used to enable the mic open command.
 * @param   none
 * @return  none
 */
void active_mic_open(){
    flag_active_mic_open = 1;
    app_active_remote_timer = clock_time()|1;
}

/**
 * @brief   this function is used to read the audio frame size.
 * @param   none
 * @return  the audio frame size
 */
u8 read_audio_frame_size(){

    return auido_frame_size;
}

/**
 * @brief       this function is used to set the audio frame size.
 * @param[in]   frame_size:The frame size(data length exchange)
 * @return      none
 */
void set_audio_frame_size(u8 frame_size){
    auido_frame_size = 20;

    if(frame_size >= 123){
        auido_frame_size = 120;
    }else if(frame_size >= 63){
        auido_frame_size = 60;
    }else if(frame_size >= 43){
        auido_frame_size = 40;
    }else if(frame_size >= 33){
        auido_frame_size = 30;
    }else{
        auido_frame_size = 20;
    }
}

/**
 * @brief   initial settings before sending voice.
 * @param   none
 * @return  none
 */
void google_mic_enable(){

    app_audio_parameter_init();
    ui_enable_mic(1);

    //app_active_remote_timer = 0;
    app_audio_transfer_timer = clock_time() | 1;

    google_voice_ctl |= FLAG_NOTIFY_AUDIO_DATA;
    google_voice_ctl |= FLAG_GOOGLE_AUDIO_FIRST_SYNC;

    google_voice_ctl |= FLAG_DELAY_NOTIFY;
    app_audio_start_delay  = clock_time() | 1;
}

/**
 * @brief   set FLAG_GOOGLE_SEARCH bit for send search key(on-request)
 * @param   none
 * @return  none
 */
#define PREVENT_TIME    600 //ms
_attribute_data_retention_ u32 prevent_repeat_trigger_on_request_tick = 0;
void google_voice_on_request(){

    //Prevent repeated triggers(2020.12.15)
    printf("google_voice_on_request v0.4\n");
    if(prevent_repeat_trigger_on_request_tick && (!clock_time_exceed(prevent_repeat_trigger_on_request_tick,PREVENT_TIME*1000))) return;
    prevent_repeat_trigger_on_request_tick = clock_time()|1;
    google_voice_ctl |= FLAG_GOOGLE_SEARCH;
}

/**
 * @brief   set FLAG_GOOGLE_DPAD_SELECT bit for send DPAD cmd.(ver 0.4e 3.2.3)
 * @param   none
 * @return  none
 */
void google_voice_dpad_select(){
    //v0.4 should process ok key, v1.0 does not
    if(google_voice_ver == 4){
        google_voice_ctl |= FLAG_GOOGLE_DPAD_SELECT;
    }
}

/**
 * @brief       this function is used to start the voice.
 * @param[in]   start_reason:audio start reason
 * @return      none
 */
u8 app_audio_key_start(u8 start_reason){

    printf("app_audio_key_start , start_reason %d\n",start_reason);

    //atv_char_rx_ccc = 1;// force to 1

    if(!atv_char_rx_ccc) return 1;                        //ATVV_CHAR_AUDIO ccc is disabled
    if(google_voice_ver != 10) return 2;                //google voice version error

    //if(start_reason > used_assistant_model) return 3;    //Unavailable model
    //if(!enable_rsp_gaps_rsp)    return 3;

    u8 sendBuff[20]  = {0};
    switch(start_reason){
        case REASON_PTT:
        {
            printf("audio_start_reason = REASON_PTT\n");
            audio_start_reason = REASON_PTT;
        }
        case REASON_HTT:
        {
            if(start_reason == REASON_HTT){
                audio_start_reason = REASON_HTT;
                printf("audio_start_reason = REASON_HTT\n");
            }
            stream_id++;    //increase stream_id
            if(stream_id > 0x80) stream_id = 1;    //stream_id range 0x01~0x80
            google_voice_codec_used = CODEC_USED_16K;
//            google_voice_codec_used = CODEC_USED_8K;

            //init audio parameter
            sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
            sendBuff[1] = audio_start_reason;
            sendBuff[2] = U16_LO(google_voice_codec_used);
            sendBuff[3] = stream_id;

            bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 4);

            google_mic_enable();

            break;
        }
        case REASON_MICOPEN:
        {
            printf("audio_start_reason = REASON_MICOPEN\n");
            google_voice_on_request();
            active_mic_open();
            break;
        }
        default:
        {
            return 4;    //not supported
            break;
        }
    }
    return 0;
}

/**
 * @brief       this function is used to stop the voice.
 * @param[in]   reason:Audio stop reason
 * @return      none
 */
u8 app_audio_key_stop(u8 reason){
    ui_enable_mic(0);
    app_active_remote_timer = 0;
    app_audio_transfer_timer = 0;

    audio_stop_reason = reason;
    google_voice_ctl |= FLAG_AUDIO_CLOSE;

    return 0;
}

/**
 * @brief   this function is used to stop the voice by HTT.
 * @param   none
 * @return  0: ready to stop voice  1:voice start reason is not HTT
 */
u8 app_audio_key_stop_by_htt(){
    if(audio_start_reason != REASON_HTT) return 1;

    app_audio_key_stop(REASON_RELEASE_HTT);

    return 0;
}

/**
 * @brief   according to the version of google voice, start the corresponding process
 * @param   none
 * @return  none
 */
void google_voice_start(){

    if(google_voice_ver == 10){
        app_audio_key_start(read_assistant_model());

    }else{
        google_voice_on_request();
    }

}

/**
 * @brief   this function is used to stop the voice.
 * @param   none
 * @return  none
 */
void google_voice_stop(){
    if(audio_start_reason == REASON_HTT)
        app_audio_key_stop_by_htt();
}

/**
 * @brief       google voice 0.4 processing function.
 * @param[in]   pw:received RF data
 * @return      none
 */
void google_voice_v0p4(rf_packet_att_data_t *pw){
    u8 sendBuff[20]  = {0};

    u8 cmd = pw->dat[0];
    if(cmd == AUDIO_GOOGLE_CMD_OPEN)
    {
        //v0.4e proc

        u8 len = pw->l2cap-3;
        u16 ATV_codec_used = 2;
        printf("received pkt len:%d\n",len);

        array_printf(pw->dat,len);

        if(len == 3){
            ATV_codec_used = (pw->dat[1]<<8) + pw->dat[2];
        }else{
            ATV_codec_used = 2;
        }


        printf("received v0.4 mic open:%d\n",ATV_codec_used);
        switch(ATV_codec_used){
            case CODEC_USED_8K:
            {
                printf("ADPCM, 8khz/16bit\n");

                google_voice_codec_used = CODEC_USED_8K; //8k
                google_voice_packet_length = VOICE_V0P4_ADPCM_PACKET_LEN ; // 2 is just for 4*n
                google_voice_pcm_sample_packet = VOICE_V0P4_ADPCM_UNIT_SIZE*2;  //512.

                app_audio_send_index = 0;

                sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
                bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 1);

                google_mic_enable();

                break;
            }
            case CODEC_USED_16K:
            {
                printf("ADPCM, 16khz/16bit\n");

                google_voice_codec_used = CODEC_USED_16K;
                google_voice_packet_length = VOICE_V0P4_ADPCM_PACKET_LEN ; // 2 is just for 4*n
                google_voice_pcm_sample_packet = VOICE_V0P4_ADPCM_UNIT_SIZE;  //256

                app_audio_send_index = 0;

                sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
                bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 1);

                google_mic_enable();

                break;
            }
            case CODEC_USED_OPUS:
                //printf("Opus(future),current sdk do Not support\n");
            default:
            {
                printf("only support 8k voice\n");
                printf("default ADPCM, 8khz/16bit\n");

                google_voice_codec_used = CODEC_USED_8K; //8k
                google_voice_packet_length = VOICE_V0P4_ADPCM_PACKET_LEN ; // 2 is just for 4*n
                google_voice_pcm_sample_packet = VOICE_V0P4_ADPCM_UNIT_SIZE*2;  //512.


                app_audio_send_index = 0;

                sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
                bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 1);

                google_mic_enable();
                //err parameter
                //printf("default\n");
//                sendBuff[0] = ATV_MIC_CHAR_CTL_MIC_OPEN_ERROR;
//                sendBuff[1] = U16_HI(ERROR_INVALIED_CODEC);
//                sendBuff[2] = U16_LO(ERROR_INVALIED_CODEC);
//                bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 3);
                break;
            }
        }
        printf("google_voice_pcm_sample_packet:%d\n",google_voice_pcm_sample_packet);
        printf("google_voice_packet_length:%d\n",google_voice_packet_length);
    }else if(cmd == AUDIO_GOOGLE_CMD_CLOSE){
        printf("AUDIO_GOOGLE_CMD_CLOSE\n");
        //u8 close_stream_id = pw->dat[1];

        ui_enable_mic(0);

        //send audio close cmd
        google_voice_ctl |= FLAG_AUDIO_CLOSE;
    }
}

/**
 * @brief       verify stream id.
 * @param[in]   received stream id from host
 * @return      0: OK  1: audio_start_reason is not REASON_MICOPEN
 *              2: stream id out of range  3: the received id is inconsistent with the current id
 */
u8 verify_stream_id(u8 id){

    switch(id){
        case 0x00:        //  Check if start reason is on-request.
        {
            if(audio_start_reason != REASON_MICOPEN)    return 1;
            break;
        }
        case 0xff:        //any ongoing audio stream.
        {
            break;
        }
        default:        //when start reason is htt or ptt,check the stream_id.
        {
            if(id > 0x80)    return 2;    //audio stream id range: 0x01 ~ 0x80
            if(id != stream_id) return 3;
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
void google_voice_v1p0(rf_packet_att_data_t *pw){
    if(pw->dat[0] == AUDIO_GOOGLE_CMD_OPEN){
        printf("AUDIO_GOOGLE_CMD_OPEN\n");
        //check ccc
        if(!atv_char_rx_ccc){
            printf("atv_char_rx_ccc is disabled\n");

            //notifications are disabled while audio data transfer is in progress
            google_voice_ctl |= FLAG_GOOGLE_OPEN_ERROR;
            mic_open_error_code = ERROR_CCC_NOT_ENABLED;
            return;
        }

        if(!flag_active_mic_open){
            printf("mic is not activated\n");
        }

        u8 mic_mode = pw->dat[1];
        if(google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA){    //ongoing audio stream
            printf("ongoing audio stream_audio_start_reason:%d\n",audio_start_reason);
            switch(audio_start_reason){
                case REASON_PTT:
                case REASON_HTT:
                {
                    //send open error.microphone is open because of ongoing PTT/HTT interaction
                    mic_open_error_code = ERROR_ONGOING_PTT_HTT;
                    google_voice_ctl |= FLAG_GOOGLE_OPEN_ERROR;
                    break;
                }
                case REASON_MICOPEN:
                default:
                {
                    //send audio stop and restart audio or ignore cmd
                    //Prevent repeated triggers(2020.12.15)
                    ui_enable_mic(0);
                    google_voice_ctl |= FLAG_AUDIO_CLOSE;
                    audio_stop_reason = REASON_UPCOMING_AUDIO_START;
                    audio_start_reason = REASON_MICOPEN;
                    break;
                }
            }
        }
        else{
            printf("new audio. mic mode:0x%x \n",mic_mode);
            //start audio
            switch(mic_mode){
                case PLAYBACK_MODE:
                {
                    mic_open_mode = PLAYBACK_MODE;
                    break;
                }
                case CAPTURE_MODE:
                default:
                {
                    mic_open_mode = CAPTURE_MODE;
                    break;
                }
            }

            //SEND AUDIO START
            audio_start_reason = REASON_MICOPEN;
            google_voice_codec_used = CODEC_USED_16K;

            u8 sendBuff[4] = {0};
            //init audio parameter
            sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
            sendBuff[1] = audio_start_reason;
            sendBuff[2] = U16_LO(google_voice_codec_used);
            sendBuff[3] = 0x00;        //an audio stream which was initiated by the MIC_OPEN command

            printf("sendBuff:");
            array_printf(sendBuff,4);
            bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 4);

            google_mic_enable();
        }
    }else if(pw->dat[0] == AUDIO_GOOGLE_CMD_CLOSE){
        printf("AUDIO_GOOGLE_CMD_CLOSE\n");
        u8 close_stream_id = pw->dat[1];

        if(verify_stream_id(close_stream_id)) return;

        //close ongoing audio stream
        ui_enable_mic(0);
        google_voice_ctl |= FLAG_AUDIO_CLOSE;
        audio_stop_reason = REASON_MICCLOSE;

    }else if(pw->dat[0] == AUDIO_GOOGLE_CMD_EXTEND){
        printf("AUDIO_GOOGLE_CMD_EXTEND\n");
        u8 close_stream_id = pw->dat[1];

        if(verify_stream_id(close_stream_id)) return;

        //fresh the timeout
        app_audio_transfer_timer = clock_time() | 1;
    }
}

/**
 * @brief       google voice processing function.
 * @param[in]   p: received RF data
 * @return      none
 */
int app_auido_google_callback(void* p){
    //u8 sendBuff[20]  = {0};
    rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;

    u8 google_command = pw->dat[0];

    printf("google_command:%x\n",google_command);

    if(google_command == AUDIO_GOOGLE_CMD_CAP){
        u16 voice_ver = (pw->dat[1]<<8) + pw->dat[2];
        printf("STB google voice ver: 0x%x\n",voice_ver);
        if(ENABLE_GOOGLE_VOICE_1P0 && voice_ver == GOOGLE_VOICE_VERSION_1P0){
            printf("GOOGLE_VOICE_VERSION_1P0\n");
            array_printf(pw->dat,pw->l2cap-3);

            google_voice_ver = 10;
            //google_voice_para_init();
            //u8 supported_assistant_models = pw->dat[5];
            printf("supported_assistant_models:0x%x\n",pw->dat[5]);


            set_assistant_model(pw->dat[5]);
        }
        else{
            printf("GOOGLE_VOICE_VERSION_0P4\n");

            google_voice_ver = 0x04;
        }
        get_cap_received = true;
        google_voice_ctl |=  FLAG_GOOGLE_CAPS_RESP;
        google_voice_para_init();
    }else{
        if(google_voice_ver == 10){
            google_voice_v1p0(pw);
        }else{
            google_voice_v0p4(pw);
        }
    }

    return 0;
}

/**
 * @brief   google command process for google voice v0.4.
 * @param   none
 * @return  none
 */
void google_cmd_v0p4_proc(){

    if(google_voice_ctl & FLAG_GOOGLE_DPAD_SELECT){
        //audio stop. google voice auido end
        ble_sts_t ret;
        u8 sendData = ATV_MIC_CHAR_CTL_DPAD_SELECT;
        ret = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, &sendData, 1);
//        if(ret != BLE_SUCCESS) return;
//        google_voice_ctl &= ~FLAG_GOOGLE_DPAD_SELECT;
        if(ret == BLE_SUCCESS)
            google_voice_ctl &= ~FLAG_GOOGLE_DPAD_SELECT;
    }

    if(google_voice_ctl & FLAG_AUDIO_CLOSE){
        //audio stop. google voice auido end
        printf("google_cmd_v0p4_proc_close\r\n");
        ble_sts_t ret;
        u8 sendData = ATV_MIC_CHAR_CTL_AUDIO_STOP;
        ret = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, &sendData, 1);

        if(ret == BLE_SUCCESS){
            google_voice_ctl &= ~FLAG_AUDIO_CLOSE;
            google_voice_ctl = 0;
            ui_enable_mic(0);
        }
    }

    if(google_voice_ctl & FLAG_GOOGLE_AUDIO_SYNC){
        //audio sync

        int *p = mic_encoder_data_buffer ();
        u16 frame_no;
        if(p){
            frame_no = (p[0] <<8) + p[1];
        }else{
            frame_no = adpcm_sequence_num;
        }

        ble_sts_t ret;
        u8 sendBuff[3] = {0};
        sendBuff[0] = 0x0A;
        sendBuff[1] = U16_HI(frame_no);
        sendBuff[2] = U16_LO(frame_no);
        ret = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 3);

        if(ret == BLE_SUCCESS)
            google_voice_ctl &= ~FLAG_GOOGLE_AUDIO_SYNC;

    }

    if(google_voice_ctl & FLAG_GOOGLE_OPEN_ERROR){
        //mic open error.
        //Timeout1.This is a timeout on the Android TV device.
        ble_sts_t ret;
        u8 sendBuff[3] = {0};
        sendBuff[0] = ATV_MIC_CHAR_CTL_MIC_OPEN_ERROR;
        sendBuff[1] = U16_HI(ERROR_INVALIED_CODEC);
        sendBuff[2] = U16_LO(ERROR_INVALIED_CODEC);

        ret  = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 3);
        if(ret== BLE_SUCCESS){
            google_voice_ctl &= ~ATV_MIC_CHAR_CTL_MIC_OPEN_ERROR;
            google_voice_ctl = 0;
            ui_enable_mic(0);
        }
    }
}

/**
 * @brief   google command process for google voice v1.0.
 * @param   none
 * @return  none
 */
void google_cmd_v1p0_proc(){

    if(!atv_char_rx_ccc && (google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA)){
        //notifications are disabled while audio data transfer is in progress
        google_voice_ctl |= FLAG_AUDIO_CLOSE;
        audio_stop_reason = REASON_DISABLE_CCC;
    }

    // v1.0 should also process FLAG_GOOGLE_DPAD_SELECT
    if(google_voice_ctl & FLAG_GOOGLE_DPAD_SELECT){
        //audio stop. google voice auido end
        ble_sts_t ret;
        u8 sendData = ATV_MIC_CHAR_CTL_DPAD_SELECT;
        ret = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, &sendData, 1);
        if(ret == BLE_SUCCESS)
            google_voice_ctl &= ~FLAG_GOOGLE_DPAD_SELECT;
    }


    //audio stop
    if(google_voice_ctl & FLAG_AUDIO_CLOSE){
        //audio stop. google voice auido end
        //printf("google_cmd_v1p0_proc_close\r\n");
        ble_sts_t ret;
        u8 sendBuff[2] = {0};
        sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_STOP;
        sendBuff[1] = audio_stop_reason;

        ret  = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 2);
        printf("google_cmd_v1p0_proc_close ret=%x\r\n",ret);
        if(ret == BLE_SUCCESS){

            printf("ATV_MIC_CHAR_CTL_AUDIO_STOP:0x%x\n",audio_stop_reason);
            google_voice_ctl &= ~FLAG_AUDIO_CLOSE;
            google_voice_ctl = 0;
            ui_enable_mic(0);

            if(audio_stop_reason == REASON_UPCOMING_AUDIO_START){
                google_voice_ctl |= FLAG_GOOGLE_AUDIO_START;
            }
        }
    }

    //audio start
    if(google_voice_ctl & FLAG_GOOGLE_AUDIO_START){
        //audio start. triggered by an upcoming AUDIO_START command;

        google_voice_codec_used = CODEC_USED_16K;

        ble_sts_t ret;
        u8 sendBuff[4] = {0};
        sendBuff[0] = ATV_MIC_CHAR_CTL_AUDIO_START;
        sendBuff[1] = audio_start_reason;
        sendBuff[2] = U16_LO(google_voice_codec_used);
        sendBuff[3] = 0x00;        //an audio stream which was initiated by the MIC_OPEN command

        ret  = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 4);

        if(ret == BLE_SUCCESS){
            google_voice_ctl &= ~FLAG_GOOGLE_AUDIO_START;
            //start send audio data
            google_mic_enable();
        }
    }

    //mic open error
    if(google_voice_ctl & FLAG_GOOGLE_OPEN_ERROR){
        //mic open error.
        //Timeout1.This is a timeout on the Android TV device.
        printf("google_voice_ctl & FLAG_GOOGLE_OPEN_ERROR:%x",mic_open_error_code);

        ble_sts_t ret;
        u8 sendBuff[3] = {0};
        sendBuff[0] = ATV_MIC_CHAR_CTL_MIC_OPEN_ERROR;
        sendBuff[1] = U16_HI(mic_open_error_code);
        sendBuff[2] = U16_LO(mic_open_error_code);

        ret  = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 3);
        if(ret == BLE_SUCCESS){
            google_voice_ctl &= ~ATV_MIC_CHAR_CTL_MIC_OPEN_ERROR;
            google_voice_ctl = 0;
            ui_enable_mic(0);
        }
    }
}

/**
 * @brief   notify CAPS_RESP packet.
 * @param   none
 * @return  none
 */
void google_get_rsp(void){

    u8 sendBuff[9] = {0};
    if(google_voice_ver == 10){

        sendBuff[0] = ATV_MIC_CHAR_CTL_CAPS_RESP;
        sendBuff[1] = U16_HI(GOOGLE_VOICE_VERSION_1P0);     //major version
        sendBuff[2] = U16_LO(GOOGLE_VOICE_VERSION_1P0);     //minor version
        sendBuff[3] = CODEC_SUPPORTED_8K16K;                //codecs supported (1)
        sendBuff[4] = read_assistant_model();               //set assistant interaction model.  on-request,ptt,htt
        sendBuff[5] = 0x00;                                 //audio frame size (2)
        sendBuff[6] = read_audio_frame_size();              //audio frame size (2)
        //sendBuff[7] = (read_audio_frame_size() == 20)?0:1;  //extra configuration (1)
        sendBuff[7] = ((read_audio_frame_size()+3) > app_mtu_size)?1:0;       //extra configuration (1)
        sendBuff[8] = 0x00;                                 //reserved(1)

    }
    else{
        sendBuff[0] = ATV_MIC_CHAR_CTL_CAPS_RESP;
        sendBuff[1] = U16_HI(GOOGLE_VOICE_VERSION_0P4);     //major version
        sendBuff[2] = U16_LO(GOOGLE_VOICE_VERSION_0P4);     //minor version
        sendBuff[3] = 0x00;                                 //codecs_supported high
        sendBuff[4] = CODEC_SUPPORTED_8K;                   //CODEC_SUPPORTED_8K16K;
        sendBuff[5] = 0x00;                                 //frame lengths high
        sendBuff[6] = 0x86;                                 //frame lengths low -- 134Byts
        sendBuff[7] = 0x00;                                 //packet lengths high
        sendBuff[8] = 0x14;                                 //packet lengths low -- 20 Bytes
    }
    ble_sts_t ret;
    ret = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 9);
    //printf("google_get_rsp_ret=%x\r\n",ret);
    if(ret == BLE_SUCCESS){
        if(google_voice_ver == 10)
        {
            printf("read_audio_frame_size():%d\n",sendBuff[6]);
        }
        printf("send FLAG_GOOGLE_CAPS_RESP_ver:%d\n",google_voice_ver);
        google_voice_ctl &= ~FLAG_GOOGLE_CAPS_RESP;
     }
}

_attribute_data_retention_ u8 notify_get_rsp_en = 0;
_attribute_data_retention_ u32 notify_get_rsp_tick = 0;
_attribute_data_retention_ u32 notify_get_rsp_delay_time = 6000000;
/**
 * @brief   google command process at app_audio_task.
 * @param   none
 * @return  none
 */
void google_cmd_proc(){
     ble_sts_t ret;

//    if(app_active_remote_timer && clock_time_exceed(app_active_remote_timer, APP_AUDIO_GOOGLE_TIMEOUT1)){
//        //wait mic open cmd
//        printf("app_active_remote_timer timeout\n");
//        app_active_remote_timer = 0;
//
//        mic_open_error_code = ERROR_RCU_NOT_ACTIVE;    //google voice v1.0
//        google_voice_ctl = 0;
//        google_voice_ctl |= FLAG_GOOGLE_OPEN_ERROR;
//    }

    if(app_audio_transfer_timer && clock_time_exceed(app_audio_transfer_timer, APP_AUDIO_GOOGLE_TIMEOUT2)){
        printf("app_audio_transfer_timer timeout\n");
        ui_enable_mic(0);    //app_audio_disable();

        app_audio_transfer_timer = 0;

        if(google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA){
            audio_stop_reason = REASON_TIMEOUT;
            google_voice_ctl |= FLAG_AUDIO_CLOSE;
        }
    }

    if(google_voice_ctl & FLAG_GOOGLE_SEARCH){
        //google voice CHAR_CTL search
        if (blc_ll_getTxFifoNumber() < (14 - 3)){
            printf("sendData = ATV_MIC_CHAR_CTL_SEARCH\n");
            u8 sendData[2] = {0};

            sendData[0] = ATV_MIC_CHAR_CTL_SEARCH;
            ret = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendData, 1);
            printf("search 1 ret=%x\r\n",ret);

            sendData[0] = 0x21;
            sendData[1] = 0x02;
            ret = bls_att_pushNotifyData(HID_CONSUME_REPORT_INPUT_DP_H, sendData, 2); //  8 HID_NORMAL_KB_REPORT_INPUT_DP_H

            printf("search 2 ret=%x\r\n",ret);

            sendData[0] = 0x00;
            sendData[1] = 0x00;
            ret = bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, sendData, 2);

            printf("search 3 ret=%x\r\n",ret);

            google_voice_ctl &= ~FLAG_GOOGLE_SEARCH;
            extern void app_ota_status(u8 status);
            app_ota_status(0);
        }
    }

    if(google_voice_ctl & FLAG_GOOGLE_CAPS_RESP){

        if(notify_get_rsp_en)
        {
            if(notify_get_rsp_tick && clock_time_exceed(notify_get_rsp_tick,notify_get_rsp_delay_time))
            {
                notify_get_rsp_en = 0;
                notify_get_rsp_tick = 0;
                notify_get_rsp_delay_time = 6000000;
                google_get_rsp();
            }
        }
        else
        {
            google_get_rsp();
        }
    }

    if(google_voice_ver == 10){
        google_cmd_v1p0_proc();
    }else{
        google_cmd_v0p4_proc();
    }
}

/**
 * @brief   delay caps rsp time.
 * @param   none
 * @return  none
 */
void google_get_rsp_delay(void)
{
    if(notify_get_rsp_en)
    {
        notify_get_rsp_tick = clock_time()|1;
        notify_get_rsp_delay_time = 300000;
    }
}

void google_reset_rsp_delay(void)
{
    notify_get_rsp_delay_time = 12000000;
}


/**
 * @brief   notify audio data process for google voice v0.4.
 * @param   none
 * @return  none
 */
void google_voice_data_notify_v0p4_proc(){

    //////////////////////////////////////////////////////////////////
    if(blc_ll_getTxFifoNumber() >= (7 + app_audio_send_index)) return;
    int *p = mic_encoder_data_buffer ();

    if(p == 0) return;
    u8 audio_send_length;

    for(u8 i=0; i<7; i++){
        if(app_audio_send_index < 6){
            audio_send_length = 20;
        }else if(app_audio_send_index == 6){
            audio_send_length = 14;
        }else{
            audio_send_length = 0;
        }

        if(BLE_SUCCESS == bls_att_pushNotifyData(AUDIO_GOOGLE_RX_DP_H, (u8*)p + app_audio_send_index*20, audio_send_length)){
            app_audio_send_index++;
        }else{
            return ;
        }

        if(app_audio_send_index == 7){
            app_audio_send_index = 0;
            mic_encoder_data_read_ok();

            app_audio_sync_serial ++;
            if(app_audio_sync_serial > 10){
                app_audio_sync_serial =0;
                google_voice_ctl |= FLAG_GOOGLE_AUDIO_SYNC;
            }
            return ;
        }
    }
}

u8  previous_sampling_rate = U16_LO(CODEC_USED_16K);    //default sampling rate:16k
u16 previous_seq_no = 0xffff;
/**
 * @brief   notify audio data process for google voice v1.0
 * @param   none
 * @return  none
 */
void google_voice_data_notify_v1p0_proc(){

    int *p = mic_encoder_data_buffer ();

    if(p){
        //printf("google voice 1p0\n");

        u8 *pd = (u8*)p;
        u16 current_seq_no = (pd[0]<<8) + pd[1];
        u8  current_sampling_rate = pd[2];

        u8 need_buffer_count = 0;
        u8 send_sync_cmd_flag = 0;

        //check seq no and sampling rate before notify audio data
        if(current_sampling_rate != previous_sampling_rate){
            send_sync_cmd_flag |= SAMPLING_CHANGE;
            need_buffer_count = 1;
        }

        if((u16)(current_seq_no - previous_seq_no) != 1){
            send_sync_cmd_flag |= PACKET_LOSS;
            need_buffer_count = 1;
        }

        app_audio_sync_serial ++;
        if(app_audio_sync_serial >= 20){
            app_audio_sync_serial = 0;
            send_sync_cmd_flag |= SYNC_PACKET;
            need_buffer_count = 1;
        }

        if(google_voice_ctl & FLAG_GOOGLE_AUDIO_FIRST_SYNC){
            printf("FLAG_GOOGLE_AUDIO_FIRST_SYNC\n");
            google_voice_ctl &= ~FLAG_GOOGLE_AUDIO_FIRST_SYNC;
            //send_sync_cmd_flag |= FIRST_SYNC;
            //need_buffer_count = 1;
            previous_sampling_rate = U16_LO(CODEC_USED_16K);    //default sampling rate:16k
            previous_seq_no = 0xffff;
            app_audio_sync_serial = 0;
            printf("start rate:0x%x \n",current_sampling_rate);
        }

        //adpcm packet: header:6byte, adpcm data:120byte,dummy:2byte
        need_buffer_count += (VOICE_V1P0_ADPCM_PACKET_LEN - 8)/auido_frame_size;


        if(blc_ll_getTxFifoNumber() >= (14 - need_buffer_count + app_audio_send_index)) return;


        //check sync data
        if(send_sync_cmd_flag && !app_audio_send_index){

            u8 sendBuff[7] = {0};
            //send sync cmd
            u32 notify_seq_no = current_seq_no * ((VOICE_V1P0_ADPCM_PACKET_LEN - 8)/auido_frame_size);
            //printf("notify_seq_no:%d",notify_seq_no);

            sendBuff[0] = ATV_MIC_CHAR_CTL_SYNC;
            sendBuff[1] = current_sampling_rate;            //The audio codec that will be used for the audio stream after this sync point.
            sendBuff[2] = (notify_seq_no >> 8) & 0xFF;      //Sequence number of the audio frame
            sendBuff[3] = notify_seq_no & 0xFF;             //Sequence number of the audio frame

            sendBuff[4] = pd[3];                            //Predicted ADPCM value.
            sendBuff[5] = pd[4];                            //Predicted ADPCM value.
            sendBuff[6] = pd[5];                            //step index (1) Index in ADPCM step size table.

            if(send_sync_cmd_flag&FIRST_SYNC){
                printf("Predicted ADPCM value1:0x%x\n",sendBuff[4]);
                printf("Predicted ADPCM value2:0x%x\n",sendBuff[5]);
                printf("step index:0x%x\n",sendBuff[6]);
                printf("need_buffer_count:0x%x\n",need_buffer_count);
                printf("(VOICE_V1P0_ADPCM_PACKET_LEN - 8)/auido_frame_size:%d\n",(VOICE_V1P0_ADPCM_PACKET_LEN - 8)/auido_frame_size);
            }

            ble_sts_t ret = bls_att_pushNotifyData(AUDIO_GOOGLE_CTL_DP_H, sendBuff, 7);
            if(ret == BLE_SUCCESS){

                //printf("send_sync_cmd_flag:%d\n",send_sync_cmd_flag);
                if(send_sync_cmd_flag&SAMPLING_CHANGE) printf("SAMPLING_CHANGE previous_sampling_rate:%d_current:%d\n",previous_sampling_rate,current_sampling_rate);
                if(send_sync_cmd_flag&PACKET_LOSS) printf("PACKET_LOSS previous_seq_no:%d_current:%d\n",previous_seq_no,current_seq_no);
//                array_printf(sendBuff,7);

                app_audio_send_index++;
            }else{
                return ;
            }
        }

        //send audio data
        for(u8 i=0; i< ((VOICE_V1P0_ADPCM_PACKET_LEN - 8)/auido_frame_size); i++){

            u8 audio_addr_shift = app_audio_send_index - (send_sync_cmd_flag==0)?0:1;

//            printf("send_sync_cmd_flag:%d\n",send_sync_cmd_flag);
//            printf("audio_addr_shift:%d\n",audio_addr_shift);
            if(BLE_SUCCESS == bls_att_pushNotifyData(AUDIO_GOOGLE_RX_DP_H, pd + 6 + audio_addr_shift *auido_frame_size, auido_frame_size)){
                app_audio_send_index++;
                //array_printf(pd + 6 + audio_addr_shift *auido_frame_size,20);
                //printf("a");
            }else{
                return;
            }


            if(app_audio_send_index >= need_buffer_count){
//                printf("need_buffer_count:0x%x\n",need_buffer_count);
//                printf("app_audio_send_index:0x%x\n",app_audio_send_index);
                app_audio_send_index = 0;
                //printf("notify data:");
                //array_printf(pd,10);

                mic_encoder_data_read_ok();

                previous_sampling_rate = current_sampling_rate;
                previous_seq_no = current_seq_no;

                return;
            }
        }
    }
}

/**
 * @brief   notify audio data process.
 * @param   none
 * @return  none
 */
void google_voice_data_notify_proc(){
    //send voice data
    if( google_voice_ver == 10){
        google_voice_data_notify_v1p0_proc();
    }else{
        //google voice version error
        google_voice_data_notify_v0p4_proc();
    }
}

/**
 * @brief   google voice task at main loop.
 * @param   none
 * @return  none
 */
void app_audio_task(void){

    if(app_active_remote_timer && clock_time_exceed(app_active_remote_timer, APP_AUDIO_GOOGLE_TIMEOUT1)){
        //wait mic open cmd
        printf("app_active_remote_timer timeout\n");
        app_active_remote_timer = 0;
        flag_active_mic_open = 0;
    }

    if(!google_voice_ctl) return;

    if(device_in_connection_state)
        google_cmd_proc();

    if(google_voice_ctl & FLAG_DELAY_NOTIFY){

        if(clock_time_exceed(app_audio_start_delay,30*1000)){

            google_voice_ctl &= ~FLAG_DELAY_NOTIFY;
            audio_set_mute_pga(1);
            printf("start notify audio data\n");
            if(google_voice_ver == 10){
                if(google_voice_codec_used == CODEC_USED_16K){
                    printf("init adpcm unit size: to 240\n");
                    google_voice_pcm_sample_packet = VOICE_V1P0_ADPCM_UNIT_SIZE;
                }else if(google_voice_codec_used == CODEC_USED_8K){
                    google_voice_pcm_sample_packet = VOICE_V1P0_ADPCM_UNIT_SIZE*2;
                }
            }else{
                if(google_voice_codec_used == CODEC_USED_16K){
                    printf("init adpcm unit size: to 256\n");
                    google_voice_pcm_sample_packet = VOICE_V0P4_ADPCM_UNIT_SIZE;
                }else if(google_voice_codec_used == CODEC_USED_8K){
                    google_voice_pcm_sample_packet = VOICE_V0P4_ADPCM_UNIT_SIZE*2;
                }
            }
        }
        return;
    }

    if((google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA) == 0) return;
    prevent_repeat_trigger_on_request_tick = 0;
    proc_mic_encoder();

    google_voice_data_notify_proc();
}

#else

#endif
