// Project: TEST_64F0Ax_TIM1_PWM.prj
// Device:  FT64F0AX
// Memory:  PROM=10Kx14, SRAM=1KB, EEPROM=128
// Description: TIM1 CH1互补输出周期为32kHz的方波
//
// RELEASE HISTORY
// VERSION DATE     DESCRIPTION
// 1.3     24-1-30  更新文件头描述
//
//                FT64F0AX  TSSOP20
//              -------------------
// NC----------|1(PA5)   	(PA4)20|-----------NC
// NC----------|2(PA6)   	(PA3)19|-----------NC
// NC----------|3(PA7)   	(PA2)18|-----------NC
// TIM1_CH1N---|4(PC0)   	(PA1)17|-----------NC
// NC----------|5(PC1)		(PA0)16|-----TIM1_CH1
// NC----------|6(PB7)		(PB0)15|-----------NC
// GND---------|7(GND)		(PB1)14|-----------NC
// NC----------|8(PB6)		(PB2)13|-----------NC
// VDD---------|9(VDD)		(PB3)12|-----------NC
// NC----------|10(PB5)		(PB4)11|-----------NC
//				-------------------
//
//*********************************************************

#include "ir_module.h"

const ir_waveform_spec_t ir_nec_waveform_spec[10] = {
    { 560,   560   },        // [0] = logic 0
    { 560,   1690  },        // [1] = logci 1
    { 9000,  4500  },        // [2] = leading header
    { 560,   40500 },        // [3] = stop
    { 9000,  2250  },        // [4] = leading repeat
};

bool ir_protocol_send_nec(uint8_t address1,uint8_t address2, uint8_t key)
{
    if (ir_data.state == IR_STATE_REPEAT && ir_change_key == IR_CHANGE_IDLE)
    {
        ir_change_key = IR_CHANGE_COMMAND;
        ir_protocol_stop();
        return 0;
    }
    if (ir_data.state != IR_STATE_IDLE || ir_code != IR_CODE_NEC) 
    {
        return 1;
    }

    ir_data.data[0] = address1;
    ir_data.data[1] = address2;
    ir_data.data[2] = key;
    ir_data.data[3] = ~key;

    ir_data.state = IR_STATE_COMMAND;
    ir_data.pending_stop = false;

    ir_data.command_idx = 0;
    ir_data.data_idx = 0;
    ir_data.repeat_idx = 0;

    ir_start_send();

    return 0;
}

bool ir_protocol_send_ne c_raw(uint8_t len)
{
    if (ir_data.state != IR_STATE_IDLE || ir_code != IR_CODE_NEC) 
    {
        return 1;
    }

    ir_data.data[0] = rx_data[4];
    ir_data.data[1] = rx_data[5];
    ir_data.data[2] = rx_data[6];
    ir_data.data[3] = rx_data[7];

    ir_data.state = IR_STATE_COMMAND;
    ir_data.pending_stop = false;

    ir_data.command_idx = 0;
    ir_data.data_idx = 0;
    ir_data.repeat_idx = 0;

    ir_start_send();

    return 0;
}

void ir_protocol_config_nec()
{
    ir_data.carry_frequcy_hz = 38000;

    // ir_data.waveform_spec[0].h = ir_nec_waveform_spec[0].h;
    // ir_data.waveform_spec[0].l = ir_nec_waveform_spec[0].l;

    // ir_data.waveform_spec[1].h = ir_nec_waveform_spec[1].h;
    // ir_data.waveform_spec[1].l = ir_nec_waveform_spec[1].l;

    // ir_data.waveform_spec[2].h = ir_nec_waveform_spec[2].h;
    // ir_data.waveform_spec[2].l = ir_nec_waveform_spec[2].l;

    // ir_data.waveform_spec[3].h = ir_nec_waveform_spec[3].h;
    // ir_data.waveform_spec[3].l = ir_nec_waveform_spec[3].l;

    // ir_data.waveform_spec[4].h = ir_nec_waveform_spec[4].h;
    // ir_data.waveform_spec[4].l = ir_nec_waveform_spec[4].l;

    // ir_data.waveform_spec[5].h = ir_nec_waveform_spec[5].h;
    // ir_data.waveform_spec[5].l = ir_nec_waveform_spec[5].l;
    ir_data.waveform_spec[0] = 560/2;
    ir_data.waveform_spec[1] = 560/2;

    ir_data.waveform_spec[2] = 560/2;
    ir_data.waveform_spec[3] = 1690/2;

    ir_data.waveform_spec[4] = 8900/2;
    ir_data.waveform_spec[5] = 4400/2;

    ir_data.waveform_spec[6] = 560/2;
    ir_data.waveform_spec[7] = 40500/2;

    ir_data.waveform_spec[8] = 8900/2;
    ir_data.waveform_spec[9] = 2250/2;

    ir_data.waveform_spec[10] = 560/2;
    ir_data.waveform_spec[11] = 96190/2;

    ir_data.command[0] = 0x02; // leading header
    ir_data.command[1] = 0x80; // [data]
    ir_data.command[2] = 0x03; // stop
    ir_data.command_len = 3;
    ir_data.command_idx = 0;

    ir_data.data[0] = 0;
    ir_data.data[1] = 0;
    ir_data.data[2] = 0;
    ir_data.data[3] = 0;
    ir_data.data_idx = 0;
    ir_data.data_len = 32;

    ir_data.repeat[0] = 0x04;   // repeat
    ir_data.repeat[1] = 0x05;   // stop
    ir_data.repeat_len = 2;
    ir_data.repeat_idx = 0;

    ir_code = IR_CODE_NEC;

    ir_config_carrier(ir_data.carry_frequcy_hz);
    ir_config_timer(500);
}

