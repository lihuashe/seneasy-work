/******************************************************************************
 * @file     app_custom.c
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/
#include "../tl_common.h"
#include "../drivers.h"
#include "../app_config.h"
#include "app_custom.h"
#include "app_custom_lib.h"
#include "stack/ble/ble.h"
#include "app_test.h"
#include "../app_find_me/app_fms.h"
#include "../key_module/key_driver.h"
#include "../profile_module/dis_profile.h"
#include "app_rcu.h"

const u8 *pREMOTE_G10 = (u8 *)("RemoteG10");
const u8 *pREMOTE_G20 = (u8 *)("RemoteG20");

extern u8 *p_kb_map_ble;
extern u8 *p_kb_map_ir;
extern u8 *p_kb_map_normal;
extern  _attribute_data_retention_ u8 my_FWRevtrs [];
extern void kb_p_map_init(void);
extern u8 device_in_connection_state;


typedef enum
{
    APP_CUSTOM_DATA_UPDATE_PREPARE=1,
    APP_CUSTOM_DATA_UPDATE_PREPARE_END,
    APP_CUSTOM_DATA_UPDATE_ERASE,
    APP_CUSTOM_DATA_UPDATE_START,
    APP_CUSTOM_DATA_UPDATE_WRITEFAIL,
    APP_CUSTOM_DATA_UPDATE_ERASE_SUCC,
    APP_CUSTOM_DATA_UPDATE_END
}app_custom_data_update_sts;


typedef struct{
    u8 vid_src;
    u16 vid;
    u16 pid;
    u16 prod_ver;
}app_pnpid_format_t;

#define APP_CUSTOM_CMD_DATA_UPDATE_PREPARE      0xED00
#define APP_CUSTOM_CMD_DATA_UPDATE_START        0xED01
#define APP_CUSTOM_CMD_DATA_UPDATE_END          0xED02

#define LAYOUT_MASK_BIT0               0X01
#define LAYOUT_MASK_BIT1               0X02
#define LAYOUT_MASK_BIT2               0X04
#define LAYOUT_MASK_BIT5               0X20
#define LAYOUT_MASK_BIT6               0X40

_attribute_data_retention_  u8 en_powerkey_cache=1;
_attribute_data_retention_  u8 en_google_wakeuppack=1;
_attribute_data_retention_  u8 en_ble_adv=1;
_attribute_data_retention_  u8 en_slave_rpa=0;
#if !CLOSE_CODE_RAM
_attribute_data_retention_  u8 wakeup_key1[APP_CUSTOM_WAKEUPKEY_NUM];
_attribute_data_retention_  u8 wakeup_key2[APP_CUSTOM_WAKEUPKEY_NUM];
_attribute_data_retention_  u8 comb_key_keyid[9]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};  //back,home,center,mute,setting,guide,subtitle,down
_attribute_data_retention_  u8 nec_ir_addr1_table[48]={0};
_attribute_data_retention_  u8 nec_ir_addr2_table[48]={0};
_attribute_data_retention_  u8 nec_ir_cmd1_table[48]={0};
_attribute_data_retention_  u8 nec_ir_cmd2_table[48]={0};
_attribute_data_retention_  u8 key_map_ble[48]={0};
#endif
extern _tCustom_Data _custom_data;


_attribute_data_retention_ u8 app_custom_data_update_over_ota_flash_addr_pos=0;
_attribute_data_retention_ u8 app_custom_data_update_over_ota_flash_page_pos=0;


/***************************************************************************************************************

 *** If Kb drive pin number and scan pin number changed, it want to changed below in keyboard.c :            ***

 *** 1) macro
       #define KB_DRIVE_PIN_MAX_NUM   KB_DRIVE_PIN_G20
       #define KB_DRIVE_PIN_G10       3
       #define KB_DRIVE_PIN_G20       6
       #define KB_SCAN_PIN            8

 *** 2) The arrays a[][],b[][] in the kb_remap_key_row()
     u8 a[8][3]={{0},{0},{0},{0},{0},{0},{0},{0}};
     u8 b[8][6]={{0},{0},{0},{0},{0},{0},{0},{0}};

 *** 3) The arrays a[][],b[][] in the kb_scan_row()
     u8 a[8][3]={{0},{0},{0},{0},{0},{0},{0},{0}};
     u8 b[8][6]={{0},{0},{0},{0},{0},{0},{0},{0}};

 ****************************************************************************************************************/

 /***************************************************************************************************************

 *** If Kb drive pins and scan pins changed, and want to modify keymap    :           ***
 *** 1) Change the pin setting(drive pin and scan pin) contained in app_config.h      ***
 *** 2) The following three points also need to be changed synchronously              ***

 ****************************************************************************************************************/

