#include "common.h"
#include "soc.h"
#include "pmc_reset.h"
#include "uart.h"

void main()
{
	reg_write(PMC_BASE, APBDEV_PMC_SCRATCH42_0, 0);

	uart_print("Dumping IROM:\n\n");
	uart_dump_memory((uint8_t *)BOOTROM_START, BOOTROM_SIZE);

	uart_print("\n\nFinished dumping IROM!\n");
	uart_print_line();

	pmc_reset_rcm();
}
