############################################################################################
#
# XIHAO makefile
#
############################################################################################


# Optimization: 0, 1, 2, 3, s, ultrasize
ifeq ($(CONFIG_OPTIMIZATION_LEVEL), 0)
OPTIMIZE = 0
else ifeq ($(CONFIG_OPTIMIZATION_LEVEL), 1)
OPTIMIZE = 1
else ifeq ($(CONFIG_OPTIMIZATION_LEVEL), 2)
OPTIMIZE = 2
else ifeq ($(CONFIG_OPTIMIZATION_LEVEL), 4)
OPTIMIZE = ultrasize
else ifeq ($(CONFIG_OPTIMIZATION_LEVEL), 7)
OPTIMIZE = ultrasize
else
OPTIMIZE = s
endif

# CPU
CPU		= -mthumb -mcpu=cortex-m4 # -mfloat-abi=hard -mfpu=fpv4-sp-d16 # M4 (Use Hard FPU)

# list ASM source files headers
ASMSRC = \
	$(ASM_SOURCES)

# source files groups and paths
SRCPATHS = $(dir $(SOURCES)) $(dir $(ASMSRC))

# build obj files direcotry (move to Makefile)
#BUILDDIR	= .build

############################################################################################

# verbose mode
ifeq ($(V),1)
TRACE_CC  =
TRACE_LD  =
TRACE_AS  =
TRACE_AR  =
TRACE_STRIP =
Q=
else
TRACE_CC  = @echo "	CC    $<"
TRACE_AS  = @echo "	AS    $<"
TRACE_LD  = @echo "Linking $@ ..."
TRACE_AR  = @echo " AR    $<"
TRACE_STRIP = @echo "STRIP     $<"
Q=@
endif

# Target thing
TARGET					:= $(PROJECT)
TARGET_LIB				:= $(TARGET).a
TARGET_ELF	 			:= $(TARGET).elf
TARGET_BIN				:= $(TARGET).bin
TARGET_HEX				:= $(TARGET).hex
TARGET_MAP				:= $(TARGET).map
TARGET_DIS				:= $(TARGET).dis
OBJCPFLAGS_ELF_TO_BIN	 = -Obinary
OBJCPFLAGS_ELF_TO_HEX	 = -Oihex
OBJCPFLAGS_HEX_TO_BIN	 = -Obinary -Iihex

# Tools
CC			= arm-none-eabi-gcc
AS			= arm-none-eabi-as
LD			= arm-none-eabi-ld
AR			= arm-none-eabi-ar cr
STRIP		= arm-none-eabi-strip
OBJCP		= arm-none-eabi-objcopy
OBJDUMP		= arm-none-eabi-objdump
OBJSIZE		= arm-none-eabi-size

# Optimize
ifeq ($(OPTIMIZE), ultrasize)
CC_OPTIMIZE = -Os -fdata-sections -ffunction-sections
LD_OPTIMIZE = -Wl,--gc-sections
else
CC_OPTIMIZE = -O$(OPTIMIZE)
LD_OPTIMIZE =
endif

# Warnings
WARNINGS 	= -Wall -Wformat=0 -Wstrict-aliasing=0

INSTRUMENT_FUNCTIONS =

LDSCRIPTS=$(patsubst %, -T%,$(LDSCRIPT))

CPPFLAGS ?=
CCFLAGS  ?=
ASFLAGS  ?=
ASSFLAGS ?=
LDFLAGS  ?=

# Flags (-N: prevent link error in FLASH version: "not enough room for program headers")
CPPFLAGS	+= $(INCLUDES) $(DEFINES)
CCFLAGS		+= $(CPU) -g3 -std=gnu99 $(WARNINGS) $(CC_OPTIMIZE) $(INSTRUMENT_FUNCTIONS)
ASFLAGS		+= $(CPU)
LDFLAGS		+= -nostartfiles -static $(CCFLAGS) $(LD_OPTIMIZE) $(LDSCRIPTS) -Wl,-Map=$(TARGET_MAP),--cref -Wl,--print-memory-usage -N
#DEPFLAGS	= -MT $@ -MMD -MP -MF $*.d

# Librarys
LDLIBS		= --specs=nano.specs -lc -lnosys -lm -u _printf_float -Xlinker "-(" $(LDLIBS_ROM) -Xlinker "-)"

