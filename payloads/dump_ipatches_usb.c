#include "common.h"
#include "strings.h"
#include "soc.h"
#include "bootrom_usb_helper.h"
#include "pmc_reset.h"

void main()
{
	char buf[4096];
	//volatile uint32_t reg = 0;

	usb_init();

	//sprintf(buf, "hello there %d; 0x%08x; %s\n", 420, 0x69, "Nope");
	//usb_transfer_data(buf, strlen(buf));

	//sprintf(buf, "Select: 0x%08x; Reg 1: 0x%08x", reg_read(IPATCH_BASE, IPATCH_SELECT), reg_read(IPATCH_BASE, IPATCH_REGS));
	//usb_transfer_data(buf, strlen(buf));

	uint32_t ipatch_select = reg_read(IPATCH_BASE, IPATCH_SELECT);
	int32_t counter = 0;

	while (ipatch_select & 0x1) {
		volatile uint32_t reg = reg_read(IPATCH_BASE, IPATCH_REGS + counter*4);
		//sprintf(buf, "Patch %d: 0x%08x;\n", counter, reg);
		//usb_transfer_data(buf, strlen(buf));
		
		sprintf(buf, "Patch %d; Address 0x%08x: 0x%04x;\n",
			counter, BOOTROM_START | ((reg >> 16) << 1), reg & 0xffff);
		usb_transfer_data(buf, strlen(buf));

		counter++;
		ipatch_select >>= 1;
	}

	sprintf(buf, "Patch select: 0x%08x\n", ipatch_select);
	usb_transfer_data(buf, strlen(buf));

	uint32_t count = reg_read(FUSE_BASE, FUSE_BOOTROM_PATCH_SIZE_0);
	sprintf(buf, "Fuse count: 0x%08x\n", count);
	usb_transfer_data(buf, strlen(buf));

	pmc_reset();
}
