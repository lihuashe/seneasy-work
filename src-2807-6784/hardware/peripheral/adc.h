/**
 * @file adc.h
 * @brief ADC driver
 * @date Tue 16 May 2017 02:52:30 PM CST
 * @author XIHAO
 *
 * @defgroup ADC ADC
 * @ingroup PERIPHERAL
 * @brief ADC driver
 * @details
 *
 * General Purpose (GP) ADC
 *
 * The XH98xx is equipped with a high-speed low power 12-bit general purpose Analog-to-Digital Converter
 * (GPADC). It can operate in unipolar (single ended) mode as well as in bipolar (differential) mode.
 * The ADC has its own voltage regulator (LDO) of 1.2 V, which represents the full scale reference voltage.
 *
 * Features:
 * - 12-bit dynamic ADC with 65 ns conversion time
 * - Maximum sampling rate 3.3M sample/s
 * - Ultra-low power (5 uA typical supply current at 100k sample/s)
 * - Single-ended as well as differential input with two input scales
 * - Eight single-ended or four differential external input channels
 * - Battery monitoring function
 * - Chopper function
 * - Offset and zero scale adjust
 * - Common-mode input level adjust
 *
 * @{
 *
 * @example example_adc.c
 * This is an example of how to use the ADC to measure voltage.
 *
 *
 */

#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"
#include "cpft_sys_area.h"

/*********************************************************************
 * MACROS
 */
#define ADC_VREF_0P6V  0
#define ADC_VREF_1P25V 1
#define ADC_PGA_GAIN_0P125 0xC
/*********************************************************************
 * TYPEDEFS
 */
/* ADC channels */
typedef enum
{
    ADC_CH_0 = 0, // b0000 channel temperature
    ADC_CH_1 = 1, // b0001 channel vbat
    ADC_CH_2 = 2, // b0010 channel gpio<13>
    ADC_CH_3 = 3, // b0011 channel gpio<8>
    ADC_CH_4 = 4, // b0100 channel gpio<9>
    ADC_CH_5 = 5, // b0101 channel gpio<10>
    ADC_CH_6 = 6, // b0110 channel gpio<11>
    ADC_CH_7 = 7, // b0111 channel gpio<12>
    ADC_CH_8 = 8, // b1000 channel gpio<2>
    ADC_CH_9 = 9, // b1001 channel gpio<3>
    ADC_CH_MAX,
} adc_channel_t;

/* ADC sampling cycles */
typedef enum
{
    ADC_SAMPLING_CYCLES_32 = 1,
    ADC_SAMPLING_CYCLES_64 = 2,
    ADC_SAMPLING_CYCLES_128 = 3,
    ADC_SAMPLING_CYCLES_MAX,
} adc_sampling_cycles_t;

/* ADC temperature */
typedef enum
{
    ADC_TEMPER_20 = 0, // temperature 20C
    ADC_TEMPER_21 = 1, // temperature 21C
    ADC_TEMPER_22 = 2, // temperature 22C
    ADC_TEMPER_23 = 3, // temperature 23C
    ADC_TEMPER_24 = 4, // temperature 24C
    ADC_TEMPER_25 = 5, // temperature 25C
    ADC_TEMPER_26 = 6, // temperature 26C
    ADC_TEMPER_27 = 7, // temperature 27C
    ADC_TEMPER_MAX,
} adc_temper_refer_t;

/* ADC event callback */
typedef void (*adc_callback_t)(uint32_t event, int adc_data);

/* ADC dma configuration */
typedef struct
{
    /// DMA fifo enable
    bool use_fifo;
    /// Fifo buffer
    void *buffer;
    /// Fifo buffer length
    uint32_t buffer_len;
    /// DMA block link list item
    dma_llip_t *block_llip;
    /// DMA block link list item number
    uint32_t block_num;
    /// Event callback
    dma_callback_t callback;
} adc_dma_config_t;

typedef enum
{
    ADC_CALIB_CP = 0,
    ADC_CALIB_FT = 1,
} adc_calib_sel_t;

typedef struct {
    struct {
        float    gain_error;
        uint16_t vos;
        uint16_t vos_temp;
    }data[ADC_SAMPLING_CYCLES_MAX-1]; // [0]:32 [1]:64 [2]:128
} ft_flash_gpadc_calib_t;

