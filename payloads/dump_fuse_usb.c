#include "common.h"
#include "size.h"
#include "soc.h"
#include "bootrom_usb_helper.h"

//void transfer_data(void *data, uint32_t size);

void main() {
	
	char str[] = "Hello, this is a message from Surface 2 :)";
	
	usb_transfer_data(str, sizeof(str));
	
	usb_transfer_data((void*)FUSE_BASE, SZ_1K);
/*
	for (int i = 0; i < 2; i++) {
		transfer_data((uint32_t*)(IROM_START+i*0x1000), 0x1000);
	}	
*/	
	
	while(1);
}
