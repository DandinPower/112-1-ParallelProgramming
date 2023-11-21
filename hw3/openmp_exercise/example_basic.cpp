#include <stdio.h>
#include <omp.h>

void exercise_0() {
    printf("Hello World! (Master)!\n");

    #pragma omp parallel
    {
        printf("Hi! (Multithreading)\n");
    }
}

void exercise_1() {

    #pragma omp parallel num_threads(4)
    {
        int ID = omp_get_thread_num();
        printf("Hi! I am thread number %d.\n", ID);
    }

    omp_set_num_threads(3);
    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf("Hi! I am thread number %d.\n", ID);
    }
}

int main(void) {
    // exercise_0();
    exercise_1();
    return 0;
}