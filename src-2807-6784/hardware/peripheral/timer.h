/**
 * @file timer.h
 * @brief timer driver
 * @date Wed 31 May 2017 07:16:05 PM CST
 * @author XIHAO
 *
 * @defgroup Timer Timer
 * @ingroup PERIPHERAL
 * @brief timer/PWM driver
 * @details Timer driver
 *
 * The Timer includes three identical 32-bit Timer Counter channels.
 * Each channel can be independently programmed to perform a wide
 * range of functions including frequency measurement, event counting,
 * interval measurement, pulse generation, delay timing and pulse
 * width modulation. Each channel drives an internal interrupt signal
 * which can be programmed to generate processor interrupts.
 *
 * @{
 *
 * @example example_timer.c
 * This is an example of how to use the timer
 *
 * @example example_pwm.c
 * This is an example of how to use the pwm
 *
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/// timer event callback
typedef void (*tim_callback_t)(XH_TIM_Type * tim);

typedef void (*tim_1_callback_t)(XH_TIM_1_Type * tim);


/// TIM DMA
typedef enum
{
    TIM_DMA_BURST_LEN_1UNIT = 0,
    TIM_DMA_BURST_LEN_2UNIT,
    TIM_DMA_BURST_LEN_3UNIT,
    TIM_DMA_BURST_LEN_4UNIT,
    TIM_DMA_BURST_LEN_5UNIT,
    TIM_DMA_BURST_LEN_6UNIT,
    TIM_DMA_BURST_LEN_7UNIT,
    TIM_DMA_BURST_LEN_8UNIT,
    TIM_DMA_BURST_LEN_9UNIT,
    TIM_DMA_BURST_LEN_10UNIT,
    TIM_DMA_BURST_LEN_11UNIT,
    TIM_DMA_BURST_LEN_12UNIT,
    TIM_DMA_BURST_LEN_13UNIT,
    TIM_DMA_BURST_LEN_14UNIT,
    TIM_DMA_BURST_LEN_15UNIT,
    TIM_DMA_BURST_LEN_16UNIT,
    TIM_DMA_BURST_LEN_17UNIT,
    TIM_DMA_BURST_LEN_18UNIT,
    
    TIM_DMA_BURST_LEN_RESERVED,
    
} tim_dma_burstlen_t;
    
/// TIM mode
typedef enum
{
    /// Work as timer mode
    TIM_TIMER_MODE,
    /// Work as PWM mode
    TIM_PWM_MODE,
    /// Work as CAP mode
    TIM_CAP_MODE,
}tim_mode_t;

/// PWM polarity
typedef enum
{
    TIM_PWM_POL_HIGH2LOW,
    TIM_PWM_POL_LOW2HIGH,
}tim_pwm_pol_t;

/// PWM force output level
typedef enum
{
    TIM_PWM_FORCE_LOW     = 4,
    TIM_PWM_FORCE_HIGH    = 5,
    TIM_PWM_FORCE_DISABLE = 6,
}tim_pwm_force_level_t;

/// PWM channels
typedef enum
{
    TIM_PWM_CHANNEL_0,
    TIM_PWM_CHANNEL_1,
    TIM_PWM_CHANNEL_2,
    TIM_PWM_CHANNEL_3,
    TIM_PWM_CHANNEL_NUM,
}tim_pwm_channel_t;

typedef enum
{
    UP_COUNT   = 0,
    DOWN_COUNT = 1,
} tim_cnt_mode_t;

typedef enum
{
    TIM_CLK_DIV_0        = 0,
    TIM_CLK_DIV_1        = 1,
    TIM_CLK_DIV_2        = 2,
    TIM_CLK_DIV_RESERVED = 3,
} tim_clock_div_t;

typedef enum
{
    EDGE_ALIGNED_MODE     = 0,
    CENTER_ALIGNED_MODE_1 = 1,
    CENTER_ALIGNED_MODE_2 = 2,
    CENTER_ALIGNED_MODE_3 = 3,
} tim_align_mode_t;


/// timer configuration
typedef struct
{
    /// delay or period, unit is us
    uint32_t period_us;
    /// event callback
    tim_callback_t callback;
}tim_timer_config_t;

/// timer configuration
typedef struct
{
    /// delay or period, unit is us
    uint32_t period_us;
    /// event callback
    tim_1_callback_t callback;
}tim_1_timer_config_t;


/// PWM channel configuration
typedef struct
{
    /// PWM polarity
    tim_pwm_pol_t pol;
    /// pulse counter value
    uint16_t pulse_count;
    //complementary_output_enable
    bool complementary_output_enable;
    
}tim_pwm_channel_config_t;

/// PWM configuration
typedef struct
{
    /// Frequency for every count
    uint32_t count_freq;
    /// Period counter
    uint16_t period_count;
  
    // dead_time generate step
    uint16_t dead_time;
    
    /// Channel configuration
    struct
    {
        /// channel enable
        bool enable;
        /// channel configuration
        tim_pwm_channel_config_t config;
    }channel[TIM_PWM_CHANNEL_NUM];
    /// Event callback
    tim_callback_t callback;
}tim_pwm_config_t;

/// PWM configuration
typedef struct
{
    /// Frequency for every count
    uint32_t count_freq;
    /// Period counter
    uint16_t period_count;
  
    // dead_time generate step
    uint16_t dead_time;
    
    /// Channel configuration
    struct
    {
        /// channel enable
        bool enable;
        /// channel configuration
        tim_pwm_channel_config_t config;
    }channel[TIM_PWM_CHANNEL_NUM];
    /// Event callback
    tim_1_callback_t callback;
}tim_1_pwm_config_t;

typedef struct
{
    bool           use_fifo;    /**< fifo buffer */
    uint32_t      *buffer;      /**< DMA fifo buffer */
    uint32_t       buffer_len;  /**< DMA buffer len */
    dma_llip_t    *block_llip;  /**< block llip */
    uint32_t       block_num;   /**< block num */
    __IO uint32_t *tim_addr;    /**< Absolute address of the TIM register to access */
    uint32_t       num;         /**< number of registers under updating //DMA burst length */
    uint32_t       req;         /**< TIM DMA request source: TIM_DIER_UDE, TIM_DIER_CC1DE, ..., TIM_DIER_TDE */
    dma_callback_t callback;    /**<  DMA event callback */
} ir_tim_dma_config_t;

