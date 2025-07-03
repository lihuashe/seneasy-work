#include "pan216_reg.h"

/**
 * @brief Count trailing zeros
 *
 * @param val
 * @return int
 */
uint8_t __ctz(uint8_t val)
{
    for (int i = 0; i < 8; i++)
    {
        if (val & (1 << i))
        {
            return i;
        }
    }

    return 8;
}

/**************************************************************************/
/*                  Basic Read/Write Operation                            */
/**************************************************************************/

/**
 * @brief This function reads bytes from the register
 *
 * @param[in] addr Register address
 * @param[out] buffer Buffer provided for data from the register
 * @param[in] len Number of bytes to read
 * @return PAN216_Err_t
 */
static void PAN216_Read(uint8_t addr, uint8_t *buffer, uint8_t len)
{
#if INTERFACE_MODE == USE_I2C
    Pan216_Funs.i2c_read_buf(addr, buffer, (uint16_t)len);
#else
    Pan216_Funs.spi_cs_low();
    Pan216_Funs.spi_writebyte(addr << 1);
    for (int i = 0; i < len; i++)
    {
        buffer[i] = Pan216_Funs.spi_readbyte();
    }
    Pan216_Funs.spi_cs_high();
#endif
}

/**
 * @brief This function writes bytes to the register
 *
 * @param[in] addr Register address
 * @param[in] buffer Buffer provided for data to write to the register
 * @param[in] len Number of bytes to write
 * @return PAN216_Err_t
 */
static void PAN216_Write(uint8_t addr, uint8_t *buffer, uint8_t len)
{
#if (INTERFACE_MODE == USE_I2C)
    Pan216_Funs.i2c_write_buf(addr, buffer, len);
#else
    Pan216_Funs.spi_cs_low();
    Pan216_Funs.spi_writebyte(((addr << 1) | 0x01));
    for (int i = 0; i < len; i++)
    {
        Pan216_Funs.spi_writebyte(buffer[i]);
    }
    Pan216_Funs.spi_cs_high();
#endif
}

/**************************************************************************/
/*                         Basic Register Operation                       */
/**************************************************************************/

/**
 * @brief Read one byte from the register in the current page
 * @param[in] addr Register address to read
 * @return Value read from the register
 */
uint8_t PAN216_ReadReg(uint8_t addr)
{
    uint8_t tmp;

    PAN216_Read(addr, &tmp, 1);

    return tmp;
}

/**
 * @brief Write to the global register in the current page and check
 * @param[in] addr Register address to write
 * @param[in] value Value to write to the register
 * @return Result
 */
PAN216_Err_t PAN216_WriteReg(uint8_t addr, uint8_t value)
{
    PAN216_Write(addr, &value, 1);

#if SPI_CHECK_WRITE
    uint8_t tmp;
    tmp = PAN216_ReadReg(addr);
    if (tmp != value)
    {
        printf("write reg [readback:0x%02x] writeval:0x%02x error \r\n ", tmp, value);
        return PAN216_ERR;
    }
#endif

    return PAN216_OK;
}

/**
 * @brief Write to the global register in the current page without check
 * @param[in] addr Register address to write
 * @param[in] value Value to write to the register
 * @return Result
 */
PAN216_Err_t PAN216_WriteRegUnchecked(uint8_t addr, uint8_t value)
{
    PAN216_Write(addr, &value, 1);
    return PAN216_OK;
}

/**
 * @brief Write continuous register values (buffer) in a specific address page
 * @param[in] addr Register start address
 * @param[in] buffer Values to write
 * @param[in] len Buffer length
 * @return Result
 */
PAN216_Err_t PAN216_WriteRegs(uint8_t addr, uint8_t *buffer, uint8_t len)
{
    PAN216_Write(addr, buffer, len);
    return PAN216_OK;
}

