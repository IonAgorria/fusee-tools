#include "soc.h"
#include "common.h"
#include "pmc_reset.h"
#include "strings.h"
#include "bootrom_usb_helper.h"

uint32_t swap_endian(uint32_t input)
{
	uint8_t group[4];
	int32_t i;

	for (i = 0; i < 4; i++)
		group[i] = input >> i * 8;

	return group[0] << 24 | group[1] << 16 |
	       group[2] <<  8 | group[3] <<  0;
}

void main()
{
	char buf[4096];

	uint32_t sbk[4];
	int32_t i;
	
	for (i = 0; i < 4; i++) {
		sbk[i] = reg_read(FUSE_PRIVATE_KEY0, i * 4);
		sbk[i] = swap_endian(sbk[i]);
	}

	sprintf(buf, "Dumped SBK 0x%08x 0x%08x 0x%08x 0x%08x\n",
		sbk[0], sbk[1], sbk[2], sbk[3]);

	usb_transfer_data(buf, strlen(buf));

	pmc_reset();
}
