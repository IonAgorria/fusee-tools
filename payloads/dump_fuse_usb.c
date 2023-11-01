#include <stdint.h>
#include "size.h"
#include "soc.h"
#include "bootrom_usb_helper.h"

// Tegra common
#define IROM_START 0xfff00000U


#define IPATCH_BASE 0x6001dc00U


#define FUSE_BASE 0x7000f800U

//void transfer_data(void *data, uint32_t size);

int main() {
	
	char str[] = "Hello, this is a message from Surface 2 :)";
	
	usb_transfer_data(str, sizeof(str));
	
	usb_transfer_data((void*)FUSE_BASE, SZ_1K);
/*
	for (int i = 0; i < 2; i++) {
		transfer_data((uint32_t*)(IROM_START+i*0x1000), 0x1000);
	}	
*/	
	
	while(1);
	
	return 0;
}
