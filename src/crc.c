
__uint32_t shift(__uint32_t crc, size_t byte) {
    while (byte > 0) {
        crc = _mm_crc32_u8(crc, 0);
        byte--;
    }
    return crc;
}

__uint32_t shift_s8(__uint32_t crc, size_t byte) {
    __uint64_t crc64 = crc;
    while (byte >= 8) {
        crc64 = _mm_crc32_u64(crc64, 0);
        byte -= 8;
    }
    crc = (__uint32_t) crc64;
    if (byte >= 4) {
        crc = _mm_crc32_u32(crc, 0);
        byte -= 4;
    }
    if (byte >= 2) {
        crc = _mm_crc32_u16(crc, 0);
        byte -= 2;
    }
    if (byte >= 1) {
        crc = _mm_crc32_u8(crc, 0);
    }
    return crc;
}


__uint32_t crc_data(void *data, __uint32_t crc, size_t byte) {

    __uint64_t crc64 = crc;
    __uint64_t *p64 = data;
    while (byte >= 8) {
        crc64 = _mm_crc32_u64(crc64, *p64);
        byte -= 8;
        p64++;
    }

    __uint32_t *p32 = (__uint32_t *) p64;
    crc = (__uint32_t) crc64;
    if (byte >= 4) {
        crc = _mm_crc32_u32(crc, *p32);
        byte -= 4;
        p32++;
    }

    __uint16_t *p16 = (__uint16_t *) p32;
    if (byte >= 2) {
        crc = _mm_crc32_u16(crc, *p16);
        byte -= 2;
    }

    __uint8_t *p8 = (__uint8_t *) p16;
    if (byte >= 1) {
        crc = _mm_crc32_u8(crc, *p8);
    }
    return crc;
}

