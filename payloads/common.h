#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define __unused __attribute__((__unused__))

//Workaround in case there is code before main(), make sure we jump into main()
void main();
__unused void payload_main() {
	main();
}

#define _REG(base, off) *(volatile unsigned int *)((base) + (off))
#define reg_write(base, off, value) _REG(base, off) = value
#define reg_clear(base, off, value) _REG(base, off) &= ~value
#define reg_set(base, off, value) _REG(base, off) |= value
#define reg_read(base, off) _REG(base, off)

#define PINMUX_BASE					(0x70003000)
#define PMC_BASE					(0x7000e400)
#define APBDEV_PMC_SCRATCH0_0			(0x50)
#define APBDEV_PMC_SCRATCH41_0			(0x140)
#define APBDEV_PMC_SCRATCH42_0			(0x144)
#define BIT(X) (1<<X)

#define BOOTROM_START			0xfff00000

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

#endif //COMMON_H
