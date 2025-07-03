/**
 * @file sf_base_ex.h
 * @brief 
 * @date Mon 16 Nov 2020 04:23:47 PM CST
 * @author XIHAO
 *
 * @defgroup 
 * @ingroup 
 * @brief 
 * @details 
 *
 * @{
 */

#ifndef __SF_BASE_EX_H__
#define __SF_BASE_EX_H__

#ifdef __cplusplus
extern "C"
{ /*}*/
#endif

/*********************************************************************
 * INCLUDES
 */


/*********************************************************************
 * MACROS
 */
#define SFB_DONE                        0x01
#define SFB_CMD_READ                    1
#define SFB_CMD_WRITE                   2
#define SFB_NODMA_WRITE_DATA_LEN_MAX    8
#define SFB_NODMA_READ_DATA_LEN_MAX     4

#define DIV_ROUND_UP(n,d)     (((n) + (d) - 1) / (d))

/*********************************************************************
 * TYPEDEFS
 */
#pragma pack(1)
typedef struct __spi_cmd_t
{
    uint32_t cmd:2;
    uint32_t cs:1;
    uint32_t lcd2lane:1;
    uint32_t keepCs:1;
    uint32_t cmdBits:7;
    uint32_t dataBytes:20;
}spi_cmd_t;
#pragma pack()

typedef struct __sfb_env_t
{
    // critical object
    XH_SF_Type *critical_obj;

    // callback
    sfb_callback_t callback[SFB_MODULE_NUM];

    // Configuration save space
    struct {
        uint32_t ctrl;
        uint32_t cs;
    }configuration_save[SFB_MODULE_NUM][SFB_CS_NUM];

    // 2lane mode for LCD
    uint8_t lcd2lane[SFB_MODULE_NUM][SFB_CS_NUM];
}sfb_env_t;

/*********************************************************************
 * EXTERN VARIABLES
 */
extern sfb_env_t sfb_env;

/*********************************************************************
 * EXTERN FUNCTIONS
 */
/**
 * @brief  sfb irqn
 *
 * @param[in] sf  sf
 *
 * @return IRQn
 **/
__STATIC_INLINE IRQn_Type sfb_irqn(XH_SF_Type *sf)
{
    return sf==XH_SF ? SF_IRQn : SF1_IRQn;
}


#ifdef __cplusplus
/*{*/ }
#endif

#endif

/** @} */

