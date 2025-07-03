/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     lp4081h_driver.c
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-20
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <board.h>
#include <string.h>
#include <adc_lib.h>
#include <rtl876x_rcc.h>
#include <rtl876x_adc.h>
#include <rtl876x_nvic.h>
#include <os_timer.h>
#include <swtimer.h>
#include <trace.h>
#include "lp4081h_driver.h"
#include "bas.h"
#include "rcu_application.h"
#include "rcu_gap.h"
#include "app_task.h"
#include "app_section.h"
#include "common_macros.h"

/*============================================================================*
 *                              Must Functions 
 *============================================================================*/
void lp4081h_init(void)
{
    PRINT("lp4081h_init !!!\r\n");

	i2c_init();

}

/**
 * @brief 写入一个字节的数据
 * @param data 
 */
void lp4081h_write_byte(uint8_t *data, uint8_t data_len)
{  
    //uint8_t status = 0;
    
	i2c_write_byte(LP4081H_ADDR, data, data_len);
    //PRINT("status:%d data:%d\n",status, *data);
}


/**
 * @brief 读取一个字节的数据
 * @param data 
 * @return 
 */
uint8_t lp4081h_read_one_byte(uint8_t reg_addr)
{
    uint8_t reg_data;
    //uint8_t status = 0;
    //status = i2c_read_byte(LP4081H_ADDR, reg_addr, &reg_data);
    //PRINT("status:%d reg_addr:0x%1x data:0x%1x\n",status, reg_addr, reg_data);

    i2c_read_byte(LP4081H_ADDR, reg_addr, &reg_data);

    return reg_data;
}


/**
 * @brief 设置从设备地址
 * @param  
 */
void lp4081h_addr_set_send(void)
{
    uint8_t addr = LP4081H_ADDR;
    lp4081h_write_byte(&addr, 1);
}

/*============================================================================*
 *                              Must Variables
 *============================================================================*/

LP4081H_Config_t lp4081h_config = {

    .BatFetMode = LP4081H_AUTO_MODE,
    .BatManagement = LP4081H_EN_NTC | LP4081H_EN_EOC_CONTINUE,
    .BatVsysDpm = LP4081H_VsysDpm_4_0V,
    .ChgCurLimit = LP4081H_CurLimit_650mA,
    .ChgTrickle = LP4081H_Iter_15mA,
    .ChgTriVolLimit = LP4081H_TriVolLimit_3_0V,
    .ConstChgCur = LP4081H_ConstCur_550mA,
    .ConstChgVol = LP4081H_BAT_CV_4_2V,
    .DischargeCurLimit = LP4081H_DisCurLimit_440mA,
    .DoubleCurMode = true,
    .IntMaskSet = 0,

};


/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief 这是一个向寄存器写入数据的函数
 * @param reg_addr 寄存器地址
 * @param data 要写入的一个字节的数据
 */
void lp4081h_write_reg_and_data(uint8_t reg_addr, uint8_t data)
{
#if NEED_SEND_LP4081H_ADDR
    lp4081h_addr_set_send();
#endif

    uint8_t reg_data[2] = {reg_addr, data};
	lp4081h_write_byte(reg_data, 2);
    //PRINT("reg_addr:%d  %d\n", reg_addr, data);
}


/**
 * @brief 这是一个向寄存器读取数据的函数
 * @param reg_addr 寄存器地址
 */
uint8_t lp4081h_read_reg_and_data(uint8_t reg_addr)
{
#if NEED_SEND_LP4081H_ADDR
    lp4081h_addr_set_send();
#endif

    return lp4081h_read_one_byte(reg_addr);
}


/**
 * @brief 获取电池管理system的状态
 * @param  
 * @return 状态值
 */
