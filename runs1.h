#pragma once

#include <omp.h>
#include <stdio.h>

void test1()
{
//watch: https://www.youtube.com/watch?v=I2EaVMjZRRY

    omp_set_num_threads(10); // this overrides the OMP_NUM_THREADS environment variable

    printf("Max number of threads: %d\n", omp_get_max_threads());
    printf("Thread limit: %d\n", omp_get_thread_limit());
    printf("Number of processors: %d\n", omp_get_num_procs());

//https://passlab.github.io/OpenMPProgrammingBook/openmp_c/2_Syntax.html
    #pragma omp parallel num_threads(7) //overrides temporarily
    printf("This is a thread.\n");

#define N 100000
#define CHUNKSIZE 1000
    int i, chunk = CHUNKSIZE;
    float a[N], b[N], c[N];

    // initialize vectors
    for (i = 0; i < N; i++)
    {
        a[i] = i;
        b[i] = i * N;
    }

#pragma omp parallel shared(a, b, c, chunk) private(i)
    {
#pragma omp for schedule(dynamic, chunk)
        for (i = 0; i < N; i++)
            c[i] = a[i] + b[i];

        printf("Thread: %d Chunk: %d\n", omp_get_thread_num(), chunk);
    } // end of parallel

    // for (i = 0; i < 10; i++)
    //     printf("%17.1f %17.1f %17.1f\n", a[i], b[i], c[i]);
    // printf("...\n");
    // for (i = N - 10; i < N; i++)
    //     printf("%17.1f %17.1f %17.1f\n", a[i], b[i], c[i]);
}
