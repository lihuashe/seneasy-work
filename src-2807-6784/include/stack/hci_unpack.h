/*********************************************************************
 * @file hci_unpack.h
 * @brief 
 * @version 1.0
 * @date Tue 18 Nov 2014 05:03:09 PM CST
 * @author XIHAO
 *
 * @note 
 */

#ifndef __HCI_UNPACK_H__
#define __HCI_UNPACK_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */


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
 * @brief hci_unpack() Unpack a RAW HCI packet to a message and sent it
 *
 * @param[in] msg  msg->param, The RAW HCI data, eg: 01 03 0C 00 (RESET Command)
 *
 * @return unpack msg
 **/
co_msg_t * hci_unpack(co_msg_t * const msg);

#ifdef __cplusplus
}
#endif

#endif

