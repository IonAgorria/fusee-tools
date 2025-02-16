#include "common.h"
#include "bootrom_usb_helper.h"
#include "soc.h"

#define IPATCH_BASE					(0x6001dc00)
#define IPATCH_SELECT				(0x0)

void dump_memory(uint8_t *src, uint32_t size);

void main()
{
	reg_write(IPATCH_BASE, IPATCH_SELECT, 0x0);
	usb_transfer_data((void*)BOOTROM_START, BOOTROM_SIZE);

	while(1);
}