typedef struct {
    struct {
        uint16_t code_0p6;
        uint16_t code_1p25;
    }data[ADC_SAMPLING_CYCLES_MAX-1]; // [0]:32 [1]:64 [2]:128
} ft_flash_gpadc_temp_calib_t;

typedef struct {
    uint16_t code_short[ADC_SAMPLING_CYCLES_MAX-1];
    float vout_vbat;
} ft_flash_gpadc_temp_calib_new_t;

typedef struct {
    uint16_t gain_error;
    uint16_t vos;
    uint16_t vos_temp;
} cp_efuse_gpadc_calib_t;

typedef struct {
    cp_efuse_gpadc_calib_t *efuse;
    ft_flash_gpadc_calib_t *flash;
    ft_flash_gpadc_temp_calib_t *flash_temp;
    ft_flash_gpadc_temp_calib_new_t  *flash_temp_new;
} cpft_adc_calib_t;

/*********************************************************************
 * EXTERN VARIABLES
 */

/*********************************************************************
 * EXTERN FUNCTIONS
 */
/*********************************************************************
 * @func adc_is_running()
         brief note: ADC is running or not

 * @parameters:
		 none

 * @return
         bool
 *********************************************************************/
bool adc_is_running(void);
    
/*********************************************************************
 * @func adc_open()
         brief note: ADC power on

 * @parameters:
         none

 * @return
         void
 *********************************************************************/
void adc_open(void);

/*********************************************************************
 * @func adc_close()
         brief note: ADC power down

 * @parameters:
         none

 * @return
         void
 *********************************************************************/
void adc_close(void);


#if ADC_USE_DEFAULT_CALIB_DATA
/*********************************************************************
 * @func adc_set_calibarate_param()
         brief note: adc config calibrate data

 * @parameters:
         cpft_adc_calib_t *config;

 * @return
         void
 *********************************************************************/
void adc_set_calibarate_param(cpft_adc_calib_t *config)
#endif

/*********************************************************************
 * @func adc_temperature_cal_by_sw()
         brief note: adc read by channel

 * @parameters:
         out: gpadc code
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         temperature
 *********************************************************************/
