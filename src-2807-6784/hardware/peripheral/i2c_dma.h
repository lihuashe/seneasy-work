/**
 * @file i2c.h
 * @brief i2c driver
 * @date Wed 31 May 2017 07:15:14 PM CST
 * @author XIHAO
 *
 * @defgroup I2C I2C
 * @ingroup PERIPHERAL
 * @brief I2C driver
 * @details I2C driver
 *
 * The I2C is a master or slave interface. It supports 100, 400 and 800 KHz clock rates
 * for controlling EEPROM and etc. The I2C interface provides several data formats and
 * can fit various I2C peripherals. Sequential read and write are supported to improve
 * throughputs. The I2C support DMA operation for extra MCU free data transfer. The I2C
 * work as ether master or slave, but cannot change the working mode after configuration.
 *
 * @{
 *
 * @example example_i2c.c
 * This is an example of how to use the i2c
 *
 */

#ifndef __I2C_DMA_H__
#define __I2C_DMA_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"


/*********************************************************************
 * MACROS
 */
#define I2C_MODULE_NUM  1

// compatible old version
#define i2c_open(mode, speed)                                    i2c_open_x(XH_I2C, mode, speed)
#define i2c_close()                                              i2c_close_x(XH_I2C)
#define i2c_master_write(addr, tx_buf, tx_len)                   i2c_master_write_x(XH_I2C, addr, tx_buf, tx_len)
#define i2c_master_read(addr, rx_buf, rx_len)                    i2c_master_read_x(XH_I2C, addr, rx_buf, rx_len)

/*********************************************************************
 * TYPEDEFS
 */
/// I2C mode


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief i2c initialize
 *
 * @param[in] i2c  i2c device address
 * @param[in] mode  mode
 * @param[in] speed  speed
 *
 * @return None
 **/
void i2c_open_x(XH_I2C_Type *i2c, i2c_mode_t mode, uint32_t speed);

/**
 * @brief i2c close
 *
 * @param[in] i2c  i2c device address
 * @return None
 **/
void i2c_close_x(XH_I2C_Type *i2c);

/**
 * @brief i2c master write data
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] tx_buf  transmit data buffer
 * @param[in] tx_len  transmit data length
 *
 * @retval true success
 * @retval false fail
 **/
bool i2c_master_write_x(XH_I2C_Type *i2c, uint16_t addr, uint8_t *tx_buf, uint32_t tx_len);

/**
 * @brief i2c master read data
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] rx_buf receive data buffer
 * @param[in] rx_len receive buffer length
 *
 * @retval true success
 * @retval false fail
 **/
bool i2c_master_read_x(XH_I2C_Type *i2c, uint16_t addr, uint8_t *rx_buf, uint32_t rx_len);

/**
 * @brief i2c master read memery (EEPROM)
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] offset  memery offset
 * @param[in] alen  memery offset bytes
 * @param[in] rx_buf  receive data buffer
 * @param[in] rx_len  receive data lenght
 *
 * @retval true success
 * @retval false fail
 **/
bool i2c_master_read_mem_x(XH_I2C_Type *i2c, uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *rx_buf, uint32_t rx_len);

/**
 * @brief i2c master write memery (EEPROM)
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] offset  memery offset
 * @param[in] alen  memery offset bytes
 * @param[in] tx_buf  transmit data buffer
 * @param[in] tx_len  transmit data length
 *
 * @retval true success
 * @retval false fail
 **/
bool i2c_master_write_mem_x(XH_I2C_Type *i2c, uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *tx_buf, uint32_t tx_len);


/**
 * @brief i2c dma config
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] i2c_dma_t i2c dma data structure
 * @param[in] dma_callback_t  callback function
 *
 **/
void i2c_dma_config(XH_I2C_Type *i2c, i2c_dma_t *dma, dma_callback_t callback);

/**
 * @brief i2c master read memery(EEPROM) through dma 
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] i2c_dma_t i2c dma data structure
 * @param[in] offset  memery offset
 * @param[in] alen  memery offset bytes
 * @param[in] rx_buf  recv data buffer
 * @param[in] rx_len  recv data length
 *
 * @retval true success
 * @retval false fail
 **/
void i2c_master_read_mem_dma(XH_I2C_Type *i2c,i2c_dma_t *dma, uint16_t addr, uint32_t offset,uint32_t alen, uint8_t *rx_buf, uint32_t rx_len);

/**
 * @brief i2c master write memery(EEPROM) through dma 
 *
 * @param[in] i2c  i2c device address
 * @param[in] addr  i2c slave address
 * @param[in] offset  memery offset
 * @param[in] alen  memery offset bytes
 * @param[in] tx_buf  send data buffer
 * @param[in] tx_len  send data length
 *
 * @retval true success
 * @retval false fail
 **/
void i2c_master_write_mem_dma(XH_I2C_Type *i2c,i2c_dma_t *dma, uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *tx_buf, uint32_t tx_len);



#ifdef __cplusplus
}
#endif

#endif

/** @} */

