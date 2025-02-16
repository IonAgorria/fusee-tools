#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define __unused __attribute__((__unused__))

#define DMB() asm volatile ("" : : : "memory")

#define PINMUX_BASE				(0x70003000)

#define PMC_BASE				(0x7000e400)
#define   APBDEV_PMC_SCRATCH0_0			(0x50)
#define   APBDEV_PMC_SCRATCH4_0			(0x60)
#define   APBDEV_PMC_SCRATCH24_0		(0xfc)
#define   APBDEV_PMC_SCRATCH32_0		(0x11c)
#define   APBDEV_PMC_SCRATCH33_0		(0x120)
#define   APBDEV_PMC_SCRATCH41_0		(0x140)
#define   APBDEV_PMC_SCRATCH42_0		(0x144)
	
#define FUSE_BASE				(0x7000f800U)
#define   FUSE_PRIVATE_KEY0			(0x1a4)
#define   FUSE_BOOTROM_PATCH_SIZE_0		(0x19c)

/* ipatch hardware */
#define IPATCH_BASE				(0x6001dc00)
#define   IPATCH_SELECT				(0x0)
#define   IPATCH_REGS				(0x4)

/*
#define TEGRA_APB_MISC_BASE		0x70000000
#define APB_MISC_GP_HIDREV_0		0x804

enum tegra_chipid {
	TEGRA_CHIPID_UNKNOWN = 0,
	TEGRA_CHIPID_TEGRA2 = 0x20,
	TEGRA_CHIPID_TEGRA3 = 0x30,
	TEGRA_CHIPID_TEGRA11 = 0x35,
};

uint32_t chipid_reg = reg_read(TEGRA_APB_MISC_BASE, APB_MISC_GP_HIDREV_0);
uint32_t chipid = (chipid_reg >> 8) & 0xff;
printf("chip_id: 0x%02x\r\n", chipid);
*/

#define _REG(base, off) *(volatile unsigned int *)((base) + (off))
#define reg_write(base, off, value) _REG(base, off) = value
#define reg_clear(base, off, value) _REG(base, off) &= ~value
#define reg_set(base, off, value) _REG(base, off) |= value
#define reg_read(base, off) _REG(base, off)

#define swap_endian_32(input) ( \
    ((input >> 24) & 0x000000ff) | \
    ((input >>  8) & 0x0000ff00) | \
    ((input <<  8) & 0x00ff0000) | \
    ((input << 24) & 0xff000000)   \
)

#define call_func(addr) { \
	typedef void (*funcptr)(void); \
	funcptr start = (funcptr) addr; \
    DMB(); \
	start(); \
}

#define min(a, b) ((a > b) ? b : a)
#define max(a, b) ((a > b) ? a : b)

//Used to know the payload end address (size can be inferred by &END_ADDR - &main)
__unused static const uint32_t END_ADDR = 0xFADEC0DE;

#endif //COMMON_H
