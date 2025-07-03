########################################################################
# 默认变量
########################################################################
BUILD := build
SRC_C = 
SRC_S = 
INC =

########################################################################
# 项目相关配置
########################################################################
PROJECT ?= 827x_ble_remote
PROJECT_DIR = vendor/827x_ble_remote
SDK_DIR = .
# 项目特定配置
-include $(PROJECT_DIR)/flags.mk
# 芯片类型
FLAG_MCU_TYPE ?= -DCHIP_TYPE=CHIP_TYPE_8278
# 休眠时RAM保存范围
#MCU_STARTUP_FLAGS := -DMCU_STARTUP_8278
FLAG_MCU_STARTUP ?= -DMCU_STARTUP_8271_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8278_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271_RET_16K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271
# 项目特定
PROJECT_FLAG ?= 
PROJECT_FLAG += -DVERSION=$(VERSION)
PROJECT_FLAG += -DAPP_CONFIG_FILE=$(PROJECT_DIR)/app_config.h
# 链接脚本
PROJECT_LINK ?= $(SDK_DIR)/boot.link
# LD_FLAG  := boot/boot_32k_retn_8278.link
# LD_FLAG  := boot/boot_32k_retn_8271.link
#   项目源码
# SRC_PROJECT ?= $(wildcard $(PROJECT_DIR)/*.c)
#   启动代码
SRC_STARTUP ?= $(SDK_DIR)/boot/8271/cstartup_8271_RET_32K.S
OBJ_STARTUP ?= $(addprefix $(BUILD)/, $(notdir $(SRC_STARTUP:.S=.o)))
OUT_ELF ?= $(PROJECT)
OUT_BIN ?= $(PROJECT)

########################################################################
# 版本管理
########################################################################
TAG_COMMIT := $(shell git rev-list --abbrev-commit --tags --max-count=1)
TAG := $(shell git describe --abbrev=0 --tags ${TAG_COMMIT} 2>/dev/null || true)
COMMIT := $(shell git rev-parse --short HEAD)
DATE := $(shell git log -1 --format=%cd --date=format:"%Y%m%d")
VERSION := $(TAG:v%=%)

ifneq ($(COMMIT), $(TAG_COMMIT))
    VERSION := $(VERSION)-next-$(COMMIT)-$(DATE)
endif

ifeq ($(VERSION),)
    VERSION := $(COMMIT)-$(DATA)
endif

ifneq ($(shell git status --porcelain),)
    VERSION := $(VERSION)-dirty
endif

