#ifndef MEMORY_H
#define MEMORY_H

void memset(void* addr, uint8_t val, uint32_t len) {
	for (uint32_t i = 0; i < len; ++i) {
		*((uint8_t*) addr + i) = val;
	}
}

void memcpy(void* dst, void* src, uint32_t len) {
	if ((((uint32_t) dst) % 4) == 0 && (((uint32_t) src) % 4) == 0) {
		uint32_t fastlen = (len / 4) * 4;
		len = len % 4;
		for (uint32_t i = 0; i < fastlen; i += 4) {
			*((uint8_t*) dst + i) = *((uint8_t*) src + i);
		}
		src += fastlen;
		dst += fastlen;
	}
	for (uint32_t i = 0; i < len; ++i) {
		*((uint8_t*) dst + i) = *((uint8_t*) src + i);
	}
}

#endif //MEMORY_H