/**
 * @brief This function reads more than one byte from the register
 *
 * @param addr Register address
 * @param buffer Buffer provided for data from the register
 * @param len Number of bytes to read
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_ReadRegs(uint8_t addr, uint8_t *buffer, uint8_t len)
{
    PAN216_Read(addr, buffer, len);
    return PAN216_OK;
}

/**
 * @brief Transfer data to the data FIFO on the chip
 * @param[in] addr Register address to write
 * @param[in] buffer Buffer provided for data to write to the register
 * @param[in] size Number of bytes to write
 * @return Result
 */
PAN216_Err_t PAN216_WriteFifo(uint8_t addr, uint8_t *buffer, uint8_t size)
{
    return PAN216_WriteRegs(addr, buffer, size);
}

/**
 * @brief Read more than one byte from the register
 * @param[in] addr Register address to read
 * @param[in] buffer Buffer provided for data from the register
 * @param[in] size Number of bytes to read
 * @return Result
 */
PAN216_Err_t PAN216_ReadFifo(uint8_t addr, uint8_t *buffer, uint8_t size)
{
    return PAN216_ReadRegs(addr, buffer, size);
}

/**
 * @brief This function writes bits to the register by mask
 *
 * @param addr Register address
 * @param val Bits value
 * @param mask Bits mask
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_WriteRegBits(uint8_t addr, uint8_t val, uint8_t mask)
{
    uint8_t tmp;
    PAN216_Err_t ret;
    uint8_t shift = GET_SHIFT(mask);

    val <<= shift;
    if (val & (~mask))
    {
        printf("val:0x%02x mask:0x%02x error \r\n", val, mask);
        return PAN216_ERR;
    }
    val &= mask;

    tmp = PAN216_ReadReg(addr);
    ret = PAN216_WriteReg(addr, (tmp & (~mask)) | val);

    return ret;
}

/**************************************************************************/
/*                         Page Register Operation                        */
/**************************************************************************/

/**
 * @brief Switch page
 * @param[in] page Page to switch
 * @return Result
 */
PAN216_Err_t PAN216_SetPage(uint8_t page)
{
    if (PAN216_OK != PAN216_WriteReg(PAGE_CFG, page))
    {
        return PAN216_ERR;
    }

    return PAN216_OK;
}

/**
 * @brief This function writes a value to the register in a specific page
 * @param[in] page The page of the register
 * @param[in] addr Register address
 * @param[in] value Value to write
 * @return Result
 */
PAN216_Err_t PAN216_WritePageReg(uint8_t page, uint8_t addr, uint8_t value)
{
    uint8_t original_page;

    original_page = PAN216_ReadReg(PAGE_CFG);
    if (page != original_page)
    {
        P_ASSERT(PAN216_SetPage(page));
    }
    P_ASSERT(PAN216_WriteReg(addr, value));
    if (page != original_page)
    {
        P_ASSERT(PAN216_SetPage(original_page));
    }

    return PAN216_OK;
}

/**
 * @brief This function writes a value to the register in a specific page without check
 *
 * @param page Register page
 * @param addr Register address
 * @param value Register value
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_WritePageRegUnchecked(uint8_t page, uint8_t addr, uint8_t value)
{
    uint8_t original_page;

    original_page = PAN216_ReadReg(PAGE_CFG);
    if (page != original_page)
    {
        PAN216_SetPage(page);
    }
    PAN216_Write(addr, &value, 1);
    if (page != original_page)
    {
        PAN216_SetPage(original_page);
    }

    return PAN216_OK;
}

/**
 * @brief Read a value from the register in a specific page
 * @param[in] page The page of the register
 * @param[in] addr Register address
 * @return Success (register value) or failure
 */
uint8_t PAN216_ReadPageReg(uint8_t page, uint8_t addr)
{
    uint8_t value;
    uint8_t original_page;

    original_page = PAN216_ReadReg(PAGE_CFG);
    if (page != original_page)
    {
        PAN216_SetPage(page);
    }
    value = PAN216_ReadReg(addr);
    if (page != original_page)
    {
        PAN216_SetPage(original_page);
    }

    return value;
}

