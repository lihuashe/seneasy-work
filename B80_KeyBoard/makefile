
######################################################################################
# 配置编译环境
######################################################################################
BUILD := build
SRC_C = 
SRC_S = 
INC =

######################################################################################
# 选择项目名称及路径
######################################################################################
PROJECT ?= rcu
PROJECT_DIR = ./app

######################################################################################
# SDK目录
######################################################################################
SDK_DIR = ./sdk

######################################################################################
# 版本管理: 
# 	示例: [name]-[01].[02].[10],name为版本自定义名称(任意字符串),01.02.10分别为M.N.b版本
# 使用方法:
# 	make 			//使用当前分支的最新tag定义版本
#   make -e OTA=1	//使用当前分支的最新tag,次版本自动+1生产版本
######################################################################################
# TAG_COMMIT := $(shell git rev-list --abbrev-commit --tags --max-count=1)
# TAG := $(shell git describe --abbrev=0 --tags ${TAG_COMMIT} 2>/dev/null || true)
TAG    := $(shell git describe)
COMMIT := $(shell git rev-parse --short HEAD)
DATE   := $(shell git log -1 --format=%cd --date=format:"%Y%m%d")

# 如果没有一个有效的TAG,则使用默认名称:rcu-00.00
ifeq ($(TAG),)
	TAG := rcu-00.00
endif

VERSION ?= $(TAG)
# 名称
VERSION_NAME  := $(word 1,$(subst ., ,$(subst ., ,$(VERSION))))
# 主版本号
VERSION_MAJOR := $(word 2,$(subst ., ,$(subst ., ,$(VERSION))))
# 此版本号
VERSION_MINOR := $(word 3,$(subst -, ,$(subst ., ,$(VERSION))))
# build版本: finds the most recent tag that is reachable from a commit.
VERSION_REVISION := $(word 2,$(subst ., ,$(shell git describe 2>/dev/null)))

OTA_VERSION = $(shell printf $(VERSION_NAME)".%d.%d" $(VERSION_MAJOR) $(shell expr $(VERSION_MINOR) + 1))

$(info -------------------------------------------------------)
$(info VERSION: $(VERSION))	
$(info OTA:     $(OTA_VERSION))
$(info MAJOR:   $(VERSION_MAJOR))
$(info MINOR:   $(VERSION_MINOR))
$(info BUILD:   $(VERSION_REVISION))
$(info -------------------------------------------------------)
# 编译OTA版本
ifneq ($(OTA),)
	VERSION = $(OTA_VERSION)
endif

# 未定义的版本信息
ifeq ($(VERSION),)
    VERSION := $(PROJECT)-$(COMMIT)-$(DATA)
endif
# 有未提交的代码
ifneq ($(shell git status --porcelain),)
    VERSION := $(VERSION)-dirty
endif

######################################################################################
# C FLAGS配置
######################################################################################
# 项目特定编译选项配置
-include $(PROJECT_DIR)/flags.mk

# 芯片类型
FLAG_MCU_TYPE ?= -DCHIP_TYPE=CHIP_TYPE_8208
# 休眠时RAM保存范围
FLAG_MCU_STARTUP ?= -DMCU_STARTUP_FLASH

# 项目特定
FLAG_PROJECT ?= -D__PROJECT_B80_APP_CONFIG__=1
FLAG_PROJECT += -DVERSION=$(VERSION) -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_REVISION=$(VERSION_REVISION)
######################################################################################
# 链接脚本及选项
######################################################################################
# 链接脚本
PROJECT_LINK ?= $(SDK_DIR)/boot.link

