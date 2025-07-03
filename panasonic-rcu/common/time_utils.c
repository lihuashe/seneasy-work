#include "time_utils.h"
#include "py32f0xx_ll_utils.h"
#include "py32f0xx_ll_rcc.h"
#include "py32f0xx_ll_system.h"
#include "types.h"

/**
 * @brief Check if the time has exceeded the timeout threshold.
 * @param start_time The recorded start time (obtained by clock_time()).
 * @param timeout_ms The timeout threshold (unit: milliseconds).
 * @return 1 if timeout has occurred, 0 if not.
 */
bool clock_time_exceed(uint32_t start_time, uint32_t timeout_ms)
{
  return (((uint32_t)clock_time() - start_time) >= timeout_ms);
}

/**
 * @brief: 获取超时时间
 * @param {unsigned int} ref_ms
 * @return {unsigned int} ms
 */
inline unsigned int get_clock_exceed_time(unsigned int ref)
{
  return clock_time() < ref ? (0xFFFFFFFF - ref + clock_time()) : (clock_time() - ref);
}

void delay_us(uint32_t us)
{
  __IO uint32_t Delay = 1 + us * (SystemCoreClock / 24U) / 1000000U;
  do
  {
    __NOP();
  } while (Delay--);
}