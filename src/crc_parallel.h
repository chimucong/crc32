#ifndef  CRC_PARALLEL_H
#define  CRC_PARALLEL_H

#include <bits/types.h>
#include <stddef.h>

__uint32_t poly32mul(__uint32_t lhs, __uint32_t rhs);

__uint32_t x8_pow_mod(__uint64_t pow);

__uint32_t crc_p4(const void *data, size_t size);

__uint32_t crc_p8(const void *data, size_t size);

__uint32_t crc_p3(const void *data, size_t size);

#endif