/**
 * @file sf_base.c
 * @brief 
 * @date Mon, Jul  8, 2019  2:10:02 PM
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
#include "sf_base_ex.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */
sfb_env_t sfb_env = {
#ifdef CONFIG_XIP_FLASH_ALL
    /* critical_obj */  XH_SF,
#else
    /* critical_obj */  NULL,
#endif
    /* ... */
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief  sfb memcopy reverse
 *
 * @param[in] dst  dst
 * @param[in] src  src
 * @param[in] n  n
 **/
static void sfb_memcopy_reverse(uint8_t *dst, const uint8_t *src, uint32_t n)
{
    uint32_t i;
    for (i=0; i<n; ++i)
        dst[n-1-i] = src[i];
}

/**
 * @brief  sfb cpm
 *
 * @param[in] sf  sf
 *
 * @return cpm
 **/
static __IO uint32_t *sfb_cpm(XH_SF_Type *sf)
{
    return sf==XH_SF ? &XH_PSO->SF0_CFG : &XH_PSO->SF1_CFG;
}

/**
 * @brief  sfb clk
 *
 * @param[in] sf  sf
 *
 * @return clk
 **/
static uint32_t sfb_clk(XH_SF_Type *sf)
{
    return sf==XH_SF ? cpm_get_clock(CPM_SF0_CLK) : cpm_get_clock(CPM_SF1_CLK);
}


/**
 * @brief sfb_is_opened()
 *
 * @param[in] sf  sf object
 *
 * @return 
 **/
static bool sfb_is_opened(XH_SF_Type *sf)
{
    return (*sfb_cpm(sf) & CPM_SF_GATE_EN_MASK) ? false : true;
}

/**
 * @brief sfb_process_nonblock()
 *
 * @param[in] sf  sf object
 * @param[in] spi_cmd  
 * @param[in] cmd  
 * @param[in] data  
 *
 * @return 
 **/
static void sfb_process_nonblock(XH_SF_Type *sf, spi_cmd_t *spi_cmd, uint32_t cmd[2], void *data)
{
    int sf_index = sfb_index(sf);

    // check
    co_assert((sf->INTR_MASK & SFB_DONE) == 0);

    // Prevent sleep
    pmu_lowpower_prevent(PMU_LP_SF0<<sf_index);

    // Clear and Enable done IQR
    sf->RAW_INTR_STATUS = SFB_DONE;
    sf->INTR_MASK = SFB_DONE;

    // ctrl
    sf->COMMAND_DATA0_REG = cmd[0];
    sf->COMMAND_DATA1_REG = cmd[1];
    sf->ADDRESS_REG = (uint32_t)data;
    sf->COMMAND = *(uint32_t *)spi_cmd;
}

/**
 * @brief  sfb process block
 *
 * @param[in] sf  sf
 * @param[in] spi_cmd  spi cmd
 * @param[in] cmd cmd
 * @param[in] data  data
 **/
static void sfb_process_block(XH_SF_Type *sf, spi_cmd_t *spi_cmd, uint32_t cmd[2], void *data)
{
    uint32_t irq_save = 0;

    // check
    co_assert((sf->INTR_MASK & SFB_DONE) == 0);

    // critical entry
    if(sfb_env.critical_obj == sf)
        CO_DISABLE_IRQ_EX(irq_save);

    // ctrl
    sf->COMMAND_DATA0_REG = cmd[0];
    sf->COMMAND_DATA1_REG = cmd[1];
    sf->ADDRESS_REG = (uint32_t)data;
    sf->COMMAND = *(uint32_t *)spi_cmd;

    // wait done
    while(!(sf->RAW_INTR_STATUS & SFB_DONE));
    sf->RAW_INTR_STATUS = SFB_DONE;

    // critical exit
    if(sfb_env.critical_obj == sf)
        CO_RESTORE_IRQ_EX(irq_save);
}

/**
 * @brief sfb_raw_rdata()
 *
 * @param[in] sf  sf object
 * @param[in] data  
 *
 * @return 
 **/
static void sfb_raw_rdata(XH_SF_Type *sf, uint32_t data[2])
{
    data[0] = sf->READ0_REG;
//    data[1] = sf->READ1_REG;
}

/**
 * @brief sfb_raw_rdata0()
 *
 * @param[in] sf  sf object
 * @param[in] data  
 *
 * @return 
 **/
static void sfb_raw_rdata0(XH_SF_Type *sf, uint32_t data[1])
{
    data[0] = sf->READ0_REG;
}

/**
 * @brief sfb_read_nodma_1cmd_le3data()
 *
 * more efficiency than sfb_read_nodma_common
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] param  rw param
 *
 * @return 
 **/
static void sfb_read_nodma_1cmd_le3data(XH_SF_Type *sf, uint32_t cs, sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    uint32_t rdata[1];

    co_assert(cs < SFB_CS_NUM);

    co_assert(param->cmd_bits == 8);
    co_assert(param->data_bytes <= 3);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = param->cmd_bits + (param->data_bytes << 3);
    spi_cmd.dataBytes = 0;
    spi_cmd.cmd = SFB_CMD_READ;
    spi_cmd.cs = cs;

    // send cmd
    sfb_process_block(sf, &spi_cmd, param->cmd, NULL);

    // copy result
    sfb_raw_rdata0(sf, rdata);

    // convert
    sfb_memcopy_reverse((void *)param->data, (void *)rdata, param->data_bytes);
}

/**
 * @brief sfb_read_nodma()
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] param  rw param
 *
 * @return 
 **/
static void sfb_read_nodma_common(XH_SF_Type *sf, uint32_t cs, sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    uint32_t rdata[2], wdata[2] = {0}, i, i4x;
    uint32_t cur_data_bytes, cur_data_bytes_4align;
    uint32_t left_data_bytes = param->data_bytes;
    uint32_t index_data_bytes = 0;

    co_assert(cs < SFB_CS_NUM);

    uint32_t irq_save = 0;
    if(sfb_env.critical_obj == sf)
        CO_DISABLE_IRQ_EX(irq_save);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = param->cmd_bits;
    spi_cmd.dataBytes = 0;
    spi_cmd.cmd = SFB_CMD_READ;
    spi_cmd.cs = cs;
    spi_cmd.keepCs = left_data_bytes ? 1 : 0;

    // send cmd
    sfb_process_block(sf, &spi_cmd, param->cmd, NULL);

    // read data
    while(left_data_bytes)
    {
        cur_data_bytes = (left_data_bytes > SFB_NODMA_READ_DATA_LEN_MAX) ? SFB_NODMA_READ_DATA_LEN_MAX : left_data_bytes;
        left_data_bytes -= cur_data_bytes;

        // new cmd
        spi_cmd.cmdBits = cur_data_bytes << 3;
        spi_cmd.keepCs = left_data_bytes ? 1 : 0;

        // read data
        sfb_process_block(sf, &spi_cmd, wdata, NULL);

        // copy result
        sfb_raw_rdata(sf, rdata);

        // save
        cur_data_bytes_4align = cur_data_bytes >> 2;
        for (i=0,i4x=0; i4x<cur_data_bytes_4align; ++i4x,i+=4)
            sfb_memcopy_reverse(&((uint8_t *)param->data)[index_data_bytes + i], &((uint8_t *)rdata)[i], 4);
        sfb_memcopy_reverse(&((uint8_t *)param->data)[index_data_bytes + i], &((uint8_t *)rdata)[i], cur_data_bytes - i);

        index_data_bytes += cur_data_bytes;
    }

    if(sfb_env.critical_obj == sf)
        CO_RESTORE_IRQ_EX(irq_save);
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief read with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] keep_cs  keep cs
 * @param[in] param  rw param
 **/
void sfb_read_dma_ex(XH_SF_Type *sf, uint32_t cs, sfb_keep_cs_t keep_cs, sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    int sf_index = sfb_index(sf);

    co_assert(cs < SFB_CS_NUM);

    co_assert(param->data_bytes <= SFB_DMA_DATA_LEN_MAX);
    co_assert(((uint32_t)param->data & 3) == 0);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = (keep_cs==SFB_CS_BEGIN||keep_cs==SFB_CS_NOKEEP) ? param->cmd_bits : 0;
    spi_cmd.dataBytes = param->data_bytes;
    spi_cmd.cmd = SFB_CMD_READ;
    spi_cmd.cs = cs;
    spi_cmd.keepCs = (keep_cs==SFB_CS_END||keep_cs==SFB_CS_NOKEEP) ? 0 : 1;

    // op
    if(sfb_env.callback[sf_index])
        sfb_process_nonblock(sf, &spi_cmd, param->cmd, (void *)param->data);
    else
        sfb_process_block(sf, &spi_cmd, param->cmd, (void *)param->data);
}

/**
 * @brief read with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
void sfb_read_dma(XH_SF_Type *sf, uint32_t cs, sfb_rw_params_t *param)
{
    sfb_read_dma_ex(sf, cs, SFB_CS_NOKEEP, param);
}

/**
 * @brief write with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] keep_cs  keep cs
 * @param[in] param  rw param
 **/
void sfb_write_dma_ex(XH_SF_Type *sf, uint32_t cs, sfb_keep_cs_t keep_cs, sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    int sf_index = sfb_index(sf);

    co_assert(cs < SFB_CS_NUM);

    co_assert(param->data_bytes <= SFB_DMA_DATA_LEN_MAX);
    co_assert(((uint32_t)param->data & 3) == 0);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = (keep_cs==SFB_CS_BEGIN||keep_cs==SFB_CS_NOKEEP) ? param->cmd_bits : 0;
    spi_cmd.dataBytes = param->data_bytes;
    spi_cmd.cmd = SFB_CMD_WRITE;
    spi_cmd.cs = cs;
    spi_cmd.keepCs = (keep_cs==SFB_CS_END||keep_cs==SFB_CS_NOKEEP) ? 0 : 1;
    spi_cmd.lcd2lane = sfb_env.lcd2lane[sf_index][cs];

    // op
    if(sfb_env.callback[sf_index])
        sfb_process_nonblock(sf, &spi_cmd, param->cmd, (void *)param->data);
    else
        sfb_process_block(sf, &spi_cmd, param->cmd, (void *)param->data);
}

/**
 * @brief write with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
void sfb_write_dma(XH_SF_Type *sf, uint32_t cs, sfb_rw_params_t *param)
{
    sfb_write_dma_ex(sf, cs, SFB_CS_NOKEEP, param);
}

/**
 * @brief read without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
void sfb_read_nodma(XH_SF_Type *sf, uint32_t cs, sfb_rw_params_t *param)
{
    co_assert(cs < SFB_CS_NUM);

    if (param->cmd_bits == 8 && param->data_bytes <= 3)
        sfb_read_nodma_1cmd_le3data(sf, cs, param); // more efficiency
    else
        sfb_read_nodma_common(sf, cs, param);
}

/**
 * @brief write without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
void sfb_write_nodma(XH_SF_Type *sf, uint32_t cs, sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    uint32_t wdata[2], i;
    uint32_t cur_data_bytes;
    uint32_t left_data_bytes = param->data_bytes;
    uint32_t index_data_bytes = 0;

    co_assert(cs < SFB_CS_NUM);

    uint32_t irq_save = 0;
    if(sfb_env.critical_obj == sf)
        CO_DISABLE_IRQ_EX(irq_save);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = param->cmd_bits;
    spi_cmd.dataBytes = 0;
    spi_cmd.cmd = SFB_CMD_WRITE;
    spi_cmd.cs = cs;
    spi_cmd.keepCs = left_data_bytes ? 1 : 0;

    // send cmd
    sfb_process_block(sf, &spi_cmd, param->cmd, NULL);

    // read data
    while(left_data_bytes)
    {
        cur_data_bytes = (left_data_bytes > SFB_NODMA_WRITE_DATA_LEN_MAX) ? SFB_NODMA_WRITE_DATA_LEN_MAX : left_data_bytes;
        left_data_bytes -= cur_data_bytes;

        // save
        for (i=0; i<cur_data_bytes; ++i)
            ((uint8_t *)wdata)[(i&~3)+3-(i%4)] = ((uint8_t *)param->data)[index_data_bytes + i];
        index_data_bytes += cur_data_bytes;

        // new cmd
        spi_cmd.cmdBits = cur_data_bytes << 3;
        spi_cmd.keepCs = left_data_bytes ? 1 : 0;

        // read data
        sfb_process_block(sf, &spi_cmd, wdata, NULL);
    }

    if(sfb_env.critical_obj == sf)
        CO_RESTORE_IRQ_EX(irq_save);
}

/**
 * @brief open
 *
 * @param[in] sf  sf object
 **/
void sfb_open(XH_SF_Type *sf)
{
    register_set(sfb_cpm(sf), MASK_3REG(CPM_SF_SOFT_RESET,1, CPM_SF_DIV_SEL,0, CPM_SF_GATE_EN,0));
    XH_PSO_UPD_RDY();
}

/**
 * @brief close
 *
 * @param[in] sf  sf object
 **/
void sfb_close(XH_SF_Type *sf)
{
    register_set(sfb_cpm(sf), MASK_3REG(CPM_SF_SOFT_RESET,0, CPM_SF_DIV_SEL,0, CPM_SF_GATE_EN,1));
    XH_PSO_UPD_RDY();
}

/**
 * @brief config
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] config  config
 **/
void sfb_config(XH_SF_Type *sf, uint32_t cs, const sfb_config_t *config)
{
    int sf_index = sfb_index(sf);
    uint32_t clk = sfb_clk(sf);
    uint32_t div = config->freq_hz<256 ? config->freq_hz : DIV_ROUND_UP(clk, config->freq_hz);

    co_assert(cs < SFB_CS_NUM);

    if (div > (SF_CTRL_CLOCK_DIV_MASK))
        div = SF_CTRL_CLOCK_DIV_MASK;
    div &= ~1; // must be even

    // Write REG
    REGWA(&sf->CONFIGURATION[cs].CTRL, MASK_8REG(
        SF_CTRL_LCD_RD_EN,      0,
        SF_CTRL_RGB_MODE,       0,
        SF_CTRL_LCD_SPI_CTRL,   0,
        SF_CTRL_WIDTH,          0,
        SF_CTRL_DLYX_SAMPLE,    config->delay,
        SF_CTRL_BP_CLOCK_DIV,   div<2 ? 1 : 0,
        SF_CTRL_MODE,           config->transmode,
        SF_CTRL_CLOCK_DIV,      div<2 ? 2 : div));

    REGW(&sf->CONFIGURATION[cs].CS, MASK_1REG(SF_CS_POL, config->cs_pol));

    // Default disable all done irq
    sf->INTR_MASK = 0; /* disable all */
    sf->RAW_INTR_STATUS = SFB_DONE; /* clear */

    // default
    sfb_env.lcd2lane[sf_index][cs] = 0;

    // Save
    sfb_env.configuration_save[sf_index][cs].ctrl = sf->CONFIGURATION[cs].CTRL;
    sfb_env.configuration_save[sf_index][cs].cs   = sf->CONFIGURATION[cs].CS;
}

/**
 * @brief restore
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 **/
void sfb_restore(XH_SF_Type *sf, uint32_t cs)
{
    int sf_index = sfb_index(sf);

    co_assert(cs < SFB_CS_NUM);

    if(sfb_env.configuration_save[sf_index][cs].ctrl == 0)
        return;

    sf->CONFIGURATION[cs].CTRL = sfb_env.configuration_save[sf_index][cs].ctrl;
    sf->CONFIGURATION[cs].CS = sfb_env.configuration_save[sf_index][cs].cs;

    if (sfb_env.callback[sf_index])
    {
        IRQn_Type irqn = sfb_irqn(sf);
        NVIC_SetPriority(irqn, IRQ_PRIORITY_NORMAL);
        NVIC_EnableIRQ(irqn);
    }
}

/**
 * @brief  sfb critical object set
 *
 * @param[in] sf  sf
 **/
void sfb_critical_object_set(XH_SF_Type *sf)
{
    sfb_env.critical_obj = sf;
}

/**
 * @brief  sfb critical object get
 *
 * @return obj
 **/
XH_SF_Type *sfb_critical_object_get(void)
{
    return sfb_env.critical_obj;
}

/**
 * @brief sfb_enable()
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 **/
void sfb_enable(XH_SF_Type *sf, uint32_t cs)
{
    if(!sfb_is_opened(sf))
        sfb_open(sf);
    sfb_restore(sf, cs);
}

/** @} */

