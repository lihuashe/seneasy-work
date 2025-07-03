#include "fct.h"

#include "pm.h"
#include "key_evt_handle.h"
#include "pan216_port.h"
#include "pan216_ext.h"
#include "key_driver.h"
#include "time_utils.h"
#include "led.h"
#include "utility.h"
#include "string.h"
#include "2_4g_io.h"
#include "flash_io.h"
#include "fct_uart.h"
#include "fct_protocol.h"
#define FCT_DEBUG 0

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
static u8 ext_buf[RING_BUFFER_SIZE];  // 缓冲区大小
static u8 resp_buf[RING_BUFFER_SIZE]; // 应答缓冲区
static u8 resp_buf_len;               // 应答缓冲区数据长度
static RingBuffer fct_rx_buf;
volatile static uint32_t exit_clock_t = 0;

static void key_test_cb(u8 key);
const static u8 led_mapper[] = {2, 5, 9, 11, 1, 4, 8, 12, 7, 14, 6, 15, 16, 10, 17, 18, 13, 19, 3};
const static KeyEvent click_key_cb_arr[] = {
    KEY_CLICK_EVENT_ITEM(KB_CODE_ANY, key_test_cb),

};

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief led控制回调实现
 *
 * @param fctCmd
 * @param[out] resp
 * @return fct_ret_t
 */
fct_ret_t fct_ledCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    if (fctCmd->cmdDataLen < 2)
    {
        return FCT_PARAMETER_ERR;
    }

    for (size_t i = 0; i < fctCmd->cmdDataLen / 2; i++)
    {
        if (fctCmd->pCmdData[i + 1])
            led_on(led_mapper[fctCmd->pCmdData[i]]);
        else
            led_off(led_mapper[fctCmd->pCmdData[i]]);
    }
    return FCT_OK;
}

/**
 * @brief mac回调实现
 *
 * @param fctCmd
 * @param[out] resp
 * @return fct_ret_t
 */
fct_ret_t fct_getMAC(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    memcpy(resp->pCmdData, (uint8_t *)FLASH_FACTORY_MAC_START_ADDR, 6);
    resp->cmdDataLen = 6;
    return FCT_OK;
}

fct_ret_t fct_exitFct(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    fct_uart_sendto((uint8_t *)g_respExitFctBuf, sizeof(g_respExitFctBuf));
    while (HAL_UART_GetState(&UartHandle) == HAL_UART_STATE_BUSY_TX)
    {
        HAL_Delay(1);
    }
    HAL_NVIC_SystemReset();
    while (1)
        ;
    return FCT_OK;
}

/**
 * @brief 载波控制回调实现
 *
 * @param fctCmd
 * @param[out] resp
 * @return fct_ret_t
 */
fct_ret_t fct_carrierWaveCtrl(const fct_cmd_t *fctCmd, fct_resp_t *resp)
{
    if (fctCmd->cmdDataLen < 3)
    {
        return FCT_PARAMETER_ERR;
    }

    if (fctCmd->pCmdData[0] != BLE_CARRIER)
    {
        return FCT_PARAMETER_ERR;
    }

    if (fctCmd->pCmdData[2])
        PAN216_TxCarrierWave();
    else
        PAN216_ExitCarrierWave();
    return FCT_OK;
}

void key_test_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    printf(">D%d[%s]\n", __LINE__, __FUNCTION__);
#endif
    fct_setKeyState(key - 1);
    exit_clock_t = clock_time();
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

bool try2entry_fct()
{
    kb_fast_scan(NULL);
    struct key_event keyEvt = {0};
    kb_get_key_event(&keyEvt);
    if (keyEvt.cnt == 2 &&
        key_code_is_exist(keyEvt.key_code, keyEvt.cnt, KB_CODE_S4) &&
        key_code_is_exist(keyEvt.key_code, keyEvt.cnt, KB_CODE_S6))
    {
        return true;
    }
#if FCT_DEBUG
    HAL_Delay(1000);
    return true;
#endif
    return false;
}

void fct_init()
{
    led_bar_all_on(&g_ledBarMode, 500);
    PAN216_SetUpConfig((RFConfig_t *)&a7535_gRfConfig);
    RingBuffer_Init(&fct_rx_buf, ext_buf, sizeof(ext_buf));

    key_evt_reset_all();
    set_click_key_map(click_key_cb_arr, ARRAY_SIZE(click_key_cb_arr));

    fct_uart_init(&fct_rx_buf);
}

void fct_proc()
{

    if (key_get_pressed_num())
        exit_clock_t = clock_time();

    if (clock_time_exceed(exit_clock_t, 5000))
    {
#if !FCT_DEBUG
        HAL_NVIC_SystemReset();
        while (1)
            ;
#endif
    }

    uint8_t _len = 0;
    uint8_t _buf[RING_BUFFER_SIZE];
    if (fct_uart_isRecv())
    {
        _len = RingBuffer_Read(&fct_rx_buf, _buf, sizeof(_buf));
        fct_ret_t ret = fct_cmd(_buf, _len, resp_buf, &resp_buf_len);
        if (ret < FCT_ACK_ERR_MAX)
        {
            fct_uart_sendto(resp_buf, resp_buf_len);
        }
        else
        {
#if __DEBUG__ >= __DEBUG_ERR__
            printf("\033[1;31m>E%d[%s] %d \n\033[0m", __LINE__, __FUNCTION__, ret);
#endif
        }
    }
}
