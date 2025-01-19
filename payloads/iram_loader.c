#include "common.h"
#include "memory.h"
#include "pmc_reset.h"

void main()
{
	//Check end of iram loader makes sense
	uint32_t* payload_ptr = (uint32_t*) &END_ADDR;
	if (*payload_ptr != 0xFADEC0DE)  {
		pmc_reset();
	}
	//Check if code to load into IRAM has magic
	payload_ptr += 1;
	if (*payload_ptr != 0xC0DE10AD)  {
		pmc_reset();
	}
	
	//Get length
	payload_ptr += 1;
	uint32_t payload_len = *payload_ptr;
	
	//Now payload_ptr is pointing to actual code of payload
	payload_ptr += 1;

	//Where is the payload code to jump into
	void* payload_start;

	if (((uint32_t) payload_ptr & 0xFF000000) == 0x40000000) {
		//Already in IRAM? no need to move simply store the address for spray
		payload_start = payload_ptr;
	} else {
		//main() is usually located at LOAD_ADDR
		payload_start = &main;

		//Do the copy to IRAM
		memcpy(payload_start, payload_ptr, payload_len);
	}
	
	//Spray for memcpy to work
	for (uint32_t i = 0x40002000; i < 0x40003000; i += 4) {
		*((uint32_t*) i) = (uint32_t) payload_start;
	}

	//All ready, lets go!
	pmc_reset_rcm();
}
