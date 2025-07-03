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
#include "ir_repeat_code.h"
#include "ftl.h"
#include "key_handle.h"
#include "smartrcu_service.h"
#include "ir_app_config.h"
#include "os_sched.h"
#if IR_FUN

#include "ir_app_config.h"

#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
#include "ir_nec_encode.h"
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
#include "ir_nec_encode.h"
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
#include "ir_nec_encode.h"
#endif

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
static uint16_t tx_count = 0;
static IR_SendStruct gIrSendStruct = {0};
static IR_RepeatDataTypeDef gIrRepeatCodeStruct = {0};
static uint32_t sendbuf[IR_SEND_WAVE_MAX_SIZE] = {0};

static IRDA_BUF gIrBuf = {0};
//IR Table
/* ir map */
/* BLE HID code table definition */
const uint8_t gIrTable[BLE_KEY_CODE_TABLE_SIZE] =
{
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)    
    0x00,  /* VK_NC */
    0x0c,  /* VK_POWER */
    0x08,  /* VK_PAGE_UP */
    0x09,  /* VK_PAGE_DOWN */
    0x14,  /* VK_HOME */
    0x30,  /* VK_MENU */
    0x3E,  /* VK_VOICE */
    0x4C,  /* VK_ENTER */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0x00,  /* MM_ScanNext */
    0x00,  /* MM_POWER */
    0x13,  /* MM_NUMBER1 */
    0x12,  /* MM_NUMBER2 */
    0x10,  /* MM_NUMBER3 */
    0x17,  /* MM_NUMBER4 */
    0x16,  /* MM_NUMBER5 */
    0x14,  /* MM_NUMBER6 */
    0x1B,  /* MM_NUMBER7 */
    0x1A,  /* MM_NUMBER8 */
    0x18,  /* MM_NUMBER9 */
    0x1F,  /* MM_REWIND */
    0x1E,  /* MM_NUMBER0 */
    0x4C,  /* MM_FAST_FORWARD */
    0x50,  /* MM_PREVIOUS */
    0x03,  /* MM_Play_Pause */
    0x02,  /* MM_NEXT */
    0x48,  /* MM_VolumeIncrement */
    0x0B,  /* MM_VolumeDecrement */
    0x01,  /* MM_STOP */
    0x41,  /* MM_Mute */
    0x04,  /* MM_P_Increment */
    0x06,  /* MM_P_Decrement */
    0x58,  /* MM_AC_Search */
    0x5C,  /* MM_AC_Home */
    0x59,  /* MM_DPadUp */
    0x54,  /* MM_DPadLeft */
    0x55,  /* MM_DPadCenter */
    0x15,  /* MM_DPadRight */
    0x51,  /* MM_DPadDown */
    0x19,  /* MM_AC_Back */
    0x09,  /* MM_Setting */
    0x11,  /* MM_Bookmark */
    0X08,  /* MM_INFO */
    0x1C,  /* MM_Guide */
    0x45,  /* MM_YouTube */
    0x49,  /* MM_NETFLIX */
    0x07,  /* MM_PrimeVideo */
    0x25,  /* MM_Disney */
    0x2E,  /* MM_FAVORITES */
    0X20,  /* MM_Google_Play_Games */
    0x0A,  /* MM_RED */
    0x4D,  /* MM_GREEN */
    0X0E,  /* MM_YELLOW */
    0x0C,  /* MM_BLUE */
    0x18,  /* MM_REC_THREE_SEC */
    0xED,  /* MM_AC_Search_Noconnect */
    0x96,  /* MM_BugReport */
