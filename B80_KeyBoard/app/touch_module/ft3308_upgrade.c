/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ft3308_upgrade.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "ft3308_upgrade.h"
#include "ft3308_driver.h"
#include "../app_config.h"
#include "../keyboard_rcu.h"

#if(TOUCH_ENABLE)

/*
 ###########################################################
 *Sample code for firmware upgrade*
 ###########################################################
 */

/* The array variable fw_file_ft7202 stores the firmware file that you
 * want to upgrade.
 * Please modify it based on the firmware file you get.
 * Please ignore it if you get firmware file from file system, now you
 * need read firmware file using your method.
 */
const u8 fw_file_ft7202[] = {
#include "SRC_1626_FT3308_Ver0x08_20241016_app.i"
};

/* The array variable pb_file_ft7202 stores the content of "pramboot.bin"
 * You can't change it unless FocalTech provides a later "pramboot.bin"
 */
const u8 pb_file_ft7202[] = {
#include "FT6216_Pramboot_v0.3_20200430.i"
};

#define FTS_FLASH_PACKET_LENGTH                     (128)
#define FTS_MAX_LEN_ECC_CALC                        (0xFFFE) /* must be even */

/*Variables*/
static bool tp_fw_valid = false;/*to note firmware in TP controller is valid or not*/

int platform_i2c_write(u8 *data_m, u8 len)
{
    i2c_master_init(FT3308_SLAVE_DMA_ADDR, FT3308_SLAVE_DIVCLOCK);
    u8 data_n[200] = {};

    memcpy(data_n, data_m + 1, len - 1);
    i2c_write_series(data_m[0], FT3308_SLAVE_ADD_LED, (unsigned char *)data_n, len-1);

    return true;
}

int platform_i2c_read(u8 *data_m, u8 len)
{
    i2c_master_init(FT3308_SLAVE_DMA_ADDR, FT3308_SLAVE_DIVCLOCK);
    i2c_read_series(data_m[0], FT3308_SLAVE_ADD_LED, (unsigned char *)data_m, len);
    return true;
}

/*delay, unit: millisecond */
void fts_msleep(unsigned long msec)
{
    WaitMs(msec);
}


/*****************************************************************************
* Name: fts_write
* Brief: 
*   Write function via I2C bus, you should implement it based on your platform. 
*
*   The code below is only a sample code, a pseudo code. you must implement it
*  following standard I2C protocol based on your platform
*
*
* Input: @cmd: the command buffer
*        @cmdlen: length of command buffer
*        @data: the data buffer, data buffer can be NULL for commands without data fields.
*        @datalen: length of data buffer
* Output: 
* Return: 
*   0 if success, otherwise error code.
*****************************************************************************/
int fts_write(u8 *cmd, u32 cmdlen, u8 *data, u32 datalen)
{
    /*TODO based your platform*/
    int ret = 0;
    u8 txbuf[256] = { 0 };
    u32 txlen = 0;

    if (!cmd || !cmdlen || ((cmdlen + datalen) >= 256)) {
        printf("cmd is null, or cmdlen is 0, or txlen(%d) fails\r\n", cmdlen + datalen);
        return -1;
    }

    memcpy(&txbuf[0], cmd, cmdlen);
    txlen = cmdlen;
    if (data && datalen) {
        memcpy(&txbuf[txlen], data, datalen);
        txlen += datalen;
    }

    /*call platform i2c_write function to transfer I2C package to TP controller
     *platform_i2c_write() is different for different platform, based on your platform.
     */

    ret = platform_i2c_write(txbuf, txlen);
    if (ret < 0) {
        printf("i2c_write(%d) fails\r\n", cmd[0]);
        return ret;
    }

    return 0;

}