/**
 * @brief       Device layout configuration
 * @param[in]   ui_layout: from flash 0x78000
 * @return      none
 * See document "Reference RCU Flash Storage Format" (Table 3.1) for details.
 */
void app_custom_ui_layout(u8 ui_layout)
{
#if 0
    if((ui_layout == 0xff) || (ui_layout == 0))
    {
        return;
    }
    if(_custom_data.device_type == REMOTE_G10)
    {
        if((ui_layout&LAYOUT_MASK_BIT0))
        {
            key_map_ble[Kb_Map_devicelayout_Correspend_Index_RCU[0]] = CR_NOTIFICATION;
            nec_ir_cmd1_table[Kb_Map_devicelayout_Correspend_Index_RCU[0]] = IR_NOTIFICATION;
        }
        if((ui_layout&LAYOUT_MASK_BIT1))
        {

            key_map_ble[Kb_Map_devicelayout_Correspend_Index_RCU[1]] = CR_LIVETV;
            nec_ir_cmd1_table[Kb_Map_devicelayout_Correspend_Index_RCU[1]] = IR_LIVETV;
        }
        if((ui_layout&0x18) == 0x08)
        {
            key_map_ble[Kb_Map_devicelayout_Correspend_Index_RCU[2]] = CR_USERPROFILE;
            nec_ir_cmd1_table[Kb_Map_devicelayout_Correspend_Index_RCU[2]] = IR_USERPROFILE;
        }
        else if((ui_layout&0x18) == 0x10)
        {
            key_map_ble[Kb_Map_devicelayout_Correspend_Index_RCU[2]] = CR_ALLAPPS;
            nec_ir_cmd1_table[Kb_Map_devicelayout_Correspend_Index_RCU[2]] = IR_ALLAPPS;
        }
    }
#endif
}

/**
 * @brief       load nec ir table
 * @param[in]   none
 * @return      none
 */
void app_custom_set_new_ir_table(void)
{
#if !CLOSE_CODE_RAM
    u8 dat[192];
    u8 g10_table[96];
    u8 i,j;

    if(_custom_data.device_type == REMOTE_G10)
    {
        flash_read_page(APP_NEC_IR_CODE_TABLE, 48*4, dat);
        //line up g10,g20
        for(i=0;i<8;i++)
        {
            for(j=0;j<3;j++)
            {
                g10_table[i*12+4*j]   = dat[i*24+4*j];
                g10_table[i*12+4*j+1] = dat[i*24+4*j+1];
                g10_table[i*12+4*j+2] = dat[i*24+4*j+2];
                g10_table[i*12+4*j+3] = dat[i*24+4*j+3];
            }
        }
        memcpy(dat,g10_table,24*4);
        for(i=0;i<24;i++)
        {
            nec_ir_addr1_table[i] = dat[i*4];
            nec_ir_addr2_table[i] = dat[i*4+1];
            if(dat[i*4+2] != 0xff)
            {
                nec_ir_cmd1_table[i]  = dat[i*4+2];
            }
            nec_ir_cmd2_table[i]  = dat[i*4+3];
        }
    }
#endif
}

/**
 * @brief       app_custom_param_init
 * @param[in]   device_layout: from flash 0x78000
 * @return      none
 */
void app_custom_param_init(u8 device_layout)
{
    if(_custom_data.device_type == REMOTE_G10)
    {
#if !CLOSE_CODE_RAM
        p_kb_map_ble =  key_map_ble;
        memcpy(key_map_ble,(u8 *)Kb_Map_Ble_RCU,sizeof(Kb_Map_Ble_RCU));
        p_kb_map_ir = (u8 *)nec_ir_cmd1_table;
#endif
#if (PCB_VER == PCB_SRC_4526)
        memcpy(p_kb_map_ir,(u8 *)Kb_Map_Ir_RCU,sizeof(Kb_Map_Ir_RCU));
#elif (PCB_VER == PCB_SRC_3215)
        if (strcmp((char*)rcu_project_name,"EU01") == 0)
            memcpy(p_kb_map_ir,(u8 *)Kb_Map_Ir_RCU_EU01,sizeof(Kb_Map_Ir_RCU_EU01));
        else if (strcmp((char*)rcu_project_name,"LM01") == 0)
            memcpy(p_kb_map_ir,(u8 *)Kb_Map_Ir_RCU_LM01,sizeof(Kb_Map_Ir_RCU_LM01));
#endif
        p_kb_map_normal = (u8 *)Kb_Map_Normal_RCU;
    }
    //app_custom_ui_layout(device_layout);
    app_custom_set_new_ir_table();
    //kb_p_map_init();
    kb_driver_map_init();
}

