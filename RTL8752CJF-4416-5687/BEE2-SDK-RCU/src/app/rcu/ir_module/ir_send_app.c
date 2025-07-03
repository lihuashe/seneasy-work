/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file         ir_send_demo.c
* @brief        This file provides IR demo code to send data by interrupt.
* @details     Application users can call ir_learn_and_send_demo_code function to test IR learn.
* @author       elliot chen
* @date         2017-11-22
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include "system_rtl876x.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_nvic.h"
#include "rtl876x_ir.h"
#include "ir_learn.h"
#include "trace.h"
#include "rtl876x_nvic.h"
#include "rtl876x_rtc.h"
#include "app_msg.h"
#include "ir_nec_encode.h"
#include "ir_repeat_code.h"
#include "ftl.h"
#include "key_handle.h"
#if IR_FUN
#include "ir_app_config.h"
#endif
#include <app_section.h>
#include "rtc_driver.h"


#if IR_FUN

#if IR_LEARN_MODE
#define isLearnWork()                                        is_ir_learn_working()
#define IR_GetSendAttrValue(irAttr, attrValue, key_index)    ir_learn_get_attr(irAttr, attrValue, key_index)
#else
#define isLearnWork()                                        false
#define IR_GetSendAttrValue(irAttr, attrValue, key_index)    false//IR_GetLearnAttr
#endif

/*============================================================================*
 *                          Local Variables
 *============================================================================*/
#if IR_LEARN_MODE
extern IR_LearnTypeDef IR_LearnPacket;
#endif
/* Number of data which has been sent */
static uint8_t tx_count = 0;
static IR_SendStruct gIrSendStruct = {0};
static IR_RepeatDataTypeDef gIrRepeatCodeStruct = {0};
static uint32_t sendbuf[IR_SEND_WAVE_MAX_SIZE] = {0};