/*****************************************************************************
* Name: fts_read
* Brief: 
*   Read function via I2C bus, you should implement it based on your platform. 
*
*   The code below is only a sample code, a pseudo code. you must implement it
*  following standard I2C protocol based on your platform
*
*
* Input: @cmd: the command buffer
*        @cmdlen: length of command buffer
*        @datalen: length of data buffer
* Output: 
*        @data: the data buffer read from TP controller
* Return: 
*   0 if success, otherwise error code.
*****************************************************************************/
int fts_read(u8 *cmd, u32 cmdlen, u8 *data, u32 datalen)
{
    /*TODO based your platform*/
	int ret = 0;

	if (!data || !datalen) {
		printf("data is null, or datalen is 0\r\n");
		return -1;
	}

    #if 0
	if (cmd && cmdlen) {
        /*call platform i2c_write function to transfer I2C package to TP controller
         *platform_i2c_write() is different for different platform, based on your platform.
         */
		ret = platform_i2c_write(cmd, cmdlen);
		if (ret < 0) {
			printf("i2c_write(%d) fails\r\n", cmd[0]);
			return ret;
		}
	}
    #endif

    /*call platform_i2c_read function to transfer I2C package to read data from TP controller
     *platform_i2c_read() is different for different platform, based on your platform.
     */
    data[0] = cmd[0];
	ret = platform_i2c_read(data, datalen);
	if (ret < 0) {
		printf("read proc fails\r\n");
		return ret;
	}

	return 0;
}

int fts_write_reg(u8 addr, u8 val)
{
    i2c_master_init(FT3308_SLAVE_DMA_ADDR, FT3308_SLAVE_DIVCLOCK);
    i2c_write_byte(addr, FT3308_SLAVE_ADD_LED, val);
    return true;
}

int fts_read_reg(u8 addr, u8 *val)
{
    return fts_read(&addr, 1, val, 1);
}

/*Functions*/
/************************************************************************
 * Name: fts_fwupg_get_boot_id
 * Brief: read boot id from TP.
 * Input:
 * Output: @id: stores boot id
 * Return:
 *  0 if read boot id successfully, otherwise error code
***********************************************************************/
static int fts_fwupg_get_boot_id(u8 *id)
{
    int ret = 0;
    u8 cmd = 0;

    cmd = 0x55;
    ret = fts_write(&cmd, 1, NULL, 0);
    if (ret < 0) {
        printf("send command 0x55 fails\r\n");
        return ret;
    }

    fts_msleep(12);
    cmd = 0x90;
    ret = fts_read(&cmd, 1, id, 2);
    if (ret < 0) {
        printf("send command 0x90 fails\r\n");
        return ret;
    }

    printf("read boot id:0x%02x%02x\r\n", id[0], id[1]);

    return 0;
}

/************************************************************************
 * Name: fts_fwupg_check_flash_status
 * Brief: read status from tp
 * Input: @flash_status: correct value from tp
 *        @retries: read retry times
 *        @retries_delay: retry delay
 * Output:
 * Return: return true if flash status check pass, otherwise return false
***********************************************************************/
static bool fts_fwupg_check_flash_status(u16 flash_status, int retries, int retries_delay)
{
    int ret = 0;
    int i = 0;
    u8 cmd = 0;
    u8 val[2] = { 0 };
    u16 read_status = 0;

    for (i = 0; i < retries; i++) {
        cmd = 0x6A;
        ret = fts_read(&cmd , 1, val, 2);
        read_status = (((u16)val[0]) << 8) + val[1];
        if (flash_status == read_status) {
            /* printf("[UPGRADE]flash status ok\r\n"); */
            return true;
        }
        /* printf("flash status fail,ok:%04x read:%04x, retries:%d\r\n", flash_status, read_status, i); */
        fts_msleep(retries_delay);
    }

    return false;
}

