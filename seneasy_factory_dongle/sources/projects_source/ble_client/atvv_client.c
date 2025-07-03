#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utility.h"
#include "app_cfg.h"

#include "gatt_uuid_def.h"
#include "ble_central.h"
#include "adpcm.h"
#include "atvv_client.h"

#define ATVV_CTRL_AUDIO_STOP     (0x00)
#define ATVV_CTRL_AUDIO_START    (0x04)
#define ATVV_CTRL_AUDIO_SEARCH   (0x08)
#define ATVV_CTRL_AUDIO_SYNC     (0x0A)
#define ATVV_CTRL_CAPS_RSP       (0x0B)
#define ATVV_CTRL_MIC_OPEN_ERROR (0x0C)

#define ATVV_VERSION_MAJOR       (0x00)
#define ATVV_VERSION_MINOR       (0x05)
#define ATVV_VERSION             (ATVV_VERSION_MINOR<<8|ATVV_VERSION_MAJOR) //V0.5

#define ATVV_CODEC_SUPPORTED_8K_16BIT       (0x0001)
#define ATVV_CODEC_SUPPORTED_8K16K_16BIT    (0x0003)
#define ATVV_CODEC_SUPPORTED    ATVV_CODEC_SUPPORTED_8K16K_16BIT

#define ATVV_CODEC_USED_ADPCM_8K_16BIT     (0x0001)
#define ATVV_CODEC_USED_ADPCM_16K_16BIT    (0x0002)
#define ATVV_CODEC_USED    (ATVV_CODEC_USED_ADPCM_16K_16BIT)

typedef struct
{
    uint8_t u8CmdData[5];
    uint8_t u8CmdLen;

}st_atvv_client_cmd_t;
static st_atvv_client_cmd_t stAtvvCientCmd;

static uint8_t u8AtvvServiceUuid[]   = {0x64,0xb6,0x17,0xf6,0x01,0xaf,  0x7d,0xbc,  0x05,0x4f,  0x21,0x5a,  0x01,0x00,0x5e,0xab};
static uint8_t u8AtvvCharTxUuid[]    = {0x64,0xb6,0x17,0xf6,0x01,0xaf,  0x7d,0xbc,  0x05,0x4f,  0x21,0x5a,  0x02,0x00,0x5e,0xab};
static uint8_t u8AtvvCharRxUuid[]    = {0x64,0xb6,0x17,0xf6,0x01,0xaf,  0x7d,0xbc,  0x05,0x4f,  0x21,0x5a,  0x03,0x00,0x5e,0xab};
static uint8_t u8AtvvCharCtrlUuid[]  = {0x64,0xb6,0x17,0xf6,0x01,0xaf,  0x7d,0xbc,  0x05,0x4f,  0x21,0x5a,  0x04,0x00,0x5e,0xab};

static uint16_t u16AtvvTxCharHandle = 0;
static uint16_t u16AtvvRxCharHandle = 0;
static uint16_t u16AtvvRxCharDesCfgHandle = 0;
static uint16_t u16AtvvCtrlCharHandle = 0;
static uint16_t u16AtvvCtrlCharDesCfgHandle = 0;

typedef struct 
{
    uint8_t u8Major;
    uint8_t u8Minor;
}st_atvv_version_t;

typedef struct atvv_client
{
    st_atvv_version_t stAtvvVersionService;
    uint16_t u16CodesSupported;
    uint16_t u16FrameSize;
    uint16_t u16CharBytes;
}st_atvv_resp_data_t;

st_atvv_resp_data_t gstAtvvRespData;

#define ATVV_HEADE_LEN      (6)
typedef struct
{
    uint16_t u16Seq;
    uint8_t u8Id;
    uint16_t u16PrevPred;
    uint8_t u8Index;
}st_atvv_heade_t;
static st_atvv_heade_t gstAtvvHeadeLast;
static st_atvv_heade_t gstAtvvHeade;

static uint8_t u8AudioRxData[128];
static int16_t i16AudioData[256];

atvv_client_event_callback_t atvv_client_event_callback;


static void atvv_client_parse_ctrl_char_des_cfg_write_result(uint8_t u8Result)
{
    if(0 == u8Result)
    {
        PRINTF("atvv ctrl char des cfg write ok\n");
        if(atvv_client_event_callback)
        {
            atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_CHAR_EN_NTF_FINISH, NULL, 0);
        }
    }
}

