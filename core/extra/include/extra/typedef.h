#ifndef FLARE_TYPEDEF_H
#define FLARE_TYPEDEF_H

typedef uint8_t byte;
typedef uint32_t uint;
/** @bug This has the wrong size on windows (Is still the case when using MinGW?) */
typedef uint64_t ulong;
static_assert(sizeof(ulong) == 8, "llu is not 8 bytes");

#endif

