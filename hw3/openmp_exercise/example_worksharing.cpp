#include <stdio.h>
#include <omp.h>

void exercise_0() {
    const int N = 8;
    int A[N] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        
        #pragma omp for
        for (int i=0; i<N; i++) {
            A[i] = A[i] + 1;
            printf("A[%d] is computed by thread: %d\n", i, ID);
        }
    }

    for (int i=0; i<N; i++) {
        printf("%d, ", A[i]);
    }
    printf("\n");
}

int main() {
    exercise_0();
    return 0;
}