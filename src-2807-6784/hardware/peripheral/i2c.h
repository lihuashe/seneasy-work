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

#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"

typedef struct
{
    uint8_t *tx_buf1; //register addr(offset)
    uint32_t tx_len1;
    uint8_t *tx_buf2; //data which prepare to send
    uint32_t tx_len2;

    uint8_t *rx_buf; //data receive
    uint32_t rx_len;
	uint8_t *rx_buf_real;
    uint32_t rx_len_real;
}i2c_op_t;
/*********************************************************************
 * MACROS
 */
#define I2C_MODULE_NUM  1
#define I2C_SLAVE_DEFAULT_ADDR  (0x68<<1)
//#define I2C_SLAVE_DEFAULT_ADDR  0x50

#define I2CD_TIMEOUT            0x20   /**< @brief Hardware timeout.    */


#define I2C_STATUS_TFNF                      (1UL << 1)
#define I2C_STATUS_RFNE                      (1UL << 3)


typedef enum
{
    /// Master mode
    I2C_MODE_MASTER,
    /// Slave mode
    I2C_MODE_SLAVE,
    /// SMBUS device mode
    I2C_MODE_SMBUS_DEVICE,
    /// SMBUS host mode
    I2C_MODE_SMBUS_HOST,
} i2c_mode_t;


typedef void (*i2c_complete_callback_t)(const uint8_t* recv_data, const uint32_t data_size);

typedef struct
{
    XH_DMA_CH_Type *tx_dma;
    XH_DMA_CH_Type *rx_dma;
    dma_callback_t callback;
} i2c_dma_t;

void i2c_open(XH_I2C_Type *i2c, i2c_mode_t mode, uint32_t speed);
void i2c_close(XH_I2C_Type *i2c);
bool i2c_master_read(XH_I2C_Type *i2c, uint16_t addr, uint8_t* tx_buf, uint32_t tx_len, i2c_complete_callback_t cb);
bool i2c_master_write(XH_I2C_Type *i2c, uint16_t addr, uint8_t* rx_buf, uint32_t rx_len, i2c_complete_callback_t cb);
bool i2c_slave_read(XH_I2C_Type *i2c, uint8_t* rx_buf, uint32_t rx_len, i2c_complete_callback_t cb);
bool i2c_slave_write(XH_I2C_Type *i2c, uint8_t* tx_buf, uint32_t tx_len, i2c_complete_callback_t cb);

bool i2c_master_write_mem(uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *tx_buf, uint32_t tx_len);
bool i2c_master_read_mem(uint16_t addr, uint32_t offset, uint32_t alen, uint8_t *rx_buf, uint32_t rx_len);

void send_register_data(XH_I2C_Type* i2c, i2c_op_t* op);
void printfSgSlaveRecvData(void);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