/************************************************************************
 * Name: fts_pram_write_buf
 * Brief: Write pramboot program to PRAM of TP controller.
 * Input: @buf: pointer to pramboot program
 *        @len: length of pramboot program
 * Output:
 * Return:
 *  0 if success, otherwise error code
***********************************************************************/
static int fts_pram_write_buf(u8 *buf, u32 len)
{
    int ret = 0;
    u32 i = 0;
    u32 offset = 0;
    u32 remainder = 0;
    u32 packet_number;
    u32 packet_len = 0;
    u8 packet_buf[FTS_FLASH_PACKET_LENGTH + 6] = { 0 };

    printf("write pramboot to pram\r\n");
    packet_number = len / FTS_FLASH_PACKET_LENGTH;
    remainder = len % FTS_FLASH_PACKET_LENGTH;
    if (remainder > 0)
        packet_number++;
    packet_len = FTS_FLASH_PACKET_LENGTH;

    for (i = 0; i < packet_number; i++) {
        offset = i * FTS_FLASH_PACKET_LENGTH;
        /* last packet */
        if ((i == (packet_number - 1)) && remainder)
            packet_len = remainder;

        packet_buf[0] = 0xAE;
        packet_buf[1] = (offset >> 16);
        packet_buf[2] = (offset >> 8);
        packet_buf[3] = (offset);
        packet_buf[4] = (packet_len >> 8);
        packet_buf[5] = (packet_len);
        memcpy(&packet_buf[6], &buf[offset], packet_len);

        ret = fts_write(&packet_buf[0], 6, &packet_buf[6], packet_len);
        if (ret < 0) {
            printf("pramboot write data(%d) fail\r\n", i);
            return ret;
        }
    }

    return 0;
}

/************************************************************************
 * Name: fts_pram_ecc_cal_host
 * Brief: Calculate the ecc value of pramboot program.
 * Input: @buf: pointer to pramboot program
 *        @len: length of pramboot program
 * Output:
 * Return:
 *  ecc value calculated in host using XOR8 algorithm
***********************************************************************/
static int fts_pram_ecc_cal_host(u8 *buf, u32 len)
{
    u8 ecc = 0;
    u32 i = 0;

    for (i = 0; i < len; i++) {
        ecc ^= buf[i];
    }

    return (int)ecc;
}

/************************************************************************
 * Name: fts_pram_ecc_cal_tp
 * Brief: Read ecc value from TP controller.
 * Input:
 * Output:
 * Return:
 *  ecc value read from TP controller if success, otherwise error code
***********************************************************************/
static int fts_pram_ecc_cal_tp(void)
{
    int ret = 0;
    u8 value = 0;

    ret = fts_read_reg(0xCC, &value);
    if (ret < 0) {
        printf("read pramboot ecc fail\r\n");
        return ret;
    }

    return (int)value;
}

/************************************************************************
 * Name: fts_pram_write_remap
 * Brief: Write and remap to run pramboot program.
 * Input:
 * Output:
 * Return:
 *  0 if success, otherwise error code
***********************************************************************/
static int fts_pram_write_remap(void)
{
    int ret = 0;
    u8 cmd = 0;
    u8 boot_id[2] = { 0 };
    u8 *pramboot = (u8 *)pb_file_ft7202;
    u32 pramboot_length = sizeof(pb_file_ft7202);
    int ecc_in_host;
    int ecc_in_tp;

    if (pramboot_length < 0x120) {
        printf("pramboot length(%d) check fails\r\n", pramboot_length);
        return -1;
    }
    printf("pramboot_length = %d\r\n", pramboot_length);
    ret = fts_pram_write_buf(pramboot, pramboot_length);
    if (ret < 0) {
        printf( "write pramboot fail\r\n");
        return ret;
    }

    ecc_in_host = fts_pram_ecc_cal_host(pramboot, pramboot_length);
    ecc_in_tp = fts_pram_ecc_cal_tp();
    printf("pram ecc in tp:%02x, host:%02x\r\n", ecc_in_tp, ecc_in_host);
    /*  pramboot checksum != fw checksum, upgrade fail */
    if (ecc_in_host != ecc_in_tp) {
        printf("pramboot ecc check fail\r\n");
        return -2;
    }

    /*remap to run pramboot*/
    cmd = 0x08;
    ret = fts_write(&cmd, 1, NULL, 0);
    if (ret < 0) {
        printf("pram start fail\r\n");
        return ret;
    }
    fts_msleep(100);

    /*read pramboot id*/
    ret = fts_fwupg_get_boot_id(boot_id);
    if ((boot_id[0] != 0x62) || (boot_id[1] != 0x1A)) {
        printf("not in pramboot,upgrade fails\r\n");
        return -3;
    }

    return 0;
}

