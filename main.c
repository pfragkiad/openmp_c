#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// #include "runs1.h"
#include "runs2.h"

void runSerial()
{
    static long steps = 100000000;
    int i;
    double x, pi, sum = 0.0;
    double step = 1.0 / (double)steps;

    double tdata = omp_get_wtime();

    for (i = 0; i < steps; i++)
    {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    pi = step * sum;

    tdata = omp_get_wtime() - tdata; // elapsed wall time

    printf("(serial) pi is equal to %lf in %lf seconds!\n", pi, tdata);
    // pi is equal to 3.141593 in 0.233378 seconds!
}

void runParallel(int requestedNumThreads)
{
    // watch: https://www.youtube.com/watch?v=I2EaVMjZRRY
    omp_set_num_threads(requestedNumThreads); // thread id from 0 to 17

    static long steps = 100000000;

    // assume 64 byte L1 cache line size
    // https://www.cpu-world.com/CPUs/Core_i7/Intel-Core%20i7%20i7-9750H.html
    // 6 x 32 KB 8-way set associative data caches
    // L1D$	192 KiB	6x32 KiB	8-way set associative (=lines/blocks per set)
    //  6 sets-> 32 KB is the size of the set! => 32KB/8 => 4KB

    // #define USEPAD

#ifdef USEPAD
#define PAD 4096
    double sum[requestedNumThreads][PAD];
#else
    double sum[requestedNumThreads];
#endif

    double step = 1.0 / (double)steps;
    double tdata = omp_get_wtime();

    int threadsCount = requestedNumThreads; // global variable to store the real number of threads

#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int _threadsCount = omp_get_num_threads();
        if (id == 0)
            threadsCount = _threadsCount;

#ifdef USEPAD
        sum[id][0] = 0.0;
#else
        sum[id] = 0.0;
#endif
        for (int i = id; i < steps; i += _threadsCount)
        {
            double x = (i + 0.5) * step;
#ifdef USEPAD
            sum[id][0] += 4.0 / (1.0 + x * x);
#else
            sum[id] += 4.0 / (1.0 + x * x);
#endif
        }
    }
    double pi = 0.0;
    for (int i = 0; i < threadsCount; i++)
#ifdef USEPAD
        pi += sum[i][0];
#else
        pi += sum[i];
#endif
    pi *= step;

    tdata = omp_get_wtime() - tdata; // elapsed wall time

    printf("(parallel used %d threads) pi is equal to %lf in %lf seconds!\n", threadsCount, pi, tdata);
}

void runParallelWithSync(int requestedNumThreads)
{

//https://youtu.be/I2EaVMjZRRY?list=PLGj2a3KTwhRZ6uXoFt_rU5hzx0a5nHHGx&t=5630


    // watch: https://www.youtube.com/watch?v=I2EaVMjZRRY
    omp_set_num_threads(requestedNumThreads); // thread id from 0 to 17

    static long steps = 100000000;

    double pi = 0.0;

    double step = 1.0 / (double)steps;
    double tdata = omp_get_wtime();

    int threadsCount = requestedNumThreads; // global variable to store the real number of threads

#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int _threadsCount = omp_get_num_threads();
        if (id == 0)
            threadsCount = _threadsCount;

        double s = 0.0;
        for (int i = id; i < steps; i += _threadsCount)
        {
            double x = (i + 0.5) * step;
            s += 4.0 / (1.0 + x * x);
        }
#pragma omp critical
        pi += s * step;
    }

    tdata = omp_get_wtime() - tdata; // elapsed wall time

    printf("(parallel2 used %d threads) pi is equal to %lf in %lf seconds!\n", threadsCount, pi, tdata);
}

int main(int argc, char *argv[])
{
    // printf("THREADS: %d\n",omp_get_num_threads()); //will return 1 if outside!
    //  OMP_NUM_THREADS is the external environment variable
    //  in Ubuntu:
    //  export OMP_NUM_THREADS = 10
    //  echo $OMP_NUM_THREADS

    // must run in optimized mode in order to see difference! O3

    //-O3
    /*
(serial) pi is equal to 3.141593 in 0.118699 seconds!
(parallel) pi is equal to 3.141593 in 0.024355 seconds!
    */

    //-Ofast
    /*
 (serial) pi is equal to 3.141593 in 0.057023 seconds!
 (parallel) pi is equal to 3.141593 in 0.023768 seconds!
    */
    runSerial();

    for (int threads = 2; threads <= 30; threads += 2)
        runParallelWithSync(threads);

    return EXIT_SUCCESS;
}