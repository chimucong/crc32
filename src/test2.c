#include <x86intrin.h>
#include <stdio.h>
#include <time.h>
#include "crc_s1024.h"
#include "crc_s1024_p4_.h"
#include "crc_parallel.h"


__uint32_t tmp;

void print_u32(__uint32_t v) {
    tmp ^= v;
//    printf("0x%08x\t", v);
}

const __uint8_t const *generate_data(size_t size) {
    __uint8_t *data = aligned_alloc(16, sizeof(__uint8_t) * size);
    while (size > 0) {
        size--;
        data[size] = (__uint8_t) (size & 0xff);
    }
    return data;
}


void rep(size_t data_size, size_t repeat) {
    __uint8_t const *data = generate_data(data_size);
    size_t i, current;
    double duration[6] = {0, 0, 0, 0, 0, 0};

    clock_t t;
    __uint32_t crc;
    for (i = 0; i < repeat; i++) {


        crc = 0;
        t = clock();
        for (current = 0; current < data_size; current += 8) {
            crc = (uint32_t) _mm_crc32_u64(crc, *(__uint32_t *) (data + current));
        }
        t = clock() - t;
        print_u32(crc);
        duration[0] += (double) (t) / CLOCKS_PER_SEC;


        crc = 0;
        t = clock();
        for (current = 0; current < data_size; current += 1024) {
            crc = crc_s1024(&data[current], crc);
        }
        t = clock() - t;
        print_u32(crc);
        duration[1] += (double) (t) / CLOCKS_PER_SEC;


        t = clock();
        crc = crc_p4(data, data_size);
        t = clock() - t;
        print_u32(crc);
        duration[2] += (double) (t) / CLOCKS_PER_SEC;

        t = clock();
        crc = crc_p8(data, data_size);
        t = clock() - t;
        print_u32(crc);
        duration[3] += (double) (t) / CLOCKS_PER_SEC;

        t = clock();
        crc = crc_p16(data, data_size);
        t = clock() - t;
        print_u32(crc);
        duration[4] += (double) (t) / CLOCKS_PER_SEC;

        crc = 0;
        t = clock();
        for (current = 0; current < data_size; current += 1024) {
            crc = crc_s1024_p4(&data[current], crc);
        }
        t = clock() - t;
        print_u32(crc);
        duration[5] += (double) (t) / CLOCKS_PER_SEC;

    }
    free((void *) data);

    duration[0] /= repeat;
    duration[1] /= repeat;
    duration[2] /= repeat;
    duration[3] /= repeat;
    duration[4] /= repeat;
    duration[5] /= repeat;

    printf("%li\t%li\t%G\t%G\t%G\t%G\t%G\t%G\n", data_size, repeat, duration[0], duration[1], duration[2], duration[3],
           duration[4], duration[5]);
}

int main() {
    size_t i;
    for (i = 1; i <= 1024; i++) {
        rep(1024L * i, 1000);
    }

}

