/******************************************************************************
 * @file     app_att.c
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

#include "tl_common.h"
#include "application/audio/audio_config.h"
#include "stack/ble/ble.h"
#include "app_audio.h"
#include "app_ui.h"
#include "../ota_module/app_ota.h"
#include "app_ir.h"
#include "app_custom.h"
#include "app_test.h"
#include "./app_find_me/app_fms.h"
#include "../../version.h"
#include "dis_profile.h"
#include "atv_profile.h"
#include "bas_profile.h"
#include "dis_profile.h"
#include "gap_profile.h"
#include "hid_profile.h"
#include "ota_profile.h"
#include "seneasy_profile.h"
#include "tvssu_profile.h"

static const u16 clientCharacterCfgUUID     = GATT_UUID_CLIENT_CHAR_CFG;
static const u16 my_primaryServiceUUID      = GATT_UUID_PRIMARY_SERVICE;
static const u16 my_characterUUID           = GATT_UUID_CHARACTER;
static const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;
static const u16 my_IEEE11073UUID           = CHARACTERISTIC_UUID_IEEE_11073_CERT_LIST;
static const u8 att_ManuNametrs [7] = {'S','e','n','e','a','s','y'};
static const attribute_t my_Attributes[];

#if 0
static const u16 userdesc_UUID    = GATT_UUID_CHAR_USER_DESC;
static const u8 my_OtaServiceUUID[16]    = TELINK_OTA_UUID_SERVICE;
static const u8  my_OtaName[]             = {'O', 'T', 'A'};
static const u8 my_OtaUUID_TL[16]           = {TELINK_SPP_DATA_OTA};
static u8 my_OtaDataTL                      = 0x00;
static const u8 my_OtaCharValTL[19] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
    U16_LO(OTA_CMD_OUT_DP_H_TL), U16_HI(OTA_CMD_OUT_DP_H_TL),
    TELINK_SPP_DATA_OTA, //UUID
};
#endif

void my_att_init (void)
{
#if (MP_TEST_MODE)
    if (test_get_mode() == MODE_TEST) bls_att_setAttributeTable((u8 *)my_test_demo_Attributes);
    else
#endif
    bls_att_setAttributeTable ((u8 *)my_Attributes);
}

static const attribute_t my_Attributes[] = {
#if (MP_TEST_MODE)
    {ATT_END_H - 1 - 4, 0,0,0,0,0},    // total num of attribute
#else
    {ATT_END_H - 1,     0,0,0,0,0},    // total num of attribute
#endif
    /*============================================================================*
    *                                gap_profile
    *============================================================================*/
    // gap
    {7, ATT_PERMISSIONS_READ,2, 2,(u8*)(&my_primaryServiceUUID),      (u8*)(&my_gapServiceUUID),       0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_devNameCharVal),            (u8*)(&my_characterUUID),       (u8*)(my_devNameCharVal),             0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(_custom_data.device_name),     (u8*)(&my_devNameUUID),         (u8*)(_custom_data.device_name),      0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_appearanceCharVal),         (u8*)(&my_characterUUID),       (u8*)(my_appearanceCharVal),          0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_appearance),                (u8*)(&my_appearanceUIID),      (u8*)(&my_appearance),                0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_periConnParamCharVal),      (u8*)(&my_characterUUID),       (u8*)(my_periConnParamCharVal),       0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_periConnParameters),        (u8*)(&my_periConnParamUUID),   (u8*)(&my_periConnParameters),        0},
    // gatt
    {4, ATT_PERMISSIONS_READ,2, 2,                                    (u8*)(&my_primaryServiceUUID),  (u8*)(&my_gattServiceUUID),           0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_serviceChangeCharVal),      (u8*)(&my_characterUUID),       (u8*)(my_serviceChangeCharVal),       0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(serviceChangeVal),             (u8*)(&serviceChangeUUID),      (u8*)(&serviceChangeVal),             0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(serviceChangeCCC),             (u8*)(&clientCharacterCfgUUID), (u8*)(serviceChangeCCC),              0},

    /*============================================================================*
    *                                dis_profile
    *============================================================================*/
    // device Information Service
    {13, ATT_PERMISSIONS_READ,2, 2,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_devServiceUUID),       0},
    // PnP ID
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_PnCharVal),                 (u8*)(&my_characterUUID),       (u8*)(my_PnCharVal),                  0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(_custom_data.pnp_id),          (u8*)(&my_PnPUUID),             (u8*)(_custom_data.pnp_id),           0},
    // manufacture name string
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_ManuNameCharVal),           (u8*)(&my_characterUUID),       (u8*)(my_ManuNameCharVal),            0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(att_ManuNametrs),              (u8*)(&my_ManuNameUUID),        (u8*)(att_ManuNametrs),               0},
    // serial number string
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_SerialNUMCharVal),          (u8*)(&my_characterUUID),       (u8*)(my_SerialNUMCharVal),                0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(my_SerialNumtrs),              (u8*)(&my_SerialNUMUUID),       (u8*)(my_SerialNumtrs), &att_ccc_control,  0},
    // firmware revision string
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_FWRevCharVal),              (u8*)(&my_characterUUID),       (u8*)(my_FWRevCharVal),               0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_FWRevtrs),                  (u8*)(&my_FWRevUUID),           (u8*)(my_FWRevtrs),                   0},
    // software revision string
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_SWRevCharVal),              (u8*)(&my_characterUUID),       (u8*)(my_SWRevCharVal),               0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_SWRevtrs),                  (u8*)(&my_SWRevUUID),           (u8*)(my_SWRevtrs),                   0},
    // hardware revision string
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_HWRevCharVal),              (u8*)(&my_characterUUID),       (u8*)(my_HWRevCharVal),               0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_HWRevtrs),                  (u8*)(&my_HWRevUUID),           (u8*)(my_HWRevtrs),                   0},

    /*============================================================================*
    *                                hid_profile
    *============================================================================*/    
    // HID Service
    {HID_CONTROL_POINT_DP_H - HID_PS_H + 1, ATT_PERMISSIONS_READ,2,2, (u8*)(&my_primaryServiceUUID),  (u8*)(&my_hidServiceUUID),            0},
    // include battery service
    {0, ATT_PERMISSIONS_READ,2, sizeof(include),                      (u8*)(&hidIncludeUUID),         (u8*)(include),                       0},
    // protocol mode
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidProtocolModeCharVal),    (u8*)(&my_characterUUID),       (u8*)(my_hidProtocolModeCharVal),     0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(protocolMode),                 (u8*)(&hidProtocolModeUUID),    (u8*)(&protocolMode),                 0},
    // boot keyboard input report
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidbootKeyInReporCharVal),  (u8*)(&my_characterUUID),       (u8*)(my_hidbootKeyInReporCharVal),   0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(bootKeyInReport),              (u8*)(&hidbootKeyInReportUUID), (u8*)(&bootKeyInReport),              0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(bootKeyInReportCCC),           (u8*)(&clientCharacterCfgUUID), (u8*)(bootKeyInReportCCC),            0},
    // boot keyboard output report
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidbootKeyOutReporCharVal), (u8*)(&my_characterUUID),       (u8*)(my_hidbootKeyOutReporCharVal),  0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(bootKeyOutReport),             (u8*)(&hidbootKeyOutReportUUID),(u8*)(&bootKeyOutReport),             0},
    // consume report in: 4
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidReportCCinCharVal),      (u8*)(&my_characterUUID),       (u8*)(my_hidReportCCinCharVal),       0}, 
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportConsumerControlIn),      (u8*)(&hidReportUUID),          (u8*)(reportConsumerControlIn),       0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportConsumerControlInCCC),   (u8*)(&clientCharacterCfgUUID), (u8*)(reportConsumerControlInCCC),    0}, 
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportRefConsumerControlIn),   (u8*)(&reportRefUUID),          (u8*)(reportRefConsumerControlIn),    0},
    // keyboard report in : 4
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidReportKEYinCharVal),     (u8*)(&my_characterUUID),       (u8*)(my_hidReportKEYinCharVal),      0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportKeyIn),                  (u8*)(&hidReportUUID),          (u8*)(reportKeyIn),                   0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportKeyInCCC),               (u8*)(&clientCharacterCfgUUID), (u8*)(reportKeyInCCC),                0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportRefKeyIn),               (u8*)(&reportRefUUID),          (u8*)(reportRefKeyIn),                0},
    // keyboard report out: 3
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidReportKEYoutCharVal),    (u8*)(&my_characterUUID),       (u8*)(my_hidReportKEYoutCharVal),     0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportKeyOut),                 (u8*)(&hidReportUUID),          (u8*)(reportKeyOut),                  0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportRefKeyOut),              (u8*)(&reportRefUUID),          (u8*)(reportRefKeyOut),               0},
    // report map: 3
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidReportMapCharVal),       (u8*)(&my_characterUUID),       (u8*)(my_hidReportMapCharVal),        0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportMap),                    (u8*)(&hidReportMapUUID),       (u8*)(reportMap),                     0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(extServiceUUID),               (u8*)(&extReportRefUUID),       (u8*)(&extServiceUUID),               0},
    // hid information: 2
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidinformationCharVal),     (u8*)(&my_characterUUID),       (u8*)(my_hidinformationCharVal),      0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(hidInformation),               (u8*)(&hidinformationUUID),     (u8*)(hidInformation),                0},
    // control point: 2
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidCtrlPointCharVal),       (u8*)(&my_characterUUID),       (u8*)(my_hidCtrlPointCharVal),        0}, 
    {0, ATT_PERMISSIONS_WRITE,2,sizeof(controlPoint),                 (u8*)(&hidCtrlPointUUID),       (u8*)(&controlPoint),                 0},

    /*============================================================================*
    *                                bas_profile
    *============================================================================*/   
    // Battery Service
    {4, ATT_PERMISSIONS_READ,2, 2,(u8*)(&my_primaryServiceUUID),      (u8*)(&my_batServiceUUID),       0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_batCharVal),                (u8*)(&my_characterUUID),       (u8*)(my_batCharVal),                 0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_batVal),                    (u8*)(&my_batCharUUID),         (u8*)(my_batVal),                     0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(batteryValueInCCC),            (u8*)(&clientCharacterCfgUUID), (u8*)(batteryValueInCCC),             0},

    /*============================================================================*
    *                                ota_profile
    *============================================================================*/
