#include <stdio.h>
#include <stdbool.h>

#include "cmsis_os2.h"
#include "rtx_lib.h"

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_gpio.h"
#include "hw_usb_dcd.h"

#include "app_cfg.h"
#include "boards.h"
#include "utility.h"
#include "debug.h"

#include "ble_cb_event_def.h"
#include "ble_init.h"
#include "ble_event.h"

#include "ble_central.h"

#include "led.h"
#include "ipc.h"

#include "hid_client.h"
#include "atvv_client.h"
#include "battery_client.h"
#include "ota_client.h"
#include "app_dongle.h"

#include "msg_queue.h"
#include "nr_micro_shell.h"


//=====================================================================================================================
//define
//=====================================================================================================================
#define APP_MSG_NUM            (10)
#define APP_TASK_STACK_SIZE    (512)

#define USB_QUEUE_SIZE (64)
#define USB_PACK_NUM (8)
#define RECV_USB_QUEUE_BUFFER_SIZE (USB_QUEUE_SIZE * USB_PACK_NUM)

//=====================================================================================================================
//enum
//=====================================================================================================================

typedef enum
{
    EN_SERVICE_CFG_STATE_GET_HID_CHAR_HANDLE        = 0x00,
    EN_SERVICE_CFG_STATE_EN_HID_CHAR_NTF            = 0x01,
    EN_SERVICE_CFG_STATE_GET_ATVV_CHAR_HANDLE       = 0x02,
    EN_SERVICE_CFG_STATE_EN_ATVV_CHAR_NTF           = 0x03,
    EN_SERVICE_CFG_STATE_GET_BATTERY_CHAR_HANDLE    = 0x04,
    EN_SERVICE_CFG_STATE_EN_BATTERY_CHAR_NTF        = 0x05,
    EN_SERVICE_CFG_STATE_GET_OTA_CHAR_HANDLE        = 0x06,
    EN_SERVICE_CFG_STATE_EN_OTA_CHAR_NTF            = 0x07,
    EN_SERVICE_CFG_STATE_FINISH                     = 0x08,
}EN_SERVICE_CFG_STATE_T;

typedef struct
{
    uint8_t  u8MsgModule;
    uint8_t  u8Event;
    uint32_t u32Length;
    void *p;
    uint16_t u16Reserve;
    
}stAppMsg_t;

//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
// task
static osRtxThread_t AppTaskCB __attribute__((aligned(4), section(".bss.os.thread.cb")));
static uint64_t AppStack[APP_TASK_STACK_SIZE];
void app_task(void *argument);

// message
osMessageQueueId_t AppMsgId = NULL;
static uint32_t AppTaskMsgQueueCb[osRtxMessageQueueCbSize / 4U];
static uint32_t AppTaskMsgQueueMem[osRtxMessageQueueMemSize(10, sizeof(stAppMsg_t)) / 4U];

// service
static uint16_t gu16HidServiceStartHandle = 0;
static uint16_t gu16HidServiceEndHandle = 0;
static uint16_t gu16AtvvServiceStartHandle = 0;
static uint16_t gu16AtvvServiceEndHandle = 0;
static uint16_t gu16BatteryServiceStartHandle = 0;
static uint16_t gu16BatteryServiceEndHandle = 0;
static uint16_t gu16OtaServiceStartHandle = 0;
static uint16_t gu16OtaServiceEndHandle = 0;

EN_SERVICE_CFG_STATE_T genServiceCfgState = EN_SERVICE_CFG_STATE_GET_HID_CHAR_HANDLE;

pMsgQueueId_t recvUsbMsgQueueId;
stQueueDef_t recvUsbStQueueDef;
uint8_t recvUsbQueueBuffer[RECV_USB_QUEUE_BUFFER_SIZE];
uint8_t recvUsbMsgBufferTemp[USB_QUEUE_SIZE];

//=====================================================================================================================
//function
//=====================================================================================================================
static void app_dongle_service_cfg(void);
static void app_result_check(const char *pcharTag, uint32_t u32Line, int8_t i8ErrCode);
static void ble_central_event_process(stAppMsg_t *stAppMsg);
static void atvv_client_event_process(stAppMsg_t *stAppMsg);
static void hid_client_event_process(stAppMsg_t *stAppMsg);
static void battery_client_event_process(stAppMsg_t *stAppMsg);
static void ota_client_event_process(stAppMsg_t *stAppMsg);

