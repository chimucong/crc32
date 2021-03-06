#include <x86intrin.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "crc_s1024.h"
#include "crc_s1024_p4_.h"
#include "crc_parallel.h"
#include "crc_s2048.h"
#include <sys/resource.h>
//#include <time.h>
//#include <stdlib.h>
//#include <bits/time.h>

static double seconds() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec + now.tv_nsec / 1000000000.0;
}

uint64_t GetTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec;
}

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

static inline uint64_t RDTSC() {
    unsigned int hi, lo;
    __asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t) hi << 32) | lo;
}


extern uint32_t calculateCRC32C(const void *buf1, size_t len, uint32_t crc);

void rep(size_t data_size, size_t repeat, __uint8_t const *data) {
    clock();
    size_t i, current;
    double duration[6] = {0, 0, 0, 0, 0, 0};

    __uint32_t crc;
    uint64_t t1, t2;


    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        crc = 0;
        for (current = 0; current < data_size; current += 1024) {
            crc = crc_s1024(&data[current], crc);
        }
        print_u32(crc);
    }
    t2 = RDTSC();
    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        crc = 0;
        for (current = 0; current < data_size; current += 1024) {
            crc = crc_s1024(&data[current], crc);
        }
        print_u32(crc);
    }
    t2 = RDTSC();
    duration[1] += (t2 - t1);


    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        crc = crc_p4(data, data_size);
        print_u32(crc);
    }
    t2 = RDTSC();
    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        crc = crc_p4(data, data_size);
        print_u32(crc);
    }
    t2 = RDTSC();
    duration[2] += (t2 - t1);

    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        crc = calculateCRC32C(data, data_size, 0);
//        crc = crc_p8(data, data_size);
        print_u32(crc);
    }
    t2 = RDTSC();
    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        crc = calculateCRC32C(data, data_size, 0);
//        crc = crc_p8(data, data_size);
        print_u32(crc);
    }
    t2 = RDTSC();
    duration[3] += (t2 - t1);
    crc = 0;
    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        for (current = 0; current < data_size; current += 2048) {
            crc = crc_s2048(&data[current], crc);
        }
        print_u32(crc);
    }
    t2 = RDTSC();
    crc = 0;
    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        for (current = 0; current < data_size; current += 2048) {
            crc = crc_s2048(&data[current], crc);
        }
        print_u32(crc);
    }
    t2 = RDTSC();
    duration[4] += (t2 - t1);

    crc = 0;
    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        for (current = 0; current < data_size; current += 1024) {
            crc = crc_s1024_p4(&data[current], crc);
        }
        print_u32(crc);
    }
    t2 = RDTSC();
    crc = 0;
    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        for (current = 0; current < data_size; current += 1024) {
            crc = crc_s1024_p4(&data[current], crc);
        }
        print_u32(crc);
    }
    t2 = RDTSC();
    duration[5] += (t2 - t1);

    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        crc = 0;
        for (current = 0; current < data_size; current += 8) {
            crc = (uint32_t) _mm_crc32_u64(crc, *(__uint64_t *) (data + current));
        }
        print_u32(crc);
    }
    t2 = RDTSC();

    t1 = RDTSC();
    for (i = 0; i < repeat; i++) {
        crc = 0;
        for (current = 0; current < data_size; current += 8) {
            crc = (uint32_t) _mm_crc32_u64(crc, *(__uint64_t *) (data));
        }
        print_u32(crc);
    }
    t2 = RDTSC();
    duration[0] += (t2 - t1);


    duration[0] /= repeat;
    duration[1] /= repeat;
    duration[2] /= repeat;
    duration[3] /= repeat;
    duration[4] /= repeat;
    duration[5] /= repeat;

    printf("%li\t%li\t%G\t%G\t%G\t%G\t%G\t%G\n", data_size, repeat, duration[0], duration[1], duration[2],
           duration[3],
           duration[4], duration[5]);
}

int main() {


    if (setpriority(PRIO_PROCESS, 0, -20) < 0) {
        perror("setpriority");
        return 0;
    }

    unsigned long mask = 0x0100000000; /* processors 33 */
    unsigned int len = sizeof(mask);
    if (sched_setaffinity(0, len, &mask) < 0) {
        perror("sched_setaffinity");
        return -1;
    } else {
        printf("\nsched_setaffinity success\n");
    }

    size_t i;
    __uint8_t const *data = generate_data(1024L * 1024L);
    for (i = 1; i <= 1024; i++) {
        rep(2048L * i, 10000, data);
    }


//    long i = CLOCKS_PER_SEC;
//    int sum = 0;
//    clock_t t1, t2;
//    t1 = clock();
//    for (int j = 0; j < 10; ++j) {
//        sum += j;
//    }
//    t2 = clock();
//    printf("clocks:%ld\nsum:%d", t2 - t1, sum);

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

