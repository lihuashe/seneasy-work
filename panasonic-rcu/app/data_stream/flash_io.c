/**
 * @file flash_io.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief 数据持久化实现
 * @version 0.1
 * @date 2025-03-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "py32f0xx_hal.h"
#include "app.h"
#include <string.h>
#include "pm.h"
#include "flash_io.h"
extern void APP_ErrorHandler(void);

/*============================================================================*
 *                              Local Variables
 *============================================================================*/


/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief  Erase Flash
 * @param  None
 * @retval None
 */
static void APP_FlashErase(void)
{
	uint32_t PAGEError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct = {0};

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGEERASE;		   /* Erase type: FLASH_TYPEERASE_PAGEERASE = Page erase, FLASH_TYPEERASE_SECTORERASE = Sector erase */
	EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;		   /* Starting address for erase */
	EraseInitStruct.NbPages = sizeof(app) / FLASH_PAGE_SIZE + 1;   /* Number of pages to be erased */
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) /* Perform page erase, PAGEError returns the page with erase error, returns 0xFFFFFFFF for successful erase */
	{
#if __DEBUG__ >= __DEBUG_ERR__
		my_log("\033[1;31m>E%d[%s] ret:%d APP_FlashErase Fail\n\033[0m", __LINE__, __FUNCTION__, PAGEError);
#endif
		APP_ErrorHandler();
	}
}

/**
 * @brief  Program Flash
 * @param  None
 * @retval None
 */
static void APP_FlashProgram(void)
{
	uint32_t flash_program_start = FLASH_USER_START_ADDR;				/* Start address for flash program */
	uint32_t flash_program_end = (FLASH_USER_START_ADDR + sizeof(app)); /* End address for flash program */
	uint32_t src_buf[FLASH_PAGE_SIZE];									/* Pointer to the array */
	memcpy(src_buf, &app, sizeof(app));
	uint32_t *src = (uint32_t *)src_buf; /* Pointer to the array */

	while (flash_program_start < flash_program_end)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, flash_program_start, src) == HAL_OK) /* Execute Program */
		{
			flash_program_start += FLASH_PAGE_SIZE; /* Set flash start pointer to the first page */
			src += FLASH_PAGE_SIZE / 4;				/* Update the data pointer */
		}
	}
}

/**
 * @brief  Check if Flash is blank
 * @param  None
 * @retval None
 */
static void APP_FlashBlank(void)
{
	uint32_t addr = 0;

	while (addr < sizeof(app))
	{
		if (0xFFFFFFFF != HW32_REG(FLASH_USER_START_ADDR + addr))
		{
			APP_ErrorHandler();
		}
		addr += 4;
	}
}
/**
 * @brief 从主控芯片UID中选择4个字节作为应用UID
 *
 */
static void APP_getUidFromChipUid()
{
	uint8_t *chip_uid_addr = (uint8_t *)UID_BASE;
	app.unique_id[0] = chip_uid_addr[0];
	app.unique_id[1] = chip_uid_addr[4];
	app.unique_id[2] = chip_uid_addr[7];
	app.unique_id[3] = chip_uid_addr[9];
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

void flash_save()
{
	if (!memcmp(&app, (uint8_t *)FLASH_USER_START_ADDR, sizeof(app_t)))
		return;
#if __DEBUG__ >= __DEBUG_INFO__
	my_log(">I%d[%s] app.chipMode : 0x%02X\n", __LINE__, __FUNCTION__, app.chipMode);
#endif
	/* Unlock FLASH */
	HAL_FLASH_Unlock();

	/* Erase FLASH */
	APP_FlashErase();

	/* Check if FLASH is blank */
	// APP_FlashBlank();

	/* program FLASH */
	APP_FlashProgram();

	/* Lock FLASH */
	HAL_FLASH_Lock();
}

void flash_load()
{
	memcpy(&app, (uint8_t *)FLASH_USER_START_ADDR, sizeof(app));

#if __DEBUG__ >= __DEBUG_INFO__
	my_log(">I%d[%s] app.chipMode : 0x%02X\n", __LINE__, __FUNCTION__, app.chipMode);
	my_log(">I%d[%s] app.unique_id : ", __LINE__, __FUNCTION__);
	for (size_t i = 0; i < sizeof(app.unique_id); i++)
	{
		my_log("0x%02X, ", app.unique_id[i]);
	}
	my_log("\n");
#endif
	if (app.chipMode != A7535 && app.chipMode != XN297L || app.modelType != MODEL_TYPE)
	{
		app = app_default;

		flash_save();
#if __DEBUG__ >= __DEBUG_INFO__
		my_log(">I%d[%s] select default config\n", __LINE__, __FUNCTION__);
		my_log(">I%d[%s] app.chipMode : 0x%02X\n", __LINE__, __FUNCTION__, app.chipMode);
		my_log(">I%d[%s] app.unique_id : ", __LINE__, __FUNCTION__);
		for (size_t i = 0; i < sizeof(app.unique_id); i++)
		{
			my_log("0x%02X, ", app.unique_id[i]);
		}
		my_log("\n");
#endif
	}
	if (0xFFFFFFFF == HW32_REG(FLASH_FACTORY_MAC_START_ADDR))
		APP_getUidFromChipUid();
	else
		memcpy(app.unique_id, (uint8_t *)FLASH_FACTORY_MAC_START_ADDR, 4);
}
