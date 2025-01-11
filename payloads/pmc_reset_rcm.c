#define NO_MAIN_WORKAROUND
#include "common.h"
#include "pmc_reset.h"

void main()
{
	pmc_reset_rcm();
}
