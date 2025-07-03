
#ifndef FCT_FCT_H_
#define FCT_FCT_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>

#define _CMD_DATA_LEN_INDEX 0

#define APP_VERSION

    typedef enum
    {
        FCT_CMD_PAIR_INDEX = 0,
        FCT_CMD_SET_CHANNEL_INDEX,
        FCT_CMD_APP_VERSION_INDEX,
        FCT_CMD_GET_MAC_INDEX,
        FCT_CMD_LED_CTRL_INDEX,
        FCT_CMD_CARRIER_WAVE_CTRL_INDEX, 
        FCT_CMD_GET_ALL_INFO_INDEX, 
        FCT_CMD_KEY_CTRL_INDEX, 
    } FCT_CmdIndex;

bool try2entry_fct();
void fct_init();
void fct_proc();
void uart_rx_irq();
#ifdef __cplusplus
}
#endif

#endif  // FCT_FCT_H_
