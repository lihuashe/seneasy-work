/********************************************************************************************************
 * @file     hci_event.h
 *
 * @brief    This is the header file for BLE SDK
 *
 * @author	 BLE GROUP
 * @date         12,2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#ifndef HCI_EVENT_H_
#define HCI_EVENT_H_


#include "stack/ble/ble_common.h"


/**
 *  @brief  Definition for general HCI event packet
 */
typedef struct {
	u8	 	   type;
	u8         eventCode;
	u8         paraLen;
	u8         parameters[1];
} hci_event_t;


typedef struct {
	u8         numHciCmds;
	u8         opCode_OCF;
	u8		   opCode_OGF;
	u8         returnParas[1];
} hci_cmdCompleteEvt_evtParam_t;


typedef struct {
	u8         status;
	u8         numHciCmds;
	u8         opCode_OCF;
	u8		   opCode_OGF;
} hci_cmdStatusEvt_evtParam_t;

typedef struct{
	u16 		connHandle;
	u16 		numOfCmpPkts;
}numCmpPktParamRet_t;

typedef struct {
	u8         numHandles;
	numCmpPktParamRet_t retParams[1];//TODO
} hci_numOfCmpPktEvt_t;

typedef struct{
	u8  status;
	u16 connHandle;
	u8  verNr;
	u16 compId;
	u16 subVerNr;
}hci_readRemVerInfoCmplEvt_t;

typedef struct {
	u8		   type;
	u8         eventCode;
	u8         paraLen;
	u8         subEventCode;
	u8         parameters[1];
} hci_le_metaEvt_t;



/**
 *  @brief  Event Parameters for "7.7.5 Disconnection Complete event"
 */
typedef struct {
	u8         status;
	u16        connHandle;
	u8         reason;
} hci_disconnectionCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.8 Encryption Change event"
 */
typedef struct {
	u8	status;
	u16	handle;
	u8  enc_enable;
} event_enc_change_t;

typedef struct {
	u8         status;
	u16        connHandle;
	u8         encryption_enable;
} hci_le_encryptEnableEvt_t;

/**
 *  @brief  Event Parameters for "7.7.39 Encryption Key Refresh Complete event"
 */
typedef struct {
	u8	status;
	u16	handle;
} event_enc_refresh_t;

/**
 *  @brief  Definition for HCI Encryption Key Refresh Complete event
 */
typedef struct {
	u8         status;
	u16        connHandle;
} hci_le_encryptKeyRefreshEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.1 LE Connection Complete event"
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u8         role;
	u8         peerAddrType;
	u8         peerAddr[6];
	u16        connInterval;
	u16        slaveLatency;
	u16        supervisionTimeout;
	u8         masterClkAccuracy;
} hci_le_connectionCompleteEvt_t;


/* ACL Connection Role */
typedef enum {
	LL_ROLE_MASTER 	= 0,
	LL_ROLE_SLAVE 	= 1,
} acl_connection_role_t;

/**
 *  @brief  Event Parameters for "7.7.65.2 LE Advertising Report event"
 */
typedef struct {
	u8	subcode;
	u8	nreport;
	u8	event_type;
	u8	adr_type;
	u8	mac[6];
	u8	len;
	u8	data[1];
} event_adv_report_t;

/* Advertise report event type */
typedef enum {
	ADV_REPORT_EVENT_TYPE_ADV_IND 		= 0x00,
	ADV_REPORT_EVENT_TYPE_DIRECT_IND 	= 0x01,
	ADV_REPORT_EVENT_TYPE_SCAN_IND 		= 0x02,
	ADV_REPORT_EVENT_TYPE_NONCONN_IND 	= 0x03,
	ADV_REPORT_EVENT_TYPE_SCAN_RSP 		= 0x04,
} advReportEventType_t;



/**
 *  @brief  Event Parameters for "7.7.65.3 LE Connection Update Complete event"
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u16        connInterval;
	u16        connLatency;
	u16        supervisionTimeout;
} hci_le_connectionUpdateCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.4 LE Read Remote Features Complete event"
 */
#define LL_FEATURE_SIZE                                      			8
typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u8		   feature[LL_FEATURE_SIZE];
} hci_le_readRemoteFeaturesCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.5 LE Long Term Key Request event"
 */
