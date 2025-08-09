#ifndef BOOTROM_USB_H
#define BOOTROM_USB_H

#include "size.h"
#include "sleep.h"
#include "soc.h"
#include "memory.h"

#define BLOCK_SIZE (0x1000)
#define USB_DMA_BUFFER_ADDR ((void*) 0x4003E000)

#if defined(T20)
    #define BR_FUNC_USB_INIT (0xfff03050 + 1)
    #define BR_FUNC_USB_INIT_ARG1 (0x40001228) 
    #define BR_FUNC_USB_EP1_SEND (0xfff03122 + 1)
    #define BR_FUNC_USB_EP1_RESET (0xfff031e8 + 1)
#elif defined(T30)
    //#define BR_FUNC_USB_INIT (0xfff04fb4 + 1)
    //#define BR_FUNC_USB_INIT_ARG1 (0x400019b0) 
    #define BR_FUNC_USB_EP1_SEND (0xfff05092 + 1)
    #define BR_FUNC_USB_EP1_RESET (0xfff04996 + 1)
    #define BR_FUNC_USB_EP1_RECV (0xfff050de + 1)
    #define BR_FUNC_DRAM_SETUP (0xfff01c12 + 1)
    //#define BR_FUNC_BOOT_FROM_RAM (0xfff01260 +1)
#elif defined(T114)
    #define BR_FUNC_USB_EP1_SEND (0xfff06ccc + 1)
    #define BR_FUNC_USB_EP1_RESET (0xfff0679e + 1)
#elif defined(T124)
    #define BR_FUNC_USB_EP1_SEND (0x001065c0 + 1)
    #define BR_FUNC_USB_EP1_RESET (0x00106092 + 1)
#else
    #error No SoC specified
#endif

void usb_init() {
#if defined(BR_FUNC_USB_INIT) && BR_FUNC_USB_INIT != 0
	void (*br_func_usb_init)(uint32_t*) = (void (*)(uint32_t*)) BR_FUNC_USB_INIT;
	//br_func_usb_init(*((uint32_t**) BR_FUNC_USB_INIT_ARG1));
	br_func_usb_init((uint32_t*) BR_FUNC_USB_INIT_ARG1);
	msleep(500);
#endif
}

#if defined(BR_FUNC_USB_EP1_RESET) && BR_FUNC_USB_EP1_RESET != 0
void usb_reset() {
	void (*br_func_usb_ep1_reset)(uint32_t, uint32_t) = (void (*)(uint32_t, uint32_t)) BR_FUNC_USB_EP1_RESET;
	br_func_usb_ep1_reset(0, 1);
}
#endif

#if defined(BR_FUNC_USB_EP1_SEND) && BR_FUNC_USB_EP1_SEND != 0
int usb_send(void* addr, uint32_t size) {
	uint32_t ret = 0;
	uint32_t sent = 0;
	uint32_t transfer_size = 0;
	uint32_t (*br_func_usb_ep1_send)(void*, uint32_t, uint32_t*) = (uint32_t (*)(void*, uint32_t, uint32_t*)) BR_FUNC_USB_EP1_SEND;
	
	if (USB_DMA_BUFFER_ADDR <= addr && addr < (USB_DMA_BUFFER_ADDR + 0x1000)) {
		return -1;
	}

	while (0 < size) {
		transfer_size = size;
		if (transfer_size > BLOCK_SIZE) transfer_size = BLOCK_SIZE;

		// Copy payload to 0x1000 aligned buffer
		memcpy(USB_DMA_BUFFER_ADDR, addr, transfer_size);

		ret = br_func_usb_ep1_send(USB_DMA_BUFFER_ADDR, transfer_size, &sent);
		if (ret)
			return ret;
		
		addr += sent;
		size -= sent;
	}
	
	return 0;
}
#endif

#if defined(BR_FUNC_USB_EP1_RECV) && BR_FUNC_USB_EP1_RECV != 0
int usb_recv(void* addr, uint32_t size) {
	uint32_t ret = 0;
	uint32_t recv = 0;
	uint32_t transfer_size = 0;
	uint32_t (*br_func_usb_ep1_recv)(void*, uint32_t, uint32_t*) = (uint32_t (*)(void*, uint32_t, uint32_t*)) BR_FUNC_USB_EP1_RECV;
	//return br_func_usb_ep1_recv(addr, size, &recv);
	
	if (USB_DMA_BUFFER_ADDR <= addr && addr < (USB_DMA_BUFFER_ADDR + 0x1000)) {
		return -1;
	}

	while (0 < size) {
		transfer_size = size;
		if (transfer_size > BLOCK_SIZE) transfer_size = BLOCK_SIZE;

		ret = br_func_usb_ep1_recv(USB_DMA_BUFFER_ADDR, transfer_size, &recv);
		if (ret)
			return ret;

		// Copy data from 0x1000 aligned buffer to target location
		memcpy(addr, USB_DMA_BUFFER_ADDR, recv);
		
		addr += recv;
		size -= recv;
	}
	
	return 0;
}
#endif

#if defined(BR_FUNC_DRAM_SETUP) && BR_FUNC_DRAM_SETUP != 0
/**
 * Set up DRAM based on BCT found at 0x4000:0100
 */
void dram_setup() {
	void (*br_func_dram_setup)(void) = (void (*)(void))BR_FUNC_DRAM_SETUP;
	br_func_dram_setup();
}
#endif

#endif //BOOTROM_USB_H
