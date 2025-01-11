#ifndef PMC_RESET_H
#define PMC_RESET_H

#define pmc_reset() \
	while (1) { \
        *((unsigned int*) (PMC_BASE + APBDEV_PMC_SCRATCH0_0)) = 0x0; \
		*((unsigned int*) (PMC_BASE)) = 0x10; \
	}

//Reset to APX via PMC regs
#define pmc_reset_rcm() \
	while (1) { \
        *((unsigned int*) (PMC_BASE + APBDEV_PMC_SCRATCH0_0)) = 0x2; \
		*((unsigned int*) (PMC_BASE)) = 0x10; \
	}

#endif //PMC_RESET_H
