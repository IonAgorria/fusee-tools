#ifndef TEGRA_UART_H
#define TEGRA_UART_H

#define HEX_CHAR(x) ((((x) + '0') > '9') ? ((x) + '7') : ((x) + '0'))

#define PMC_BASE					(0x7000e400)
#define APBDEV_PMC_SCRATCH41_0				(0x140)
#define APBDEV_PMC_SCRATCH42_0				(0x144)
#define MAGIC_VALUE					(0x42)
#define APBDEV_PMC_IO_DPD_REQ_0				(0x1b8)
#define UART_DPD_BIT					(1 << 14)

#define PINMUX_BASE					(0x70003000)

#define PINMUX_AUX_ULPI_DATA0_0				(0x00)	/* T30 uart-a tx */
#define PINMUX_AUX_ULPI_DATA1_0				(0x04)	/* T30 uart-a rx */

#define PINMUX_AUX_SDMMC3_DAT1_0 			(0x39c)	/* T114 uart-a tx */
#define PINMUX_AUX_SDMMC3_CMD_0				(0x394)	/* T114 uart-a rx */

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
#define UARTB_BASE					(0x70006040)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 		(0x10)
#define CLK_RST_CONTROLLER_RST_DEVICES_L_0 		(0x04)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTA_0 		(0x178)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTB_0 		(0x17c)
#define UARTA_CAR_MASK 					(1 << 6)
#define UARTB_CAR_MASK 					(1 << 7)

#if defined(UART_A_USE) || defined(UART_B_USE)

#define CLK_RST_CONTROLLER_CLK_OUT_ENB 			CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0
#define CLK_RST_CONTROLLER_RST_DEVICES 			CLK_RST_CONTROLLER_RST_DEVICES_L_0

#if defined(UART_A_USE)
  #define UART_BASE 					UARTA_BASE
  #define UART_CAR_MASK 				UARTA_CAR_MASK
  #define CLK_RST_CONTROLLER_CLK_SOURCE_UART 		CLK_RST_CONTROLLER_CLK_SOURCE_UARTA_0
#elif defined(UART_B_USE)
  #define UART_BASE 					UARTB_BASE
  #define UART_CAR_MASK 				UARTB_CAR_MASK
  #define CLK_RST_CONTROLLER_CLK_SOURCE_UART 		CLK_RST_CONTROLLER_CLK_SOURCE_UARTB_0
#endif

#endif /* common */

void uart_print(const char *string);
void putc(int c, void *stream);
void uart_init();
char* utoa(unsigned int value, char* result, int base);
#endif
