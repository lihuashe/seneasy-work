/********************************************************************************************************
 * @file	calibration.h
 *
 * @brief	This is the header file for B85m
 *
 * @author	Driver Group
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

/**
 *  For flash packaged chips: users cannot use OTP (because all OTP areas will not be verified when the chip leaves the factory, and all areas cannot be guaranteed to be available).
 */
#define OTP_PACKAGE		1
#define FLASH_PACKAGE	2
#if _CHIP_IS_OTP_
#define PACKAGE_TYPE	OTP_PACKAGE
#else
#define PACKAGE_TYPE	FLASH_PACKAGE
#endif
/*
 * For otp products, if all codes cannot be executed in ram code, there is a risk of crash,
 * It is necessary to calibrate the voltage of VDD_1V2 to reduce the risk,through this macro distinction, it can distinguish whether it is a program with full ram code.
 */
#define OTP_ALL_SRAM_CODE    0

#ifndef FLASH_ADC_VREF_CALIB_ADDR_64K
#define FLASH_ADC_VREF_CALIB_ADDR_64K	0xe0c7
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_128K
#define FLASH_ADC_VREF_CALIB_ADDR_128K	0x1e0c7
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_512K
#define FLASH_ADC_VREF_CALIB_ADDR_512K	0x770c7
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_1M
#define FLASH_ADC_VREF_CALIB_ADDR_1M	0xfe0c7
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_2M
#define FLASH_ADC_VREF_CALIB_ADDR_2M	0x1fe0c7
#endif

#ifndef FLASH_ADC_VREF_CALIB_ADDR_4M
#define FLASH_ADC_VREF_CALIB_ADDR_4M	0x3fe0c7
#endif


#ifndef FLASH_CAP_VALUE_ADDR_64K
#define FLASH_CAP_VALUE_ADDR_64K		0xe000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_128K
#define FLASH_CAP_VALUE_ADDR_128K		0x1e000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_512K
#define FLASH_CAP_VALUE_ADDR_512K		0x7e000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_1M
#define FLASH_CAP_VALUE_ADDR_1M			0xfe000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_2M
#define FLASH_CAP_VALUE_ADDR_2M			0x1fe000
#endif

#ifndef FLASH_CAP_VALUE_ADDR_4M
#define FLASH_CAP_VALUE_ADDR_4M			0x3fe000
#endif


#ifndef OTP_ADC_VREF_FT_CALIB_ADDR
#define OTP_ADC_VREF_FT_CALIB_ADDR			0x3fcc
#endif

#ifndef OTP_ADC_VREF_CP_CALIB_ADDR
#define OTP_ADC_VREF_CP_CALIB_ADDR			0x3fd0
#endif

#ifndef OTP_CAP_VALUE_ADDR
#define OTP_CAP_VALUE_ADDR					0x3fc8
#endif

#ifndef OTP_VDD_1V2_CALIB_ADDR
#define OTP_VDD_1V2_CALIB_ADDR             0x3fc0
#endif

/**
 * @brief		This function is used to calibrate the user's parameters.
 * 				This function is to read the calibration value stored in otp,
 * 				and use the calibration value to configure the chip to improve chip performance.
 * 				(reduce adc measurement error, reduce frequency offset, etc.)
 * @return		none.
 */
void user_read_otp_value_calib(void);

/**
 * @brief		This function is used to calibrate the user's parameters.
 * 				This function is to read the calibration value stored in flash,
 * 				and use the calibration value to configure the chip to improve chip performance.
 * 				(reduce adc measurement error, reduce frequency offset, etc.)
 * @return		none.
 */
void user_read_flash_value_calib(void);

#endif

