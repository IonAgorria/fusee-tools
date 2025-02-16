#ifndef SOC_H
#define SOC_H

#if defined(T20) | defined(T30)
	#define BOOTROM_SIZE			(48 * 1024)
#elif defined(T114) | defined(T124)
	#define BOOTROM_SIZE			(64 * 1024)
#else
	#error No SoC specified
#endif

#if defined(T20) | defined(T30) | defined(T114)
	#define BOOTROM_START			(0xfff00000)
#elif defined(T124)
	#define BOOTROM_START			(0x00100000)
#else
	#error No SoC specified
#endif

#if defined(T20)
	//TODO
	#define BOOTROM_START_POST_IPATCH	(BOOTROM_START)
	#define IROM_PATCH_ADDRESS		(0x0)
#elif defined(T30)
	#define BOOTROM_START_POST_IPATCH	(BOOTROM_START + 0x1004)
	#define IROM_PATCH_ADDRESS		(BOOTROM_START + 0x1cd4)
#elif defined(T114)
	#define BOOTROM_START_POST_IPATCH	(BOOTROM_START + 0x1008)
	#define IROM_PATCH_ADDRESS		(BOOTROM_START + 0x22ac)
#elif defined(T124)
	//TODO
	#define BOOTROM_START_POST_IPATCH	(BOOTROM_START)
	#define IROM_PATCH_ADDRESS		(0x0)
#else
	#error No SoC specified
#endif

#endif //SOC_H
