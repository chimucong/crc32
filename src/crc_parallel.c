#include "crc_parallel.h"
#include <x86intrin.h>

static const __uint32_t const x8_pow_pow2_table[64] = {
        0x00800000, 0x00008000, 0x82f63b78, 0x6ea2d55c, 0x18b8ea18, 0x510ac59a, 0xb82be955, 0xb8fdb1e7,
        0x88e56f72, 0x74c360a4, 0xe4172b16, 0x0d65762a, 0x35d73a62, 0x28461564, 0xbf455269, 0xe2ea32dc,
        0xfe7740e6, 0xf946610b, 0x3c204f8f, 0x538586e3, 0x59726915, 0x734d5309, 0xbc1ac763, 0x7d0722cc,
        0xd289cabe, 0xe94ca9bc, 0x05b74f3f, 0xa51e1f42, 0x40000000, 0x20000000, 0x08000000, 0x00800000,
        0x00008000, 0x82f63b78, 0x6ea2d55c, 0x18b8ea18, 0x510ac59a, 0xb82be955, 0xb8fdb1e7, 0x88e56f72,
        0x74c360a4, 0xe4172b16, 0x0d65762a, 0x35d73a62, 0x28461564, 0xbf455269, 0xe2ea32dc, 0xfe7740e6,
        0xf946610b, 0x3c204f8f, 0x538586e3, 0x59726915, 0x734d5309, 0xbc1ac763, 0x7d0722cc, 0xd289cabe,
        0xe94ca9bc, 0x05b74f3f, 0xa51e1f42, 0x40000000, 0x20000000, 0x08000000, 0x00800000, 0x00008000
};

__uint32_t poly32mul(__uint32_t lhs, __uint32_t rhs) {
    __m128i a, b, c;
    __uint64_t lhsa __attribute__ ((aligned (16))) = lhs;
    __uint64_t rhsa __attribute__ ((aligned (16))) = rhs;

    a = _mm_loadl_epi64((__m128i *) &lhsa);
    b = _mm_loadl_epi64((__m128i *) &rhsa);
    c = _mm_clmulepi64_si128(a, b, 0x00) << 1;
    __uint32_t *p32 = (__uint32_t *) &c;
    //p32[1] are lower bits
    //p32[0] are higher bits
    return _mm_crc32_u32(0, p32[0]) ^ p32[1];

}

__uint32_t x8_pow_mod(__uint64_t pow) {
    __uint32_t acc = 0x80000000; // 1
    int index = 0;
    while (pow > 0) {
        if (pow & 0x00000001) {
            acc = poly32mul(acc, x8_pow_pow2_table[index]);
        }
        index++;
        pow = pow >> 1;
    }
    return acc;
}

// size must be a multiple of 8*4
__uint32_t crc_p4(const void *data, size_t size) {
    size_t block_size = size / 4;
    __uint64_t *p1 = (__uint64_t *) data;
    __uint64_t *p2 = (__uint64_t *) (data + block_size);
    __uint64_t *p3 = (__uint64_t *) (data + block_size * 2);
    __uint64_t *p4 = (__uint64_t *) (data + block_size * 3);
    __uint64_t c1 = 0, c2 = 0, c3 = 0, c4 = 0;
    size_t current;
    for (current = 0; current < block_size / 8; current++) {
        c1 = _mm_crc32_u64(c1, p1[current]);
        c2 = _mm_crc32_u64(c2, p2[current]);
        c3 = _mm_crc32_u64(c3, p3[current]);
        c4 = _mm_crc32_u64(c4, p4[current]);
    }
    c1 = poly32mul((__uint32_t) c1, x8_pow_mod(block_size * 3));
    c2 = poly32mul((__uint32_t) c2, x8_pow_mod(block_size * 2));
    c3 = poly32mul((__uint32_t) c3, x8_pow_mod(block_size));
    c1 = c1 ^ c2 ^ c3 ^ c4;
    return (__uint32_t) (c1 & 0xffffffff);
}

// size must be a multiple of 8*3
__uint32_t crc_p3(const void *data, size_t size) {
    size_t block_size = size / 3;
    __uint64_t *p1 = (__uint64_t *) data;
    __uint64_t *p2 = (__uint64_t *) (data + block_size);
    __uint64_t *p3 = (__uint64_t *) (data + block_size * 2);
    __uint64_t c1 = 0, c2 = 0, c3 = 0;
    size_t current;
    for (current = 0; current < block_size / 8; current++) {
        c1 = _mm_crc32_u64(c1, p1[current]);
        c2 = _mm_crc32_u64(c2, p2[current]);
        c3 = _mm_crc32_u64(c3, p3[current]);
    }
    c1 = poly32mul((__uint32_t) c1, x8_pow_mod(block_size * 2));
    c2 = poly32mul((__uint32_t) c2, x8_pow_mod(block_size));
    c1 = c1 ^ c2 ^ c3;
    return (__uint32_t) (c1 & 0xffffffff);
}

__uint32_t crc_p8(const void *data, size_t size) {
    size_t block_size = size / 8;
    __uint64_t *p1 = (__uint64_t *) data;
    __uint64_t *p2 = (__uint64_t *) (data + block_size);
    __uint64_t *p3 = (__uint64_t *) (data + block_size * 2);
    __uint64_t *p4 = (__uint64_t *) (data + block_size * 3);
    __uint64_t *p5 = (__uint64_t *) (data + block_size * 4);
    __uint64_t *p6 = (__uint64_t *) (data + block_size * 5);
    __uint64_t *p7 = (__uint64_t *) (data + block_size * 6);
    __uint64_t *p8 = (__uint64_t *) (data + block_size * 7);
    __uint64_t c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, c7 = 0, c8 = 0;
    size_t current;
    for (current = 0; current < block_size / 8; current++) {
        c1 = _mm_crc32_u64(c1, p1[current]);
        c2 = _mm_crc32_u64(c2, p2[current]);
        c3 = _mm_crc32_u64(c3, p3[current]);
        c4 = _mm_crc32_u64(c4, p4[current]);
        c5 = _mm_crc32_u64(c5, p5[current]);
        c6 = _mm_crc32_u64(c6, p6[current]);
        c7 = _mm_crc32_u64(c7, p7[current]);
        c8 = _mm_crc32_u64(c8, p8[current]);
    }
    c1 = poly32mul((__uint32_t) c1, x8_pow_mod(block_size * 7));
    c2 = poly32mul((__uint32_t) c2, x8_pow_mod(block_size * 6));
    c3 = poly32mul((__uint32_t) c3, x8_pow_mod(block_size * 5));
    c4 = poly32mul((__uint32_t) c4, x8_pow_mod(block_size * 4));
    c5 = poly32mul((__uint32_t) c5, x8_pow_mod(block_size * 3));
    c6 = poly32mul((__uint32_t) c6, x8_pow_mod(block_size * 2));
    c7 = poly32mul((__uint32_t) c7, x8_pow_mod(block_size));
    c1 = c1 ^ c2 ^ c3 ^ c4 ^ c5 ^ c6 ^ c7 ^ c8;
    return (__uint32_t) (c1 & 0xffffffff);
}