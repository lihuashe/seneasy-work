#define __DEBUG__ __DEBUG_CLOSE__

#include "control_protocol.h"
#include "control_state.h"
#include "pan216_ext.h"
#include "2_4g_io.h"
#include "flash_io.h"
#include <string.h>
#include "my_log.h"
#include "app.h"

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
const uint8_t fs01_frame_preamble_and_addr[] = {0xAA, 0xAA, 0xAA, 0xBB, 0xCC, 0xDD};

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/**
 *
 */
static uint8_t set_checksum(void *p, uint8_t p_len)
{
    uint8_t *payload = p;
    uint32_t checksum = 0;
    for (size_t i = 0; i < p_len; i++)
    {
        checksum += payload[i];
    }
    return checksum & 0xFF;
}

void a7535_format(RfSendType t, CtrlPayload_A7535 *controlData)
{
    // 保留位置0xFF
    controlData->ctrl_data.reserved = 0xFF;

    controlData->checksum = set_checksum(&controlData->unique_id, sizeof(CtrlPayload_A7535) - sizeof(fs01_frame_preamble_and_addr) - 1);

#if __DEBUG__ >= __DEBUG_DEV__
    my_log("\n");
    my_log("===================================\n");
    my_log("main_mode      : 0x%02x\n", controlData->ctrl_data.main_mode);
    my_log("fan_speed      : 0x%02x\n", controlData->ctrl_data.fan_speed);
    my_log("air_direction  : 0x%02x\n", controlData->ctrl_data.air_direction);
    my_log("air_type       : 0x%02x\n", controlData->ctrl_data.air_type);
    my_log("time_setting   : 0x%02x\n", controlData->ctrl_data.time_setting);
    my_log("remain_hours   : 0x%02x\n", controlData->ctrl_data.remain_hours);
    my_log("remain_minutes : 0x%02x\n", controlData->ctrl_data.remain_minutes);
    my_log("light_ctrl     : 0x%02x\n", controlData->ctrl_data.light_ctrl);
    my_log("wifi_ctrl      : 0x%02x\n", controlData->ctrl_data.wifi_ctrl);
    my_log("voice_ctrl     : 0x%02x\n", controlData->ctrl_data.voice_ctrl);
    my_log("model_type     : 0x%02x\n", controlData->ctrl_data.model_type);
    my_log("===================================\n");
#endif
}