uint8_t lp4081h_get_bat_status(void)
{
    //lp4081h_read_reg_and_data(Lp4081h_Control_Register0);
    //lp4081h_read_reg_and_data(Lp4081h_Control_Register1);
    //lp4081h_read_reg_and_data(Lp4081h_Control_Register2);
    //lp4081h_read_reg_and_data(Lp4081h_Control_Register3);
    
    return lp4081h_read_reg_and_data(Lp4081h_Status_Register_0);
}


/**
 * @brief 获取电池温度范围
 * @param  
 * @return 返回温度
 */
int lp4081h_get_bat_temperature(void)
{
    uint8_t read_data = lp4081h_read_reg_and_data(Lp4081h_Status_Register_1);

    switch (read_data)
    {
        case 0:
            return 0xffff;//无效信息
        case 1:
            return -1;//低于0
        case 2:
            return 9;//0-10
        case 3:
            return 35;//10-45
        case 4:
            return 46;//>45
        default:
            break;
    }

	return 0xffff;
}

/**
 * @brief 配置中断屏蔽
 * @param interrupt_status 要屏蔽的中断     LP4081H_INT_RESERVED | LP4081H_INT_CHG
 */
void lp4081h_set_interrupt_mask(uint8_t interrupt_status)
{
    
    lp4081h_write_reg_and_data(Lp4081h_Interrupt_Mask_Register, interrupt_status);
}


/**
 * @brief 设置电流参数，电池工作模式
 * @param double_current 充电电流扩展：1表示所有相关电流乘以2.5，0表示默认设置
 * @param out_current 放电电流限制：00: 440mA, 01: 1A, 10: 2A, 11: 3A "2000mA"
 * @param in_current  输入电流限制：1表示1.5A，0表示650mA "1500mA"
 * @param bat_fet_mode  BAT FET操作模式控制：00: 旁路, 01: 掉电8秒, 10: 自动, 11: 船模式
 */
void lp4081h_set_charging_current(uint8_t double_current, char *out_current, char *in_current, uint8_t bat_fet_mode)
{
    uint8_t reserved = 0, out_bit, in_bit;
    int out_c = 0;
    int in_c = 0;   

    sscanf(out_current, "%d", &out_c);
    sscanf(in_current, "%d", &in_c);

    if (out_c >= 440 && out_c <= 3000)
    {
        out_bit = out_c / 1000;
    }

    if (in_c == 1500)
    {
        in_bit = 1;
    }
    else
    {
        in_bit = 0;
    }

    uint8_t reg_data = reserved + (double_current << 5) + (out_bit << 3) + (in_bit << 2) + (bat_fet_mode);

    //PRINT("%02X %02X\n", Lp4081h_Control_Register0, reg_data);
    lp4081h_write_reg_and_data(Lp4081h_Control_Register0, reg_data);
}


/**
 * @brief 配置控制寄存器1，设置充电电压和涓流电流
 * @param voltage 充电电压(4.0-4.5v),默认4.2v，"4.2V"，0.05v递增
 * @param trickle 涓流电流(1-15mA)
 */
void lp4081h_set_charging_voltage(char *voltage, uint8_t trickle)
{
    int v_4x = 0;
    int v_10 = 0, v_1 = 0;
    uint8_t reg_voltage_data, reg_trickle_data;
    
    sscanf(voltage, "%d.%d", &v_10, &v_1);
    if (v_1 < 10)
    {
        v_4x = v_10 * 100 + v_1 * 10;
    }
    else
    {
        v_4x = v_10 * 100 + v_1;
    }


    if (v_4x >= 400 && v_4x <= 450)
    {
        reg_voltage_data = (v_4x - 400)/5; 
    }

    if (trickle >= 1 && trickle <= 15)
    {
        reg_trickle_data = trickle/2;
    }

    uint8_t reg_data = (reg_trickle_data << 5) + reg_voltage_data; 

    //PRINT("%02X %02X\n", Lp4081h_Control_Register1, reg_data);
    lp4081h_write_reg_and_data(Lp4081h_Control_Register1, reg_data);
}



