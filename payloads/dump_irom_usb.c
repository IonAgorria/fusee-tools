#include "common.h"
#include "bootrom_usb_helper.h"
#include "soc.h"
#include "pmc_reset.h"

void main()
{
	reg_write(IPATCH_BASE, IPATCH_SELECT, 0x0);

	usb_init();
	usb_transfer_data((void*)BOOTROM_START, BOOTROM_SIZE);

	pmc_reset();
}
