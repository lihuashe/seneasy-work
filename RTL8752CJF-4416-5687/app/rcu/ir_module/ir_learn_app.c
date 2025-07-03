/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file         ir_learn_app.c
* @brief        This file provides application layer application code for how to use IR learin driver.
* @details
* @author       elliot chen
* @date         2017-11-16
* @version      v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "app_section.h"
#include "ir_learn.h"
#include "app_msg.h"
#include "trace.h"
#include "key_handle.h"
#include "swtimer.h"
#include "ftl.h"
#include "crc16btx.h"
#include "app_msg.h"
#include "trace.h"
#include "os_timer.h"
#include "rcu_application.h"
#include "smartrcu_handle.h"
#if IR_FUN
#include "ir_app_config.h"
#endif

#include "mem_config.h"
#include "led_driver.h"
#if IR_LEARN_MODE

/*============================================================================*
 *                          Local Variables
 *============================================================================*/
IR_LearnTypeDef IR_LearnPacket;
static IR_LearnStruct  gIrLearnData = {.isConfigUpdate = false};

//static bool gAllowEnterDLPS   = true;
//static bool gIsIRLearnWork    = false;

static IR_LearnStorgeInfo gIrLearnStorgeInfo[IR_LEARN_MAX_KEY_NUM] = {0};

TimerHandle_t ir_learn_timer = NULL;
/*============================================================================*
 *                         Local Functions Declaration
 *============================================================================*/
bool ir_learn_wave_storged(uint32_t key_value);
bool ir_learn_storge_config(void);
bool ir_learn_load_config(void);

void ir_learn_msg_form_isr(uint8_t isEnd);
void ir_learn_timer_callback(TimerHandle_t pxTimer);
void ir_learn_init_timer(void);
bool ir_learn_reset_ftl_learning_data(void);

/*============================================================================*
 *                         External Variables Declaration
 *============================================================================*/
extern T_APP_GLOBAL_DATA app_global_data;
/*============================================================================*
 *                         External Functions
 *============================================================================*/
bool app_send_msg_to_apptask(T_IO_MSG *p_msg);
#define ir_send_msg_to_app app_send_msg_to_apptask