//=====================================================================================================================
//                                            task
//=====================================================================================================================
/**
 * @brief  App task and message queue init.
 * @param  None.
 * @return true or false.
 */
static bool app_task_init(void)
{
    const osMessageQueueAttr_t AppMsgAueueAttr =
    {
        .name = "AppRcuMsgAueueAttr",
        .attr_bits = 0,
        .cb_mem = AppTaskMsgQueueCb,
        .cb_size = sizeof(AppTaskMsgQueueCb),
        .mq_mem = AppTaskMsgQueueMem,
        .mq_size = sizeof(AppTaskMsgQueueMem),
    };
    
    const osThreadAttr_t AppThreadAttr =
    {
        "app_rcu_task",
        osThreadJoinable,
        &AppTaskCB,
        sizeof(AppTaskCB),
        &AppStack[0],
        sizeof(AppStack),
        osPriorityNormal,
        0
    };
    
    osThreadId_t RcuTaskThreadId;
    
    // create app rcu message
    AppMsgId = osMessageQueueNew(APP_MSG_NUM, sizeof(stAppMsg_t), &AppMsgAueueAttr);
    if(NULL == AppMsgId)
    {
        PRINTF("create AppMsgId failed\n");
        return false;
    }
    
    // create app rcu task
    RcuTaskThreadId = osThreadNew(app_task, NULL, &AppThreadAttr);
    if (NULL == RcuTaskThreadId)
    {
        PRINTF("RcuTaskThreadId NULL\n");
        return false;
    }
    
    return true;
}

/**
 * @brief  Sends a message to app task.
 * @param  u8Modlue: Indicates which module sent this message, @ref EN_MSG_MODULE_T.
 * @param  u8Event: Indicates which event in this message.
 * @param  pu8Data: The data of this message.
 * @param  u32Len: The length of message data.
 * @return true or false.
 */
static bool send_msg_app_task(uint8_t u8Modlue, uint8_t u8Event, uint8_t *pu8Data, uint32_t u32Len)
{
    uint32_t u32Timeout = osWaitForever;
    stAppMsg_t stAppMsg;

    if (__get_IPSR())
    {
        // in interrupt
        u32Timeout = 0;
    }
    
    stAppMsg.u8MsgModule = u8Modlue;
    stAppMsg.u8Event = u8Event;
    stAppMsg.p = pu8Data;
    stAppMsg.u32Length = u32Len;

    osStatus_t Status = osMessageQueuePut(AppMsgId, &stAppMsg, 0, u32Timeout);
    if (osOK == Status)
    {
        return true;
    }
    else
    {
        PRINTF("send_msg_app_task failed: %d, %X\n", Status, Status);
        return false;
    }
}


//=====================================================================================================================
//                                            module callback
//=====================================================================================================================
/**
 * @brief  BLE module event callback function.
 * @param  u8Event: Indicates which event in this message.
 * @param  pu8Data: The data of this message.
 * @param  u32Len: The length of message data.
 * @return None.
 */
static void ble_event_callback(uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen)
{
    if(!send_msg_app_task(EN_MSG_MODULE_BLE, u8Event, pu8Data, u16DataLen))
    {
        app_result_check(__func__, __LINE__, -1);
    }
}

/**
 * @brief  BLE central module event callback function.
 * @param  u8Event: Indicates which event in this message.
 * @param  pu8Data: The data of this message.
 * @param  u32Len: The length of message data.
 * @return None.
 */
static void ble_central_event_callback(uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen)
{
    switch(u8Event)
    {
        case EN_BLE_CENTRAL_EVENT_FIND_SERVICE_FINISH:
        {
            if(!send_msg_app_task(EN_MSG_MODULE_BLE_CENTRAL, u8Event, pu8Data, u16DataLen))
            {
                app_result_check(__func__, __LINE__, -1);
            }
            break;
        }
        //notify data used host buffer, if not copy, process in host task.
        case EN_BLE_CENTRAL_EVENT_NOTIFY:
        {
            stAppMsg_t stAppMsg;
            stAppMsg.u8Event = u8Event;
            stAppMsg.p = pu8Data;
            stAppMsg.u32Length = u16DataLen;
            ble_central_event_process(&stAppMsg);//host task call
            break;
        }
        default : break;
    }
}