#if (GOOGLE_OTA)
    {5, ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_OtaServiceUUID),       0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_OtaCharVal),                (u8*)(&my_characterUUID),       (u8*)(my_OtaCharVal),                 0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(my_OtaData),                   (u8*)(&my_OtaUUID),             (&my_OtaData),                        &app_ota_handle, 0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(my_Otaccc),                    (u8*)(&clientCharacterCfgUUID), (u8*)(&my_Otaccc),                    &att_ccc_control  },
    {0, ATT_PERMISSIONS_READ,2, sizeof (my_OtaName),                  (u8*)(&userdesc_UUID),          (u8*)(my_OtaName),                    0},
#endif
#if (TELINK_OTA)
    // telink ota
    {4, ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_OtaServiceUUID),       0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_OtaCharVal),                (u8*)(&my_characterUUID),       (u8*)(my_OtaCharVal),                 0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(my_OtaData),                   (u8*)(&my_OtaUUID),             (&my_OtaData),                        &otaWrite, &otaRead},
    {0, ATT_PERMISSIONS_READ,2, sizeof (my_OtaName),                  (u8*)(&userdesc_UUID),          (u8*)(my_OtaName),                    0},
#endif
    /*============================================================================*
    *                                atv_profile
    *============================================================================*/
    // GOOGLE AUDIO service
