#include "fct_protocol.h"
#include <string.h>
#include "utility.h"
#include "fct_static_info.h"
#include "my_log.h"
/*============================================================================*
 *                              Local Variables
 *============================================================================*/

// MSB 7-0,16-8,24-17,32-18 LSB
static u8 keyState[KEY_NUM % 8 ? KEY_NUM / 8 : KEY_NUM / 8 + 1];
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

#if FCT_LINK_MODE == FCT_URAT_MODE
const u8 g_respExitFctBuf[] = {0xFF, 0x55, 0x04, 0x00, 0x02, 0x00, 0x62, 0x68};
#else
const u8 g_respExitFctBuf[] = {0x55, 0x04, 0x00, 0x02, 0x00, 0x62, 0x68};
#endif

/*============================================================================*
 *                              Weak Functions
 *============================================================================*/

__weak fct_ret_t fct_updateConnParameter(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_getSN(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_getCERT(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_getCTEI(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_getMAC(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_setSN(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_setCERT(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_setCTEI(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_entryFactoryMode(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_getBatteryVal(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_carrierWaveCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_voiceCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_exitFct(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_gSensorCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_ledCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_motorCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_buzzerCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

__weak fct_ret_t fct_irCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    /* NOTE : This function should not be modified, when the callback is needed,
        could be implemented in the user file */
    return FCT_CMD_NO_SUPPORT;
}

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

static fct_ret_t __cmd(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    fct_ret_t ret = FCT_OK;
    switch (fctCmd->cmd)
    {
    case FCT_CMD_BLE_UG_CONN_PARAM:
        ret = fct_updateConnParameter(fctCmd, resp);
        break;
    case FCT_CMD_PROTOCOL_VERSION_QUERY:
        memcpy(resp->pCmdData, FCT_PROTOCOL_VERSION, sizeof(FCT_PROTOCOL_VERSION));
        resp->cmdDataLen = sizeof(FCT_PROTOCOL_VERSION) - 1;
        ret = FCT_OK;
        break;
    case FCT_CMD_SOFTWARE_VERSION_QUERY:
        memcpy(resp->pCmdData, FW_VERSION_STR, sizeof(FW_VERSION_STR));
        resp->cmdDataLen = sizeof(FW_VERSION_STR) - 1;
        ret = FCT_OK;
        break;
    case FCT_CMD_HARDWARE_VERSION_QUERY:
        memcpy(resp->pCmdData, HW_VERSION_STR, sizeof(HW_VERSION_STR));
        resp->cmdDataLen = sizeof(HW_VERSION_STR) - 1;
        ret = FCT_OK;
        break;
    case FCT_CMD_PRODUCT_NAME_QUERY:
        memcpy(resp->pCmdData, PRODUCT_NAME_STR, sizeof(PRODUCT_NAME_STR));
        resp->cmdDataLen = sizeof(PRODUCT_NAME_STR) - 1;
        ret = FCT_OK;
        break;
    case FCT_CMD_SN_QUERY:
        ret = fct_getSN(fctCmd, resp);
        break;
    case FCT_CMD_CERT_QUERY:
        ret = fct_getCERT(fctCmd, resp);
        break;
    case FCT_CMD_CTEI_QUERY:
        ret = fct_getCTEI(fctCmd, resp);
        break;
    case FCT_CMD_MAC_QUERY:
        ret = fct_getMAC(fctCmd, resp);
        break;
    case FCT_CMD_WRITE_SN:
        ret = fct_setSN(fctCmd, resp);
        break;
    case FCT_CMD_WRITE_CERT:
        ret = fct_setCERT(fctCmd, resp);
        break;
    case FCT_CMD_WRITE_CTEI:
        ret = fct_setCTEI(fctCmd, resp);
        break;
    case FCT_CMD_KEY_COUNT_QUERY:
        resp->pCmdData[0] = KEY_NUM;
        resp->cmdDataLen = 1;
        break;
    case FCT_CMD_FACTORY_MODE:
        ret = fct_entryFactoryMode(fctCmd, resp);
        break;
    case FCT_CMD_BATTERY_LEVEL_QUERY:
        ret = fct_getBatteryVal(fctCmd, resp);
        break;
    case FCT_CMD_KEY_SELF_CHECK:
        memcpy(resp->pCmdData, keyState, sizeof(keyState));
        resp->cmdDataLen = sizeof(keyState);
        break;
    case FCT_CMD_CARRIER_WAVE_CTRL:
        ret = fct_carrierWaveCtrl(fctCmd, resp);
        break;
    case FCT_CMD_VOICE_CONTROL:
        ret = fct_voiceCtrl(fctCmd, resp);
        break;
    case FCT_CMD_EXIT_FACTORY_MODE:
        ret = fct_exitFct(fctCmd, resp);
        break;
    case FCT_CMD_G_SENSOR_CTRL:
        ret = fct_gSensorCtrl(fctCmd, resp);
        break;
    case FCT_CMD_LED_CTRL:
        ret = fct_ledCtrl(fctCmd, resp);
        break;
    case FCT_CMD_MOTOR_CTRL:
        ret = fct_motorCtrl(fctCmd, resp);
        break;
    case FCT_CMD_BUZZER_CTRL:
        ret = fct_buzzerCtrl(fctCmd, resp);
        break;
    case FCT_CMD_IR_CTRL:
        ret = fct_irCtrl(fctCmd, resp);
        break;

    default:
        ret = FCT_CMD_INVALID;
        break;
    }
    return ret;
}
/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/**
 * @brief
 *
 * @param pData 从通信接口接收到的数据
 * @param len 数据长度
 * @param[out] resp 应答数据
 * @param[out] resp_len 应答数据长度
 * @return fct_ret_t 大于FCT_ACK_ERR_MAX时，通信数据错误，此时无应答数据resp返回
 */
fct_ret_t fct_cmd(const u8 *pData, u8 len, u8 *resp, u8 *resp_len)
{
#if 1
    printf("rx: ");
    for (size_t i = 0; i < len; i++)
    {
        printf("%02X ", pData[i]);
    }
    printf("\n");
#endif
    *resp_len = 0;
    fct_cmd_packet_t *cmdPacket = (fct_cmd_packet_t *)pData;
    if (len < 6 || cmdPacket->packet_len < 2)
    {
        return FCT_LEN_ERR;
    }

#if FCT_LINK_MODE == FCT_URAT_MODE
    if (cmdPacket->urat_fix != 0xFF) // Sync byte (UART通信强制要求)
    {
        return FCT_SYNC_ERR;
    }
#elif FCT_LINK_MODE == FCT_BLE_MODE

#endif

    if (cmdPacket->head != 0x55) // Packet head
    {
        return FCT_HEAD_ERR;
    }
    u8 checkSum = get_checksum((void *)&cmdPacket->packet_len, cmdPacket->packet_len + 1);
    if (checkSum != cmdPacket->cmd_data[cmdPacket->packet_len - 2])
    {
        return FCT_CHECK_SUM_ERR;
    }

    fct_cmd_t cmd = {0};
    fct_resp_t fct_resp = {0};
    fct_resp_packet_t *respPacket = (fct_resp_packet_t *)resp;

    cmd.cmdDataLen = cmdPacket->packet_len - 2;
    cmd.did = cmdPacket->did;
    cmd.cmd = cmdPacket->cmd;
    cmd.pCmdData = (u8 *)cmdPacket->cmd_data;

    fct_resp.did = cmdPacket->did;
    fct_resp.cmd = cmdPacket->cmd;
    fct_resp.pCmdData = respPacket->resp_data;

    fct_ret_t ret = __cmd((fct_cmd_t *)&cmd, &fct_resp);
    
#if FCT_LINK_MODE == FCT_URAT_MODE
    respPacket->urat_fix = 0xFF; // Sync byte (UART通信强制要求)
    (*resp_len)++;
#endif
    respPacket->head = 0x55;                          // Packet head
    respPacket->packet_led = fct_resp.cmdDataLen + 4; // Packet load length
    respPacket->did = fct_resp.did;                   // Device id： 通用设备
    respPacket->ack = 2;                              // 命令类型： ACK
    respPacket->ret_state = ret;                      // ACK状态代码
    respPacket->cmd = fct_resp.cmd;                   // 所属的命令ID
    respPacket->resp_data[fct_resp.cmdDataLen] = get_checksum((void *)&respPacket->packet_led, respPacket->packet_led + 1);
    (*resp_len) += respPacket->packet_led + 3;

    return ret;
}

/**
 * @brief
 *
 * @param key 按键键值，0开始计数
 */
void fct_setKeyState(u8 key)
{
    // MSB 7-0,16-8,24-17,32-18 LSB
    keyState[sizeof(keyState) - 1 - key / 8] |= 1 << (key % 8);
}