/**
 * @brief  HID client event callback function.
 * @param  u8Event: Indicates which event in this message.
 * @param  pu8Data: The data of this message.
 * @param  u32Len: The length of message data.
 * @return None.
 */
static void hid_client_event_callback(uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen)
{
    switch(u8Event)
    {
        case EN_HID_CLIENT_EVENT_CHAR_FIND_FINISH:
        case EN_HID_CLIENT_EVENT_CHAR_EN_NTF_FINISH:
        {
            if(!send_msg_app_task(EN_MSG_MODULE_HID_CLIENT, u8Event, pu8Data, u16DataLen))
            {
                app_result_check(__func__, __LINE__, -1);
            }
            break;
        }
        //notify data used host buffer, if not copy, process in host task.
        case EN_HID_CLIENT_EVENT_KEY_CHAR_DATA:
        case EN_HID_CLIENT_EVENT_CONSUMER_CHAR_DATA:
        {
            stAppMsg_t stAppMsg;
            stAppMsg.u8Event = u8Event;
            stAppMsg.p = pu8Data;
            stAppMsg.u32Length = u16DataLen;
            hid_client_event_process(&stAppMsg);
            break;
        }
        default : break;
    }
}

/**
 * @brief  ATVV client event callback function.
 * @param  u8Event: Indicates which event in this message.
 * @param  pu8Data: The data of this message.
 * @param  u32Len: The length of message data.
 * @return None.
 */
static void atvv_client_event_callback(uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen)
{
    switch(u8Event)
    {
        case EN_ATVV_CLIENT_EVENT_CHAR_FIND_FINISH:
        case EN_ATVV_CLIENT_EVENT_CHAR_EN_NTF_FINISH:
        {
            if(!send_msg_app_task(EN_MSG_MODULE_ATVV_CLIENT, u8Event, pu8Data, u16DataLen))
            {
                app_result_check(__func__, __LINE__, -1);
            }
            break;
        }
        //notify data used host buffer, if not copy, process in host task.
        case EN_ATVV_CLIENT_EVENT_AUDIO_DATA:
        case EN_ATVV_CLIENT_EVENT_CAPS_RSP:
        case EN_ATVV_CLIENT_EVENT_AUDIO_START:
        {
            stAppMsg_t stAppMsg;
            stAppMsg.u8Event = u8Event;
            stAppMsg.p = pu8Data;
            stAppMsg.u32Length = u16DataLen;
            atvv_client_event_process(&stAppMsg);
            break;
        }
        default : break;
    }
}

/**
 * @brief  Battery client event callback function.
 * @param  u8Event: Indicates which event in this message.
 * @param  pu8Data: The data of this message.
 * @param  u32Len: The length of message data.
 * @return None.
 */
static void battery_client_event_callback(uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen)
{
    switch(u8Event)
    {
        case EN_BATTERY_CLIENT_EVENT_CHAR_FIND_FINISH:
        case EN_BATTERY_CLIENT_EVENT_CHAR_EN_NTF_FINISH:
        {
            if(!send_msg_app_task(EN_MSG_MODULE_BATTERY_CLIENT, u8Event, pu8Data, u16DataLen))
            {
                app_result_check(__func__, __LINE__, -1);
            }
            break;
        }
        //notify data used host buffer, if not copy, process in host task.
        case EN_BATTERY_CLIENT_EVENT_BATTERY_LEVEL_DATA:
        {
            stAppMsg_t stAppMsg;
            stAppMsg.u8Event = u8Event;
            stAppMsg.p = pu8Data;
            stAppMsg.u32Length = u16DataLen;
            battery_client_event_process(&stAppMsg);
            break;
        }
        default : break;
    }
}

/**
 * @brief  OTA client event callback function.
 * @param  u8Event: Indicates which event in this message.
 * @param  pu8Data: The data of this message.
 * @param  u32Len: The length of message data.
 * @return None.
 */