#if BLE_AUDIO_ENABLE
    {9, ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&ATVV_Server_UUID),         0},
    // TX
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(my_MicCharTx),                 (u8*)(&my_characterUUID),        (u8*)(my_MicCharTx),                 0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(atv_char_tx_data),             (u8*)(&ATVV_Char_UUID_TX),       (u8*)(&atv_char_tx_data),            &app_auido_google_callback},
    // RX
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_MicCharRx),                 (u8*)(&my_characterUUID),        (u8*)(my_MicCharRx),                 0},
    {0, ATT_PERMISSIONS_READ,16,sizeof(atv_char_rx_data),             (u8*)(&ATVV_Char_UUID_RX),       (u8*)(&atv_char_rx_data),            0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(atv_char_rx_ccc),              (u8*)(&clientCharacterCfgUUID),  (u8*)(&atv_char_rx_ccc),             &att_ccc_control},
    //CTRL
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_MicCharCtl),                (u8*)(&my_characterUUID),        (u8*)(my_MicCharCtl),                0},
    {0, ATT_PERMISSIONS_READ,16,sizeof(atv_char_ctl_data),            (u8*)(&ATVV_Char_UUID_CTL),      (u8*)(&atv_char_ctl_data),           0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(atv_char_ctl_ccc),             (u8*)(&clientCharacterCfgUUID),  (u8*)(&atv_char_ctl_ccc),            &att_ccc_control},
#endif

    /*============================================================================*
    *                                seneasy_profile
    *============================================================================*/
	// Smart RCU Serivce
	{6, ATT_PERMISSIONS_READ,2, 16, 								  (u8*)(&my_primaryServiceUUID), 	(u8*)(uuid_service_seneasy_rcu),    0}, 
	{0, ATT_PERMISSIONS_READ,2, sizeof(char_decl_seneasy_rcu_rd),	  (u8*)(&my_characterUUID),		    (u8*)(char_decl_seneasy_rcu_rd),    0}, 
	{0, ATT_PERMISSIONS_READ,16,0,		                              (u8*)(uuid_char_seneasy_rcu_rd),	 0,                                 &att_ccc_control, 0}, 
	{0, ATT_PERMISSIONS_READ,2, sizeof(char_decl_seneasy_rcu_wd),     (u8*)(&my_characterUUID),		    (u8*)(char_decl_seneasy_rcu_wd),    0},
	{0, ATT_PERMISSIONS_RDWR,16,0,									  (u8*)(uuid_char_seneasy_rcu_wr),	 0,                                 seneasy_rcu_svc_attr_write_cb,0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(char_desc_smart_rcu_ccc),      (u8*)(&clientCharacterCfgUUID),   (u8*)(&char_desc_smart_rcu_ccc),    &seneasy_rcu_ccc_update_cb},

