#include "common.h"
#include "bootrom_usb_helper.h"
#include "pmc_reset.h"

void main() {
	char str[] = "Hello, this is a message over USB :)";
    
	usb_transfer_data(str, sizeof(str));

    pmc_reset_rcm();
}