/*============================================================================*
 *                         Local Functions
 *============================================================================*/

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void ir_learn_board_init(void)
{
    Pad_Config(IR_LEARN_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pinmux_Config(IR_LEARN_PIN, IRDA_RX);
}

/**
* @brief  Application code for IR learn module init.
* @param   No parameter.
* @return  void
*/
void ir_learn_module_init(void)
{
    // if (is_ir_send_working())
    // {
    //     ir_send_exit();
    // }

    // if (true == os_timer_start(&ir_learn_timer))
    // {
    //     APP_PRINT_INFO0("[IR] time out timer start success!");
    // }
    // else
    // {
    //     APP_PRINT_ERROR0("[IR] time out timer start failed!");
    // }

    // ir_learn_board_init();
    /* Initialize IR learn data structure */
    memset(&IR_LearnPacket, 0, sizeof(IR_LearnTypeDef));
    // DataTrans_RegisterIRLearnHandlerCB(ir_learn_msg_form_isr);
    // IR_Learn_Init();

    /*ir learn pin pull low*/
    // Pinmux_Deinit(IR_SEND_PIN);
    // Pad_Config(IR_SEND_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
    //            PAD_OUT_ENABLE, PAD_OUT_LOW);

    APP_PRINT_INFO1("[IR] IR_Learn_Module_Init, rcu state: %d.", app_global_data.rcu_status);
    // LED_BLINK(LED_1, LED_TYPE_BLINK_IR_LEARN_MODE, 0);

    // gIrLearnData.irLearnStage = IR_LEARN_INIT;
    // gIrLearnData.irLearnKey = IR_LEARN_KEY_RELEASE;

    ir_learn_load_config();

    // /*IR Module disable*/
    // IR_Cmd(IR_MODE_RX, DISABLE);
}

/**
 * @brief  ir learn timer init.
 *@param   void.
 * @return     void.
 * @note       none.
**/
void ir_learn_init_timer(void)
{
    APP_PRINT_INFO0("[IR] init IR Learn timer");
    /*ir_learn_timer is used for ir learn*/
    if (false == os_timer_create(&ir_learn_timer, "ir_learn_timer",  1, \
                                 IR_LEARN_TIMEOUT, false, ir_learn_timer_callback))
    {
        APP_PRINT_ERROR0("[IR] timer creat failed!");
    }
}

/**
 * @brief  ir learn timer callback.
 *@param   TimerHandle_t pxTimer.
 * @return     void.
 * @note       none.
**/
void ir_learn_timer_callback(TimerHandle_t pxTimer)
{
    APP_PRINT_INFO0("[IR] ir learn time out");
    ir_learn_exit();
}
/**
* @brief  Application code for IR learn data process.
* @param  IR sub type msg.
* @return  bool.
*          ture   specific msg can be handle;
*          false  ir msg can not be handled.
*/
bool ir_learn_msg_proc(uint16_t msg_sub_type)
{
    bool ret = false;
    if (msg_sub_type == IO_MSG_TYPE_IR_LEARN_DATA){
        T_IO_MSG bee_io_msg;
        bee_io_msg.type = IO_MSG_TYPE_IR;
        bee_io_msg.subtype = IO_MSG_TYPE_IR_LEARN_STOP;
        ir_send_msg_to_app(&bee_io_msg);
        APP_PRINT_INFO0("[ir_learn_msg_proc] IO_MSG_TYPE_IR_LEARN_DATA");
        ret = true;
    }
    else if (msg_sub_type == IO_MSG_TYPE_IR_LEARN_STOP){

        gIrLearnData.irLearnKey = smartrcu_ir_key_data.ir_key_index;
        IR_LearnPacket.freq = (float)smartrcu_ir_key_data.freq;
        IR_LearnPacket.buf_index = smartrcu_ir_key_data.ir_buff_index;
        memcpy(&IR_LearnPacket.ir_buf, smartrcu_ir_key_data.ir_buffer, sizeof(smartrcu_ir_key_data.ir_buffer));
        APP_PRINT_INFO1("[ir_learn_msg_proc] gIrLearnData.irLearnKey = %d",gIrLearnData.irLearnKey);
        /*pick up the last ir data*/
        IR_Learn_Decode(&IR_LearnPacket);
        /* Decode IR carrier freqency */
        // IR_Learn_Freq(&IR_LearnPacket);
        /* Data reduction */
        // IR_Learn_ConvertData(&IR_LearnPacket);
        SmartRcu_ir_Decode(&IR_LearnPacket);
        /*storge ir learn valid wave*/
        ir_learn_wave_storged(gIrLearnData.irLearnKey);
        /*storge ir learn config*/
        ir_learn_storge_config();

        // LED_BLINK(LED_1, LED_TYPE_BLINK_IR_LEARN_SUCCESS, 1);

        ir_learn_exit();
        APP_PRINT_INFO0("[ir_learn_msg_proc] IO_MSG_TYPE_IR_LEARN_STOP");
        APP_PRINT_INFO1("[IR] IR learn stop msg, fre: %d.", (uint32_t)(IR_LearnPacket.freq * 1000));
        ret = true;
    }
    else if (msg_sub_type == IO_MSG_TYPE_INVE_IR_LEARN_DATA){

        T_IO_MSG bee_io_msg;
        bee_io_msg.type = IO_MSG_TYPE_IR;
        bee_io_msg.subtype = IO_MSG_TYPE_INVE_IR_LEARN_STOP;
        ir_send_msg_to_app(&bee_io_msg);
        APP_PRINT_INFO0("[ir_learn_msg_proc] IO_MSG_TYPE_INVE_IR_LEARN_DATA");
        ret = true;
    }
    else if (msg_sub_type == IO_MSG_TYPE_INVE_IR_LEARN_STOP){

        gIrLearnData.irLearnKey = smartrcu_ir_key_data.inve_ir_key_index;
        IR_LearnPacket.freq = (float)smartrcu_ir_key_data.freq;
        IR_LearnPacket.buf_index = smartrcu_ir_key_data.inve_ir_buff_index;
        memcpy(&IR_LearnPacket.ir_buf, smartrcu_ir_key_data.inve_ir_buffrt, sizeof(smartrcu_ir_key_data.inve_ir_buffrt));
        APP_PRINT_INFO1("[ir_learn_msg_proc] gIrLearnData.irLearninveKey = %d",gIrLearnData.irLearnKey);
        /*pick up the last ir data*/
        IR_Learn_Decode(&IR_LearnPacket);
        /* Decode IR carrier freqency */
        // IR_Learn_Freq(&IR_LearnPacket);
        /* Data reduction */
        // IR_Learn_ConvertData(&IR_LearnPacket);
        SmartRcu_ir_Decode(&IR_LearnPacket);
        /*storge ir learn valid wave*/
        ir_learn_wave_storged(gIrLearnData.irLearnKey);
        /*storge ir learn config*/
        ir_learn_storge_config();

        // LED_BLINK(LED_1, LED_TYPE_BLINK_IR_LEARN_SUCCESS, 1);
        APP_PRINT_INFO0("[ir_learn_msg_proc] IO_MSG_TYPE_INVE_IR_LEARN_STOP");
        ir_learn_exit();
        APP_PRINT_INFO1("[IR] IR learn stop msg, fre: %d.", (uint32_t)(IR_LearnPacket.freq * 1000));
        ret = true;
    }
    return ret;
}
/**
* @brief  Application code for IR learn key press handle.
* @param  uint32_t key_index_1;
*         uint32_t key_index_2.
* @return  void
*/
bool ir_learn_press_handle(uint32_t key_index_1, uint32_t key_index_2, uint32_t key_length)
{
    if ((gIrLearnData.irLearnStage != IR_LEARN_INIT)
        && (gIrLearnData.irLearnStage != IR_LEARN_KEY_RELEASE))
    {
        APP_PRINT_INFO0("[IR] Err, not in ir learn mode.");
        return false;
    }
    if (false == is_ir_learn_key_valid(key_index_1))
    {
        APP_PRINT_INFO1("[IR] Err, this key(key index: 0x%x) is not ir learn key.", key_index_1);
        return false;
    }

    if (true == os_timer_restart(&ir_learn_timer, IR_LEARN_TIMEOUT))
    {
        APP_PRINT_INFO0("[IR] time out timer restart success!");
    }
    else
    {
        APP_PRINT_ERROR0("[IR] time out timer restart failed!");
    }

    gIrLearnData.irLearnKey = key_index_1;
    gIrLearnData.irLearnStage = IR_LEARN_KEY_PRESS;

    /** ir learn pin pull low.
    *   @note: real chip may not need this, fpga platform can not excute Pad_Config()
    *   when exit from dlps in app_exit_dlps_config function.
    */
    {
        Pinmux_Deinit(IR_SEND_PIN);
        Pad_Config(IR_SEND_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                   PAD_OUT_ENABLE, PAD_OUT_LOW);
    }

    LED_BLINK(LED_1, LED_TYPE_BLINK_IR_LEARN_WAITING, 0);

    APP_PRINT_INFO0("[IR] ir learn key press.");

    IR_Cmd(IR_MODE_RX, ENABLE);

    return true;
}


/**
* @brief  Application code for IR learn key release handle.
* @param  uint32_t key_index_1;
*         uint32_t key_index_2.
* @return  void
*/
bool ir_learn_key_release_handle(void)
{
    LED_BLINK_EXIT(LED_1, LED_TYPE_BLINK_IR_LEARN_WAITING);

    if (gIrLearnData.irLearnStage != IR_LEARN_KEY_PRESS)
    {
        //APP_PRINT_INFO0("[IR] ir learn in idle state, key release no action.");
        return false;
    }

    APP_PRINT_INFO0("[IR] ir learn key release.");

    gIrLearnData.irLearnStage = IR_LEARN_KEY_RELEASE;
    gIrLearnData.irLearnKey = INVALID_KEY;

    IR_Cmd(IR_MODE_RX, DISABLE);

    return true;

}

/**
* @brief  Application code for IR learn key release handle.
* @param  uint32_t key_index_1;
*         uint32_t key_index_2.
* @return  void
*/
void ir_learn_exit(void)
{
    APP_PRINT_INFO0("[IR] ir mode exit.");

    gIrLearnData.irLearnKey    = INVALID_KEY;
    gIrLearnData.irLearnStage  = IR_LEARN_IDLE;

    LED_BLINK_EXIT(LED_1, LED_TYPE_BLINK_IR_LEARN_MODE);

    /* Close IR learn */
    IR_Learn_DeInit();
}

/**
* @brief   Check ir learn if working.
* @param   void.
* @return  bool true   working;
*               false  not in working;
*/
bool is_ir_learn_working(void)
{
    if (gIrLearnData.irLearnStage == IR_LEARN_IDLE)
    {
        return false;
    }
    return true;
}
/**
* @brief   Check if ir learn allow enter dlps.
* @param   void.
* @return  bool true   working;
*               false  not in working;
*/
bool ir_learn_check_dlps(void)
{
    //return gAllowEnterDLPS;
    /*ir learn not in work, allow dlps*/
    if (gIrLearnData.irLearnStage == IR_LEARN_IDLE)
    {
        return true;
    }

    /*ir learn in work, key press not allow dlps*/
    if (gIrLearnData.irLearnStage == IR_LEARN_KEY_PRESS)
    {
        return false;
    }

    return true;
}

/**
* @brief   storge one ir learn key.
*          newest {[0]-[1]-----[IR_LEARN_MAX_KEY_NUM-1]} oldest.
* @param   uint32_t key_value.
* @return  bool true   success;
*               false  failed;
*/
/* bool IR_Learn_Storged(uint32_t key_value)
{
    uint32_t last_key = key_value;
    uint32_t current_key = key_value;
    //sort
    for (uint8_t index = 0; index < IR_LEARN_MAX_KEY_NUM; index ++)
    {
        current_key = gIrLearnStorgeInfo[index];
        gIrLearnStorgeInfo[index] = last_key;
        last_key = current_key;
        if (current_key == key_value)
        {
            break;
        }
    }
    //
    return true;
}*/

bool ir_learn_reset_ftl_learning_data(void)
{
    bool result = true;

    memset(gIrLearnStorgeInfo, 0, sizeof(gIrLearnStorgeInfo));
    /*save config */
    if (0 != ftl_save(gIrLearnStorgeInfo,
                      IR_WAVE_CONFIG_BASE_ADDR,
                      sizeof(gIrLearnStorgeInfo)))
    {
        APP_PRINT_INFO0("[ir_learn_reset_ftl_learning_data] fail");
        result = false;
    }
    else{
        APP_PRINT_INFO0("[ir_learn_reset_ftl_learning_data] suc");
    }
    return result;
}

bool ir_learn_wave_storged(uint32_t key_value)
{
    uint32_t ret = true;
    IR_LearnStorgeInfo last_info = {.key_value = key_value, 0};
    IR_LearnStorgeInfo current_info = {.key_value = key_value, 0};

    if (key_value == INVALID_KEY)
    {
        return false;
    }

    /*sort*/
    for (uint8_t index = 0; index < IR_LEARN_MAX_KEY_NUM; index ++)
    {
        current_info.key_value = gIrLearnStorgeInfo[index].key_value;
        current_info.offset = gIrLearnStorgeInfo[index].offset;
        current_info.frequency = gIrLearnStorgeInfo[index].frequency;

        gIrLearnStorgeInfo[index].key_value = last_info.key_value;
        gIrLearnStorgeInfo[index].offset = last_info.offset;
        gIrLearnStorgeInfo[index].frequency = last_info.frequency;

        last_info.key_value = current_info.key_value;
        last_info.offset = current_info.offset;
        last_info.frequency = current_info.frequency;

        if (gIrLearnStorgeInfo[index].isValid == false)
        {
            gIrLearnStorgeInfo[0].offset = index;
            gIrLearnStorgeInfo[index].isValid = true;
            break;
        }
        gIrLearnStorgeInfo[0].offset = current_info.offset;
        if (current_info.key_value == key_value)
        {
            break;
        }

    }
    gIrLearnStorgeInfo[0].frequency = IR_LearnPacket.freq;
    
    /*save*/
    ret = ftl_save(IR_LearnPacket.ir_buf,
                   IR_WAVE_DATA_BASE_ADDR + gIrLearnStorgeInfo[0].offset * IR_LEARN_WAVE_MAX_SIZE * 4,
                   IR_LEARN_WAVE_MAX_SIZE * 4
                  );
    APP_PRINT_INFO2("[IR] storge data flag: %d, 0 success, otherwise failed, offset: %d", ret,
                    gIrLearnStorgeInfo[0].offset);

    return true;
}

/**
* @brief   storge ir learn config.
* @param   void.
* @return  uint32_t  success;
*                    failed return INVALID_OFFSET;
*/
bool ir_learn_storge_config(void)
{
    uint32_t ret = 0;
    uint32_t ConfigCRCValue = 0;
    /*save config */
    ret = ftl_save(gIrLearnStorgeInfo,
                   IR_WAVE_CONFIG_BASE_ADDR,
                   sizeof(gIrLearnStorgeInfo)
                  );
    ConfigCRCValue = btxfcs(0, (uint8_t *)gIrLearnStorgeInfo, sizeof(gIrLearnStorgeInfo));

    /*save config crc value*/
    ret += ftl_save(&ConfigCRCValue,
                    IR_WAVE_CONFIG_BASE_ADDR + sizeof(gIrLearnStorgeInfo),
                    sizeof(uint32_t)
                   );
    APP_PRINT_INFO0("[IR] IR_LearnConfigStorge");
    return ret;
}
/**
* @brief   load ir learn config.
* @param   uint32_t void.
* @return  bool true update already or update success;
*               false not update;
*/
bool ir_learn_load_config(void)
{
    uint32_t ret = 0, ConfigCRCValue = 0;
    bool flag = true;
    // APP_PRINT_INFO1("[ir_learn_load_config] gIrLearnData.isConfigUpdate = %d",gIrLearnData.isConfigUpdate);
    if (gIrLearnData.isConfigUpdate == false)
    {
        ret = ftl_load(gIrLearnStorgeInfo, IR_WAVE_CONFIG_BASE_ADDR, sizeof(gIrLearnStorgeInfo));
        ret += ftl_load(&ConfigCRCValue, IR_WAVE_CONFIG_BASE_ADDR + sizeof(gIrLearnStorgeInfo),
                        sizeof(uint32_t));
        if (ret != 0)
        {
            flag = false;
        }
        if (ConfigCRCValue != btxfcs(0, (uint8_t *)gIrLearnStorgeInfo, sizeof(gIrLearnStorgeInfo)))
        {
            flag = false;
        }
        gIrLearnData.isConfigUpdate = true;

        for (uint8_t index  = 0; index < IR_LEARN_MAX_KEY_NUM; index ++)
        {
            // DBG_DIRECT("gIrLearnStorgeInfo[%d].isValid = %d,key_value = %d,offset = %d",index,gIrLearnStorgeInfo[index].isValid,gIrLearnStorgeInfo[index].key_value,gIrLearnStorgeInfo[index].offset);
            if (flag == false)
            {
                gIrLearnStorgeInfo[index].isValid   = false;
                gIrLearnStorgeInfo[index].frequency = INVALID_FREQUENCY;
                gIrLearnStorgeInfo[index].key_value = INVALID_KEY;
                gIrLearnStorgeInfo[index].offset    = INVALID_OFFSET;
                continue;
            }
            /*check current config if valid*/
            if (gIrLearnStorgeInfo[index].isValid ==  false)
            {
                flag = false;
            }
            /*handle invalid offset or invalid key value*/
            if ((gIrLearnStorgeInfo[index].offset > (IR_LEARN_MAX_KEY_NUM - 1))
                || (false == is_ir_learn_key_valid(gIrLearnStorgeInfo[index].key_value)))
            {
                flag = false;
            }
            // DBG_DIRECT("flag1 = %d",flag);
            /*check if has two same key value*/
            for (uint8_t loop = index + 1; loop < IR_LEARN_MAX_KEY_NUM; loop ++)
            {
                /*if flag is already false, there is no need to check follow*/
                if (flag == false)
                {
                    break;
                }
                // DBG_DIRECT("flag2 = %d",flag);

                // DBG_DIRECT("gIrLearnStorgeInfo[%d].key_value = %d",index,gIrLearnStorgeInfo[index].key_value);
                // DBG_DIRECT("gIrLearnStorgeInfoloop[%d].key_value = %d",loop,gIrLearnStorgeInfo[loop].key_value);
                // DBG_DIRECT("gIrLearnStorgeInfo[%d].offset = %d",index,gIrLearnStorgeInfo[index].offset);
                // DBG_DIRECT("gIrLearnStorgeInfoloop[%d].offset = %d",loop,gIrLearnStorgeInfo[loop].offset);

                if ((gIrLearnStorgeInfo[index].key_value == gIrLearnStorgeInfo[loop].key_value)
                    && ((gIrLearnStorgeInfo[index].offset == gIrLearnStorgeInfo[loop].offset)))
                {
                    flag = false;
                    break;
                }
            }
            // DBG_DIRECT("flag3 = %d",flag);
            if (flag == false)
            {
                //index = 0;
                gIrLearnStorgeInfo[index].isValid = false;
                gIrLearnStorgeInfo[index].frequency = INVALID_FREQUENCY;
                gIrLearnStorgeInfo[index].key_value = INVALID_KEY;
                gIrLearnStorgeInfo[index].offset    = INVALID_OFFSET;
            }
        }
    }
    return true;
}

/**
* @brief   get offset of specific key.
* @param   uint32_t key_value.
* @return  uint32_t  success;
*                    failed return INVALID_OFFSET;
*/
uint32_t ir_learn_get_key_offset(uint32_t key_value)
{
    for (uint8_t index = 0; index < IR_LEARN_MAX_KEY_NUM; index ++)
    {
        if ((gIrLearnStorgeInfo[index].key_value == key_value) && (gIrLearnStorgeInfo[index].isValid))
        {
            APP_PRINT_INFO2("gIrLearnStorgeInfo[%d].key_value = %d",index,gIrLearnStorgeInfo[index].key_value);
            APP_PRINT_INFO2("gIrLearnStorgeInfo[%d].offset = %d",index,gIrLearnStorgeInfo[index].offset);
            return gIrLearnStorgeInfo[index].offset;
        }
    }
    return INVALID_OFFSET;
}

/**
* @brief   get frequency of specific key ir learn.
* @param   uint32_t key_value.
* @return  uint32_t  success;
*                    failed return INVALID_FREQUENCY;
*/
float ir_learn_get_key_frequency(uint32_t key_value)
{
    for (uint8_t index = 0; index < IR_LEARN_MAX_KEY_NUM; index ++)
    {
        if ((gIrLearnStorgeInfo[index].key_value == key_value) && (gIrLearnStorgeInfo[index].isValid))
        {
            return gIrLearnStorgeInfo[index].frequency;
        }
    }
    return INVALID_FREQUENCY;
}

/**
* @brief   check ir learn key value if valid.
* @param   uint32_t key_value.
* @return  bool   valid   true;
*                 invalid false;
*/

bool is_ir_learn_key_valid(uint32_t key_value)
{
    T_KEY_INDEX_DEF key = (T_KEY_INDEX_DEF)key_value;
    DBG_DIRECT("is_ir_learn_key_valid key = %d",key);
    switch (key)
    {
    case  VK_NC               :
    case  MM_POWER            :
    case  MM_VolumeIncrement  :
    case  MM_VolumeDecrement  :
    case  MM_Mute             :
    case  MM_POWER_INCERSE    :
    case  MM_NETFLIX_INCERSE  :
    case  MM_YouTube_INCERSE  :
        return true;
    default:
        return false;

    }
}

/**
* @brief   ir learn msg handle from ir learn driver isr.
* @param   uint8_t isEnd .0 FALSE, 1 TRUE
* @return  bool   valid   true;
*                 invalid false;
*/
void ir_learn_msg_form_isr(uint8_t isEnd)
{
    //Add applcation code here which send msg from IR ISR to application layer to notify handling data
    T_IO_MSG bee_io_msg;
    bee_io_msg.type = IO_MSG_TYPE_IR;

    if (isEnd)
    {
        /*send ir learn data message to app*/
        bee_io_msg.subtype = IO_MSG_TYPE_IR_LEARN_STOP;
    }
    else
    {
        /*send ir learn data message to app*/
        bee_io_msg.subtype = IO_MSG_TYPE_IR_LEARN_DATA;
    }
    ir_send_msg_to_app(&bee_io_msg);
}
/**
* @brief   get ir learn attribute.
* @param   IR_ATTR irAttr ir atribute, void* attrValue attr value;
* @return  bool   true get success;
*                 false get failed;
*/
bool ir_learn_get_attr(IR_ATTR irAttr, void *attrValue, uint32_t key_index)
{
    if (is_ir_learn_working())
    {
        APP_PRINT_INFO0("[IR] Ir learn is working.");
        return false;
    }
    APP_PRINT_INFO0("[IR] ir_learn_get_attr");
    /*update IR config*/
    ir_learn_load_config();
    APP_PRINT_INFO1("[IR] irAttr = %d",irAttr);
    /*get specific attr value*/
    switch (irAttr)
    {
    case IR_ATTR_OFFSET:
        *(uint32_t *)attrValue = ir_learn_get_key_offset(key_index);
        break;
    case IR_ATTR_FREQ:
        {
            float *pf = (float *)attrValue;
            *pf  = ir_learn_get_key_frequency(key_index);
            if (*pf == INVALID_FREQUENCY)
            {
                return false;
            }
        }
        break;
    default:
        *(uint32_t *)attrValue = (uint32_t)INVALID_VALUE;
        break;
    }

    if ((INVALID_OFFSET == *(uint32_t *)attrValue)
        || (INVALID_VALUE == *(uint32_t *)attrValue))
    {
        return false;
    }

    return true;
}
// void smartrcu_ir_learn_mag(T_SMARTRCU_IR_KEY_DATA *p_smartrcu_ir_key_data)
// { 
//     T_SMARTRCU_IR_KEY_DATA smartrcu_ir_key_data;
//     memcpy(&smartrcu_ir_key_data, p_smartrcu_ir_key_data, sizeof(T_SMARTRCU_IR_KEY_DATA));
//     gIrLearnData.irLearnKey = smartrcu_ir_key_data.ir_key_index;
//     IR_LearnPacket.freq = (float)smartrcu_ir_key_data.freq;
//     IR_LearnPacket.buf_index = smartrcu_ir_key_data.ir_buff_index;
//     memcpy(&IR_LearnPacket.ir_buf, smartrcu_ir_key_data.ir_buffer, sizeof(smartrcu_ir_key_data.ir_buffer));  
//     // for(uint8_t i = 0;i <= IR_LearnPacket.buf_index;i ++){
//     //     APP_PRINT_INFO2("[smartrcu_ir_learn_mag] IR_LearnPacket.ir_buf[%d] = %d",i,IR_LearnPacket.ir_buf[i]);
//     // }
//     T_IO_MSG bee_io_msg;
//     bee_io_msg.type = IO_MSG_TYPE_IR;
//     bee_io_msg.subtype = IO_MSG_TYPE_IR_LEARN_STOP;
//     ir_send_msg_to_app(&bee_io_msg);
// }
void smartrcu_ir_learn_mag(uint8_t ir_learn_type)
{ 
    if(ir_learn_type == LEARN_DATA){
        T_IO_MSG bee_io_msg;
        bee_io_msg.type = IO_MSG_TYPE_IR;
        bee_io_msg.subtype = IO_MSG_TYPE_IR_LEARN_DATA;
        ir_send_msg_to_app(&bee_io_msg);
    }
    else if(ir_learn_type == INVE_LEARN_DATA){
        T_IO_MSG bee_io_msg;
        bee_io_msg.type = IO_MSG_TYPE_IR;
        bee_io_msg.subtype = IO_MSG_TYPE_INVE_IR_LEARN_DATA;
        ir_send_msg_to_app(&bee_io_msg);
    }
}

#endif /*end Micro IR_LEARN_MODE */



/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

