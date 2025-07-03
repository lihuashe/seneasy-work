/*******************************************************************************************
*                    Copyright (c) 2021, XIHAO Technology Co., Ltd.
* @file        driver_nec_code.h
* @author      Reborn    
* @version     V1.0    
* @date        2020-12-30
* @brief       红外发送处理文件
*********************************************************************************************/
#ifndef __DRIVER_NEC_CODE_H
#define __DRIVER_NEC_CODE_H

#include <stdint.h>
#include <stdbool.h>
#include "board_config.h"
#include "ir_base_config.h"

/*NEC 波形*/
#define NEC_WAVEFORM_LEDE_CODE0		9000
#define NEC_WAVEFORM_LEDE_CODE1		4500
#define NEC_WAVEFORM_DATA_1_H		560
#define NEC_WAVEFORM_DATA_1_L		1690
#define NEC_WAVEFORM_DATA_0_H		560
#define NEC_WAVEFORM_DATA_0_L		560
#define NEC_WAVEFORM_STOP_BIT		107000
#define NEC_SECOND_WAVEFORM_LEDE_CODE0		9000
#define NEC_SECOND_WAVEFORM_LEDE_CODE1		2250
#define NEC_SECOND_WAVEFORM_DATA_1_H		560
#define NEC_SECOND_WAVEFORM_STOP_BIT		96190

void ir_nec_decode(uint8_t add1, uint8_t add2, uint8_t cmd, IR_DATA_T *IR_Send_struct);

#endif //__DRIVER_NEC_CODE_H

