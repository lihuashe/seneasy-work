/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bct3236_driver.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-18
* @version  v1.0
*********************************************************************************************************
*/

#ifndef I2C_INIT_H_
#define I2C_INIT_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <swtimer.h>
#include <rtl876x.h>
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "aw21036_driver.h"
#include "rtl876x_i2c.h"
#include "rtl876x.h"
#include "system_rtl876x.h"
#include "rtl876x_pinmux.h"
#include "app_msg.h"
#include "app_task.h"


/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define NEED_SHARED_BUS                 1

#define DEV_I2Cx                        I2C0
#define APBP_I2C                        APBPeriph_I2C0
#define APBP_I2C_CLOCK                  APBPeriph_I2C0_CLOCK

#define I2C_SDA_PIN                     P4_3
#define I2C_SCL_PIN                     P4_2

#define PINMUX_I2C_CLK                  I2C0_CLK                    
#define PINMUX_I2C_DAT                  I2C0_DAT 

#define DEVICE_ADDR			            0x3C  /* AD0——>GND */
#define DEVICE_ADDR_WRITE		        (DEVICE_ADDR << 1) | 0  
#define DEVICE_ADDR_READ		        (DEVICE_ADDR << 1) | 1  


/*============================================================================*
 *                          Functions
 *============================================================================*/
void i2c_init(void);

uint8_t i2c_read_byte(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data);

uint8_t i2c_write_byte(uint8_t slave_addr, uint8_t *data, uint8_t data_len);
#endif /* I2C_INIT_H_ */

