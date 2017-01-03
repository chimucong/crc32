#include "crc_s2048.h"
#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>


#define BLOCK_SIZE 672
#define BLOCK_SIZE_8 (BLOCK_SIZE/8)

extern const __uint32_t const mul_table_256[256];

extern const __uint32_t const mul_table_336[256];

extern const __uint32_t const mul_table_672[256];

__inline __uint32_t crc_s2048(const void *buffer, __uint32_t crc_init) {

    __uint64_t crc0, crc1, crc2, tmp;
    const __uint64_t *p;
    p = buffer;
    crc1 = crc2 = 0;
    crc0 = _mm_crc32_u64(crc_init, p[0]);

    int i;
    for (i = 0; i < 84; i++) {
        crc1 = _mm_crc32_u64(crc1, p[1 + BLOCK_SIZE_8 + i]);
        crc2 = _mm_crc32_u64(crc2, p[1 + BLOCK_SIZE_8 * 2 + i]);
        crc0 = _mm_crc32_u64(crc0, p[1 + i]);
    }

    tmp = p[127];
    tmp ^= mul_table_336[crc1 & 0xFF];
    tmp ^= ((__uint64_t) mul_table_336[(crc1 >> 8) & 0xFF]) << 8;
    tmp ^= ((__uint64_t) mul_table_336[(crc1 >> 16) & 0xFF]) << 16;
    tmp ^= ((__uint64_t) mul_table_336[(crc1 >> 24) & 0xFF]) << 24;

    tmp ^= mul_table_672[crc0 & 0xFF];
    tmp ^= ((__uint64_t) mul_table_672[(crc0 >> 8) & 0xFF]) << 8;
    tmp ^= ((__uint64_t) mul_table_672[(crc0 >> 16) & 0xFF]) << 16;
    tmp ^= ((__uint64_t) mul_table_672[(crc0 >> 24) & 0xFF]) << 24;

    return (__uint32_t) _mm_crc32_u64(crc2, tmp);
}