typedef enum
{
    CAP_MODE_NONE    = 0,
    CAP_MODE_FALLING = 1,
    CAP_MODE_RISING  = 2,
    CAP_MODE_BOTH    = 3,
}cap_mode_t;

typedef enum
{
    CAP_PRESCALE_NONE = 0,
    CAP_PRESCALE_2    = 1,
    CAP_PRESCALE_4    = 2,
    CAP_PRESCALE_8    = 3,
} cap_prescale_t;

typedef enum
{
    CAP_FILTER_NONE = 0,
    CAP_FILTER_1    = 1,
    CAP_FILTER_2    = 2,
    CAP_FILTER_3    = 3,
    CAP_FILTER_4    = 4,
    CAP_FILTER_5    = 5,
    CAP_FILTER_6    = 6,
    CAP_FILTER_7    = 7,
    CAP_FILTER_8    = 8,
    CAP_FILTER_9    = 9,
    CAP_FILTER_10   = 10,
    CAP_FILTER_11   = 11,
    CAP_FILTER_12   = 12,
    CAP_FILTER_13   = 13,
    CAP_FILTER_14   = 14,
    CAP_FILTER_15   = 15,
} cap_filter_t;

typedef enum
{
    CAP_MAP_TI1 = 0,
    CAP_MAP_TI2 = 1,
    CAP_MAP_TI3 = 2,
    CAP_MAP_TI4 = 3,
} cap_map_t;

typedef struct
{
    cap_mode_t	   cap_mode;
    cap_map_t 	   cap_map;
    cap_prescale_t   prescale;
    cap_filter_t	   filter;
    tim_callback_t    callback;
}CAPChannelConfig;

