EABI = arm-none-eabi
AS = $(EABI)-as
CC = $(EABI)-gcc
OBJDIR = objs
PRJC = bin/temp_control.elf

C_SRC = \
				src/init.c \
				src/systick.c \
				src/adc.c \
				src/usart.c \
				src/string.c \
				src/irq_usart.c \
				src/main.c

S_SRC = \
				src/asm/startup.s

C_OBJ = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(C_SRC)))
S_OBJ = $(addprefix $(OBJDIR)/,$(patsubst %.s,%.o,$(S_SRC)))
OBJSDIR = $(sort $(dir $(C_OBJ) $(S_OBJ)))

DEFINE = -D__DEBUG__

ARCH = armv6-m
CPU = cortex-m0

.PHONY: default all clean toHex

default:
	[ -d 'bin' ] || mkdir -v 'bin'
	[ -d $(OBJDIR) ] || mkdir -v $(OBJDIR)
	[ -d "$(OBJSDIR)" ] || mkdir -pv $(OBJSDIR)
	make $(PRJC)

$(PRJC):$(S_OBJ) $(C_OBJ)
	$(CC) -o $@ $^ \
		-mtune=$(CPU) \
		-mcpu=$(CPU) \
		-march=$(ARCH) \
		-specs=nosys.specs \
		-T ./stm32f042.ld \
		-mfloat-abi=soft -msoft-float \
		-lc -lm -lnosys \
		-g -O2

$(OBJDIR)/%.o:%.c
	$(CC) -o $@ -c $< \
		$(DEFINE) \
		-mtune=$(CPU) \
		-mcpu=$(CPU) \
		-march=$(ARCH) \
		-mfloat-abi=soft -msoft-float \
		-mthumb \
		-g -O2

$(OBJDIR)/%.o:%.s
	$(AS) -o $@ $< \
		-mcpu=$(CPU) \
		-march=$(ARCH) \
		-mthumb

all:
	make clean default toHex

clean:
	[ ! -d bin ] || rm -rv bin
	[ ! -d objs ] || rm -rv objs

toHex:
	./toHex.sh
