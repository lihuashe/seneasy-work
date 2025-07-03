#include "peripheral.h"

#include "app_uart.h"

#if REMOTE_UART_ENABLE

void app_uart_init(void)
{
	pinmux_config(PIN_UART1_TX,  PINMUX_UART1_SDA_O_CFG);
//  pinmux_config(PIN_UART1_RX,  PINMUX_UART1_SDA_I_CFG);
 pmu_pin_mode_set(BIT_MASK(PIN_UART1_TX), PMU_PIN_MODE_PP);
//  pmu_pin_mode_set(BIT_MASK(PIN_UART1_RX), PMU_PIN_MODE_PU);
	
	uart_open(DEBUG_UART, DEBUG_UART_BAUDRATE, UART_FLOW_CTRL_DISABLED, NULL);
}

#endif
