#******************************************************************************************
# * @file     Makefile
# * 
# * @author   张凯华(zhangkaihua@seneasy.com)
# * 
# * @brief    
# * 
# * @date     2022-06-29
# * 
# * @attention
# * 
# *  Copyright 2022 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
# * 
# ******************************************************************************************/

# ******************************************************************************************/
# 1. 配置编译环境
# ******************************************************************************************/
TARGET ?= rcu
SCTTER_FILE ?= $(PROJ_DIR)/board/app.sct
PROJ_DIR = ./app
SDK_DIR ?= ./BEE2-SDK-RCU
# Build path
BUILD_DIR = build
BIN_DIR = release

# ******************************************************************************************/
# 版本管理
# ******************************************************************************************/
GET_VERSION=$(SDK_DIR)/tool/version_generation/git_generate_version.sh v $(SDK_DIR)/tool/version_generation $(BUILD_DIR)/version.h $(TARGET)

# ******************************************************************************************/
# 2. sources && include
# ******************************************************************************************/
# project sources
PROJ_SOURCES = 
PROJ_INCS = 
# 添加$(PROJ_DIR)下包含2级目录的所有*.c文件
PROJ_FOLDER := $(shell find $(PROJ_DIR) -maxdepth 2 -type d)
PROJ_SOURCES := $(foreach dir, $(PROJ_FOLDER), $(wildcard $(dir)/*.c))
PROJ_INCS := $(PROJ_FOLDER)

# SDK Sources
SDK_SOURCES = 
SDK_INCS= 
# mcu
SDK_SOURCES +=  \
${SDK_DIR}/src/mcu/rtl876x/overlay_mgr.c \
${SDK_DIR}/src/mcu/rtl876x/system_rtl8762c.c	\
${SDK_DIR}/src/mcu/peripheral/rtl876x_3wire_spi.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_adc.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_codec.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_gdma.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_gpio.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_i2c.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_i2s.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_io_dlps.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_ir.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_keyscan.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_lcd.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_lpc.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_qdec.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_rcc.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_tim.c \
${SDK_DIR}/src/mcu/peripheral/rtl876x_uart.c

#flash
SDK_SOURCES += \
${SDK_DIR}/src/flash/silent_dfu_flash.c \
# ${SDK_DIR}/src/flash/flash_hs_read.c \
# ${SDK_DIR}/src/flash/ftl_app_cb.c 


# module/data_uart_cmd
# SDK_SOURCES += \
# ${SDK_DIR}/src/mcu/module/data_uart_cmd/data_uart_dlps.c \
# ${SDK_DIR}/src/mcu/module/data_uart_cmd/data_uart.c \
# ${SDK_DIR}/src/mcu/module/data_uart_cmd/data_uart_dlps.c \
# ${SDK_DIR}/src/mcu/module/data_uart_cmd/user_cmd_parse.c 
# SDK_INCS += ${SDK_DIR}/src/mcu/module/data_uart_cmd
# platform
SDK_SOURCES += \
# ${SDK_DIR}/src/platform/dfu_flash.c \
# ${SDK_DIR}/src/platform/psram_platform.c \
# ${SDK_DIR}/src/platform/qspi_lcd_platform.c \
# ${SDK_DIR}/src/platform/retarget.c \
# ${SDK_DIR}/src/platform/system_trace.c
# ble/privacy
SDK_SOURCES += \
${SDK_DIR}/src/ble/privacy/privacy_mgnt.c
# ble/profile/server
SDK_SOURCES += \
${SDK_DIR}/src/ble/profile/server/atvv_service.c \
${SDK_DIR}/src/ble/profile/server/bas.c \
${SDK_DIR}/src/ble/profile/server/cscs.c \
${SDK_DIR}/src/ble/profile/server/dis.c \
${SDK_DIR}/src/ble/profile/server/gls.c \
${SDK_DIR}/src/ble/profile/server/hids_rmc.c \
${SDK_DIR}/src/ble/profile/server/hrs.c \
${SDK_DIR}/src/ble/profile/server/hts.c \
${SDK_DIR}/src/ble/profile/server/ias.c \
${SDK_DIR}/src/ble/profile/server/ipss.c \
${SDK_DIR}/src/ble/profile/server/lls.c \
${SDK_DIR}/src/ble/profile/server/lns.c \
${SDK_DIR}/src/ble/profile/server/plxs.c \
${SDK_DIR}/src/ble/profile/server/rcu_dfu_service.c \
${SDK_DIR}/src/ble/profile/server/rcu_ota_service.c \
${SDK_DIR}/src/ble/profile/server/rscs.c \
${SDK_DIR}/src/ble/profile/server/simple_ble_service.c \
${SDK_DIR}/src/ble/profile/server/sps.c \
${SDK_DIR}/src/ble/profile/server/tps.c \
${SDK_DIR}/src/ble/profile/server/vendor_service.c \
${SDK_DIR}/src/ble/profile/server/voice_service.c \
${SDK_DIR}/src/ble/profile/server/wss.c
# sdk includes
SDK_INCS += \
${SDK_DIR}/inc/app \
${SDK_DIR}/inc/bluetooth/gap \
${SDK_DIR}/inc/bluetooth/gap/gap_lib \
${SDK_DIR}/inc/bluetooth/profile \
${SDK_DIR}/inc/bluetooth/profile/server \
${SDK_DIR}/inc/os \
${SDK_DIR}/inc/os/FreeRTOS \
${SDK_DIR}/inc/os/system_trace \
${SDK_DIR}/inc/peripheral \
$(SDK_DIR)/inc/platform \
$(SDK_DIR)/inc/platform/cmsis \
$(SDK_DIR)/src/ble/privacy

#startup_rtl8762c_ARMCC.s
ASM_SOURCES = ${SDK_DIR}/src/mcu/rtl876x/arm/startup_rtl8762c.s

# all c sourcefiles
C_SOURCES = $(SDK_SOURCES) $(PROJ_SOURCES)
# C includes
C_INCLUDES =-I$(BUILD_DIR) $(addprefix -I, $(PROJ_INCS)) $(addprefix -I, $(SDK_INCS))
# sources END

# ******************************************************************************************/
# 3. mdk arm complier
# ******************************************************************************************/
# a).在系统环境变量中设定编译器地址,如D:D:\Keil_v5\ARM\ARMCC;
ifneq ($(shell type armcc.exe | grep "not found"),"")
ARMCC = armcc.exe
ARMASM = armasm.exe
ARMLINK = armlink.exe
FROMELF = fromelf.exe
# b).单独设定MDK_ARM_PATH
else
KEIL_MDK_PATH ?= /mnt/d/keil/

ifeq ($(KEIL_MDK_PATH), )
$(error "无法找到编译器,请先设定KEIL编译器路径")
endif

ARMCC = $(KEIL_MDK_PATH)/ARM/ARMCC/bin/armcc.exe
ARMASM = $(KEIL_MDK_PATH)/ARM/ARMCC/bin/armasm.exe
ARMLINK = $(KEIL_MDK_PATH)/ARM/ARMCC/bin/armlink.exe
FROMELF = $(KEIL_MDK_PATH)/ARM/ARMCC/bin/fromelf.exe
endif

# ******************************************************************************************/
# ASFLAGS, CFLAGS, LDFLAGS
# ******************************************************************************************/
# macros 
# AS defines
AS_DEFS = 
# C defines
C_DEFS = -DARMCM4_FP

# AS includes
AS_INCLUDES = \

PER_INCLUDE=  \
--preinclude $(SDK_DIR)/inc/platform/app_define.h \
--preinclude $(PROJ_DIR)/board/mem_config.h

KEIL_INCLUDE= \
-I$(MDK_ARM_PAHT)/ARM/PACK/ARM/CMSIS/5.7.0/Device\ARM\ARMCM4\Include \
-I$(MDK_ARM_PAHT)/ARM\CMSIS\Include

# compile flags
ASFLAGS = --cpu Cortex-M4.fp.sp --li -g --16 --apcs=interwork $(C_INCLUDES) --cpreproc $(KEIL_INCLUDE) --pd "ARMCM4_FP SETA 1"
CFLAGS  = --c99 -c --cpu Cortex-M4.fp.sp --li -g -O2 --apcs=interwork $(C_DEFS) $(C_INCLUDES) --gnu $(PER_INCLUDE) $(KEIL_INCLUDE)

# LDFLAGS
LIBS = \
$(SDK_DIR)/bin/ROM.lib \
$(SDK_DIR)/bin/bee2_sdk.lib \
$(SDK_DIR)/bin/gap_utils.lib \
$(SDK_DIR)/bin/msbc_lib.lib \
$(SDK_DIR)/bin/sbc_lib.lib \
$(SDK_DIR)/bin/ima_adpcm_lib.lib \
$(SDK_DIR)/bin/bee2_adc_lib.lib \
$(SDK_DIR)/bin/auto_k_rf_bonding_lib_DUT.lib \
$(SDK_DIR)/bin/auto_k_rf.lib

LDSCRIPT = --strict --scatter "$(SCTTER_FILE)"
LDMISC = --diag_suppress=L6314,L6312 --datacompressor off --summary_stderr --info summarysizes --map --load_addr_map_info --xref --callgraph --symbols
LDINFO = --info sizes --info totals --info unused --info veneers

# ******************************************************************************************/
# 主目标
# ******************************************************************************************/
# default action: build all
.PHONY : all
all:$(BUILD_DIR)/$(TARGET).axf
	@rm -rf $(BIN_DIR)
	@-mkdir $(BIN_DIR)
	@$(FROMELF) --bin --output $(BIN_DIR) $<
#	@$(FROMELF) -acd --interleave=source -o "$(BIN_DIR)/$(TARGET).disasm" $<
#	@$(SDK_DIR)/tool/prepend_header/prepend_header.exe -t app_code -p $(BIN_DIR)/app.bin -m 1 -i "$(PROJ_DIR)/board/mp.ini" -c sha256 -a "$(SDK_DIR)\tool\key.json"
	@$(SDK_DIR)/tool/prepend_header/prepend_header.exe -t app_code -p $(BIN_DIR)/app.bin -m 1 -i "$(PROJ_DIR)/board/mp.ini" -c sha256 -a "$(SDK_DIR)\tool\key.json"
	@$(SDK_DIR)/tool/md5/md5.exe "$(BIN_DIR)/app.bin" >> /dev/null
	@$(SDK_DIR)/tool/md5/md5.exe "$(BIN_DIR)/app_MP.bin"
	@rm "$(BIN_DIR)/app_MP.bin"

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))

