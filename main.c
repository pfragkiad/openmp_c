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
    omp_set_num_threads(requestedNumThreads); // thread id from 0 to 17

    static long steps = 100000000;

    double sum[requestedNumThreads];

    double step = 1.0 / (double)steps;
    double tdata = omp_get_wtime();

    int threadsCount = requestedNumThreads; // global variable to store the real number of threads

#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int _threadsCount = omp_get_num_threads();
        if (id == 0)
            threadsCount = _threadsCount;

        sum[id] = 0.0;
        for (int i = id; i < steps; i += _threadsCount)
        {
            double x = (i + 0.5) * step;
            sum[id] += 4.0 / (1.0 + x * x);
        }
    }
    double pi = 0.0;
    for (int i = 0; i < threadsCount; i++)
        pi += sum[i];
    pi *= step;

    tdata = omp_get_wtime() - tdata; // elapsed wall time

    printf("(parallel used %d threads) pi is equal to %lf in %lf seconds!\n", threadsCount, pi, tdata);
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

    for (int threads = 2; threads <= 30; threads+=2)
        runParallel(threads);

    return EXIT_SUCCESS;
}