/************************************************************************
* Name: fts_fwupg_reset_to_romboot
* Brief: Send software reset(07) to reset into romboot when TP is in pramboot
*   or bootloader, not in romboot.
* Input:
* Output:
* Return: return 0 if success, otherwise return error code
***********************************************************************/
static int fts_fwupg_reset_to_romboot(void)
{
    int ret = 0;
    int i = 0;
    u8 cmd = 0x07;
    u8 boot_id[2] = { 0 };

    ret = fts_write(&cmd, 1, NULL, 0);
    if (ret < 0) {
        printf("BOOT reset cmd write fail\r\n");
        return ret;
    }
    fts_msleep(10);  /*the delay must be 6~26 ms, otherwise fails*/

    for (i = 0; i < 10; i++) {
        ret = fts_fwupg_get_boot_id(boot_id);
        if ((boot_id[0] == 0x62) && (boot_id[1] == 0x16))
            break;
        fts_msleep(1);
    }
    if (i >= 10) {
        printf("reset to romboot fail\r\n");
        return -1;
    }

    return 0;
}

/************************************************************************
* Name: fts_fwupg_enter_into_boot
* Brief: enter into boot environment, ready for upgrade
* Input:
* Output:
* Return: return 0 if success, otherwise return error code
***********************************************************************/
static int fts_fwupg_enter_into_boot(void)
{
    int ret = 0;
    u8 boot_id[2] = { 0 };

    /*software reset to boot mode*/
    if (tp_fw_valid) {
        ret = fts_write_reg(0xFC, 0xAA);
        if (ret < 0) {
            printf("write FC=0xAA fail\r\n");
            return ret;
        }
        fts_msleep(10);

        ret = fts_write_reg(0xFC, 0x55);
        if (ret < 0) {
            printf("write FC=0x55 fail\r\n");
            return ret;
        }

        fts_msleep(80);
    }

    /*confirm TP controller is int romboot state*/
    ret = fts_fwupg_get_boot_id(boot_id);

    if ((boot_id[0] != 0x62) || (boot_id[1] != 0x16)) {
        printf("tp isn't in romboot, need reset to romboot\r\n");
        ret = fts_fwupg_reset_to_romboot();
        if (ret < 0) {
            printf("reset to romboot fail\r\n");
            return ret;
        }
    }

    /*write pramboot.bin to pram*/
    ret = fts_pram_write_remap();
    if (ret < 0) {
        printf("pram write_remap fail\r\n");
        return ret;
    }

    return 0;
}

/************************************************************************
 * Name: fts_fwupg_erase
 * Brief: erase flash area
 * Input: delay - delay after erase
 * Output:
 * Return: return 0 if success, otherwise return error code
 ***********************************************************************/
static int fts_fwupg_erase(u32 delay)
{
    int ret = 0;
    u8 cmd = 0;
    bool flag = false;

    printf("**********erase now**********\r\n");
    /*send to erase flash*/
    cmd = 0x61;
    ret = fts_write(&cmd, 1, NULL, 0);
    if (ret < 0) {
        printf("erase cmd fail\r\n");
        return ret;
    }
    fts_msleep(delay);

    /* read status 0xF0AA: success */
    flag = fts_fwupg_check_flash_status(0xF0AA, 50, 400);
    if (!flag) {
        printf("ecc flash status check fail\r\n");
        return -1;
    }

    return 0;
}

/************************************************************************
 * Name: fts_flash_write_buf
 * Brief: write buf data to flash address
 * Input: @saddr - start address data write to flash
 *        @buf - data buffer
 *        @len - data length
 *        @delay - delay after write
 * Output:
 * Return: return data ecc of host if success, otherwise return error code
 ***********************************************************************/
