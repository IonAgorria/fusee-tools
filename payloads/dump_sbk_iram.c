#include "common.h"
#include "pmc_reset.h"

void main()
{
    uint32_t* sbk = (uint32_t*) 0x40010000;
	*((uint32_t*) (sbk - 2)) = 0xC0DECAFE;
	*((uint32_t*) (sbk - 1)) = 0x00FF0100;

    for (uint32_t i = 0; i < 4; i++) {
        sbk[i] = reg_read(FUSE_BASE, FUSE_PRIVATE_KEY0 + i * 4);
        sbk[i + 4] = swap_endian_32(sbk[i]);
    }
    
    pmc_reset();
}