typedef struct {
	u8         subEventCode;
	u16        connHandle;
	u8         random[8];
	u16        ediv;
} hci_le_longTermKeyRequestEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.6 LE Remote Connection Parameter Request event"
 */
typedef struct {
	u8         subEventCode;
	u16        connHandle;
	u16        IntervalMin;
	u16        IntervalMax;
	u16		   latency;
	u16		   timeout;
} hci_le_remoteConnParamReqEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.7 LE Data Length Change event"
 */
typedef struct {
	u8         subEventCode;
	u16        connHandle;  //no aligned, can not be used as pointer
	u16  	   maxTxOct;
	u16		   maxTxtime;
	u16  	   maxRxOct;
	u16		   maxRxtime;
} hci_le_dataLengthChangeEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.8 LE Read Local P-256 Public Key Complete event"
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u8         localP256Key[64];
} hci_le_readLocalP256KeyCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.9 LE Generate DHKey Complete event"
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u8         DHKey[32];
} hci_le_generateDHKeyCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.10 LE Enhanced Connection Complete event"
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u16		   connHandle;
	u8		   role;
	u8		   PeerAddrType;
	u8         PeerAddr[6];
	u8         localRslvPrivAddr[6];
	u8         Peer_RslvPrivAddr[6];
	u16        connInterval;
	u16        connLatency;
	u16        superTimeout;
	u8         masterClkAccuracy;
} hci_le_enhancedConnCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.12 LE PHY Update Complete event"
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u8 		   tx_phy;
	u8		   rx_phy;
} hci_le_phyUpdateCompleteEvt_t;



/**
 *  @brief  Event Parameters for "7.7.65.13 LE Extended Advertising Report event"
 */
typedef struct{
	u16		event_type;			// 2
	u8		address_type;       // 1
	u8		address[6];			// 6
	u8		primary_phy;		// 1
	u8		secondary_phy;		// 1
	u8		advertising_sid;	// 1
	u8		tx_power;			// 1
	u8		rssi;				// 1
	u16		perd_adv_inter;		// 2   Periodic_Advertising_Interval
	u8		direct_address_type;// 1
	u8		direct_address[6];	// 6
	u8		data_length;		// 1
	u8		data[1];
} extAdvEvt_info_t;

typedef struct{
	u8	 sub_code;
	u16  sync_handle;
	u8   tx_power;

	u8   rssi;
	u8   unused;
	u8   data_status;
	u8   data_len;

	u8   data[1];
}le_periodAdvReportEvt_t;

#define EXTADV_INFO_LENGTH				24	 //byte number from "event_type" to "data_length"
#define EXTADV_RPT_DATA_LEN_MAX			229  //253 - 24 = 229

typedef struct{
	u8		subEventCode;
	u8		num_reports;
	u8		advEvtInfo[1];
} hci_le_extAdvReportEvt_t;


/* Extended Advertising Report Event Event_Type mask*/
typedef enum{
	EXTADV_RPT_EVT_MASK_CONNECTABLE					=	BIT(0),
	EXTADV_RPT_EVT_MASK_SCANNABLE     				=	BIT(1),
	EXTADV_RPT_EVT_MASK_DIRECTED  				    =   BIT(2),
	EXTADV_RPT_EVT_MASK_SCAN_RESPONSE     			=	BIT(3),
	EXTADV_RPT_EVT_MASK_LEGACY          			=	BIT(4),

	EXTADV_RPT_DATA_COMPLETE     					=	0x00,
	EXTADV_RPT_DATA_INCOMPLETE_MORE_TO_COME         =	0x20,
	EXTADV_RPT_DATA_INCOMPLETE_TRUNCATED          	=	0x40,
	EXTADV_RPT_DATA_RFU				 	          	=	0x60,

	EXTADV_RPT_EVTTYPE_MASK		         			=	0x1F,
	EXTADV_RPT_DATA_STATUS_MASK		         		=	0x60,
}extAdvRptEvtMask_t;


