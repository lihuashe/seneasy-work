/*********************************************************************
 * @file hci_pack.h
 * @brief 
 * @version 1.0
 * @date Tue 18 Nov 2014 05:00:52 PM CST
 * @author XIHAO
 *
 * @note 
 */

#ifndef __HCI_PACK_H__
#define __HCI_PACK_H__

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
 * @brief hci_pack(), Pack a RAW HCI packet from a message
 *
 * @param[in] msg msg->param, it define in co_msg_hci_param.h
 *
 * @return None
 **/
co_msg_t *hci_pack(co_msg_t * const msg);


#ifdef __cplusplus
}
#endif

#endif

