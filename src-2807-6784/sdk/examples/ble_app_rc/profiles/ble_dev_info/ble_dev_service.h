#ifndef __BLE_DEV_SERVICE_H
#define __BLE_DEV_SERVICE_H


#include "ble.h"


#define    DIS_MANUFACTURER_NAME_CHAR_SUP       0x0001	//Indicate if Manufacturer Name String Char. is supported
#define    DIS_MODEL_NB_STR_CHAR_SUP            0x0002	//Indicate if Model Number String Char. is supported
#define    DIS_SERIAL_NB_STR_CHAR_SUP           0x0004	//Indicate if Serial Number String Char. is supported
#define    DIS_HARD_REV_STR_CHAR_SUP            0x0008	//Indicate if Hardware Revision String Char. supports indications
#define    DIS_FIRM_REV_STR_CHAR_SUP            0x0010	//Indicate if Firmware Revision String Char. is writable
#define    DIS_SW_REV_STR_CHAR_SUP              0x0020	//Indicate if Software Revision String Char. is writable
#define    DIS_SYSTEM_ID_CHAR_SUP               0x0040	//Indicate if System ID Char. is writable
#define    DIS_IEEE_CHAR_SUP                    0x0080	//Indicate if IEEE Char. is writable
#define    DIS_PNP_ID_CHAR_SUP                  0x0100	//Indicate if PnP ID Char. is writable
#define    DIS_ALL_FEAT_SUP                     0x01FF	//All features are supported

#define DIS_FEATURES             (DIS_ALL_FEAT_SUP - DIS_IEEE_CHAR_SUP - DIS_SYSTEM_ID_CHAR_SUP)



// DIS Manufacturer Name String
#define DIS_MANU_STR 		"Seneasy"

// DIS Model Number String
#define DIS_MODEL_NUMBER  	"SRC-2807-6784"

// DIS Serial Number String
#define DIS_SERIAL_NUMBER 	"EU01"

// DIS Hardware Revision String
#define DIS_HARD_VERSION  	"V1.0"

// DIS Firmware Revision String
#define DIS_FIRM_VERSION  	"EU01-R76EBSYXHv.01.01"

// DIS Software Revision String
#define DIS_SOFT_VERSION  	"EU01-R76EBSYXHv.01.01"

// DIS IEEE 11073-20601 Regulatory Certification Data List
#define DIS_2A2A          	"\xFF\xEE\xDD\xCC\xBB\xA"

// DIS SYSTEM ID
#define DIS_SYSTEM_ID     	"\x00\x00\x01\x02\x00\x03\x04\x05"

// DIS PNP ID
#define DIS_PNP_ID        	"\x02\x8A\x24\x66\x82\x01\x00"


enum {
    IDX_DIS_SVC,
#if ((DIS_FEATURES & DIS_MANUFACTURER_NAME_CHAR_SUP) == DIS_MANUFACTURER_NAME_CHAR_SUP)
    IDX_MANU_CHAR,
    IDX_MANU_VAL,
#endif
	
#if ((DIS_FEATURES & DIS_MODEL_NB_STR_CHAR_SUP) == DIS_MODEL_NB_STR_CHAR_SUP)	
    IDX_MODEL_CHAR,
    IDX_MODEL_VAL,
#endif
	
#if ((DIS_FEATURES & DIS_SERIAL_NB_STR_CHAR_SUP) == DIS_SERIAL_NB_STR_CHAR_SUP)	
    IDX_SERIAL_CHAR,
    IDX_SERIAL_VAL,
#endif
	
#if ((DIS_FEATURES & DIS_HARD_REV_STR_CHAR_SUP) == DIS_HARD_REV_STR_CHAR_SUP)    
	IDX_H_REV_CHAR,
    IDX_H_REV_VAL,
#endif
	
#if ((DIS_FEATURES & DIS_FIRM_REV_STR_CHAR_SUP) == DIS_FIRM_REV_STR_CHAR_SUP)	
	IDX_F_REV_CHAR,
    IDX_F_REV_VAL,
#endif

#if ((DIS_FEATURES & DIS_SW_REV_STR_CHAR_SUP) == DIS_SW_REV_STR_CHAR_SUP)
	IDX_S_REV_CHAR,
    IDX_S_REV_VAL,
#endif

#if ((DIS_FEATURES & DIS_SYSTEM_ID_CHAR_SUP) == DIS_SYSTEM_ID_CHAR_SUP)
	IDX_SYSID_CHAR,
    IDX_SYSID_VAL,
#endif

#if ((DIS_FEATURES & DIS_IEEE_CHAR_SUP) == DIS_IEEE_CHAR_SUP)
	IDX_2A2A_CHAR,
    IDX_2A2A_VAL,
#endif

#if ((DIS_FEATURES & DIS_PNP_ID_CHAR_SUP) == DIS_PNP_ID_CHAR_SUP)
	IDX_PNPID_CHAR,
    IDX_PNPID_VAL,
#endif
};

void ble_dev_service_init(void);

#endif //__BLE_DEV_SERVICE_H