# 编译所有asm源码
vpath %.s  $(dir $(ASM_SOURCES))	
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo "assembling $<..."
	@$(ARMASM) $(ASFLAGS) --list $(@:%.o=%.lst) --xref -o $@ --depend $(@:%.o=%.d) --depend_format=unix $<

# 编译所有c源码
vpath %.c  $(dir $(C_SOURCES))
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) FORCE
	@echo "compiling $<..."
#	@$(ARMCC) $(CFLAGS) -o $@ --omf_browse $(@:%.o=%.crf) --depend $(@:%.o=%.d) --depend_format=unix --no_depend_system_headers $<
	@$(ARMCC) $(CFLAGS) -o $@ --depend $(@:%.o=%.d) --depend_format=unix --no_depend_system_headers $<
# 链接
$(BUILD_DIR)/$(TARGET).axf: $(OBJECTS)
	@$(ARMLINK) --cpu=Cortex-M4.fp.sp $(LIBS) $(OBJECTS) $(LDSCRIPT) $(LDMISC) $(LDINFO) --list $(BUILD_DIR)/$(TARGET).map -o $(BUILD_DIR)/$(TARGET).axf

# 转换成.hex
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.axf | $(BUILD_DIR)
	@$(FROMELF) $< --i32combined --output $@

# 创建编译文件目录
$(BUILD_DIR):
	@-mkdir $@		

FORCE:
	@$(GET_VERSION)
#	@$(SDK_DIR)/tool/version_generation/git_generate_version.sh SRC-v $(SDK_DIR)/tool/version_generation $(PROJ_DIR)/board/version.h 

#######################################
# clean up
#######################################
clean:
	@-rm -fR $(BUILD_DIR)
	@-rm -fR $(BIN_DIR)
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
