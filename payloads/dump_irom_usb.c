#include "common.h"
#include "bootrom_usb_helper.h"
#include "soc.h"
#include "pmc_reset.h"

void main()
{
	reg_write(IPATCH_BASE, IPATCH_SELECT, 0x0);

	usb_transfer_data((void*)BOOTROM_START, IROM_SIZE);

	pmc_reset();
}
