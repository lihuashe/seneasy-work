 /**
  @addtogroup xh_sdm_api
  @{
  @defgroup xh_sdm_error SoftDevice Manager Error Codes
  @{
     
  @brief Error definitions for the SDM API
*/

/* Header guard */
#ifndef XH_ERROR_SDM_H__
#define XH_ERROR_SDM_H__

#include "xh_error.h"

#define XH_ERROR_SDM_LFCLK_SOURCE_UNKNOWN              (XH_ERROR_SDM_BASE_NUM + 0)  ///< Unknown lfclk source.
#define XH_ERROR_SDM_INCORRECT_INTERRUPT_CONFIGURATION (XH_ERROR_SDM_BASE_NUM + 1)  ///< Incorrect interrupt configuration (can be caused by using illegal priority levels, or having enabled SoftDevice interrupts).
#define XH_ERROR_SDM_INCORRECT_CLENR0                  (XH_ERROR_SDM_BASE_NUM + 2)  ///< Incorrect CLENR0 (can be caused by erronous SoftDevice flashing).

#endif // XH_ERROR_SDM_H__

/**
  @}
  @}
*/