/**
 * @brief       load wakeup key by mask
 * @param[in]   wakeup_mask1,wakeup_mask2
 * @return      none
 * See document "Reference RCU Flash Storage Format" (Table 3.2) for details.
 */
void app_custom_wakeup_key_load(u8* wakeup_mask1, u8* wakeup_mask2)
{
#if !CLOSE_CODE_RAM
    //printf("%s \n ", __FUNCTION__);
    u8 j=0,i;
    u8 wakeupkey_num=0;
    u8 *p_kb_map;

    u16 mask1 = (u16)(((u16)wakeup_mask1[0]<<8) | ((u16)wakeup_mask1[1]));
    u16 mask2 = (u16)(((u16)wakeup_mask2[0]<<8) | ((u16)wakeup_mask2[1]));

    if((mask1 != 0xffff) && (mask2 != 0xffff) && ((mask1 & mask2) == 0))
    {
        if(_custom_data.device_type == REMOTE_G10)
        {
            wakeupkey_num = APP_CUSTOM_WAKEUPKEY_NUM-4;
            p_kb_map = (u8*)Kb_Wakeup_Key_Map_Correspend_Index_RCU;
        }

        for(i=0;i<wakeupkey_num;i++)
        {
            if(mask1&0x01)
            {
                wakeup_key1[j] = p_kb_map[i];
                j++;
            }
            mask1 = mask1>>1;
        }
        j=0;
        for(i=0;i<wakeupkey_num;i++)
        {
            if(mask2&0x01)
            {
                wakeup_key2[j] = p_kb_map[i];
                j++;
            }
            mask2 = mask2>>1;
        }

        //printf("wakeup key1:");
        //array_printf(wakeup_key1, wakeupkey_num);
        //printf("wakeup key2:");
        //array_printf(wakeup_key2, wakeupkey_num);
    }
#endif
}

/**
 * @brief       update manufacturer data by id
 * @param[in]   id (See document "Reference RCU Flash Storage Format" (Table 3.1) for details)
 * @return      none
 */
void app_custom_data_update_by_id(u8 id, u8* data, u8 length)
{
    switch(id)
    {
        case 1:  //device type
            if((data[0] == REMOTE_G10) || (data[0] == REMOTE_G20))
            {
                _custom_data.device_type = data[0];
            }
            break;

        case 2:  //vendor source
            _custom_data.pnp_id[0] = data[0];
            break;

        case 3:  //vid
            _custom_data.pnp_id[1] = data[0];
            _custom_data.pnp_id[2] = data[1];
            break;

        case 4:  //pid
            _custom_data.pnp_id[3] = data[0];
            _custom_data.pnp_id[4] = data[1];
            break;

        case 5:  //product version
            _custom_data.pnp_id[5] = data[0];
            _custom_data.pnp_id[6] = data[1];
            break;

        case 6:  //device name length
            _custom_data.device_name_length = data[0];
            if(_custom_data.device_name_length>16)
                _custom_data.device_name_length = 16;
            break;

        case 7:  //device name
            memcpy(_custom_data.device_name,data,_custom_data.device_name_length);

            break;

        case 8:  //device ui layout
            _custom_data.device_ui_layout = data[0];
            break;

        case 9:  //wakeup key 1
            _custom_data.wakeup_key_1[0] = data[0];
            _custom_data.wakeup_key_1[1] = data[1];
            break;

        case 10: //wakeup key 2
            _custom_data.wakeup_key_2[0] = data[0];
            _custom_data.wakeup_key_2[1] = data[1];
            break;

        case 11: //wakeup packet custom format
            memcpy(_custom_data.wakeup_packet,data,length);
            break;

        case 31: //enable or disable slave rpa function
            en_slave_rpa = (0 == data[0])?1:0;
            break;

        case 32: //enable/disable ble adv
            en_ble_adv = data[0];
            break;

        case 33: //wakeup packet sending mode
            en_google_wakeuppack = data[0];
            break;

        case 34: //cache power switch
            en_powerkey_cache = data[0];
            break;
    }
}

/**
 * @brief       update manufacturer data
 * @param[in]   none
 * @return      none
 * See document "Reference RCU Flash Storage Format" (Chapter 6) for details.
 */
