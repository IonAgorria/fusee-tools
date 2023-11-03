#ifndef PMC_RESET_H
#define PMC_RESET_H

static void pmc_reset(unsigned int scratch0) {
	while (1) {
		*((unsigned int*) (PMC_BASE + APBDEV_PMC_SCRATCH0_0)) = scratch0;
		*((unsigned int*) (PMC_BASE)) = 0x10;
	}
}

//Reset to APX via PMC regs
__unused static void pmc_reset_rcm() {
    pmc_reset(0x2);
}

#endif //PMC_RESET_H
