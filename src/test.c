#include <x86intrin.h>
#include <stdio.h>
#include <sys/time.h>
#include "crc_s1024.h"
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
    double duration[5] = {0, 0, 0, 0, 0};
    struct timeval t1, t2;

    __uint32_t crc;
    for (i = 0; i < repeat; i++) {


        crc = 0;
        gettimeofday(&t1, NULL);
        for (current = 0; current < data_size; current += 4) {
            crc = _mm_crc32_u32(crc, *(__uint32_t *) (data + current));
        }
        gettimeofday(&t2, NULL);
        print_u32(crc);
        duration[0] += (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
                       (double) (t2.tv_sec - t1.tv_sec);

        crc = 0;
        gettimeofday(&t1, NULL);
        for (current = 0; current < data_size; current += 1024) {
            crc = crc_s1024(&data[current], crc);
        }
        gettimeofday(&t2, NULL);
        print_u32(crc);
        duration[1] += (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
                       (double) (t2.tv_sec - t1.tv_sec);


        gettimeofday(&t1, NULL);
        crc = crc_p4(data, data_size);
        gettimeofday(&t2, NULL);
        print_u32(crc);
        duration[2] += (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
                       (double) (t2.tv_sec - t1.tv_sec);

        gettimeofday(&t1, NULL);
        crc = crc_p8(data, data_size);
        gettimeofday(&t2, NULL);
        print_u32(crc);
        duration[3] += (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
                       (double) (t2.tv_sec - t1.tv_sec);

        gettimeofday(&t1, NULL);
        crc = crc_p16(data, data_size);
        gettimeofday(&t2, NULL);
        print_u32(crc);
        duration[4] += (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
                       (double) (t2.tv_sec - t1.tv_sec);

    }
    free((void *) data);

    duration[0] /= repeat;
    duration[1] /= repeat;
    duration[2] /= repeat;
    duration[3] /= repeat;
    duration[4] /= repeat;

    printf("%li\t%li\t%G\t%G\t%G\t%G\t%G\n", data_size, repeat, duration[0], duration[1], duration[2], duration[3],
           duration[4]);
}

int main() {
    size_t i;
    for (i = 1; i <= 1024; i++) {
        rep(1024L * 1024L * i, 100);
    }

}

int main1() {

    //generate data
    const size_t size = 1024L * 1024L * 3L;
    size_t current;
    const __uint8_t const *data = generate_data(size);

    //timer
    struct timeval t1, t2;



    //-------
    __uint32_t crc;
    int round = 1;
    int j;
    for (j = 0; j < round; j++) {

        crc = 0;
        gettimeofday(&t1, NULL);
        for (current = 0; current < size; current += 1024) {
            crc = crc_s1024(&data[current], crc);
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

//        gettimeofday(&t1, NULL);
//        crc = crc_p3(data, size);
//        gettimeofday(&t2, NULL);
//        printf("p3\n");
//        print_u32(crc);
//        printf("Total time = %f seconds\n",
//               (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
//               (double) (t2.tv_sec - t1.tv_sec));

        gettimeofday(&t1, NULL);
        crc = crc_p8(data, size);
        gettimeofday(&t2, NULL);
        printf("p8\n");
        print_u32(crc);
        printf("Total time = %f seconds\n",
               (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
               (double) (t2.tv_sec - t1.tv_sec));

        gettimeofday(&t1, NULL);
        crc = crc_p16(data, size);
        gettimeofday(&t2, NULL);
        printf("p16\n");
        print_u32(crc);
        printf("Total time = %f seconds\n",
               (double) (t2.tv_usec - t1.tv_usec) / 1000000 +
               (double) (t2.tv_sec - t1.tv_sec));


        //-----
    }
    return 0;
}