void app_custom_data_update(void)
{
    u8 buff[256];
    u16 i;
    u8 length;
    u8 id=0;
    u16 boundlen;
    u8 boundbuff[40];
    u8 flag=0,start;

    flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR, 256, buff);
    if(buff[0] != 0xa5)
        return ;
    for(u8 k=0;k<16;k++)
    {
        if(k)
            flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR+k*256, 256, buff);
        if(flag)
        {
            flag = 0;
            app_custom_data_update_over_ota_flash_page_pos = k;
            start = app_custom_data_update_over_ota_flash_addr_pos;
        }
        else
        {
            if(buff[0] != 0xa5)
                break;
            app_custom_data_update_over_ota_flash_addr_pos = 0;
            start = 0;
        }
        if(buff[start] != 0xa5)
            break;
        for(i=start;i<=255;i++)
        {
            if(buff[i] == 0xa5)
            {
                if(i == 255)
                {
                    flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR+(k+1)*256, 1, &length);
                    flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR+(k+1)*256+1, 1, &id);
                    flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR+(k+1)*256+2, length, boundbuff);
                    app_custom_data_update_by_id(id,boundbuff,length);
                    app_custom_data_update_over_ota_flash_addr_pos = length+2;
                    flag = 1;
                }
                else if(i == 254)
                {
                    length = buff[i+1];
                    flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR+(k+1)*256, 1, &id);
                    boundlen = i+length+2;
                    flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR+(k+1)*256+1, length, boundbuff);
                    app_custom_data_update_by_id(id,boundbuff,length);
                    app_custom_data_update_over_ota_flash_addr_pos = length+1;
                    flag = 1;
                }
                else
                {
                    length = buff[i+1];
                    id = buff[i+2];
                    boundlen = i+length+2;
                    if(boundlen>255)
                    {
                        boundlen -= 255;
                        memcpy(boundbuff,&buff[i+3],length-boundlen);
                        flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR+(k+1)*256, boundlen, &boundbuff[length-boundlen]);
                        app_custom_data_update_by_id(id,boundbuff,length);
                        app_custom_data_update_over_ota_flash_addr_pos = boundlen;
                        flag = 1;
                    }
                    else if(boundlen == 255)
                    {
                        app_custom_data_update_by_id(id,&buff[i+3],length);
                        app_custom_data_update_over_ota_flash_addr_pos = 0;
                        flag = 1;
                    }
                    else
                    {
                        app_custom_data_update_by_id(id,&buff[i+3],length);
                        app_custom_data_update_over_ota_flash_addr_pos += (3+length);
                    }
                }

                i += length+2;
            }
            else
            {
                break;
            }
        }
        app_custom_data_update_over_ota_flash_page_pos = k;
        //printf("duo_flash_addr_pos =%x,duo_flash_page_pos =%x",app_custom_data_update_over_ota_flash_addr_pos,app_custom_data_update_over_ota_flash_page_pos);
    }
#if (!CLOSE_CODE_PRINTF)
    printf("duo_flash_addr_pos =%x,duo_flash_page_pos =%x",app_custom_data_update_over_ota_flash_addr_pos,app_custom_data_update_over_ota_flash_page_pos);
#endif
}

/**
 * @brief       app_custom_init
 * @param[in]   none
 * @return      none
 * See document "Reference RCU Flash Storage Format" and "Reference RCU Wake up Service"for details.
 */
void app_custom_init(void)
{
    u8 buffer[80];
    u8 addr_pos=0;
    u8 *pREMOTE_B046 = my_deviceName;//(u8 *)("RemoteB046");
    u8 my_PnPtrs []       = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};

    flash_read_page(APP_CUSTOM_ADDR, 80, buffer);
#if !CLOSE_CODE_RAM
    for(u8 i=0;i<APP_CUSTOM_WAKEUPKEY_NUM;i++)
    {
        wakeup_key1[i] = 0xff;
        wakeup_key2[i] = 0xff;
    }