#if (TL_SZ119_TVSSU)
    {4,ATT_PERMISSIONS_READ,2,16,(u8*)(&my_primaryServiceUUID),(u8*)(&my_SimpleSetupServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(ss_CharVal),(u8*)(&my_characterUUID),(u8*)(ss_CharVal), 0},
   	{0,ATT_PERMISSIONS_RDWR,16,sizeof(my_SimpleSetupData),(u8*)(&my_SimpleSetupUUID),(u8*)(&my_SimpleSetupData),  &seneasy_tvssu_attr_write_cb},	//value
	{0,ATT_PERMISSIONS_RDWR|ATT_PERMISSIONS_ENCRYPT_RDWR,2,sizeof(simplesetupValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&simplesetupValueInCCC), 0},
#endif 

    ////////////////////////////////////// IR /////////////////////////////////////////////////////
#if APP_IR_OVER_BLE
    //IR
    {12,ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_IrUUID),                0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_ProgControl_CharVal),    (u8*)(&my_characterUUID),        (u8*)(my_Ir_ProgControl_CharVal),    0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(ir_data),                      (u8*)(&my_Ir_ProgControlUUID),   (u8*)(ir_data),                      &app_ir_callback},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_Key_Id_CharVal),         (u8*)(&my_characterUUID),        (u8*)(my_Ir_Key_Id_CharVal),         0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(ir_data),                      (u8*)(&my_Ir_KeyIdUUID),         (u8*)(ir_data),                      &app_ir_callback},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_Code_CharVal),           (u8*)(&my_characterUUID),        (u8*)(my_Ir_Code_CharVal),           0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(ir_data),                      (u8*)(&my_Ir_CodeUUID),          (u8*)(ir_data),                      &app_ir_callback},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_Suppress_CharVal),       (u8*)(&my_characterUUID),        (u8*)(my_Ir_Suppress_CharVal),       0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(ir_data),                      (u8*)(&my_Ir_SuppressUUID),      (u8*)(ir_data),                      &app_ir_callback},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_Key_Event_CharVal),      (u8*)(&my_characterUUID),        (u8*)(my_Ir_Key_Event_CharVal),      0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(ir_data),                      (u8*)(&my_Ir_KeyEventUUID),      (u8*)(ir_data),                      0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(ir_data),                      (u8*)(&clientCharacterCfgUUID),  (u8*)(ir_data),                      &app_ir_callback},
#endif
#if FIND_ME_ENABLE
    {6, ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&FMS_Server_UUID),          0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_fms_CharCtlVal),            (u8*)(&my_characterUUID),        (u8*)(my_fms_CharCtlVal),            0},
    {0, ATT_PERMISSIONS_RDWR,16,2,                                    (u8*)(&FMS_Char_UUID_CTL),       (u8 *) g_p_app_fms_ctrl_val,         &app_fms_att_ctl_cb},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_fms_CharDataVal),           (u8*)(&my_characterUUID),        (u8*)(my_fms_CharDataVal),           0},
    {0, ATT_PERMISSIONS_RDWR,16,2,                                    (u8*)(&FMS_Char_UUID_DATA),      (u8 *)g_p_app_fms_data_val,          &app_fms_att_data_cb},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(my_fms_data_ccc),              (u8*)(&clientCharacterCfgUUID),  (u8*)(&my_fms_data_ccc),             &att_ccc_control},
#endif
};



#if (MP_TEST_MODE)
static const u8 my_TestModeServiceUUID[16] = {TEST_MODE_SERVICE_UUID};
static const u8 my_TestDataUUID[16] = {TEST_MODE_TEST_DATA_UUID};
static u8 my_TestData = 0x00;
static u8 my_TestDataCCC[2] = { 0x00, };

/// Test Mode attribute values
static const u8 my_TestCharVal[19] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_NOTIFY,
    U16_LO(TEST_MODE_DP_H), U16_HI(TEST_MODE_DP_H),
    TEST_MODE_TEST_DATA_UUID,
};
#endif

