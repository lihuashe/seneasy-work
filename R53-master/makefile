
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
PROJECT_DIR = ./project
OUT_ELF ?= $(PROJECT)
OUT_BIN ?= $(PROJECT)

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
TAG := $(shell git describe 2>/dev/null)
COMMIT := $(shell git rev-parse --short HEAD 2>/dev/null)
DATE := $(shell git log -1 --format=%cd --date=format:"%Y%m%d" 2>/dev/null)


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

OTA_VERSION = $(shell printf $(VERSION_NAME)".%02d.%02d" $(VERSION_MAJOR) $(shell expr $(VERSION_MINOR) + 1))

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
FLAG_MCU_TYPE ?= -DCHIP_TYPE=CHIP_TYPE_827x
# 休眠时RAM保存范围
#MCU_STARTUP_FLAGS := -DMCU_STARTUP_8278
FLAG_MCU_STARTUP ?= -DMCU_STARTUP_8271_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8278_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271_RET_16K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271
# 项目特定
FLAG_PROJECT ?= -DAPP_CONFIG_H=1
FLAG_PROJECT ?= -DAPP_CONFIG_PATH=\"../../../$(PROJECT)/app_config.h\"
FLAG_PROJECT ?= -include $(PROJECT_DIR)/app_config.h
FLAG_PROJECT += -DVERSION=$(VERSION) -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_REVISION=$(VERSION_REVISION)

######################################################################################
# 链接脚本及选项
######################################################################################
# 链接脚本
PROJECT_LINK ?= $(SDK_DIR)/boot.link
# LD_FLAG  := boot/boot_32k_retn_8278.link
# LD_FLAG  := boot/boot_32k_retn_8271.link
#   启动代码
SRC_STARTUP ?= $(SDK_DIR)/boot/8271/cstartup_8271_RET_32K.S
OBJ_STARTUP ?= $(addprefix $(BUILD)/, $(notdir $(SRC_STARTUP:.S=.o)))
# 静态链接库
LIBS ?= -L$(SDK_DIR)/proj_lib -llt_827x -lfirmware_encrypt -lapp_ota -llt_general_stack
LIBS += -L$(PROJECT_DIR)/tvssu_module -lTL_SZ119_TVSSU_V01_20240119_1

######################################################################################
# 源码和目录
######################################################################################
# 项目源码目录
app_exclude_dirs =
app_dirs:=$(shell find $(PROJECT_DIR) -maxdepth 1 -type d)
app_dirs:=$(basename $(patsubst ./%,%,$(app_dirs)))
app_dirs := $(filter-out $(app_exclude_dirs),$(app_dirs))
APP_SRC := $(foreach sdir,$(app_dirs),$(wildcard $(sdir)/*.c))
APP_INC := $(addprefix -I, $(app_dirs))
# SDK源码目录
sdk_exclude_dirs  = $(shell find $(SDK_DIR)/8271_ble_remote -maxdepth 2 -type d)
sdk_exclude_dirs += $(shell find $(SDK_DIR)/vendor/827x_ble_remote -maxdepth 2 -type d)
sdk_exclude_dirs += $(shell find $(SDK_DIR)/drivers/8258 -maxdepth 2 -type d)
sdk_exclude_dirs := $(basename $(patsubst ./%,%,$(sdk_exclude_dirs)))
sdk_dirs := $(shell find $(SDK_DIR) -maxdepth 3 -type d)
sdk_dirs := $(basename $(patsubst ./%,%,$(sdk_dirs)))
sdk_dirs := $(filter-out $(sdk_exclude_dirs),$(sdk_dirs))
SDK_SRC := $(foreach mdir,$(sdk_dirs),$(wildcard $(mdir)/*.c))
SDK_INC := $(addprefix -I,$(sdk_dirs))
# #test
# $(info "project src and inc:")
# $(info $(APP_SRC))
# $(info $(APP_INC))
# $(info "sdk src and inc:")
# $(info $(SDK_SRC))
# $(info $(SDK_INC))
# OBJS文件
SRC_C = $(APP_SRC) $(SDK_SRC)
INC = $(SDK_INC) $(APP_INC)
OBJS = $(addprefix $(BUILD)/,$(SRC_C:%.c=%.o))
DEPENDS = $(addprefix $(BUILD)/,$(SRC_C:%.c=%.d))

######################################################################################
# 编译器标志
######################################################################################
CFLAGS = 
CFLAGS += $(INC)
CFLAGS += $(FLAG_MCU_TYPE)
CFLAGS += $(FLAG_MCU_STARTUP)
CFLAGS += $(FLAG_PROJECT)
CFLAGS += -ffunction-sections -fdata-sections -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions

######################################################################################
# 检查和创建项目输出目录
######################################################################################
$(if $(shell if [ ! -d "$(PROJECT_DIR)" ]; then echo "1"; else echo ""; fi;), $(error "project not exist"))
$(shell if [ ! -d "$(BUILD)" ]; then mkdir $(BUILD); fi;)
$(foreach out_dir,$(app_dirs),$(shell if [ ! -d "$(BUILD)/$(out_dir)" ]; then mkdir $(BUILD)/$(out_dir); fi;))
$(shell if [ ! -d "$(BUILD)/sdk" ]; then mkdir $(BUILD)/sdk; fi;)
$(foreach out_dir,$(sdk_dirs),$(shell if [ ! -d "$(BUILD)/$(out_dir)" ]; then mkdir $(BUILD)/$(out_dir); fi;))

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
	@"./$(SDK_DIR)/tl_check_fw.sh" $(BUILD)/$(VERSION) $(BUILD)/$(PROJECT)
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

# bdt download tools
# $(BDT_TOOL) 需要在系统设置BDT_TOOL路径

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
