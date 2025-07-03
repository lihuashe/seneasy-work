/**
 * @file dma.c
 * @brief 
 * @date Tue 16 May 2017 02:52:40 PM CST
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
#define DMA_CH_NUM          4
#define DMA_CH_MASK         ((1u<<DMA_CH_NUM) - 1)

#define DMA_CH2INDEX(ch)    ((((uint32_t)ch) - XH_DMA_CH0_BASE) / (XH_DMA_CH1_BASE - XH_DMA_CH0_BASE))
#define DMA_INDEX2CH(i)     ((XH_DMA_CH_Type *)(XH_DMA_CH0_BASE + i * (XH_DMA_CH1_BASE - XH_DMA_CH0_BASE)))

#define DMA_CLEAR_BITS(reg, mask)   ((reg) = ((mask << 8) | 0))
#define DMA_SET_BITS(reg, mask)     ((reg) = ((mask << 8) | (mask)))

#define DMA_MAX_TRANS_COUNT  ((1u<<22) - 1)

#define pmu_lp_dma_ch(ch_index)    ((PMU_LP_DMA_CH0<<ch_index))

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
    uint32_t        allocated_mask;
    dma_callback_t  callback[DMA_CH_NUM];

    /**
     * Extended for XH98xx.dma.
     */
    /// Since XH98xx.dma has no means indicating the transfered block info to callback function,
    dma_block_t     *block[DMA_CH_NUM];
    /// Select between SPI1 and I2C2, they use the same req/ack pairs. True for I2C2.
    bool req_ack_pair_sel_i2c2_or_spi1;
} dma_env_t;

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static dma_env_t dma_env = {0};

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void sw_set(uint32_t *sw, uint32_t mask, uint32_t value)
{
    uint32_t sw_prev;

    sw_prev = *sw;
    sw_prev &= ~mask;
    sw_prev |= mask & value;
    *sw = sw_prev;
}

/**
 * @brief Setup channel control register.
 * @note following 13 arguments normally are the same for blocks in a list. 
 *
 * @param[in] src_req_sel   Select the request/ack handshake pair that the source
 *                          device is connected to: 0 ~ 15.
 * @param[in] dst_req_sel   Select the request/ack handshake pair that the destination
 *                          device is connected to: 0 ~ 15.
 */
static uint32_t dma_setup_ch_ctrl(uint32_t priority,
                                                 dma_burstlen_t  src_burst_size,
                                                 dma_buswidth_t  src_bus_width,
                                                 dma_buswidth_t  dst_bus_width,
                                                 dma_addr_ctrl_t src_addr_ctrl,
                                                 dma_addr_ctrl_t dst_addr_ctrl,
                                                 dma_id_t        src_req_sel,
                                                 dma_id_t        dst_req_sel,
                                                 uint32_t        int_abt_mask,
                                                 uint32_t        int_err_mask,
                                                 uint32_t        int_tc_mask)
{
    bool src_handshake = true;
    bool dst_handshake = true;
    uint32_t ctrl = {0};

    sw_set(&ctrl,MASK_7REG(DMA_ENABLE,         0, 
                            DMA_PRIORITY,       priority, 
                            DMA_SRCBURSTSIZE,   src_burst_size,
                            DMA_SRCWIDTH,       src_bus_width,
                            DMA_DSTWIDTH,       dst_bus_width,
                            DMA_DSTADDRCTRL,    dst_addr_ctrl,
                            DMA_SRCADDRCTRL,    src_addr_ctrl));

    if (src_req_sel == MEM_DMA_ID)
        src_handshake = false;
    if (dst_req_sel == MEM_DMA_ID)
        dst_handshake = false;

    // Select between SPI1 and I2C2, they use the same req/ack pairs.
    bool spi1 = false;
    bool i2c2 = false;

    if (src_handshake)
    {
        sw_set(&ctrl,MASK_2REG(DMA_SRCMODE, DMA_HANDSHAKE_MODE_HANDSHAKE, DMA_SRCREQSEL, src_req_sel));
    }
    else
        sw_set(&ctrl,MASK_1REG(DMA_SRCMODE, DMA_HANDSHAKE_MODE_NORMAL));

    if (dst_handshake)
    {
        sw_set(&ctrl,MASK_2REG(DMA_DSTMODE,DMA_HANDSHAKE_MODE_HANDSHAKE, DMA_DSTREQSEL,dst_req_sel));
    }
    else
        sw_set(&ctrl,MASK_1REG(DMA_DSTMODE, DMA_HANDSHAKE_MODE_NORMAL));

    co_assert((spi1 && i2c2) == false);
    if (spi1 || i2c2)
        dma_env.req_ack_pair_sel_i2c2_or_spi1 = i2c2;

    sw_set(&ctrl,MASK_3REG(DMA_INTABTMASK,int_abt_mask, DMA_INTERRMASK,int_err_mask, DMA_INTTCMASK,int_tc_mask));

    return ctrl;
}

