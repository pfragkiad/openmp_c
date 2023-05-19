#pragma once

#include <omp.h>
#include <stdio.h>

void run2()
{

#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int threadsCount = omp_get_num_threads(); //this is the real one

        printf("(%d of %d) hello ", id, threadsCount);
        printf(" world \n");
    } // that's a barrier here!
}