#include "common.h"
#include "tegra_uart.h"
#include <stdarg.h>

void putc(int c, void *stream) {
	// use this to keep track of if uart has been initialized
	//uart_init();

#ifdef UART_BASE
	// put the char into the tx fifo
	reg_write(UART_BASE, UART_THR_DLAB, c);

	// wait for tx fifo to clear
	while(!((reg_read(UART_BASE, UART_LSR) >> 5) & 0x01));
#endif
}

void uart_print(const char *string) {
	// use this to keep track of if uart has been initialized
	uart_init();
	
	// send all characters until NULL to uart-N
	while(*string) {
#ifdef UART_BASE
		// put the char into the tx fifo
		reg_write(UART_BASE, UART_THR_DLAB, (char) *string);

		// wait for tx fifo to clear
		while(!((reg_read(UART_BASE, UART_LSR) >> 5) & 0x01));
#endif

		// move on to next char
		++string;
	}
}


void uart_init() {
	if(reg_read(PMC_BASE, APBDEV_PMC_SCRATCH42_0) != MAGIC_VALUE) {

#ifdef UART_BASE
#if defined(T30) && defined(UART_A_USE)
		reg_write(PINMUX_BASE, PINMUX_AUX_ULPI_DATA0_0, 0b00000110); /* tx */
		reg_write(PINMUX_BASE, PINMUX_AUX_ULPI_DATA1_0, 0b00100110); /* rx */
#elif defined(T114) && defined(UART_A_USE)
		reg_write(PINMUX_BASE, PINMUX_AUX_SDMMC3_DAT1_0, 0); /* tx */
		reg_write(PINMUX_BASE, PINMUX_AUX_SDMMC3_CMD_0, 0); /* rx */
#else
    #error No UART specified
#endif

		/* clear deep power down for all uarts */
		reg_clear(PMC_BASE, APBDEV_PMC_IO_DPD_REQ_0, UART_DPD_BIT);

		/* enable clock for uart-N */
		reg_set(CAR_BASE, CLK_RST_CONTROLLER_CLK_OUT_ENB, UART_CAR_MASK);

		/* assert and deassert reset for uart-N */
		reg_set(CAR_BASE, CLK_RST_CONTROLLER_RST_DEVICES, UART_CAR_MASK);
		reg_clear(CAR_BASE, CLK_RST_CONTROLLER_RST_DEVICES, UART_CAR_MASK);

		/* set clock source to pllp for uart-N */
		reg_write(CAR_BASE, CLK_RST_CONTROLLER_CLK_SOURCE_UART, CLK_SOURCE_PLLP);

		/* set DLAB bit to enable programming of DLH/DLM registers */
		reg_set(UART_BASE, UART_LCR, LCR_DLAB);

		/* write lower 8 (DLH) and upper 8 (DLM) bits of 16 bit baud divisor */
		reg_write(UART_BASE, UART_THR_DLAB, (UART_RATE_115200 & 0xff));
		reg_write(UART_BASE, UART_IER_DLAB, (UART_RATE_115200 >> 8));

		/* clear DLAB bit to disable setting of baud divisor */
		reg_clear(UART_BASE, UART_LCR, LCR_DLAB);

		/* 8-bit word size - defaults are no parity and 1 stop bit */
		reg_write(UART_BASE, UART_LCR, LCR_WD_SIZE_8);

		/* enable tx/rx fifos */
		reg_write(UART_BASE, UART_IIR_FCR, FCR_EN_FIFO);
#endif

		/* prevent this uart-N initialization from being done on subsequent calls to uart_print() */
		reg_write(PMC_BASE, APBDEV_PMC_SCRATCH42_0, MAGIC_VALUE);
		
		/* use this reg as UART_MUTEX in a multicore set up :) */
		//reg_write(PMC_BASE, APBDEV_PMC_SCRATCH41_0, 0);
	}
}