static uint32_t dma_setup_ch_ctrl_ex(dma_block_config_t *config)
{
    bool src_handshake = true;
    bool dst_handshake = true;
    uint32_t ctrl = {0};  

    sw_set(&ctrl,MASK_7REG( DMA_ENABLE,         0, 
                             DMA_PRIORITY,       config->priority, 
                             DMA_SRCBURSTSIZE,   config->src->burst_size,
                             DMA_SRCWIDTH,       config->src->bus_width,
                             DMA_DSTWIDTH,       config->dst->bus_width,
                             DMA_DSTADDRCTRL,    config->dst->addr_ctrl,
                             DMA_SRCADDRCTRL,    config->src->addr_ctrl)); 
    
    if (config->src->id == MEM_DMA_ID)
        src_handshake = false;
    if (config->dst->id == MEM_DMA_ID)
        dst_handshake = false;

    // Select between SPI1 and I2C2, they use the same req/ack pairs.
    bool spi1 = false;
    bool i2c2 = false;

    if (src_handshake)
    {
        sw_set(&ctrl,MASK_2REG(DMA_SRCMODE,DMA_HANDSHAKE_MODE_HANDSHAKE, DMA_SRCREQSEL,config->src->id));
    }
    else
        sw_set(&ctrl,MASK_1REG(DMA_SRCMODE, DMA_HANDSHAKE_MODE_NORMAL));

    if (dst_handshake)
    {
        sw_set(&ctrl,MASK_2REG(DMA_DSTMODE,DMA_HANDSHAKE_MODE_HANDSHAKE, DMA_DSTREQSEL,config->dst->id));
    }
    else
        sw_set(&ctrl,MASK_1REG(DMA_DSTMODE, DMA_HANDSHAKE_MODE_NORMAL));

    co_assert((spi1 && i2c2) == false);
    if (spi1 || i2c2)
        dma_env.req_ack_pair_sel_i2c2_or_spi1 = i2c2;

    if (!config->intr_en)
    {
        sw_set(&ctrl,MASK_3REG(DMA_INTABTMASK,1, DMA_INTERRMASK,1, DMA_INTTCMASK,1));
    }
   
    return ctrl;
}

////////////////////////////////////////////////////////////////////////////
/**
 * @brief DMA_SETUP_LLIP()
 *
 */
#define DMA_SETUP_LLIP(llip, src_addr, dst_addr, tran_size, ctrl) \
    (llip)->SrcAddr   = src_addr;                                 \
    (llip)->DstAddr   = dst_addr;                                 \
    (llip)->TranSize  = tran_size;                                \
    (llip)->Ctrl      = ctrl;                                     \
    (llip)->LLPointer = NULL;


/**
 * @brief _dma_setup_lli()
 *
 * @param[in] lli  
 * @param[in] ctrllo  
 *
 * @return 
 **/