static void ota_client_event_callback(uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen)
{
    switch(u8Event)
    {
        case EN_OTA_CLIENT_EVENT_CHAR_FIND_FINISH:
        case EN_OTA_CLIENT_EVENT_CHAR_EN_NTF_FINISH:
        {
            if(!send_msg_app_task(EN_MSG_MODULE_OTA_CLIENT, u8Event, pu8Data, u16DataLen))
            {
                app_result_check(__func__, __LINE__, -1);
            }
            break;
        }
        case EN_OTA_CLIENT_EVENT_DATA:
        {
            stAppMsg_t stAppMsg;
            stAppMsg.u8Event = u8Event;
            stAppMsg.p = pu8Data;
            stAppMsg.u32Length = u16DataLen;
            ota_client_event_process(&stAppMsg);
            break;
        }
        default : break;
    }
}

/**
 * @brief  LED module event callback function.
 * @param  u8Event: Indicates which event in this message.
 * @param  pu8Data: The data of this message.
 * @param  u32Len: The length of message data.
 * @return None.
 */
static void led_event_callback( uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen)
{
    if(!send_msg_app_task(EN_MSG_MODULE_LED, u8Event, pu8Data, u16DataLen))
    {
        app_result_check(__func__, __LINE__, -1);
    }
}

static void ipc_receive_callback(uint8_t* pu8Data, uint16_t u16DataLen)
{
    EN_QUEUE_ERR_STA_T enRet;

    enRet = msg_queue_push(recvUsbMsgQueueId, &pu8Data[2]);

    if (enRet != QUEUE_STA_OK)
    {
        // PRINTF("put queue fail\n");
        return;
    }
    if(!send_msg_app_task(EN_MSG_MODULE_USB, pu8Data[1], NULL, u16DataLen - 2))
    {
        app_result_check(__func__, __LINE__, -1);
    }
}

//=====================================================================================================================
//                                            module event process
//=====================================================================================================================
/**
 * @brief  BLE event process function.
 * @param  stAppMsg: Pointer to BLE module message.
 * @return None.
 */
static void ble_event_process(stAppMsg_t *stAppMsg)
{
    bool bRet = false;
    uint32_t u32Ret = 0;
    switch(stAppMsg->u8Event)
    {
        case MSG_BLE_CONNECTED_IND:
        {
            PRINTF("ble_event_process MSG_BLE_CONNECTED_IND\n");
//            uint8_t u8Temp = 0;
//            PRINTF("handle %d\n", ble_get_connect_handle());
//            u32Ret = ble_central_discover_primary_services(ble_get_connect_handle());
//            PRINTF("start scan %d\n", u32Ret);
//            ble_central_send_security_req(ble_get_connect_handle());
            led_off();
            led_on();
            break;
        }
        case MSG_BLE_DISCONNECTED_IND:
        {
            // clear service
            ble_central_free_client_data_buffer();
            bRet = ble_central_start_scan();
            if(!bRet)
            {
                PRINTF("set scan start fail\n");
            }
            led_off();
            break;
        }
        case MSG_BLE_SET_SCAN_ENABLE_COMMAND_RESULT:
        {
            uint8_t *u8Buf = stAppMsg->p;
            if(0 == *u8Buf)
            {
                PRINTF("scan start\n");
                led_blink(2);
            }
            else
            {
                led_off();
            }
            break;
        }
        default : break;
    }
}

/**
 * @brief  BLE central event process function.
 * @param  stAppMsg: Pointer to BLE central message.
 * @return None.
 */