/* Extended Advertising Report Event_Type */
typedef enum{
	//Legacy
	EXTADV_RPT_EVTTYPE_LEGACY_ADV_IND 				       		= 0x0013,		//  0001 0011'b
	EXTADV_RPT_EVTTYPE_LEGACY_ADV_DIRECT_IND			       	= 0x0015,		//  0001 0101'b
	EXTADV_RPT_EVTTYPE_LEGACY_ADV_SCAN_IND	 					= 0x0012,		//  0001 0010'b
	EXTADV_RPT_EVTTYPE_LEGACY_ADV_NONCONN_IND					= 0x0010,		//  0001 0000'b
	EXTADV_RPT_EVTTYPE_LEGACY_SCAN_RSP_2_ADV_IND				= 0x001B,		//  0001 1011'b
	EXTADV_RPT_EVTTYPE_LEGACY_SCAN_RSP_2_ADV_SCAN_IND			= 0x001A,		//  0001 1010'b

	//Extended
	EXTADV_RPT_EVTTYPE_EXT_NON_CONN_NON_SCAN_UNDIRECTED    	    = 0x0000,	// Extended, Non_Connectable Non_Scannable Undirected
	EXTADV_RPT_EVTTYPE_EXT_CONNECTABLE_UNDIRECTED       		= 0x0001,	// Extended, Connectable Undirected
	EXTADV_RPT_EVTTYPE_EXT_SCANNABLE_UNDIRECTED					= 0x0002,	// Extended, Scannable Undirected
	EXTADV_RPT_EVTTYPE_EXT_NON_CONN_NON_SCAN_DIRECTED			= 0x0004,	// Extended, Non_Connectable Non_Scannable Directed
	EXTADV_RPT_EVTTYPE_EXT_CONNECTABLE_DIRECTED			       	= 0x0005,	// Extended, Connectable Directed
	EXTADV_RPT_EVTTYPE_EXT_SCANNABLE_DIRECTED					= 0x0006,	// Extended, Scannable Directed
	EXTADV_RPT_EVTTYPE_EXT_SCAN_RESPONSE						= 0x0008,	// Extended, AUX_SCAN_RESPONSE
}extAdvRptEvtType_t;  //extended advertising report event type




/* Address type */
typedef enum{
	EXTADV_RPT_PUBLIC_DEVICE_ADDRESS				=	0x00,
	EXTADV_RPT_RANDOM_DEVICE_ADDRESS     			=	0x01,
	EXTADV_RPT_PUBLIC_IDENTITY_ADDRESS  			=   0x02,
	EXTADV_RPT_RANDOM_IDENTITY_ADDRESS     			=	0x03,
	EXTADV_RPT_ANONYMOUS_ADV     					=	0xFF,
}ext_adv_adr_type_t;


#define	PERIODIC_ADV_INTER_NO_PERIODIC_ADV							0
#define	SECONDARY_PHY_NO_PACKET_ON_SECONDARY_ADV_CHN				0
#define	ADVERTISING_SID_NO_ADI_FIELD								0xFF
#define TX_POWER_INFO_NOT_AVAILABLE									127



/**
 *  @brief  Event Parameters for "7.7.65.14 LE Periodic Advertising Sync Established event"
 */
typedef struct {
	u8		subEventCode;
	u8		status;
	u16		syncHandle;
//	extadv_id_t	adverting_id;
	u8		advSID;
	u8		advAddrType;
	u8		advAddr[6];
	u8		advPHY;
	u16		perdAdvItvl;
	u8		advClkAccuracy;
}hci_le_periodicAdvSyncEstablishedEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.15 LE Periodic Advertising Report event"
 */
typedef struct {
	u8		subEventCode;
	u16		syncHandle;
	u8		txPower;
	u8		RSSI;
	u8		cteType;
	u8		dataStatus;
	u8		dataLength;  // 0 to 247 Length of the Data field
	u8		data[1];
} hci_le_periodicAdvReportEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.16 LE Periodic Advertising Sync Lost event"
 */
typedef struct {
	u8		subEventCode;
	u16		syncHandle;
} hci_le_periodicAdvSyncLostEvt_t;



typedef struct {
	u8         subEventCode;
} hci_le_scanTimeoutEvt_t;




