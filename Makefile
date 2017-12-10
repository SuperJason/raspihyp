#
# Foot Hypervisor Version
#
VERSION_MAJOR		:= 0
VERSION_MINOR		:= 1

# Build verbosity
V			:= 1
# Debug build
DEBUG			:= 1
# Build architecture
ARCH 			:= aarch64

ifeq (${V},0)
Q=@
CHECKCODE_ARGS	+=	--no-summary --terse
else
Q=
endif
export Q

ifneq (${DEBUG}, 0)
BUILD_TYPE	:=	debug
LOG_LEVEL	:=	40
else
BUILD_TYPE	:=	release
LOG_LEVEL	:=	20
endif

ROOT_DIR	:= $(PWD)
SRC_DIR		:= $(ROOT_DIR)/src
OUT_DIR		:= $(ROOT_DIR)/out
PLAT_DIR	:= $(ROOT_DIR)/platform

INCLUDES		+=      -I$(ROOT_DIR)/include/stdlib		\
				-I$(ROOT_DIR)/include/stdlib/sys	\
				-I$(ROOT_DIR)/include/drivers		\
				-I$(ROOT_DIR)/include

#DEFINES			+=

ASFLAGS			+= 	-nostdinc -ffreestanding -Wa,--fatal-warnings	\
				-Werror -Wmissing-include-dirs			\
				-mgeneral-regs-only -D__ASSEMBLY__		\
				${DEFINES} ${INCLUDES}
CFLAGS			+= 	-nostdinc -ffreestanding -Wall			\
				-Werror -Wmissing-include-dirs			\
				-mgeneral-regs-only -mstrict-align		\
				-std=c99 -c -Os	${DEFINES} ${INCLUDES} -fno-pic
CFLAGS			+=	-ffunction-sections -fdata-sections		\
				-fno-delete-null-pointer-checks

LDFLAGS			+=	--fatal-warnings -O1
LDFLAGS			+=	--gc-sections

CROSS_COMPILE           :=      $(ROOT_DIR)/../toolchains/aarch64/bin/aarch64-linux-gnu-

CC			:=	${CROSS_COMPILE}gcc
CPP			:=	${CROSS_COMPILE}cpp
AS			:=	${CROSS_COMPILE}gcc
AR			:=	${CROSS_COMPILE}ar
LD			:=	${CROSS_COMPILE}ld
OC			:=	${CROSS_COMPILE}objcopy
OD			:=	${CROSS_COMPILE}objdump
NM			:=	${CROSS_COMPILE}nm
PP			:=	${CROSS_COMPILE}gcc -E ${CFLAGS}

VERSION_STRING		:=	v${VERSION_MAJOR}.${VERSION_MINOR}

define MAKE_C

$(eval OBJ := $(1)/$(patsubst %.c,%.o,$(notdir $(2))))
$(eval PREREQUISITES := $(patsubst %.o,%.d,$(OBJ)))

$(OBJ) : $(2)
	@echo "  CC      $$<"
	$$(Q)$$(CC) $$(CFLAGS) -DIMAGE_BL$(3) -c $$< -o $$@


$(PREREQUISITES) : $(2)
	@echo "  DEPS    $$@"
	@mkdir -p $(1)
	$$(Q)$$(CC) $$(CFLAGS) -M -MT $(OBJ) -MF $$@ $$<

-include $(PREREQUISITES)

endef


define MAKE_S

$(eval OBJ := $(1)/$(patsubst %.S,%.o,$(notdir $(2))))
$(eval PREREQUISITES := $(patsubst %.o,%.d,$(OBJ)))

$(OBJ) : $(2)
	@echo "  AS      $$<"
	$$(Q)$$(AS) $$(ASFLAGS) -c $$< -o $$@

$(PREREQUISITES) : $(2)
	@echo "  DEPS    $$@"
	@mkdir -p $(1)
	$$(Q)$$(AS) $$(ASFLAGS) -M -MT $(OBJ) -MF $$@ $$<

