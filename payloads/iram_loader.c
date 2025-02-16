#include "common.h"
#include "memory.h"
#include "pmc_reset.h"
#include "sleep.h"

#ifndef T20
//#define USB
#endif
#ifdef USB
#include "bootrom_usb_helper.h"
#endif

void panik(uint16_t code, uint32_t value) {
	*((uint32_t*) (PMC_BASE + APBDEV_PMC_SCRATCH32_0)) = 0xDEAD0000 | code;
	*((uint32_t*) (PMC_BASE + APBDEV_PMC_SCRATCH33_0)) = value;
#ifdef USB
	usb_transfer_data((uint32_t*) (PMC_BASE + APBDEV_PMC_SCRATCH32_0), 8);
#endif
	pmc_reset();
}

void main()
{
	//Get own location, this has to be the first instruction
	void* pc;
	asm("mov %0, pc" : "=r"(pc));

	//Get iram loader end location
	//Subtract ARM 2 instructions compiler adds before "mov rX, pc"
	//and 2 more since PC is always 2 instructions ahead
	pc += (void*) &END_ADDR - (void*) &main - (4 * 4);

	//Check end of iram loader makes sense
	uint32_t* payload_ptr = pc;
	if (*payload_ptr != 0xFADEC0DE)  {
		panik(0x1, *payload_ptr);
	}

	//Check if code to load into IRAM has magic
	payload_ptr += 1;
	if (*payload_ptr != 0xC0DE10AD)  {
		panik(0x2, *payload_ptr);
	}

	//Get length
	payload_ptr += 1;
	uint32_t payload_len = *payload_ptr;

	//Now payload_ptr is pointing to actual code of payload
	payload_ptr += 1;

	//Where is the payload code to jump into
	void* payload_start;

	if (((uint32_t) payload_ptr & 0xFFF00000) == 0x40000000) {
		//Already in IRAM? no need to move simply store the address for spray
		payload_start = payload_ptr;
	} else {
		//main() is usually located at LOAD_ADDR
		payload_start = &main + 0x1000;
		*((uint32_t*) (payload_start - 8)) = 0x10ADC0DE;
		*((uint32_t*) (payload_start - 4)) = (uint32_t) payload_start;
		*((uint32_t*) (payload_start)) = 0xDEADDEAD;

		//Do the copy to IRAM
		memcpy(payload_start, payload_ptr, payload_len);
	}

	//Spray for memcpy to work
	for (uint32_t i = 0x40002000; i < 0x40003000; i += 4) {
		*((uint32_t*) i) = (uint32_t) payload_start;
	}

	//All ready, lets go!
	*((uint32_t*) (PMC_BASE + APBDEV_PMC_SCRATCH32_0)) = 0xC0DECAFE;
	*((uint32_t*) (PMC_BASE + APBDEV_PMC_SCRATCH33_0)) = (uint32_t) payload_start;

	pmc_reset_rcm();
}
