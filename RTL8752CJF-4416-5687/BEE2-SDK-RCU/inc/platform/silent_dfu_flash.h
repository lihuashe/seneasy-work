#ifndef DFU_FLASH_H
#define DFU_FLASH_H

#include <stdint.h>
#include <stdbool.h>
#include "patch_header_check.h"

#define SET_VALID_BITMAP(image_id)      (valid_bitmap |= BIT(image_id - OTA))
#define GET_VALID_BITMAP(image_id)      (valid_bitmap >> (image_id - OTA))

extern uint16_t valid_bitmap;

bool unlock_flash_all(void);
void lock_flash(void);

/**
 * @brief erase a sector of the flash.
 *
 * @param  signature          signature to identify FW.
 * @param  offset             offset of the image.
 * @return  0 if erase successfully, error line number otherwise
*/
uint32_t sil_dfu_flash_erase(uint16_t signature, uint32_t offset);

uint32_t sil_dfu_flash_check_blank(uint16_t image_id, uint32_t offset, uint16_t nSize);

/**
 * @brief erase a sector of the flash, will retry 3 times at most.
 *
 * @param  signature          signature to identify FW.
 * @param  offset             offset of the image.
 * @return  0 if erase successfully, error line number otherwise
*/
uint32_t sil_dfu_flash_erase_with_retry(uint16_t image_id, uint32_t offset, uint16_t size);

/**
 * @brief   copy appdata from active bank to updating bank.
 *
 * @param   image_id    signature to identify image.
 * @param   dlAddress   address the img copy to.
 * @param   dlSize      copy size.
 * @return  true if the image copied success, false otherwise
*/
bool sil_dfu_copy_img(uint16_t image_id, uint32_t dlAddress, uint32_t dlSize);

/**
 * @brief  write specified image data with specified length to flash
 * @param  signature          signature to identify FW.
 * @param  offset             offset of the image.
 * @param  length             length of data.
 * @param  p_void             pointer to data.
 * @return 0 if write FW image successfully, error line number otherwise
*/
#if (SUPPORT_TEMP_COMBINED_OTA == 1)
uint32_t sil_dfu_update(uint16_t image_id, uint32_t offset, uint32_t length,
                        uint32_t *p_void, bool is_new_image);
#else
uint32_t sil_dfu_update(uint16_t signature, uint32_t offset, uint32_t length,
                        uint32_t/*void*/ *p_void);
#endif

void dfu_set_image_ready(T_IMG_CTRL_HEADER_FORMAT *p_header);
/**
 * @brief calculate checksum of the image and compare with given checksum value.
 *
 * @param   image_id     signature to identify image.
 * @return  true if the image integrity check passes, false otherwise
*/
#if (SUPPORT_TEMP_COMBINED_OTA == 1)
bool silent_dfu_check_checksum(uint16_t image_id, uint32_t offset);
#else
bool silent_dfu_check_checksum(uint16_t image_id);
#endif

bool silent_dfu_reset(uint16_t image_id);

#endif //DFU_FLASH_H
