/**
 * @file timer.c
 * @brief 
 * @date Wed 31 May 2017 07:16:01 PM CST
 * @author XIHAO
 *
 * @addtogroup 
 * @ingroup 
 * @details 
 *
 * @{
 */

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"

/*********************************************************************
 * MACROS
 */

#define TIM_NUM                 3

#define TIM_ARR_MAX             0xFFFF
#define TIM_PSC_MAX             0xFFFF

#define TIM_STRUCT2INDEX(tim)   ((((uint32_t)tim) - XH_TIM0_BASE) / (XH_TIM1_BASE - XH_TIM0_BASE))

#define TIMER_CLK               32000000UL

/*********************************************************************
 * TYPEDEFS
 */


typedef struct
{
    struct
    {
        tim_mode_t mode;
        tim_callback_t callback;
    }tim[TIM_NUM];
}tim_env_t;

typedef struct
{
    tim_mode_t mode;
    tim_1_callback_t callback;
}tim_1_env_t;


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static tim_env_t tim_env;
static tim_1_env_t tim_1_env;


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief tim_enable_irq()
 *
 * @param[in] index  
 *
 * @return 
 **/
__STATIC_INLINE void tim_enable_irq(int index)
{
    IRQn_Type irqn = (IRQn_Type)((int)TIM0_IRQn + index);

    NVIC_SetPriority(irqn, IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(irqn);
    NVIC_EnableIRQ(irqn);
}

/**
 * @brief tim_timer_irq_handler()
 *
 * @param[in] tim  
 *
 * @return 
 **/
static void tim_timer_irq_handler(XH_TIM_Type *tim)
{
    int index = TIM_STRUCT2INDEX(tim);
    uint32_t status = tim->SR;

    // clear irq
    tim->SR = 0;

    if(tim_env.tim[index].callback)
        tim_env.tim[index].callback(tim);
}

/**
 * @brief tim_timer_irq_handler()
 *
 * @param[in] tim  
 *
 * @return 
 **/
static void tim_timer_1_irq_handler(XH_TIM_1_Type *tim)
{
    uint32_t status = tim->SR;

    // clear irq
    tim->SR = 0;

    if(tim_1_env.callback)
        tim_1_env.callback(tim);
}


/**
 * @brief tim_timer_calc_psc_arr()
 *
 * @param[in] tim  
 * @param[in] period_us  
 *
 * @return 
 **/
static bool tim_timer_calc_psc_arr(XH_TIM_Type *tim, uint32_t period_us)
{
    int index = TIM_STRUCT2INDEX(tim);

    uint32_t clock_mhz = cpm_get_clock((cpm_clk_t)((int)CPM_TIM0_CLK + index)) / 1000000;
    uint32_t psc, arr, factor;

    factor = period_us / (TIM_ARR_MAX+1);
    psc = clock_mhz * (factor + 1);
    arr = period_us / (factor + 1); // arr = (delay_us * clock_mhz) / (psc + 1) - 1

    if(psc>TIM_PSC_MAX || arr>TIM_ARR_MAX)
        return false;

    tim->PSC = psc-1;
    tim->ARR = arr-1;

    return true;
}

/**
 * @brief tim_timer_calc_psc_arr()
 *
 * @param[in] tim  
 * @param[in] period_us  
 *
 * @return 
 **/
static bool tim_1_timer_calc_psc_arr(XH_TIM_1_Type *tim, uint32_t period_us)
{
    int index = TIM_STRUCT2INDEX(tim);

    uint32_t clock_mhz = cpm_get_clock((cpm_clk_t)((int)CPM_TIM0_CLK + index)) / 1000000;
    uint32_t psc, arr, factor;

    factor = period_us / (TIM_ARR_MAX+1);
    psc = clock_mhz * (factor + 1);
    arr = period_us / (factor + 1); // arr = (delay_us * clock_mhz) / (psc + 1) - 1

    if(psc>TIM_PSC_MAX || arr>TIM_ARR_MAX)
        return false;

    tim->PSC = psc-1;
    tim->ARR = arr-1;

    return true;
}

/**
 * @brief tim_timer_config()
 *
 * @param[in] tim  
 * @param[in] config  
 *
 * @return 
 **/
static bool tim_timer_config(XH_TIM_Type *tim, const tim_timer_config_t *config)
{
    int index = TIM_STRUCT2INDEX(tim);
    bool psc_arr;

    psc_arr = tim_timer_calc_psc_arr(tim, config->period_us);
    if(!psc_arr)
        return false;

    tim->CR1 = TIM_CR1_ARPE;
    tim->DIER = 0;
    tim->RCR = 0;
    tim->CNT = 0;
    tim->SR = 0;

    tim_env.tim[index].callback = (tim_callback_t)config->callback;
    if(config->callback)
    {
        tim_enable_irq(index);
        tim->DIER = TIM_DIER_UIE;
    }

    return true;
}

/**
 * @brief tim_timer_config()
 *
 * @param[in] tim  
 * @param[in] config  
 *
 * @return 
 **/
static bool tim_1_timer_config(XH_TIM_1_Type *tim, const tim_1_timer_config_t *config)
{
    int index = TIM_STRUCT2INDEX(tim);
    bool psc_arr;

    psc_arr = tim_1_timer_calc_psc_arr(tim, config->period_us);
    if(!psc_arr)
        return false;

    tim->CR1 = TIM_CR1_ARPE;
    tim->DIER = 0;
    tim->RCR = 0;
    tim->CNT = 0;
    tim->SR = 0;

    tim_1_env.callback = (tim_1_callback_t)config->callback;
    if(config->callback)
    {
        tim_enable_irq(index);
        tim->DIER = TIM_DIER_UIE;
    }

    return true;
}

/**
 * @brief tim_pwm_config()
 *
 * @param[in] tim  
 * @param[in] config  
 *
 * @return 
 **/
static bool tim_pwm_config(XH_TIM_Type *tim, const tim_pwm_config_t *config)
{
    int index = TIM_STRUCT2INDEX(tim);
    uint32_t clock = cpm_get_clock((cpm_clk_t)((int)CPM_TIM0_CLK + index));
    uint32_t psc, i, ccer;
    uint32_t chNum = 1;

    /* Configures the peripheral.*/
    psc = (clock / config->count_freq) - 1;

    if(psc > TIM_PSC_MAX)
        return false;

    tim->DIER = 0;
    tim->CR1 = TIM_CR1_ARPE;

    /* All channels configured in PWM1 mode with preload enabled and will
       stay that way until the driver is stopped.*/
    tim->CCMR1 = _TIM_CCMR1_OC1M(6) | TIM_CCMR1_OC1PE | _TIM_CCMR1_OC2M(6) | TIM_CCMR1_OC2PE;
    if( (XH_TIM0 == tim) || (XH_TIM2 == tim) )
        tim->CCMR2 = _TIM_CCMR2_OC3M(6) | TIM_CCMR2_OC3PE | _TIM_CCMR2_OC4M(6) | TIM_CCMR2_OC4PE;

    tim->PSC = psc;
    tim->ARR = config->period_count-1;

    /* Output enables and polarities setup.*/
    ccer = 0;
    if( (XH_TIM0 == tim) || (XH_TIM2 == tim) )
    {
        chNum = TIM_PWM_CHANNEL_NUM;
    }
    else
    {
        chNum = 1;  /* timer1 support only 1 channel(ch0) pwm function */
    }
        
    for(i=0; i<chNum; ++i)
    {
        if(config->channel[i].enable)
        {
            ccer |= TIM_CCER_CCE(i);
            
            if(config->channel[i].config.complementary_output_enable)    
            {
                /* CCNE(Capture/Compare 1 complementary output enable) */
                ccer |= TIM_CCER_CCNE(i);
            }
            
            if(config->channel[i].config.pol)
            {
                ccer |= TIM_CCER_CCP(i);
                if(config->channel[i].config.complementary_output_enable)
                {
                    /* CCNP(Capture/Compare 1 complementary output polarity) */
                    ccer &= ~(TIM_CCER_CCNP(i));
                }
                                
            }          
            
            tim->CCR[i] = config->channel[i].config.pulse_count;
        }
    }
    tim->CCER = ccer;

    tim->CNT = 0;
    tim->CR2 = 0;
    tim->SR = 0;
      
    /* TIM_BDTR(Timer break and dead-time register):Main output enable 1: Enabled  */
    tim->BDTR = TIM_BDTR_MOE | config->dead_time;
    
    tim_env.tim[index].callback = (tim_callback_t)config->callback;
    if(config->callback)
    {
        tim_enable_irq(index);
        tim->DIER = TIM_DIER_UIE;
    }

    return true;
}

/**
 * @brief tim_pwm_config()
 *
 * @param[in] tim  
 * @param[in] config  
 *
 * @return 
 **/
static bool tim_1_pwm_config(XH_TIM_1_Type *tim, const tim_1_pwm_config_t *config)
{
    int index = TIM_STRUCT2INDEX(tim);
    uint32_t clock = cpm_get_clock((cpm_clk_t)((int)CPM_TIM0_CLK + index));
    uint32_t psc, i, ccer;
    uint32_t chNum = 1;

    /* Configures the peripheral.*/
    psc = (clock / config->count_freq) - 1;

    if(psc > TIM_PSC_MAX)
        return false;

    tim->DIER = 0;
    tim->CR1 = TIM_CR1_ARPE;

    /* All channels configured in PWM1 mode with preload enabled and will
       stay that way until the driver is stopped.*/
    tim->CCMR1 = _TIM_CCMR1_OC1M(6) | TIM_CCMR1_OC1PE | _TIM_CCMR1_OC2M(6) | TIM_CCMR1_OC2PE;

    tim->PSC = psc;
    tim->ARR = config->period_count-1;

    /* Output enables and polarities setup.*/
    ccer = 0;
    chNum = 1;  /* timer1 support only 1 channel(ch0) pwm function */
        
    for(i=0; i<chNum; ++i)
    {
        if(config->channel[i].enable)
        {
            ccer |= TIM_CCER_CCE(i);
            
            if(config->channel[i].config.complementary_output_enable)    
            {
                /* CCNE(Capture/Compare 1 complementary output enable) */
                ccer |= TIM_CCER_CCNE(i);
            }
            
            if(config->channel[i].config.pol)
            {
                ccer |= TIM_CCER_CCP(i);
                if(config->channel[i].config.complementary_output_enable)
                {
                    /* CCNP(Capture/Compare 1 complementary output polarity) */
                    ccer &= ~(TIM_CCER_CCNP(i));
                }
                                
            }          
            
            tim->CCR[i] = config->channel[i].config.pulse_count;
        }
    }
    tim->CCER = ccer;

    tim->CNT = 0;
    tim->CR2 = 0;
    tim->SR = 0;
      
    /* TIM_BDTR(Timer break and dead-time register):Main output enable 1: Enabled  */
    tim->BDTR = TIM_BDTR_MOE | config->dead_time;
    
    tim_1_env.callback = (tim_1_callback_t)config->callback;
    if(config->callback)
    {
        tim_enable_irq(index);
        tim->DIER = TIM_DIER_UIE;
    }

    return true;
}


XH_DMA_CH_Type* ir_tim_dma_config(XH_TIM_Type *tim, XH_DMA_CH_Type *dma, const ir_tim_dma_config_t *config)
{
    dma_id_t dma_id;
    if(dma == NULL)
        dma = dma_allocate();
    dma_id = tim==XH_TIM0 ? TIMER0_DMA_ID : TIMER2_DMA_ID;
    if(dma)
    {
        bool res;
        dma_config_t dconfig;

        dconfig.slave_id       = dma_id;
        dconfig.direction      = DMA_MEM_TO_DEV;
        dconfig.src_addr_width = DMA_SLAVE_BUSWIDTH_32BITS;
        dconfig.dst_addr_width = DMA_SLAVE_BUSWIDTH_32BITS;
        dconfig.src_burst      = DMA_BURST_LEN_1UNITS;
        dconfig.dst_burst      = DMA_BURST_LEN_1UNITS;
        dconfig.dev_flow_ctrl  = false;
        dconfig.priority       = 0;
        dconfig.callback       = config->callback;

        dconfig.lli.enable     = false;
        dconfig.lli.use_fifo   = config->use_fifo;
        dconfig.lli.src_addr   = (uint32_t)config->buffer;
        dconfig.lli.dst_addr   = (uint32_t)&tim->CCR[0];
        dconfig.lli.block_num  = config->block_num;
        dconfig.lli.block_len  = config->buffer_len/config->block_num;
        dconfig.lli.llip       = config->block_llip;

        res = dma_config(dma, &dconfig);
        if(!res)
        {
            dma_release(dma);
            return NULL;
        }
        tim->DCR = ((config->num-1)<<8) | ((uint32_t)config->tim_addr-(uint32_t)tim)/4;
        tim->DIER |= config->req;
    }
    return dma;
}

XH_DMA_CH_Type* ir_tim_1_dma_config(XH_TIM_1_Type *tim, XH_DMA_CH_Type *dma, const ir_tim_dma_config_t *config)
{
    dma_id_t dma_id = TIMER1_DMA_ID;
    if(dma == NULL)
        dma = dma_allocate();
    if(dma)
    {
        bool res;
        dma_config_t dconfig;

        dconfig.slave_id       = dma_id;
        dconfig.direction      = DMA_MEM_TO_DEV;
        dconfig.src_addr_width = DMA_SLAVE_BUSWIDTH_32BITS;
        dconfig.dst_addr_width = DMA_SLAVE_BUSWIDTH_32BITS;
        dconfig.src_burst      = DMA_BURST_LEN_1UNITS;
        dconfig.dst_burst      = DMA_BURST_LEN_1UNITS;
        dconfig.dev_flow_ctrl  = false;
        dconfig.priority       = 0;
        dconfig.callback       = config->callback;

        dconfig.lli.enable     = false;
        dconfig.lli.use_fifo   = config->use_fifo;
        dconfig.lli.src_addr   = (uint32_t)config->buffer;
        dconfig.lli.dst_addr   = (uint32_t)&tim->CCR[0];
        dconfig.lli.block_num  = config->block_num;
        dconfig.lli.block_len  = config->buffer_len/config->block_num;
        dconfig.lli.llip       = config->block_llip;

        res = dma_config(dma, &dconfig);
        if(!res)
        {
            dma_release(dma);
            return NULL;
        }
        tim->DCR = ((config->num-1)<<8) | ((uint32_t)config->tim_addr-(uint32_t)tim)/4;
        tim->DIER |= config->req;
    }
    return dma;
}

/**
 * @brief tim_set_prescale()
 *
 * @param[in] idx  
 * @param[in] ratio  
 *
 * @return ccmr1 or ccmr2
 **/
static uint16_t tim_set_prescale(uint8_t idx, cap_prescale_t ratio )
{
    uint16_t ccmr = 0;
    uint8_t i = idx;

    switch( ratio )
    {
        case CAP_PRESCALE_2:
            switch(i)
            {
                case 0:
                    ccmr |= TIM_CCMR1_IC1PSC_0;
                    break;
                case 1:
                    ccmr |= TIM_CCMR1_IC2PSC_0;
                    break;
                case 2:
                    ccmr |= TIM_CCMR2_IC3PSC_0;
                    break;
                case 3:
                    ccmr |= TIM_CCMR2_IC4PSC_0;
                    break;
            }
            break;
    
        case CAP_PRESCALE_4:
            switch(i)
            {
                case 0:
                    ccmr |= TIM_CCMR1_IC1PSC_1;
                    break;
                case 1:
                    ccmr |= TIM_CCMR1_IC2PSC_1;
                    break;
                case 2:
                    ccmr |= TIM_CCMR2_IC3PSC_1;
                    break;
                case 3:
                    ccmr |= TIM_CCMR2_IC4PSC_1;
                    break;
            }
            break;
    
        case CAP_PRESCALE_8:
            switch(i)
            {
                case 0:
                    ccmr |= TIM_CCMR1_IC1PSC_2;
                    break;
                case 1:
                    ccmr |= TIM_CCMR1_IC2PSC_2;
                    break;
                case 2:
                    ccmr |= TIM_CCMR2_IC3PSC_2;
                    break;
                case 3:
                    ccmr |= TIM_CCMR2_IC4PSC_2;
                    break;
            }
            break;

        default:
            break;
    }


    return ccmr;
}

/**
 * @brief tim_set_filter()
 *
 * @param[in] idx  
 * @param[in] filter  
 *
 * @return ccmr1 or ccmr2
 **/
static uint16_t tim_set_filter(uint8_t idx, cap_filter_t filter )
{
    uint16_t ccmr = 0;
    uint8_t i = idx;

    switch(i)
    {
        case 0:
            ccmr |= _TIM_CCMR1_IC1F(filter);
            break;
        case 1:
            ccmr |= _TIM_CCMR1_IC2F(filter);
            break;
        case 2:
            ccmr |= _TIM_CCMR2_IC3F(filter);
            break;
        case 3:
            ccmr |= _TIM_CCMR2_IC4F(filter);
            break;
        default:
            ccmr |= _TIM_CCMR1_IC1F(filter);
            break;
    }

    return ccmr;
}

/**
 * @brief tim_cap_config()
 *
 * @param[in] tim  
 * @param[in] config  
 *
 * @return 
 **/
static bool tim_cap_config(XH_TIM_Type *tim, const tim_cap_config_t *config)
{
    int index = TIM_STRUCT2INDEX(tim);
    uint16_t ccmr1 = 0;
    uint16_t ccmr2 = 0;
    uint16_t ccer = 0;
    uint16_t dier = 0;
    uint16_t ccie, ccp, ccnp;
    uint8_t i = 0;

    /* only timer0 support capture function */
    if( XH_TIM0 != tim )
        return false;

    cpm_set_clock((cpm_clk_t)((int)CPM_TIM0_CLK + index), TIMER_CLK);
    tim_enable_irq(index);

    tim->DIER = 0;
    tim->CR1 = TIM_CR1_ARPE;

    tim->PSC = config->psc;
    tim->ARR = config->arr;

    tim->CR1 &= ~TIM_CR1_DIR_MASK;
    if(DOWN_COUNT == config->cnt_mode)
    {
        tim->CR1 |= TIM_CR1_DIR_MASK;
    }

    tim->CR1 &= ~TIM_CR1_CKD;
    switch(config->clk_div)
    {
        case TIM_CLK_DIV_1:
            tim->CR1 |= TIM_CR1_CKD_0;
            break;
        case TIM_CLK_DIV_2:
            tim->CR1 |= TIM_CR1_CKD_1;
            break;
        default:
            break;
    }

    tim->CR1 &= ~TIM_CR1_CMS;/* edge-aligned mode*/
    tim->CR1 &= ~TIM_CR1_OPM;/* counter is not stopped at update event*/
    tim->CR1 &= ~(TIM_CR1_URS|TIM_CR1_UDIS);

    tim->SR = 0;

    ccmr1 = 0;
    ccmr2 = 0;
    ccer  = 0;
    dier  = 0;
    tim->CCER = 0;

    for(i=0; i<TIM_PWM_CHANNEL_NUM; i++)
    {
        if(CAP_MODE_NONE != config->channels[i].cap_mode)
        {
            if( NULL != config->channels[i].callback )
            {
                switch(i)
                {
                    case 0:
                        dier |= TIM_DIER_CC1IE;
                        break;
                    case 1:
                        dier |= TIM_DIER_CC2IE;
                        break;
                    case 2:
                        dier |= TIM_DIER_CC3IE;
                        break;
                    case 3:
                        dier |= TIM_DIER_CC4IE;
                        break;
                }
                tim_env.tim[index].callback = (tim_callback_t)config->channels[i].callback;
            }

            switch(i)
            {
                case 0:
                    ccer |= TIM_CCER_CC1E; /* capture enable*/
                    ccp = TIM_CCER_CC1P;
                    ccnp = TIM_CCER_CC1NP;
                    break;
                case 1:
                    ccer |= TIM_CCER_CC2E; /* capture enable*/
                    ccp = TIM_CCER_CC2P;
                    ccnp = TIM_CCER_CC2NP;
                    break;
                case 2:
                    ccer |= TIM_CCER_CC3E; /* capture enable*/
                    ccp = TIM_CCER_CC3P;
                    ccnp = TIM_CCER_CC3NP;
                    break;
                case 3:
                    ccer |= TIM_CCER_CC4E; /* capture enable*/
                    ccp = TIM_CCER_CC4P;
                    ccnp = TIM_CCER_CC4NP;
                    break;
            }

            /* capture property config*/
            switch(config->channels[i].cap_mode)
            {
                case CAP_MODE_RISING:
                    ccer &= ~ccp;       /*CCxNP,CCxP = 2'b00*/
		            ccer &= ~ccnp;
					break;
                case CAP_MODE_FALLING:
                    ccer |= ccp;        /*CCxNP,CCxP = 2'b01*/
                    break;
                case CAP_MODE_BOTH:
                    ccer |= ccnp|ccp;   /*CCxNP,CCxP = 2'b11*/
                    break;
                default:
                    return false;
            }

            /* Capture selection: 
            * 01: CC2 channel is configured as input, IC2 is mapped on TI1
            * 10: CC2 channel is configured as input, IC2 is mapped on TI2
            */
            switch( config->channels[i].cap_map )
            {
                case CAP_MAP_TI1:
                    if( 0 == i )
                        ccmr1 |= _TIM_CCMR1_CC1S(1);
                    else if ( 1 == i )
                        ccmr1 |= _TIM_CCMR1_CC1S(2);
                    else
                    {
                        return false;
                    }
                    break;
                    
                case CAP_MAP_TI2:
                    if( 0 == i )
                        ccmr1 |= _TIM_CCMR1_CC2S(2);
                    else if ( 1 == i )
                        ccmr1 |= _TIM_CCMR1_CC2S(1);
                    else
                    {
                        return false;
                    }
                    break;

                case CAP_MAP_TI3:
                    if( 2 == i )
                        ccmr2 |= _TIM_CCMR2_CC3S(1);
                    else if ( 3 == i )
                        ccmr2 |= _TIM_CCMR2_CC3S(2);
                    else
                    {
                        return false;
                    }
                    break;
                    
                case CAP_MAP_TI4:
                    if( 2 == i )
                        ccmr2 |= _TIM_CCMR2_CC4S(2);
                    else if ( 3 == i )
                        ccmr2 |= _TIM_CCMR2_CC4S(1);
                    else
                    {
                        return false;
                    }
                    break;
            }

            /* Set input capture prescaler : defines the ratio of the prescaler acting on CC input (IC).*/
            if( (0 == i) || (1 == i) )
            {
                ccmr1 |= tim_set_prescale(i,config->channels[i].prescale);
                ccmr1 |= tim_set_filter(i,config->channels[i].filter);
            }
            else
            {
                ccmr2 |= tim_set_prescale(i,config->channels[i].prescale);
                ccmr2 |= tim_set_filter(i,config->channels[i].filter);
            }
        }
    }

    tim->CCMR1 = ccmr1;
    tim->CCMR2 = ccmr2;
    tim->CCER  = ccer;
    tim->DIER  = dier | config->dier;

    return true;
}


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief tim initialize
 *
 * @return None
 **/
void tim_init(void)
{
}

/**
 * @brief tim configure
 *
 * @param[in] tim  tim object
 * @param[in] config  configuration
 *
 * @return None
 **/
void tim_config(XH_TIM_Type *tim, const tim_config_t *config)
{
    uint32_t index = TIM_STRUCT2INDEX(tim);

    // Reset and Open clock
    register_set1(&XH_PSO->TIM_CFG[index], CPM_TIMER_SOFT_RESET_MASK);
    register_set0(&XH_PSO->TIM_CFG[index], CPM_TIMER_GATE_EN_MASK);
    XH_PSO_UPD_RDY();
	
    switch(config->mode)
    {
        case TIM_TIMER_MODE:
            tim_timer_config(tim, &config->config.timer);
            break;

        case TIM_PWM_MODE:
            tim_pwm_config(tim, &config->config.pwm);
            break;

        case TIM_CAP_MODE:
            tim_cap_config(tim, &config->config.cap);
            break;
    }
}

/**
 * @brief tim configure
 *
 * @param[in] tim  tim object
 * @param[in] config  configuration
 *
 * @return None
 **/
void tim_1_config(XH_TIM_1_Type *tim, const tim_1_config_t *config)
{
    uint32_t index = TIM_STRUCT2INDEX(tim);

    // Reset and Open clock
    register_set1(&XH_PSO->TIM_CFG[index], CPM_TIMER_SOFT_RESET_MASK);
    register_set0(&XH_PSO->TIM_CFG[index], CPM_TIMER_GATE_EN_MASK);
    XH_PSO_UPD_RDY();
	
    switch(config->mode)
    {
        case TIM_TIMER_MODE:
            tim_1_timer_config(tim, &config->config.timer);
            break;

        case TIM_PWM_MODE:
            tim_1_pwm_config(tim, &config->config.pwm);
            break;

        default:
            break;
    }
}



/**
 * @brief Change PWM period count
 *
 * @param[in] tim  PWM object
 * @param[in] period_count  new period count, MAX:65535
 *
 * @return None
 **/
void tim_pwm_change_period_count(XH_TIM_Type *tim, uint16_t period_count)
{
    tim->ARR = period_count;
}

/**
 * @brief Change PWM period count
 *
 * @param[in] tim  PWM object
 * @param[in] period_count  new period count, MAX:65535
 *
 * @return None
 **/
void tim_1_pwm_change_period_count(XH_TIM_1_Type *tim, uint16_t period_count)
{
    tim->ARR = period_count;
}


/**
 * @brief Change PWM channel pulse count
 *
 * @param[in] tim  PWM object
 * @param[in] channel  PWM channel index
 * @param[in] pulse_count  new pulse count, MAX 65535, should be less than period_count
 *
 * @return None
 **/
void tim_pwm_channel_change_pulse_count(XH_TIM_Type *tim, tim_pwm_channel_t channel, uint16_t pulse_count)
{
    tim->CCR[channel] = pulse_count;
}

/**
 * @brief Change PWM channel pulse count
 *
 * @param[in] tim  PWM object
 * @param[in] channel  PWM channel index
 * @param[in] pulse_count  new pulse count, MAX 65535, should be less than period_count
 *
 * @return None
 **/
void tim_1_pwm_channel_change_pulse_count(XH_TIM_1_Type *tim, tim_pwm_channel_t channel, uint16_t pulse_count)
{
    tim->CCR[channel] = pulse_count;
}

/**
 * @brief Force PWM channel output
 *
 * @param[in] tim  PMW object
 * @param[in] channel  PWM channel index
 * @param[in] level  force output level
 *
 * @return None
 **/
void tim_pwm_channel_force_output(XH_TIM_Type *tim, tim_pwm_channel_t channel, tim_pwm_force_level_t level)
{
    uint32_t ccxm = (uint32_t)level;
    uint32_t pol = tim->CCER & TIM_CCER_CCP(channel);

    if(level!=TIM_PWM_FORCE_DISABLE && pol)
        ccxm ^= 1u;

    switch(channel)
    {
        case TIM_PWM_CHANNEL_0:
            tim->CCMR1 = (tim->CCMR1 & ~TIM_CCMR1_OC1M) | _TIM_CCMR1_OC1M(ccxm);
            break;
        case TIM_PWM_CHANNEL_1:
            tim->CCMR1 = (tim->CCMR1 & ~TIM_CCMR1_OC2M) | _TIM_CCMR1_OC2M(ccxm);
            break;
        case TIM_PWM_CHANNEL_2:
            tim->CCMR2 = (tim->CCMR2 & ~TIM_CCMR2_OC3M) | _TIM_CCMR2_OC3M(ccxm);
            break;
        case TIM_PWM_CHANNEL_3:
            tim->CCMR2 = (tim->CCMR2 & ~TIM_CCMR2_OC4M) | _TIM_CCMR2_OC4M(ccxm);
            break;
        default:
            break;
    }
}

/**
 * @brief Force PWM channel output
 *
 * @param[in] tim  PMW object
 * @param[in] channel  PWM channel index
 * @param[in] level  force output level
 *
 * @return None
 **/
void tim_1_pwm_channel_force_output(XH_TIM_1_Type *tim, tim_pwm_channel_t channel, tim_pwm_force_level_t level)
{
    uint32_t ccxm = (uint32_t)level;
    uint32_t pol = tim->CCER & TIM_CCER_CCP(channel);

    if(level!=TIM_PWM_FORCE_DISABLE && pol)
        ccxm ^= 1u;

    switch(channel)
    {
        case TIM_PWM_CHANNEL_0:
            tim->CCMR1 = (tim->CCMR1 & ~TIM_CCMR1_OC1M) | _TIM_CCMR1_OC1M(ccxm);
            break;
        default:
            break;
    }
}


/**
 * @brief tim start
 *
 * After config tim (timer, PWM), call this function to start
 *
 * @param[in] tim  tim object
 *
 * @return None
 **/
void tim_start(XH_TIM_Type *tim)
{
    int index = TIM_STRUCT2INDEX(tim);

    // start
    tim->CR1 |= TIM_CR1_CEN;/*timer starts running*/

    // Prevent sleep
    pmu_lowpower_prevent(PMU_LP_TIM0<<index);
}

/**
 * @brief tim start
 *
 * After config tim (timer, PWM), call this function to start
 *
 * @param[in] tim  tim object
 *
 * @return None
 **/
void tim_1_start(XH_TIM_1_Type *tim)
{
    int index = TIM_STRUCT2INDEX(tim);

    // start
    tim->CR1 |= TIM_CR1_CEN;/*timer starts running*/

    // Prevent sleep
    pmu_lowpower_prevent(PMU_LP_TIM0<<index);
}

/**
 * @brief tim stop
 *
 * @param[in] tim  tim object
 *
 * @return None
 **/
void tim_stop(XH_TIM_Type *tim)
{
    int index = TIM_STRUCT2INDEX(tim);

    // start
    tim->CR1 &= ~TIM_CR1_CEN;/*timer starts running*/

    // Alow sleep
    pmu_lowpower_allow(PMU_LP_TIM0<<index);
}

/**
 * @brief tim stop
 *
 * @param[in] tim  tim object
 *
 * @return None
 **/
void tim_1_stop(XH_TIM_1_Type *tim)
{
    int index = TIM_STRUCT2INDEX(tim);

    // start
    tim->CR1 &= ~TIM_CR1_CEN;/*timer starts running*/

    // Alow sleep
    pmu_lowpower_allow(PMU_LP_TIM0<<index);
}

void TIM0_IRQHandler(void)
{
    tim_timer_irq_handler(XH_TIM0);
}

void TIM1_IRQHandler(void)
{
    tim_timer_1_irq_handler(XH_TIM1);
}

void TIM2_IRQHandler(void)
{
    tim_timer_irq_handler(XH_TIM2);
}

void tim_dma_config(XH_TIM_Type *tim, const tim_dma_config_t *config)
{
    tim->DCR = ((config->len_in_bytes/4 - 1)<<8) | ((uint32_t)config->tim_addr-(uint32_t)tim)/4;
    tim->DIER = config->req;

    dma_dev_t dma_tim = {
        .id         = (dma_id_t)(TIMER0_DMA_ID + TIM_STRUCT2INDEX(tim)),
        .addr       = (void *)&tim->DMAR,
        .addr_ctrl  = DMA_ADDR_CTRL_FIX,
        .bus_width  = DMA_SLAVE_BUSWIDTH_32BITS,
        .burst_size = DMA_BURST_LEN_1UNITS,
    };
    
    dma_dev_t dma_mem = {
        .id         = MEM_DMA_ID,
        .addr       = config->mem_addr,
        .addr_ctrl  = DMA_ADDR_CTRL_INC,
        .bus_width  = DMA_SLAVE_BUSWIDTH_32BITS,
        .burst_size = DMA_BURST_LEN_1UNITS,
    };
    
    dma_block_config_t dmabc = {
        .src                 = config->mem_to_tim ? &dma_mem : &dma_tim,
        .dst                 = config->mem_to_tim ? &dma_tim : &dma_mem,
        .block_size_in_bytes = config->len_in_bytes,
        .priority            = 0,
        .flow_controller     = DMA_FLOW_CONTROLLER_USE_NONE,
        .intr_en             = false,
    };
        
    dma_build_block(config->block, &dmabc);
}


void tim_1_dma_config(XH_TIM_1_Type *tim, const tim_dma_config_t *config)
{
    tim->DCR = ((config->len_in_bytes/4 - 1)<<8) | ((uint32_t)config->tim_addr-(uint32_t)tim)/4;
    tim->DIER = config->req;

    dma_dev_t dma_tim = {
        .id         = (dma_id_t)(TIMER0_DMA_ID + TIM_STRUCT2INDEX(tim)),
        .addr       = (void *)&tim->DMAR,
        .addr_ctrl  = DMA_ADDR_CTRL_FIX,
        .bus_width  = DMA_SLAVE_BUSWIDTH_32BITS,
        .burst_size = DMA_BURST_LEN_1UNITS,
    };
    
    dma_dev_t dma_mem = {
        .id         = MEM_DMA_ID,
        .addr       = config->mem_addr,
        .addr_ctrl  = DMA_ADDR_CTRL_INC,
        .bus_width  = DMA_SLAVE_BUSWIDTH_32BITS,
        .burst_size = DMA_BURST_LEN_1UNITS,
    };
    
    dma_block_config_t dmabc = {
        .src                 = config->mem_to_tim ? &dma_mem : &dma_tim,
        .dst                 = config->mem_to_tim ? &dma_tim : &dma_mem,
        .block_size_in_bytes = config->len_in_bytes,
        .priority            = 0,
        .flow_controller     = DMA_FLOW_CONTROLLER_USE_NONE,
        .intr_en             = false,
    };
        
    dma_build_block(config->block, &dmabc);
}


/** @} */


