CROSS_COMPILE = arm-none-eabi-

# Use our cross-compile prefix to set up our basic cross compile environment.
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

override CFLAGS += \
	-mtune=arm7tdmi \
	-mlittle-endian \
	-fno-stack-protector \
	-fno-common \
	-fno-builtin \
	-ffreestanding \
	-std=gnu99 \
	-Werror \
	-Wall \
	-Wno-error=unused-function \
	-fomit-frame-pointer \
	-g \
	-O0

#Required for -lgcc
GCC_LIB := "/usr/lib/gcc/$(CROSS_COMPILE:-=)/$(shell $(CC) -dumpversion)"
LDFLAGS ?= -nostdlib -L$(GCC_LIB) -lgcc
#-Map=$(basename $@).map

OBJS = tegra_uart.o printf.o bootrom_usb_helper.o

all: T20 T30 T114

T20:
	$(MAKE) \
		SOC=T20 \
		UART=B \
		ENTRY_ADDR=0x40008000 \
		payloads

T30: T30_A T30_D

T30_A:
	$(MAKE) \
		SOC=T30 \
		UART=A \
		ENTRY_ADDR=0x4000A000 \
		payloads

T30_D:
	$(MAKE) \
		SOC=T30 \
		UART=D \
		ENTRY_ADDR=0x4000A000 \
		payloads

T114: T114_A T114_D

T114_A:
	$(MAKE) \
		SOC=T114 \
		UART=A \
		ENTRY_ADDR=0x4000E000 \
		payloads

T114_D:
	$(MAKE) \
		SOC=T114 \
		UART=D \
		ENTRY_ADDR=0x4000E000 \
		payloads

#T124:
#	$(MAKE) \
#		SOC=T124 \
#		ENTRY_ADDR=0x4000E000 \
#		payloads

payloads: clean_objs		\
	  dump_fuse_usb.bin	\
	  dump_ipatches_usb.bin	\
	  dump_irom_usb.bin	\
	  patch_irom.bin	\
	  dump_irom_uart.bin

%.bin: %.elf
	$(OBJCOPY) -v -O binary $(SOC)_$(UART)_$< $(SOC)_$(UART)_$@

%.elf: %.o $(OBJS)
	$(LD) $^ -T common.lds --defsym LOAD_ADDR=$(ENTRY_ADDR) $(LDFLAGS) -o $(SOC)_$(UART)_$@

%.o: %.c
	$(CC) $(CFLAGS) -D$(SOC) -DUART_$(UART)_USE $< -c -o $@

clean: clean_objs
	rm -f *.bin

clean_objs:
	rm -f *.o *.elf *.map

.PHONY: all clean