#if APP_IR_OVER_BLE
static const u8 my_IrUUID[16]               = {TELINK_IR_UUID_SERVICE};
static const u8 my_Ir_ProgControlUUID[16]   = {TELINK_IR_PROG_CONTROL_UUID_SERVICE};
static const u8 my_Ir_KeyIdUUID[16]         = {TELINK_IR_KEY_ID_UUID_SERVICE};
static const u8 my_Ir_CodeUUID[16]          = {TELINK_IR_CODE_UUID_SERVICE};
static const u8 my_Ir_SuppressUUID[16]      = {TELINK_IR_SUPPRESS_UUID_SERVICE};
static const u8 my_Ir_KeyEventUUID[16]      = {TELINK_IR_KEY_EVENT_UUID_SERVICE};
static const u8 my_Ir_ProgControl_CharVal[19] = {
    CHAR_PROP_WRITE,
    U16_LO(IR_PROG_OUT_CONTROL_H), U16_HI(IR_PROG_OUT_CONTROL_H),
    TELINK_IR_PROG_CONTROL_UUID_SERVICE
};
static const u8 my_Ir_Key_Id_CharVal[19] = {
    CHAR_PROP_WRITE,
    U16_LO(IR_KEY_ID_H), U16_HI(IR_KEY_ID_H),
    TELINK_IR_KEY_ID_UUID_SERVICE
};
static const u8 my_Ir_Code_CharVal[19] = {
    CHAR_PROP_WRITE,
    U16_LO(IR_CODE_H), U16_HI(IR_CODE_H),
    TELINK_IR_CODE_UUID_SERVICE
};
static const u8 my_Ir_Suppress_CharVal[19] = {
    CHAR_PROP_WRITE,
    U16_LO(IR_SUPPRESS_H), U16_HI(IR_SUPPRESS_H),
    TELINK_IR_SUPPRESS_UUID_SERVICE
};
static const u8 my_Ir_Key_Event_CharVal[19] = {
    CHAR_PROP_NOTIFY,
    U16_LO(IR_KEY_EVENT_H), U16_HI(IR_KEY_EVENT_H),
    TELINK_IR_KEY_EVENT_UUID_SERVICE
};
u8 ir_data[1] = {0};

extern void app_set_mtusize(void);
extern void ir_fallback_process(u16 handle, u8* buf, u16 len);

int app_ir_callback(void *p)
{
    u16 handle,len;
    u16 printf_len;

    rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;
    handle = pw->handle;

    //printf("handle = %x\r\n",handle);
    //printf("pw->l2cap=%x\r\n",pw->l2cap-3);
    printf("dat=\r\n");

    len = pw->l2cap-3;//pw->rf_len-7;
    printf_len = len;
    #if 0
    if(printf_len > 6)
        printf_len = 6;
    for(u16 i=0;i<printf_len;i++)
        printf(" %x",pw->dat[i]);
    #endif
    printf("len=%x\r\n",len);
    app_set_mtusize();
    ir_fallback_process(handle,pw->dat,len);

    return 0;
}
#endif

#if FIND_ME_ENABLE
static const u8 FMS_Server_UUID[16]    = {GOOGLE_FMS_SERVfICE_UUID};
static const u8 FMS_Char_UUID_CTL[16]  = {GOOGLE_FMS_CTL_CHAR_UUID};
static const u8 FMS_Char_UUID_DATA[16] = {GOOGLE_FMS_DATA_CHAR_UUID};
_attribute_data_retention_ u16 my_fms_data_ccc = 0;

static const u8 my_fms_CharCtlVal[19] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
    U16_LO(FMS_GOOGLE_CTL_DP_H), U16_HI(FMS_GOOGLE_CTL_DP_H),
    GOOGLE_FMS_CTL_CHAR_UUID
};

static const u8 my_fms_CharDataVal[19] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_NOTIFY,
    U16_LO(FMS_GOOGLE_DATA_DP_H), U16_HI(FMS_GOOGLE_DATA_DP_H),
    GOOGLE_FMS_DATA_CHAR_UUID
};

#endif

