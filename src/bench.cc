#include <time.h>
#include <stdint.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
//#include "crc_s1024_p4_.h"
extern "C" uint32_t crc_s1024_p4(const void *buffer, uint32_t crc_init);
extern "C" __uint32_t crc_s1024(const void *buffer, __uint32_t crc_init);
static const int BUFFER_MAX = 128 * 1024 * 1024;
static const int ALIGNMENT = 8;
static const int TRIALS = 5;

static int cmpDouble(const void *p1, const void *p2) {
    if (*(double *) p1 > *(double *) p2) return 1;
    if (*(double *) p1 == *(double *) p2) return 0;
    return -1;
}

static double seconds() {
    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec + now.tv_nsec / 1000000000.0;
}

int main() {
    char *buffer = new char[BUFFER_MAX + ALIGNMENT];
    char *aligned_buffer = (char *) (((intptr_t) buffer + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
    assert(aligned_buffer + BUFFER_MAX <= buffer + BUFFER_MAX + ALIGNMENT);

    // fill the buffer with non-zero data
    for (int i = 0; i < BUFFER_MAX; ++i) {
        aligned_buffer[i] = (char) i;
    }

    double runTimes[TRIALS];
    int length = 1024;
    const char *data = aligned_buffer;
    int iterations = BUFFER_MAX / length;
    uint32_t crc = 0;
    double startTime, duration;
    for (int j = 0; j < TRIALS; ++j) {
        crc = 0;
        startTime = seconds();
        for (int i = 0; i < iterations; ++i) {
            crc = crc_s1024_p4(data, crc);
        }
        duration = seconds() - startTime;
        runTimes[j] = duration;
    }
    qsort(runTimes, TRIALS, sizeof(double), cmpDouble);
    printf("\t%.3f", 128.0 / runTimes[(TRIALS + 1) / 2 - 1]);
    printf("\titerations:%d\n",iterations);
    delete[] buffer;
    return 0;
}