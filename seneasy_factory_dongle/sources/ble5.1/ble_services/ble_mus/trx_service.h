/**
****************************************************************************
* @file mxdTxRxService.h
* @brief
 * @author   Maxscend SoC Software Team
* @version Initial Draft
* @date 2016/11/15
* @history
* @note
* detailed description

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
/** @defgroup  profile  serverTxRx
* @{
*/
#ifndef __MXDTXRXSERVICE_H__
#define __MXDTXRXSERVICE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

    
#define MXD_TX_RX_SERVICE_UUID              0xFFF0  //
#define RX_CUS_UUID                         0xFFF2  //
#define TX_CUS_UUID                         0xFFF1  //
#define MXD_TRX_CMD_UUID                    0xFFF3  //
#define MXD_TRX_READ_FOR_ENCPT_UUID                 0xFFF4  //
#define MXD_TRX_BLOBREAD_PREPWRITE_TEST_UUID        0xFFF6  //
#define MXD_TRX_BLOBREAD_PREPWRITE_TEST_UUID2        0xFFFA  //
#define MXD_TRX_INDICATE_UUID                       0xFFF7  //
#define MXD_TRX_NOTIFY_TEST_UUID            0xFFF8
#define MXD_TRX_W_TEST_UUID                 0xFFF9

extern uint16_t txrx_server_receive( uint16_t conn_handle, uint8_t *data, uint16_t length);
extern uint32_t txrx_server_transfer( uint16_t conn_handle, uint8_t *data, uint32_t length);
extern uint16_t  trx_get_attrLenCB( uint16_t conn_handle, uint16_t attribute_handle);
extern uint16_t  trx_read_attrCB( uint16_t conn_handle, uint16_t attribute_handle, uint16_t offset, uint8_t  * buffer, uint16_t  buffer_size);
extern uint32_t trx_write_attrCB( uint16_t conn_handle, uint16_t attribute_handle, uint16_t  transaction_mode, uint16_t offset, uint8_t  *buffer, uint16_t buffer_size);
extern uint32_t trx_init_service( void );
extern uint16_t txrx_cmd_receive( uint16_t conn_handle, uint8_t *data, uint16_t length);

typedef enum TXRX_SRV_HANDLE
{
    TX_WRITE            = 0,
    RX_NOTIFY           = 1,
    RX_NOTIFY_CONFIG    = 2,
    TXRX_CMD            = 3,
    TXRX_CMD_CONFIG        = 4,
    TXRX_READ_FOR_ENCPT    = 5,

} TXRX_SRV_HANDLE_ENUM;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MXDTXRXSERVICE_H__ */
/** @} */ // 模块结尾,自行拷贝到末尾