static int fts_flash_write_buf(u32 saddr, u8 *buf, u32 len, u32 delay)
{
    int ret = 0;
    u32 i = 0;
    u32 j = 0;
    u32 packet_number = 0;
    u32 packet_len = 0;
    u32 addr = 0;
    u32 offset = 0;
    u32 remainder = 0;
    u8 packet_buf[FTS_FLASH_PACKET_LENGTH + 6] = { 0 };
    u8 cmd = 0;
    u8 val[2] = { 0 };
    u16 read_status = 0;
    u16 wr_ok = 0;

    printf("data buf start addr=0x%02x, len=0x%02x\r\n", saddr, len);
    packet_number = len / FTS_FLASH_PACKET_LENGTH;
    remainder = len % FTS_FLASH_PACKET_LENGTH;
    if (remainder > 0)
        packet_number++;
    packet_len = FTS_FLASH_PACKET_LENGTH;
    printf("write data, num:%d remainder:%d\r\n", packet_number, remainder);

    for (i = 0; i < packet_number; i++) {
        offset = i * FTS_FLASH_PACKET_LENGTH;
        addr = saddr + offset;

        /* last packet */
        if ((i == (packet_number - 1)) && remainder)
            packet_len = remainder;

        packet_buf[0] = 0xBF;
        packet_buf[1] = (addr >> 16);
        packet_buf[2] = (addr >> 8);
        packet_buf[3] = (addr);
        packet_buf[4] = (packet_len >> 8);
        packet_buf[5] = (packet_len);
        memcpy(&packet_buf[6], &buf[offset], packet_len);

        ret = fts_write(&packet_buf[0], 6, &packet_buf[6], packet_len);
        if (ret < 0) {
            printf("app write fail\r\n");
            return ret;
        }
        fts_msleep(delay);

        /* read status */
        wr_ok = 0x1000 + addr / packet_len;
        for (j = 0; j < 100; j++) {
            cmd = 0x6A;
            ret = fts_read(&cmd , 1, val, 2);
            read_status = (((u16)val[0]) << 8) + val[1];
            /*  printf("%02x %02x\r\n", wr_ok, read_status); */
            if (wr_ok == read_status) {
                break;
            }
            fts_msleep(1);
        }
    }

    return 0;
}

/************************************************************************
 * Name: fts_fwupg_ecc_cal_host
 * Brief: Calculate the ecc value of firmware program.
 * Input: @buf: pointer to firmware program
 *        @len: length of firmware program
 * Output:
 * Return:
 *  ecc value calculated in host using XOR8 algorithm
***********************************************************************/
static int fts_fwupg_ecc_cal_host(u8 *buf, u32 len)
{
    u8 ecc = 0;
    u32 i = 0;

    for (i = 0; i < len; i++) {
        ecc ^= buf[i];
    }

    return (int)ecc;
}

/************************************************************************
 * Name: fts_fwupg_ecc_cal
 * Brief: calculate and get ecc from tp
 * Input: @saddr - start address need calculate ecc
 *        @len - length need calculate ecc
 * Output:
 * Return: return data ecc of tp if success, otherwise return error code
 ***********************************************************************/
static int fts_fwupg_ecc_cal_tp(u32 saddr, u32 len)
{
    int ret = 0;
    u32 i = 0;
    u8 wbuf[7] = { 0 };
    u8 ecc;
    u32 packet_num = 0;
    u32 packet_len = 0;
    u32 remainder = 0;
    u32 addr = 0;
    u32 offset = 0;
    bool bflag = false;

    /* check sum init */
    wbuf[0] = 0x64;
    ret = fts_write(wbuf, 1, NULL, 0);
    if (ret < 0) {
        printf("ecc init cmd write fail\r\n");
        return ret;
    }

    packet_num = len / FTS_MAX_LEN_ECC_CALC;
    remainder = len % FTS_MAX_LEN_ECC_CALC;
    if (remainder)
        packet_num++;
    packet_len = FTS_MAX_LEN_ECC_CALC;
    printf("ecc calc num:%d, remainder:%d\r\n", packet_num, remainder);

    /* send commond to start checksum */
    wbuf[0] = 0x65;
    for (i = 0; i < packet_num; i++) {
        offset = FTS_MAX_LEN_ECC_CALC * i;
        addr = saddr + offset;
        if ((i == (packet_num - 1)) && remainder)
            packet_len = remainder;

        wbuf[1] = (addr >> 16);
        wbuf[2] = (addr >> 8);
        wbuf[3] = (addr);
        wbuf[4] = (packet_len >> 8);
        wbuf[5] = (packet_len);
        printf("ecc calc startaddr:0x%04x, len:%d\r\n", addr, packet_len);
        ret = fts_write(wbuf, 6, NULL, 0);
        if (ret < 0) {
            printf("ecc calc cmd write fail");
            return ret;
        }

        fts_msleep(packet_len / 256);
        /* read status if check sum is finished */
        bflag = fts_fwupg_check_flash_status(0xF055, 10, 50);
        if (!bflag) {
            printf("ecc flash status read fail\r\n");
            return -1;
        }
    }

    /* read out check sum */
    ret = fts_read_reg(0x66, &ecc);
    if (ret < 0) {
        printf( "ecc read cmd write fail\r\n");
        return ret;
    }

    return (int)ecc;
}


