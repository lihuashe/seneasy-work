/**
 * @file     ble_event.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/11/1
 * @history
 * @note     ble_peripheral ble event
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"
#include "err_def.h"

#include "llc_spec_5_1.h"
#include "llc_llp_interface.h"
#include "hci.h"
#include "gap_def.h"
#include "gap_api.h"
#include "gatt_client_api.h"

#include "ble_cb_event_def.h"
#include "ble_callback.h"

#include "app_cfg.h"
#include "utility.h"
#include "mlog.h"

#include "ble_event.h"

#define FILTER_LOCALNAME            ( 1 )
#define FILTER_ADDR_RSSI            ( 0 )

/**
 * @brief  LOCAL VARIABLES
 */
static uint16_t m_u16ConnHandle = 0;

static ble_event_callback_t ble_event_callback;

static bool bIsServicesFound = false;

static bool bIsBonded = false;
static st_pair_complete_event m_stPairParam;
static uint8_t  m_u8BlePeerMac[6] = BLE_SCAN_PEER_MAC;
static char  m_cBlePeerLocalName[] = BLE_SCAN_PEER_LOCAL_NAME;
static uint8_t  u8scanReportBuf[256];
typedef struct
{
    uint8_t u8Address[6];
    int8_t s8Rssi;
}stConnSlaveParam_t;
static stConnSlaveParam_t gstConnSlaveParam;

static void ble_event_report(uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen)
{
    if(ble_event_callback)
    {
        ble_event_callback(u8Event, pu8Data, u16DataLen);
    }
}

/**
 * @brief  BLE event advertising report.
 * @param  pu8Buf: Pointer to the save peer device advertising data.
 * @note   LE Advertising Report event package formart:
 *         - Subevent_Code,
 *         - Report Num, 1Byte
 *         - ADV Event Type, 1Byte
 *         - ADV Address Type, 1Byte
 *         - ADV Address, 6Bytes
 *         - ADV Data Length, 1Byte
 *         - ADV Data, n Bytes
 *         - RSSI, 1Byte
 * @param  u32BufSize: Advertising data length.
 */
