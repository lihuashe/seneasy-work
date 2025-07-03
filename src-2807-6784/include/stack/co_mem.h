/*********************************************************************
 * @file co_mem.h
 * @brief 
 * @version 1.0
 * @date 14/11/4 13:50:42
 * @author XIHAO
 *
 * @note 
 */

#ifndef __CO_MEM_H__
#define __CO_MEM_H__

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

#ifdef CONFIG_ALLOC_DEBUG
#define MEM_DEBUG
#endif

/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
    NO_CRIMINAL          = 0,
    WHOLE_POOL_CRIMINAL  = 1,
    SOME_BLOCK_CRIMINAL  = 2,
    HEAD_STRUCT_CRIMINAL = 3,
    TAIL_STRUCT_CRIMINAL = 4,
    UNKNOWN_CRIMINAL     = 5,
}mem_criminal_type_t;

typedef struct
{
    int cur_used_size;
    int cur_idle_size;
    int extr_struct_size;

    void *first_available_addr;
    int mem_pool_size;

    int max_mem_pool_usage_size;
}mem_info_t;

typedef int (*mem_malloc_fail_try_to_do_callback_t)(void);


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief mem_init()
 *
 * @param[in] firstAvailablyAddr  
 * @param[in] size  
 *
 * @return 
 **/
void mem_init(void *firstAvailablyAddr, int size);

/**
 * @brief mem_detect_leaks()
 *
 * @return 
 **/
int mem_detect_leaks(void);

/**
 * @brief mem_inquire_info()
 *
 * @param[in] pmemInfo  
 *
 * @return 
 **/
void mem_inquire_info(mem_info_t *pmemInfo);

/**
 * @brief mem_idle_size()
 *
 * @return 
 **/
int mem_idle_size(void);

/**
 * @brief mem_is_ok()
 *
 * @return 
 **/
bool mem_is_ok(void);

/**
 * @brief mem_detect_overflow_criminal()
 *
 * @param[in] pcriminal  
 * @param[in] plen  
 *
 * @return 
 **/
mem_criminal_type_t mem_detect_overflow_criminal(void **pcriminal, int *plen);

/**
 * @brief  mem used block num
 *
 * @param[in] max_num  max num
 *
 * @return number
 **/
int mem_used_block_num(int max_num);

/**
 * @brief mem_dump()
 *
 * @param[in] printf_dump_func  
 *
 * @return 
 **/
void mem_dump(void *printf_dump_func, bool print_each_used_block);


#ifdef MEM_DEBUG
    extern void *mem_malloc(int size, const char *pfile, const char *pfunction, int line);
    extern void *mem_calloc(int size, const char *pfile, const char *pfunction, int line);
    extern void *mem_realloc(void *pfree, int size, const char *pfile, const char *pfunction, int line);
    extern void  mem_free(void *pfree, const char *pfile, const char *pfunction, int line);

    #define MEM_MALLOC(s)     mem_malloc (s,    __FILE__, __FUNCTION__, __LINE__)
    #define MEM_CALLOC(s)     mem_calloc (s,    __FILE__, __FUNCTION__, __LINE__)
    #define MEM_REALLOC(p, s) mem_realloc(p, s, __FILE__, __FUNCTION__, __LINE__)
    #define MEM_FREE(p)       mem_free   (p,    __FILE__, __FUNCTION__, __LINE__)
#else
    extern void *mem_malloc(int size);
    extern void *mem_calloc(int size);
    extern void *mem_realloc(void *pfree, int size);
    extern void  mem_free(void *pfree);

    #define MEM_MALLOC(s)     mem_malloc (s)
    #define MEM_CALLOC(s)     mem_calloc (s)
    #define MEM_REALLOC(p, s) mem_realloc(p, s)
    #define MEM_FREE(p)       mem_free(p)
#endif

/*********************************************************************
 * EXTERN FUNCTIONS MACRO
 */

//#define co_malloc                                      MEM_MALLOC
//#define co_calloc                                      MEM_CALLOC
//#define co_realloc                                     MEM_REALLOC
//#define co_free                                        MEM_FREE
//
//#define co_mem_init                                    mem_init
//#define co_mem_malloc_fail_try_to_do_callback_register mem_malloc_fail_try_to_do_callback_register
//#define co_mem_detect_leaks                            mem_detect_leaks
//#define co_mem_inquire_info                            mem_inquire_info
//#define co_mem_get_version_info                        mem_get_version_info
//#define co_mem_detect_overflow_criminal                mem_detect_overflow_criminal


#ifdef __cplusplus
}
#endif

#endif