/*****************************************************************************
* Name: fts_ft7202_upgrade
* Brief:
*   Main flow of FT7202 firmware upgrade.
* Input:
*  @fw_file: firmware file buffer
*  @fw_file_size: firmware file size
* Output:
* Return:
*   Error code if upgrade fails, 0 if success.
*****************************************************************************/
static int fts_ft7202_upgrade(u8 *fw_file, u32 fw_file_size)
{
    int ret = 0;
    u8 rst_cmd = 0x07;
    u8 *firmware;
    u32 firmware_size;
    u32 start_address = 0;
    int ecc_in_host = 0;;
    int ecc_in_tp = 0;


    if (!fw_file || (fw_file_size < 0x120) || (fw_file_size > 64 * 1024)) {
        printf("fw file is null, or fw file size(%d) is invalid\r\n", fw_file_size);
        return -1;
    }

    /*get frmware data*/
    firmware = fw_file;
    firmware_size = fw_file_size;

    /*enter into boot environment*/
    ret = fts_fwupg_enter_into_boot();
    if (ret < 0) {
        printf("enter into boot fails\r\n");
        goto err_upgrade;
    }

    /*Erase firmware part in flash*/
    ret = fts_write_reg(0x09, 0x0B);
    if (ret < 0) {
        printf("upgrade mode(09) cmd write fail\r\n");
        goto err_upgrade;
    }

    ret = fts_fwupg_erase(60 * (firmware_size / 4096));
    if (ret < 0) {
        printf("erase cmd write fail\r\n");
        goto err_upgrade;
    }

    /*write firmware data into flash*/
    start_address = 0x0;
    ret = fts_flash_write_buf(start_address, firmware, firmware_size, 1);
    if (ret < 0 ) {
        printf("write buffer to flash fail\r\n");
        goto err_upgrade;
    }

    ecc_in_host = fts_fwupg_ecc_cal_host(firmware, firmware_size);
    ecc_in_tp = fts_fwupg_ecc_cal_tp(start_address, firmware_size);
    printf("ecc in tp:%02x, host:%02x\r\n", ecc_in_tp, ecc_in_host);
    if (ecc_in_tp != ecc_in_host) {
        printf("ecc check fail\r\n");
        goto err_upgrade;
    }

    printf("upgrade success, reset to normal boot\r\n");
    ret = fts_write(&rst_cmd, 1, NULL, 0);
    if (ret < 0) {
        printf("reset to normal boot fail\r\n");
    }
    fts_msleep(400);

    return 0;

err_upgrade:
    ret = fts_write(&rst_cmd, 1, NULL, 0);
    if (ret < 0) {
        printf("reset to normal boot fail\r\n");
    }
    return -10;
}




/*****************************************************************************
* Name: fts_fwupg_check_fw_valid
* Brief:
*   To check firmware in TP controller is valid or not.
* Input:
* Output:
* Return:
*   true: fw is valid, false: fw is invalid
*****************************************************************************/
static bool fts_fwupg_check_fw_valid(void)
{
    int ret = 0;
    int i = 0;
    u8 id = 0xFF;

    do {
        /*loop to read ID because some time is needed for TP controller initialization*/
        ret = fts_read_reg(0xA3, &id);
        if (id == 0x62) {
            printf("TP Ready,Device ID:0x%02x\r\n", id);
            return true;
        } else {
            printf("TP Not Ready,Read:0x%02x,ret:%d\r\n", id, ret);
        }
        fts_msleep(200);
    } while (i++ < 5);

    return false;
}