static void atvv_client_parse_rx_char_des_cfg_write_result(uint8_t u8Result)
{
    if(0 == u8Result)
    {
        PRINTF("atvv rx char des cfg write ok\n");
    }
    uint8_t u8Data[2] = {1, 0};
    if(0 != u16AtvvCtrlCharDesCfgHandle)
    {
        if(!ble_central_write_char_value(ble_central_get_conn_handle(), u16AtvvCtrlCharDesCfgHandle, u8Data, sizeof(u8Data), atvv_client_parse_ctrl_char_des_cfg_write_result))
        {
            PRINTF("atvv ctrl char des cfg write fail\n");
        }
    }
}

static void atvv_client_parse_tx_char_write_result(uint8_t u8Result)
{
    if(0 == u8Result)
    {
        PRINTF("atvv client write cmd ok\n");
    }
    else
    {
        PRINTF("atvv client write cmd result %d\n", u8Result);
    }
}

void atvv_client_get_srvice_handle(uint16_t *pu16StartHandle, uint16_t *pu16EndHandle)
{
    ble_central_get_service_handle_by_uuid(1, 0, u8AtvvServiceUuid, pu16StartHandle, pu16EndHandle);
}

void atvv_client_get_char_handle(void)
{
    if(!ble_central_get_char_value_handle_by_uuid(1, 0, u8AtvvCharTxUuid, &u16AtvvTxCharHandle, 0))
    {
        PRINTF("atvv tx char not found\n");
        return;
    }

    if(!ble_central_get_char_value_handle_by_uuid(1, 0, u8AtvvCharRxUuid, &u16AtvvRxCharHandle, 0))
    {
        PRINTF("atvv rx char not found\n");
        return;
    }
    if(!ble_central_get_char_des_by_char_handle(u16AtvvRxCharHandle, GATT_CLIENT_CHAR_CFG_UUID, &u16AtvvRxCharDesCfgHandle))
    {
        PRINTF("atvv rx char des cfg not found\n");
        return;
    }

    if(!ble_central_get_char_value_handle_by_uuid(1, 0, u8AtvvCharCtrlUuid, &u16AtvvCtrlCharHandle, 0))
    {
        PRINTF("atvv ctrl char not found\n");
        return;
    }
    if(!ble_central_get_char_des_by_char_handle(u16AtvvCtrlCharHandle, GATT_CLIENT_CHAR_CFG_UUID, &u16AtvvCtrlCharDesCfgHandle))
    {
        PRINTF("atvv ctrl char des cfg not found\n");
        return;
    }
    if(atvv_client_event_callback)
    {
        atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_CHAR_FIND_FINISH, NULL, 0);
    }
}

void atvv_client_enable_notify(void)
{
    uint8_t u8Data[2] = {1, 0};
    if(u16AtvvRxCharDesCfgHandle)
    {
        if(!ble_central_write_char_value(ble_central_get_conn_handle(), u16AtvvRxCharDesCfgHandle, u8Data, sizeof(u8Data), atvv_client_parse_rx_char_des_cfg_write_result))
        {
            PRINTF("atvv rx char des cfg write fail\n");
        }
    }
}