#endif
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
    0x00,  /* VK_NC */
    0x0c,  /* VK_POWER */
    0x08,  /* VK_PAGE_UP */
    0x09,  /* VK_PAGE_DOWN */
    0x14,  /* VK_HOME */
    0x30,  /* VK_MENU */
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
    0x00,  /* VK_NUMBER0 */
    0x01,  /* VK_NUMBER1 */
    0x02,  /* VK_NUMBER2 */
    0x03,  /* VK_NUMBER3 */
    0x04,  /* VK_NUMBER4 */
    0x05,  /* VK_NUMBER5 */
    0x06,  /* VK_NUMBER6 */
    0x07,  /* VK_NUMBER7 */
    0x08,  /* VK_NUMBER8 */
    0x09,  /* VK_NUMBER9 */
    0x37,  /* VK_RED */
    0x36,  /* VK_GREEN */
    0x32,  /* VK_YELLOW */
    0X1F,  /* VK_SUB */
    0x34,  /* VK_BLUE */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0x00,  /* MM_ScanNext */
    0x15,  /* MM_POWER */
    0x01,  /* MM_NUMBER1 */
    0x02,  /* MM_NUMBER2 */
    0x03,  /* MM_NUMBER3 */
    0x04,  /* MM_NUMBER4 */
    0x05,  /* MM_NUMBER5 */
    0x06,  /* MM_NUMBER6 */
    0x07,  /* MM_NUMBER7 */
    0x08,  /* MM_NUMBER8 */
    0x09,  /* MM_NUMBER9 */
    0x0A,  /* MM_TEST */
    0x00,  /* MM_NUMBER0 */
    0x49,  /* MM_SUB */
    0x47,  /* MM_RW */
    0x43,  /* MM_PLAY_PAUSE */
    0x46,  /* MM_FF */
    0x1B,  /* MM_VOL_UP */
    0x1A,  /* MM_VOL_DOWN */
    0x40,  /* MM_REC */
    0x16,  /* MM_MUTE */
    0x19,  /* MM_P_UP */
    0x18,  /* MM_P_DOWN */
    0x17,  /* MM_MENU */
    0x11,  /* MM_EXIT */
    0x0C,  /* MM_UP */
    0x0F,  /* MM_LEFT */
    0x10,  /* MM_OK */
    0x0E,  /* MM_RIGHT */
    0x0D,  /* MM_DOWN */
    0x13,  /* MM_BACK */
    0x42,  /* MM_HOME */
    0x25,  /* MM_FAVORITES */
    0X4D,  /* MM_VOICE */
    0x14,  /* MM_INFO */
    0x0B,  /* MM_GUIDE */
    0x12,  /* MM_SOURSE */
    0x4F,  /* MM_NETFLIX */
    0x4E,  /* MM_YOUTUBE */
    0x1C,  /* MM_PRIME_VIDEO */
    0X1E,  /* MM_DISNEY */
    0x53,  /* MM_RED */
    0x52,  /* MM_GREEN */
    0X51,  /* MM_YELLOW */
    0x50,  /* MM_BLUE */
    0x18,    /* MM_REC_THREE_SEC */
    0xED,  /* MM_AC_Search_Noconnect */
    0x96,  /* MM_BugReport */
#endif
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)    
    0x00,      /* VK_NC */
    0x01,      /* VK_NUMBER1 */
    0x02,      /* VK_NUMBER2 */
    0x03,      /* VK_NUMBER3 */
    0x04,      /* VK_NUMBER4 */
    0x05,      /* VK_NUMBER5 */
    0x06,      /* VK_NUMBER6 */
    0x07,      /* VK_NUMBER7 */
    0x08,      /* VK_NUMBER8 */
    0x09,      /* VK_NUMBER9 */
    0x0A,      /* VK_NUMBER0 */
    0x23,      /* VK_VolumeIncrement */
    0x24,      /* VK_VolumeDecrement */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0x00,      /* MM_ScanNext */
    0x21,      /* MM_POWER */
    0x25,      /* MM_Mute */
    0x00,      /* MM_NUMBER1 */
    0x00,      /* MM_NUMBER2 */
    0x00,      /* MM_NUMBER3 */
    0x00,      /* MM_NUMBER4 */
    0x00,      /* MM_NUMBER5 */
    0x00,      /* MM_NUMBER6 */
    0x00,      /* MM_NUMBER7 */
    0x00,      /* MM_NUMBER8 */
    0x00,      /* MM_NUMBER9 */
    0x29,      /* MM_INFO */
    0x00,      /* MM_NUMBER0 */
    0xD3,      /* MM_SWITCH */
    0x00,      /* MM_VolumeIncrement */
    0x60,      /* MM_Source */
    0x33,      /* MM_P_Increment */
    0x00,      /* MM_VolumeDecrement */
    0xD4,      /* MM_MENU */
    0x34,      /* MM_P_Decrement */
    0x59,      /* MM_CONTACT */
    0x46,      /* MM_AC_Search */
    0x10,      /* MM_Setting */
    0x15,      /* MM_DPadUp */
    0x17,      /* MM_DPadLeft */
    0x19,      /* MM_DPadCenter */
    0x18,      /* MM_DPadRight */
    0x16,      /* MM_DPadDown */
    0x48,      /* MM_AC_Back */
    0x47,      /* MM_AC_Home */
    0x32,      /* MM_TV */
    0x63,      /* MM_NETFLIX */
    0x64,      /* MM_YouTube */
    0X1F,      /* MM_PrimeVideo */
    0x72,      /* MM_YouTubeMusic */
    0x4B,      /* MM_RED */
    0x4A,      /* MM_GREEN */
    0x49,      /* MM_YELLOW */
    0x4C,      /* MM_BLUE */

    0x96,      /* MM_BugReport */
    0xD0,      /* MM_TalkBack */
    0x00,      /* MM_POWER_INCERSE */
    0x00,      /* MM_NETFLIX_INCERSE */
    0x00,      /* MM_YouTube_INCERSE */
    0x00,      /* MM_AC_Search_Noconnect */
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

