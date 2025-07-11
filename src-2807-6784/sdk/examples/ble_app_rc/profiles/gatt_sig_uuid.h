#ifndef __GATT_SIG_UUID_H
#define __GATT_SIG_UUID_H


#include <stdio.h>
#include <stdint.h>


 // SIG UUID size is always 2.
#define ATT_BT_UUID_SIZE 2

/*
 * WARNING: The 16-bit UUIDs are assigned by the Bluetooth SIG and published
 *          in the Bluetooth Assigned Numbers page. Do not change these values.
 *          Changing them will cause Bluetooth interoperability issues.
 */

/**
 * GATT Services
 */
#define GAP_SERVICE_UUID                           0x1800 // Generic Access Profile
#define GATT_SERVICE_UUID                          0x1801 // Generic Attribute Profile

/**
 * GATT Declarations
 */
#define GATT_PRIMARY_SERVICE_UUID                  0x2800 // Primary Service
#define GATT_SECONDARY_SERVICE_UUID                0x2801 // Secondary Service
#define GATT_INCLUDE_UUID                          0x2802 // Include
#define GATT_CHARACTER_UUID                        0x2803 // Characteristic

/**
 * GATT Descriptors
 */
#define GATT_CHAR_EXT_PROPS_UUID                   0x2900 // Characteristic Extended Properties
#define GATT_CHAR_USER_DESC_UUID                   0x2901 // Characteristic User Description
#define GATT_CLIENT_CHAR_CFG_UUID                  0x2902 // Client Characteristic Configuration
#define GATT_SERV_CHAR_CFG_UUID                    0x2903 // Server Characteristic Configuration
#define GATT_CHAR_FORMAT_UUID                      0x2904 // Characteristic Presentation Format
#define GATT_CHAR_AGG_FORMAT_UUID                  0x2905 // Characteristic Aggregate Format
#define GATT_VALID_RANGE_UUID                      0x2906 // Valid Range
#define GATT_EXT_REPORT_REF_UUID                   0x2907 // External Report Reference Descriptor
#define GATT_REPORT_REF_UUID                       0x2908 // Report Reference Descriptor

/**
 * GATT Characteristics
 */
#define GATT_DEVICE_NAME_UUID                           0x2A00 // Device Name
#define GATT_APPEARANCE_UUID                            0x2A01 // Appearance
#define GATT_PERI_PRIVACY_FLAG_UUID                     0x2A02 // Peripheral Privacy Flag
#define GATT_RECONNECT_ADDR_UUID                        0x2A03 // Reconnection Address
#define GATT_PERI_CONN_PARAM_UUID                       0x2A04 // Peripheral Preferred Connection Parameters
#define GATT_SERVICE_CHANGED_UUID                       0x2A05 // Service Changed
#define GATT_ADDR_RESOL_SUPP                            0x2AA6 // Central Address Resolution supported
#define GATT_RSLV_PRIV_ADDR_ONLY                        0x2AC9 // Resolvable Private Address only

/*
 * WARNING: The 16-bit UUIDs are assigned by the Bluetooth SIG and published
 *          in the Bluetooth Assigned Numbers page. Do not change these values.
 *          Changing them will cause Bluetooth interoperability issues.
 */

/**
 * GATT Service UUIDs
 */
