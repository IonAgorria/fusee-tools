CROSS_COMPILE = arm-none-eabi-

# Use our cross-compile prefix to set up our basic cross compile environment.
CC      = $(CROSS_COMPILE)gcc
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

override CFLAGS += \
	-mtune=arm7tdmi \
	-mlittle-endian \
	-ffunction-sections \
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
	-fdce -fdse -ftree-dce -ftree-dse \
	-finline-functions-called-once \
	-O0

LDFLAGS ?= -nostdlib --gc-sections
#-Map=$(basename $@).map

all: T20 T30 T114

T20:
	$(MAKE) \
		SOC=T20 \
		UART=B \
		ENTRY_ADDR=0x40008000 \
		payloads

T30: T30_A T30_D T30_E

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

T30_E:
	$(MAKE) \
		SOC=T30 \
		UART=E \
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

payloads: clean_objs \
	make_folder \
	idivmod.o \
	pmc_reset.bin \
	patch_irom.bin \
	dump_fuse_usb.bin \
	dump_ipatches_usb.bin \
	dump_irom_usb.bin \
	dump_irom_uart.bin \
	dump_sbk_usb.bin

make_folder:
	mkdir -p out/tmp/ out/$(SOC)

%.bin: %.elf
	$(OBJCOPY) -v -O binary out/tmp/$(SOC)_$< out/$(SOC)/$@
	rm out/tmp/$(SOC)_$<

%_thumb.bin: %_thumb.elf
	$(OBJCOPY) -v -O binary out/tmp/$(SOC)_$< out/$(SOC)/$@
	rm out/tmp/$(SOC)_$<

%_uart.bin: %_uart.elf
	$(OBJCOPY) -v -O binary out/tmp/$(SOC)_$(UART)_$< out/$(SOC)/$(@:.bin=_$(UART).bin)
	rm out/tmp/$(SOC)_$(UART)_$<

%.elf: %.o
	$(LD) $^ out/tmp/idivmod.o -T common.lds --defsym LOAD_ADDR=$(ENTRY_ADDR) $(LDFLAGS) -o out/tmp/$(SOC)_$@

%_thumb.elf: %_thumb.o
	$(LD) $^ -T common.lds --defsym LOAD_ADDR=$(ENTRY_ADDR) $(LDFLAGS) -o out/tmp/$(SOC)_$@

%_uart.elf: %_uart.o
	$(LD) $^ out/tmp/idivmod.o -T common.lds --defsym LOAD_ADDR=$(ENTRY_ADDR) $(LDFLAGS) -o out/tmp/$(SOC)_$(UART)_$@

%.o: %.c
	$(CC) $(CFLAGS) -D$(SOC) $< -c -o $@

%_thumb.o: %.c
	$(CC) $(CFLAGS) -mthumb -D$(SOC) $< -c -o $@

%_uart.o: %_uart.c
	$(CC) $(CFLAGS) -D$(SOC) -DUART_$(UART)_USE $< -c -o $@

%.o: %.S
	$(AS) -c -o out/tmp/$@ $<

clean: clean_objs
	rm -f out/*/*.bin

clean_objs:
	rm -f *.o *.map out/tmp/*

.PHONY: all clean
