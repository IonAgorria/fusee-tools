#include "common.h"
#include "size.h"
#include "soc.h"
#include "bootrom_usb_helper.h"
#include "pmc_reset.h"

void main() {
	char str[] = "Hello, dumping fuses:\n";

	usb_init();
	usb_send(str, sizeof(str));

	usb_send((void*)FUSE_BASE, SZ_1K);

	pmc_reset();
}
