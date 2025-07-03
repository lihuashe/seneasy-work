 
#ifndef XH_SVC__
#define XH_SVC__

#include <stdbool.h>
#include <stdint.h>
#define SVCALL_AS_NORMAL_FUNCTION

#define xh_flash_write                  hse_flash_write
#define xh_flash_page_erase             hse_flash_erase

#define xh_ble_enable                   ble_stack_enable
#define xh_app_evt_wait                 ble_evt_wait
#define xh_ble_user_mem_reply           ble_user_mem_reply
#define xh_power_system_off             sys_power_system_off
#define softdevice_ble_evt_handler_set  ble_evt_handler_set
#define softdevice_sys_evt_handler_set  sys_evt_handler_set
#define xh_ble_uuid_encode              ble_uuid_encode
#define xh_ble_uuid_vs_add              ble_uuid_vs_add
#define xh_ble_tx_buffer_count_get      ble_tx_buffer_count_get
#define xh_softdevice_enable            softdevice_enable

#define xh_ble_version_get              ble_version_get
#define xh_ble_opt_get                  ble_opt_get
#define xh_ble_opt_set                  ble_opt_set

#define xh_ble_gap_address_set          gap_address_set
#define xh_ble_gap_address_get          gap_address_get
#define xh_ble_gap_adv_data_set         gap_adv_data_set
#define xh_ble_gap_adv_start            gap_adv_start
#define xh_ble_gap_adv_stop             gap_adv_stop
#define xh_ble_gap_conn_param_update    gap_conn_param_update
#define xh_ble_gap_disconnect           gap_disconnect
#define xh_ble_gap_tx_power_set         gap_tx_power_set
#define xh_ble_gap_appearance_set       gap_appearance_set
#define xh_ble_gap_appearance_get       gap_appearance_get
#define xh_ble_gap_ppcp_set             gap_ppcp_set
#define xh_ble_gap_ppcp_get             gap_ppcp_get
#define xh_ble_gap_device_name_set      gap_device_name_set
#define xh_ble_gap_device_name_get      gap_device_name_get
#define xh_ble_gap_authenticate         gap_authenticate
#define xh_ble_gap_sec_params_reply     gap_sec_params_reply
#define xh_ble_gap_auth_key_reply       gap_auth_key_reply
#define xh_ble_gap_auth_yesno_reply     smp_auth_yesno_reply
#define xh_ble_gap_encrypt              gap_encrypt
#define xh_ble_gap_sec_info_reply       gap_sec_info_reply
#define xh_ble_gap_conn_sec_get         gap_conn_sec_get
#define xh_ble_gap_set_phy              gap_set_preference_phy
#define xh_ble_gap_set_data_length      gap_set_data_length
#define xh_ble_gap_rssi_start           gap_rssi_start
#define xh_ble_gap_rssi_stop            gap_rssi_stop
#define xh_ble_gap_rssi_get             gap_rssi_get
#define xh_ble_gap_scan_start           gap_scan_start
#define xh_ble_gap_scan_stop            gap_scan_stop
#define xh_ble_gap_connect              gap_connect
#define xh_ble_gap_connect_cancel       gap_connect_cancel

#define xh_ble_gatts_service_add            gatts_service_add
#define xh_ble_gatts_include_add            gatts_include_add
#define xh_ble_gatts_characteristic_add     gatts_characteristic_add
#define xh_ble_gatts_descriptor_add         gatts_descriptor_add
#define xh_ble_gatts_value_set              gatts_value_set
#define xh_ble_gatts_value_get              gatts_value_get
#define xh_ble_gatts_hvx                    gatts_hvx
#define xh_ble_gatts_service_changed        gatts_service_changed
#define xh_ble_gatts_rw_authorize_reply     gatts_rw_authorize_reply
#define xh_ble_gatts_sys_attr_set           gatts_sys_attr_set
#define xh_ble_gatts_sys_attr_get           gatts_sys_attr_get

#define xh_ble_gattc_read                   gattc_read
#define xh_ble_gattc_write                  gattc_write
#define xh_ble_gattc_primary_services_discover gattc_primary_services_discover
#define xh_ble_gattc_relationships_discover    gattc_relationships_discover
#define xh_ble_gattc_characteristics_discover  gattc_characteristics_discover
#define xh_ble_gattc_descriptors_discover      gattc_descriptors_discover
#define xh_ble_gattc_char_value_by_uuid_read   gattc_char_value_by_uuid_read
#define xh_ble_gattc_char_values_read          gattc_char_values_read
#define xh_ble_gattc_hv_confirm                gattc_hv_confirm

#define SVCALL(number, return_type, signature) return_type signature  

/**@brief Set ATT database buffer size.
 *
 * @param[in]    buf    Pointer to an empty buffer where ATT database will be placed.
 * @param[in]    len    Length of the buffer pointed by buf.
 *
 * @note          This function must be called before the GATT database is initialized
 *
 * @retval ::XH_SUCCESS Init success
 * @retval ::HSE_ERROR_INVALID_STATE ready inited
 */
uint32_t ble_set_db_buffer(uint8_t* buf, uint16_t len);

/**@brief Get ATT database buffer used size.
 * @retval Size of used database.
 */
uint32_t ble_get_db_buffer_free_size(void);

/**@brief Set prepare write buffer size, If not set, use the default value: ATT_MAX_MTU
 * @retval ::XH_SUCCESS success
 * @retval ::HSE_ERROR_INVALID_STATE should set db buffer by calling ble_set_db_buffer first
 */
uint32_t ble_set_prepare_write_cache(uint8_t* buf, uint16_t len);

/**@brief Get gap link status.
 * @retval True if link encrypted, else False
 */
uint8_t  gap_encrypt_get(void);

/**@brief Set private key & public key for security connection pairing
 * @param[in]    pri_key    private key, little endian, must be global variable
 * @param[in]    pub_key    public key, little endian, must be global variable
 * @retval if success
 */
bool smp_sc_set_key(uint8_t *pri_key, uint8_t *pub_key);

/**@brief Check RPA can be resolved by the given IRK
 * @param[in]    bdaddr     resolvable private address, little endian
 * @param[in]    irk        IRK, little endian
 * @retval if resolved success
 */
uint8_t gap_rpa_check(uint8_t *bdaddr, uint8_t *irk);

/**@brief Get current MTU size
 * @retval MTU
 */
uint16_t att_mtu_get(void);

/**@brief Exchange MTU
 * @retval ::XH_SUCCESS MTU change request sent.
 * @retval ::HSE_ERROR_INVALID_STATE connection not established.
 * @retval ::HSE_ERROR_BUSY Other ATT request pending.
 */
uint32_t gattc_mtu_exchange(uint16_t mtu);

/**@brief set IRK & identity address
 * @param[in]    irk        IRK, little endian, if irk == NULL, irk is automatically generated
 * @param[in]    addr       addr,little endian, if addr == NULL, internal public address will be used
 * @retval ::XH_SUCCESS    success
 */
uint8_t gap_irk_addr_set(uint8_t *irk, uint8_t *addr);

/**@brief Generate resolvable private address from irk
 * @param[in]    irk        IRK, little endian, if irk == NULL, internal IRK will be used
 * @param[out]   bdaddr     resolvable private address, little endian
 * @retval ::XH_SUCCESS    success
 */
uint8_t gap_rpa_gen(uint8_t *irk, uint8_t *rpa_address);

#endif // XH_SVC__
