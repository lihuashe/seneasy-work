/******************************************************************************
 * @file     drivers.h
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

#include "config.h"

#if(__TL_LIB_8255__ || (MCU_CORE_TYPE == MCU_CORE_8255))
#include "drivers/8255/driver_8255.h"
#elif(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258))
#include "drivers/8258/driver_8258.h"
#elif(__TL_LIB_8278__ || (MCU_CORE_TYPE == MCU_CORE_8278))
#include "drivers/8278/driver_8278.h"
#else
#include "drivers/8258/driver_8258.h"
#endif