// #if IR_LEARN_MODE
//     /*ir learn pin pull low*/
//     Pinmux_Deinit(IR_LEARN_PIN);
//     Pad_Config(IR_LEARN_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
//                PAD_OUT_LOW);
// #endif
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
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
    if (IRDA_SUCCEED != irda_nec_encode((uint8_t)IR_SEND_ADDR, gIrTable[key_value], &gIrBuf))
    {
        APP_PRINT_INFO0("[IR] IR local data encode failed!");
        return false;
    }
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
    if (IRDA_SUCCEED != irda_nec_encode((uint8_t)IR_SEND_ADDR, gIrTable[key_value], &gIrBuf))
    {
        APP_PRINT_INFO0("[IR] IR local data encode failed!");
        return false;
    }
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
    if (IRDA_SUCCEED != irda_nec_encode((uint8_t)IR_SEND_ADDR, gIrTable[key_value], &gIrBuf))
    {
        APP_PRINT_INFO0("[IR] IR local data encode failed!");
        return false;
    }
#endif
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
    
    APP_PRINT_INFO2("[IR] ret1 =%d, ret2 = %d",ret1,ret2);
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
        APP_PRINT_INFO1("[IR] IR Send Learn data, ADDR: %d.", (IR_WAVE_DATA_BASE_ADDR + gIrSendStruct.irLearnDataOffset * IR_LEARN_WAVE_MAX_SIZE * 4));
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
    // for(uint8_t i = 0;i < IR_SEND_WAVE_MAX_SIZE;i++){
    //     APP_PRINT_INFO2("[ir_send_start] sendbuf[%d]: %x.",i,sendbuf[i]);
    // }
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

#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
            APP_PRINT_INFO0("[IR] IR send repeat code.");
            IR_SendBuf(gIrRepeatCodeStruct.irBuf, gIrRepeatCodeStruct.bufLen, DISABLE);
            gIrSendStruct.irSendStage = IR_SEND_REPEAT_CODE;
            gIrSendStruct.isIrSendComplete = IR_SEND_NOT_COMPLETED;

            IR_INTConfig(IR_INT_TF_LEVEL, ENABLE);
            /*enable RTC*/
            rtc_driver_set_comp_0_params(IR_REPEAT_CODE_PERIOD, ir_send_repeat_code_handler);

            ret = true;   
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
            APP_PRINT_INFO0("[IR] IR send repeat code.");
            IR_SendBuf(gIrRepeatCodeStruct.irBuf, gIrRepeatCodeStruct.bufLen, DISABLE);
            gIrSendStruct.irSendStage = IR_SEND_REPEAT_CODE;
            gIrSendStruct.isIrSendComplete = IR_SEND_NOT_COMPLETED;

            IR_INTConfig(IR_INT_TF_LEVEL, ENABLE);
            /*enable RTC*/
            rtc_driver_set_comp_0_params(IR_REPEAT_CODE_PERIOD, ir_send_repeat_code_handler);

            ret = true;   
#endif 
        }
    }
    else if (msg_sub_type == IO_MSG_TYPE_IR_SEND_COMPLETE)
    {
        if (gIrSendStruct.irSendKeyState == IR_SEND_KEY_RELEASE)
        {
            ir_send_exit();
            return ret;
        }
        if(gIrSendStruct.irSendkeyData == MM_VolumeIncrement || gIrSendStruct.irSendkeyData == MM_VolumeDecrement) {
            if(smartrcu_global_data.send_smartrcu_ir_state == 1 && (true == is_ir_learn_key_valid(gIrSendStruct.irSendkeyData))){
                APP_PRINT_INFO0("[IR] IR send cyclic code");
                os_delay(80);
                if (false == ir_send_module_init(gIrSendStruct.irSendkeyData))
                {
                    ret = false;
                    return ret;
                }
                if (true == ir_send_start())
                {
                    gIrSendStruct.irSendKeyState = IR_SEND_KEY_PRESS;
                    gIrSendStruct.irSendStage = IR_SEND_CAMMAND;
                    gIrSendStruct.isIrSendComplete = IR_SEND_NOT_COMPLETED;
                }  
            }
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

    // APP_PRINT_INFO0("[IR] IR Handler");

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
    APP_PRINT_INFO0("[IR] ir_send_exit.");
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
        gIrSendStruct.irSendkeyData = key_index_1;
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
    APP_PRINT_INFO0("[IR] ir_send_key_release_handle.");
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