static void ble_event_adv_report(uint8_t *pu8Buf, uint32_t u32BufSize)
{
    uint32_t i;
    uint8_t  u8ReportsNum = 0;
    uint8_t  u8ParametersCurrentLength = 2;
    uint8_t  u8NextReportOffset = 2;
    uint16_t u16ReportSize;
    uint8_t  u8AdvDataLen;
    bool bIsCreatConnect = false;


    // 1. Check adv report buffer and buffer length.
    if ((NULL == pu8Buf) || (u32BufSize < 2))
    {
        PRINTF("u32BufSize[<2] is Error!\n");
        return;
    }

    // 2. Check adv report number.
    u8ReportsNum = pu8Buf[1];
    if (0 == u8ReportsNum)
    {
        PRINTF("u8ReportsNum[=0] is Error\n");
        return;
    }

    // 3. Check adv report.
    for (i = 0; i < u8ReportsNum; i++)
    {
        // EventType(1) + AddressType(1) + Address(6) + AdvDataLength(1) + Data(variable) + RSSI(1)
        u8AdvDataLen = pu8Buf[u8NextReportOffset + 8];
        u16ReportSize = 1 + 1 + 6 + 1 + u8AdvDataLen + 1;

        u8ParametersCurrentLength += u16ReportSize;
        u8NextReportOffset += u16ReportSize;
    }

    if (u8ParametersCurrentLength != u32BufSize)
    {
        // Invalid adv report
        PRINTF("[u8ParametersCurrentLength != u32BufSize] is Error\n");
        return;
    }

    // 4. Parse and report adv.
    for (u8NextReportOffset = 2; u8NextReportOffset < u32BufSize; u8NextReportOffset += u16ReportSize)
    {
        u8AdvDataLen = pu8Buf[u8NextReportOffset + 8];
        u16ReportSize = 1 + 1 + 6 + 1 + u8AdvDataLen + 1;

        // 4.1 Adv event type.
        EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_EVENT_TYPE_T enAdvEventType = pu8Buf[u8NextReportOffset];

        // 4.2 Adv address Type.
        EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T enAdvAddrType = pu8Buf[u8NextReportOffset + 1];

        // 4.3 Adv address
        uint8_t *pu8Address = &pu8Buf[u8NextReportOffset + 2];
        uint8_t *pu8AddrPeer = &pu8Buf[u8NextReportOffset + 5];

        // 4.4 Adv data
        uint8_t *pu8Data = &pu8Buf[u8NextReportOffset + 9];

        // 4.5 RSSI
        int8_t s8Rssi = (int8_t)(pu8Buf[u8NextReportOffset + 9 + u8AdvDataLen]);

        //bound device mac
        if(bIsBonded)
        {
            if(0 == memcmp(m_stPairParam.peer_addr, pu8Address, sizeof(m_stPairParam.peer_addr)/sizeof(m_stPairParam.peer_addr[0])))
            {
                // 4.7 Disable scan.
                rom_gap_api_set_scan_enable(false, true);
                PRINTF("bound device adv, stop scan\n");
                bIsCreatConnect = true;
            }
        }
//        PRINTF("Scan Report Event: \n");
//        PRINTF("EventType: %02X\n", enAdvEventType);
//        PRINTF("AddrType:  %02X\n", enAdvAddrType);
//        PRINTF("Address:   %02X: %02X: %02X: %02X: %02X: %02X\n",
//               u8scanReportBuf[2], u8scanReportBuf[3], u8scanReportBuf[4],
//               u8scanReportBuf[5], u8scanReportBuf[6], u8scanReportBuf[7]);
//        PRINTF("RSSI:      %d\n", s8Rssi);
//        PRINTF("DataLen:   %d\n", u8AdvDataLen);

#if FILTER_LOCALNAME
// 4.6 Check local name (Scan response)
        if(LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_EVENT_TYPE_SCAN_RSP == enAdvEventType)
        {
            uint8_t u8AdvDataAdTypeLengthIndex = 0;
            uint8_t u8AdvDataAdTypeLength = 0;
            
            uint8_t u8AdvDataAdTypeNameStartIndex = 0;
            uint8_t u8AdvDataAdTypeNameLength = 0;
            
            uint8_t u8PraseAdvDataRemainLength = u8AdvDataLen;
            while(u8PraseAdvDataRemainLength)
            {
                u8AdvDataAdTypeLength = pu8Data[u8AdvDataAdTypeLengthIndex];
                if(GAP_ADTYPE_LOCAL_NAME_COMPLETE == pu8Data[u8AdvDataAdTypeLengthIndex + 1])
                {
                    u8AdvDataAdTypeNameStartIndex = u8AdvDataAdTypeLengthIndex+2;
                    u8AdvDataAdTypeNameLength = u8AdvDataAdTypeLength - 1;
                }
                u8AdvDataAdTypeLengthIndex = u8AdvDataAdTypeLengthIndex + u8AdvDataAdTypeLength+1;
                u8PraseAdvDataRemainLength -= (u8AdvDataAdTypeLength + 1);
            }
            
            if(u8AdvDataAdTypeNameLength == strlen(m_cBlePeerLocalName))
            {
                if(0 == memcmp(&pu8Data[u8AdvDataAdTypeNameStartIndex], m_cBlePeerLocalName, strlen(m_cBlePeerLocalName)))
                {
                    u8scanReportBuf[0] = enAdvEventType;
                    u8scanReportBuf[1] = enAdvAddrType;
                    memcpy(&u8scanReportBuf[2], pu8Address, 6);
                    u8scanReportBuf[8] = s8Rssi;
                    u8scanReportBuf[9] = u8AdvDataLen;
                    memcpy(&u8scanReportBuf[10], pu8Data, u8AdvDataLen);

                    PRINTF("Scan Report Event: \n");
                    PRINTF("EventType: %02X\n", enAdvEventType);
                    PRINTF("AddrType:  %02X\n", enAdvAddrType);
                    PRINTF("Address:   %02X: %02X: %02X: %02X: %02X: %02X\n",
                           u8scanReportBuf[2], u8scanReportBuf[3], u8scanReportBuf[4],
                           u8scanReportBuf[5], u8scanReportBuf[6], u8scanReportBuf[7]);
                    PRINTF("RSSI:      %d\n", s8Rssi);
                    PRINTF("DataLen:   %d\n", u8AdvDataLen);

                    // 4.7 Disable scan.
                    rom_gap_api_set_scan_enable(false, false);

                    // 4.8 Creat connect.
                    bIsCreatConnect = true;
                }
            }
        }
#elif FILTER_ADDR_RSSI
        // 4.6 Check adv address.
        if (0 == memcmp(pu8AddrPeer, m_u8BlePeerMac, sizeof(m_u8BlePeerMac)))
        {
            if(gstConnSlaveParam.u8Address[5] != m_u8BlePeerMac[2])
            {
                memcpy(gstConnSlaveParam.u8Address, pu8Address, 6);
                gstConnSlaveParam.s8Rssi = s8Rssi;
            }
            else 
            {
                if(0 == memcmp(gstConnSlaveParam.u8Address, pu8Address, 6))
                {
                    u8scanReportBuf[0] = enAdvEventType;
                    u8scanReportBuf[1] = enAdvAddrType;
                    memcpy(&u8scanReportBuf[2], pu8Address, 6);
                    u8scanReportBuf[8] = s8Rssi;
                    u8scanReportBuf[9] = u8AdvDataLen;
                    memcpy(&u8scanReportBuf[10], pu8Data, u8AdvDataLen);

                    PRINTF("Scan Report Event: \n");
                    PRINTF("EventType: %02X\n", enAdvEventType);
                    PRINTF("AddrType:  %02X\n", enAdvAddrType);
                    PRINTF("Address:   %02X: %02X: %02X: %02X: %02X: %02X\n",
                           u8scanReportBuf[2], u8scanReportBuf[3], u8scanReportBuf[4],
                           u8scanReportBuf[5], u8scanReportBuf[6], u8scanReportBuf[7]);
                    PRINTF("RSSI:      %d\n", s8Rssi);
                    PRINTF("DataLen:   %d\n", u8AdvDataLen);

                    // 4.7 Disable scan.
                    rom_gap_api_set_scan_enable(false, false);

                    // 4.8 Creat connect.
                    bIsCreatConnect = true;
                }
                else
                {
                    if(gstConnSlaveParam.s8Rssi < s8Rssi)
                    {
                        memcpy(gstConnSlaveParam.u8Address, pu8Address, 6);
                        gstConnSlaveParam.s8Rssi = s8Rssi;
                    }
                }
            }
        }
#else

#endif
        if(bIsCreatConnect)
        {
            bIsCreatConnect = false;
            // 4.8 Creat connect.
            stGapCreateConnection_t stConnParam = 
            {
                .u16ConnIntervalMin1250us  = BLE_CONNECT_INTERVAL,
                .u16ConnIntervalMax1250us  = BLE_CONNECT_INTERVAL,
                .u16SupervisionTimeout10ms = BLE_CONNECT_TIMEOUT,
                .u16ConnLatency            = 0,
                .u16ScanInterval625us      = BLE_SCAN_INTERVAL,
                .u16ScanWindow625us        = BLE_SCAN_WINDOW,
                .enumOwnAddressType        = GAP_OWN_ADDRESS_TYPE_PUBLIC,
                .enumPeerAddressType       = GAP_INITIATOR_PEER_ADDRESS_TYPE_PUBLIC,
                .enumInitiatorFilterPolicy = GAP_INITIATOR_FILTER_POLICY_WHITELIST_IS_NOT_USED,
                .u8ScanChannelMap          = BLE_SCAN_CHANNEL,
            };
            memcpy(stConnParam.pu8PeerAddress, pu8Address, 6);
            rom_gap_api_create_connection(&stConnParam);
            break;
        }
    }
}


