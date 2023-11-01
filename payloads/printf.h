#ifndef PRINTF_H
#define PRINTF_H

void putc(int c, void *stream) {
	UART_PUTC(c);
}

void printf(const char *fmt, ...) {
	va_list ap;
	uart_init();

	va_start(ap, fmt);
	kvprintf(fmt, uart_putc, (void*)1, 10, ap);
	va_end(ap);
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

#endif //PRINTF_H