########################################################################
# 源码
########################################################################
INC =
SRC_C = 
# 项目相关头文件
INC += $(addprefix -I$(PROJECT_DIR)/,$(PROJECT_INC))
# 项目相关源码
SRC_C += $(addprefix $(PROJECT_DIR)/,$(PROJECT_SRC))
# SDK头文件
INC += -I$(SDK_DIR)
INC += -I$(SDK_DIR)/drivers/8258
INC += -I$(SDK_DIR)/vendor/common
INC += -I$(SDK_DIR)/common
INC += -I$(SDK_DIR)/common/config
INC += -I$(SDK_DIR)/application/app
INC += -I$(SDK_DIR)/application/audio
INC += -I$(SDK_DIR)/application/keyboard
INC += -I$(SDK_DIR)/application/print
INC += -I$(SDK_DIR)/application/usbstd
INC += -I$(SDK_DIR)/drivers/8258
INC += -I$(SDK_DIR)/drivers/8258/flash
# SDK源代码
SRC_C += $(wildcard $(SDK_DIR)/vendor/common/*.c)
SRC_C += $(wildcard $(SDK_DIR)/application/app/*.c)
SRC_C += $(wildcard $(SDK_DIR)/application/audio/*.c)
SRC_C += $(wildcard $(SDK_DIR)/application/keyboard/*.c)
SRC_C += $(wildcard $(SDK_DIR)/application/print/*.c)
SRC_C += $(wildcard $(SDK_DIR)/application/usbstd/*.c)
SRC_C += $(wildcard $(SDK_DIR)/common/*.c)
SRC_C += $(wildcard $(SDK_DIR)/drivers/8278/*.c)
SRC_DIR = $(dir $(SRC_C))
# OBJS文件
OBJS = $(addprefix $(BUILD)/, $(notdir $(SRC_C:.c=.o)))
# 静态链接库
LIBS ?= -llt_8278 -lfirmware_encrypt
# 编译器标志
CFLAGS = 
CFLAGS += $(INC)
CFLAGS += $(FLAG_MCU_TYPE)
CFLAGS += $(FLAG_MCU_STARTUP)
CFLAGS += $(PROJECT_FLAG)
CFLAGS += -ffunction-sections -fdata-sections -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions
########################################################################
# 设置输出路径
########################################################################
$(if $(shell if [ ! -d "$(PROJECT_DIR)" ]; then echo "1"; else echo ""; fi;), $(error "project not exist"))
$(shell if [ ! -d "$(BUILD)" ]; then mkdir $(BUILD); fi;)

########################################################################
# all target
########################################################################
all: $(BUILD)/$(PROJECT).elf secondary-outputs

$(BUILD)/$(PROJECT).elf: $(OBJS) $(BUILD)/div_mod.o $(OBJ_STARTUP) 
	@echo 'Building target: $@'
	@tc32-elf-ld --gc-sections -L$(SDK_DIR)/proj_lib -L$(SDK_DIR)/application/audio -T $(PROJECT_LINK) -o"$(BUILD)/$(PROJECT).elf" $(OBJS) $(BUILD)/div_mod.o $(OBJ_STARTUP) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build	

vpath %.c $(dir $(SRC_C))
$(BUILD)/%.o: %.c | $(PROJECT_DIR)/app_config.h
	@echo "build $<"
	@tc32-elf-gcc $(CFLAGS) -c -o"$@" "$<"

$(BUILD)/div_mod.o:$(SDK_DIR)/div_mod.S
	@echo 'Building file: $<'
	@tc32-elf-gcc $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished building: $<'

$(OBJ_STARTUP):$(SRC_STARTUP)
	@echo 'Building file: $<'
	@tc32-elf-gcc $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished building: $<'

post-build:
#   @"$(SDK_DIR)/tl_check_fw.sh" $(BUILD)/$(VERSION) $(BUILD)/$(PROJECT)
	@tc32-elf-objcopy -v -O binary $(BUILD)/$(PROJECT).elf $(BUILD)/$(VERSION).bin
	@"$(SDK_DIR)/tools/check_fw" $(BUILD)/$(VERSION).bin
#	@"$(SDK_DIR)/check_fw" $(BUILD)/$(VERSION).bin FW_ENC_KEY
	@echo ' '

secondary-outputs: $(BUILD)/$(PROJECT).lst  sizedummy

$(BUILD)/$(PROJECT).lst: $(BUILD)/$(PROJECT).elf
	@echo 'Invoking: TC32 Create Extended Listing'
	@tc32-elf-objdump -x -D -l -S $(BUILD)/$(PROJECT).elf  >"$(BUILD)/$(PROJECT).lst"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy:  $(BUILD)/$(PROJECT).elf
	@echo 'Invoking: Print Size'
	@tc32-elf-size -t $(BUILD)/$(PROJECT).elf
	@echo 'Finished building: $@'
	@echo ' '

.PHONY: clean reset erase flash
clean:
	@rm -rf $(BUILD)/*

# bdt download tools
# $(BDT_TOOL) 需要在系统设置BDT_TOOL路径
BDT_TOOL ?= $(SDK_DIR)/tools/tcdb

# 检查是否是在wsl环境
kernel_release = $(shell uname -r)
#   :wsl中运行
ifeq ($(findstring microsoft,$(kernel_release)),microsoft)
	is_wsl_host:=1
#   :非wsl中运行
else 
	is_wsl_host:=0
endif


reset:
ifeq ($(is_wsl_host),1)
	@$(BDT_TOOL) 1 8278 rst -f
else
	@$(BDT_TOOL) 8278 rst -f
endif

erase:
ifeq ($(is_wsl_host),1)
# active
	@$(BDT_TOOL) 1 8278 ac
# erase
	@$(BDT_TOOL) 1 8278 wf 0 -s 256k -e
else 
# active
	@$(BDT_TOOL) 8278 activate
# erase
	@$(BDT_TOOL) 8278 wf 0 -s 256k -e
endif

flash:
ifeq ($(is_wsl_host),1)
# active
	@$(BDT_TOOL) 1 8278 ac
# download
	@$(BDT_TOOL) 1 8278 wf 0 -i "$(BUILD)/$(VERSION).bin" 
# reset
	@$(BDT_TOOL) 1 8278 rst -f
else
# active
	@$(BDT_TOOL) 8278 activate
# download
	@$(BDT_TOOL) 8278 wf 0 -i "$(BUILD)/$(VERSION).bin" 
# reset
	@$(BDT_TOOL) 8278 rst -f
endif