/**
 * @brief  handle all ble slave message.
 * @param  None.
 */
static void ble_event_handler(uint32_t u32Cmd, const void *pData, uint32_t u32Len)
{
    uint8_t *pu8Buf = (uint8_t *)pData;

    // MLOGD ( 0xa002, "ble_event_proc u32Cmd = 0x%X", u32Cmd );
    // PRINTF ( "ble_event_proc u32Cmd = 0x%X\n", u32Cmd );

    switch (u32Cmd)
    {
        case MSG_BLE_ADV_REPORT_IND:
        {
            ble_event_adv_report(pu8Buf, u32Len);
            //ble_event_report(MSG_BLE_ADV_REPORT_IND, pu8Buf, u32Len);// may message *p to refresh
            break;  
        }
        
        case MSG_BLE_CONNECTED_IND:
        {
            stHciEventParamVendorConnectionComplete_t *pstEvent = (stHciEventParamVendorConnectionComplete_t *)pu8Buf;

            MLOGD(0xa003, "MSG_BLE_CONNECTED_IND = 0x%X", pstEvent->Connection_Handle);
            PRINTF("[MSG_BLE_CONNECTED_IND]Handle = 0x%04X\n", pstEvent->Connection_Handle);

            m_u16ConnHandle = pstEvent->Connection_Handle;
            uint32_t u32Ret = rom_gatt_client_api_discover_primary_services(m_u16ConnHandle);
            PRINTF("discover_primary_services %d\n", u32Ret);
            if(u32Ret == 0)
            {
                bIsServicesFound = true;
            }
            rom_gap_api_send_security_req(m_u16ConnHandle);
            ble_event_report(MSG_BLE_CONNECTED_IND, NULL, 0);
            break;
        }
        case MSG_BLE_DISCONNECTED_IND:
        {
            stHciEventParamDisconnectionComplete_t *pstEvt = (stHciEventParamDisconnectionComplete_t *)pu8Buf;

            MLOG88D(0xa005, "MSG_BLE_DISCONNECTED_IND Handle = 0x%X, reason = 0x%X", pstEvt->Connection_Handle,
                    pstEvt->Reason);
            PRINTF("[MSG_BLE_DISCONNECTED_IND] Handle = 0x%X, reason = 0x%X\n", pstEvt->Connection_Handle,
                   pstEvt->Reason);

            m_u16ConnHandle = 0;
            bIsServicesFound = false;
            ble_event_report(MSG_BLE_DISCONNECTED_IND, NULL, 0);
            break;
        }
        case MSG_BLE_ENCRYPTED_CHANGED_IND:
        {
            stHciEventParamEncryptionChange_t *pstEvt = (stHciEventParamEncryptionChange_t *)pu8Buf;

            MLOG88D(0xa007, "MSG_BLE_ENCRYPTED_CHANGED_IND Handle = 0x%X, Encryption_Enabled = 0x%X",
                    pstEvt->Connection_Handle, pstEvt->Encryption_Enabled);
            PRINTF("[MSG_BLE_ENCRYPTED_CHANGED_IND] Handle = 0x%X, Encryption_Enabled = 0x%X\n",
                   pstEvt->Connection_Handle, pstEvt->Encryption_Enabled);
            break;
        }
        case MSG_BLE_ENCRYPTED_REFRESH_IND:
        {
            stHciEventParamEncryptionKeyRefreshComplete_t *pstEvt =
                (stHciEventParamEncryptionKeyRefreshComplete_t *)pu8Buf;

            MLOGD(0xa008, "MSG_BLE_ENCRYPTED_REFRESH_IND Handle = 0x%X", pstEvt->Connection_Handle);
            PRINTF("[MSG_BLE_ENCRYPTED_REFRESH_IND] Handle = 0x%X\n", pstEvt->Connection_Handle);
            break;
        }
        case MSG_BLE_LL_VERSION_IND:
        {
            stHciEventParamReadRemoteVersionInformationComplete_t *pstEvt =
                (stHciEventParamReadRemoteVersionInformationComplete_t *)pu8Buf;

            stGapSetPhy_t stPhy;
            stPhy.u8PreferTxPhys = GAP_PREFER_PHY_2M;
            stPhy.u8PreferRxPhys = GAP_PREFER_PHY_2M;
            // rom_gap_api_connection_set_phy(pstEvt->Connection_Handle, &stPhy);

            MLOG1688D(0xa009, "MSG_BLE_LL_VERSION_IND Manufacturer_Name = 0x%X, Handle = 0x%X, Version = 0x%X",
                      pstEvt->Manufacturer_Name, pstEvt->Connection_Handle, pstEvt->Version);
            PRINTF("[MSG_BLE_LL_VERSION_IND] Manufacturer_Name = 0x%X, Handle = 0x%X, Version = 0x%X\n",
                   pstEvt->Manufacturer_Name, pstEvt->Connection_Handle, pstEvt->Version);
            break;
        }
        case MSG_BLE_LL_FEATRUE_IND:
        {
            stHciEventParamLEReadRemoteFeaturesComplete_t *pstEvt =
                (stHciEventParamLEReadRemoteFeaturesComplete_t *)pu8Buf;

            MLOGD(0xa00a, "MSG_BLE_LL_FEATRUE_IND Handle = 0x%X", pstEvt->LE_Features);
            MLOGXD(0xa00b, "MSG_BLE_LL_FEATRUE_IND LE_Features", (uint8_t *)&pstEvt->LE_Features, 8);
            PRINTF("[MSG_BLE_LL_FEATRUE_IND] Handle = 0x%LX\n", pstEvt->LE_Features);
            break;
        }
        case MSG_BLE_CONNECTION_UPDATE_COMPLETE_IND:
        {
            stHciEventParamLEConnectionUpdateComplete_t *pstEvt = (stHciEventParamLEConnectionUpdateComplete_t *)pu8Buf;

            MLOG161616D(0xa00c,
                        "MSG_BLE_CONNECTION_UPDATE_COMPLETE_IND Conn_Interval = 0x%X, Conn_Latency = 0x%X, "
                        "Supervision_Timeou = 0x%X",
                        pstEvt->Conn_Interval, pstEvt->Conn_Latency, pstEvt->Supervision_Timeout);
            PRINTF(
                "[MSG_BLE_CONNECTION_UPDATE_COMPLETE_IND] Conn_Interval = 0x%X, Conn_Latency = 0x%X, "
                "Supervision_Timeou = 0x%X\n",
                pstEvt->Conn_Interval, pstEvt->Conn_Latency, pstEvt->Supervision_Timeout);
            break;
        }
        case MSG_BLE_DATA_LENGTH_UPDATE_COMPLETE_IND:
        {
            stHciEventParamLEDataLengthChange_t *pstEvt = (stHciEventParamLEDataLengthChange_t *)pu8Buf;

            MLOG161616D(0xa00d,
                        "MSG_BLE_DATA_LENGTH_UPDATE_COMPLETE_IND Handle = 0x%X, MaxTxOctets = 0x%X, MaxRxOctets = 0x%X",
                        pstEvt->Connection_Handle, pstEvt->MaxTxOctets, pstEvt->MaxRxOctets);
            break;
        }
        case MSG_BLE_PHY_UPDATE_COMPLETE_IND:
        {
            stHciEventParamLEPhyUpdateComplete_t *pstEvt = (stHciEventParamLEPhyUpdateComplete_t *)pu8Buf;

            MLOG1688D(0xa00e, "MSG_BLE_PHY_UPDATE_COMPLETE_IND Handle = 0x%X, TX_PHY = 0x%X, RX_PHY = 0x%X",
                      pstEvt->Connection_Handle, pstEvt->TX_PHY, pstEvt->RX_PHY);
            PRINTF("[MSG_BLE_PHY_UPDATE_COMPLETE_IND] Handle = 0x%X, TX_PHY = 0x%X, RX_PHY = 0x%X\n",
                   pstEvt->Connection_Handle, pstEvt->TX_PHY, pstEvt->RX_PHY);
            break;
        }
        case MSG_BLE_MTU_EXCHANGED_IND:
        {
            st_mtu_exchange_complete_event *pstEvt = (st_mtu_exchange_complete_event *)pu8Buf;

            MLOG1616D(0xa00f, "MSG_BLE_MTU_EXCHANGED_IND Handle = 0x%X, mtu = %d", pstEvt->conn_handle, pstEvt->mtu);
            PRINTF("[MSG_BLE_MTU_EXCHANGED_IND] Handle = 0x%X, mtu = %d\n", pstEvt->conn_handle, pstEvt->mtu);
            if(!bIsServicesFound)
            {
                uint32_t u32Ret = rom_gatt_client_api_discover_primary_services(m_u16ConnHandle);
                PRINTF("discover_primary_services %d\n", u32Ret);
                if(u32Ret == 0)
                {
                    bIsServicesFound = true;
                }
            }
            break;
        }
        case MSG_BLE_LLCAP_CONN_UPDATE_RSP_IND:
        {
            st_llcap_conn_update_rsp_event *pstEvt = (st_llcap_conn_update_rsp_event *)pu8Buf;
            break;
        }
        case MSG_BLE_PAIR_USER_PASSKEYREQ_IND:
        {
            uint16_t u16ConnectionHandle = *((uint16_t *)pu8Buf);
            MLOGD(0xa010, "MSG_BLE_PAIR_USER_PASSKEYREQ_IND Handle = 0x%X", u16ConnectionHandle);
            break;
        }
        case MSG_BLE_PAIR_USER_PASSKEY_DISPLAY_IND:
        {
            st_passkey_display_event *pstEvt = (st_passkey_display_event *)pu8Buf;

            MLOG3216D(0xa011, "MSG_BLE_PAIR_USER_PASSKEY_DISPLAY_IND passkey = %d, Handle = 0x%X",
                      rom_little_endian_read_32(pstEvt->passkey, 0), pstEvt->conn_handle);
            break;
        }
        case MSG_BLE_PAIR_USER_PASSKEYREQ_CONF_IND:
        {
            st_passkey_display_event *pstEvt = (st_passkey_display_event *)pu8Buf;

            MLOG3216D(0xa012, "MSG_BLE_PAIR_USER_PASSKEYREQ_CONF_IND passkey = %d, Handle = 0x%X",
                      rom_little_endian_read_32(pstEvt->passkey, 0), pstEvt->conn_handle);
            break;
        }
        case MSG_BLE_PAIR_USER_PASSKEY_DISPLAY_CANCEL_IND:
        {
            uint16_t u16ConnectionHandle = *((uint16_t *)pu8Buf);

            MLOGD(0xa013, "MSG_BLE_PAIR_USER_PASSKEY_DISPLAY_CANCEL_IND Handle = 0x%X", u16ConnectionHandle);
            break;
        }
        case MSG_BLE_PAIR_COMPLETED_IND:
        {
            st_pair_complete_event *pstPairEvt = (st_pair_complete_event *)pu8Buf;

            MLOGD(0xa014, "MSG_BLE_PAIR_COMPLETED_IND Handle = 0x%X  ", pstPairEvt->conn_handle);
            break;
        }
        case MSG_BLE_PAIR_FAIL_IND:
        {
            // Pair fail reason
            st_pair_failed_event *pstPairEvt = (st_pair_failed_event *)pu8Buf;

            MLOG88D(0xa015, "MSG_BLE_PAIR_FAIL_IND Handle = 0x%X, reason = 0x%X", pstPairEvt->conn_handle,
                    pstPairEvt->reason);
            break;
        }
        case MSG_LTK_REQ_WHEN_RECONECT_AFTER_PAIR_IND:
        {
            st_ltk_req_event *pstEvt = (st_ltk_req_event *)pu8Buf;

            MLOG1616D(0xa016, "MSG_LTK_REQ_WHEN_RECONECT_AFTER_PAIR_IND Handle = 0x%X, ediv = 0x%X",
                      pstEvt->conn_handle, pstEvt->ediv);
            break;
        }
        case MSG_BLE_READ_RSSI_COMMAND_RESULT:
        {
            stHciReturnParametersHCIReadRSSI_t *pstReturnParam = (stHciReturnParametersHCIReadRSSI_t *)pData;
            MLOG88D(0xa021, "MSG_BLE_READ_RSSI_COMMAND_RESULT - Status:0x%02x RSSI:-%udBm", pstReturnParam->Status,
                    (~(pstReturnParam->RSSI)) + 1);
            break;
        }
        case MSG_BLE_DISCONNECT_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_SET_RANDOM_ADDRESS_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_SET_ADVERTISING_PARAMETERS_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa022, "MSG_BLE_SET_ADVERTISING_PARAMETERS_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_SET_ADVERTISING_DATA_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];

            MLOGD(0xa023, "MSG_BLE_SET_ADVERTISING_DATA_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_SET_SCAN_RESPONSE_DATA_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];

            MLOGD(0xa024, "MSG_BLE_SET_SCAN_RESPONSE_DATA_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_SET_ADVERTISING_ENABLE_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa025, "MSG_BLE_SET_ADVERTISING_ENABLE_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_SET_SCAN_PARAMETERS_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa026, "MSG_BLE_SET_SCAN_PARAMETERS_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_SET_SCAN_ENABLE_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa027, "MSG_BLE_SET_SCAN_ENABLE_COMMAND_RESULT - Status:0x%02x", u8Status);
            ble_event_report(MSG_BLE_SET_SCAN_ENABLE_COMMAND_RESULT, NULL, 0);
            break;
        }
        case MSG_BLE_CLEAR_WHITE_LIST_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa028, "MSG_BLE_CLEAR_WHITE_LIST_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_ADD_DEVICE_TO_WHITE_LIST_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa029, "MSG_BLE_ADD_DEVICE_TO_WHITE_LIST_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_REMOVE_DEVICE_FROM_WHITE_LIST_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa02a, "MSG_BLE_REMOVE_DEVICE_FROM_WHITE_LIST_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_CREATE_CONNECTION_CANCEL_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_CONNECTION_UPDATE_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa02b, "MSG_BLE_CONNECTION_UPDATE_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_LONG_TERM_KEY_REQUEST_REPLY_COMMAND_RESULT:
        {
            stHciReturnParametersLELongTermKeyRequestReply_t *pstResult =
                (stHciReturnParametersLELongTermKeyRequestReply_t *)pu8Buf;
            break;
        }
        case MSG_BLE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_COMMAND_RESULT:
        {
            stHciReturnParametersLELongTermKeyRequestNegativeReply_t *pstResult =
                (stHciReturnParametersLELongTermKeyRequestNegativeReply_t *)pu8Buf;
            break;
        }
        case MSG_BLE_SET_DATA_LENGTH_COMMAND_RESULT:
        {
            stHciReturnParametersLESetDataLength_t *pstResult = (stHciReturnParametersLESetDataLength_t *)pu8Buf;
            break;
        }
        case MSG_BLE_SET_PHY_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_READ_REMOTE_FEATURES_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_ADD_DEVICE_TO_RESOLVING_LIST_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_REMOVE_DEVICE_FROM_RESOLVING_LIST_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_CLEAR_RESOLVING_LIST_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_SET_ADDRESS_RESOLUTION_ENABLE_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_SET_PRIVACY_MODE_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_READ_CHANNEL_MAP_COMMAND_RESULT:
        {
            stHciReturnParametersLEReadChannelMap_t *pstResult = (stHciReturnParametersLEReadChannelMap_t *)pu8Buf;
            break;
        }
        case MSG_BLE_SET_ADVERTISING_SET_RANDOM_ADDRESS_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa02c, "MSG_BLE_SET_ADVERTISING_SET_RANDOM_ADDRESS_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_REMOVE_ADVERTISING_SET_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_CLEAR_ADVERTISING_SETS_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        case MSG_BLE_SET_PUBLIC_ADDRESS_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa033, "MSG_BLE_SET_PUBLIC_ADDRESS_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_CREATE_CONNECTION_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            MLOGD(0xa034, "MSG_BLE_CREATE_CONNECTION_COMMAND_RESULT - Status:0x%02x", u8Status);
            break;
        }
        case MSG_BLE_UPDATE_CHANNEL_MAP_COMMAND_RESULT:
        {
            uint8_t u8Status = pu8Buf[0];
            break;
        }
        default:
            break;
    }
}

uint16_t ble_get_connect_handle(void)
{
    return m_u16ConnHandle;
}

/**
 * @brief  ble event init.
 */
void ble_event_init(ble_event_callback_t ble_event_callback_cb)
{
    ble_event_callback = ble_event_callback_cb;
    ble_callback_set_host_stack_send_event_callback(ble_event_handler);
}