void atvv_client_process_notify_data(uint8_t *pu8Data, uint16_t u16DataLen)
{
    // PRINTF("u16DataLen %d\n", u16DataLen);
    uint16_t u16CharValueHandle = rom_little_endian_read_16(pu8Data, 0);
    u16DataLen -= 2;
    uint16_t u16DataStartIndex = 2;
    static bool bIsFirstPack = true;
    static uint16_t u16AudioDataIdx = 0;
    static stCodecState_t stCodecState;
    if(u16CharValueHandle == u16AtvvRxCharHandle)
    {
        PRINTF("S=%d\n",  (pu8Data[2] << 8) | pu8Data[3]);
        if(atvv_client_event_callback)
        {
            atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_AUDIO_DATA, pu8Data+2, u16DataLen);
        }
//        if(u16DataLen == gstAtvvRespData.u16FrameSize) // long pack
//        {
//            gstAtvvHeade.u16Seq = (pu8Data[2] << 8) | pu8Data[3];
//            gstAtvvHeade.u8Id = pu8Data[4];
//            gstAtvvHeade.u16PrevPred = (pu8Data[5] << 8) | pu8Data[6];
//            gstAtvvHeade.u8Index = pu8Data[7];
//            if(gstAtvvHeade.u16Seq != 0)
//            {
//                if(gstAtvvHeade.u16Seq != gstAtvvHeadeLast.u16Seq + 1)
//                {
//                    PRINTF("audio pack lost %d\n", gstAtvvHeadeLast.u16Seq + 1);
//                    PRINTF("current seq %d\n", gstAtvvHeade.u16Seq);
//                }
//            }
//            PRINTF("seq%d\n", gstAtvvHeade.u16Seq);
//            gstAtvvHeadeLast.u16Seq = gstAtvvHeade.u16Seq;
//            u16DataStartIndex += ATVV_HEADE_LEN;
//            adpcm_decode(&stCodecState, pu8Data+u16DataStartIndex, (u16DataLen-ATVV_HEADE_LEN)<<1, i16AudioData);
//            if(atvv_client_event_callback)
//            {
//                atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_AUDIO_DATA, (uint8_t *)i16AudioData, (u16DataLen-ATVV_HEADE_LEN)<<1);
//            }
//        }
//        else if(u16DataLen <= 20) // short pack
//        {
//            if(bIsFirstPack)
//            {
//                bIsFirstPack = false;
//                gstAtvvHeade.u16Seq = (pu8Data[2] << 8) | pu8Data[3];
//                gstAtvvHeade.u8Id = pu8Data[4];
//                gstAtvvHeade.u16PrevPred = (pu8Data[5] << 8) | pu8Data[6];
//                gstAtvvHeade.u8Index = pu8Data[7];
//                if(gstAtvvHeade.u16Seq != 0)
//                {
//                    if(gstAtvvHeade.u16Seq != gstAtvvHeadeLast.u16Seq + 1)
//                    {
//                        PRINTF("audio pack lost %d\n", gstAtvvHeadeLast.u16Seq + 1);
//                        PRINTF("current seq %d\n", gstAtvvHeade.u16Seq);
//                    }
//                }
//                PRINTF("seq%d\n", gstAtvvHeade.u16Seq);
//                gstAtvvHeadeLast.u16Seq = gstAtvvHeade.u16Seq;
//                u16DataStartIndex += ATVV_HEADE_LEN;
//                memcpy(u8AudioRxData, pu8Data+u16DataStartIndex, u16DataLen-ATVV_HEADE_LEN);
//                u16AudioDataIdx = u16DataLen-ATVV_HEADE_LEN;
//            }
//            else
//            {
//                memcpy(u8AudioRxData+u16AudioDataIdx, pu8Data, u16DataLen);
//                u16AudioDataIdx += u16DataLen;
//                if(u16AudioDataIdx == gstAtvvRespData.u16FrameSize - ATVV_HEADE_LEN)
//                {
//                    adpcm_decode(&stCodecState, u8AudioRxData, (gstAtvvRespData.u16FrameSize - ATVV_HEADE_LEN)<<1, i16AudioData);
//                    if(atvv_client_event_callback)
//                    {
//                        atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_AUDIO_DATA, (uint8_t *)i16AudioData, (gstAtvvRespData.u16FrameSize - ATVV_HEADE_LEN)<<1);
//                    }
//                    bIsFirstPack = true;
//                }
//            }
//        }
    }
    else if(u16CharValueHandle == u16AtvvCtrlCharHandle)
    {
        uint8_t u8Cmd = pu8Data[2];
        switch(u8Cmd)
        {
            case ATVV_CTRL_AUDIO_STOP:
            {
                PRINTF("ATVV_CTRL_AUDIO_STOP\n");
                break;
            }
            case ATVV_CTRL_AUDIO_START:
            {
                atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_AUDIO_START, NULL, 0);
                stCodecState.index = 0;
                stCodecState.valprev = 0;
                PRINTF("ATVV_CTRL_AUDIO_START\n");
                break;
            }
            case ATVV_CTRL_AUDIO_SEARCH:
            {
                atvv_client_send_cmd(ATVV_CMD_MIC_OPEN);
                PRINTF("ATVV_CTRL_AUDIO_SEARCH\n");
                break;
            }
            case ATVV_CTRL_AUDIO_SYNC:
            {
                PRINTF("ATVV_CTRL_AUDIO_SYNC\n");
                break;
            }
            case ATVV_CTRL_CAPS_RSP:
            {
                PRINTF("ATVV_CTRL_CAPS_RSP\n");
                gstAtvvRespData.stAtvvVersionService.u8Major = pu8Data[3];
                gstAtvvRespData.stAtvvVersionService.u8Minor = pu8Data[4];
                gstAtvvRespData.u16CodesSupported =  pu8Data[5] << 8 | pu8Data[6];
                gstAtvvRespData.u16FrameSize = pu8Data[7] << 8 | pu8Data[8];
                gstAtvvRespData.u16CharBytes = pu8Data[9] << 8 | pu8Data[10];
                PRINTF("AtvvVersionService %d.%d\n", gstAtvvRespData.stAtvvVersionService.u8Major, gstAtvvRespData.stAtvvVersionService.u8Minor);
                PRINTF("CodesSupported %04X\n", gstAtvvRespData.u16CodesSupported);
                PRINTF("FrameSize %d\n", gstAtvvRespData.u16FrameSize);
                PRINTF("CharBytes %d\n", gstAtvvRespData.u16CharBytes);
                if(atvv_client_event_callback)
                {
                    atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_CAPS_RSP, pu8Data, u16DataLen+2);
                }
                break;
            }
            case ATVV_CTRL_MIC_OPEN_ERROR:
            {
                PRINTF("ATVV_CTRL_MIC_OPEN_ERROR\n");
                break;
            }
            default: break;
        }
    }
}

