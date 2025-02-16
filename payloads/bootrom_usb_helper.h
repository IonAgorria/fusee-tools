#ifndef BOOTROM_USB_H
#define BOOTROM_USB_H

#include "size.h"
#include "sleep.h"
#include "soc.h"

#define BLOCK_SIZE (0x1000)

#if defined(T20)
	#define write_ep1_in_sync (0xFFF03122 + 1)
	#define usb_reset_ep1 (0xFFF031E8 + 1)
#elif defined(T30)
	#define write_ep1_in_sync (0xFFF05092 + 1)
	#define usb_reset_ep1 (0xFFF04996 + 1)
#elif defined(T114)
	#define write_ep1_in_sync (0xfff06ccc + 1)
	#define usb_reset_ep1 (0xfff0679e + 1)
#else
	#error No SoC specified
#endif

void usb_init() {
#if defined(T20)
	void (*usb_do_enumeration)(uint32_t*) = (void (*)(uint32_t*))(0xfff03050 + 1);
	usb_do_enumeration((uint32_t*) 0x40001228);
	msleep(500);
#endif
}

void usb_transfer_data(void* data, uint32_t size) {
	uint32_t ret = 0;

	void (*write_ep1)(uint32_t*, uint32_t, uint32_t*) =
		(void (*)(uint32_t*, uint32_t, uint32_t*))write_ep1_in_sync;

	for (int i = 0; i < size; i += BLOCK_SIZE) {
		uint32_t left = size - i;

		if (left > BLOCK_SIZE) left = BLOCK_SIZE;
		write_ep1(data + i, left, &ret);

#if defined(usb_reset_ep1) && usb_reset_ep1 != 0
		void (*usb_reset)(uint32_t, uint32_t) = (void (*)(uint32_t, uint32_t))usb_reset_ep1;
		usb_reset(0, 1);
#endif
	}
}

#endif
