#define NO_MAIN_WORKAROUND
#include "common.h"
#include "soc.h"
#include "pmc_reset.h"

void main()
{
	reg_write(IPATCH_BASE, IPATCH_SELECT, 0x0);
    uint32_t* src = (uint32_t*) BOOTROM_START;
    uint32_t* dst = (uint32_t*) 0x40009000;
	*((uint32_t*) (dst - 2)) = 0xC0DECAFE;
	*((uint32_t*) (dst - 1)) = 0x00FF0200;
    uint32_t blocks = (64 * 1024) / 4;
    
    for (uint32_t i = 0; i < (0x18000 / 4); i++) {
        dst[i] = i < blocks ? src[i] : 0;
    }

    pmc_reset();
}
