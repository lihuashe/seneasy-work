/*********************************************************************
 * @file dump.h
 * @brief 
 * @version 1.0
 * @date 14/10/30 13:30:20
 * @author XIHAO
 *
 * @note 
 */

#ifndef __DUMP_H__
#define __DUMP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>

/*********************************************************************
 * MACROS
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
 * @brief dump_hci_cmd_opcode()
 *
 * @param[in] opcode  
 *
 * @return 
 **/
const char*  dump_hci_cmd_opcode(uint16_t opcode);

/**
 * @brief dump_hci_event_opcode()
 *
 * @param[in] opcode  
 *
 * @return 
 **/
const char*  dump_hci_event_opcode(uint8_t opcode);

/**
 * @brief dump_hci_err_code()
 *
 * @param[in] err_code  
 *
 * @return 
 **/
const char* dump_hci_err_code(uint8_t err_code);

/**
 * @brief hci_dump()
 *
 * @param[in] pdata  
 * @param[in] len  
 * @param[in] is_rcvd  
 *
 * @return 
 **/
void hci_dump(const uint8_t *pdata, uint16_t len, uint8_t is_rcvd);

#ifdef __cplusplus
}
#endif

#endif

