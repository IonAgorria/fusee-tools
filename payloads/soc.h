#ifndef SOC_H
#define SOC_H

#if defined(T20) | defined(T30)
    #define IROM_SIZE (48 * 1024)
#elif defined(T114)
    #define IROM_SIZE (64 * 1024)
#else
    #error No SoC specified
#endif

#endif //SOC_H