#if (MP_TEST_MODE)
static const attribute_t my_test_demo_Attributes[] = {
    {ATT_END_H - 1, 0,0,0,0,0},    // total num of attribute
    /////////////////////////////////// 1. Gap /////////////////////////////////////////////////////////
    // 0001 - 0007              (1-7)
    {7, ATT_PERMISSIONS_READ,2, 2,(u8*)(&my_primaryServiceUUID),      (u8*)(&my_gapServiceUUID),        0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_devNameCharVal),            (u8*)(&my_characterUUID),        (u8*)(my_devNameCharVal),            0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(_custom_data.device_name),     (u8*)(&my_devNameUUID),          (u8*)(_custom_data.device_name),     0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_appearanceCharVal),         (u8*)(&my_characterUUID),        (u8*)(my_appearanceCharVal),         0},
    {0, ATT_PERMISSIONS_READ,2, sizeof (my_appearance),               (u8*)(&my_appearanceUIID),       (u8*)(&my_appearance),               0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_periConnParamCharVal),      (u8*)(&my_characterUUID),        (u8*)(my_periConnParamCharVal),      0},
    {0, ATT_PERMISSIONS_READ,2, sizeof (my_periConnParameters),       (u8*)(&my_periConnParamUUID),    (u8*)(&my_periConnParameters),       0},

    /////////////////////////////////// 2. Gatt ////////////////////////////////////////////////////////
    // 0008 - 000b              (8-11)
    {4, ATT_PERMISSIONS_READ,2, 2,(u8*)(&my_primaryServiceUUID),      (u8*)(&my_gattServiceUUID),       0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_serviceChangeCharVal),      (u8*)(&my_characterUUID),        (u8*)(my_serviceChangeCharVal),      0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(serviceChangeVal),             (u8*)(&serviceChangeUUID),       (u8*)(&serviceChangeVal),            0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(serviceChangeCCC),             (u8*)(&clientCharacterCfgUUID),  (u8*)(serviceChangeCCC),             0},

    /////////////////////////////////// 3. Device Information Service //////////////////////////////////
    // 000c - 0014
    // PnP ID                   (12-14)
    {9, ATT_PERMISSIONS_READ,2, 2,(u8*)(&my_primaryServiceUUID),      (u8*)(&my_devServiceUUID),        0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_PnCharVal),                 (u8*)(&my_characterUUID),        (u8*)(my_PnCharVal),                 0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(_custom_data.pnp_id),          (u8*)(&my_PnPUUID),              (u8*)(_custom_data.pnp_id),          0},
    // firmware revision string (15-16)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_FWRevCharVal),              (u8*)(&my_characterUUID),        (u8*)(my_FWRevCharVal),              0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_FWRevtrs),                  (u8*)(&my_FWRevUUID),            (u8*)(my_FWRevtrs),                  0},
    // software revision string (17-18)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_SWRevCharVal),              (u8*)(&my_characterUUID),        (u8*)(my_SWRevCharVal),              0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_SWRevtrs),                  (u8*)(&my_SWRevUUID),            (u8*)(my_SWRevtrs),                  0},
    // hardware revision string (19-20)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_HWRevCharVal),              (u8*)(&my_characterUUID),        (u8*)(my_HWRevCharVal),              0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_HWRevtrs),                  (u8*)(&my_HWRevUUID),            (u8*)(my_HWRevtrs),                  0},

    /////////////////////////////////// 4. HID Service /////////////////////////////////////////////////
    // 0015 - 002F
    // primary                  (21)
    {HID_CONTROL_POINT_DP_H - HID_PS_H + 1, ATT_PERMISSIONS_READ,2,2, (u8*)(&my_primaryServiceUUID),   (u8*)(&my_hidServiceUUID),           0},
    // include battery service  (22)
    {0, ATT_PERMISSIONS_READ,2, sizeof(include),                      (u8*)(&hidIncludeUUID),          (u8*)(include),                      0},
    // protocol mode            (23-24)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidProtocolModeCharVal),    (u8*)(&my_characterUUID),        (u8*)(my_hidProtocolModeCharVal),    0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(protocolMode),                 (u8*)(&hidProtocolModeUUID),     (u8*)(&protocolMode),                0},
    // boot keyboard input report (char-val-client)   (25-27)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidbootKeyInReporCharVal),  (u8*)(&my_characterUUID),        (u8*)(my_hidbootKeyInReporCharVal),  0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(bootKeyInReport),              (u8*)(&hidbootKeyInReportUUID),  (u8*)(&bootKeyInReport),             0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(bootKeyInReportCCC),           (u8*)(&clientCharacterCfgUUID),  (u8*)(bootKeyInReportCCC),           0},
    // boot keyboard output report (char-val)         (28-29)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidbootKeyOutReporCharVal), (u8*)(&my_characterUUID),        (u8*)(my_hidbootKeyOutReporCharVal), 0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(bootKeyOutReport),             (u8*)(&hidbootKeyOutReportUUID), (u8*)(&bootKeyOutReport),            0},
    // consume report in: 4 (char-val-client-ref)     (30-33)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidReportCCinCharVal),      (u8*)(&my_characterUUID),        (u8*)(my_hidReportCCinCharVal),      0}, 
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportConsumerControlIn),      (u8*)(&hidReportUUID),           (u8*)(reportConsumerControlIn),      0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportConsumerControlInCCC),   (u8*)(&clientCharacterCfgUUID),  (u8*)(reportConsumerControlInCCC),   0}, 
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportRefConsumerControlIn),   (u8*)(&reportRefUUID),           (u8*)(reportRefConsumerControlIn),   0},
    // keyboard report in : 4 (char-val-client-ref)   (34-37)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidReportKEYinCharVal),     (u8*)(&my_characterUUID),        (u8*)(my_hidReportKEYinCharVal),     0},   
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportKeyIn),                  (u8*)(&hidReportUUID),           (u8*)(reportKeyIn),                  0},   
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportKeyInCCC),               (u8*)(&clientCharacterCfgUUID),  (u8*)(reportKeyInCCC),               0},  
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportRefKeyIn),               (u8*)(&reportRefUUID),           (u8*)(reportRefKeyIn),               0},   
    // keyboard report out: 3 (char-val-ref)          (38-40)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidReportKEYoutCharVal),    (u8*)(&my_characterUUID),        (u8*)(my_hidReportKEYoutCharVal),    0},          
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportKeyOut),                 (u8*)(&hidReportUUID),           (u8*)(reportKeyOut),                 0},   
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(reportRefKeyOut),              (u8*)(&reportRefUUID),           (u8*)(reportRefKeyOut),              0}, 
    // report map: 3            (41-43)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidReportMapCharVal),       (u8*)(&my_characterUUID),        (u8*)(my_hidReportMapCharVal),       0},               
    {0, ATT_PERMISSIONS_READ,2, sizeof(reportMap),                    (u8*)(&hidReportMapUUID),        (u8*)(reportMap),                    0},   
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(extServiceUUID),               (u8*)(&extReportRefUUID),        (u8*)(&extServiceUUID),              0},   
    // hid information: 2       (44-45)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidinformationCharVal),     (u8*)(&my_characterUUID),        (u8*)(my_hidinformationCharVal),     0},             
    {0, ATT_PERMISSIONS_READ,2, sizeof(hidInformation),               (u8*)(&hidinformationUUID),      (u8*)(hidInformation),               0},   
    // control point: 2         (46-47)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_hidCtrlPointCharVal),       (u8*)(&my_characterUUID),        (u8*)(my_hidCtrlPointCharVal),       0},              
    {0, ATT_PERMISSIONS_WRITE,2,sizeof(controlPoint),                 (u8*)(&hidCtrlPointUUID),        (u8*)(&controlPoint),                0},   

    /////////////////////////////////// 5. Battery Service /////////////////////////////////////////////
    // 0030 - 0033              (48-51)
    {4, ATT_PERMISSIONS_READ,2, 2,(u8*)(&my_primaryServiceUUID),      (u8*)(&my_batServiceUUID),        0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_batCharVal),                (u8*)(&my_characterUUID),        (u8*)(my_batCharVal),                0},             
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_batVal),                    (u8*)(&my_batCharUUID),          (u8*)(my_batVal),                    0},    
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(batteryValueInCCC),            (u8*)(&clientCharacterCfgUUID),  (u8*)(batteryValueInCCC),            0}, 

    /////////////////////////////////// 6. OTA /////////////////////////////////////////////////////////
    // 0034 - 0038              (52-56)
    {5, ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_OtaServiceUUID), 0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_OtaCharVal),                (u8*)(&my_characterUUID),        (u8*)(my_OtaCharVal),                0},           
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(my_OtaData),                   (u8*)(&my_OtaUUID),              (&my_OtaData),                       &app_ota_write,0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(my_Otaccc),                    (u8*)(&clientCharacterCfgUUID),  (u8*)(&my_Otaccc),                   &att_ccc_control},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_OtaName),                   (u8*)(&userdesc_UUID),           (u8*)(my_OtaName),                   0},

    /////////////////////////////////// 7. Audio ///////////////////////////////////////////////////////
    /********************************************************************************************
    * GOOGLE AUDIO service
    ********************************************************************************************/
    // 0039 - 0041
    // primary                  (57)
