#include <string.h>
#include "co_debug.h"
#include "co.h"
#include "hid_service.h"
#include "gatt_sig_uuid.h"
#include "app_gap.h"

#define MOUSE_DATA_LEN_AAA	4

#define HID_REPORT_TYPE_INPUT       0x01
#define HID_REPORT_TYPE_OUTPUT      0x02
#define HID_REPORT_TYPE_FEATURE     0x03
#define HID_REPORT_TYPE_WR          0x10    /// Input report with Write capabilities

#define ATT_LEN_DESC  sizeof(uint16_t)

// Start handle
static uint16_t m_start_handle;

static const uint8_t serv_hid[]     = UUID16_ARR(HID_SERV_UUID);
static const uint8_t char_info[]    = UUID16_ARR(HID_INFORMATION_UUID); //{ 0x4A, 0x2A };  
static const uint8_t char_rep_map[] = UUID16_ARR(REPORT_MAP_UUID);	// { 0x4B, 0x2A };  
static const uint8_t char_ctrl[]    = UUID16_ARR(HID_CTRL_PT_UUID); // { 0x4C, 0x2A };
static const uint8_t char_report[]  = UUID16_ARR(REPORT_UUID); //{ 0x4D, 0x2A };
static const uint8_t char_mode[]    = UUID16_ARR(PROTOCOL_MODE_UUID); //{ 0x4E, 0x2A };
static const uint8_t char_bki_rep[] = UUID16_ARR(BOOT_KEY_INPUT_UUID); //{ 0x22, 0x2A };
static const uint8_t char_bko_rep[] = UUID16_ARR(BOOT_KEY_OUTPUT_UUID); //{ 0x32, 0x2A };