/**
 *  @brief  Event Parameters for "7.7.65.18 LE Advertising Set Terminated event"
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u8         advHandle;
	u16		   connHandle;
	u8		   num_compExtAdvEvt;
} hci_le_advSetTerminatedEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.19 LE Scan Request Received event"
 */
typedef struct {
	//TODO
} hci_le_scanReqRcvdEvt_t;


/**
 *  @brief  Event Parameters for "7.7.65.20 LE Channel Selection Algorithm event"
 */
typedef struct {
	u8         subEventCode;
	u16        connHandle;
	u8		   channel_selection_algotihm;
} hci_le_chnSelectAlgorithmEvt_t;





/**
 * @brief     The Disconnection Complete event occurs when a connection is terminated.
 * @param[in]     status  							0x00 successfully completed.
 * 																	0x01 �C 0xFF failed to complete. [Vol 2] Part D, Error Codes.
 * @param[in]	  connHandle 				Connection_Handle
 * @param[in]	  reason  							Reason for disconnection. See [Vol 2] Part D, Error Codes.
 * @return			  1,0,-1
 */
int hci_disconnectionComplete_evt(u8 status, u16 connHandle, u8 reason);

/**
 * @brief      The Command Complete event is used by the Controller for most commands
 * 					  to transmit return status of a command and the other event parameters that are
 * 					  specified for the issued HCI command.
 * @param[in]     paraLen  						length of parameter
 * @param[in]	  para 								Pointer of parameter
 * @param[in]	  opCode_ogf  				OpCode Group Field
 * @param[in]	  numHciCmds 				The Number of HCI command packets which are allowed to be sent to the
 * 																	Controller from the Host.
 * @param[in]	  result  							event result
 * @return			  1,0,-1
 */
int hci_cmdComplete_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 paraLen, u8 *para, u8 *result);

/**
 * @brief      The Command Status event is used to indicate that the command described by
 * 					  the Command_Opcode parameter has been received, and that the Controller
 * 					  is currently performing the task for this command.
 * @param[in]     status  							0x00 successfully completed.
 * 																	0x01 �C 0xFF failed to complete. [Vol 2] Part D, Error Codes.
 * @param[in]	  opCode_ocf 				OpCode Command Field
 * @param[in]	  opCode_ogf  				OpCode Group Field
 * @param[in]	  numHciCmds 				The Number of HCI command packets which are allowed to be sent to the
 * 																	Controller from the Host.
 * @param[in]	  result  							event result
 * @return			  1,0,-1
 */
void hci_cmdStatus_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 status, u8 *result);

/**
 * @brief       The LE Connection Complete event indicates to both of the Hosts forming the connection that a new connection has been created.
 * @param[in]     status  							0x00 successfully completed.
 * 																	0x01 �C 0xFF failed to complete. [Vol 2] Part D, Error Codes.
 * @param[in]	  connHandle 				Connection_Handle
 * @param[in]	  role  								0x00 Connection is master, 0x01 Connection is slave
 * @param[in]	  peerAddrType 			0x00 Peer is using a Public Device Address, 0x01 Peer is using a Random Device Address
 * @param[in]	  peerAddr  					Public Device Address or Random Device Address of the peer device
 * @param[in]	  connInterval  				Connection interval
 * @param[in]	  slaveLatency  				Slave latency
 * @param[in]	  supervisionTimeout  	Connection supervision timeout
 * @param[in]	  masterClkAccuracy 		Master Clock Accuracy
 * @return			  1,0,-1
 */
int hci_le_connectionComplete_evt(u8 status, u16 connHandle, u8 role, u8 peerAddrType, u8 *peerAddr,
                                   u16 connInterval, u16 slaveLatency, u16 supervisionTimeout, u8 masterClkAccuracy);

/**
 * @brief       The LE Connection Update Complete event is used to indicate that the Controller process to update the connection has completed.
 * @param[in]     status  							0x00 successfully completed.
 * 																	0x01 �C 0xFF failed to complete. [Vol 2] Part D, Error Codes.
 * @param[in]	  connHandle 				Connection_Handle
 * @param[in]	  connInterval  				Connection interval
 * @param[in]	  connLatency  				Slave latency
 * @param[in]	  supervisionTimeout  	Connection supervision timeout
 * @return			  1,0,-1
 */