#   项目源码
SRC_PROJECT ?= $(wildcard $(PROJECT_DIR)/*.c)
#   启动代码
SRC_STARTUP ?= $(SDK_DIR)/chip/B80/boot/cstartup_flash.S
OBJ_STARTUP ?= $(addprefix $(BUILD)/, $(notdir $(SRC_STARTUP:.S=.o)))
OUT_ELF ?= $(PROJECT)
OUT_BIN ?= $(PROJECT)

# 静态链接库
LIBS ?= -L$(SDK_DIR)/chip/B80/proj_lib -llt_8208 -lfirmware_encrypt -L$(PROJECT_DIR)/firefly_link_layer -ltl_tpll

######################################################################################
# 源码和目录
######################################################################################
# 项目源码目录
INC  =
SRC_C  = 
INC += -I$(PROJECT_DIR)
SRC_C += $(SRC_PROJECT)

# 头文件
INC += -I"."
INC += -I"$(SDK_DIR)/chip/B80/drivers"
INC += -I$(SDK_DIR)/vendor/common
INC += -I$(SDK_DIR)/common
INC += -I$(SDK_DIR)/common/config
INC += -I$(PROJECT_DIR)/application/keyboard
INC += -I$(PROJECT_DIR)/application/usbstd
INC += -I$(PROJECT_DIR)/*_module
INC += -I$(PROJECT_DIR)/firefly_link_layer
INC += -I$(SDK_DIR)/chip/B80/drivers
INC += -I$(SDK_DIR)/chip/B80/drivers/flash
INC += -I$(SDK_DIR)/chip/B80/drivers/ext_driver

# 源代码
SRC_C += $(wildcard $(SDK_DIR)/vendor/common/*.c)
SRC_C += $(wildcard $(PROJECT_DIR)/application/keyboard/*.c)
SRC_C += $(wildcard $(PROJECT_DIR)/application/usbstd/*.c)
SRC_C += $(wildcard $(PROJECT_DIR)/*_module/*.c)
SRC_C += $(wildcard $(PROJECT_DIR)/firefly_link_layer/*.c)
SRC_C += $(wildcard $(SDK_DIR)/common/*.c)
SRC_C += $(wildcard $(SDK_DIR)/chip/B80/drivers/*.c)
SRC_C += $(wildcard $(SDK_DIR)/chip/B80/drivers/flash/*.c)
SRC_C += $(wildcard $(SDK_DIR)/chip/B80/drivers/ext_driver/*.c)
SRC_DIR = $(dir $(SRC_C))

# OBJS文件
OBJS = $(addprefix $(BUILD)/, $(notdir $(SRC_C:.c=.o)))
DEPENDS = $(addprefix $(BUILD)/,$(SRC_C:%.c=%.d))
######################################################################################
# 编译器标志
######################################################################################
CFLAGS = 
CFLAGS += $(INC)
CFLAGS += $(FLAG_MCU_TYPE)
CFLAGS += $(FLAG_MCU_STARTUP)
CFLAGS += $(FLAG_PROJECT)
CFLAGS += -ffunction-sections \
		  -fdata-sections \
		  -Wall \
		  -O2 \
		  -fpack-struct \
		  -fshort-enums \
		  -finline-small-functions \
		  -std=gnu99 \
		  -fshort-wchar \
		  -fms-extensions

######################################################################################
# 检查和创建项目输出目录
######################################################################################
$(if $(shell if [ ! -d "$(PROJECT_DIR)" ]; then echo "1"; else echo ""; fi;), $(error "project not exist"))
$(shell if [ ! -d "$(BUILD)" ]; then mkdir $(BUILD); fi;)
# $(shell if [ ! -f "$(BUILD)/app_config.h" ]; then cp $(PROJECT_DIR)/app_config.h $(BUILD)/app_config.h; fi;)

# all target
all: $(BUILD)/$(PROJECT).elf secondary-outputs

$(BUILD)/$(PROJECT).elf: $(OBJS) $(BUILD)/div_mod.o $(OBJ_STARTUP) 
	@echo 'Building target: $@'
	@tc32-elf-ld --gc-sections -L"./proj_lib" -L"./application/audio" -T $(PROJECT_LINK) -o"$(BUILD)/$(PROJECT).elf" $(OBJS) $(BUILD)/div_mod.o $(OBJ_STARTUP) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build	

-include $(BUILD)/*.d
vpath %.c $(dir $(SRC_C))
$(BUILD)/%.o: %.c | $(PROJECT_DIR)/app_config.h
	@echo "build $<"
	@tc32-elf-gcc $(CFLAGS) -c -MMD -o"$@" "$<"

$(BUILD)/div_mod.o:$(SDK_DIR)/div_mod.S
	@echo 'Building file: $<'
	@tc32-elf-gcc $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished building: $<'

$(OBJ_STARTUP):$(SRC_STARTUP)
	@echo 'Building file: $<'
	@tc32-elf-gcc $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished building: $<'

post-build:
	@"./sdk/tools/tl_check_fw.sh" $(BUILD)/$(VERSION) $(BUILD)/$(PROJECT)
	@echo ' '

secondary-outputs: $(BUILD)/$(PROJECT).lst  sizedummy

$(BUILD)/$(PROJECT).lst: $(BUILD)/$(PROJECT).elf
	@echo 'Invoking: TC32 Create Extended Listing'
	@tc32-elf-objdump -x -D -l -S $(BUILD)/$(PROJECT).elf  >"$(BUILD)/$(PROJECT).lst"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy:  $(BUILD)/$(PROJECT).elf
	@echo 'Invoking: Print Size'
	tc32-elf-size -t $(BUILD)/$(PROJECT).elf
	@echo 'Finished building: $@'
	@echo ' '

.PHONY: clean reset erase flash
clean:
	@rm -rf $(BUILD)/*
down:
	@rm -rf $(BUILD)/fyll*.* && make && make flash
 
# --------------------------------------------------
# bdt download tools
#	1. 设置系统环境变量 BDT_TOOL="Telink烧录工具安装位置",如:
#		export BDT_TOOL='/mnt/d/mcu/telink/BDT_v5.7.3/config/Cmd_download_tool.exe'
#	2. 指定芯片的型号,如:B80/8258/8278
# --------------------------------------------------
MCU_TYPE = B80
BDT_TOOL_DIR = $(dir $(BDT_TOOL))

# 检查是否是在wsl环境
kernel_release = $(shell uname -r)
#   :wsl中运行
ifeq ($(findstring microsoft,$(kernel_release)),microsoft)
	is_wsl_host:=1
#   :非wsl中运行
else 
	is_wsl_host:=0
endif

# 复位芯片
reset:
	@cd $(BDT_TOOL_DIR) && $(BDT_TOOL) 1 $(MCU_TYPE) rst -f

# 擦除整个flash
erase:
# active
	@cd $(BDT_TOOL_DIR) && $(BDT_TOOL) 1 $(MCU_TYPE) ac
# erase
	@cd $(BDT_TOOL_DIR) && $(BDT_TOOL) 1 $(MCU_TYPE) wf 0 -s 256k -e

# 烧录
flash:
# temporary copy the bin file to windows 
	@cp $(BUILD)/$(VERSION).bin	$(BDT_TOOL_DIR)
# active
	@cd $(BDT_TOOL_DIR) && $(BDT_TOOL) 1 $(MCU_TYPE) ac
# unlock
	@cd $(BDT_TOOL_DIR) && $(BDT_TOOL) 1 $(MCU_TYPE) lf 0 0 
# download
	@cd $(BDT_TOOL_DIR) && $(BDT_TOOL) 1 $(MCU_TYPE) wf 0 -i "$(VERSION).bin" 
# reset
	@cd $(BDT_TOOL_DIR) && $(BDT_TOOL) 1 $(MCU_TYPE) rst -f
# delete bin file
	@cd $(BDT_TOOL_DIR) && rm $(VERSION).bin

# 串口log
monitor:
	@cd $(BDT_TOOL_DIR) && cmd.exe /c python -m serial.tools.miniterm COM56 115200