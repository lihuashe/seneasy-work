/**
  @addtogroup xh_soc_api
  @{
  @defgroup xh_soc_error SoC Library Error Codes
  @{
     
  @brief Error definitions for the SoC library

*/

/* Header guard */
#ifndef XH_ERROR_SOC_H__
#define XH_ERROR_SOC_H__

#include "xh_error.h"

/* Mutex Errors */
#define XH_ERROR_SOC_MUTEX_ALREADY_TAKEN                 (XH_ERROR_SOC_BASE_NUM + 0)  ///< Mutex already taken

/* NVIC errors */
#define XH_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE        (XH_ERROR_SOC_BASE_NUM + 1)  ///< NVIC interrupt not available
#define XH_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED (XH_ERROR_SOC_BASE_NUM + 2)  ///< NVIC interrupt priority not allowed
#define XH_ERROR_SOC_NVIC_SHOULD_NOT_RETURN              (XH_ERROR_SOC_BASE_NUM + 3)  ///< NVIC should not return

/* Power errors */
#define XH_ERROR_SOC_POWER_MODE_UNKNOWN                  (XH_ERROR_SOC_BASE_NUM + 4)  ///< Power mode unknown
#define XH_ERROR_SOC_POWER_POF_THRESHOLD_UNKNOWN         (XH_ERROR_SOC_BASE_NUM + 5)  ///< Power POF threshold unknown
#define XH_ERROR_SOC_POWER_OFF_SHOULD_NOT_RETURN         (XH_ERROR_SOC_BASE_NUM + 6)  ///< Power off should not return

/* Rand errors */
#define XH_ERROR_SOC_RAND_NOT_ENOUGH_VALUES              (XH_ERROR_SOC_BASE_NUM + 7)  ///< RAND not enough values

/* PPI errors */
#define XH_ERROR_SOC_PPI_INVALID_CHANNEL                 (XH_ERROR_SOC_BASE_NUM + 8)  ///< Invalid PPI Channel
#define XH_ERROR_SOC_PPI_INVALID_GROUP                   (XH_ERROR_SOC_BASE_NUM + 9)  ///< Invalid PPI Group

#endif // XH_ERROR_SOC_H__
/**
  @}
  @}
*/
