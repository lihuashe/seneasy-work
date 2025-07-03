/**
 * @file 2_4g_io.h
 * @author likeliang (likeliang@seneasy.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "py32f0xx_hal.h"
#include "types.h"
#ifndef APP_DATA_STREAM_2_4G_IO_H_
#define APP_DATA_STREAM_2_4G_IO_H_

#ifdef __cplusplus
extern "C" {
#endif


#define A7535_CHANNEL                      78


#define XN297L_CHANNEL_TABLE               {35, 45, 55}
#define XN297L_TX_CNT                       12
#define XN297L_CTRL_ADDR    0xC4, 0xCC, 0xCC, 0xCC, 0xC1
#define XN297L_PAIR_ADDR    0xA1, 0xCC, 0xCC, 0xCC, 0xC1

enum{
    RF_IO_STATUS_IDEL,
    RF_IO_STATUS_SEND_1st_FRAME,
    RF_IO_STATUS_SEND_2nd_FRAME,
};

    typedef struct
    {
        uint8_t data[32];
        uint8_t len;
    } rf_io_buf_t;

extern const RFConfig_t a7535_gRfConfig;
extern const RFConfig_t xn297l_gRfConfig;
extern bool g_xn297l_pair_f;
extern TIM_HandleTypeDef  rf_2_4g_TimHandle;

void rf_2_4g_init();
void rf_2_4g_proc();
void rf_2_4g_sendto(uint8_t *data, uint8_t len);
uint8_t rf_2_4g_read(uint8_t *data, uint8_t len);
void rf_2_4g_entryPair();
bool rf_2_4g_is_busy();
void rf_2_4g_tim17_irq();

#ifdef __cplusplus
}
#endif

#endif  // APP_DATA_STREAM_2_4G_IO_H_
