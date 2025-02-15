#include "common.h"
#include "soc.h"

#define DESIRED_SECURITY_MODE		3
#define IPATCH_SLOT			0 // maybe overwrite already present ipatches

/**
 * Patches over a given address in the IROM using the IPATCH hardware.
 */
void ipatch_word(uint8_t slot, uint32_t addr, uint16_t new_value)
{
	uint32_t slot_value;
	uint32_t offset;

	/* Mark the relevant ipatch slot as not-in-use */
	reg_clear(IPATCH_BASE, IPATCH_SELECT, (1 << slot));

	/* Compute the new patch value */
	offset = (addr & 0xFFFF) >> 1;
	slot_value = (offset << 16) | new_value;

	/* Figure out the location of the slot to touch */
	reg_write(IPATCH_BASE, IPATCH_REGS + (slot * 4), slot_value);

	/*
	 * Apply the new one. Disable all other ipatches.
	 * Not sure if this is good. Could cause warmboot
	 * related problems
	 */
	reg_write(IPATCH_BASE, IPATCH_SELECT, (1 << slot));
}

void main()
{
	/*
	 * Patch the movs r0, #x instruction to always
	 * return 3 (Developer Mode)
	 * https://github.com/NVIDIA/tegrarcm/blob/master/src/rcm.h#L111
	 */
	ipatch_word(IPATCH_SLOT, IROM_PATCH_ADDRESS,
		    0x2000 | DESIRED_SECURITY_MODE);

	/*
	 * Clear bit0 to indicate that this is a fresh boot,
	 * and then set bit2 to trigger RCM.
	 */
	reg_write(PMC_BASE, APBDEV_PMC_SCRATCH0_0, (1 << 1));

	call_func(BOOTROM_START_POST_IPATCH);
}
