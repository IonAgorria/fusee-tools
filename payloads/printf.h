#ifndef PRINTF_H
#define PRINTF_H

static void printf_putc(int c, void *stream) {
	uart_putc(c);
}

static void printf(const char *fmt, ...) {
	va_list ap;
	uart_init();

	va_start(ap, fmt);
	kvprintf(fmt, printf_putc, (void*)1, 10, ap);
	va_end(ap);
}

#endif //PRINTF_H
