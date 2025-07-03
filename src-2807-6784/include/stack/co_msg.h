/*********************************************************************
 * @file co_msg.h
 * @brief 
 * @version 1.0
 * @date 14/11/3 19:34:18
 * @author XIHAO
 *
 * @note 
 */

#ifndef __CO_MSG_H__
#define __CO_MSG_H__

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

#define CO_MSG_LENGTH_MAX           (CONFIG_LE_ACL_PKT_SIZE + 32)

#define CO_MSG_STATUS_SHOULD_FREE   0x01
#define CO_MSG_STATUS_IS_SAVED      0x02

#define CO_MSG_GET_PARAM(msg)       ((msg)->param)

#define CO_MSG_BUFFER_SIZE(param_size)  (sizeof(co_msg_t) + (param_size))

// msg alloc
#define CO_MSG_ALLOC(id, dst, src, param_type) \
    (param_type *)co_msg_alloc(id, dst, src, sizeof(param_type))

// msg alloc declare
#define CO_MSG_ALLOC_DECLARE(param, id, dst, src, param_type) \
    param_type *param = (param_type *)co_msg_alloc(id, dst, src, sizeof(param_type))

// msg alloc declare with length
#define CO_MSG_ALLOC_DECLARE_LENGTH(param, id, dst, src, param_type, length) \
    param_type *param = (param_type *)co_msg_alloc(id, dst, src, length)