inline void set_nibble_in_buffer(uint8_t * pbuff, uint8_t nibble_value, uint8_t pos)
{
    if (pos % 2 == 0) 
    {
        pbuff[pos / 2] = (pbuff[pos / 2] & 0x0F) | ((nibble_value & 0xF0));
    } else 
    {
        pbuff[pos / 2] = (pbuff[pos / 2] & 0xF0) | (nibble_value >> 4 & 0x0F);
    }
}

/**
 * @brief   ir_protocal_config_wave
 * 
 * | byte    | type        | sample value | comment    | 
 * | 0       | flag        | a6           |            |
 * | 1       | dict_len    | 07           |            |
 * | 2-3     | dict[0]     | 15 00        | LSB ->MSB
 * | 4-5     | dict[1]     | 40 00        |        
 *                         | 56 01        |
 *                         | ab 00        |
 *                         | f7 05        |
 *                         | 56 00        |
 * | 2n:2n+1 | dict[n]     | 47 0e        |
 * | 2n+2    | wave_len    | 48           |
 * | 2n+3... | [wave]      | 23 01 0f 81 0f 61 01 01 01 00 01 01 01 00 01 01 0f a1 0f 41 01 01 01 04 25 06
 * 
*/
bool ir_protocol_send_wave(uint8_t bytes)
{   
    uint8_t i, len, offset;  
    uint8_t * p;

    uint8_t nibble,w,c;
    uint8_t number;
    bool escape_to_number = false;
    uint8_t escape_to_pulse = 0;

    if (ir_wave.state != IR_STATE_IDLE || ir_data.state != IR_STATE_IDLE) 
    {
        return 1;
    }

    // 暂不支持非压缩波形
    if (rx_data[4] & 0x80 == 0)
    {
        return 1;
    }

    offset = 4;

    // 配置定时器时基
    ir_wave.flags = rx_data[offset++];
    ir_config_timer((uint8_t)(ir_wave.flags & 0x7F));

    // 波形字典
    len = rx_data[offset++];
    ir_wave.dict_len = len;
    for ( i = 0; i < len; i++)
    {
        p = (uint8_t *)&ir_wave.dict[i];
        *p = rx_data[offset++];
        p++;
        *p = rx_data[offset++];
    }

    // 获取波形长度
    len = rx_data[offset++];     // 波形脉冲个数, 一个logic包含一个高脉冲和一个低脉冲
    ir_wave.command_len = len;

    // 解码波形数据
    ir_wave.command_idx = 0;
    ir_wave.pulse_idx = 0;
    for (; offset < bytes; offset++)
    {
        w = rx_data[offset];
        
        for (i = 0; i < 2; i++) 
        {
            nibble = w & 0xF0;

            // 转化为原始波形
            if (escape_to_pulse != 0) 
            {
                ir_wave.pulse[(uint8_t)ir_wave.pulse_idx] <<= 4;
                nibble = nibble >> 4 & 0x0F;
                ir_wave.pulse[(uint8_t)ir_wave.pulse_idx] = ir_wave.pulse[(uint8_t)ir_wave.pulse_idx] | nibble;
                escape_to_pulse--;
                if (escape_to_pulse == 0)
                {
                    // 波形序列里面放置一个f,代表原始波形,长度=puluse[pulse_idx]
                    set_nibble_in_buffer(ir_wave.command, 0xF0, ir_wave.command_idx++);
                    ir_wave.pulse_idx++;
                }
            }
            else
            {
                // 非转义字符
                if ((nibble != 0xF0))
                {
                    // 转义为重复数量
                    if (escape_to_number)
                    {
                        number = nibble >> 4  & 0x0F;
                        escape_to_number = false;
                    }
                    // 非转义符号
                    else
                    {
                        number = 1;
                        c = nibble;
                    }

                    // 存入number个符号到buff
                    do {
                        set_nibble_in_buffer(ir_wave.command, c, ir_wave.command_idx++);
                        number--;
                    } while (number > 0);

                }
                else
                {
                    // 连续的ff,取接下来的4个nibble为原始16bit脉冲长度
                    if (escape_to_number)
                    {
                        ir_wave.pulse[(uint8_t)ir_wave.pulse_idx] = 0;
                        escape_to_pulse = 4;
                        escape_to_number = false;
                    }
                    // 否则转义为数量
                    else
                    { 
                        escape_to_number = true;
                    }
                }
            }
            w = w << 4;
        }
    }

    // 启动发码
    ir_wave.state = IR_STATE_COMMAND;
    ir_code = IR_CODE_UNIVERSAL;
    ir_wave.pulse_idx = 0;
    ir_wave.command_idx = 0;
    ir_start_send();

    return 0;
}

bool ir_protocol_set_wave_frames(uint8_t cnt, uint16_t delay)
{
    if (ir_wave.state != IR_STATE_IDLE)
    {
        return 1;
    }

    ir_wave.frames = cnt;
    ir_wave.delay = delay;

    return 0;
}

void ir_protocol_init()
{
    ir_data.state = IR_STATE_IDLE;
    ir_wave.state = IR_STATE_IDLE;
    ir_code = IR_CODE_NEC;

    ir_protocol_config_nec();
}