/**
 * @brief 设置是否开启充电，开启涓流电压，恒流充电电流
 * @param en_chg 开启充电 1/0
 * @param tri_voltage 涓流电压阈值 1/0 3.0/2.8v
 * @param const_chg 恒流充电电流 "880mA"
 */
void lp4081h_set_const_chg(uint8_t en_chg, uint8_t tri_voltage, char *const_chg)
{
    int c_chg = 0;
    uint8_t chg_reg_data = 0;

    sscanf(const_chg, "%d", &c_chg);

    if (c_chg < 25 || c_chg > 1175) {
        PRINT("Error: Charging current out of range.\n");
        return;
    }

    if (c_chg >= 25 && c_chg <= 450)
    {
        chg_reg_data = (c_chg - 25) / 13;
    }
    else if (c_chg > 450 && c_chg <= 1175)
    {
        chg_reg_data = (32 + 2) + (c_chg - 450) / 25;
    }

    //PRINT("%d\n", chg_reg_data);
    uint8_t reg_data = (en_chg << 7) + (tri_voltage << 6) + chg_reg_data;

    //PRINT("%02X %02X\n", Lp4081h_Control_Register2, reg_data);
    lp4081h_write_reg_and_data(Lp4081h_Control_Register2, reg_data); 
}


/**
 * @brief 配置看门狗、电池智能调度
 * @param status  看门狗 NTC电池智能管理 涓流定时器 启用电池满电后继续涓流  
 * @param vsysdpm 设置开启智能调度的电压阈值 "3.2v"
 */
void lp4081h_set_wtd_ntc_vsysdpm(uint8_t status, char *vsysdpm)
{
    uint8_t v_xx = 0;
    int v_10 = 0, v_1 = 0;
    uint8_t vsysdpm_voltage = 0;
    sscanf(vsysdpm, "%d.%d", &v_10, &v_1);

    v_xx = v_10 * 10 + v_1;

    if (v_xx >= 30 && v_xx <= 44)
    {
        vsysdpm_voltage = (v_xx - 30) / 2;
    }
   
    uint8_t reg_data = status + vsysdpm_voltage;
    
    lp4081h_write_reg_and_data(Lp4081h_Control_Register3, reg_data); 
}


/**
 * @brief 对充电ic整个系统设置
 * @param de_chg    开启双倍
 * @param out_c     输出max电流
 * @param in_c      输入电流
 * @param chg_v     充电电压
 * @param chg_const 充电电流
 * @param trickle   涓流大小
 */
void lp4081h_chg_system_set(uint8_t de_chg, char *out_c, char *in_c, 
                                char *chg_v, char *chg_const, char *trickle)
{
    int tri_c = 0;
    sscanf(trickle, "%d", &tri_c);

    if (tri_c > 15)
        return ;

    lp4081h_set_charging_current(de_chg, out_c, in_c, LP4081H_AUTO_MODE);
    lp4081h_set_charging_voltage(chg_v, (uint8_t)tri_c);
    lp4081h_set_const_chg(0, 1, chg_const);

    uint8_t reg_data = lp4081h_read_reg_and_data(Lp4081h_Control_Register3);
    lp4081h_set_wtd_ntc_vsysdpm(reg_data & 0xF8, "4.0V");
}


/**
 * @brief 充电IC的系统配置函数
 * @param lp4081h 
 */