static void ble_central_event_process(stAppMsg_t *stAppMsg)
{
    switch(stAppMsg->u8Event)
    {
        case EN_BLE_CENTRAL_EVENT_FIND_SERVICE_FINISH:
        {
            // get service handle
            hid_client_get_srvice_handle(&gu16HidServiceStartHandle, &gu16HidServiceEndHandle);
            atvv_client_get_srvice_handle(&gu16AtvvServiceStartHandle, &gu16AtvvServiceEndHandle);
            battery_client_get_srvice_handle(&gu16BatteryServiceStartHandle, &gu16BatteryServiceEndHandle);
            ota_client_get_srvice_handle(&gu16OtaServiceStartHandle, &gu16OtaServiceEndHandle);
            PRINTF("HID handle %d - %d\n", gu16HidServiceStartHandle, gu16HidServiceEndHandle);
            PRINTF("ATVV handle %d - %d\n", gu16AtvvServiceStartHandle, gu16AtvvServiceEndHandle);
            PRINTF("BATTERY handle %d - %d\n", gu16BatteryServiceStartHandle, gu16BatteryServiceEndHandle);
            PRINTF("OTA handle %d - %d\n", gu16OtaServiceStartHandle, gu16OtaServiceEndHandle);
            // start service cfg flow
            genServiceCfgState = EN_SERVICE_CFG_STATE_GET_HID_CHAR_HANDLE;
            app_dongle_service_cfg();
            break;
        }
        case EN_BLE_CENTRAL_EVENT_NOTIFY:
        {
            uint16_t u16CharValueHandle = rom_little_endian_read_16(stAppMsg->p, 0);
            PRINTF("EN_BLE_CENTRAL_EVENT_NOTIFY:%d\n",u16CharValueHandle);
            if(gu16HidServiceStartHandle < u16CharValueHandle && u16CharValueHandle < gu16HidServiceEndHandle)
            {
                hid_client_process_notify_data(stAppMsg->p, stAppMsg->u32Length);
            }
            else if(gu16AtvvServiceStartHandle < u16CharValueHandle && u16CharValueHandle < gu16AtvvServiceEndHandle)
            {
                atvv_client_process_notify_data(stAppMsg->p, stAppMsg->u32Length);
            }
            else if(gu16BatteryServiceStartHandle < u16CharValueHandle && u16CharValueHandle < gu16BatteryServiceEndHandle)
            {
                battery_client_process_notify_data(stAppMsg->p, stAppMsg->u32Length);
            }
            else if(gu16OtaServiceStartHandle < u16CharValueHandle && u16CharValueHandle < gu16OtaServiceEndHandle)
            {
                ota_client_process_notify_data(stAppMsg->p, stAppMsg->u32Length);
            }
            break;
        }
        default : break;
    }
}

/**
 * @brief  HID client event process function.
 * @param  stAppMsg: Pointer to HID client message.
 * @return None.
 */
static void hid_client_event_process(stAppMsg_t *stAppMsg)
{
    switch(stAppMsg->u8Event)
    {
        case EN_HID_CLIENT_EVENT_CHAR_FIND_FINISH:
        {
            genServiceCfgState = EN_SERVICE_CFG_STATE_EN_HID_CHAR_NTF;
            app_dongle_service_cfg();
            break;
        }
        case EN_HID_CLIENT_EVENT_CHAR_EN_NTF_FINISH:
        {
            genServiceCfgState = EN_SERVICE_CFG_STATE_GET_ATVV_CHAR_HANDLE;
            app_dongle_service_cfg();
            break;
        }
        case EN_HID_CLIENT_EVENT_KEY_CHAR_DATA:
        {
            PRINTF("hid key report\n");
            uint8_t *pu8Data = stAppMsg->p;
            for(int i=0; i<stAppMsg->u32Length; i++)
            {
                PRINTF("%02X ", pu8Data[i]);
            }
            PRINTF("\n");
            bool bRet;
            bRet = ipc_send(EN_MSG_MODULE_HID_CLIENT, EN_HID_CLIENT_EVENT_KEY_CHAR_DATA, stAppMsg->p, stAppMsg->u32Length);
            if(!bRet)
            {
                PRINTF("ipc send fail\n");
            }
            //usb_device_hid_keyboard_data_report(stAppMsg->p, stAppMsg->u32Length);
            break;
        }
        case EN_HID_CLIENT_EVENT_CONSUMER_CHAR_DATA:
        {
            PRINTF("hid consumer report\n");
            uint8_t *pu8Data = stAppMsg->p;
            for(int i=0; i<stAppMsg->u32Length; i++)
            {
                PRINTF("%02X ", pu8Data[i]);
            }
            PRINTF("\n");
            bool bRet;
            bRet = ipc_send(EN_MSG_MODULE_HID_CLIENT, EN_HID_CLIENT_EVENT_CONSUMER_CHAR_DATA, stAppMsg->p, stAppMsg->u32Length);
            if(!bRet)
            {
                PRINTF("ipc send fail\n");
            }
            //usb_device_hid_consumer_data_report(stAppMsg->p, stAppMsg->u32Length);
            break;
        }
        default : break;
    }
}