#define IMMEDIATE_ALERT_SERV_UUID       0x1802  // Immediate Alert
#define LINK_LOSS_SERV_UUID             0x1803  // Link Loss
#define TX_PWR_LEVEL_SERV_UUID          0x1804  // Tx Power
#define CURRENT_TIME_SERV_UUID          0x1805  // Current Time Service
#define REF_TIME_UPDATE_SERV_UUID       0x1806  // Reference Time Update Service
#define NEXT_DST_CHANGE_SERV_UUID       0x1807  // Next DST Change Service
#define GLUCOSE_SERV_UUID               0x1808  // Glucose
#define THERMOMETER_SERV_UUID           0x1809  // Health Thermometer
#define DEVINFO_SERV_UUID               0x180A  // Device Information
#define NWA_SERV_UUID                   0x180B  // Network Availability
#define HEARTRATE_SERV_UUID             0x180D  // Heart Rate
#define PHONE_ALERT_STS_SERV_UUID       0x180E  // Phone Alert Status Service
#define BATT_SERV_UUID                  0x180F  // Battery Service
#define BLOODPRESSURE_SERV_UUID         0x1810  // Blood Pressure
#define ALERT_NOTIF_SERV_UUID           0x1811  // Alert Notification Service
#define HID_SERV_UUID                   0x1812  // Human Interface Device
#define SCAN_PARAM_SERV_UUID            0x1813  // Scan Parameters
#define RSC_SERV_UUID                   0x1814  // Running Speed and Cadence
#define CSC_SERV_UUID                   0x1816  // Cycling Speed and Cadence
#define CYCPWR_SERV_UUID                0x1818  // Cycling Power
#define LOC_NAV_SERV_UUID               0x1819  // Location and Navigation
#define IPSS_SERV_UUID                  0x1820  // Internet Protocol Support
#define FITMACHINE_SERV_UUID            0x1826  // Fitmachine

/**
 * GATT Characteristic UUIDs
 */