#if BLE_AUDIO_ENABLE
    {9, ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&ATVV_Server_UUID),         0},
    // TX                       (58-59)
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(my_MicCharTx),                 (u8*)(&my_characterUUID),        (u8*)(my_MicCharTx),                 0},
    {0, ATT_PERMISSIONS_RDWR,16,sizeof(atv_char_tx_data),             (u8*)(&ATVV_Char_UUID_TX),       (u8*)(&atv_char_tx_data),            &app_auido_google_callback},    
    // RX                       (60-62)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_MicCharRx),                 (u8*)(&my_characterUUID),        (u8*)(my_MicCharRx),                 0},
    {0, ATT_PERMISSIONS_READ,16,sizeof(atv_char_rx_data),             (u8*)(&ATVV_Char_UUID_RX),       (u8*)(&atv_char_rx_data),            0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(atv_char_rx_ccc),              (u8*)(&clientCharacterCfgUUID),  (u8*)(&atv_char_rx_ccc),             &att_ccc_control},
    // CTRL                     (63-65)
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_MicCharCtl),                (u8*)(&my_characterUUID),        (u8*)(my_MicCharCtl),                0},
    {0, ATT_PERMISSIONS_READ,16,sizeof(atv_char_ctl_data),            (u8*)(&ATVV_Char_UUID_CTL),      (u8*)(&atv_char_ctl_data),           0},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(atv_char_ctl_ccc),             (u8*)(&clientCharacterCfgUUID),  (u8*)(&atv_char_ctl_ccc),            &att_ccc_control},
