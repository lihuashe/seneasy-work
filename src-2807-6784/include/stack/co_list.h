/**
 * @file co_list.h
 * @brief list
 * @date Tue 09 Dec 2014 04:37:42 PM CST
 * @author XIHAO
 *
 * @defgroup CO_LIST List
 * @ingroup COROUTINE
 * @brief List Module
 * @details
 *
 * @{
 */

#ifndef __CO_LIST_H__
#define __CO_LIST_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "co_tools.h"

/*********************************************************************
 * MACROS
 */
/// @cond
#define CO_LIST_POISON1    (co_list_t *)0xFF123456
#define CO_LIST_POISON2    (co_list_t *)0xFF234567
/// @endcond

/**
 * @brief Traverse all list item
 *
 * @param[in] pos  list position
 * @param[in] head  list header
 **/
#define co_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * @brief Traverse all list item
 *
 * @param[in] pos  list position
 * @param[in] head  list header
 * @param[in] type  list item type struct
 **/
#define co_list_for_each_ex(pos, head, type) \
    for (pos = (type *)((co_list_t *)(head))->next; pos != (type *)(head); pos = (type *)((co_list_t *)pos)->next)

/**
 * @brief Traverse all list item, can delete item in loop
 *
 * @param[in] pos  list position
 * @param[in] n  list next position (tmp value)
 * @param[in] head  list header
 **/
#define co_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

/**
 * @brief Traverse all list item, can delete item in loop
 *
 * @param[in] pos  list position
 * @param[in] n  list next position (tmp value)
 * @param[in] head  list header
 * @param[in] type  list item type struct
 **/
#define co_list_for_each_safe_ex(pos, n, head, type) \
    for (pos = (type *)((co_list_t *)(head))->next, n = (type *)((co_list_t *)pos)->next; pos != (type *)(head); pos = n, n = (type *)((co_list_t *)pos)->next)


/*********************************************************************
 * TYPEDEFS
 */

/// list structure
typedef struct __co_list
{
    /// The next pointer
    struct __co_list * volatile next;
    /// The prev pointer
    struct __co_list * volatile prev;
}co_list_t;

/**
 * @brief list insert compare callback
 *
 * @param[in] me  new insert node
 * @param[in] qnode  current node
 * @param[in] param  param
 *
 * @return whether insert
 **/
typedef bool (*cmp_insert_callback_t)(const co_list_t *me, const co_list_t *qnode, void *param);

/**
 * @brief list extract compare callback
 *
 * @param[in] qnode  current node
 * @param[in] param  param
 *
 * @return whether extract
 **/
typedef bool (*cmp_extract_callback_t)(const co_list_t *qnode, void *param);

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * INLINE FUNCTIONS
 */

/**
 * @brief list initialize
 *
 * @param[in] head  list head
 *
 * @return None
 **/
STATIC_INLINE void co_list_init(co_list_t *head)
{
    head->next = head;
    head->prev = head;
}

/// @cond
/**
 * @brief __co_list_add()
 *
 * @param[in] new  
 * @param[in] prev  
 * @param[in] next  
 *
 * @return 
 **/
STATIC_INLINE void __co_list_add(co_list_t *new, co_list_t *prev, co_list_t *next)
{
    co_assert(next->prev == prev);
    co_assert(prev->next == next);

    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}
/// @endcond

/**
 * @brief add a item to front
 *
 * @param[in] new  new item
 * @param[in] head  list head
 *
 * @return None
 **/
STATIC_INLINE void co_list_add_front(co_list_t *new, co_list_t *head)
{
    __co_list_add(new, head, head->next);
}

/**
 * @brief add a item to tail
 *
 * @param[in] new  new item
 * @param[in] head  list head
 *
 * @return None
 **/
STATIC_INLINE void co_list_add(co_list_t *new, co_list_t *head)
{
    __co_list_add(new, head->prev, head);
}

/**
 * @brief insert a item to a specified place
 *
 * if cmp_cb() return true, insert the front of the cmp-node.
 * if no true return, insert the tail
 *
 * @param[in] new  new item
 * @param[in] head  list head
 * @param[in] cmp_cb  compare callback
 * @param[in] param  param
 *
 * @return None
 **/
STATIC_INLINE void co_list_insert(co_list_t *new, co_list_t *head, cmp_insert_callback_t cmp_cb, void *param)
{
    co_list_t *pos;

    co_list_for_each(pos, head)
    {
        if(cmp_cb(new, pos, param))
        {
            __co_list_add(new, pos->prev, pos);
            return;
        }
    }

    __co_list_add(new, pos->prev, pos);
}

/**
 * @brief delete a item
 *
 * @param[in] entry  
 *
 * @return 
 **/
STATIC_INLINE void co_list_del(co_list_t *entry)
{
    co_assert(entry->prev->next == entry);
    co_assert(entry->next->prev == entry);

    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    entry->next = CO_LIST_POISON1;
    entry->prev = CO_LIST_POISON2;
}

/**
 * @brief find a item
 *
 * @param[in] head  list head
 * @param[in] item  item
 *
 * @return if find, return it, otherwise return NULL
 **/