/// timer configuration
typedef struct
{
    uint16_t		psc;
    uint16_t		arr;
    tim_cnt_mode_t	cnt_mode;
    tim_clock_div_t clk_div;
    uint8_t 		repeat_cnt;
    tim_callback_t	callback;

    CAPChannelConfig channels[4];

    uint16_t		 dier;			/*CCxOF enable or not*/
}tim_cap_config_t;

/// TIM configuration
typedef struct
{
    /// TIM mode
    tim_mode_t mode;
    /// TIM config
    union
    {
        /// Use as timer
        tim_timer_config_t timer;
        /// Use as PWM
        tim_pwm_config_t pwm;
        /// Use as CAP
        tim_cap_config_t cap;
    }config;
}tim_config_t;


/// TIM configuration
typedef struct
{
    /// TIM mode
    tim_mode_t mode;
    /// TIM config
    union
    {
        /// Use as timer
        tim_1_timer_config_t timer;
        /// Use as PWM
        tim_1_pwm_config_t pwm;
        /// Use as CAP
        tim_cap_config_t cap;
    }config;
}tim_1_config_t;


/// TIM DMA config
typedef struct
{
    /// dma direction: true for mem to tim, false for tim to mem.
    bool mem_to_tim;
    /// Absolute address of the TIM register to access
    __IO uint32_t *tim_addr;
    /// address of the memory buffer
    uint32_t *mem_addr;
    /// length of the DMA transfer in bytes
    uint32_t len_in_bytes;
    /// TIM DMA request source: TIM_DIER_UDE, TIM_DIER_CC1DE, ..., TIM_DIER_TDE
    uint32_t req;
    /// DMA block transfer to build.
    dma_block_t *block;
} tim_dma_config_t;
/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief tim initialize
 *
 * @return None
 **/
void tim_init(void);

/**
 * @brief tim configure
 *
 * @param[in] tim  tim object
 * @param[in] config  configuration
 *
 * @return None
 **/
void tim_config(XH_TIM_Type *tim, const tim_config_t *config);

void tim_1_config(XH_TIM_1_Type *tim, const tim_1_config_t *config);

/**
 * @brief Change PWM period count
 *
 * @param[in] tim  PWM object
 * @param[in] period_count  new period count, MAX:65535
 *
 * @return None
 **/
void tim_pwm_change_period_count(XH_TIM_Type *tim, uint16_t period_count);

/**
 * @brief Change PWM channel pulse count
 *
 * @param[in] tim  PWM object
 * @param[in] channel  PWM channel index
 * @param[in] pulse_count  new pulse count, MAX 65535, should be less than period_count
 *
 * @return None
 **/
void tim_pwm_channel_change_pulse_count(XH_TIM_Type *tim, tim_pwm_channel_t channel, uint16_t pulse_count);

/**
 * @brief Force PWM channel output
 *
 * @param[in] tim  PMW object
 * @param[in] channel  PWM channel index
 * @param[in] level  force output level
 *
 * @return None
 **/
void tim_pwm_channel_force_output(XH_TIM_Type *tim, tim_pwm_channel_t channel, tim_pwm_force_level_t level);

/**
 * @brief tim start
 *
 * After config tim (timer, PWM), call this function to start
 *
 * @param[in] tim  tim object
 *
 * @return None
 **/
void tim_start(XH_TIM_Type *tim);

void tim_1_start(XH_TIM_1_Type *tim);

/**
 * @brief tim stop
 *
 * @param[in] tim  tim object
 *
 * @return None
 **/
void tim_stop(XH_TIM_Type *tim);

void tim_1_stop(XH_TIM_1_Type *tim);

/**
 * @brief tim dma configure and build dma block transfer.
 *
 * @param[in] tim  tim object
 * @param[in] config  tim dma configuration
 *
 * @return None
 **/
void tim_dma_config(XH_TIM_Type *tim, const tim_dma_config_t *config);

void tim_1_dma_config(XH_TIM_1_Type *tim, const tim_dma_config_t *config);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

