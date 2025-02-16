#include "common.h"
#include "size.h"
#include "soc.h"
#include "bootrom_usb_helper.h"
#include "pmc_reset.h"

void main() {
	char str[] = "Hello, dumping fuses:\n";

	usb_transfer_data(str, sizeof(str));

	usb_transfer_data((void*)FUSE_BASE, SZ_1K);

	pmc_reset();
}