static const uint8_t hid_report_map[] = {
    //keyboard report in
    0x05, 0x01,         // Usage Pg (Generic Desktop)
    0x09, 0x06,         // Usage (Keyboard)
    0xA1, 0x01,         // Collection: (Application)
    0x85, 0x01, 		// Report Id (keyboard)
    0x05, 0x07,         // Usage Pg (Key Codes)
    0x19, 0xE0,         // Usage Min (224)  VK_CTRL:0xe0
    0x29, 0xE7,         // Usage Max (231)  VK_RWIN:0xe7
    0x15, 0x00,         // Log Min (0)
    0x25, 0x01,         // Log Max (1)
    0x75, 0x01,         // Report Size (1)   1 bit * 8
    0x95, 0x08,         // Report Count (8)
    0x81, 0x02,         // Input: (Data, Variable, Absolute)
    0x95, 0x01,         // Report Count (1)
    0x75, 0x08,         // Report Size (8)
    0x81, 0x01,         // Input: (Constant)
    //keyboard output
    //5 bit led ctrl: NumLock CapsLock ScrollLock Compose kana
    0x75, 0x08,         // Report Size (8)
    0x95, 0x06,         // Report Count (6)
    0x15, 0x00,         // Log Min (0)
    0x25, 0xFF,         // Log Max (255)
    0x05, 0x07,         // Usage Pg (Key Codes)
    0x19, 0x00,         // Usage Min (0)
    0x29, 0x13,         // Usage Max (19)
    0x09, 0x00,         // Usage
    0x19, 0x15,         // Usage Min (21)
    0x29, 0xFF,         // Usage Max (255)
    0x81, 0x00,         // Input: (Data, Array)
    0xC0,               // End Collection
    //consumer report in
    0x05, 0x0C,         // Usage Page (Consumer)
    0x09, 0x01,         // Usage (Consumer Control)
    0xA1, 0x01,         // Collection (Application)
    0x85, 0x02,  		// Report Id
    0x95, 0x01,         // global, report count 1
    0x75, 0x10,         // global, report size 16 bits
    0x15, 0x00,         // global, min  0x01
    0x26, 0xff, 0x03,   // global, max  0x28c
    0x19, 0x00,         // local,  min  0x01
    0x2a, 0xff, 0x03,   // local,  max  0x28c
    0x81, 0x00,         // main,  input data variable, absolute
    0xc0,               // main, end collection

#if 0
	//consumer report in
	0x05, 0x0C,   // Usage Page (Consumer)
	0x09, 0x01,   // Usage (Consumer Control)
	0xA1, 0x01,   // Collection (Application)
	0x85, 0x01,   //     Report Id
	0x75,0x10,     //global, report size 16 bits
	0x95,0x02,     //global, report count 1
	0x15,0x01,     //global, min  0x01
	0x26,0x8c,0x02,  //global, max  0x28c
	0x19,0x01,     //local, min   0x01
	0x2a,0x8c,0x02,  //local, max    0x28c
	0x81,0x00,     //main,  input data varible, absolute
	0xc0,        //main, end collection
	
	//keyboard report in
	0x05, 0x01,     // Usage Pg (Generic Desktop)
	0x09, 0x06,     // Usage (Keyboard)
	0xA1, 0x01,     // Collection: (Application)
	0x85, 0x02,     // Report Id (keyboard)
				  //
	0x05, 0x07,     // Usage Pg (Key Codes)
	0x19, 0xE0,     // Usage Min (224)  VK_CTRL:0xe0
	0x29, 0xE7,     // Usage Max (231)  VK_RWIN:0xe7
	0x15, 0x00,     // Log Min (0)
	0x25, 0x01,     // Log Max (1)
				  //
				  // Modifier byte
	0x75, 0x01,     // Report Size (1)   1 bit * 8
	0x95, 0x08,     // Report Count (8)
	0x81, 0x02,     // Input: (Data, Variable, Absolute)
				  //
				  // Reserved byte
	0x95, 0x01,     // Report Count (1)
	0x75, 0x08,     // Report Size (8)
	0x81, 0x01,     // Input: (static constant)

	//keyboard output
	//5 bit led ctrl: NumLock CapsLock ScrollLock Compose kana
	0x95, 0x05,    //Report Count (5)
	0x75, 0x01,    //Report Size (1)
	0x05, 0x08,    //Usage Pg (LEDs )
	0x19, 0x01,    //Usage Min
	0x29, 0x05,    //Usage Max
	0x91, 0x02,    //Output (Data, Variable, Absolute)
	//3 bit reserved
	0x95, 0x01,    //Report Count (1)
	0x75, 0x03,    //Report Size (3)
	0x91, 0x01,    //Output (static constant)

	// Key arrays (6 bytes)
	0x95, 0x06,     // Report Count (6)
	0x75, 0x08,     // Report Size (8)
	0x15, 0x00,     // Log Min (0)
	0x25, 0xFF,     // Log Max (241)
	0x05, 0x07,     // Usage Pg (Key Codes)
	0x19, 0x00,     // Usage Min (0)
	0x29, 0xFF,     // Usage Max (241)
	0x81, 0x00,     // Input: (Data, Array)

	0xC0,            // End Collection
	
		//mouse report in
	0x05, 0x01,  // Usage Page (Generic Desktop)
	0x09, 0x02,  // Usage (Mouse)
	0xA1, 0x01,  // Collection (Application)
	0x85, 0x03,  // Report Id
	0x09, 0x01,  //   Usage (Pointer)
	0xA1, 0x00,  //   Collection (Physical)
	0x05, 0x09,  //	 Usage Page (Buttons)
	0x19, 0x01,  //	 Usage Minimum (01) - Button 1
	0x29, 0x03,  //	 Usage Maximum (03) - Button 3
	0x15, 0x00,  //	 Logical Minimum (0)
	0x25, 0x01,  //	 Logical Maximum (1)
	0x75, 0x01,  //	 Report Size (1)
	0x95, 0x05,  //	 Report Count (3)
	0x81, 0x02,  //	 Input (Data, Variable, Absolute) - Button states
	0x75, 0x03,  //	 Report Size (5)
	0x95, 0x01,  //	 Report Count (1)
	0x81, 0x01,  //	 Input (Constant) - Padding or Reserved bits

	0x05, 0x01, 	 //  Usage Page (Generic Desktop Control)
	0x09, 0x30,  // Usage (X)
	0x09, 0x31,  // Usage (Y)

#if (MOUSE_DATA_LEN_AAA==4)
	0x15, 0x81, //  LOGICAL_MINIMUM(0)
	0x25, 0x7f,
	0x75, 0x08, //  Report Size (8)
	0x95, 0x02, //  Report Count (2)
	0x81, 0x06, //  Input (Data, Variable, Relative)
#elif (MOUSE_DATA_LEN_AAA==6)
	0x16, 0x01, 0x80, //  LOGICAL_MINIMUM(0)
	0x26, 0xff, 0x7f,
	0x75, 0x10, //  Report Size (16)
	0x95, 0x02, //  Report Count (2)
	0x81, 0x06, //  Input (Data, Variable, Relative)
#endif

	//0x05,0x01,			 //  Usage Page (Generic Desktop Control)
	0x09, 0x38, 		 //  Usage (Wheel)
	0x15, 0x81, 		 //  Logical Minimum (-4)
	0x25, 0x7F, 		 //  Logical Maximum (3)
	0x75, 0x08, 		 //  Report Size (3)
	0x95, 0x01, 		 //  Report Count (1)
	0x81, 0x06, 		 //  Input (Data, Variable, Relative)

	0xC0,		  //   End Collection
	0xC0,		  // End Collection
#endif
};

