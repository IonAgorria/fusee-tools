#ifndef BOOTROM_USB_H
#define BOOTROM_USB_H

#include "size.h"
#include "soc.h"

#if defined(T20)
    //TODO define these
    #define write_ep1_in_sync (0 + 1)
    #define usb_reset_ep1 (0 + 1)
#elif defined(T30)
    #define write_ep1_in_sync (0xFFF05092 + 1)
    #define usb_reset_ep1 (0xFFF04996 + 1)
#elif defined(T114)
    #define write_ep1_in_sync (0xfff06ccc + 1)
    #define usb_reset_ep1 (0xfff0679e + 1)
#else
    #error No SoC specified
#endif

void usb_transfer_data(void* data, uint32_t size) {
	uint32_t ret = 0;
	
	void (*write_ep1)(uint32_t*, uint32_t, uint32_t*) = (void (*)(uint32_t*, uint32_t, uint32_t*))write_ep1_in_sync;
	write_ep1(data, size, &ret);

	void (*usb_reset)(uint32_t, uint32_t) = (void (*)(uint32_t, uint32_t))usb_reset_ep1;
	usb_reset(0, 1);
}


#endif