bool atvv_client_send_cmd(uint8_t u8Cmd)
{
    stAtvvCientCmd.u8CmdLen = 0;
    switch(u8Cmd)
    {
        case ATVV_CMD_GET_CAPS:
        {
            stAtvvCientCmd.u8CmdData[0] = ATVV_CMD_GET_CAPS;
            stAtvvCientCmd.u8CmdData[1] = ATVV_VERSION_MAJOR;
            stAtvvCientCmd.u8CmdData[2] = ATVV_VERSION_MINOR;
            stAtvvCientCmd.u8CmdData[3] = (uint8_t)((ATVV_CODEC_SUPPORTED >> 8) & 0xFF);
            stAtvvCientCmd.u8CmdData[4] = (uint8_t)(ATVV_CODEC_SUPPORTED & 0xFF);
            stAtvvCientCmd.u8CmdLen = 5;
            break;
        }
        case ATVV_CMD_MIC_OPEN:
        {
            stAtvvCientCmd.u8CmdData[0] = ATVV_CMD_MIC_OPEN;
            stAtvvCientCmd.u8CmdData[1] = (uint8_t)((ATVV_CODEC_USED >> 8) & 0xFF);
            stAtvvCientCmd.u8CmdData[2] = (uint8_t)(ATVV_CODEC_USED & 0xFF);
            stAtvvCientCmd.u8CmdLen = 3;
            break;
        }
        case ATVV_CMD_MIC_CLOSE:
        {
            stAtvvCientCmd.u8CmdData[0] = ATVV_CMD_MIC_CLOSE;
            stAtvvCientCmd.u8CmdLen = 1;
            break;
        }
        case ATVV_CMD_SEARCH_END:
        {
            stAtvvCientCmd.u8CmdData[0] = ATVV_CMD_SEARCH_END;
            stAtvvCientCmd.u8CmdLen = 1;
            break;
        }
        default : break;
    }
    if(0 != stAtvvCientCmd.u8CmdLen)
    {
        if(!ble_central_write_char_value(ble_central_get_conn_handle(), u16AtvvTxCharHandle, stAtvvCientCmd.u8CmdData, stAtvvCientCmd.u8CmdLen, atvv_client_parse_tx_char_write_result))
        {
            PRINTF("atvv client send cmd fail\n");
        }
    }
    return true;
}

void atvv_client_event_callback_regiest(atvv_client_event_callback_t cb)
{
    if(cb)
    {
        atvv_client_event_callback = cb;
    }
}