/******************************************************************************
 * @file     random.h
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

#pragma once

//(By lirui)
#define  TRNG_MODE_ENABLE           1



/**
 * @brief     This function performs to get one random number.
 *            TRNG Base address is 0x4400,read 4bytes one time to judge whether generator success.
 *            and get 8bytes later to return the random value in order to sure the value is right.
 * @param[in] none.
 * @return    the value of one random number.
 */
void random_generator_init(void);

/**
 * @brief     This function performs to get one random number
 * @param[in] none.
 * @return    the value of one random number.
 */
unsigned int rand(void);



/**
 * @brief This function serves to set adc sampling and get results.
 * @param[in]  none.
 * @return the result of sampling.
 */
unsigned short adc_rng_result(void);


/**
 * @brief This function is used for ADC configuration of ADC supply voltage sampling.
 * @return none
 */
void rng_init(void);



/**
 * @brief      This function performs to get a serial of random number.
 * @param[in]  len- the length of random number
 * @param[in]  data - the first address of buffer store random number in
 * @return     the result of a serial of random number..
 */
void generateRandomNum(int len, unsigned char *data);


