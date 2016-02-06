#include <x86intrin.h>
#include <stdio.h>
#include <sys/time.h>
#include "crc_1024.h"
#include "crc_parallel.h"

void print_u32(__uint32_t v) {
    printf("0x%08x\n", v);
}

const __uint8_t const *generate_data(size_t size) {
    __uint8_t *data = malloc(sizeof(__uint8_t) * size);
    while (size > 0) {
        size--;
        data[size] = (__uint8_t) (size & 0xff);
    }
    return data;
}

int main() {

    //generate data
    const size_t size = 1024L * 1024L * 1024L * 3L;
    size_t current;
    const __uint8_t const *data = generate_data(size);

    //timer
    struct timeval t1, t2;

    __uint32_t crc;
    int j;
    for (j = 0; j < 10; j++) {

        crc = 0;
        gettimeofday(&t1, NULL);
        for (current = 0; current < size; current += 1024) {
            crc = crc_1024(&data[current], crc);
        }
        gettimeofday(&t2, NULL);
        printf("1024\n");
        print_u32(crc);
        printf("Total time = %f seconds\n",
               (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
               (double) (t2.tv_sec - t1.tv_sec));

        gettimeofday(&t1, NULL);
        crc = crc_p4(data, size);
        gettimeofday(&t2, NULL);
        printf("p4\n");
        print_u32(crc);
        printf("Total time = %f seconds\n",
               (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
               (double) (t2.tv_sec - t1.tv_sec));

        gettimeofday(&t1, NULL);
        crc = crc_p3(data, size);
        gettimeofday(&t2, NULL);
        printf("p3\n");
        print_u32(crc);
        printf("Total time = %f seconds\n",
               (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
               (double) (t2.tv_sec - t1.tv_sec));

        gettimeofday(&t1, NULL);
        crc = crc_p8(data, size);
        gettimeofday(&t2, NULL);
        printf("p8\n");
        print_u32(crc);
        printf("Total time = %f seconds\n",
               (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
               (double) (t2.tv_sec - t1.tv_sec));
    }
    return 0;
}