void lp4081h_sys_config(LP4081H_Config_t *lp4081h) 
{
    uint8_t reg_data;

    // 配置中断屏蔽寄存器 (0x02)
    reg_data = 0;
    if (lp4081h->IntMaskSet & (1 << 6)) reg_data |= LP4081H_INT_BAT_UV;
    if (lp4081h->IntMaskSet & (1 << 5)) reg_data |= LP4081H_INT_EOC;
    if (lp4081h->IntMaskSet & (1 << 4)) reg_data |= LP4081H_INT_SCP;
    if (lp4081h->IntMaskSet & (1 << 3)) reg_data |= LP4081H_INT_CHG;
    if (lp4081h->IntMaskSet & (1 << 2)) reg_data |= LP4081H_INT_PGD;
    if (lp4081h->IntMaskSet & (1 << 1)) reg_data |= LP4081H_INT_TIMER;
    if (lp4081h->IntMaskSet & (1 << 0)) reg_data |= LP4081H_INT_NTC;
    lp4081h_write_reg_and_data(Lp4081h_Interrupt_Mask_Register, reg_data);

    // 配置控制寄存器0 (0x03)
    reg_data = 0;
    reg_data |= (lp4081h->DoubleCurMode) << 5; // ICC_EXT
    reg_data |= (lp4081h->DischargeCurLimit << 4) & 0x03; // IDIS_LIM
    reg_data |= (lp4081h->ChgCurLimit) << 2; // EN_CHG
    reg_data |= (lp4081h->BatFetMode) & 0x03; // DIS[1:0]
    lp4081h_write_reg_and_data(Lp4081h_Control_Register0, reg_data);

    // 配置控制寄存器1 (0x04)
    reg_data = 0;
    reg_data |= (lp4081h->ChgTrickle) << 5; // Iter[2:0]
    reg_data |= (lp4081h->ConstChgVol) & 0x07; // BAT_CV[4:0] (higher bits)
    lp4081h_write_reg_and_data(Lp4081h_Control_Register1, reg_data);

    // 配置控制寄存器2 (0x05)
    reg_data = 0;
    reg_data |= (lp4081h->ChgTriVolLimit) << 6; // VTRI
    reg_data |= (lp4081h->ConstChgCur) & 0x3F; // ICC[5:0]
    lp4081h_write_reg_and_data(Lp4081h_Control_Register2, reg_data);

    // 配置控制寄存器3 (0x06)
    reg_data = 0;
    reg_data |= (lp4081h->BatManagement & LP4081H_RESET_WTD); // WTD
    reg_data |= (lp4081h->BatManagement & LP4081H_EN_WTD); // EN_WTD
    reg_data |= (lp4081h->BatManagement & LP4081H_EN_NTC); // EN_NTC
    reg_data |= (lp4081h->BatManagement & LP4081H_EN_TRI_TIMER); // EN_Tri_Timer
    reg_data |= (lp4081h->BatManagement & LP4081H_EN_EOC_CONTINUE); // EN_Term
    reg_data |= (lp4081h->BatVsysDpm) & 0x07; // DPM[2:0]
    lp4081h_write_reg_and_data(Lp4081h_Control_Register3, reg_data);

    // 添加延迟以确保配置生效
    //delay_ms(10);
}

void lp4081h_default_config(void)
{
    lp4081h_sys_config(&lp4081h_config);
}


/**
 * @brief 开启充电
 * @param  
 */
void lp4081h_start_chg(void)
{
    uint8_t reg_data = lp4081h_read_reg_and_data(Lp4081h_Control_Register2);
    
    lp4081h_write_reg_and_data(Lp4081h_Control_Register2, LP4081H_MOV_L_BIT(1,7) | reg_data); 
}


/**
 * @brief 停止充电
 * @param  
 */
void lp4081h_stop_chg(void)
{
    uint8_t reg_data = lp4081h_read_reg_and_data(Lp4081h_Control_Register2);
    
    lp4081h_write_reg_and_data(Lp4081h_Control_Register2, ~LP4081H_MOV_L_BIT(1,7) & reg_data); 
    //PRINT("lp4081h_stop_chg %02X %02X\n", Lp4081h_Control_Register2, ~LP4081H_MOV_L_BIT(1,7) & reg_data);
}

void lp4081h_chg_tri_start(void)
{
    uint8_t reg_data = lp4081h_read_reg_and_data(Lp4081h_Control_Register3);
    
    lp4081h_write_reg_and_data(Lp4081h_Control_Register3, LP4081H_EN_TRI_TIMER | reg_data); 
}


/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/