static bool dma_setup_lli(dma_config_t *config, uint32_t ctrl)
{
    dma_lli_t *lli = &config->lli;
    dma_llip_t *llip = config->lli.llip;
    uint32_t i;

    for(i = 0; i < lli->block_num; i++)
    {
        switch(config->direction)
        {
            case DMA_MEM_TO_MEM:
                DMA_SETUP_LLIP(llip + i,
                               lli->src_addr + i * lli->block_len,
                               lli->dst_addr + i * lli->block_len,
                               lli->block_len / (1<<config->dst_addr_width),
                               ctrl);
                break;

            case DMA_MEM_TO_DEV:
                DMA_SETUP_LLIP(llip + i,
                               lli->src_addr + i * lli->block_len,
                               lli->dst_addr,
                               lli->block_len / (1<<config->dst_addr_width),
                               ctrl);
                break;

            case DMA_DEV_TO_MEM:
                DMA_SETUP_LLIP(llip + i,
                               lli->src_addr,
                               lli->dst_addr + i * lli->block_len,
                               lli->block_len / (1<<config->src_addr_width),
                               ctrl);
                break;

            default:
                llip[i].LLPointer = NULL;
                return false;
        }

        if(llip[i].TranSize > DMA_MAX_TRANS_COUNT)
            return false;

        llip[i].LLPointer = &llip[i+1];
    }

    if(lli->use_fifo)
    {
        llip[lli->block_num-1].LLPointer = &llip[0]; // ? loop back? what it does with fifo?
    }
    else
    {
        llip[lli->block_num-1].LLPointer = NULL;
    }

    return true;
}


/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void DMA_IRQHandler(void)
{
    uint32_t i, status_abort, status_err, status_tc;

    status_tc = REGR(&XH_DMAC->IntStatus.status, MASK_POS(DMAC_TC));
    status_abort = REGR(&XH_DMAC->IntStatus.status, MASK_POS(DMAC_ABORT));
    status_err = REGR(&XH_DMAC->IntStatus.status, MASK_POS(DMAC_ERROR));
    XH_DMAC->IntStatus.all = 0x00FFFFFF;

    for (i=0; i<DMA_CH_NUM; i++)
    {
        uint32_t mask = 1<<i;
        XH_DMA_CH_Type *ch = DMA_INDEX2CH(i);
        dma_status_t status = DMA_STATUS_UNKNOWN;
        uint32_t src_addr, dst_addr, size;

        if (status_tc & mask)
        {
            status = DMA_STATUS_BLOCK_OK;
            if (dma_env.block[i])
            {
                src_addr = dma_env.block[i]->SrcAddr;
                dst_addr = dma_env.block[i]->DstAddr;
                size     = dma_env.block[i]->TranSize;
                
                if (dma_env.block[i]->LLPointer)
                    dma_env.block[i] = (dma_block_t *)dma_env.block[i]->LLPointer;
                else
                    dma_env.block[i] = NULL;
                
                /* 加入这个后软件控制红外发射才能使用*/
                if (dma_env.callback[i]) {
                    dma_env.callback[i](status, src_addr, dst_addr, size);
                }

            }
            if (ch->TranSize != 0)
               REGW1(&ch->Ctrl, DMA_ENABLE_MASK);
            else
                pmu_lowpower_allow(pmu_lp_dma_ch(i));
        }
        else if(status_abort & mask)
        {
            status = DMA_STATUS_ABORT;
            if (dma_env.callback[i])
                dma_env.callback[i](status, ch->SrcAddr, ch->DstAddr, ch->TranSize & 0x003FFFFF);
            pmu_lowpower_allow(pmu_lp_dma_ch(i));
        }
        else if (status_err & mask)
        {
            status = DMA_STATUS_ERROR;
            if (dma_env.callback[i])
                dma_env.callback[i](status, ch->SrcAddr, ch->DstAddr, ch->TranSize & 0x003FFFFF);
            pmu_lowpower_allow(pmu_lp_dma_ch(i));
        }

    }
}

/**
 * @brief DMA initialize
 *
 * @return None
 **/
