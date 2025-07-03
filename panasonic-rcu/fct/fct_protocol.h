
#ifndef FCT_FCT_PROTOCOL_H_
#define FCT_FCT_PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __weak
#define __weak __attribute__((weak))
#endif /* __weak */
#include "types.h"

    /*============================================================================*
     *                              Fct Configuration
     *============================================================================*/

#define FCT_URAT_MODE 0
#define FCT_BLE_MODE 1
#define FCT_LINK_MODE FCT_URAT_MODE

#define FCT_PROTOCOL_VERSION "V1.0.0"

    typedef enum RetVal
    {
        FCT_OK,
        FCT_CMD_INVALID,    // 无效命令
        FCT_PARAMETER_ERR,  // 命令数据错误
        FCT_DID_INVALID,    // 设备ID错误
        FCT_NOT_AUTH,       // 没有权限
        FCT_CMD_NO_SUPPORT, // 命令不支持

        FCT_ACK_ERR_MAX, // 应答上位机的错误与通信错误分界

        FCT_LEN_ERR,       // 长度错误
        FCT_SYNC_ERR,      // 同步字节0xFF
        FCT_HEAD_ERR,      // 包头错误0x55
        FCT_CHECK_SUM_ERR, // 校验错误
        FCT_UNKNOWN_ERR    // 未知错误
    } fct_ret_t;

    typedef enum CmdID
    {
        FCT_CMD_RESERVED = 0,
        FCT_CMD_BLE_UG_CONN_PARAM = 0x01,      // BLE更新连接参数
        FCT_CMD_PROTOCOL_VERSION_QUERY = 0x10, // 协议版本查询
        FCT_CMD_SOFTWARE_VERSION_QUERY,        // 软件版本查询
        FCT_CMD_HARDWARE_VERSION_QUERY,        // 硬件版本查询
        FCT_CMD_PRODUCT_NAME_QUERY,            // 产品名称查询
        FCT_CMD_SN_QUERY,                      // SN查询
        FCT_CMD_CERT_QUERY,                    // CERT查询
        FCT_CMD_CTEI_QUERY,                    // CTEI查询
        FCT_CMD_MAC_QUERY,                     // MAC查询
        FCT_CMD_WRITE_SN,                      // 写SN
        FCT_CMD_WRITE_CERT,                    // 写CERT
        FCT_CMD_WRITE_CTEI,                    // 写CTEI
        FCT_CMD_KEY_COUNT_QUERY,               // 按键数量查询
        FCT_CMD_FACTORY_MODE,                  // 工厂模式
        FCT_CMD_BATTERY_LEVEL_QUERY = 0x30,    // 读电池电量
        FCT_CMD_KEY_SELF_CHECK,                // 按键自检
        FCT_CMD_CARRIER_WAVE_CTRL = 0x60,      // 单载波模式
        FCT_CMD_VOICE_CONTROL,                 // 语音控制
        FCT_CMD_EXIT_FACTORY_MODE,             // 产测退出|清配|立即休眠
        FCT_CMD_G_SENSOR_CTRL = 0x64,          // 控制G-sensor
        FCT_CMD_LED_CTRL,                      // 控制LED
        FCT_CMD_MOTOR_CTRL,                    // 控制电机
        FCT_CMD_BUZZER_CTRL,                   // 控制蜂鸣器
        FCT_CMD_IR_CTRL,                       // 控制红外
    } fct_cmd_id_t;

    enum CWaveCtrlParam
    {
        BLE_CARRIER,
        RF_433_CARRIER,
    };

    enum GSensorCtrlParam
    {
        DATA_CMD
    };

    typedef struct Cmd
    {
        u8 did;
        u8 cmd;
        u8 *pCmdData;
        u8 cmdDataLen;
    } fct_cmd_t, fct_resp_t;

    typedef struct  PacketCmd
    {
#if FCT_LINK_MODE == FCT_URAT_MODE
        u8 urat_fix;
#endif
        u8 head;
        u8 packet_len;
        u8 did;
        u8 cmd;
        u8 cmd_data[0];
    } __attribute__((packed)) fct_cmd_packet_t;

    typedef struct PacketResp
    {
#if FCT_LINK_MODE == FCT_URAT_MODE
        u8 urat_fix;
#endif
        u8 head;
        u8 packet_led;
        u8 did;
        u8 ack;
        u8 ret_state;
        u8 cmd;
        u8 resp_data[0];
    } __attribute__((packed)) fct_resp_packet_t;

    fct_ret_t fct_cmd(const u8 *pData, u8 len, u8 *resp, u8 *resp_len);
    void fct_setKeyState(u8 key);
#if FCT_LINK_MODE == FCT_URAT_MODE
    extern const u8 g_respExitFctBuf[8];
#else
extern const u8 g_respExitFctBuf[7];
#endif

#ifdef __cplusplus
}
#endif

#endif // FCT_FCT_PROTOCOL_H_
