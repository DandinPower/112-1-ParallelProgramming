#include <stdio.h>
#include <omp.h>
#include <unistd.h>

void example_private() {
    int var = 10;
    printf("var: private variable\n");

    #pragma omp parallel private(var)
    {
        const int ID = omp_get_thread_num();
        var += 1;
        printf("var: %d | computed by thread: %d\n", var, ID);
    }
}

void example_firstprivate() {
    int var = 10;
    printf("var: private variable\n");

    #pragma omp parallel firstprivate(var)
    {
        const int ID = omp_get_thread_num();
        var += 1;
        printf("var: %d | computed by thread: %d\n", var, ID);
    }
}

void example_shared() {
    int var = 10;
    printf("var: shared variable\n");

    #pragma omp parallel shared(var)
    {
        const int ID = omp_get_thread_num();
        var += 1;
        printf("var: %d | computed by thread: %d\n", var, ID);
    }
}
int main() {
    // example_private();
    // example_firstprivate();
    example_shared();
    return 0;
}