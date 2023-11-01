#include <stdint.h>
#include "common.h"
#include "soc.h"
#include "strings.h"
#include "uart.h"


void dump_memory(uint8_t *src, uint32_t size);

void main()
{
	printf("Dump irom\r\n");

	dump_memory((uint8_t *)BOOTROM_START, IROM_SIZE);

	printf("bye.\r\n");

	while(1);
}