STATIC_INLINE co_list_t *co_list_find(co_list_t *head, void *item)
{
    co_list_t *pos;

    co_list_for_each(pos, head)
    {
        if(pos == item)
            return pos;
    }

    return NULL;
}

/**
 * @brief extract a item
 *
 * @param[in] head  list head
 * @param[in] item  item
 *
 * @return if find, extract it, otherwise extract NULL
 **/
STATIC_INLINE co_list_t *co_list_extract(co_list_t *head, void *item)
{
    co_list_t *pos;

    pos = co_list_find(head, item);

    if(pos)
        co_list_del(pos);

    return pos;
}

/**
 * @brief find a item
 *
 * if cmp_cb() return true, extract it and delete it from q.
 * if no true return, extact NULL.
 *
 * @param[in] head  list head
 * @param[in] cmp_cb  compare callback
 * @param[in] param  param
 *
 * @return if find, return it, otherwise return NULL
 **/
STATIC_INLINE co_list_t *co_list_find_ex(co_list_t *head, cmp_extract_callback_t cmp_cb, void *param)
{
    co_list_t *pos;

    co_list_for_each(pos, head)
    {
        if(cmp_cb(pos, param))
            return pos;
    }

    return NULL;
}

/**
 * @brief extract a item
 *
 * if cmp_cb() return true, extract it and delete it from q.
 * if no true return, extact NULL.
 *
 * @param[in] head  list head
 * @param[in] cmp_cb  compare callback
 * @param[in] param  param
 *
 * @return if find, extract it, otherwise extract NULL
 **/
STATIC_INLINE co_list_t *co_list_extract_ex(co_list_t *head, cmp_extract_callback_t cmp_cb, void *param)
{
    co_list_t *pos;

    pos = co_list_find_ex(head, cmp_cb, param);

    if(pos)
        co_list_del(pos);

    return pos;
}

/**
 * @brief whether is empty
 *
 * @param[in] head  list head
 *
 * @return empty?
 **/
STATIC_INLINE bool co_list_is_empty(const co_list_t *head)
{
    return head->next == head;
}

/**
 * @brief whether is first
 *
 * @param[in] node  check item
 * @param[in] head  list head
 *
 * @return first?
 **/
STATIC_INLINE bool co_list_is_first(const co_list_t *node, const co_list_t *head)
{
    return head->next == node;
}

/**
 * @brief whether is invalid
 *
 * @param[in] entry  check item
 *
 * @return deleted
 **/
STATIC_INLINE bool co_list_is_invalid(const co_list_t *entry)
{
    return (entry==CO_LIST_POISON1 ||
            entry==CO_LIST_POISON2 ||
            entry->next==CO_LIST_POISON1 ||
            entry->prev==CO_LIST_POISON2) ? true : false;
}

/**
 * @brief number of list
 *
 * @param[in] head  list head
 *
 * @return number
 **/
STATIC_INLINE unsigned co_list_num(const co_list_t *head)
{
    co_list_t *pos;
    unsigned num = 0;

    co_list_for_each(pos, head)
    {
        ++num;
    }

    return num;
}

/**
 * @brief get first item from list
 *
 * @param[in] head  list head
 *
 * @retval NULL no item
 * @retval NOT_NULL first item
 **/
STATIC_INLINE co_list_t *co_list_first(co_list_t *head)
{
    return co_list_is_empty(head) ? NULL : head->next;
}

/**
 * @brief get last item from list
 *
 * @param[in] head  list head
 *
 * @retval NULL no item
 * @retval NOT_NULL last item
 **/
STATIC_INLINE co_list_t *co_list_last(co_list_t *head)
{
    return co_list_is_empty(head) ? NULL : head->prev;
}

/**
 * @brief move one item to another list
 *
 * @param[in] item  moved item
 * @param[in] head  list head
 *
 * @return None
 **/
STATIC_INLINE void co_list_move(co_list_t *item, co_list_t *head)
{
    co_list_del(item);
    co_list_add(item, head);
}

/**
 * @brief  co list push
 *
 * @param[in] new  new
 * @param[in] head  head
 **/
STATIC_INLINE void co_list_push(co_list_t *new, co_list_t *head)
{
    co_list_add(new, head);
}

/**
 * @brief  co list push front
 *
 * @param[in] new  new
 * @param[in] head  head
 **/
STATIC_INLINE void co_list_push_front(co_list_t *new, co_list_t *head)
{
    co_list_add_front(new, head);
}

/**
 * @brief  co list pop
 *
 * @param[in] head  head
 **/
STATIC_INLINE co_list_t *co_list_pop(co_list_t *head)
{
    co_list_t *node = co_list_first(head);

    if (node)
        co_list_del(node);

    return node;
}

/**
 * @brief  co list pop behind
 *
 * @param[in] head  head
 **/
STATIC_INLINE co_list_t *co_list_pop_behind(co_list_t *head)
{
    co_list_t *node = co_list_last(head);

    if (node)
        co_list_del(node);

    return node;
}

/*********************************************************************
 * EXTERN FUNCTIONS
 */



#ifdef __cplusplus
}
#endif

#endif

/** @} */