#define ALERT_LEVEL_UUID                0x2A06  // Alert Level
#define TX_PWR_LEVEL_UUID               0x2A07  // Tx Power Level
#define DATE_TIME_UUID                  0x2A08  // Date Time
#define DAY_OF_WEEK_UUID                0x2A09  // Day of Week
#define DAY_DATE_TIME_UUID              0x2A0A  // Day Date Time
#define EXACT_TIME_256_UUID             0x2A0C  // Exact Time 256
#define DST_OFFSET_UUID                 0x2A0D  // DST Offset
#define TIME_ZONE_UUID                  0x2A0E  // Time Zone
#define LOCAL_TIME_INFO_UUID            0x2A0F  // Local Time Information
#define TIME_WITH_DST_UUID              0x2A11  // Time with DST
#define TIME_ACCURACY_UUID              0x2A12  // Time Accuracy
#define TIME_SOURCE_UUID                0x2A13  // Time Source
#define REF_TIME_INFO_UUID              0x2A14  // Reference Time Information
#define TIME_UPDATE_CTRL_PT_UUID        0x2A16  // Time Update Control Point
#define TIME_UPDATE_STATE_UUID          0x2A17  // Time Update State
#define GLUCOSE_MEAS_UUID               0x2A18  // Glucose Measurement
#define BATT_LEVEL_UUID                 0x2A19  // Battery Level
#define TEMP_MEAS_UUID                  0x2A1C  // Temperature Measurement
#define TEMP_TYPE_UUID                  0x2A1D  // Temperature Type
#define IMEDIATE_TEMP_UUID              0x2A1E  // Intermediate Temperature
#define MEAS_INTERVAL_UUID              0x2A21  // Measurement Interval
#define BOOT_KEY_INPUT_UUID             0x2A22  // Boot Keyboard Input Report
#define SYSTEM_ID_UUID                  0x2A23  // System ID
#define MODEL_NUMBER_UUID               0x2A24  // Model Number String
#define SERIAL_NUMBER_UUID              0x2A25  // Serial Number String
#define FIRMWARE_REV_UUID               0x2A26  // Firmware Revision String
#define HARDWARE_REV_UUID               0x2A27  // Hardware Revision String
#define SOFTWARE_REV_UUID               0x2A28  // Software Revision String
#define MANUFACTURER_NAME_UUID          0x2A29  // Manufacturer Name String
#define IEEE_11073_CERT_DATA_UUID       0x2A2A  // IEEE 11073-20601 Regulatory Certification Data List
#define CURRENT_TIME_UUID               0x2A2B  // Current Time
#define MAGN_DECLINE_UUID				0x2A2C  
#define DEVICE_TYPE_UUID				0x2A2D
#define APPID_UUID						0x2A2E
#define CMCC_MODEL_NUMBER_UUID			0x2A2F
#define SCAN_REFRESH_UUID               0x2A31  // Scan Refresh
#define BOOT_KEY_OUTPUT_UUID            0x2A32  // Boot Keyboard Output Report
#define BOOT_MOUSE_INPUT_UUID           0x2A33  // Boot Mouse Input Report
#define GLUCOSE_CONTEXT_UUID            0x2A34  // Glucose Measurement Context
#define BLOODPRESSURE_MEAS_UUID         0x2A35  // Blood Pressure Measurement
#define IMEDIATE_CUFF_PRESSURE_UUID     0x2A36  // Intermediate Cuff Pressure
#define HEARTRATE_MEAS_UUID             0x2A37  // Heart Rate Measurement
#define BODY_SENSOR_LOC_UUID            0x2A38  // Body Sensor Location
#define HEARTRATE_CTRL_PT_UUID          0x2A39  // Heart Rate Control Point
#define NETWORK_AVAIL_UUID              0x2A3E  // Network Availability
#define ALERT_STATUS_UUID               0x2A3F  // Alert Status
#define RINGER_CTRL_PT_UUID             0x2A40  // Ringer Control Point
#define RINGER_SETTING_UUID             0x2A41  // Ringer Setting
#define ALERT_CAT_ID_BMASK_UUID         0x2A42  // Alert Category ID Bit Mask
#define ALERT_CAT_ID_UUID               0x2A43  // Alert Category ID
#define ALERT_NOTIF_CTRL_PT_UUID        0x2A44  // Alert Notification Control Point
#define UNREAD_ALERT_STATUS_UUID        0x2A45  // Unread Alert Status
#define NEW_ALERT_UUID                  0x2A46  // New Alert
#define SUP_NEW_ALERT_CAT_UUID          0x2A47  // Supported New Alert Category
#define SUP_UNREAD_ALERT_CAT_UUID       0x2A48  // Supported Unread Alert Category
#define BLOODPRESSURE_FEATURE_UUID      0x2A49  // Blood Pressure Feature
#define HID_INFORMATION_UUID            0x2A4A  // HID Information
#define REPORT_MAP_UUID                 0x2A4B  // Report Map
#define HID_CTRL_PT_UUID                0x2A4C  // HID Control Point
#define REPORT_UUID                     0x2A4D  // Report
#define PROTOCOL_MODE_UUID              0x2A4E  // Protocol Mode
#define SCAN_INTERVAL_WINDOW_UUID       0x2A4F  // Scan Interval Window
#define PNP_ID_UUID                     0x2A50  // PnP ID
#define GLUCOSE_FEATURE_UUID            0x2A51  // Glucose Feature
#define RECORD_CTRL_PT_UUID             0x2A52  // Record Access Control Point
#define RSC_MEAS_UUID                   0x2A53  // RSC Measurement
#define RSC_FEATURE_UUID                0x2A54  // RSC Feature
#define SC_CTRL_PT_UUID                 0x2A55  // SC Control Point
#define CSC_MEAS_UUID                   0x2A5B  // CSC Measurement
#define CSC_FEATURE_UUID                0x2A5C  // CSC Feature
#define SENSOR_LOC_UUID                 0x2A5D  // Sensor Location
#define CYCPWR_MEAS_UUID                0x2A63  // Cycling Power Measurement
#define CYCPWR_VECTOR_UUID              0x2A64  // Cycling Power Vector
#define CYCPWR_FEATURE_UUID             0x2A65  // Cycling Power Feature
#define CYCPWR_CTRL_PT_UUID             0x2A66  // Cycling Power Control Point
#define LOC_SPEED_UUID                  0x2A67  // Location and Speed
#define NAV_UUID                        0x2A68  // Navigation
#define POS_QUALITY_UUID                0x2A69  // Position Quality
#define LN_FEATURE_UUID                 0x2A6A  // LN Feature
#define LN_CTRL_PT_UUID                 0x2A6B  // LN Control Point  

#define CMCC_SIGN_UUID                		0x4A01
#define CMCC_ANDLINK_TOKEN_UUID       		0x4A02
#define CMCC_ANDLINK_DEVICE_TYPE_UUID 		0x4A03
#define CMCC_OTHER_INFOS_UUID         		0x4A04