-include $(PREREQUISITES)

endef


define MAKE_LD

$(eval PREREQUISITES := $(1).d)

$(1) : $(2)
	@echo "  PP      $$<"
	$$(Q)$$(AS) $$(ASFLAGS) -P -E -D__LINKER__ -o $$@ $$<

$(PREREQUISITES) : $(2)
	@echo "  DEPS    $$@"
	@mkdir -p $$(dir $$@)
	$$(Q)$$(AS) $$(ASFLAGS) -M -MT $(1) -MF $$@ $$<

endef


define MAKE_OBJS
	$(eval C_OBJS := $(filter %.c,$(2)))
	$(eval REMAIN := $(filter-out %.c,$(2)))
	$(eval $(foreach obj,$(C_OBJS),$(call MAKE_C,$(1),$(obj))))

	$(eval S_OBJS := $(filter %.S,$(REMAIN)))
	$(eval REMAIN := $(filter-out %.S,$(REMAIN)))
	$(eval $(foreach obj,$(S_OBJS),$(call MAKE_S,$(1),$(obj))))

	$(and $(REMAIN),$(error Unexpected source files present: $(REMAIN)))
endef

define SOURCES_TO_OBJS
	$(notdir $(patsubst %.c,%.o,$(filter %.c,$(1)))) \
	$(notdir $(patsubst %.S,%.o,$(filter %.S,$(1))))
endef

.PHONY: raspihyp clean help
all: raspihyp

SOURCES		:= 	$(PLAT_DIR)/raspihyp.S		\
			$(PLAT_DIR)/debug.S		\
			$(PLAT_DIR)/plat_helpers.S	\
			$(PLAT_DIR)/platform_up_stack.S	\
			$(PLAT_DIR)/console.c		\
			$(ROOT_DIR)/lib/stdlib/std.c	\
			$(SRC_DIR)/hyp_printf.c		\
			$(SRC_DIR)/hyp_main.c
LINKERFILE	:= $(OUT_DIR)/raspihyp.ld
LINKERFILE_SRC	:= $(PLAT_DIR)/raspihyp.ld.S

$(eval $(call MAKE_OBJS,$(OUT_DIR),$(SOURCES)))
$(eval $(call MAKE_LD,$(LINKERFILE),$(LINKERFILE_SRC)))

OBJS    := $(addprefix $(OUT_DIR)/,$(call SOURCES_TO_OBJS,$(SOURCES)))
MAPFILE	:= $(OUT_DIR)/raspihyp.map
DUMP    := $(OUT_DIR)/raspihyp.dump
BIN     := $(OUT_DIR)/raspihyp.bin
ELF     := $(OUT_DIR)/raspihyp.elf

$(ELF) : $(OBJS) $(LINKERFILE)
	@echo "  LD      $@"
	@echo 'const char build_message[] = "Built : "__TIME__", "__DATE__; \
	       const char version_string[] = "${VERSION_STRING}";' | \
		$(CC) $(CFLAGS) -xc - -o $(OUT_DIR)/build_message.o
	$(Q)$(LD) -o $@ $(LDFLAGS) -Map=$(MAPFILE) --script $(LINKERFILE) \
					$(OUT_DIR)/build_message.o $(OBJS)

$(DUMP) : $(ELF)
	@echo "  OD      $@"
	${Q}${OD} -dx $< > $@

$(BIN) : $(ELF)
	@echo "  BIN     $@"
	$(Q)$(OC) -O binary $< $@
	@echo
	@echo "Built $@ successfully"
	@echo

$(OUT_DIR) :
	$(Q)mkdir -p "$@"


raspihyp: $(OUT_DIR) $(BIN) $(DUMP)

clean:
	@echo "  CLEAN"
	${Q}rm -rf ${OUT_DIR}

help:
	@echo "usage: ${MAKE} PLAT=<${HELP_PLATFORMS}> <all|clean>"
	@echo ""
