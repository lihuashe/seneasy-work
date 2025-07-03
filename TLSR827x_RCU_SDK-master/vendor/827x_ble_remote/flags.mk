# 项目源码
PROJECT_SRC = \
	main.c \
	app.c \
	gap.c \
	app_att.c \
	batt_module/battery_check.c \
	ir_module/app_ir.c \
	ir_module/rc_ir.c \
	key_module/app_ui.c \
	test_module/app_phytest.c \
	test_module/app_ptm.c \
	test_module/app_test.c \
	test_module/seneasy_rcu_service.c \
	utilities/app_ecdsa.c \
	utilities/app_fifo_byte.c \
	utilities/app_flash_write.c \
	utilities/app_pke.c \
	utilities/sha256.c \
	voice_module/app_audio.c
# 项目头文件
PROJECT_INC = \
	. \
	batt_module \
	ir_module \
	key_module \
	ota_module \
	test_module \
	utilities \
	voice_module
	
# 芯片类型
# FLAG_MCU_TYPE ?= -DCHIP_TYPE=CHIP_TYPE_8278
# 休眠时RAM保存范围
#MCU_STARTUP_FLAGS := -DMCU_STARTUP_8278
# FLAG_MCU_STARTUP ?= -DMCU_STARTUP_8271_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8278_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271_RET_16K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271_RET_32K
# MCU_STARTUP_FLAGS := -DMCU_STARTUP_8271
# 项目特定
PROJECT_FLAG ?= -D__PROJECT_8278_BLE_REMOTE__=1
# PROJECT_FLAG += -DVERSION=$(VERSION)
# 链接脚本
# PROJECT_LINK ?= boot.link
# LD_FLAG  := boot/boot_32k_retn_8278.link
# LD_FLAG  := boot/boot_32k_retn_8271.link

#   启动代码
# SRC_STARTUP ?= boot/8271/cstartup_8271_RET_32K.S
# OBJ_STARTUP ?= $(addprefix $(BUILD)/, $(notdir $(SRC_STARTUP:.S=.o)))
# OUT_ELF ?= $(PROJECT)
# OUT_BIN ?= $(PROJECT)