#define GATT_UUID_FITNESS_MACHINE_FEATURE           0x2ACC
#define GATT_UUID_TREADMILL_DATA                    0x2ACD //跑步机数据
#define GATT_UUID_CROSS_TRAINER_DATA                0x2ACE //椭圆机数据
#define GATT_UUID_ROWER_DATA                        0x2AD1 //划船机数据
#define GATT_UUID_INDOOR_BIKE_DATA                  0x2AD2 //单车数据
#define GATT_UUID_STEP_CLIMBER_DATA                 0x2ACF //攀岩机
#define GATT_UUID_STAIR_CLIMBER_DATA                0x2AD0  //爬楼梯机

#define GATT_UUID_TRAINING_STATUS                   0x2AD3
#define GATT_UUID_SUPPORT_SPEED_RANGE               0x2AD4
#define GATT_UUID_SUPPORT_INCLINATION_RANGE         0x2AD5
#define GATT_UUID_SUPPORT_RESISTANCE_LEVEL_RANGE    0x2AD6
#define GATT_UUID_SUPPORT_HEART_RATE_RANGE          0x2AD7
#define GATT_UUID_SUPPORT_POWER_RANGE               0x2AD8
#define GATT_UUID_FITNESS_MACHINE_CONTROL_POINT     0x2AD9
#define GATT_UUID_FITNESS_MACHINE_STATUS            0x2ADA

/**
 * GATT Unit UUIDs
 */
#define GATT_UNITLESS_UUID                    0x2700  // <Symbol>, <Expressed in terms of SI base units>
#define GATT_UNIT_LENGTH_METER_UUID           0x2701  // m, m
#define GATT_UNIT_MASS_KGRAM_UUID             0x2702  // kg, kg
#define GATT_UNIT_TIME_SECOND_UUID            0x2703  // s, s
#define GATT_UNIT_ELECTRIC_CURRENT_A_UUID     0x2704  // A, A
#define GATT_UNIT_THERMODYN_TEMP_K_UUID       0x2705  // K, K
#define GATT_UNIT_AMOUNT_SUBSTANCE_M_UUID     0x2706  // mol, mol
#define GATT_UNIT_LUMINOUS_INTENSITY_C_UUID   0x2707  // cd, cd

#define GATT_UNIT_AREA_SQ_MTR_UUID            0x2710  // m^2, m^2
#define GATT_UNIT_VOLUME_CUBIC_MTR_UUID       0x2711  // m^3, m^3
#define GATT_UNIT_VELOCITY_MPS_UUID           0x2712  // m/s, m s^-1
#define GATT_UNIT_ACCELERATION_MPS_SQ_UUID    0x2713  // m/s^2, m s^-2
#define GATT_UNIT_WAVENUMBER_RM_UUID          0x2714  // ? m^-1
#define GATT_UNIT_DENSITY_KGPCM_UUID          0x2715  // p, kg m^-3
#define GATT_UNIT_SURFACE_DENSITY_KGPSM_UUID  0x2716  // pA, kg m^-2
#define GATT_UNIT_SPECIFIC_VOLUME_CMPKG_UUID  0x2717  // v, m^3 kg^-1
#define GATT_UNIT_CURRENT_DENSITY_APSM_UUID   0x2718  // j, A m^-2
#define GATT_UNIT_MAG_FIELD_STRENGTH_UUID     0x2719  // H, A m
#define GATT_UNIT_AMOUNT_CONC_MPCM_UUID       0x271A  // c, mol m^-3
#define GATT_UNIT_MASS_CONC_KGPCM_UUID        0x271B  // c, kg m^-3
#define GATT_UNIT_LUMINANCE_CPSM_UUID         0x271C  // Lv, cd m^-2
#define GATT_UNIT_REFRACTIVE_INDEX_UUID       0x271D  // n, 1
#define GATT_UNIT_RELATIVE_PERMEABLILTY_UUID  0x271E  // u, 1
#define GATT_UNIT_PLANE_ANGLE_RAD_UUID        0x2720  // rad, m m-1
#define GATT_UNIT_SOLID_ANGLE_STERAD_UUID     0x2721  // sr, m2 m-2
#define GATT_UNIT_FREQUENCY_HTZ_UUID          0x2722  // Hz, s-1
#define GATT_UNIT_FORCE_NEWTON_UUID           0x2723  // N, m kg s-2
#define GATT_UNIT_PRESSURE_PASCAL_UUID        0x2724  // Pa, N/m2 = m2 kg s-2
#define GATT_UNIT_ENERGY_JOULE_UUID           0x2725  // J, N m = m2 kg s-2
#define GATT_UNIT_POWER_WATT_UUID             0x2726  // W, J/s = m2 kg s-3
#define GATT_UNIT_E_CHARGE_C_UUID             0x2727  // C, sA
#define GATT_UNIT_E_POTENTIAL_DIF_V_UUID      0x2728  // V, W/A = m2 kg s-3 A-1