static IRDA_BUF gIrBuf = {0};
//IR Table
/* ir map */
/* BLE HID code table definition */
const uint8_t gIrTable[BLE_KEY_CODE_TABLE_SIZE] =
{
#if (RCU_HD_PLATFORM_SEL != G_MIN_DEMO_RCU)
    0x00,  /* VK_NC */
    0x18,  /* VK_POWER */
    0x08,  /* VK_PAGE_UP */
    0x09,  /* VK_PAGE_DOWN */
    0x56,  /* VK_MENU */
    0x14,  /* VK_HOME */
    0x3E,  /* VK_VOICE */
    0x4C,  /* VK_ENTER */
    0x57,  /* VK_EXIT */
    0x0C,  /* VK_LEFT */
    0x0E,  /* VK_RIGHT */
    0x4D,  /* VK_UP */
    0x48,  /* VK_DOWN */
    0x00,  /* VK_MOUSE_EN */
    0x7F,  /* VK_VOLUME_MUTE */
    0x49,  /* VK_VOLUME_UP */
    0x4B,  /* VK_VOLUME_DOWN */
    0x3F,  /* VK_VOICE_STOP */
    0x00,  /* VK_TV_POWER */
    0x5A,  /* VK_TV_SIGNAL */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0x00,  /* MM_ScanNext */
    0x00,  /* MM_ScanPrevious */
    0x00,  /* MM_Stop */
    0x00,  /* MM_Play_Pause */
    0x00,  /* MM_Mute */
    0x00,  /* MM_BassBoost */
    0x00,  /* MM_Loudness */
    0x00,  /* MM_VolumeIncrement */
    0x00,  /* MM_VolumeDecrement */
    0x00,  /* MM_BassIncrement */
    0x00,  /* MM_BassDecrement */
    0x00,  /* MM_TrebleIncrement */
    0x00,  /* MM_TrebleDecrement */
    0x00,  /* MM_AL_ConsumerControl */
    0x00,  /* MM_AL_EmailReader */
    0x00,  /* MM_AL_Calculator */
    0x00,  /* MM_AL_LocalMachineBrowser */
    0x00,  /* MM_AC_Search */
    0x00,  /* MM_AC_Home */
    0x00,  /* MM_AC_Back */
    0x00,  /* MM_AC_Forward */
    0x00,  /* MM_AC_Stop */
    0x00,  /* MM_AC_Refresh */
    0x00,  /* MM_AC_Bookmarks */
#endif
#else
    0x00,  /* VK_NC */
    0x18,  /* VK_POWER */
    0x08,  /* VK_PAGE_UP */
    0x09,  /* VK_PAGE_DOWN */
    0x56,  /* VK_MENU */
    0x14,  /* VK_HOME */
    0x3E,  /* VK_VOICE */
    0x4C,  /* VK_ENTER */
    0x57,  /* VK_EXIT */
    0x0C,  /* VK_LEFT */
    0x0E,  /* VK_RIGHT */
    0x4D,  /* VK_UP */
    0x48,  /* VK_DOWN */
    0x00,  /* VK_MOUSE_EN */
    0x7F,  /* VK_VOLUME_MUTE */
    0x23,  /* VK_VOLUME_UP */
    0x24,  /* VK_VOLUME_DOWN */
    0x3F,  /* VK_VOICE_STOP */
    0x01,  /* VK_TV_POWER */
    0x5A,  /* VK_TV_SIGNAL */
    0x63,  /* VK_NETFLIX */
    0x64,  /* VK_YOUTUBE */
    0x67,  /* VK_APP04 */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0x00,  /* MM_ScanNext */
    0x00,  /* MM_ScanPrevious */
    0x00,  /* MM_Stop */
    0x52,  /* MM_Play_Pause */
    0x25,  /* MM_Mute */
    0x00,  /* MM_BassBoost */
    0x00,  /* MM_Loudness */
    0x00,  /* MM_VolumeIncrement */
    0x00,  /* MM_VolumeDecrement */
    0x00,  /* MM_BassIncrement */
    0x00,  /* MM_BassDecrement */
    0x00,  /* MM_TrebleIncrement */
    0x00,  /* MM_TrebleDecrement */
    0x00,  /* MM_AL_ConsumerControl */
    0x00,  /* MM_AL_EmailReader */
    0x00,  /* MM_AL_Calculator */
    0x00,  /* MM_AL_LocalMachineBrowser */
    0x46,  /* MM_AC_Search */
    0x47,  /* MM_AC_Home */
    0x48,  /* MM_AC_Back */
    0x00,  /* MM_AC_Forward */
    0x56,  /* MM_AC_Stop */
    0x00,  /* MM_AC_Refresh */
    0x74,  /* MM_AC_Bookmarks */
    0x71,  /* MM_Dashboard */
    0x15,  /* MM_DPadUp */
    0x16,  /* MM_DPadDown */
    0x17,  /* MM_DPadLeft */
    0x18,  /* MM_DPadRight */
    0x19,  /* MM_DPadCenter */
    0x32,  /* MM_Guide */
    0x61,  /* MM_Live */
    0x96,  /* MM_BugReport */
#endif
#endif
};

/*============================================================================*
 *                          Local Functions Declearation
 *============================================================================*/
void ir_send_int_handler(void);
void ir_send_repeat_code_handler(void);
void ir_send_exit(void);

/*============================================================================*
 *                          External Functions
 *============================================================================*/