/*****************************************************************************
* Name: fts_fwupg_need_upgrade
* Brief:
*   To check firmware upgrade is needed or not. Host need upgrade firmware in
*   the following situations(one of them):
*     a. firwmare in TP controller is invalid.
*     b. firmware versions in TP controller and host are different.
* Input:
*  @fw_file: firmware file buffer
*  @fw_file_size: firmware file size
* Output:
* Return:
*   true: firmware upgrade is needed, false: not needed
*****************************************************************************/
static bool fts_fwupg_need_upgrade(u8 *fw_file, u32 fw_file_size)
{
    int ret = 0;
    u8 fw_ver_in_host = 0;
    u8 fw_ver_in_tp = 0;

    if (!tp_fw_valid) {
        printf("firmware in TP controller is invalid, need upgrade\r\n");
        return true;
    }

    ret = fts_read_reg(0xA6, &fw_ver_in_tp);
    if (ret < 0) {
        printf("read firmware version from reg0xA6 fals\r\n");
        return false;
    }

    if (fw_file_size > 0x120)
        fw_ver_in_host = fw_file[0x010E];
    printf("fw version in tp:%02x, host:%02x\r\n", fw_ver_in_tp, fw_ver_in_host);
    if (fw_ver_in_tp != fw_ver_in_host) {
        return true;
    }

    return false;
}


/*****************************************************************************
* Name: fts_fwupg_upgrade
* Brief:
*   The whole flow of firmware upgrade.
* Input:
*  @fw_file: firmware file buffer
*  @fw_file_size: firmware file size
* Output:
* Return:
*   Error code if upgrade fails, 0 if success.
*****************************************************************************/
static int fts_fwupg_upgrade(u8 *fw_file, u32 fw_file_size)
{
    int ret = 0;
    bool upgrade_flag = false;
    u8 fw_ver = 0;
    u8 upgrade_count = 0;

    /*check firmware upgrade is needed or not*/
    upgrade_flag = fts_fwupg_need_upgrade(fw_file, fw_file_size);
    printf("fw upgrade flag:%d\r\n", upgrade_flag);
    if (upgrade_flag) {
        do {
            upgrade_count++;
            ret = fts_ft7202_upgrade(fw_file, fw_file_size);
            if (ret >= 0) {
                fts_read_reg(0xA6, &fw_ver);
                printf("success upgrade to fw version %02x\r\n", fw_ver);
                break;
            }
        } while (upgrade_count < 2);
    }

    return ret;
}

int fts_fwupg_auto_upgrade(void)
{
    int ret = 0;
    u8 *fw_file = (u8 *)fw_file_ft7202;
    u32 fw_file_size = sizeof(fw_file_ft7202);
    rf_24G_deep_tick = 0;

    printf("********************auto upgrade********************\r\n");
    /* Get firmware file, and check firmware size
     * we use all.i as an example(variable fw_file_ft7202 stores the firmware).
     * You should read it if you use all.bin
     */
    if (!fw_file || (fw_file_size < 0x120)) {
        printf("fw_file is null, or firmare file size(%d) is invalid\r\n", fw_file_size);
        return -1;
    }

    /*check firmware is valid or not*/
    
    tp_fw_valid = fts_fwupg_check_fw_valid();
    if (!tp_fw_valid) {
        printf("firmware in flash is invalid\r\n");
    }

    /******run firmware upgrade flow******/
    ret = fts_fwupg_upgrade(fw_file, fw_file_size);
    if (ret < 0)
        printf("**********tp fw upgrade fails**********\r\n");
    else
        printf("**********tp fw no upgrade/upgrade success**********\r\n");
    
    rf_24G_deep_tick = clock_time();

    return ret;
}

#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