static const uint8_t val_hid_info[] = { 0x11, 0x01, 0x00, 0x01 };

static const uint8_t val_pro_mode[] = { 0x01 };

static uint8_t val_bki_rep[]  = { 0x00 };
static uint8_t val_bko_rep[]  = { 0x00 };

static uint8_t report_all0[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t hid_ref1[]  = { 0x01, HID_REPORT_TYPE_INPUT };
static const uint8_t hid_ref2[]  = { 0x02, HID_REPORT_TYPE_INPUT };
static const uint8_t hid_ref3[]  = { 0x03, HID_REPORT_TYPE_INPUT };
static const uint8_t hid_ref4[]  = { 0x58, HID_REPORT_TYPE_INPUT };
static const uint8_t hid_ref5[]  = { 0x58, HID_REPORT_TYPE_OUTPUT };


static uint16_t desc_kbi  = 0x0000; // disabled by default
static uint16_t desc_rep1 = 0x0000;
static uint16_t desc_rep3 = 0x0000;
static uint16_t desc_rep5 = 0x0000;
static uint16_t desc_rep2 = 0x0000;
static uint16_t desc_rep4 = 0x0000;


  static const att_db_t att_db[] = { // no permission needed, PR, PW default to 0
        // Index                 p_UUID           p_value                v_len                properties     128bit PR  PW
        [IDX_HID_SVC]           = {ATT_SERV_UUID, (uint8_t *)serv_hid,     sizeof(serv_hid),      ATT_PROP_READ, 0, 0, 0},
        
		[IDX_HID_INFO_CHAR]     = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_INFO_VAL]      = {char_info,     (uint8_t *)val_hid_info, sizeof(val_hid_info),  ATT_PROP_READ, 0, 1, 1},
      
		[IDX_HID_CTRL_CHAR]     = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_CTRL_VAL]      = {char_ctrl,     NULL,                    0,                	  ATT_PROP_WRITE_CMD, 0, 1, 1},
        
		[IDX_HID_REP_MAP_CHAR]  = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_REP_MAP_VAL]   = {char_rep_map,  (uint8_t *)hid_report_map,  sizeof(hid_report_map),   ATT_PROP_READ, 0, 1, 1},
        
		[IDX_HID_PRO_MODE_CHAR] = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_PRO_MODE_VAL]  = {char_mode,     (uint8_t *)val_pro_mode, sizeof(val_pro_mode),  ATT_PROP_READ  | ATT_PROP_WRITE_CMD,  0, 1, 1},
        
		[IDX_HID_BKI_REP_CHAR]  = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_BKI_REP_VAL]   = {char_bki_rep,  (uint8_t *)val_bki_rep,  sizeof(val_bki_rep),   ATT_PROP_READ  | ATT_PROP_WRITE  | ATT_PROP_NTF,  0, 1, 1},
        [IDX_HID_BKI_REP_DESC]  = {ATT_CCCD_UUID, (uint8_t *) &desc_kbi,   ATT_LEN_DESC,          ATT_PROP_WRITE | ATT_PROP_READ,  0, 1, 1, sizeof(uint16_t)},
        [IDX_HID_BKO_REP_CHAR]  = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_BKO_REP_VAL]   = {char_bko_rep,  (uint8_t *)val_bko_rep,  sizeof(val_bko_rep),   ATT_PROP_READ  | ATT_PROP_WRITE_CMD | ATT_PROP_WRITE,  0, 1, 1},
        
		[IDX_HID_REPORT1_CHAR]  = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_REPORT1_VAL]   = {char_report,   (uint8_t *)report_all0,  sizeof(report_all0),   ATT_PROP_READ  | ATT_PROP_NTF,  0, 1, 1},
        [IDX_HID_REPORT1_REF]   = {ATT_RRD_UUID,  (uint8_t *)hid_ref1,     sizeof(hid_ref1),      ATT_PROP_READ, 0, 1, 0},
        [IDX_HID_REPORT1_DESC]  = {ATT_CCCD_UUID, (uint8_t *) &desc_rep1,  ATT_LEN_DESC,          ATT_PROP_WRITE | ATT_PROP_READ,  0, 1, 1, sizeof(uint16_t)},
        
		[IDX_HID_REPORT2_CHAR]  = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_REPORT2_VAL]   = {char_report,   (uint8_t *)report_all0,  sizeof(report_all0),   ATT_PROP_READ  | ATT_PROP_NTF,  0, 1, 1},
        [IDX_HID_REPORT2_REF]   = {ATT_RRD_UUID,  (uint8_t *)hid_ref2,     sizeof(hid_ref2),      ATT_PROP_READ, 0, 1, 0},
        [IDX_HID_REPORT2_DESC]  = {ATT_CCCD_UUID, (uint8_t *) &desc_rep2,  ATT_LEN_DESC,          ATT_PROP_WRITE | ATT_PROP_READ,  0, 1, 1, sizeof(uint16_t)},
        
		[IDX_HID_REPORT3_CHAR]  = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_REPORT3_VAL]   = {char_report,   (uint8_t *)report_all0,  sizeof(report_all0),   ATT_PROP_READ  | ATT_PROP_NTF,  0, 1, 1},
        [IDX_HID_REPORT3_REF]   = {ATT_RRD_UUID,  (uint8_t *)hid_ref3,     sizeof(hid_ref3),      ATT_PROP_READ, 0, 1, 0},
        [IDX_HID_REPORT3_DESC]  = {ATT_CCCD_UUID, (uint8_t *) &desc_rep3,  ATT_LEN_DESC,          ATT_PROP_WRITE | ATT_PROP_READ,  0, 1, 1, sizeof(uint16_t)},
        
		[IDX_HID_REPORT4_CHAR]  = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_REPORT4_VAL]   = {char_report,   (uint8_t *)report_all0,  sizeof(report_all0),                     ATT_PROP_READ  | ATT_PROP_NTF,  0, 1, 1},
        [IDX_HID_REPORT4_REF]   = {ATT_RRD_UUID,  (uint8_t *)hid_ref4,     sizeof(hid_ref4),      ATT_PROP_READ, 0, 1, 0},
		[IDX_HID_REPORT4_DESC]  = {ATT_CCCD_UUID, (uint8_t *) &desc_rep4,  ATT_LEN_DESC,          ATT_PROP_WRITE | ATT_PROP_READ,  0, 1, 1, sizeof(uint16_t)},
        
        [IDX_HID_REPORT5_CHAR]  = {ATT_CHAR_UUID, NULL,                    0,                     ATT_PROP_READ, 0, 0, 0},
        [IDX_HID_REPORT5_VAL]   = {char_report,   NULL,  									 0,   ATT_PROP_READ  | ATT_PROP_WRITE  | ATT_PROP_NTF,  0, 1, 1},
        [IDX_HID_REPORT5_REF]   = {ATT_RRD_UUID,  (uint8_t *)hid_ref5,     sizeof(hid_ref5),      ATT_PROP_READ, 0, 1, 0},
        [IDX_HID_REPORT5_DESC]  = {ATT_CCCD_UUID, (uint8_t *) &desc_rep5,  ATT_LEN_DESC,          ATT_PROP_WRITE | ATT_PROP_READ,  0, 1, 1, sizeof(uint16_t)},
        
    };
  
