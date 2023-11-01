#include <stdint.h>
#include "common.h"
#include "soc.h"
#include "uart.h"

void uart_dump_memory(uint8_t* src, uint32_t size) {	
	for (uint32_t i = 0; i < size; i++) {
		UART_PUTC(src[i]);
	}
}

void main()
{
	uart_print("Dumping IROM:\n\n");

	uart_dump_memory((uint8_t *)BOOTROM_START, IROM_SIZE);

	uart_print("\n\nFinished dumping IROM!\n");

	while(1);
}

