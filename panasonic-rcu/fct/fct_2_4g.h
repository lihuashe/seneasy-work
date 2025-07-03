
#ifndef FCT_FCT_2_4G_H_
#define FCT_FCT_2_4G_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "utility.h"
#include <stdint.h>

void fct_2_4g_init(RingBuffer *rbuf);
bool fct_2_4g_isRecv();
int8_t fct_2_4g_sendto(uint8_t *pData, uint8_t len);

void fct_2_4g_setChannel(uint8_t channel);
void fct_2_4g_setChannels(uint8_t *pArr, uint8_t len);
void fct_2_4g_setHoppingInterval(uint8_t interval_ms);
void fct_2_4g_setTxRetryMaxCnt(uint8_t cnt);
void fct_2_4g_setRetryTimeout(uint8_t timeout_ms);

void fct_2_4g_proc();

#ifdef __cplusplus
}
#endif

#endif  // FCT_FCT_2_4G_H_