#endif
    _custom_data.device_type = buffer[0];
    _custom_data.wakeup_packet[0] = 0xff;
    if((_custom_data.device_type == 0xff) || ((_custom_data.device_type != REMOTE_G10) && (_custom_data.device_type != REMOTE_G20)))
    {
        //printf("default custom info\r\n");
        _custom_data.device_type = REMOTE_G10;
        _custom_data.device_name_length = sizeof(my_deviceName);//10;
        memcpy(_custom_data.device_name,pREMOTE_B046,sizeof(my_deviceName));
        memcpy(_custom_data.pnp_id,my_PnPtrs,7);
        _custom_data.device_ui_layout= 0;
        app_custom_data_update();
        app_custom_wakeup_key_load(_custom_data.wakeup_key_1,_custom_data.wakeup_key_2);

        u8 my_woble_adv_type = ADV_TYPE_NONCONNECTABLE_UNDIRECTED;
        flash_read_page(APP_WOBLE_ADV_TYPE_ADDR, 1, &my_woble_adv_type);
        my_woble_adv_type = (0xff == my_woble_adv_type)?ADV_TYPE_CONNECTABLE_UNDIRECTED:ADV_TYPE_NONCONNECTABLE_UNDIRECTED;
        app_woble_init_settings(_custom_data.wakeup_packet, my_woble_adv_type);

        //my_FWRevtrs[3] = (_custom_data.device_type) + 0x30;
        //my_FWRevtrs[4] = (_custom_data.device_ui_layout) + 0x30;
        app_custom_param_init(_custom_data.device_ui_layout);

        return;
    }

    memcpy(_custom_data.pnp_id,&buffer[1],7);

    _custom_data.device_name_length = buffer[8];

    if(_custom_data.device_name_length>16)
        memcpy(_custom_data.device_name,&buffer[9],16);
    else
        memcpy(_custom_data.device_name,&buffer[9],_custom_data.device_name_length);

    addr_pos = 9 + _custom_data.device_name_length;
    _custom_data.device_ui_layout = buffer[addr_pos];
    addr_pos = addr_pos+1;

    //wakeup key
    _custom_data.wakeup_key_1[0] = buffer[addr_pos];
    _custom_data.wakeup_key_1[1] = buffer[addr_pos+1];
    _custom_data.wakeup_key_2[0] = buffer[addr_pos+2];
    _custom_data.wakeup_key_2[1] = buffer[addr_pos+3];

    u8 wakeup_key_pos = addr_pos + 4;
    if(buffer[wakeup_key_pos] != 0xff)
        memcpy(_custom_data.wakeup_packet,&buffer[wakeup_key_pos],buffer[wakeup_key_pos]);

    if(_custom_data.device_name_length>16)
        _custom_data.device_name_length = 16;


    flash_read_page(APP_EN_CACHEKEY, 1, &en_powerkey_cache);
    flash_read_page(APP_EN_GOOGLE_WAKEUPPACK, 1, &en_google_wakeuppack);
    flash_read_page(APP_EN_BLE_ADV, 1, &en_ble_adv);

    u8 read_data = 0xff;
    flash_read_page(APP_EN_SLAVE_RPA, 1, &read_data);
    en_slave_rpa = (0 == read_data)?1:0;
#if FIND_ME_ENABLE
    u8 my_en_periodic_wakeup = 0;
    flash_read_page(APP_EN_PERIODIC_WAKE_UP, 1, &my_en_periodic_wakeup);
    g_p_app_fms_ctrl->en_fms = (0==my_en_periodic_wakeup)?1:0;

    if(1 == g_p_app_fms_ctrl->en_fms){
        u16 read_data = 0;
         flash_read_page(APP_PERIODIC_WAKE_UP_TIMER, 2, (u8 *)&read_data);

         if((0 == read_data) || (0xffff == read_data)){
             /* 220707. 0 or 0xffff to disable periodic wakeup */
             g_p_app_fms_ctrl->en_periodic_wakeup = 0;
         }else if(read_data<=1440){
             /* 220707. The valid range is changed to 1~1440(min) */
             g_p_app_fms_ctrl->periodic_wakeup_interval = read_data;
             g_p_app_fms_ctrl->en_periodic_wakeup = 1;
         }else{
            /* If the interval data in the flash is wrong, use 30 */
             g_p_app_fms_ctrl->periodic_wakeup_interval = 30;
             g_p_app_fms_ctrl->en_periodic_wakeup = 1;
         }
    }
#endif
    app_custom_data_update();
    //my_FWRevtrs[3] = (_custom_data.device_type) + 0x30;
    //my_FWRevtrs[4] = (_custom_data.device_ui_layout) + 0x30;
    app_custom_wakeup_key_load(_custom_data.wakeup_key_1,_custom_data.wakeup_key_2);

    u8 my_woble_adv_type = ADV_TYPE_CONNECTABLE_UNDIRECTED;
    flash_read_page(APP_WOBLE_ADV_TYPE_ADDR, 1, &my_woble_adv_type);
    my_woble_adv_type = (0xff == my_woble_adv_type)?ADV_TYPE_CONNECTABLE_UNDIRECTED:ADV_TYPE_NONCONNECTABLE_UNDIRECTED;
    app_woble_init_settings(_custom_data.wakeup_packet, my_woble_adv_type);

    app_custom_param_init(_custom_data.device_ui_layout);


    /* 230214 by ken. read TX power settings */
    extern void app_rcu_set_TxPower(u8 Index);

    app_pnpid_format_t *p_my_pnpid = (app_pnpid_format_t *)_custom_data.pnp_id;

    if((0x01 == p_my_pnpid->vid_src) && (0x0957 == p_my_pnpid->vid) && (0x0005 == p_my_pnpid->pid)){
        /* Customized settings for special customers */
        app_rcu_set_TxPower(RF_POWER_P7p37dBm);
    }else{
        u8 my_TxPoewr = 0x00;
        flash_read_page(APP_TX_POWER_ADDR, 1 , &my_TxPoewr);
        if(0xff != my_TxPoewr){
            app_rcu_set_TxPower(my_TxPoewr);
        }
    }

    app_rcu_set_TxPower(MY_RF_POWER_INDEX);

    /* 230220 by ken. read couch off settings */
    u8 my_CouchMode = 0x00;
    flash_read_page(APP_COUCH_MODE_ADDR, 1 , &my_CouchMode);
    extern u8 app_rcu_CouchMode;
    app_rcu_CouchMode = (1==my_CouchMode)?1:0;


}

