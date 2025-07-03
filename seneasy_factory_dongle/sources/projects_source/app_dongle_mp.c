#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "RegMXD2670.h"
#include "err_def.h"

#include "app_cfg.h"
#include "utility.h"

#include "msg_queue.h"

#include "ipc.h"
#include "hid_client.h"
#include "atvv_client.h"

#include "hw_usb_dcd.h"
#include "usb_device.h"
#include "adpcm.h"

#include "app_dongle_mp.h"

#define ATVV_HEADE_LEN (6)

#define MSG_QUEUE_SIZE (256)
#define AUDIO_PACK_NUM (8)
#define QUEUE_BUFFER_SIZE (MSG_QUEUE_SIZE * AUDIO_PACK_NUM)

#define USB_QUEUE_SIZE (68)
#define USB_PACK_NUM (8)
#define RECV_USB_QUEUE_BUFFER_SIZE (USB_QUEUE_SIZE * USB_PACK_NUM)

typedef struct
{
    uint16_t u16Seq;
    uint8_t u8Id;
    uint16_t u16PrevPred;
    uint8_t u8Index;
} st_atvv_heade_t;

typedef struct
{
    uint8_t u8Major;
    uint8_t u8Minor;
} st_atvv_version_t;

typedef struct atvv_client
{
    st_atvv_version_t stAtvvVersionService;
    uint16_t u16CodesSupported;
    uint16_t u16FrameSize;
    uint16_t u16CharBytes;
} st_atvv_resp_data_t;

static st_atvv_heade_t gstAtvvHeadeLast;
static st_atvv_heade_t gstAtvvHeade;
static uint8_t u8AudioRxData[128];
static int16_t i16AudioData[256];
st_atvv_resp_data_t gstAtvvRespData;

static stCodecState_t stCodecState;

pMsgQueueId_t MsgQueueId;
stQueueDef_t stQueueDef;

uint8_t u8QueueBuffer[QUEUE_BUFFER_SIZE];
uint8_t u8MsgBufferTemp[MSG_QUEUE_SIZE];

pMsgQueueId_t recvUsbMsgQueueId;
stQueueDef_t recvUsbStQueueDef;
uint8_t recvUsbQueueBuffer[RECV_USB_QUEUE_BUFFER_SIZE];
uint8_t recvUsbMsgBufferTemp[USB_QUEUE_SIZE];

static void ipc_data_process(uint8_t *pu8Data, uint16_t u16DataLen);

static void atvv_decode(uint8_t *pu8Data, uint16_t u16DataLen, uint8_t *pu8DstData, uint32_t *pu32DstDataLen)
{
    uint16_t u16DataStartIndex = 2;
    static bool bIsFirstPack = true;
    static uint16_t u16AudioDataIdx = 0;

    u16DataLen -= 2;
    if (u16DataLen <= 20) // short pack
    {
        if (bIsFirstPack)
        {
            bIsFirstPack = false;
            gstAtvvHeade.u16Seq = (pu8Data[2] << 8) | pu8Data[3];
            gstAtvvHeade.u8Id = pu8Data[4];
            gstAtvvHeade.u16PrevPred = (pu8Data[5] << 8) | pu8Data[6];
            gstAtvvHeade.u8Index = pu8Data[7];
            if (gstAtvvHeade.u16Seq != 0)
            {
                if (gstAtvvHeade.u16Seq != gstAtvvHeadeLast.u16Seq + 1)
                {
                    PRINTF("audio pack lost %d\n", gstAtvvHeadeLast.u16Seq + 1);
                    PRINTF("current seq %d\n", gstAtvvHeade.u16Seq);
                }
            }
            PRINTF("S:%d\n", gstAtvvHeade.u16Seq);
            gstAtvvHeadeLast.u16Seq = gstAtvvHeade.u16Seq;
            u16DataStartIndex += ATVV_HEADE_LEN;
            memcpy(u8AudioRxData, pu8Data + u16DataStartIndex, u16DataLen - ATVV_HEADE_LEN);
            u16AudioDataIdx = u16DataLen - ATVV_HEADE_LEN;
        }
        else
        {
            memcpy(u8AudioRxData + u16AudioDataIdx, pu8Data, u16DataLen);
            u16AudioDataIdx += u16DataLen;
            if (u16AudioDataIdx == gstAtvvRespData.u16FrameSize - ATVV_HEADE_LEN)
            {
                adpcm_decode(&stCodecState, u8AudioRxData, (gstAtvvRespData.u16FrameSize - ATVV_HEADE_LEN) << 1, i16AudioData);
                //                if(atvv_client_event_callback)
                //                {
                //                    atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_AUDIO_DATA, (uint8_t *)i16AudioData, (gstAtvvRespData.u16FrameSize - ATVV_HEADE_LEN)<<1);
                //                }
                pu8DstData = (uint8_t *)i16AudioData;
                *pu32DstDataLen = (gstAtvvRespData.u16FrameSize - ATVV_HEADE_LEN) << 2;
                bIsFirstPack = true;
            }
        }
    }
    else
    {
        gstAtvvHeade.u16Seq = (pu8Data[2] << 8) | pu8Data[3];
        gstAtvvHeade.u8Id = pu8Data[4];
        gstAtvvHeade.u16PrevPred = (pu8Data[5] << 8) | pu8Data[6];
        gstAtvvHeade.u8Index = pu8Data[7];
        if (gstAtvvHeade.u16Seq != 0)
        {
            if (gstAtvvHeade.u16Seq != gstAtvvHeadeLast.u16Seq + 1)
            {
                PRINTF("audio pack lost %d\n", gstAtvvHeadeLast.u16Seq + 1);
                PRINTF("current seq %d\n", gstAtvvHeade.u16Seq);
            }
        }
        PRINTF("S=%d\n", gstAtvvHeade.u16Seq);
        gstAtvvHeadeLast.u16Seq = gstAtvvHeade.u16Seq;
        u16DataStartIndex += ATVV_HEADE_LEN;
        adpcm_decode(&stCodecState, pu8Data + u16DataStartIndex, (u16DataLen - ATVV_HEADE_LEN) << 1, i16AudioData);
        //        if(atvv_client_event_callback)
        //        {
        //            atvv_client_event_callback(EN_ATVV_CLIENT_EVENT_AUDIO_DATA, (uint8_t *)i16AudioData, (u16DataLen-ATVV_HEADE_LEN)<<1);
        //        }
        pu8DstData = (uint8_t *)i16AudioData;
        *pu32DstDataLen = (u16DataLen - ATVV_HEADE_LEN) << 2;
    }
}