/**
 * @brief Write continuous register values (buffer) in a specific address page
 * @param[in] page The page of the register
 * @param[in] addr Register start address
 * @param[in] buffer Values to write
 * @param[in] len Buffer length
 * @return Result
 */
PAN216_Err_t PAN216_WritePageRegs(uint8_t page, uint8_t addr, uint8_t *buffer, uint8_t len)
{
    uint8_t original_page;

    original_page = PAN216_ReadReg(PAGE_CFG);
    if (page != original_page)
    {
        PAN216_SetPage(page);
    }
    PAN216_WriteRegs(addr, buffer, len);
    if (page != original_page)
    {
        PAN216_SetPage(original_page);
    }

    return PAN216_OK;
}

/**
 * @brief This function reads more than one byte from the register
 *
 * @param page Page of the register
 * @param addr Register address
 * @param buffer Buffer provided for data from the register
 * @param len Number of bytes to read
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_ReadPageRegs(uint8_t page, uint8_t addr, uint8_t *buffer, uint8_t len)
{
    uint8_t original_page;

    original_page = PAN216_ReadReg(PAGE_CFG);
    if (page != original_page)
    {
        PAN216_SetPage(page);
    }
    PAN216_Read(addr, buffer, len);
    if (page != original_page)
    {
        PAN216_SetPage(original_page);
    }

    return PAN216_OK;
}

/**************************************************************************/
/*                         Register Bit Operation                         */
/**************************************************************************/
/**
 * @brief Write bits to the register by mask
 * @param page Page of the register
 * @param addr Register address
 * @param mask Bits mask
 * @param value Bits value
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_WriteRegWithMask(uint8_t page, uint8_t addr, uint8_t mask, uint8_t value)
{
    uint8_t tmp;
    PAN216_Err_t ret;

    value &= mask;
    tmp = PAN216_ReadPageReg(page, addr);
    ret = PAN216_WritePageReg(page, addr, (tmp & (~mask)) | value);

    return ret;
}

/**
 * @brief This function reads the register value with mask
 *
 * @param page Register page
 * @param addr Register address
 * @param mask Bits mask
 * @return uint8_t
 */
uint8_t PAN216_ReadRegWithMask(uint8_t page, uint8_t addr, uint8_t mask)
{
    uint8_t tmp;

    tmp = PAN216_ReadPageReg(page, addr);

    return tmp & mask;
}

/**
 * @brief Set bits to 1
 *
 * @param page Page of the register
 * @param addr Register address
 * @param mask Bits mask
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_SetRegBits(uint8_t page, uint8_t addr, uint8_t mask)
{
    uint8_t tmp;
    PAN216_Err_t ret;

    tmp = PAN216_ReadPageReg(page, addr);
    ret = PAN216_WritePageReg(page, addr, tmp | mask);

    return ret;
}

/**
 * @brief Set bits to 0
 *
 * @param page Page of the register
 * @param addr Register address
 * @param mask Bits mask
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_ResetRegBits(uint8_t page, uint8_t addr, uint8_t mask)
{
    uint8_t tmp;
    PAN216_Err_t ret;

    tmp = PAN216_ReadPageReg(page, addr);
    ret = PAN216_WritePageReg(page, addr, tmp & (~mask));

    return ret;
}

/**
 * @brief Write bits to the register by mask
 * @param page Page of the register
 * @param addr Register address
 * @param mask Bits mask
 * @param value Bits value
 * @return PAN216_Err_t
 */
PAN216_Err_t PAN216_WritePageRegBits(uint8_t page, uint8_t addr, uint8_t val, uint8_t mask)
{
    uint8_t tmp;
    PAN216_Err_t ret;

    uint8_t shift = GET_SHIFT(mask);

    val <<= shift;
    val &= mask;

    tmp = PAN216_ReadPageReg(page, addr);
    ret = PAN216_WritePageReg(page, addr, (tmp & (~mask)) | val);

    return ret;
}