#endif

    /////////////////////////////////// 8. IR //////////////////////////////////////////////////////////
    // 0042 - 004D              (66-77)
#if APP_IR_OVER_BLE
    {12,ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_IrUUID),                0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_ProgControl_CharVal),    (u8*)(&my_characterUUID),        (u8*)(my_Ir_ProgControl_CharVal),    0},       
    {0, ATT_PERMISSIONS_RDWR,16,0,                                    (u8*)(&my_Ir_ProgControlUUID),    0,                                  &app_ir_callback},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_Key_Id_CharVal),         (u8*)(&my_characterUUID),        (u8*)(my_Ir_Key_Id_CharVal),         0},             
    {0, ATT_PERMISSIONS_RDWR,16,0,                                    (u8*)(&my_Ir_KeyIdUUID),          0,                                  &app_ir_callback},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_Code_CharVal),           (u8*)(&my_characterUUID),        (u8*)(my_Ir_Code_CharVal),           0},                  
    {0, ATT_PERMISSIONS_RDWR,16,0,                                    (u8*)(&my_Ir_CodeUUID),           0,                                  &app_ir_callback},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_Suppress_CharVal),       (u8*)(&my_characterUUID),        (u8*)(my_Ir_Suppress_CharVal),       0},          
    {0, ATT_PERMISSIONS_RDWR,16,0,                                    (u8*)(&my_Ir_SuppressUUID),       0,                                  &app_ir_callback},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_Ir_Key_Event_CharVal),      (u8*)(&my_characterUUID),        (u8*)(my_Ir_Key_Event_CharVal),      0},           
    {0, ATT_PERMISSIONS_RDWR,16,0,                                    (u8*)(&my_Ir_KeyEventUUID),       0,                                  0},
    {0, ATT_PERMISSIONS_RDWR,2, 0,                                    (u8*)(&clientCharacterCfgUUID),   0,                                  &app_ir_callback},
#endif
#if FIND_ME_ENABLE
    {6, ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&FMS_Server_UUID),          0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_fms_CharCtlVal),            (u8*)(&my_characterUUID),        (u8*)(my_fms_CharCtlVal),            0},
    {0, ATT_PERMISSIONS_RDWR,16,2,                                    (u8*)(&FMS_Char_UUID_CTL),       (u8 *) g_p_app_fms_ctrl_val,         &app_fms_att_ctl_cb},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_fms_CharDataVal),           (u8*)(&my_characterUUID),        (u8*)(my_fms_CharDataVal),           0},
    {0, ATT_PERMISSIONS_RDWR,16,2,                                    (u8*)(&FMS_Char_UUID_DATA),      (u8 *)g_p_app_fms_data_val,          &app_fms_att_data_cb},
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(my_fms_data_ccc),              (u8*)(&clientCharacterCfgUUID),  (u8*)(&my_fms_data_ccc),             &att_ccc_control},
#endif

    /////////////////////////////////// 9. TEST MODE ///////////////////////////////////////////////////
    // 004E - 0051              (78-81)
    {4, ATT_PERMISSIONS_READ,2, 16,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_TestModeServiceUUID),   0},
    {0, ATT_PERMISSIONS_READ,2, sizeof(my_TestCharVal),               (u8*)(&my_characterUUID),        (u8*)(my_TestCharVal),               0},     
    {0, ATT_PERMISSIONS_WRITE,16,sizeof(my_TestData),                 (u8*)(&my_TestDataUUID),         (&my_TestData),                      &test_write_cb, 0}, 
    {0, ATT_PERMISSIONS_RDWR,2, sizeof(my_TestDataCCC),               (u8*)(&clientCharacterCfgUUID),  (u8*)(my_TestDataCCC),               0},
};
#endif
