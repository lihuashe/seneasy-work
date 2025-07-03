/**********************************************************************************************************************
 * @file     hw_cpto.h
 * @version  V1.0
 * @date     2022/02/18
 * @history
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 **********************************************************************************************************************/

#ifndef __HW_CPTO_H__
#define __HW_CPTO_H__

#ifdef __cplusplus
#if __cplusplus
 extern "C" {
#endif
#endif /* __cplusplus */




/**
 * @brief  128bit AES encrypt.
 * @param  pu32Key: Key used for AES algorithm, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_aes_encrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut);


/**
 * @brief  128bit AES decrypt.
 * @param  pu32Key: Key used for AES algorithm, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_aes_decrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut);


/**
 * @brief  192bit AES encrypt.
 * @param  pu32Key: Key used for AES algorithm, its length should be 24Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_aes192_encrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut);


/**
 * @brief  192bit AES decrypt.
 * @param  pu32Key: Key used for AES algorithm, its length should be 24Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_aes192_decrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut);


/**
 * @brief  256bit AES encrypt.
 * @param  pu32Key: Key used for AES algorithm, its length should be 32Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_aes256_encrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut);


/**
 * @brief  256bit AES decrypt.
 * @param  pu32Key: Key used for AES algorithm, its length should be 32Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_aes256_decrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut);


/**
 * @brief  cmac decrypt.
 * @param  pu32Key: Key used for CMAC algorithm, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length can be 0, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @param  u32DataInLen: length of the output buffer.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_cmac_encrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut,
                                        uint32_t u32DataInLen);


/**
 * @brief  ECB encrypt.
 * @param  pu32Key: Key used for ECB algorithm, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ecb_encrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut);


/**
 * @brief  CBC encrypt.
 * @param  pu32Key: Key used for CBC algorithm, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @param  pu32IV: pointer to the IV buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_cbc_encrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut, uint32_t *pu32IV);


/**
 * @brief  CTR encrypt.
 * @param  pu32Key: Key used for CTR algorithm, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @param  pu32IV: pointer to the IV buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ctr_encrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut, uint32_t *pu32IV);


/**
 * @brief  CTR decrypt.
 * @param  pu32Key: Key used for CTR algorithm, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: pointer to the input buffer, its length should be 16Bytes, little endian.
 * @param  pu32DataOut: pointer to the output buffer, its length should be 16Bytes, little endian.
 * @param  pu32IV: pointer to the IV buffer, its length should be 16Bytes, little endian.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ctr_decrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut, uint32_t *pu32IV);


/**
 * @brief  config parameter for CCM encrypt/decrypt.
 * @param  u8CcmCfg: bit 4 directionbit 1: M->S 0: S->M;[3:2]llid of PDU header.
 * @param  pu32IV: IV, 8bytes.
 * @param  pu32EncPktCnt: Encrypt packet counter, only for encryption.
 * @param  pu32DecPktCnt: Decrypt packet counter, only for decryption.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_cpto_config_for_ccm(uint8_t u8CcmCfg, uint32_t* pu32IV, uint32_t* pu32EncPktCnt,
                                               uint32_t* pu32DecPktCnt);


/**
 * @brief  CCM encrypt.
 * @param  pu32Key: ccm session key, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: data to be encrypted, little endian.
 * @param  pu32DataOut: buffer for plaint data after encrypted, little endian.
 * @param  u16DataInlen: length of pu32DataIn.
 * @param  pu32GeneratedMIC: mic.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ccm_encrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut,
                                       uint16_t u16DataInlen, uint32_t* pu32GeneratedMIC);


/**
 * @brief  CCM decrypt.
 * @param  pu32Key: ccm session key, its length should be 16Bytes, little endian.
 * @param  pu32DataIn: data to be decrypted,  little endian.
 * @param  pu32DataOut: buffer for plaint data after decrypted, little endian.
 * @param  u16DataInlen: length of pu32DataIn.
 * @param  pu32GeneratedMIC: mic.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_ccm_decrypt(uint32_t* pu32Key, uint32_t* pu32DataIn, uint32_t* pu32DataOut,
                                       uint16_t u16DataInlen, uint32_t* pu32GeneratedMIC);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HW_CPTO_H__ */

