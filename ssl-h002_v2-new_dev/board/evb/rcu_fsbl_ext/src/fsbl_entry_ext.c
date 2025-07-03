#include <stdint.h>
#include <string.h>
#include "fsbl_ext_define.h"
#include "fsbl_ext_section.h"
#include "flash_device.h"
#include "patch_header_check.h"
#include "rom_uuid.h"
#include "system_rtl876x.h"
#include "rtl876x.h"
#include "trace.h"
#include "version.h"
#include <keyscan_basic_driver.h>
#include <rtl876x_keyscan.h>
#include "rtl876x_wdg.h"
#include "test_mode.h"

#if (RELEASE_VERSION == 1)
#undef DBG_DIRECT
#define DBG_DIRECT(...)
#endif

#define FLASH_TABLE_MAGIC_PATTERN           (0x5a5a12a5)

///////////////////////////////////////////////////////

extern unsigned int Image$$FSBL_RAM_TEXT_SECTION$$Base;
extern unsigned int Image$$FSBL_RAM_TEXT_SECTION$$Length;
extern unsigned int Load$$FSBL_RAM_TEXT_SECTION$$Base;

#pragma diag_suppress 1296 /* disable warning 1296(extened constant initialiser used)*/
PATCH_FLASH_HEADER const T_IMG_HEADER_FORMAT img_header =
{
    .ctrl_header =
    {
        .ic_type = DEFINED_IC_TYPE,
        .secure_version = 0,
        .ctrl_flag.flag_value.xip = 0,
#if (FSBL_RAM_CODE_ENCRYPTED == 1)
        .ctrl_flag.flag_value.enc = 1,
#else
        .ctrl_flag.flag_value.enc = 0,
#endif
        .ctrl_flag.flag_value.load_when_boot = 1,  //must be 1
        .ctrl_flag.flag_value.enc_load = 0,
        .ctrl_flag.flag_value.enc_key_select = ENC_KEY_OCEK_WITH_OEMCONST,
        .ctrl_flag.flag_value.not_ready = 0,
        .ctrl_flag.flag_value.not_obsolete = 1,
        .ctrl_flag.flag_value.integrity_check_en_in_boot = 0,
        .image_id = AppData1,
        .payload_len = 0xC00,    //Will modify by build tool later
    },
    .uuid = DEFINE_rom_uuid,

    .load_base = (uint32_t) &Load$$FSBL_RAM_TEXT_SECTION$$Base,
    .load_len = (uint32_t) &Image$$FSBL_RAM_TEXT_SECTION$$Length,
    .exe_base = (uint32_t) &Image$$FSBL_RAM_TEXT_SECTION$$Base,

    .image_base = FSBL_FLASH_ADR,
    .magic_pattern = FLASH_TABLE_MAGIC_PATTERN,
    .fsbl_ext_img_id = AppData1, // need be same with image_id
    .fsbl_ext_load_pattern = FSBL_EXT_PATTERN,

    .git_ver =
    {
        .ver_info.sub_version._version_major = VERSION_MAJOR,
        .ver_info.sub_version._version_minor = VERSION_MINOR,
        .ver_info.sub_version._version_revision = VERSION_REVISION,
        .ver_info.sub_version._version_reserve = VERSION_BUILDNUM % 32, //only 5 bit
        ._version_commitid = VERSION_GCID,
        ._customer_name = {CN_1, CN_2, CN_3, CN_4, CN_5, CN_6, CN_7, CN_8},
    }
};

PATCH_FLASH_HEADER const T_AUTH_HEADER_FORMAT auth_header =
{
    .payload_mac = {[0 ... 15] = 0xFF},
    .header_mac = {[0 ... 15] = 0xFF},
};

#if FSBL_RAM_CODE_ENCRYPTED
#define ENC_ALIGN_SECTION __attribute__((aligned(16), used, section(".enc.dummy.align")));
const uint8_t enc_dummy_align[16] ENC_ALIGN_SECTION;
#endif

void bss_ram_init(void)
{
    extern char Image$$FSBL_RAM_TEXT_SECTION$$ZI$$Base[];
    extern unsigned int Image$$FSBL_RAM_TEXT_SECTION$$ZI$$Length;

    memset(Image$$FSBL_RAM_TEXT_SECTION$$ZI$$Base,
           0,
           (unsigned int)&Image$$FSBL_RAM_TEXT_SECTION$$ZI$$Length);
}

T_SW_RESET_REASON get_reset_reason(void)
{
    T_BTAON_FAST_TEST_MODE_TYPE aon;
    aon.d16 = btaon_fast_read_safe(BTAON_FAST_TEST_MODE);
    return (T_SW_RESET_REASON)(aon.s.reset_reason);
}

///////////////////////////////////////////////////////
PATCH_RAM_START_SECTION
bool fsbl_ext_entry(void)
{
    bool ret = true;

    if (RESET_REASON_POWERDOWN != get_reset_reason())
    {
        return false;
    }

    bss_ram_init();

#if (RELEASE_VERSION == 0)
    DBG_DIRECT("wake up from dss. secure_boot ext version: %d.%d.%d.%d", VERSION_MAJOR, VERSION_MINOR,
               VERSION_REVISION, VERSION_BUILDNUM);
#endif

    keyscan_pinmux_config();
    keyscan_init_driver(KeyScan_Manual_Sel_Bit, KeyScan_Debounce_Disable);
    //add user code here

    return ret;
}
