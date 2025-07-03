/**
 * @file hw_radio.h
 * @brief 
 * @date Mon 07 Dec 2015 05:51:07 PM CST
 * @author XIHAO
 *
 * @addtogroup 
 * @ingroup 
 * @details 
 *
 * @{
 */

#ifndef __HW_RADIO_H__
#define __HW_RADIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
    RF_TX_POWER_7DBM    = 100,
    RF_TX_POWER_5DBM    = 18,
    RF_TX_POWER_4DBM    = 12,
    RF_TX_POWER_2DBM    = 9,
    RF_TX_POWER_1DBM    = 8,
    RF_TX_POWER_0DBM    = 7,
    RF_TX_POWER_N5DBM   = 4,
    RF_TX_POWER_N20DBM  = 2,
    RF_TX_POWER_N28DBM  = 1, // -25

    // Humanized description
    RF_TX_POWER_MAX    = RF_TX_POWER_7DBM,
    RF_TX_POWER_MIN    = RF_TX_POWER_0DBM,
    RF_TX_POWER_NORMAL = RF_TX_POWER_7DBM,//RF_TX_POWER_0DBM,
}rf_tx_power_t;

typedef void (*rf_event_callback_t)(void);

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief rf_tx_power_set()
 *
 * @param[in] power  
 *
 * @return 
 **/
void rf_tx_power_set(rf_tx_power_t power);

/**
 * @brief rf_tx_power_get()
 *
 * @return 
 **/
rf_tx_power_t rf_tx_power_get(void);

/**
 * @brief rf_freq_offset_get()
 *
 * Capacity: -250kHz ~ +250kHz, +-10kHz
 *
 * @return 
 **/
int16_t rf_freq_offset_get(void);

/**
 * @brief rf_single_tone_enable()
 *
 * @param[in] enable  true or false
 * @param[in] freq  2402 ...
 * @param[in] payload  payload
 *
 * @return None
 **/
void rf_single_tone_enable(bool enable, uint32_t freq, uint8_t payload);

/**
 * @brief rf carrier enable
 *
 * @param[in] enable  true or false
 * @param[in] freq  2402 ...
 *
 * @return None
 **/
void rf_carrier_enable(bool enable, uint32_t freq);

/**
 * @brief  rf full rx enable
 *
 * @param[in] enable  enable
 * @param[in] freq  2402MHz ... 2480MHz
 **/
void rf_full_rx_enable(bool enable, uint32_t freq);

/**
 * @brief  rf txrx pin enable
 *
 * @param[in] enable  enable
 * @param[in] tx_pin  tx pin
 * @param[in] rx_pin  rx pin
 * @param[in] pol  polarity, 0 or 1
 **/
void rf_txrx_pin_enable(bool enable, int tx_pin, int rx_pin, int pol);

/**
 * @brief  rf event init
 **/
void rf_event_init(void);

/**
 * @brief rf enable event register
 *
 * @param[in] callback  event callback
 *
 * @return None
 **/
void rf_event_register(rf_event_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
