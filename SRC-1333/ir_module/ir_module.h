#ifndef __IR_MODULE_H
#define __IR_MODULE_H

#include "stdint.h"
#include "stdbool.h"
#include "uart.h"
#include "ir_driver.h"

void ir_protocol_init();

void ir_protocol_config_nec();

bool ir_protocol_send_nec(uint8_t address1, uint8_t address2, uint8_t key);

bool ir_protocol_send_nec_raw(uint8_t len);

bool ir_protocol_send_wave(uint8_t bytes);

bool ir_protocol_set_wave_frames(uint8_t cnt, uint16_t delay);

#define ir_protocol_stop ir_stop_send

#endif
