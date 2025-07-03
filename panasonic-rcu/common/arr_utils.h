
#ifndef APP_MAIN_APPLICATION_UTILS_ARR_UTILS_H_
#define APP_MAIN_APPLICATION_UTILS_ARR_UTILS_H_

#include "types.h"
#include "utility.h"

/**
 * @brief: 移除数组内第一个匹配key的值
 * @param key 要移除的值
 * @param T 数组类型
 * @return {none}
 */
#define ARRAR_REMOVE(arr, len, key, T)                 \
    do                                                 \
    {                                                  \
        foreach (__i, len)                             \
        {                                              \
            if (arr[__i] == key)                       \
            {                                          \
                len--;                                 \
                foreach_range(__i0, __i, len)          \
                {                                      \
                    SWAP(arr[__i0], arr[__i0 + 1], T); \
                }                                      \
                break;                                 \
            }                                          \
        }                                              \
                                                       \
    } while (0)

/**
 * @brief: 移除数组内第pos+1个的值
 * @param pos 要移除的位置
 * @param T 数组类型
 * @return {none}
 */
#define ARRAR_REMOVE_POS(arr, len, pos, T)   \
    do                                       \
    {                                        \
        len--;                               \
        foreach_range(__i, pos, len)         \
        {                                    \
            SWAP(arr[__i], arr[__i + 1], T); \
        }                                    \
    } while (0)

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif // APP_MAIN_APPLICATION_UTILS_ARR_UTILS_H_
