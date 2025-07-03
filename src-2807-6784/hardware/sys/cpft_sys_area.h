/**
 * @file cpft_sys_area.h
 * @brief 
 * @date Wed 14 Oct 2020 03:03:24 PM CST
 * @author XIHAO
 *
 * @defgroup 
 * @ingroup 
 * @brief 
 * @details 
 *
 * @{
 */

#ifndef _CPFT_SYS_AREA_H_
#define _CPFT_SYS_AREA_H_

/*********************************************************************
 * INCLUDES
 */
#include "otp.h"

/*********************************************************************
 * MACROS
 */
#define CPFT_DATA_EFUSE    128
#define CPFT_DATA_FLASH    ((0x00400000 + sfs_capacity()) - sizeof(cpft_sys_area_t))

/*********************************************************************
 * TYPEDEFS
 */

//type definition of calibration valid bitmap
typedef union {
    uint8_t data;
    struct {
        uint8_t calib32m_valid:1;
        uint8_t adc_voltage_valid:1;
        uint8_t temp_compensation_valid:1;
        uint8_t reserved:2;
        uint8_t customer:3;
    };
} cpft_bitmap_t;

typedef union {
    uint32_t data;
    struct {
        uint32_t rxadc_1p2:2;
        uint32_t ldo_pfd:2;
        uint32_t trim_vref:4;         // Voltage
        uint32_t pa_ldo:4;            // Voltage
        uint32_t dig_ldo:4;           // Voltage
        uint32_t pkdadc_trim_vbias:3; // Voltage
        uint32_t dcdc_vout:5;         // Voltage
        uint32_t vco_ldo:3;           // Voltage
        uint32_t au_sel_bias:5;       // Voltage
    };
} cpft_voltage_calib_t;

typedef union {
    uint32_t data;
    struct {
        uint32_t gain_error:10; // ADC
        uint32_t vos:14;        // ADC
        uint8_t  vos_temp:8;    // ADC
    };
} cpft_gpadc_calib_t;

typedef struct {
    cpft_voltage_calib_t voltage;
    cpft_gpadc_calib_t gpadc;
    uint8_t calib32m;
    cpft_bitmap_t bitmap;
    uint8_t uuid[6];
} __PACKED cpft_sys_area_t;

#endif

