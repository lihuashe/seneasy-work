/*******************************************************************************************
*                    Copyright (c) 2021, XIHAO Technology Co., Ltd.
* @file        driver_nec_code.c
* @author      Reborn    
* @version     V1.0    
* @date        2020-12-30
* @brief       红外发送处理文件
*********************************************************************************************/

#include "driver_nec_code.h"
#include "peripheral.h"
#include "co_tools.h"
#include "app_ir.h"


#if (REMOTE_IR_ENABLE)

 /**
 * @brief        nec解码
 * @param[in]    addr1--客户码
 * @param[in]    addr1--数据码
 * @param[out]   ir_data--解码数据
 */
void ir_nec_decode(uint8_t addr1, uint8_t addr2, uint8_t cmd, IR_DATA_T *ir_data)
{
    uint32_t stop_bit = 0;
	
	memset(ir_data,0,sizeof(IR_DATA_T));
	
	//lead code
    levle_value_package_handle(HIGH_LEVEL_STATUS,FIRST_FRAME_DATA_LEVEL,NEC_WAVEFORM_LEDE_CODE0,ir_data);
    levle_value_package_handle(LOW_LEVEL_STATUS,FIRST_FRAME_DATA_LEVEL,NEC_WAVEFORM_LEDE_CODE1,ir_data);

	uint32_t code_data = (~cmd)<<24 | cmd<<16 | addr2<<8 | addr1;

	//data code
	for(int i=0;i<32;i++){
		if(code_data & BIT(i)){
			//waveform for logic_1
			levle_value_package_handle(HIGH_LEVEL_STATUS,FIRST_FRAME_DATA_LEVEL,NEC_WAVEFORM_DATA_1_H,ir_data);
			levle_value_package_handle(LOW_LEVEL_STATUS,FIRST_FRAME_DATA_LEVEL,NEC_WAVEFORM_DATA_1_L,ir_data);
		}
		else{
			//waveform for logic_0
			levle_value_package_handle(HIGH_LEVEL_STATUS,FIRST_FRAME_DATA_LEVEL,NEC_WAVEFORM_DATA_0_H,ir_data);
			levle_value_package_handle(LOW_LEVEL_STATUS,FIRST_FRAME_DATA_LEVEL,NEC_WAVEFORM_DATA_0_L,ir_data);
		}
	}

	//first frame stop bit
	stop_bit = NEC_WAVEFORM_STOP_BIT - ir_data->ir_first_frame_time;
    levle_value_package_handle(HIGH_LEVEL_STATUS,FIRST_FRAME_DATA_LEVEL,NEC_WAVEFORM_DATA_0_H,ir_data);
	
	if (stop_bit > 0)
	{
		levle_value_package_handle(LOW_LEVEL_STATUS,FIRST_FRAME_STOP_LEVEL,stop_bit,ir_data);
	}
	
	//second frame data
	levle_value_package_handle(HIGH_LEVEL_STATUS,SECOND_FRAME_DATA_LEVEL,NEC_SECOND_WAVEFORM_LEDE_CODE0,ir_data);
	levle_value_package_handle(LOW_LEVEL_STATUS,SECOND_FRAME_DATA_LEVEL,NEC_SECOND_WAVEFORM_LEDE_CODE1,ir_data);
	levle_value_package_handle(HIGH_LEVEL_STATUS,SECOND_FRAME_DATA_LEVEL,NEC_SECOND_WAVEFORM_DATA_1_H,ir_data);
	
	levle_value_package_handle(LOW_LEVEL_STATUS,SECOND_FRAME_STOP_LEVEL,NEC_SECOND_WAVEFORM_STOP_BIT,ir_data);
	
	ir_data->repeat_type = SECOND_FRAME_REPEAT;
	ir_data->ir_carrier_fre = 38000;
}

#endif

