#ifndef __COMMON_MACROS_H__
#define __COMMON_MACROS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

typedef void (*u8_Func)(uint8_t);
typedef void (*u16_Func)(uint16_t);
typedef void (*u32_Func)(uint32_t);
typedef void (*void_Func)(void);


// 调试宏
#ifdef DEBUG
    #define LOG(fmt, ...) printf("DEBUG: " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG(fmt, ...)
#endif


// 判断宏
#define IF_DO(condition, ctrl)  do { \
    if((condition)) ctrl; \
} while (0)


// 断言宏
#define ASSERT(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Assertion failed: (%s), function %s, file %s, line %d.\n", \
                #condition, __FUNCTION__, __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

// 跟踪宏
#define TRACE(fmt, ...) printf("TRACE: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// 最大值和最小值宏
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// 数组长度宏
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

// 交换两个变量的宏
#define SWAP(x, y) do { \
    typeof(x) temp = (x); \
    (x) = (y); \
    (y) = temp; \
} while (0)

// 字符串连接宏
#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)

// 动态内存分配宏
#define MALLOC(size)        malloc(size)
#define CALLOC(count, size) calloc(count, size)
#define REALLOC(ptr, size)  realloc(ptr, size)
#define FREE(ptr)           do { free(ptr); ptr = NULL; } while (0)

// 错误处理宏
#define CHECK_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Error: NULL pointer detected at %s:%d\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

// 循环计数宏
#define FOR_EACH(item, array, length) \
    for (size_t i = 0; i < (length); i++) { \
        item = (array)[i];

// 结束循环计数宏
#define END_FOR_EACH() }

// 时间测量宏
#define START_TIMER() clock_t start = clock()
#define END_TIMER() do { \
    clock_t end = clock(); \
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC; \
    printf("Time spent: %f seconds\n", time_spent); \
} while (0)

// 字符串比较宏
#define STR_EQ(a, b) (strcmp((a), (b)) == 0)

// 取整宏
#define ROUND_UP(x, multiple) (((x) + (multiple) - 1) / (multiple) * (multiple))
#define ROUND_DOWN(x, multiple) ((x) / (multiple) * (multiple))

// 位操作宏
#define BIT_SET(var, bit) ((var) |= (1U << (bit)))
#define BIT_CLEAR(var, bit) ((var) &= ~(1U << (bit)))
#define BIT_TOGGLE(var, bit) ((var) ^= (1U << (bit)))
#define BIT_CHECK(var, bit) ((var) & (1U << (bit)))
#define BIT_LEFT_SHIFT(var, n) ((var) << (n))
#define BIT_RIGHT_SHIFT(var, n) ((var) >> (n))

// 限制值的范围宏
#define CLAMP(value, min, max) (MAX(min, MIN(value, max)))

// 计算平方和立方宏
#define SQUARE(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))

// 计算绝对值宏
#define ABS(x) ((x) < 0 ? -(x) : (x))

// 生成随机数宏
#define RANDOM_INT(min, max) (rand() % ((max) - (min) + 1) + (min))

// 结构体初始化宏
#define INIT_STRUCT(s) memset(&(s), 0, sizeof(s))

// // 线程安全的打印宏
// #include <pthread.h>
// #define THREAD_SAFE_PRINT(fmt, ...) do { \
//     static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER; \
//     pthread_mutex_lock(&print_mutex); \
//     printf(fmt, ##__VA_ARGS__); \
//     pthread_mutex_unlock(&print_mutex); \
// } while (0)

// 计算平均值宏
#define AVERAGE(a, b) (((a) + (b)) / 2.0)

// 计算斜边长度（勾股定理）宏
#define HYPOT(x, y) (sqrt(SQUARE(x) + SQUARE(y)))

// 计算最小公倍数和最大公约数宏
#define GCD(a, b) ({ \
    int _a = (a), _b = (b); \
    while (_b != 0) { \
        int temp = _b; \
        _b = _a % _b; \
        _a = temp; \
    } \
    _a; \
})
#define LCM(a, b) ((a) / GCD(a, b) * (b))

// 计算幂的宏
#define POWER(base, exp) ({ \
    double result = 1.0; \
    for (int _i = 0; _i < (exp); _i++) { \
        result *= (base); \
    } \
    result; \
})

// 计算数组的总和宏
#define ARRAY_SUM(arr, length) ({ \
    typeof(*(arr)) sum = 0; \
    for (size_t _i = 0; _i < (length); _i++) { \
        sum += (arr)[_i]; \
    } \
    sum; \
})

// 计算数组的平均值宏
#define ARRAY_AVERAGE(arr, length) (ARRAY_SUM(arr, length) / (length))

#endif // __COMMON_MACROS_H__