/**
 * @brief  ATVV client event process function.
 * @param  stAppMsg: Pointer to ATVV client message.
 * @return None.
 */
static void atvv_client_event_process(stAppMsg_t *stAppMsg)
{
    switch(stAppMsg->u8Event)
    {
        case EN_ATVV_CLIENT_EVENT_CHAR_FIND_FINISH:
        {
            genServiceCfgState = EN_SERVICE_CFG_STATE_EN_ATVV_CHAR_NTF;
            app_dongle_service_cfg();
            break;
        }
        case EN_ATVV_CLIENT_EVENT_CHAR_EN_NTF_FINISH:
        {
            PRINTF("EN_ATVV_CLIENT_EVENT_CHAR_EN_NTF_FINISH\n");
            genServiceCfgState = EN_SERVICE_CFG_STATE_GET_BATTERY_CHAR_HANDLE;
            app_dongle_service_cfg();
            break;
        }
        case EN_ATVV_CLIENT_EVENT_AUDIO_DATA:
        {
            //PRINTF("atvv %d\n", stAppMsg->u32Length);
            bool bRet;
            bRet = ipc_send(EN_MSG_MODULE_ATVV_CLIENT, EN_ATVV_CLIENT_EVENT_AUDIO_DATA, stAppMsg->p, stAppMsg->u32Length);
            if(!bRet)
            {
                PRINTF("ipc send fail\n");
            }
//            if( EN_DEVICE_NONE_ERR != usb_device_send_mic_data( (uint8_t*)(stAppMsg->p), (stAppMsg->u32Length)<<1 ))
//            {
//                PRINTF("USB MIC Send Fail!!!\n");
//            }
            break;
        }
        case EN_ATVV_CLIENT_EVENT_CAPS_RSP:
        {
            PRINTF("EN_ATVV_CLIENT_EVENT_CAPS_RSP %d\n", stAppMsg->u32Length);
            bool bRet;
            bRet = ipc_send(EN_MSG_MODULE_ATVV_CLIENT, EN_ATVV_CLIENT_EVENT_CAPS_RSP, stAppMsg->p, stAppMsg->u32Length);
            if(!bRet)
            {
                PRINTF("ipc send fail\n");
            }
            break;
        }
        case EN_ATVV_CLIENT_EVENT_AUDIO_START:
        {
            PRINTF("EN_ATVV_CLIENT_EVENT_AUDIO_START\n");
            bool bRet;
            bRet = ipc_send(EN_MSG_MODULE_ATVV_CLIENT, EN_ATVV_CLIENT_EVENT_AUDIO_START, stAppMsg->p, stAppMsg->u32Length);
            if(!bRet)
            {
                PRINTF("ipc send fail\n");
            }
            break;
        }
        default : break;
    }
}

/**
 * @brief  Battery client event process function.
 * @param  stAppMsg: Pointer to Battery client message.
 * @return None.
 */
static void battery_client_event_process(stAppMsg_t *stAppMsg)
{
    switch(stAppMsg->u8Event)
    {
        case EN_BATTERY_CLIENT_EVENT_CHAR_FIND_FINISH:
        {
            genServiceCfgState = EN_SERVICE_CFG_STATE_EN_BATTERY_CHAR_NTF;
            app_dongle_service_cfg();
            break;
        }
        case EN_BATTERY_CLIENT_EVENT_CHAR_EN_NTF_FINISH:
        {
            genServiceCfgState = EN_SERVICE_CFG_STATE_GET_OTA_CHAR_HANDLE;
            app_dongle_service_cfg();
            PRINTF("EN_BATTERY_CLIENT_EVENT_CHAR_EN_NTF_FINISH\n");
            break;
        }
        case EN_BATTERY_CLIENT_EVENT_BATTERY_LEVEL_DATA:
        {
            uint8_t *pu8BatteryLevel = stAppMsg->p;
            PRINTF("Battery Level: %d%%\n", pu8BatteryLevel[2]);
            break;
        }
        default : break;
    }
}

/**
 * @brief  OTA client event process function.
 * @param  stAppMsg: Pointer to OTA client message.
 * @return None.
 */
