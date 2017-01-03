#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <nmmintrin.h>

void table_generator(size_t size) {
    int i, j;
    uint32_t crc;
    for (i = 0; i < 256; i++) {
        crc = (uint32_t) i;
        for (j = 0; j < size; j++) {
            crc = _mm_crc32_u8(crc, 0);
        }
        printf("0x%08x,\t", crc);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
}


int main() {
    table_generator(256*3);
    return 0;
}