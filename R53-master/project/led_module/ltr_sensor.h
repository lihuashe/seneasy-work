/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ltr_sensor.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef LTR_SENSOR_H_
#define LTR_SENSOR_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define LTR_DEVICE_ADDR			(0x23 << 1)


/******************************************************
 *
 * Register List
 *
 *****************************************************/
#define REG_ALS_CONFIG			0x7F    //RW value: 0x07  ALS Config
#define REG_ALS_CONTR			0x80    //RW value: 0x20  ALS operation mode control
#define REG_ALS_INT_TIME		0x85    //RW value: 0x06  ALS integration time and measurement rate in active mode
#define REG_PART_ID			    0x86    //R  value: 0x1C  Part Number ID and revision IDs
#define REG_ALS_STATUS			0x88    //R  value: 0x00  ALS Status
#define REG_IR_DATA_LSB			0x89    //R  value: 0x00  ALS measurement IR data,LSB
#define REG_IR_DATA_MSB			0x8A    //R  value: 0x00  ALS measurement IR data,MSB
#define REG_ALS_DATA_LSB		0x8B    //R  value: 0x00  ALS data,Lower Byte
#define REG_ALS_DATA_MSB	    0x8C    //R  value: 0x00  ALS data,Upper Byte
#define REG_MAIN_CONTR			0xAD    //RW value: 0x00  Main Control Setting
#define REG_DARK_CONTR			0xB4    //RW value: 0x00  Dark Control Setting
#define REG_DARK_CONFIG			0xB9    //R  value: 0x00  Dark Offset Map Value

extern u8 asl_status_error_cnt;
extern u16 ASL_Lux_Calc;
extern u8 factory_Read_ASL;
extern u16 Notify_Sy_Lux;
/*============================================================================*
 *                          Functions
 *============================================================================*/
extern void ltr_sensor_init(void);
extern u16 ALS_Read_Data(void);
extern u8 ALS_Read_Status(void);
extern u8 ALS_Read_Status_Init(void);
extern void ALS_Disable(void);
extern void ltr_sensor_init_TEST(void);
extern u8 ALS_Read_Status_TEST(void);
#endif /* LTR_SENSOR_H_ */