void dma_init(void)
{
    memset(&dma_env, 0, sizeof(dma_env_t));

    // Reset and Open clock
    register_set0(&XH_PSO->DMA_CFG, CPM_DMA_GATE_EN_MASK);
    XH_PSO_UPD_RDY();

    /* Reset dma core, disable all channels */
    REGW1(&XH_DMAC->DMACtrl, DMAC_RESET_MASK);

    /* Clear all interrupts on all channels. */
    XH_DMAC->IntStatus.all = 0x00FFFFFF;

    // enable interrupt
    NVIC_SetPriority(DMA_IRQn, IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(DMA_IRQn);
    NVIC_EnableIRQ(DMA_IRQn);
}

/**
 * @brief DMA allocate a new channel object
 *
 * @retval NOTNULL new DMA object
 * @retval NULL No DMA to allocate
 **/
XH_DMA_CH_Type *dma_allocate(void)
{
    int i;

    for(i=0; i<DMA_CH_NUM; ++i)
    {
        if((dma_env.allocated_mask & (1u<<i)) == 0)
        {
            dma_env.allocated_mask |= 1u<<i;
            return DMA_INDEX2CH(i);
        }
    }

    return NULL;
}

/**
 * @brief DMA release a channel object
 *
 * @param[in] ch  DMA channel object (return by dma_allocate)
 *
 * @return None
 **/
void dma_release(XH_DMA_CH_Type *ch)
{
    dma_env.allocated_mask &= ~(1u<<DMA_CH2INDEX(ch));
}

/**
 * @brief DMA config
 *
 * @param[in] ch  DMA channel object
 * @param[in] config  dma configurtaion
 *
 * @retval true success
 * @retval false fail
 **/
bool dma_config(XH_DMA_CH_Type *ch, dma_config_t *config)
{
    uint32_t ctrl = {0};
    uint32_t ch_index = DMA_CH2INDEX(ch);

    bool int_mask = config->callback ? false : true;

    switch (config->direction)
    {
        case DMA_MEM_TO_MEM:
            ctrl = dma_setup_ch_ctrl(config->priority, config->src_burst,
                                     config->src_addr_width, config->dst_addr_width,
                                     DMA_ADDR_CTRL_INC, DMA_ADDR_CTRL_INC,
                                     MEM_DMA_ID, MEM_DMA_ID,
                                     int_mask, int_mask, int_mask);
            break;

        case DMA_MEM_TO_DEV:
            ctrl = dma_setup_ch_ctrl(config->priority, config->src_burst,
                                     config->src_addr_width, config->dst_addr_width,
                                     DMA_ADDR_CTRL_FIX, DMA_ADDR_CTRL_FIX,
                                     MEM_DMA_ID, config->slave_id,
                                     int_mask, int_mask, int_mask);
            break;
/* //原先的 trans_count大于1时 dma_start中src_addr是累加的 
        case DMA_MEM_TO_DEV:
            ctrl = dma_setup_ch_ctrl(config->priority, config->src_burst,
                                     config->src_addr_width, config->dst_addr_width,
                                     DMA_ADDR_CTRL_INC, DMA_ADDR_CTRL_FIX,
                                     MEM_DMA_ID, config->slave_id,
                                     int_mask, int_mask, int_mask);
            break;
*/
        case DMA_DEV_TO_MEM:
            ctrl = dma_setup_ch_ctrl(config->priority, config->src_burst,
                                     config->src_addr_width, config->dst_addr_width,
                                     DMA_ADDR_CTRL_FIX, DMA_ADDR_CTRL_INC,
                                     config->slave_id, MEM_DMA_ID,
                                     int_mask, int_mask, int_mask);
            break;

        case DMA_DEV_TO_DEV:
            // currently not supported, due to dma_config_t now has only one slave_id.
        case DMA_DIR_MAX:
        default:
            return false;
            //break;
    }

    // Select between SPI1 and I2C2, they use the same req/ack pairs.
    if (dma_env.req_ack_pair_sel_i2c2_or_spi1)
        XH_SYS->MON |= (1u<<16);
    else
        XH_SYS->MON &= ~(1u<<16);

    if(config->lli.enable)
    {
        bool res = dma_setup_lli(config, ctrl);
        if(!res)
            return false;
        // first block of linked list
        ch->SrcAddr = config->lli.llip->SrcAddr;
        ch->DstAddr = config->lli.llip->DstAddr;
        ch->TranSize = config->lli.llip->TranSize;
        ch->LLPointer = config->lli.llip->LLPointer;
        dma_env.block[ch_index] = ch;
    }
    else
    {
        // single block settings will be setup independently in dma_start_single_block().
        ch->LLPointer = NULL;
    }

    ch->Ctrl = ctrl;
    dma_env.callback[ch_index] = config->callback;

    if(config->callback)
    {
        // enable interrupt
        NVIC_SetPriority(DMA_IRQn, IRQ_PRIORITY_HIGH);
        NVIC_ClearPendingIRQ(DMA_IRQn);
        NVIC_EnableIRQ(DMA_IRQn);

        // some clock auto gate when WFI (DMA should open)
        if(XH_PMU->SW_STATUS & PMU_SW_STATUS_AGGRESSIVE_CPUGATE_MASK)
            REGW0(XH_SYS->PINMUX, SYS_PINMUX_SYSPLL_GT_CPUCLK_HW_CTRL_MASK);
    }

    return true;
}

/**
 * @brief DMA start a single block transfer.
 *
 * @param[in] ch  DMA channel object
 * @param[in] src_addr  source address
 * @param[in] dst_addr  dest address
 * @param[in] trans_count  source bus transfer count
 *
 * @retval true success
 * @retval false fail
 **/
bool dma_start(XH_DMA_CH_Type *ch, uint32_t src_addr, uint32_t dst_addr, uint32_t trans_count)
{
    if(trans_count > DMA_MAX_TRANS_COUNT)
        return false;

#if 1
    static dma_block_t block;
    block.Ctrl = ch->Ctrl;
    block.SrcAddr = src_addr;
    block.DstAddr = dst_addr;
    block.TranSize = trans_count;
    block.LLPointer = NULL;
    dma_env.block[DMA_CH2INDEX(ch)] = &block;
    *ch = block;
#else    
    ch->SrcAddr    = src_addr;
    ch->DstAddr    = dst_addr;
    ch->TranSize   = trans_count;
    ch->LLPointer   = NULL;
#endif
    
    REGW1(&ch->Ctrl, DMA_ENABLE_MASK);

    pmu_lowpower_prevent(pmu_lp_dma_ch(DMA_CH2INDEX(ch)));

    return true;
}

/**
 * @brief DMA start with LINK LIST
 *
 * @param[in] ch  DMA channel object
 *
 * @return None
 **/
void dma_start_with_lli(XH_DMA_CH_Type *ch)
{
    uint32_t ch_index = DMA_CH2INDEX(ch);

    REGW1(&ch->Ctrl, DMA_ENABLE_MASK);

    pmu_lowpower_prevent(pmu_lp_dma_ch(ch_index));
}

/**
 * @brief DMA build block transfer
 *
 * @note  Source/Destination address must be aligned to source/destination bus width.
 *        Block size must be aligned to source/destination (bus_width * burst_size).
 *
 * @param[out] block            Pointer to DMA block transfer object
 * @param[in]  config           dma block config
 *
 * @return Pointer to the block built or NULL when fail.
 **/
dma_block_t * dma_build_block(dma_block_t *block, dma_block_config_t *config)
{
    co_assert(block != NULL);
    if (block == NULL || config == NULL)
        return NULL;

    block->Ctrl = dma_setup_ch_ctrl_ex(config); // Mask interrupts by default.
    block->SrcAddr = (uint32_t)config->src->addr;
    block->DstAddr = (uint32_t)config->dst->addr;
    block->TranSize = config->block_size_in_bytes / (1u<<config->src->bus_width);
    block->LLPointer = NULL;

    return block;
}

/**
 * @brief DMA append block to the tail of a block list.
 *
 * @param[in] list    block list to be appendded to.
 * @param[in] block   block to append.
 *
 * @return Pointer to the list head.
 **/
dma_block_t *dma_append_block(dma_block_t *list, dma_block_t *block)
{
    if (list == NULL)
        list = block;
    else
    {
        dma_block_t *p = list;
        while (p->LLPointer != NULL)
        {
            p = (dma_block_t *)p->LLPointer;
        }
        p->LLPointer = block;
    }
    return list;
}

/**
 * @brief DMA start block transfers
 *
 * @param[in] ch      Pointer to DMA channel object, auto-allocated when NULL is specified.
 * @param[in] block   List of blocks to transfer.
 * @param[in] cb      Callback function.
 *
 * @return DMA channel used or NULL when fail.
 **/
XH_DMA_CH_Type *dma_start_transfer(XH_DMA_CH_Type *ch, dma_block_t *block, dma_callback_t cb)
{
    if (block == NULL)
        return NULL;

    if (ch == NULL)
    {
        ch = dma_allocate();
        if (ch == NULL)
            return NULL;
    }

    int ch_index = DMA_CH2INDEX(ch);

    // To simplify usage, interrupts of all blocks must all enabled or disabled.
    dma_env.callback[ch_index] = cb;
    dma_env.block[ch_index] = block;
    bool int_mask = cb ? false : true;
    for (dma_block_t *p = block; p != NULL; p = (dma_block_t *)p->LLPointer)
    {
       REGW(&p->Ctrl, MASK_3REG(DMA_INTABTMASK,int_mask, DMA_INTERRMASK,int_mask, DMA_INTTCMASK,int_mask));  
    }

    // To simplify usage, src/dst of all blocks must all select SPI1 or I2C2 when using one of them is used.
    if (dma_env.req_ack_pair_sel_i2c2_or_spi1)
        XH_SYS->MON |= (1u<<16);
    else
        XH_SYS->MON &= ~(1u<<16);
    
    *ch = *block;
    // some clock auto gate when WFI (DMA should open)
    if (XH_PMU->SW_STATUS & PMU_SW_STATUS_AGGRESSIVE_CPUGATE_MASK)
        REGW0(XH_SYS->PINMUX, SYS_PINMUX_SYSPLL_GT_CPUCLK_HW_CTRL_MASK);

    REGW1(&ch->Ctrl, DMA_ENABLE_MASK);
    pmu_lowpower_prevent(pmu_lp_dma_ch(ch_index));
    
    return ch;
}

/**
 * @brief DMA stop
 *
 * @param[in] ch  DMA channel object
 *
 * @return None
 **/
void dma_stop(XH_DMA_CH_Type *ch)
{
    uint32_t ch_index = DMA_CH2INDEX(ch);
    uint32_t ch_mask = 1u << ch_index;

    XH_DMAC->ChAbort = ch_mask;

    REGW0(&ch->Ctrl, DMA_ENABLE_MASK);

    while (REGR(&XH_DMA_CH0->Ctrl, MASK_POS(DMA_ENABLE)))
        ;

    REGW(&XH_DMAC->IntStatus.status, MASK_3REG(DMAC_TC,ch_mask, DMAC_ABORT,ch_mask, DMAC_ERROR,ch_mask));
    NVIC_DisableIRQ(DMA_IRQn);
    pmu_lowpower_allow(pmu_lp_dma_ch(ch_index));
}

/**
 * @brief DMA wait stop
 *
 * @param[in] ch  DMA channel object
 *
 * @return None
 **/
void dma_wait_stop(XH_DMA_CH_Type *ch)
{
    uint32_t ch_index = DMA_CH2INDEX(ch);
    uint32_t ch_mask = 1u << ch_index;
    
    while (REGR(&ch->Ctrl, MASK_POS(DMA_ENABLE)) || ch->TranSize || ch->LLPointer)
        ;
    
    REGW(&XH_DMAC->IntStatus.status, MASK_3REG(DMAC_TC,ch_mask, DMAC_ABORT,ch_mask, DMAC_ERROR,ch_mask));   
    pmu_lowpower_allow(pmu_lp_dma_ch(ch_index));
}

/*********************************************************************
 * HELPER FUNCTIONS
 */
dma_dir_t dma_get_direction(dma_id_t src_id, dma_id_t dst_id)
{
    if (src_id >= DMA_MAX_ID)
        return DMA_DIR_MAX;

    if (src_id == MEM_DMA_ID)
    {
        if (dst_id == MEM_DMA_ID)
            return DMA_MEM_TO_MEM;
        else
            return DMA_MEM_TO_DEV;
    }
    
    if (dst_id == MEM_DMA_ID)
        return DMA_DEV_TO_MEM;
    else
        return DMA_DEV_TO_DEV;
}

/** @} */