float adc_temperature_cal_by_sw(int out, adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_voltage_cal_by_sw()
         brief note: adc read by channel

 * @parameters:
         out: gpadc code
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         voltage (mV)
 *********************************************************************/
float adc_voltage_cal_by_sw(int out, adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_voltage_read_by_channel()
         brief note: adc read by channel

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         voltage (mV)
 *********************************************************************/
uint16_t adc_voltage_read_by_channel(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_vbat_voltage_read()
         brief note: adc read vbat voltage

 * @parameters:
         none 

 * @return
         voltage (mV)
 *********************************************************************/
uint16_t adc_vbat_voltage_read(void);

/*********************************************************************
 * @func adc_temperature_read_normal()
         brief note: adc temperature read

 * @parameters:
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         temperature
 *********************************************************************/
int adc_temperature_read_normal(adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_temperature_read()
         brief note: adc temperature read for system init

 * @parameters:
         none

 * @return
         temperature
 *********************************************************************/
int adc_temperature_read(void);

#ifdef ADC_USE_CALIB
/*********************************************************************
 * @func adc_calib_first_step()
         brief note: calib the gpadc according to the cmd from FT, the first step use 0.8V on GPIO3

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

 * @return
         bool: false: fail   true: succ
 *********************************************************************/
bool adc_calib_first_step(adc_channel_t channel);

/*********************************************************************
 * @func adc_calib_second_step()
         brief note: calib the gpadc according to the cmd from FT

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         temper:    0x0: 20'C    0x4: 24'C
                    0x1: 21'C    0x5: 25'C
                    0x2: 22'C    0x6: 26'C
                    0x3: 23'C    0x7: 27'C
         calib_sel: 0x0: ADC_CALIB_CP
                    0x1: ADC_CALIB_FT

 * @output
         void* pCalibDataSave

 * @return
         bool: false: fail   true: succ
 *********************************************************************/
bool adc_calib_second_step(adc_channel_t channel, adc_temper_refer_t temp, adc_calib_sel_t calib_sel, void *pCalibDataSave);

/*********************************************************************
 * @func adc_calib_third_step()
         brief note: start adc to get vbat output volt and a output volt value and judge the result

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         int inputVoltValue :input volt at the unit of micro v

 * @return
         bool: false: fail   true: succ
 *********************************************************************/
bool adc_calib_third_step(adc_channel_t channel, int inputVoltValue);

/*********************************************************************
 * @func adc_calib_compen_step()
         brief note: recording the gpadc compensation calibration param

 * @parameters:
         calib_sel: 0x0: ADC_CALIB_CP
                    0x1: ADC_CALIB_FT

 * @output
         cpft_sys_area_t *pCalibDataSave

 * @return
         bool: false: fail   true: succ
 *********************************************************************/
bool adc_calib_compen_step(adc_calib_sel_t calib_sel, cpft_sys_area_t *pCalibDataSave);
#endif

/*********************************************************************
 * @func adc_voltage_read_single_begin_nostop()
         brief note: read voltage single channel nostop begin

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         void
 *********************************************************************/
void adc_voltage_read_single_begin_nostop(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_voltage_read_single_nostop()
         brief note: read voltage single channel nostop

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

 * @return
         voltage (mV)
 *********************************************************************/
uint16_t adc_voltage_read_single_nostop(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_voltage_read_multi_begin_nostop()
         brief note: read voltage multi channel nostop begin

 * @parameters:
         chBmp:     channel select

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         void
 *********************************************************************/
void adc_voltage_read_multi_begin_nostop(uint16_t chBmp, adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_voltage_read_multi_nostop()
         brief note: read voltage multi channel nostop

 * @parameters:
         chBmp:     channel select

         cha_val:   voltage (mV)

 * @return
         void
 *********************************************************************/
void adc_voltage_read_multi_nostop(uint16_t chBmp, float *cha_val, adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_temperature_read_begin_nostop()
         brief note: read temperature nostop begin

 * @parameters:
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

 * @return
         void
 *********************************************************************/
void adc_temperature_read_begin_nostop(adc_sampling_cycles_t sampling_cycles);

/*********************************************************************
 * @func adc_temperature_read_nostop()
         brief note: read temperature nostop

 * @parameters:
         none

 * @return
         temperature
 *********************************************************************/
int adc_temperature_read_nostop(adc_sampling_cycles_t sampling_cycles);

#ifdef ADC_DMA_ENALBE
/*********************************************************************
 * @func adc_dma_config()
         brief note: ADC DMA configuration

 * @parameters:
         dma: NULL: ADC will allocate a new dma; Not NULL: use it as adc dma

         config: ADC dma configuration structure

         src_addr: the adc output address

 * @return
         NULL No DMA valid or error happen
         object Current used DMA object
 *********************************************************************/
XH_DMA_CH_Type *adc_dma_config(XH_DMA_CH_Type *dma, const adc_dma_config_t *config, uint32_t *src_addr);
#endif

/*********************************************************************
 * @func adc_voltage_read_by_channel_irq()
         brief note: use irq handle data

 * @parameters:
         channel:   0x0: temperature    0x4: gpio<9>    0x8: gpio<2>
                    0x1: vabt           0x5: gpio<10>   0x9: gpio<3>
                    0x2: gpio<13>       0x6: gpio<11>
                    0x3: gpio<8>        0x7: gpio<12>

         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

         cb:  adc_callback

 * @return
         bool
 *********************************************************************/
bool adc_voltage_read_by_channel_irq(adc_channel_t channel, adc_sampling_cycles_t sampling_cycles, adc_callback_t cb);

/*********************************************************************
 * @func adc_temperature_read_irq()
         brief note: use irq handle data

 * @parameters:
         sampling_cycles:   0x1: 32cycles
                            0x2: 64cycles
                            0x3: 128cycles

         cb:  adc_callback

 * @return
         bool
 *********************************************************************/
bool adc_temperature_read_irq(adc_sampling_cycles_t sampling_cycles, adc_callback_t cb);



#ifdef __cplusplus
}
#endif

#endif

/** @} */