/**
 * @brief       Get device type
 * @param[in]   none
 * @return      1:G10 2:g20
 */
u8 app_custom_get_device_type(void)
{
    return _custom_data.device_type;
}

/**
 * @brief       Determine whether the button is a wakeup key
 * @param[in]   keyid
 * @return      1:yes 0:no
 */
u8 app_custom_is_wakeup_key(u8 keyid)
{
#if !CLOSE_CODE_RAM
    u8 i;

    for(i=0;i<APP_CUSTOM_WAKEUPKEY_NUM;i++)
    {
        if((keyid == wakeup_key1[i]) || (keyid == wakeup_key2[i]))
        {
            printf("wakeup_key =%x\r\n",keyid);
            return 1;
        }
    }
#endif
    return 0;
}

/**
 * @brief       Determine whether enable wakeup key function
 * @param[in]   keyid
 * @return      1:yes 0:no
 */
u8 app_custom_is_enable_wakeup_key(void)
{
    u16 wakeupkey1_mask = (u16)(((u16) _custom_data.wakeup_key_1[0]<<8) | ((u16) _custom_data.wakeup_key_1[1]));
    u16 wakeupkey2_mask = (u16)(((u16) _custom_data.wakeup_key_2[0]<<8) | ((u16) _custom_data.wakeup_key_2[1]));

    if((0 == wakeupkey1_mask) && (0 == wakeupkey2_mask)){
        return 0;
    }

    if((wakeupkey1_mask != 0xffff) && (wakeupkey2_mask != 0xffff) && ((wakeupkey1_mask & wakeupkey2_mask) == 0))
        return 1;
    else
        return 0;
}

/**
 * @brief       Get KEYID
 * @param[in]   keyid
 * @return      1 or 2
 */
u8  app_custom_wakeupkey_packet_index(u8 keyid)
{
#if !CLOSE_CODE_RAM
    u8 i;

    for(i=0;i<APP_CUSTOM_WAKEUPKEY_NUM;i++)
    {
        if(keyid == wakeup_key1[i])
            return 1;
        else if(keyid == wakeup_key2[i])
            return 2;
    }
#endif
    return 0;
}


#if APP_DATA_UPDATE_OVER_BLE
_attribute_data_retention_ app_custom_data_update_sts app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_END;

/**
 * @brief       parse data update commands and packets
 * @param[in]   data and len
 * @return      none
 */