// msg local var declare
#define CO_MSG_LOCAL_DECLARE(param, id, dst, src, param_type) \
    do { \
        CO_ALIGN(4) uint8_t __msg_buffer##param[sizeof(co_msg_t) + sizeof(param_type)]; \
        param_type *param = (param_type *)co_msg_build((co_msg_t *)__msg_buffer##param, id, dst, src, sizeof(param_type))

// msg local var declare with length
#define CO_MSG_LOCAL_DECLARE_LENGTH(param, id, dst, src, param_type, length) \
    do { \
        co_assert(length <= CO_MSG_LENGTH_MAX); \
        CO_ALIGN(4) uint8_t __msg_buffer##param[sizeof(co_msg_t) + CO_MSG_LENGTH_MAX]; \
        param_type *param = (param_type *)co_msg_build((co_msg_t *)__msg_buffer##param, id, dst, src, length)

// msg local execute
#define CO_MSG_LOCAL_EXECUTE(param) \
        co_msg_execute(param, false/*is_msg_alloc*/); \
    } while (0)

/*********************************************************************
 * TYPEDEFS
 */

typedef enum
{
    LAYER_NULL,
    LAYER_BB,
    LAYER_BLI,
    LAYER_LL,
    LAYER_HCI,
    LAYER_VHCI,
    LAYER_GAP,
    LAYER_L2CAP,
    LAYER_SMP,
    LAYER_ATT,
    LAYER_GATT,
    LAYER_ACI,
    LAYER_IOT,
    LAYER_BSP,
    LAYER_PROFILE,
    LAYER_NRF,
    LAYER_APP,
    LAYER_USR,
    LAYER_DEBUG,
    LAYER_MAX,
}co_layer_id_t;

/**
 * @brief co_msg_callback_t()
 *
 * @param[in] id  
 * @param[in] dst  
 * @param[in] src  
 * @param[in] param  
 *
 * @return 
 **/
typedef void (*co_msg_callback_t)(co_msg_id_t id, co_layer_id_t dst, co_layer_id_t src, void *param);

typedef struct
{
    co_q_t q;

    uint8_t status;

    co_msg_id_t   id;
    co_layer_id_t src;
    co_layer_id_t dst;

#ifdef CONFIG_ASSERT
    uint16_t buffer_len;
#endif

    //
    // NOTE:
    //
    // There are (2+5) extra bytes:
    //  1: reserve
    //  5: header[5]
    //  1: buffer[1]
    // =>
    //  1byte : aligning & reserve & may save offset
    //  5byte : pack for hci/aci header & may save offset
    //  nbyte : param data
    //  1byte : guard
    //
    // NOTE:
    //  The offset is relatived to 'header', and save on (param-1) place (1byte space)
    //

    uint8_t *param;
    uint16_t param_len;
    uint8_t  reserve;
    uint8_t  header[1+4];

    //is align
    uint8_t  buffer[1];
}co_msg_t;


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */
/**
 * @brief co_msg_init()
 *
 * @return 
 **/
void co_msg_init(void);

/**
 * @brief  co msg build
 *
 * @param[in] msg  msg
 * @param[in] id  id
 * @param[in] dst  dst
 * @param[in] src  src
 * @param[in] param_len  param len
 **/
void *co_msg_build(co_msg_t *msg, co_msg_id_t id, co_layer_id_t dst, co_layer_id_t src, uint16_t param_len);

/**
 * @brief co_msg_alloc(), Alloc a message
 *
 * @param[in] id  Message id
 * @param[in] dst  The destination layer
 * @param[in] src  The source layer
 * @param[in] param_len  Parameter length
 *
 * @return The pointer of parameter
 **/
void *co_msg_alloc(co_msg_id_t id, co_layer_id_t dst, co_layer_id_t src, uint16_t param_len);

/**
 * @brief co_msg_send(), Send the message
 *
 * @param[in] param  The pointer of parameter, it is alloced by co_msg_alloc()
 *
 * @return None
 **/
void co_msg_send(const void *param);

/**
 * @brief co_msg_send_front()
 *
 * @param[in] param  
 *
 * @return 
 **/
void co_msg_send_front(const void *param);

/**
 * @brief  co msg execute
 *
 * @param[in] msg  msg
 **/
void co_msg_execute(const void *param, bool is_msg_alloc);

/**
 * @brief co_msg_handle_synchronously()
 *
 * @param[in] param  want to handle massage parametter
 * @param[in] wait_msg_id  expact to wait massage, and this msg dst_id is the src_id of 'param'.
 *
 * @return The  return parametter of waiting massage
 **/
const void *co_msg_handle_synchronously(const void *param, co_msg_id_t wait_msg_id);

/**
 * @brief co_msg_save()
 *
 * @param[in] param  
 * @param[in] save_q  
 *
 * @return 
 **/
void co_msg_save(const void *param, co_q_t *save_q);

/**
 * @brief co_msg_is_saved()
 *
 * @param[in] param  
 *
 * @return 
 **/
bool co_msg_is_saved(const void *param);

/**
 * @brief co_msg_save_send()
 *
 * @param[in] save_q_item  
 *
 * @return 
 **/
void co_msg_save_send(co_q_t *save_q_item);

/**
 * @brief co_msg_save_send_all()
 *
 * @param[in] save_q  
 *
 * @return 
 **/
void co_msg_save_send_all(co_q_t *save_q);

/**
 * @brief co_msg_save_send2null()
 *
 * @param[in] save_q_item  
 *
 * @return 
 **/
void co_msg_save_send2null(co_q_t *save_q_item);

/**
 * @brief co_msg_save_free()
 *
 * @param[in] save_q_item  
 *
 * @return 
 **/
void co_msg_save_free(co_q_t *save_q_item);

/**
 * @brief co_msg_free()
 *
 * @param[in] param  
 *
 * @return 
 **/
void co_msg_free(void *param);

/**
 * @brief co_set_workmode()
 *
 * @param[in] workmode  
 *
 * @return 
 **/
void co_set_workmode(co_workmode_t workmode);

/**
 * @brief co_get_workmode()
 *
 * @return 
 **/
co_workmode_t co_get_workmode(void);

/**
 * @brief co_msg_register()
 *
 * @param[in] dst  
 * @param[in] rx_cb  
 *
 * @return 
 **/
void co_msg_register(co_layer_id_t dst, co_msg_callback_t rx_cb);

/**
 * @brief co_msg_param2msg()
 *
 * @param[in] param  
 *
 * @return 
 **/
STATIC_INLINE co_msg_t *co_msg_param2msg(const void *param)
{
    return container_of((uint8_t *)(param)-*((uint8_t *)(param)-1), co_msg_t, header);
}

/**
 * @brief co_msg_msg2param()
 *
 * @param[in] msg  
 *
 * @return 
 **/
STATIC_INLINE void *co_msg_msg2param(const co_msg_t *msg)
{
    return msg->param;
}

/**
 * @brief co_msg_increase_param_offset()
 *
 * @param[in] msg  
 * @param[in] increase  
 *
 * @return 
 **/
STATIC_INLINE void co_msg_increase_param_offset(co_msg_t *msg, int increase)
{
    uint8_t save = *(msg->param - 1);

    msg->param += increase;

    co_assert(msg->param >= msg->header);
    co_assert(msg->param <= msg->buffer + msg->buffer_len);

    co_assert(msg->param_len > increase);

    msg->param_len -= increase;

    *(msg->param - 1) = save + increase;
}

/**
 * @brief co_msg_increase_param_offset_ex()
 *
 * @param[in] param  
 * @param[in] increase  
 *
 * @return 
 **/
STATIC_INLINE void co_msg_increase_param_offset_ex(void **param, int increase)
{
    co_msg_t *msg = co_msg_param2msg(*param);

    co_msg_increase_param_offset(msg, increase);

    *param = msg->param;
}

/**
 * @brief co_msg_get_param_offset()
 *
 * @param[in] msg  
 *
 * @return 
 **/
STATIC_INLINE unsigned co_msg_get_param_offset(co_msg_t *msg)
{
    return *(msg->param - 1);
}

/**
 * @brief co_msg_get_param_offset_ex()
 *
 * @param[in] param  
 *
 * @return 
 **/
STATIC_INLINE unsigned co_msg_get_param_offset_ex(void *param)
{
    return *((uint8_t *)param - 1);
}

/**
 * @brief co_msg_save_get_param_length()
 *
 * @param[in] save_q_item  
 *
 * @return 
 **/
STATIC_INLINE uint16_t co_msg_save_get_param_length(co_q_t *save_q_item)
{
    co_msg_t *msg = (co_msg_t *)save_q_item;

    co_assert(save_q_item != NULL);

    return msg->param_len;
}

/**
 * @brief co_msg_save_get_param()
 *
 * @param[in] save_q_item  
 *
 * @return 
 **/
STATIC_INLINE void *co_msg_save_get_param(co_q_t *save_q_item)
{
    co_msg_t *msg = (co_msg_t *)save_q_item;

    co_assert(save_q_item != NULL);

    return msg->param;
}

/**
 * @brief co_msg_save_get_id()
 *
 * @param[in] save_q_item  
 *
 * @return 
 **/
STATIC_INLINE co_msg_id_t co_msg_save_get_id(co_q_t *save_q_item)
{
    co_msg_t *msg = (co_msg_t *)save_q_item;

    co_assert(save_q_item != NULL);

    return msg->id;
}

#ifdef __cplusplus
}
#endif

#endif

