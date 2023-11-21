#include <stdio.h>
#include <omp.h>
#include <unistd.h>

void parallel_for() {
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

void parallel_sections() {
    const int N = 8;
    int A[N] = {1, 2, 3, 4, 5, 6, 7, 8};

    #pragma omp parallel
    {
        int ID = omp_get_thread_num();

        #pragma omp sections
        {
            // for thread 0
            #pragma omp section
            {
                for (int i=0; i < N/2; i++) {
                    A[i] = A[i] + 1;
                    printf("A[%d] is computed by thread: %d\n", i, ID);
                    usleep(1000*100);
                }
            }

            // for thread 1
            #pragma omp section
            {
                for (int i=N/2; i < N; i++) {
                    A[i] = A[i] + 1;
                    printf("A[%d] is computed by thread: %d\n", i, ID);
                    usleep(1000*100);
                }
            }
        }
    }

    for (int i=0; i<N; i++) {
        printf("%d, ", A[i]);
    }
    printf("\n");
}

void parallel_single() {
    #pragma omp parallel num_threads(3)
    {
        const int ID = omp_get_thread_num();
        printf("**Outside** single section, I am thread number %d\n", ID);

        #pragma omp single
        {
            printf("**Inside** single section, I am thread number %d\n", ID);
        }
    }
}

int main() {
    // parallel_for();
    // parallel_sections();
    parallel_single();
    return 0;
}