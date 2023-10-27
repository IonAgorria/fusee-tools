#include <stdint.h>
#include "printf.h"
#include "tegra_uart.h"
#include "common.h"
#include "soc.h"


void dump_memory(uint8_t *src, uint32_t size);

void main()
{
	printf("Dump irom\r\n");

	dump_memory((uint8_t *)BOOTROM_START, IROM_SIZE);

	printf("bye.\r\n");

	while(1);
}

void dump_memory(uint8_t *src, uint32_t size) {
	printf("Dump %u Bytes from %p:\r\n", size, src);
	printf("            00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
	
	for (uint32_t i = 0; i < size; i++) {
		if (i % 16 == 0) {
			printf("%p: ", &src[i]);
		}
		
		printf("%02x ", src[i]);
		
		if (i % 16 == 15) {
			printf("\r\n");
		}
	}
}