static void ipc_receive_callback(uint8_t *pu8Data, uint16_t u16DataLen)
{
    EN_QUEUE_ERR_STA_T enRet;
    if (msg_queue_is_full(MsgQueueId))
    {
        PRINTF("queue is full\n");
        return;
    }

    memcpy(u8MsgBufferTemp, &u16DataLen, sizeof(uint16_t));
    memcpy(&u8MsgBufferTemp[sizeof(uint16_t)], pu8Data, u16DataLen);
    enRet = msg_queue_push(MsgQueueId, u8MsgBufferTemp);
    if (enRet != QUEUE_STA_OK)
    {
        PRINTF("put queue fail\n");
    }
}

/**
 * @brief
 *
 * @param pu8Data
 * @param u8DataLen
 */
static void usb_cdc_receive_callback(uint8_t *pu8Data, uint8_t u8DataLen)
{
    EN_QUEUE_ERR_STA_T enRet;
    if (msg_queue_is_full(recvUsbMsgQueueId))
    {
        // PRINTF("queue is full\n");
        return;
    }
    uint8_t recvUsbMsgBufferTemp[USB_QUEUE_SIZE];

    recvUsbMsgBufferTemp[0] = u8DataLen;
    memcpy(&recvUsbMsgBufferTemp[1], pu8Data, u8DataLen);
    enRet = msg_queue_push(recvUsbMsgQueueId, recvUsbMsgBufferTemp);
    if (enRet != QUEUE_STA_OK)
    {
        // PRINTF("put queue fail\n");
    }
}

