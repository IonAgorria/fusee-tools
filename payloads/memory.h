#ifndef MEMORY_H
#define MEMORY_H

void memset(void* addr, uint8_t val, uint32_t len) {
    for (uint32_t i = 0; i < len; ++i) {
        *((uint8_t*) addr + i) = val;
    }
}

void memcpy(void* dst, void* src, uint32_t len) {
    for (uint32_t i = 0; i < len; ++i) {
        *((uint8_t*) dst + i) = *((uint8_t*) src + i);
    }
}

#endif //MEMORY_H
