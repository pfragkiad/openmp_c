#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "runs1.h"

int main(int argc, char *argv[])
{
    // OMP_NUM_THREADS is the external environment variable
    // in Ubuntu:
    // export OMP_NUM_THREADS = 10
    // echo $OMP_NUM_THREADS

    omp_set_num_threads(18); // thread id from 0 to 17

#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int threadsCount = omp_get_num_threads();

        printf("(%d of %d) hello ", id, threadsCount);
        printf(" world \n");
    } // that's a barrier here!

    return EXIT_SUCCESS;
}