static void ipc_data_process(uint8_t *pu8Data, uint16_t u16DataLen)
{
    uint8_t u8Module = pu8Data[0];
    uint8_t u8Event = pu8Data[1];
    uint8_t *pu8RealData = &pu8Data[2];
    switch (u8Module)
    {
#if USB_HID_EN
    case EN_MSG_MODULE_HID_CLIENT:
    {
        switch (u8Event)
        {
        case EN_HID_CLIENT_EVENT_KEY_CHAR_DATA:
        {
            usb_device_hid_keyboard_data_report(pu8Data + 2, u16DataLen - 2);
            break;
        }
        case EN_HID_CLIENT_EVENT_CONSUMER_CHAR_DATA:
        {
            usb_device_hid_consumer_data_report(pu8Data + 2, u16DataLen - 2);
            break;
        }
        default:
            break;
        }
        break;
    }
#endif
    case EN_MSG_MODULE_ATVV_CLIENT:
    {
        switch (u8Event)
        {
        case EN_ATVV_CLIENT_EVENT_AUDIO_DATA:
        {
            // PRINTF("atvv %d\n", u16DataLen);
            EN_USB_DEVICE_ERR_CODE enRet;
            // decode
            uint8_t *pu8DstData;
            uint32_t u32DstDataLen = 0;

            atvv_decode(pu8Data, u16DataLen, pu8DstData, &u32DstDataLen);
            // PRINTF("DstDataLen %d\n", u32DstDataLen);
            if (u32DstDataLen != 0)
            {
                __disable_irq();
                enRet = usb_device_send_mic_data((uint8_t *)i16AudioData, u32DstDataLen);
                __enable_irq();
                if (enRet != EN_DEVICE_NONE_ERR)
                {
                    PRINTF("USB MIC Send Fail!!!\n");
                }
            }

            break;
        }
        case EN_ATVV_CLIENT_EVENT_CAPS_RSP:
        {
            PRINTF("ATVV_CTRL_CAPS_RSP %d\n", u16DataLen);
            gstAtvvRespData.stAtvvVersionService.u8Major = pu8RealData[3];
            gstAtvvRespData.stAtvvVersionService.u8Minor = pu8RealData[4];
            gstAtvvRespData.u16CodesSupported = pu8RealData[5] << 8 | pu8RealData[6];
            gstAtvvRespData.u16FrameSize = pu8RealData[7] << 8 | pu8RealData[8];
            gstAtvvRespData.u16CharBytes = pu8RealData[9] << 8 | pu8RealData[10];
            PRINTF("AtvvVersionService %d.%d\n", gstAtvvRespData.stAtvvVersionService.u8Major, gstAtvvRespData.stAtvvVersionService.u8Minor);
            PRINTF("CodesSupported %04X\n", gstAtvvRespData.u16CodesSupported);
            PRINTF("FrameSize %d\n", gstAtvvRespData.u16FrameSize);
            PRINTF("CharBytes %d\n", gstAtvvRespData.u16CharBytes);
            break;
        }
        case EN_ATVV_CLIENT_EVENT_AUDIO_START:
        {
            stCodecState.index = 0;
            stCodecState.valprev = 0;
            PRINTF("EN_ATVV_CLIENT_EVENT_AUDIO_START\n");
            break;
        }
        default:
            break;
        }
    }
    case EN_MSG_MODULE_USB:
        switch (u8Event)
        {
        case 0:
            __disable_irq();
            usb_device_send_cdc_data(pu8Data + 2, u16DataLen - 2);
            __enable_irq();
            break;

        default:
            __disable_irq();
            usb_device_send_cdc_data(pu8Data + 2, u16DataLen - 2);
            __enable_irq();
            break;
        }
        break;
    default:
        break;
    }
}

void app_dongle_mp(void)
{
    uint32_t u32Primask = 0;

    MsgQueueId = msg_queue_create(&stQueueDef, u8QueueBuffer, QUEUE_BUFFER_SIZE, AUDIO_PACK_NUM, MSG_QUEUE_SIZE, QUEUE_MODE_NO_OVERFLOW);
    if (MsgQueueId == NULL)
    {
        PRINTF("MsgQueueId Create fail\n");
    }
    recvUsbMsgQueueId = msg_queue_create(&recvUsbStQueueDef, recvUsbQueueBuffer,
                                         RECV_USB_QUEUE_BUFFER_SIZE, USB_PACK_NUM,
                                         USB_QUEUE_SIZE, QUEUE_MODE_NO_OVERFLOW);
    if (recvUsbMsgQueueId == NULL)
    {
        PRINTF("recvUsbMsgQueueId Create fail\n");
    }

    usb_device_init();
    ipc_module_init(ipc_receive_callback);
    usb_cdc_recive_callback_register(usb_cdc_receive_callback);

    while (1)
    {
        if (!msg_queue_is_empty(MsgQueueId))
        {
            msg_queue_pop(MsgQueueId, u8MsgBufferTemp);
            ipc_data_process(&u8MsgBufferTemp[2], BUILD_UINT16(u8MsgBufferTemp[0], u8MsgBufferTemp[1]));
        }

        if (!msg_queue_is_empty(recvUsbMsgQueueId))
        {
            msg_queue_pop(recvUsbMsgQueueId, recvUsbMsgBufferTemp);
            ipc_send(EN_MSG_MODULE_USB, 0, &recvUsbMsgBufferTemp[1], recvUsbMsgBufferTemp[0]);
#if 0
            __disable_irq();
            usb_device_send_cdc_data(&recvUsbMsgBufferTemp[1], recvUsbMsgBufferTemp[0]);
            __enable_irq();
#endif
        }
    }
}