void app_custom_data_update_process(u8 *data,u16 len)
{
    u8 datasend[3];
    u16 crc=0;
    u32 flash_addr=0,pos=0;
    u8 readdata[16];
    u8 result=0;
    u16 cmd = data[0] | (data[1]<<8);

    if(cmd == APP_CUSTOM_CMD_DATA_UPDATE_PREPARE)
    {
        if(app_custom_data_update_state == APP_CUSTOM_DATA_UPDATE_WRITEFAIL)
        {
            app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_ERASE;
        }
        else
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("APP_CUSTOM_CMD_DATA_UPDATE_PREPARE \r\n");
            #endif
            app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_PREPARE;
        }
    }
    else if(cmd == APP_CUSTOM_CMD_DATA_UPDATE_START)
    {
        app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_START;
    }
    else if(cmd == APP_CUSTOM_CMD_DATA_UPDATE_END)
    {
        if(app_custom_data_update_state == APP_CUSTOM_DATA_UPDATE_START)
        {
            datasend[0] = 0xEC;
            datasend[1] = 2;
            datasend[2] = 0;
            bls_att_pushNotifyData(OTA_CMD_OUT_DP_H, datasend, sizeof(datasend));
            app_custom_init();
            app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_END;
        }
    }

    if(app_custom_data_update_state == APP_CUSTOM_DATA_UPDATE_START)
    {
        if(len == 20)
        {
            crc = (data[19]<<8) | data[18];
            pos = (data[0]) | (data[1]<<8);
            extern unsigned short crc16 (unsigned char *pD, int len);
            if(crc == crc16(data,18))
            {
                {
                    flash_addr = APP_DATA_UPDATE_OVER_OTA_ADDR +  pos*16 + app_custom_data_update_over_ota_flash_addr_pos + \
                                 app_custom_data_update_over_ota_flash_page_pos*256;
                    //printf("flash_addr=%x\r\n",flash_addr);
                    flash_write_page(flash_addr,16,&data[2]);
                    flash_read_page(flash_addr,16, readdata);
                    if(memcmp(&data[2],readdata,16))
                    {
                        #if (!CLOSE_CODE_PRINTF)
                        printf("crc error\r\n");
                        #endif
                        result = 1;
                    }
                }
            }
            else
            {
                result = 1;
            }
            if(result == 1)
            {
                datasend[0] = 0xEC;
                datasend[1] = 2;
                datasend[2] = 1;
                bls_att_pushNotifyData(OTA_CMD_OUT_DP_H, datasend, sizeof(datasend));
                app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_WRITEFAIL;   //data invalid, must erase
            }
        }
    }
}

/**
 * @brief       Erase data update area
 * @param[in]   none
 * @return      -1:already erase  0:not erase
 */
int app_custom_data_update_sector_erase_timeoutcb(void)
{
    extern u8 is_mic_enable(void);
    if(is_mic_enable() == 1)
        return 0;
    if(bls_ll_requestConnBrxEventDisable() > 120)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("app_custom_data_update_sector_erase_timeoutcb\r\n");
        #endif
        bls_ll_disableConnBrxEvent();
        flash_erase_sector(APP_DATA_UPDATE_OVER_OTA_ADDR);
        bls_ll_restoreConnBrxEvent();
        return -1;
    }
    return 0;
}

/**
 * @brief       loop interface for erase data update area
 * @param[in]   none
 * @return      none
 */
void app_custom_data_update_sector_erase_loop(void)
{
    u8 datasend[3];
    if(app_custom_data_update_state == APP_CUSTOM_DATA_UPDATE_PREPARE)
    {
        if(device_in_connection_state)
        {
            datasend[0] = 0XEC;
            datasend[1] = 0;
            datasend[2] = 0;
            bls_att_pushNotifyData(OTA_CMD_OUT_DP_H, datasend, sizeof(datasend));
        }
        app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_PREPARE_END;
    }
    else if(app_custom_data_update_state == APP_CUSTOM_DATA_UPDATE_WRITEFAIL)
    {
        if(app_custom_data_update_sector_erase_timeoutcb() == -1)
        {
            app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_ERASE_SUCC;
            app_custom_data_update_over_ota_flash_addr_pos = 0;
            app_custom_data_update_over_ota_flash_page_pos = 0;
        }
    }
    else if(app_custom_data_update_state == APP_CUSTOM_DATA_UPDATE_ERASE)
    {
        if(app_custom_data_update_sector_erase_timeoutcb() == -1)
        {
            app_custom_data_update_state = APP_CUSTOM_DATA_UPDATE_PREPARE;
            app_custom_data_update_over_ota_flash_addr_pos = 0;
            app_custom_data_update_over_ota_flash_page_pos = 0;
        }
    }
}
#endif

/**
 * @brief       This function writes the buffer's content to the flash.
 * @param[in]   addr - the start address of the area.
 * @param[in]   len  - the length(in byte) of content needs to write into the flash.
 * @param[in]   buf  - the start address of the content needs to write into.
 * @return      0: success, -1: not empty, -2: validation error
 */
static int _app_custom_flash_write(unsigned long addr, unsigned long len, unsigned char *buf)
{
    unsigned long i = 0;
    unsigned char data = 0;
    for (i = 0; i < len; i++)
    {
        flash_read_page(addr + i, 1, &data);
        if (data != 0xff)
            return -1;
    }

    flash_write_page(addr, len, buf);

    for (i = 0; i < len; i++)
    {
        flash_read_page(addr + i, 1, &data);
        if (*(buf + i) != data)
            return -2;
    }

    return 0;
}

/**
 * @brief       This function print the buffer's content.
 * @param[in]   addr - the start address of the area.
 * @param[in]   len  - the length(in byte) of content needs to print.
 * @return      none
 */