void hid_service_init(void)
{
    static const gatt_serv_db_t service_db = { att_db, sizeof(att_db) / sizeof(att_db[0]) };
    gatt_database_add(&service_db, &m_start_handle);
    co_assert(m_start_handle);

    //co_evt_register(VOICE_ENCODER_EVT, hid_evt_callback);
}



void hid_evt_cb(ble_evt_t *p_evt)
{
    if (p_evt->header.evt_id == BLE_EVT_TX_COMPLETE) 
	{
     
    }
	else if (p_evt->header.evt_id == BLE_GAP_EVT_CONNECTED) 
	{
		desc_rep1 = 0;
		desc_rep2 = 0;
		desc_rep3 = 0;
		desc_rep4 = 0;
		desc_rep5 = 0;
	}
	else if (p_evt->header.evt_id == BLE_GATTS_EVT_WRITE) 
	{
		uint8_t* data = p_evt->evt.gatts_evt.params.write.data;
        uint16_t len = p_evt->evt.gatts_evt.params.write.len;
		
        switch (p_evt->evt.gatts_evt.params.write.handle - m_start_handle) 
        {
            case IDX_HID_REPORT1_DESC:
				desc_rep1 = 1;
				break;
			case IDX_HID_REPORT2_DESC:
				desc_rep2 = 1;
				break;
            case IDX_HID_REPORT3_DESC:
				desc_rep3 = 1;
				break;
            case IDX_HID_REPORT4_DESC:
				desc_rep4 = 1;
				break;
            case IDX_HID_REPORT5_DESC:
				desc_rep5 = 1;
				break;			
			case IDX_HID_REPORT5_VAL:
			{

			}
			break;
        }
    }
}


bool hid_cccd_is_enable(void)
{
	if (desc_rep1 && desc_rep2 && desc_rep3 && desc_rep4 && desc_rep5)
	{
		return true;
	}
	return false;
}

bool hid_gatt_report_notify(uint8_t rpt_info_id, uint8_t *p_data, uint16_t len)
{
	uint8_t pkg_num;

	if (!gap_get_connected_status())
	{
		return false;
	}
	
	if (!gap_encrypt_get()) {
        return false;
    }
	
    xh_ble_tx_buffer_count_get(&pkg_num);
    if (pkg_num <= 1 || len == 0) 
	{
		APP_LOG_ERR("rf fifo full or len err,send key fail\r\n");
        return false;
    } 
	else
	{
		uint16_t att_handle;
		att_handle = m_start_handle + rpt_info_id;
		
		ble_gatts_hvx_params_t hvx = {
			att_handle, BLE_GATT_HVX_NOTIFICATION, 0, &len, (uint8_t *)p_data
		};
		
		
		xh_ble_gatts_hvx(0, &hvx);
        
    }
	return true;
}


