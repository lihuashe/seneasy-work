#ifndef __TIMER_DMA_H__
#define __TIMER_DMA_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"
#include "xh98xx_config.h"
#include "dma.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
#define TIM_STRUCT2INDEX(tim)   ((((uint32_t)tim) - XH_TIM0_BASE) / (XH_TIM1_BASE - XH_TIM0_BASE))
#define TIM_TEST_BUF_SIZE       (8*4)

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */
void print_buf(void *buf, int length, char *title);
void cmd_dma_mem_to_tim_comm(XH_TIM_Type *timer);
void dma_timer(void);




#ifdef __cplusplus
}
#endif

#endif

/** @} */

