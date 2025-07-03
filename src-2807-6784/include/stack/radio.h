/**
 * @file radio.h
 * @brief 
 * @date Tue 08 Dec 2015 02:21:58 PM CST
 * @author XIHAO
 *
 * @addtogroup 
 * @ingroup 
 * @details 
 *
 * @{
 */

#ifndef __RADIO_H__
#define __RADIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "features.h"
#include "hw_radio.h"
#include "hw_radio.h"
//#include "hw_transceiver.h"

/*********************************************************************
 * MACROS
 */

#define rf_init()               HWradio_Initialise()
#define rf_enable(a)            HWradio_Enable(a)
#define rf_service(a, b, c)     HWradio_LE_Service(a, b, c)
//#define rf_tx_complete()        HWradio_TxComplete()
#define rf_rx_complete()        HWradio_LE_RxComplete()
#define rf_setup_tifs(a)        HWradio_Setup_For_TIFS_Event(a)
#define rf_setup_next_txrx(a)   HWradio_LE_Setup_Radio_For_Next_TXRX(a)
#define rf_read_rssi()          HWradio_LE_Read_RSSI()
#define rf_cleanup()            HWradio_CleanUpRx()
#ifdef CONFIG_FPGA
#define rf_disable_all_spi()    HWradio_DisableAllSpiWrites()
#else
#define rf_disable_all_spi()
#endif

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERN VARIABLES
 */

/*********************************************************************
 * EXTERN FUNCTIONS
 */

#ifdef __cplusplus
}
#endif

#endif

/** @} */

