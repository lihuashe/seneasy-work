#ifndef FSBL_EXT_SECTION_H_
#define FSBL_EXT_SECTION_H_

#define SECTION(_name) __attribute__ ((__section__(_name)))
#define PATCH_FLASH_HEADER           SECTION(".patch.flash.header")
#define PATCH_FLASH_HEADER_EXT       SECTION(".patch.flash.header_ext")
#define PATCH_FLASH_START_SECTION    SECTION(".patch.flash.start")
#define PATCH_FLASH_TEXT_SECTION     SECTION(".patch.flash.text")
#define PATCH_FLASH_RODATA_SECTION   SECTION(".patch.flash.rodata")
#define PATCH_FLASH_RAM_TEXT_SECTION SECTION(".patch.flash.ram_text")

#define PATCH_RAM_START_SECTION      SECTION(".patch.ram.start")

#endif  // FSBL_EXT_SECTION_H_