static void ota_client_event_process(stAppMsg_t *stAppMsg)
{
    switch(stAppMsg->u8Event)
    {
        case EN_OTA_CLIENT_EVENT_CHAR_FIND_FINISH:
        {
            genServiceCfgState = EN_SERVICE_CFG_STATE_EN_OTA_CHAR_NTF;
            app_dongle_service_cfg();
            break;
        }
        case EN_OTA_CLIENT_EVENT_CHAR_EN_NTF_FINISH:
        {
            PRINTF("EN_OTA_CLIENT_EVENT_CHAR_EN_NTF_FINISH\n");
            genServiceCfgState = EN_SERVICE_CFG_STATE_FINISH;
            app_dongle_service_cfg();
            break;
        }
        case EN_OTA_CLIENT_EVENT_DATA:
        {
            uint8_t *pu8OtaData = stAppMsg->p;
            uint16_t u16Len = stAppMsg->u32Length;
            
            break;
        }
        default : break;
    }
}

/**
 * @brief  LED event process function.
 * @param  stAppMsg: Pointer to LED module message.
 * @return None.
 */
static void led_event_process(stAppMsg_t *stAppMsg)
{
    switch(stAppMsg->u8Event)
    {
        default: break;
    }
}

/**
 * @brief 处理接收到M33发送的数据，已弃用
 * 
 * @param stAppMsg 
*/
static void ipc_event_process(stAppMsg_t *stAppMsg)
{
    #if 0 
    uint8_t *pu8Data = stAppMsg->p;
    switch(stAppMsg->u8Event)
    {
        case EN_IPC_EVENT_DATA_REPORT:
        {
            uint8_t u8Module = pu8Data[0];
            uint8_t u8Event = pu8Data[1];
            if(!send_msg_app_task(u8Module, u8Event, pu8Data+2, stAppMsg->u32Length-2))
            {
                app_result_check(__func__, __LINE__, -1);
            }
        }
        default : break;
    }
    #endif
}

/**
 * @brief 处理接收到USB数据
 * 
 * @param stAppMsg 
*/
static void usb_cdc_event_process(stAppMsg_t *stAppMsg)
{
    bool bRet = false;
    EN_QUEUE_ERR_STA_T enRet;
    static uint8_t _buf[USB_QUEUE_SIZE];
    switch(stAppMsg->u8Event)
    {
        case 0:
        {
            enRet = msg_queue_pop(recvUsbMsgQueueId, _buf);
            if (enRet != QUEUE_STA_OK)
            {
                PRINTF("msg queue pop fail\n");
                break;
            }
            
            for (size_t i = 0; i < stAppMsg->u32Length; i++)
            {
                shell(_buf[i]);
            }
            

            // bRet = ipc_send(EN_MSG_MODULE_USB, 0, _buf, stAppMsg->u32Length);
            // if(!bRet)
            // {
            //     PRINTF("ipc send fail\n");
            // }
        }
        default : break;
    }
}

//=====================================================================================================================
//                                            app dongle
//=====================================================================================================================
/**
 * @brief  Config services function.
 * @param  None.
 * @return None.
 */
static void app_dongle_service_cfg(void)
{
    switch(genServiceCfgState)
    {
        case EN_SERVICE_CFG_STATE_GET_HID_CHAR_HANDLE:
        {
            // find char handle
            PRINTF("hid_client_get_char_handle\n");
            hid_client_get_char_handle();
            break;
        }
        case EN_SERVICE_CFG_STATE_EN_HID_CHAR_NTF:
        {
            PRINTF("hid_client_enable_notify\n");
            hid_client_enable_notify();
            break;
        }
        case EN_SERVICE_CFG_STATE_GET_ATVV_CHAR_HANDLE:
        {
            PRINTF("atvv_client_get_char_handle\n");
            atvv_client_get_char_handle();
            break;
        }
        case EN_SERVICE_CFG_STATE_EN_ATVV_CHAR_NTF:
        {
            PRINTF("atvv_client_enable_notify\n");
            atvv_client_enable_notify();
            break;
        }
        case EN_SERVICE_CFG_STATE_GET_BATTERY_CHAR_HANDLE:
        {
            PRINTF("battery_client_get_char_handle\n");
            battery_client_get_char_handle();
            break;
        }
        case EN_SERVICE_CFG_STATE_EN_BATTERY_CHAR_NTF:
        {
            PRINTF("battery_client_enable_notify\n");
            battery_client_enable_notify();
            break;
        }
        case EN_SERVICE_CFG_STATE_GET_OTA_CHAR_HANDLE:
        {
            PRINTF("ota_client_get_char_handle\n");
            ota_client_get_char_handle();
            break;
        }
        case EN_SERVICE_CFG_STATE_EN_OTA_CHAR_NTF:
        {
            PRINTF("ota_client_enable_notify\n");
            ota_client_enable_notify();
            break;
        }
        case EN_SERVICE_CFG_STATE_FINISH:
        {
            atvv_client_send_cmd(ATVV_CMD_GET_CAPS);
            break;
        }
        default : break;
    }
}

