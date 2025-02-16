#include "common.h"
#include "soc.h"
#include "pmc_reset.h"

void main()
{
	reg_write(IPATCH_BASE, IPATCH_SELECT, 0x0);
	uint32_t* src = (uint32_t*) BOOTROM_START;

#if defined(T20) | defined(T30)
	uint32_t* dst = (uint32_t*) 0x40009000;
	uint32_t blocks = BOOTROM_SIZE / 4;
#else
	uint32_t* dst = (uint32_t*) 0x40038000;
	uint32_t blocks = 0x8000 / 4;
#endif

	*((uint32_t*) (dst - 2)) = 0xC0DECAFE;
	*((uint32_t*) (dst - 1)) = 0x00FF0200;
    
	for (uint32_t i = 0; i < blocks; i++) {
		dst[i] = src[i];
	}

	pmc_reset_rcm();
}
