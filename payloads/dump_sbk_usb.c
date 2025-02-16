#include "soc.h"
#include "common.h"
#include "pmc_reset.h"
#include "strings.h"
#include "bootrom_usb_helper.h"

void main()
{
	char buf[4096];

	uint32_t sbk[4];
	int32_t i;

	for (i = 0; i < 4; i++) {
		sbk[i] = reg_read(FUSE_BASE, FUSE_PRIVATE_KEY0 + i * 4);
		sbk[i] = swap_endian_32(sbk[i]);
	}

	sprintf(buf, "Dumped SBK [ 0x%08x 0x%08x 0x%08x 0x%08x ]\n",
		sbk[0], sbk[1], sbk[2], sbk[3]);

	usb_init();
	for (i = 0; i < 10; ++i) {
		usb_transfer_data(buf, strlen(buf));
		msleep(100);
	}

	pmc_reset();
}
