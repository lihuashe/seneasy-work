#ifndef LLC_TOOL_H
#define LLC_TOOL_H

#define LLC_TOOL_MIN(x,y) ((x>=y)?(y):(x))
#define LLC_TOOL_MAX(x,y) ((x>=y)?(x):(y))

bool rom_llc_tool_is_buf_4_byte_aligned(void* pBuf);

bool rom_llc_tool_is_buf_8_byte_aligned(void* pBuf);

uint16_t rom_llc_tool_read_little_endian_uint16(uint8_t *pu8Buf);

void rom_llc_tool_write_little_endian_uint16(uint8_t *pu8Buf, uint16_t u16Value);

uint32_t rom_llc_tool_read_little_endian_uint32(uint8_t *pu8Buf);

void rom_llc_tool_write_little_endian_uint32(uint8_t *pu8Buf, uint32_t u32Value);

uint64_t rom_llc_tool_read_little_endian_uint64(uint8_t *pu8Buf);;

void rom_llc_tool_write_little_endian_uint64(uint8_t *pu8Buf, uint64_t u64Value);

uint8_t rom_llc_tool_byte_bit_reverse(uint8_t u8Input);

bool rom_llc_tool_is_phy_coded(uint8_t enumLlpPhy);

uint32_t rom_llc_tool_calculate_trx_time_us_by_pdu_size(uint16_t u16PduSize, EN_LL_PHY_T enumLlpPhy);

uint16_t rom_llc_tool_calculate_pdu_payload_max_octets_by_tx_time_without_cte(uint16_t u16MaxTxTimeUs, EN_LL_PHY_T enumLlpPhy);

bool rom_llc_tool_is_array_all_zero ( uint8_t *pu8Array, uint16_t u16Len);

#endif
