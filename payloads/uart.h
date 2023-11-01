#ifndef TEGRA_UART_H
#define TEGRA_UART_H

#define HEX_CHAR(x) ((((x) + '0') > '9') ? ((x) + '7') : ((x) + '0'))

#define MAGIC_VALUE					(0x42)
#define APBDEV_PMC_IO_DPD_REQ_0				(0x1b8)
#define UART_DPD_BIT					(1 << 14)

#define PINMUX_BASE					(0x70003000)

#define APB_MISC_PP_TRISTATE_REG_B_0			(0x18)	/* T20 uart-b UAD TRISTATE */
#define UAD_TRISTATE_ON					(1 << 21)

#define APB_MISC_PP_PIN_MUX_CTL_A_0			(0x80)	/* T20 uart-b UAD GROUP MUX */
#define UAD_SEL_MASK					(0b11 << 6)

#define PINMUX_AUX_ULPI_DATA0_0				(0x00)	/* T30 uart-a tx */
#define PINMUX_AUX_ULPI_DATA1_0				(0x04)	/* T30 uart-a rx */

#define PINMUX_AUX_ULPI_CLK_0				(0x20)	/* T30 uart-d tx */
#define PINMUX_AUX_ULPI_DIR_0				(0x24)	/* T30 uart-d rx */

#define PINMUX_AUX_SDMMC3_DAT1_0 			(0x39c)	/* T114 uart-a tx */
#define PINMUX_AUX_SDMMC3_CMD_0				(0x394)	/* T114 uart-a rx */

#define PINMUX_AUX_GMI_A16_0 				(0x230)	/* T114 uart-d tx */
#define PINMUX_AUX_GMI_A17_0				(0x234)	/* T114 uart-d rx */

#define CAR_BASE					(0x60006000)
#define CLK_SOURCE_PLLP					(0x0)

#define UART_THR_DLAB					(0x00)
#define UART_IER_DLAB					(0x04)
#define UART_IIR_FCR					(0x08)
#define UART_LCR					(0x0c)
#define UART_LSR					(0x14)

/* aka 408000000/115200/29 - works for IROM, assuming 408000000 PLLP */
// 122 is original for APX
#define UART_RATE_115200				(122)
/* clear TX FIFO */
#define FCR_TX_CLR					(1 << 2)
/* clear RX FIFO */
#define FCR_RX_CLR					(1 << 1)
/* enable TX & RX FIFO */
#define FCR_EN_FIFO					(1 << 0)
/* Divisor Latch Access Bit */
#define LCR_DLAB					(1 << 7)
/* word length of 8 */
#define LCR_WD_SIZE_8					(0x3)

#define UARTA_BASE					(0x70006000)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 		(0x10)
#define CLK_RST_CONTROLLER_RST_DEVICES_L_0 		(0x04)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTA_0 		(0x178)
#define UARTA_CAR_MASK 					(1 << 6)

#define UARTB_BASE					(0x70006040)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTB_0 		(0x17c)
#define UARTB_CAR_MASK 					(1 << 7)

#define UARTD_BASE					(0x70006300)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_U_0		(0x18)
#define CLK_RST_CONTROLLER_RST_DEVICES_U_0		(0x0c)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTD_0		(0x1c0)
#define UARTD_CAR_MASK					(1 << 1)

#if defined(UART_A_USE)
  #define UART_BASE 					UARTA_BASE
  #define CLK_RST_CONTROLLER_CLK_OUT_ENB 		CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0
  #define CLK_RST_CONTROLLER_RST_DEVICES 		CLK_RST_CONTROLLER_RST_DEVICES_L_0
  #define UART_CAR_MASK 				UARTA_CAR_MASK
  #define CLK_RST_CONTROLLER_CLK_SOURCE_UART 		CLK_RST_CONTROLLER_CLK_SOURCE_UARTA_0
#elif defined(UART_B_USE)
  #define UART_BASE 					UARTB_BASE
  #define CLK_RST_CONTROLLER_CLK_OUT_ENB 		CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0
  #define CLK_RST_CONTROLLER_RST_DEVICES 		CLK_RST_CONTROLLER_RST_DEVICES_L_0
  #define UART_CAR_MASK 				UARTB_CAR_MASK
  #define CLK_RST_CONTROLLER_CLK_SOURCE_UART 		CLK_RST_CONTROLLER_CLK_SOURCE_UARTB_0
#elif defined(UART_D_USE)
  #define UART_BASE					UARTD_BASE
  #define CLK_RST_CONTROLLER_CLK_OUT_ENB		CLK_RST_CONTROLLER_CLK_OUT_ENB_U_0
  #define CLK_RST_CONTROLLER_RST_DEVICES		CLK_RST_CONTROLLER_RST_DEVICES_U_0
  #define UART_CAR_MASK					UARTD_CAR_MASK
  #define CLK_RST_CONTROLLER_CLK_SOURCE_UART		CLK_RST_CONTROLLER_CLK_SOURCE_UARTD_0
#else
    #error No UART specified
#endif

void uart_init() {
	if(reg_read(PMC_BASE, APBDEV_PMC_SCRATCH42_0) != MAGIC_VALUE) {

#ifdef UART_BASE
#if defined(T20) && defined(UART_B_USE)
		/* T20 cannot configure individual pins, only groups */
		reg_set(PINMUX_BASE, APB_MISC_PP_TRISTATE_REG_B_0, UAD_TRISTATE_ON);
		reg_clear(PINMUX_BASE, APB_MISC_PP_PIN_MUX_CTL_A_0, UAD_SEL_MASK);
#elif defined(T30) && defined(UART_A_USE)
		reg_write(PINMUX_BASE, PINMUX_AUX_ULPI_DATA0_0, 0b00000110); /* tx */
		reg_write(PINMUX_BASE, PINMUX_AUX_ULPI_DATA1_0, 0b00100110); /* rx */
#elif defined(T30) && defined(UART_D_USE)
		reg_write(PINMUX_BASE, PINMUX_AUX_ULPI_CLK_0, 0b00000110); /* tx */
		reg_write(PINMUX_BASE, PINMUX_AUX_ULPI_DIR_0, 0b00100110); /* rx */
#elif defined(T114) && defined(UART_A_USE)
		reg_write(PINMUX_BASE, PINMUX_AUX_SDMMC3_DAT1_0, 0); /* tx */
		reg_write(PINMUX_BASE, PINMUX_AUX_SDMMC3_CMD_0, 0); /* rx */
#elif defined(T114) && defined(UART_D_USE)
		reg_write(PINMUX_BASE, PINMUX_AUX_GMI_A16_0, 0); /* tx */
		reg_write(PINMUX_BASE, PINMUX_AUX_GMI_A17_0, 0); /* rx */
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

void printf(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	kvprintf(fmt, putc, (void*)1, 10, ap);
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

#endif
