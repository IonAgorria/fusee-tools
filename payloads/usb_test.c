#include "common.h"
#include "bootrom_usb_helper.h"
#include "pmc_reset.h"
#include "sleep.h"

void main() {
	char str[] = "Hello, this is a message over USB :)\n";

	usb_init();
	for (int i = 0; i < 60; ++i) {
		usb_transfer_data(str, sizeof(str));
		msleep(1000);
	}

	pmc_reset_rcm();
}