#define GATT_UNIT_CELSIUS_TEMP_DC_UUID        0x272F  // oC, t/oC = T/K - 273.15

#define GATT_UNIT_TIME_MINUTE_UUID            0x2760  // min, 60 s
#define GATT_UNIT_TIME_HOUR_UUID              0x2761  // h, 3600 s
#define GATT_UNIT_TIME_DAY_UUID               0x2762  // d, 86400 s
#define GATT_UNIT_PLANE_ANGLE_DEGREE_UUID     0x2763  // o, (pi/180) rad
#define GATT_UNIT_PLANE_ANGLE_MINUTE_UUID     0x2764  // ', (pi/10800) rad
#define GATT_UNIT_PLANE_ANGLE_SECOND_UUID     0x2765  // '', (pi/648000) rad
#define GATT_UNIT_AREA_HECTARE_UUID           0x2766  // ha, 10^4 m^2
#define GATT_UNIT_VOLUME_LITRE_UUID           0x2767  // l, 10^-3 m^3
#define GATT_UNIT_MASS_TONNE_UUID             0x2768  // t, 10^3 kg

#define GATT_UINT_LENGTH_YARD_UUID            0x27A0  // yd, 0.9144 m
#define GATT_UNIT_LENGTH_PARSEC_UUID          0x27A1  // pc, 3.085678 ?1016 m
#define GATT_UNIT_LENGTH_INCH_UUID            0x27A2  // in, 0.0254 m
#define GATT_UNIT_LENGTH_FOOT_UUID            0x27A3  // ft, 0.3048 m
#define GATT_UNIT_LENGTH_MILE_UUID            0x27A4  // mi, 1609.347 m
#define GATT_UNIT_PRESSURE_PFPSI_UUID         0x27A5  // psi, 6.894757 ?103 Pa
#define GATT_UNIT_VELOCITY_KMPH_UUID          0x27A6  // km/h, 0.2777778 m^s-1
#define GATT_UNIT_VELOCITY_MPH_UUID           0x27A7  // mi/h, 0.44704 m^ s-1
#define GATT_UNIT_ANGULAR_VELOCITY_RPM_UUID   0x27A8  // r/min, 0.1047198 rad s-1
#define GATT_UNIT_ENERGY_GCAL_UUID            0x27A9  //
#define GATT_UNIT_ENERGY_KCAL_UUID            0x27AA  // kcal, 4190.02 J
#define GATT_UNIT_ENERGY_KWH_UUID             0x27AB  // kWh, 3600000 J
#define GATT_UNIT_THERMODYN_TEMP_DF_UUID      0x27AC  // oF, t/oF = T/K ?1.8 - 459.67
#define GATT_UNIT_PERCENTAGE_UUID             0x27AD  // %
#define GATT_UNIT_PER_MILE_UUID               0x27AE  //
#define GATT_UNIT_PERIOD_BPM_UUID             0x27AF  //
#define GATT_UNIT_E_CHARGE_AH_UUID            0x27B0  //
#define GATT_UNIT_MASS_DENSITY_MGPD_UUID      0x27B1  //
#define GATT_UNIT_MASS_DENSITY_MMPL_UUID      0x27B2  //
#define GATT_UNIT_TIME_YEAR_UUID              0x27B3  //
#define GATT_UNIT_TIME_MONTH_UUID             0x27B4  //



#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define UUID16_ARR(uuid16)  {uuid16&0xff,(uuid16&0xff00)>>8}
#define UUID128_ARR(uuid16)       {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, \
                                            0x00, 0x10, 0x00, 0x00, (uuid16 & 0xff),(uuid16&0xff00)>>8, 0x00, 0x00}


											
#endif //__GATT_SIG_UUID_H