# Files
C_SOURCES=$(filter %.c, $(SOURCES))
S_SOURCES=$(filter %.s, $(SOURCES))
SS_SOURCES=$(filter %.S, $(SOURCES))
FILES_C_OBJ =  $(patsubst %, $(BUILDDIR)/%, $(filter %.o, $(C_SOURCES:%.c=%.o))) 
FILES_S_OBJ =  $(patsubst %, $(BUILDDIR)/%, $(filter %.o, $(S_SOURCES:%.s=%.o))) 
FILES_SS_OBJ =  $(patsubst %, $(BUILDDIR)/%, $(filter %.o, $(SS_SOURCES:%.S=%.o))) 
FILES_DEPEND = $(patsubst %, $(BUILDDIR)/%, $(filter %.d, $(SOURCES:%.c=%.d))) 

# Out file
ifeq ($(LDSCRIPT), )
OUTFILES = $(TARGET_LIB)

LIB_PATH=$(shell pwd)
LIB_BASE=$(shell cd ../../../; pwd)
LIB_DIR=$(shell realpath --relative-to=$(LIB_BASE) $(LIB_PATH))
LIB_OUT=../../../../../trunk/rwble/$(LIB_DIR)
else
OUTFILES = $(TARGET_HEX) $(TARGET_BIN)
endif

# Paths where to search for Sources
#VPATH=$(SRCPATHS)

# PHONY
.PHONY: all clean print

# Target
all: $(OUTFILES)
	
# lib
$(TARGET_LIB):$(FILES_C_OBJ) $(FILES_S_OBJ)
	$(Q)$(AR) $(TARGET_LIB) $^
	$(Q)$(STRIP) -S $(TARGET_LIB)
	@mkdir -p $(LIB_OUT) >/dev/null
	@cp -f $(TARGET_LIB) $(LIB_OUT)
	@echo "Build done"

# bin
$(TARGET_BIN) : $(TARGET_HEX)
	$(Q)$(OBJCP) $(OBJCPFLAGS_HEX_TO_BIN) $< $@
	@cp -f $(TARGET_BIN) a.bin
	@cp -f $(TARGET_ELF) a.axf
	@echo "\nEach object size:" >> $(TARGET_MAP)
	@find $(BUILDDIR) -name '*.o' | xargs $(OBJSIZE) -B -d >> $(TARGET_MAP)
	@echo "Build done"

# hex
$(TARGET_HEX) : $(TARGET_ELF)
	$(Q)$(OBJCP) $(OBJCPFLAGS_ELF_TO_HEX) $< $@

# elf
$(TARGET_ELF) : $(FILES_C_OBJ) $(FILES_S_OBJ) $(FILES_SS_OBJ)
	$(TRACE_LD)
	$(Q)$(CC) $+ $(LDFLAGS) $(LDLIBS) -o $@ -lm
	@echo "--------------------------------------------------"
	$(Q)$(OBJSIZE) -B -d $@
	$(Q)$(OBJDUMP) -d $@ > $(TARGET_DIS)

# c -> o
$(BUILDDIR)/%.o : $(SDK_BASE)%.c
$(BUILDDIR)/%.o : $(SDK_BASE)%.c $(BUILDDIR)/%.d
	$(TRACE_CC)
	@mkdir -p $(@D) >/dev/null
	$(Q)$(CC) -MT $@ -MMD -MP -MF $(BUILDDIR)/$*.Td $(CCFLAGS) $(CPPFLAGS) -c $< -o $@
	@mv -f $(BUILDDIR)/$*.Td $(BUILDDIR)/$*.d

# s -> o
$(BUILDDIR)/%.o : $(SDK_BASE)%.s
	$(TRACE_AS)
	@mkdir -p $(@D) >/dev/null
	$(Q)$(AS) $(ASFLAGS) -c $< -o $@

# S -> o
$(BUILDDIR)/%.o : $(SDK_BASE)%.S
	$(TRACE_AS)
	@mkdir -p $(@D) >/dev/null
	$(Q)$(CC) $(CCFLAGS) $(CPPFLAGS) $(ASSFLAGS) -c $< -o $@

# d
$(BUILDDIR)/%.d: ;
.PRECIOUS: $(BUILDDIR)/%.d

# Include dependent (*.d)
-include $(FILES_DEPEND)

# some info
print:
	@for f in $(SOURCES); do echo $$f; done
	@for f in $(INCLUDES); do echo $$f; done

# Clean
clean:
	$(Q)rm -rf $(BUILDDIR) $(TARGET_ELF) $(TARGET_BIN) $(TARGET_HEX) $(TARGET_MAP) $(TARGET_DIS) $(TARGET_LIB) a.bin a.axf
#	$(Q)find -type f -regex ".*\.\(hex\|bin\|elf\|axf\|map\|o\|d\|Td\)$$" | xargs rm -f

