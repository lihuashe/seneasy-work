/**
 * @file co_q.h
 * @brief queue
 * @date Tue 09 Dec 2014 04:37:42 PM CST
 * @author XIHAO
 *
 * @defgroup CO_QUEUE Queue
 * @ingroup COROUTINE
 * @brief Queue Module
 * @details
 *
 * @{
 */

#ifndef __CO_Q_H__
#define __CO_Q_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "co_tools.h"
#include "co_list.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/// Queue structure
typedef co_list_t co_q_t;

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * INLINE FUNCTIONS
 */

/**
 * @brief queue initialize
 *
 * @param[in] head  queue head
 *
 * @return None
 **/
STATIC_INLINE void co_q_init(co_q_t *head)
{
    co_list_init((co_list_t *)head);
}

/**
 * @brief push a item to front
 *
 * @param[in] new  new item
 * @param[in] head  queue head
 *
 * @return 
 **/
STATIC_INLINE void co_q_push_front(co_q_t *new, co_q_t *head)
{
    co_list_add_front((co_list_t *)new, (co_list_t *)head);
}

/**
 * @brief push a item to behind
 *
 * @param[in] new  new item
 * @param[in] head  queue head
 *
 * @return None
 **/
STATIC_INLINE void co_q_push(co_q_t *new, co_q_t *head)
{
    co_list_add((co_list_t *)new, (co_list_t *)head);
}

/**
 * @brief wheth is empty
 *
 * @param[in] head  queue head
 *
 * @return empty?
 **/
STATIC_INLINE bool co_q_is_empty(const co_q_t *head)
{
    return co_list_is_empty((co_list_t *)head);
}

/**
 * @brief peek item from front
 *
 * @param[in] head  queue head
 *
 * @retval NULL no item
 * @retval NOT_NULL the first item
 **/
STATIC_INLINE co_q_t *co_q_peek(co_q_t *head)
{
    return (co_q_t *)co_list_first((co_list_t *)head);
}

/**
 * @brief peek item from behind
 *
 * @param[in] head  queue head
 *
 * @retval NULL no item
 * @retval NOT_NULL the last item
 **/
STATIC_INLINE co_q_t *co_q_peek_behind(co_q_t *head)
{
    return (co_q_t *)co_list_last((co_list_t *)head);
}

/**
 * @brief pop item
 *
 * @param[in] head  queue head
 *
 * @retval NULL no item
 * @retval NOT_NULL the item
 **/
STATIC_INLINE co_q_t *co_q_pop(co_q_t *head)
{
    co_q_t *node = co_q_peek(head);

    if (node)
        co_list_del((co_list_t *)node);

    return node;
}

/**
 * @brief pop item
 *
 * @param[in] head  queue head
 *
 * @retval NULL no item
 * @retval NOT_NULL the item
 **/
STATIC_INLINE co_q_t *co_q_pop_behind(co_q_t *head)
{
    co_q_t *node = co_q_peek_behind(head);

    if (node)
        co_list_del((co_list_t *)node);

    return node;
}

/**
 * @brief move item to another queue
 *
 * @param[in] item  moved item
 * @param[in] head  queue head
 *
 * @return None
 **/
STATIC_INLINE void co_q_move(co_q_t *item, co_q_t *head)
{
    co_list_move((co_list_t *)item, (co_list_t *)head);
}


/*********************************************************************
 * EXTERN FUNCTIONS
 */



#ifdef __cplusplus
}
#endif

#endif

/** @} */