bool app_send_msg_to_apptask(T_IO_MSG *p_msg);
#define ir_send_msg_to_app app_send_msg_to_apptask

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup IR_Send_Demo_Exported_Functions IR_Send_Demo_Exported_Functions
  * @brief
  * @{
  */

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void ir_send_board_init(void)
{
    Pad_Config(IR_SEND_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pinmux_Config(IR_SEND_PIN, IRDA_TX);

#if IR_LEARN_MODE
    /*ir learn pin pull low*/
    Pinmux_Deinit(IR_LEARN_PIN);
    Pad_Config(IR_LEARN_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_LOW);
#endif
}

/**
  * @brief  initialization of RTC, which is used for repeat code send.
  * @param   No parameter.
  * @return  void
  */
void ir_send_repeat_code_init(void)
{
    IR_Repeat_Code_Encode(gIrSendStruct.irSendFrequency, &gIrRepeatCodeStruct);

    /*enable RTC*/
    rtc_driver_set_comp_0_params(IR_REPEAT_CODE_PERIOD, ir_send_repeat_code_handler);
}

/**
  * @brief  Initialize IR peripheral.
  * @param   No parameter.
  * @return  void
  */
void ir_send_driver_init(void)
{
    /* Enable IR clock */
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);

    /* Initialize IR */
    IR_InitTypeDef IR_InitStruct;
    IR_StructInit(&IR_InitStruct);
    IR_InitStruct.IR_Freq           = gIrSendStruct.irSendFrequency;//IR_LearnPacket.freq;
    IR_InitStruct.IR_DutyCycle      = 2;//IR_LearnPacket.duty_cycle;
    IR_InitStruct.IR_Mode           = IR_MODE_TX;
    IR_InitStruct.IR_TxInverse      = IR_TX_DATA_NORMAL;
    IR_InitStruct.IR_TxFIFOThrLevel = IR_TX_FIFO_THR_LEVEL;
    IR_Init(&IR_InitStruct);
    IR_Cmd(IR_MODE_TX, ENABLE);

    /* Configure NVIC */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = IR_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* Modify IR interrupt handle */
    RamVectorTableUpdate(IR_VECTORn, ir_send_int_handler);
}

bool ir_send_local_config(uint32_t key_value)
{
    gIrSendStruct.irSendFrequency = IR_SEND_FREQUENCY;
    gIrBuf.pBuf = sendbuf;

    if (IRDA_SUCCEED != irda_nec_encode((uint8_t)IR_SEND_ADDR, gIrTable[key_value], &gIrBuf))
    {
        APP_PRINT_INFO0("[IR] IR local data encode failed!");
        return false;
    }

    return true;
}
/**
  * @brief  demo code of IR send module init.
  * @param   uint32_t key_value.
  * @return  bool
  */
bool ir_send_module_init(uint32_t key_value)
{
    bool ret1 = true, ret2 = true;

    ret1 = IR_GetSendAttrValue(IR_ATTR_FREQ, &gIrSendStruct.irSendFrequency, key_value);
    ret2 = IR_GetSendAttrValue(IR_ATTR_OFFSET, &gIrSendStruct.irLearnDataOffset, key_value);

    /*keep ir send buffer clean before next send start*/
    memset(sendbuf, 0, sizeof(sendbuf));

    if ((false == ret1) || (false == ret2))
    {
        APP_PRINT_INFO1("[IR] IR Send local data, key_value = %d.", key_value);
        ir_send_local_config(key_value);

        /* Initialize IR with repeat code */
        ir_send_board_init();
        ir_send_driver_init();
        ir_send_repeat_code_init();
    }
#if IR_LEARN_MODE
    else/*send ir learn wave data*/
    {
        APP_PRINT_INFO1("[IR] IR Send Learn data, offset: %d.", gIrSendStruct.irLearnDataOffset);
        ftl_load(sendbuf, IR_WAVE_DATA_BASE_ADDR + gIrSendStruct.irLearnDataOffset * IR_LEARN_WAVE_MAX_SIZE
                 * 4, IR_LEARN_WAVE_MAX_SIZE * 4);

        /* Initialize IR without repeat code */
        ir_send_board_init();
        ir_send_driver_init();
    }
#endif

    APP_PRINT_INFO0("[IR] IR_Send_Module_Init.");

    return true;
}

/**
  * @brief  demo code of IR send start.
  * @param   No parameter.
  * @return  void
  */
bool ir_send_start(void)
{
    /* Start to send first bytes data of encoded data */
    IR_SendBuf(sendbuf/*IR_LearnPacket.ir_buf*/, IR_TX_FIFO_SIZE, DISABLE);
    /* Record number which has been sent */
    tx_count = IR_TX_FIFO_SIZE;

    /* Enable IR threshold interrupt. when TX FIFO offset <= threshold value, trigger interrupt*/
    IR_INTConfig(IR_INT_TF_LEVEL, ENABLE);

    return true;
}
/**
  * @brief   IR app send msg process.
  * @param   uint16_t msg_sub_type.
  * @return  bool
  */
bool ir_send_msg_proc(uint16_t msg_sub_type)
{
    bool ret = false;
    //APP_PRINT_INFO1("[IR] IR send sub type: %d", msg_sub_type);
    if (msg_sub_type == IO_MSG_TYPE_IR_SEND_REPEAT_CODE)
    {
        if (gIrSendStruct.isIrSendComplete != IR_SEND_COMPLETED)
        {
            ir_send_exit();
            ret = false;
        }
        else
        {
            APP_PRINT_INFO0("[IR] IR send repeat code.");
            IR_SendBuf(gIrRepeatCodeStruct.irBuf, gIrRepeatCodeStruct.bufLen, DISABLE);
            gIrSendStruct.irSendStage = IR_SEND_REPEAT_CODE;
            gIrSendStruct.isIrSendComplete = IR_SEND_NOT_COMPLETED;

            IR_INTConfig(IR_INT_TF_LEVEL, ENABLE);
            /*enable RTC*/
            rtc_driver_set_comp_0_params(IR_REPEAT_CODE_PERIOD, ir_send_repeat_code_handler);

            ret = true;
        }
    }
    else if (msg_sub_type == IO_MSG_TYPE_IR_SEND_COMPLETE)
    {
        if (gIrSendStruct.irSendKeyState == IR_SEND_KEY_RELEASE)
        {
            ir_send_exit();
        }
    }
    return ret;
}
/**
* @brief IR interrupt handler function.
* @param   No parameter.
* @return  void
*/
void ir_send_int_handler(void) DATA_RAM_FUNCTION;
void ir_send_int_handler(void)
{
    /* Mask IR interrupt */
    IR_MaskINTConfig(IR_INT_TF_LEVEL, ENABLE);

    APP_PRINT_INFO0("[IR] IR Handler");

    /* Continue to send by interrupt */
    if (IR_GetINTStatus(IR_INT_TF_LEVEL) == SET)
    {

        if (gIrSendStruct.irSendStage == IR_SEND_CAMMAND)
        {
            /* The remaining data is larger than the TX FIFO length */
            if (((IR_SEND_WAVE_MAX_SIZE/*IR_LearnPacket.buf_index + 1*/) - tx_count) >= IR_TX_FIFO_SIZE)
            {
                IR_SendBuf(sendbuf/*IR_LearnPacket.ir_buf*/ + tx_count, (IR_TX_FIFO_SIZE - IR_TX_FIFO_THR_LEVEL),
                           DISABLE);
                tx_count += (IR_TX_FIFO_SIZE - IR_TX_FIFO_THR_LEVEL);
            }
            else if (((IR_SEND_WAVE_MAX_SIZE/*IR_LearnPacket.buf_index + 1*/) - tx_count) > 0)
            {
                /* The remaining data is less than the TX FIFO length */

                /*  Configure TX threshold level to zero and trigger interrupt when TX FIFO is empty */
                IR_SetTxThreshold(0);
                IR_SendBuf(sendbuf/*IR_LearnPacket.ir_buf*/ + tx_count,
                           (IR_SEND_WAVE_MAX_SIZE/*IR_LearnPacket.buf_index + 1*/) - tx_count,
                           DISABLE);
                tx_count += ((IR_SEND_WAVE_MAX_SIZE/*IR_LearnPacket.buf_index + 1*/) - tx_count);
            }
            else
            {
                /* Tx completed */

                /* Disable IR tx empty interrupt */
                IR_INTConfig(IR_INT_TF_LEVEL, DISABLE);
                tx_count = 0;

                gIrSendStruct.isIrSendComplete = IR_SEND_COMPLETED;
                APP_PRINT_INFO0("[IR] Ir send cammand complete.");
            }
        }
        else if (gIrSendStruct.irSendStage == IR_SEND_REPEAT_CODE)
        {
            gIrSendStruct.isIrSendComplete = IR_SEND_COMPLETED;
            APP_PRINT_INFO0("[IR] Ir send repeate code complete.");
        }

        if (gIrSendStruct.isIrSendComplete == IR_SEND_COMPLETED)
        {
            T_IO_MSG bee_io_msg;
            bee_io_msg.type = IO_MSG_TYPE_IR;

            /*send ir learn data message to app*/
            bee_io_msg.subtype = IO_MSG_TYPE_IR_SEND_COMPLETE;
            ir_send_msg_to_app(&bee_io_msg);
        }

        /* Clear threshold interrupt */
        IR_ClearINTPendingBit(IR_INT_TF_LEVEL_CLR);
    }

    /* Unmask IR interrupt */
    IR_MaskINTConfig(IR_INT_TF_LEVEL, DISABLE);
}
/**
  * @brief  RTC interrupt handle function.
  * @param  None.
  * @return None.
  */
void ir_send_repeat_code_handler(void)
{
    APP_PRINT_INFO0("[IR] IR Repeat Code Handler.");

    T_IO_MSG bee_io_msg;
    bee_io_msg.type = IO_MSG_TYPE_IR;
    bee_io_msg.subtype = IO_MSG_TYPE_IR_SEND_REPEAT_CODE;
    ir_send_msg_to_app(&bee_io_msg);
}
/**
* @brief  Application code for IR send key press handle.
* @param  uint32_t key_index_1;
*         uint32_t key_index_2.
* @return  void
*/
void ir_send_exit(void)
{
    gIrSendStruct.irSendStage = IR_SEND_IDLE;
    gIrSendStruct.isIrSendComplete = IR_SEND_NOT_COMPLETED;

    rtc_driver_clear_comp_0();

    IR_DeInit();
    IR_INTConfig(IR_INT_TF_LEVEL, DISABLE);

}

/**
* @brief  Application code for IR send key press handle.
* @param  uint32_t key_index_1;
*         uint32_t key_index_2.
* @return  void
*/
void ir_send_key_press_handle(uint32_t key_index_1, uint32_t key_index_2, uint32_t key_length)
{
    if (isLearnWork())
    {
        return;
    }

    if (false == ir_send_module_init(key_index_1))
    {
        return;
    }
    if (true == ir_send_start())
    {
        gIrSendStruct.irSendKeyState = IR_SEND_KEY_PRESS;
        gIrSendStruct.irSendStage = IR_SEND_CAMMAND;
        gIrSendStruct.isIrSendComplete = IR_SEND_NOT_COMPLETED;
    }
}

/**
* @brief  Application code for IR send key release handle.
* @param  uint32_t key_index_1;
*         uint32_t key_index_2.
* @return  void
*/
void ir_send_key_release_handle(void)
{
    if (isLearnWork())
    {
        return;
    }

    gIrSendStruct.irSendKeyState = IR_SEND_KEY_RELEASE;
    if (gIrSendStruct.isIrSendComplete == IR_SEND_COMPLETED)
    {
        ir_send_exit();
    }
}
/**
* @brief   check ir send if allow enter dlps.
* @param   void.
* @return  bool true allowed, false not allowed.
*/
bool ir_send_check_dlps(void)
{
    /*ir send in idle*/
    if (gIrSendStruct.irSendStage == IR_SEND_IDLE)
    {
        return true;
    }

    /*ir send not in idle and not send completed*/
    return false;
}

/**
* @brief   check ir send is working.
* @param   void.
* @return  bool working, false not working.
*/
bool is_ir_send_working(void)
{
    if (gIrSendStruct.irSendStage == IR_SEND_IDLE)
    {
        return false;
    }
    return true;
}

#endif /*end Micro @IR_FUN*/

/** @} */ /* End of group IR_Send_Demo_Exported_Functions */
/** @} */ /* End of group IR_SEND_DEMO */


/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

