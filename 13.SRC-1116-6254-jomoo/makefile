
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
PROJECT_DIR = .
OUT_ELF ?= $(PROJECT)
OUT_BIN ?= $(PROJECT)

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

# 如果没有一个有效的TAG,则使用默认名称:rcu-00.00
ifeq ($(TAG),)
	TAG := rcu-00.00
endif

VERSION ?= $(TAG)
# 名称
VERSION_NAME  := $(word 1,$(subst V, ,$(subst ., ,$(VERSION))))
# 主版本号
VERSION_MAJOR := $(word 2,$(subst V, ,$(subst ., ,$(VERSION))))
# 此版本号
VERSION_MINOR := $(word 3,$(subst V, ,$(subst ., ,$(VERSION))))
# build版本: finds the most recent tag that is reachable from a commit.
VERSION_REVISION := $(word 2,$(subst -, ,$(shell git describe 2>/dev/null)))

OTA_VERSION = $(shell printf $(VERSION_NAME)"V%d.%d" $(VERSION_MAJOR) $(shell expr $(VERSION_MINOR) + 1))

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
FLAG_MCU_TYPE ?= -DMCU_CORE_B80=1 
FLAG_ROM_TYPE ?= -DMCU_STARTUP_FLASH

# 项目特定
FLAG_PROJECT += -D_PRJ_SENEASY=1
FLAG_PROJECT += -DVERSION=$(VERSION) -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_REVISION=$(VERSION_REVISION)
FLAG_PROJECT += -DFW_VERSION_NUM=V$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_REVISION)
######################################################################################
# 链接脚本及选项
######################################################################################
# 链接脚本
PROJECT_LINK ?= boot.link

#   启动代码
SRC_STARTUP ?= chip/B80/boot/cstartup_flash.S \
			   chip/B80/boot/cstartup_otp.S

OBJ_STARTUP ?= $(addprefix $(BUILD)/, $(SRC_STARTUP:%.S=%.o))

# 静态链接库
LIBS ?= -Lchip/B80/drivers/lib -ldriver_flash
######################################################################################
# 源码和目录
######################################################################################
# 项目源码目录
# app_exclude_dirs = $(shell find demo -maxdepth 2 -type d)
# app_dirs:=$(shell find $(PROJECT_DIR) -maxdepth 3 -type d)
# app_dirs:=$(basename $(patsubst ./%,%,$(app_dirs)))
# app_dirs := $(filter-out $(app_exclude_dirs),$(app_dirs))

app_dirs :=
app_dirs += $(shell find app -maxdepth 4 -type d)
app_dirs += $(shell find chip -maxdepth 4 -type d)
app_dirs += $(shell find common -maxdepth 4 -type d)
app_dirs:=$(basename $(patsubst ./%,%,$(app_dirs)))
APP_SRC := $(foreach sdir,$(app_dirs),$(wildcard $(sdir)/*.c))
APP_INC := $(addprefix -I, $(app_dirs))

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

# div_mod
SRC_DIV := common/div_mod.S
OBJS_DIV := $(addprefix $(BUILD)/,$(SRC_DIV:%.S=%.o))

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
		  -fpack-struct -fshort-enums \
		  -finline-small-functions \
		  -std=gnu99 \
		  -fshort-wchar \
		  -fms-extensions 

######################################################################################
# 检查和创建项目输出目录
######################################################################################
$(if $(shell if [ ! -d "$(PROJECT_DIR)" ]; then echo "1"; else echo ""; fi;), $(error "project not exist"))
$(shell if [ ! -d "$(BUILD)" ]; then mkdir $(BUILD); fi;)
$(foreach out_dir,$(app_dirs),$(shell if [ ! -d "$(BUILD)/$(out_dir)" ]; then mkdir $(BUILD)/$(out_dir); fi;))

# all target
all: $(BUILD)/$(PROJECT).elf secondary-outputs

$(BUILD)/$(PROJECT).elf: $(OBJS) $(OBJ_STARTUP) $(OBJS_DIV)
	@echo 'Building target: $@'
	@tc32-elf-ld --gc-sections -T $(PROJECT_LINK) -o"$(BUILD)/$(PROJECT).elf" $(OBJS) $(OBJ_STARTUP) $(OBJS_DIV) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build	

-include $(BUILD)/*.d
vpath %.c $(dir $(SRC_C))
$(BUILD)/%.o: %.c
	@echo "build $<"
	@tc32-elf-gcc $(CFLAGS) -c -MMD -o"$@" "$<"

vpath %.S $(dir $(SRC_STARTUP))
$(BUILD)/%.o: %.S
	@echo 'Building file: $<'
	@tc32-elf-gcc $(FLAG_ROM_TYPE) $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished building: $<'

vpath %.S $(dir $(SRC_DIV))
$(BUILD)/.o: %.S
	@echo 'Building file: $<'
	@tc32-elf-gcc $(FLAG_ROM_TYPE) -c -o"$@" "$<"
	@echo 'Finished building: $<'

post-build:
	@"tools/tl_check_fw.sh" $(BUILD)/$(VERSION) $(BUILD)/$(PROJECT)
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

# --------------------------------------------------
# bdt download tools
#	1. 设置系统环境变量 BDT_TOOL="Telink烧录工具安装位置",如:
#		export BDT_TOOL='/mnt/d/mcu/telink/BDT_v5.7.3/config/Cmd_download_tool.exe'
#	2. 指定芯片的型号,如:B80/8258/8278
# --------------------------------------------------
MCU_TYPE = B80
BDT_TOOL = /mnt/c/Telink_sdk/BDT/BDT/config/Cmd_download_tool.exe
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

#链接下载
down:  
	make -j12 && make flash -j12


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