void xn297l_format(RfSendType t, CtrlPayload_XN297L *controlData)
{
    controlData->checksum = set_checksum(&controlData->unique_id, sizeof(CtrlPayload_XN297L) - sizeof(fs01_frame_preamble_and_addr) - 1);
    // 保留位置0xFF
    memset(controlData->ctrl_data.reserved, 0xFF, sizeof(controlData->ctrl_data.reserved));

#if __DEBUG__ >= __DEBUG_DEV__
    my_log("\n");
    my_log("===================================\n");
    my_log("main_mode      : 0x%02x\n", controlData->ctrl_data.main_mode);
    my_log("fan_speed      : 0x%02x\n", controlData->ctrl_data.fan_speed);
    my_log("air_direction  : 0x%02x\n", controlData->ctrl_data.air_direction);
    my_log("air_type       : 0x%02x\n", controlData->ctrl_data.air_type);
    my_log("time_setting   : 0x%02x\n", controlData->ctrl_data.time_setting);
    my_log("remain_hours   : 0x%02x\n", controlData->ctrl_data.remain_hours);
    my_log("remain_minutes : 0x%02x\n", controlData->ctrl_data.remain_minutes);
    my_log("light_ctrl     : 0x%02x\n", controlData->ctrl_data.light_ctrl);
    my_log("wifi_ctrl      : 0x%02x\n", controlData->ctrl_data.wifi_ctrl);
    my_log("voice_ctrl     : 0x%02x\n", controlData->ctrl_data.voice_ctrl);
    my_log("model_type     : 0x%02x\n", controlData->ctrl_data.model_type);
    my_log("smell          : 0x%02x\n", controlData->ctrl_data.smell);
    my_log("bluetooth_music: 0x%02x\n", controlData->ctrl_data.bluetooth_music);
    my_log("filter         : 0x%02x\n", controlData->ctrl_data.filter);
    my_log("===================================\n");
#endif
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/**
 *
 */
void send_ctrl_data(RfSendType t)
{
    uint8_t buf[48];
    memset(&buf, 0xFF, sizeof(buf));
    CtrlPayload *controlData = (CtrlPayload *)buf;

    // 添加前导码与地址
    memcpy(&controlData->preamble_fixed, fs01_frame_preamble_and_addr, sizeof(fs01_frame_preamble_and_addr));

    // 添加UNIQUE ID
    memcpy(&controlData->unique_id, app.unique_id, sizeof(app.unique_id));

    // 复制控制数据到payload
    memcpy(&controlData->ctrl_data, &g_current_ctrl, sizeof(controlData->ctrl_data));

    // 根据不同发送类型对控制数据进行配置修改
    switch (t)
    {
    case RF_SEND_POWER_STOP:
        // 待机，辅助功能发送【无设定:0xFF】
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.time_setting = 0;
        controlData->ctrl_data.remain_hours = 0;
        controlData->ctrl_data.remain_minutes = 0;
        controlData->ctrl_data.main_mode = MODE_STOP;

        break;
    case RF_SEND_MAIN_FUN:
        // 主功能设定时主功能按设定值进行发码，辅助功能发送【无设定:0xFF】
        controlData->ctrl_data.wifi_ctrl = 0xFF;
        controlData->ctrl_data.light_ctrl = 0xFF;
        controlData->ctrl_data.voice_ctrl = 0xFF;
        controlData->ctrl_data.smell = 0xFF;
        controlData->ctrl_data.bluetooth_music = 0xFF;
        controlData->ctrl_data.filter = 0xFF;
        break;
    case RF_SEND_BABY_FUN:
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.main_mode = MODE_BABY;
        break;
    case RF_SEND_NANOE_X_FUN:
        controlData->ctrl_data.wifi_ctrl = 0xFF;
        controlData->ctrl_data.light_ctrl = 0xFF;
        controlData->ctrl_data.voice_ctrl = 0xFF;
        controlData->ctrl_data.smell = 0xFF;
        controlData->ctrl_data.bluetooth_music = 0xFF;
        controlData->ctrl_data.filter = 0xFF;
        break;
    case RF_SEND_AIR_SET:
        // 各模式下，风向，风种有效时，按下风向，风种按键,【时间设定】和【剩余时间】 发送【无设定:0xFF】
        controlData->ctrl_data.time_setting = 0xFF;
        controlData->ctrl_data.remain_hours = 0xFF;
        controlData->ctrl_data.remain_minutes = 0xFF;
        controlData->ctrl_data.wifi_ctrl = 0xFF;
        controlData->ctrl_data.light_ctrl = 0xFF;
        controlData->ctrl_data.voice_ctrl = 0xFF;
        controlData->ctrl_data.smell = 0xFF;
        controlData->ctrl_data.bluetooth_music = 0xFF;
        controlData->ctrl_data.filter = 0xFF;
        break;
    case RF_SEND_LIGHT:
        // 辅助功能设定时(照明/WiFi设定)，主功能(No.1 运转模式~No.7剩余时间(分))设定为【无设定:0xFF】
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.light_ctrl = LIGHT_TOGGLE;
        break;
    case RF_SEND_WIFI_RESET_AP:
        // 辅助功能设定时(照明/WiFi设定)，主功能(No.1 运转模式~No.7剩余时间(分))设定为【无设定:0xFF】
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.wifi_ctrl = WIFI_RESET_AP;
        break;
    case RF_SEND_WIFI:
        // 辅助功能设定时(照明/WiFi设定)，主功能(No.1 运转模式~No.7剩余时间(分))设定为【无设定:0xFF】
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.wifi_ctrl = WIFI_ON;
        break;
    case RF_SEND_NETWORK_CONFIG:
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.wifi_ctrl = WIFI_RESET_TMALL;
        break;
    case RF_SEND_SMELL:
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.smell = SMELL_TOGGLE;
        break;
    case RF_SEND_BLUETOOTH_MUSIC:
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.smell = BLUETOOTH_MUSIC_TOGGLE;
        break;
    case RF_SEND_FILTER:
        memset(&controlData->ctrl_data, 0xFF, sizeof(controlData->ctrl_data));
        controlData->ctrl_data.smell = FILTER;
        break;
    default:
        return;
    }
    // 设定机型
    controlData->ctrl_data.model_type = app.modelType;
    if (app.chipMode == A7535)
    {
        a7535_format(t, (CtrlPayload_A7535 *)buf);
        rf_2_4g_sendto((uint8_t *)buf, sizeof(CtrlPayload_A7535));
    }
    else if (app.chipMode == XN297L)
    {
        xn297l_format(t, (CtrlPayload_XN297L *)buf);
        rf_2_4g_sendto((uint8_t *)buf, sizeof(CtrlPayload_XN297L));
    }
}

void send_pair_data()
{
    PairPayload pair_data;
    memset(&pair_data, 0, sizeof(pair_data));

    // 添加前导码与地址
    memcpy(&pair_data.preamble_fixed, fs01_frame_preamble_and_addr, sizeof(fs01_frame_preamble_and_addr));

    // 设置配对ID
    memset(pair_data.id, 0xF0, sizeof(pair_data.id));

    // 设置配对UNIQUE ID
    memcpy(&pair_data.unique_id, app.unique_id, sizeof(pair_data.unique_id));

    // 设置配对机型
    pair_data.model_type = app.modelType;

    // 计算校验和
    if (app.chipMode == A7535)
    {
        PairPayload_A7535 *a7535_pair_data = (PairPayload_A7535 *)&pair_data;
        a7535_pair_data->checksum = set_checksum(&a7535_pair_data->id, sizeof(PairPayload_A7535) - sizeof(fs01_frame_preamble_and_addr) - 1);
        rf_2_4g_sendto((uint8_t *)a7535_pair_data, sizeof(PairPayload_A7535));
    }
    else if (app.chipMode == XN297L)
    {
        pair_data.checksum = set_checksum(&pair_data.id, sizeof(PairPayload_XN297L) - sizeof(fs01_frame_preamble_and_addr) - 1);
        rf_2_4g_sendto((uint8_t *)&pair_data, sizeof(PairPayload_XN297L));
        rf_2_4g_entryPair();
    }
}