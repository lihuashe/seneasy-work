
#ifndef FCT_FCT_UART_H_
#define FCT_FCT_UART_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "utility.h"
#include <stdint.h>

extern UART_HandleTypeDef UartHandle;

void fct_uart_init(RingBuffer *rbuf);
bool fct_uart_isRecv();
HAL_StatusTypeDef fct_uart_sendto(uint8_t *pData, uint8_t len);


#ifdef __cplusplus
}
#endif

#endif  // FCT_FCT_UART_H_
