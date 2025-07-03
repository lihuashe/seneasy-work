#ifndef FSBL_EXT_DEFINE_H
#define FSBL_EXT_DEFINE_H
#include "..\..\rcu\flash_map.h"
#include "..\..\rcu\mem_config.h"

/*============================================================================*
 *  The following macros determined different features
 *============================================================================*/
#define FSBL_RAM_CODE_ENCRYPTED     0
#define RELEASE_VERSION             0


/*============================================================================*
 *  Flash secure boot build address and size
 *============================================================================*/
#if (APP_BANK == 0)
#define FSBL_FLASH_ADR              BANK0_APP_DATA1_ADDR
#define FSBL_FLASH_SIZE             BANK0_APP_DATA1_SIZE
#else
#define FSBL_FLASH_ADR              BANK1_APP_DATA1_ADDR
#define FSBL_FLASH_SIZE             BANK1_APP_DATA1_SIZE
#endif
/*============================================================================*
 *  Flash secure boot ram address and size
 *============================================================================*/
#define FSBL_RAM_ADDR               (DATA_RAM_START_ADDR + DATA_RAM_TOTAL_SIZE - FSBL_FLASH_SIZE) //reuse heap ram, sync with master branch, can't be modified
#define FSBL_RAM_SIZE               (FSBL_FLASH_SIZE)


#endif // FSBL_EXT_DEFINE_H
