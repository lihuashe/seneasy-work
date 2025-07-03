/*********************************************************************
 * @file co_port.h
 * @brief 
 * @version 1.0
 * @date Wed 12 Nov 2014 05:56:47 PM CST
 * @author XIHAO
 *
 * @note 
 */

#ifndef __CO_PORT_H__
#define __CO_PORT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "co.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERN VARIABLES
 */

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief co_port_init()
 *
 * @return 
 **/
void co_port_init(void);

/**
 * @brief co_sys_time()
 *
 * @return the current system tick, its uint is 312.5us in XH98xx
 **/
uint32_t co_sys_time(void);

/**
 * @brief co_sys_timer_start()
 *
 * @param[in] time  absolutely time
 *
 * @return 
 **/
void co_sys_timer_start(uint32_t time);

/**
 * @brief co_sys_timer_stop()
 *
 * @return 
 **/
void co_sys_timer_stop(void);

/**
 * @brief co_sys_timer_past()
 *
 * @param[in] time  
 *
 * @return 
 **/
bool co_sys_timer_past(uint32_t time);

/**
 * @brief co_stack_info()
 *
 * @param[in] base  
 * @param[in] size  
 *
 * @return 
 **/
void co_stack_info(uint8_t **base, uint32_t *size);

/**
 * @brief co_heap_info()
 *
 * @param[in] base  
 * @param[in] size  
 *
 * @return 
 **/
void co_heap_info(uint8_t **base, uint32_t *size);

/**
 * @brief co_sleep()
 *
 * @param[in] status  
 *
 * @return 
 **/
void co_sleep(co_power_status_t status);

/**
 * @brief  co power system status handler
 *
 * @return status
 **/
co_power_status_t co_power_system_status_handler(void);

#ifdef __cplusplus
}
#endif

#endif