int hci_le_connectionUpdateComplete_evt(u8 status, u16 connHandle, u16 connInterval,
                                         u16 connLatency, u16 supervisionTimeout);


/**
 * @brief       The LE Read Remote Features Complete event is used to indicate the
 * 					   completion of the process of the Controller obtaining the features used
 * 					   on the connection and the features supported by the remote Bluetooth
 * 					   device specified by the Connection_Handle event parameter.
 * @param[in]     status  							0x00 successfully completed.
																	0x01 �C 0xFF failed to complete. [Vol 2] Part D, Error Codes.
 * @param[in]	  connHandle 				Connection_Handle
 * @param[in]	  feature  						LE features. See [Vol 6] Part B, Section 4.6.
 * @return			  1,0,-1
 */
int hci_le_readRemoteFeaturesComplete_evt(u8 status, u16 connHandle, u8 *feature);



/**
 * @brief       The LE PHY Update Complete Event is used to indicate that the Controller has
 * 					   changed the transmitter PHY or receiver PHY in use.
 * @param[in]     status  							0x00 successfully completed.
																	0x01 �C 0xFF failed to complete. [Vol 2] Part D, Error Codes.
 * @param[in]	  connHandle 				Connection_Handle
 * @param[in]	  new_phy  						0x01 The receiver PHY for the connection is LE 1M
 * 																	0x02 The receiver PHY for the connection is LE 2M
 * @return			  1,0,-1
 */
int hci_le_phyUpdateComplete_evt(u16 connhandle,u8 status, u8 new_phy);

/**
 * @brief       The LE Long Term Key Request event indicates that the master device is
 * 					  attempting to encrypt or re-encrypt the link and is requesting the Long
 * 					  Term Key from the Host.
 * @param[in]	  connHandle 				Connection_Handle
 * @param[in]	  random  						64-bit random number.
 * @param[in]	  ediv  								16-bit encrypted diversifier
 * @param[in]	  result  							event result
 * @return			  1,0,-1
 */
int hci_le_longTermKeyRequest_evt(u16 connHandle, u8 *random, u16 ediv, u8 *result);

/**
 * @brief       This event is generated when local P-256 key generation is complete.
 * @param[in]     status  							0x00 successfully completed.
																	0x01 �C 0xFF failed to complete. [Vol 2] Part D, Error Codes.
 * @param[in]	  localP256Key 				Local P-256 public key.
 * @return			  1,0,-1
 */
int hci_le_readLocalP256KeyComplete_evt(u8* localP256Key, u8 status);

/**
 * @brief       This event indicates that LE Diffie Hellman key generation has been completed
 * 				   	   by the Controller.
 * @param[in]     status  							0x00 successfully completed.
 * 																	0x01 �C 0xFF failed to complete. [Vol 2] Part D, Error Codes.
 * @param[in]	  DHkey 							Diffie Hellman Key.
 * @return			  1,0,-1
 */
int hci_le_generateDHKeyComplete_evt(u8* DHkey, u8 status);


/**
 * @brief       The Encryption Change event is used to indicate that the change of the
 * 					   encryption mode has been completed.
 * @param[in]	  connHandle 				Connection Handle
 * @param[in]	  encrypt_en					0x00 Link Level Encryption is OFF.
 * 																	0x01 Link Level Encryption is ON with E0 for BR/EDR.
 * 																			 Link Level Encryption is ON with AES-CCM for LE.
 * 																	0x02 Link Level Encryption is ON with AES-CCM for BR/EDR.
 * @return			  1,0,-1
 */
int hci_le_encryptChange_evt(u16 connhandle, u8 encrypt_en);


/**
 * @brief       The Encryption Key Refresh Complete event is used to indicate to the Host
 * 					  that the encryption key was refreshed on the given Connection_Handle any
 * 					  time encryption is paused and then resumed.
 * @param[in]	  connHandle 				Connection Handle
 * @return			  1,0,-1
 */
int hci_le_encryptKeyRefresh_evt(u16 connhandle);


#endif /* HCI_EVENT_H_ */