void _app_custom_flash_print(unsigned long addr, unsigned long len)
{
    unsigned long i = 0;
    unsigned char data = 0;

    printf("Flash(0x%x) = \r\n", addr);
    for (i = 0; i < len; i++)
    {
        flash_read_page(addr + i, 1, &data);
        if ((i + 1) % 16 == 0)
            printf("%02x\r\n", data);
        else if ((i + 1) % 16 == 1)
            printf("    %02x ", data);
        else
            printf("%02x ", data);
    }
}

/**
 * @brief       Write wake up settings
 * @param[in]   none
 * @return      0: success, -1: not initialized, -2: not empty, -3: validation error
 */
static int app_custom_write_wakeup_settings(void)
{
    u8 device_name_length = 0;
    u16 pos = 0;
    int res = 0;

    flash_read_page(APP_CUSTOM_ADDR + 8, 1, &device_name_length);
    if (device_name_length == 0xff)
        return -1;

#if 0   // For debugging purposes only
    printf("[app_custom_write_wakeup_settings] ");
    _app_custom_flash_print(APP_CUSTOM_ADDR, 256);
#endif

    // Write Wake-up Key
    u8 wakeup_key[4] = { 0x00, 0x04, 0x00, 0x20 };    // POWER, NETFLIX

    pos = 9 + device_name_length + 1;
    res = _app_custom_flash_write(APP_CUSTOM_ADDR + pos, sizeof(wakeup_key)/sizeof(wakeup_key[0]), wakeup_key);
    if (res != 0)
    {
        if (res == -1)
            printf("[app_custom_write_wakeup_settings] Wake-up Key area is not empty.\r\n");
        else if (res == -2)
            printf("[app_custom_write_wakeup_settings] Wake-up Key written value validation error.\r\n");

        return res - 1;
    }

#if 0   // Enable if custom format is applied
    // Write Wake-up Packet Custom Format
    u8 wakeup_packet[34] = {                            // Wake-up Packet Custom Format
        0x1e, 0x16, 0x1d, 0x17, 0x02, 0x01, 0x04, 0x03, 0x03, 0x12, 0x18, 0x03, 0x19, 0x80, 0x01, 0x12,
        0xff, 0xff, 0xff, 0x41, 0x6d, 0x6c, 0x6f, 0x67, 0x69, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00 };

    pos = 9 + device_name_length + 1 + 4;
    res = _app_custom_flash_write(APP_CUSTOM_ADDR + pos, sizeof(wakeup_packet)/sizeof(wakeup_packet[0]), wakeup_packet);
    if (res != 0)
    {
        if (res == -1)
            printf("[app_custom_write_wakeup_settings] Wake-up Packet area is not empty.\r\n");
        else if (res == -2)
            printf("[app_custom_write_wakeup_settings] Wake-up Packet written value validation error.\r\n");

        return res - 1;
    }
#endif

#if 0   // For debugging purposes only
    printf("[app_custom_write_wakeup_settings] ");
    _app_custom_flash_print(APP_CUSTOM_ADDR, 256);
#endif

    return 0;
}

/**
 * @brief       Write conditionally to custom flash area
 * @param[in]   none
 * @return      none
 */
void app_custom_write_conditional (void)
{
    // Condition - PnP ID
    app_pnpid_format_t condition_pnpids[] = {   // SEI models
        { 0x01, 0x0957, 0x0006, 0xFF },
        { 0x01, 0x0957, 0x0019, 0xFF },
        { 0x01, 0x0957, 0x000A, 0xFF },
        { 0x01, 0x0957, 0x001A, 0xFF }
    };

    // Device PnP ID
    app_pnpid_format_t my_pnpid = { 0x00, };
    flash_read_page(APP_CUSTOM_ADDR + 1, 7, (u8 *)&my_pnpid);

    // Check condition
    u8 condition_match = 0;
    for (u8 i = 0; i < sizeof(condition_pnpids)/sizeof(condition_pnpids[0]); i++) {
        if ((my_pnpid.vid_src == condition_pnpids[i].vid_src) &&
            (my_pnpid.vid == condition_pnpids[i].vid) &&
            (my_pnpid.pid == condition_pnpids[i].pid)) {
            condition_match = 1;
            break;
        }
    }
    if (condition_match == 0) {
        printf("[app_custom_data_write] Condition mismatch. vid_src: 0x%04X, vid: 0x%04X, pid: 0x%04X\r\n",
            my_pnpid.vid_src, my_pnpid.vid, my_pnpid.pid);
        return;
    }

    int res = app_custom_write_wakeup_settings();
    printf("[app_custom_data_write] Writing Wake-up Settings : %s(%d) !!\r\n", (res == 0) ? "Completed" : "Stopped", res);
}