/**
 * @brief  Dongle module init.
 * @param  None.
 * @return true or false.
 */
static bool app_dongle_module_init(void)
{
    recvUsbMsgQueueId = msg_queue_create(&recvUsbStQueueDef, recvUsbQueueBuffer,
                                         RECV_USB_QUEUE_BUFFER_SIZE, USB_PACK_NUM,
                                         USB_QUEUE_SIZE, QUEUE_MODE_NO_OVERFLOW);
    if (recvUsbMsgQueueId == NULL)
    {
        PRINTF("recvUsbMsgQueueId Create fail\n");
    }

    shell_init();

    // 1. module init
    ipc_module_init(ipc_receive_callback);
    led_init(led_event_callback);
    
    // 2. ble init
    ble_init();
    
    hid_client_event_callback_regiest(hid_client_event_callback);
    atvv_client_event_callback_regiest(atvv_client_event_callback);
    battery_client_event_callback_regiest(battery_client_event_callback);
    ota_client_event_callback_regiest(ota_client_event_callback);
    
    ble_event_init(ble_event_callback);
    ble_central_init(ble_central_event_callback);

    PRINTF("app_dongle_module_init done\n");
    return true;
}

/**
 * @brief  This function is convenient for outputting debugging information.
 * @param  pcharTag The information of string type.
 * @param  u32Line Indicates the debugging information from which line.
 * @param  i8ErrCode The error code of debugging information.
 * @return None.
 */
static void app_result_check(const char *pcharTag, uint32_t u32Line, int8_t i8ErrCode)
{
    if(pcharTag != NULL)
    {
        PRINTF("%s, Line:%d Code:%d\n", pcharTag, u32Line, i8ErrCode);
    }
}

/**
 * @brief  Dongle init.
 * @param  None.
 * @return None.
 */
void app_dongle_init(void)
{
    app_task_init();
}

/**
 * @brief  Dongle app task.
 * @param  argument: app task param.
 * @return None.
 */
void app_task(void *argument)
{
    stAppMsg_t stAppMsg;
    osStatus_t Status = osOK;
    app_dongle_module_init();
    while(1)
    {
        Status = osMessageQueueGet(AppMsgId, &stAppMsg, NULL, osWaitForever);
        if(Status == osOK)
        {
            switch(stAppMsg.u8MsgModule)
            {
                case EN_MSG_MODULE_BLE_CENTRAL:
                {
                    ble_central_event_process(&stAppMsg);
                    break;
                }
                case EN_MSG_MODULE_HID_CLIENT:
                {
                    hid_client_event_process(&stAppMsg);
                    break;
                }
                case EN_MSG_MODULE_ATVV_CLIENT:
                {
                    atvv_client_event_process(&stAppMsg);
                    break;
                }
                case EN_MSG_MODULE_BATTERY_CLIENT:
                {
                    battery_client_event_process(&stAppMsg);
                    break;
                }
                case EN_MSG_MODULE_OTA_CLIENT:
                {
                    ota_client_event_process(&stAppMsg);
                    break;
                }
                case EN_MSG_MODULE_LED:
                {
                    led_event_process(&stAppMsg);
                    break;
                }
                case EN_MSG_MODULE_BLE:
                {
                    ble_event_process(&stAppMsg);
                    break;
                }
                case EN_MSG_MODULE_USB:
                {
                    usb_cdc_event_process(&stAppMsg);
                    break;
                }
                default: break;
            }
        }
        else
        {
            PRINTF("app_rcu_task get msg failed:%d, 0x%X\n", Status, Status);
        }
    }
}