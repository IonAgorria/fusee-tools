#include "common.h"
#include "pmc_reset.h"
#include "sleep.h"
#include "memory.h"

#include <stdbool.h>
#include "tegra_aes.h"

#define USB
#ifdef USB
#include "bootrom_usb_helper.h"
#endif

const u8 ZERO_KEY[AES128_KEY_LENGTH] = {};

void main()
{
	uint32_t* sbk = (uint32_t*) 0x40010000;
	*((uint32_t*) (sbk - 2)) = 0xC0DECAFE;
	*((uint32_t*) (sbk - 1)) = 0x00FF0100;

	for (uint32_t i = 0; i < 4; i++) {
		sbk[i] = reg_read(FUSE_BASE, FUSE_PRIVATE_KEY0 + i * 4);
		sbk[i + 4] = swap_endian_32(sbk[i]);
	}

#ifdef USB
	usb_init();
	usb_transfer_data(sbk, AES128_KEY_LENGTH);
	for (int i = 0; i < 4; ++i) {
		usb_transfer_data((void*) 0x6001b600 + (TEGRA_AES_HW_TABLE_LENGTH * i),
				  TEGRA_AES_HW_TABLE_LENGTH);
	}
#endif

	struct tegra_aes_priv priv;
	tegra_aes_probe(&priv);
	//We load the SBK as IV on 2nd so it can be leaked by calling decrypt with 0's key
	tegra_aes_set_key_for_key_slot(&priv, 128, (u8*) ZERO_KEY, (u8*) sbk, 1);
	//Also load SBK as key and IV on 3rd
	tegra_aes_set_key_for_key_slot(&priv, 128, (u8*) sbk, (u8*) sbk, 2);

#ifdef USB
	for (int i = 0; i < 4; ++i) {
		usb_transfer_data((void*) 0x6001b600 + (TEGRA_AES_HW_TABLE_LENGTH * i),
				  TEGRA_AES_HW_TABLE_LENGTH);
	}
#endif

	//Done!
	call_func(BOOTROM_START_POST_IPATCH);
	pmc_reset_rcm();
}
