#include "common.h"
#include "memory.h"
#include "soc.h"

void copy_sbk(void* dst, void* src) {
	memcpy(dst, src, 16);
	for (uint32_t i = 0; i < 4; i++) {
		((uint32_t*) dst)[i + 4] = swap_endian_32(((uint32_t*) src)[i]);
	}
}

void main()
{
	void* iram = (void*) 0x40010000;

	//Copy to IRAM from fuses first
	memset(iram, 0xEA, 0x20);
	copy_sbk(iram, (void*) FUSE_BASE + FUSE_PRIVATE_KEY0);

	//Now copy around IRAM
	for (int i = 1; i < 0x100; ++i) {
		void* dst = iram + i * 0x100;
		copy_sbk(dst, iram);
		*((uint32_t*) (dst - 4)) = 0xC0DECAFE;
		*((uint32_t*) (dst + 0x20)) = 0xC0DECAFE;
	}

	//Also copy to some  PMC scratch registers
	copy_sbk((uint32_t*) (PMC_BASE + APBDEV_PMC_SCRATCH4_0), iram);
	copy_sbk((uint32_t*) (PMC_BASE + APBDEV_PMC_SCRATCH24_0), iram);
	copy_sbk((uint32_t*) (PMC_BASE + APBDEV_PMC_SCRATCH32_0), iram);

	//Clear bit0 to indicate that this is a fresh boot
	reg_write(PMC_BASE, APBDEV_PMC_SCRATCH0_0, 0);

	//Go back to bootrom
	call_func(BOOTROM_START);
}
