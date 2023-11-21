#include <stdio.h>
#include <omp.h>
#include <unistd.h>

void example_barrier() {
    printf("Case 1, without barrier!\n");
    #pragma omp parallel num_threads(2)
    {
        const int ID = omp_get_thread_num();
        printf("A1 I am thread %d\n", ID);
        printf("A2 I am thread %d\n", ID);
        printf("B I am thread %d\n", ID);
    }

    printf("Case 2, with barrier!\n");
    #pragma omp parallel num_threads(2)
    { 
        const int ID = omp_get_thread_num();
        printf("A1 I am thread %d\n", ID);
        printf("A2 I am thread %d\n", ID);
        #pragma omp barrier
        printf("B I am thread %d\n", ID);
    }
}

void example_critical() {
    int number = 0;
    int N = 10000;
    #pragma omp parallel num_threads(2)
    {
        #pragma omp for
        for (int i=0; i<N; i++) {
            number++;
        }

        #pragma omp single
        printf("Without critical, the number is :%d\n", number);
    }

    number = 0;
    #pragma omp parallel num_threads(2)
    {
        #pragma omp for
        for (int i=0; i<N; i++) {
            #pragma omp critical
            {
                number++;   
            }
        }

        #pragma omp single
        printf("With critical, the number is :%d\n", number);
    }
}

void example_ordered() {
    const int N = 8;
    int A[N] = {1, 2, 3, 4, 5, 6, 7, 8};
    #pragma omp parallel
    {
        const int ID = omp_get_thread_num();
        
        #pragma omp for ordered
        for (int i=0; i<N; i++) {
            
            #pragma omp ordered 
            {
                A[i] = A[i] + 1;
                printf("A[%d] is computed by thread : %d\n", i, ID);
            }
        }
    }
}

void example_reduction() {
    int sum = 0;
    #pragma omp parallel
    {
        #pragma omp for reduction(+:sum)
        // #pragma omp for
        for (int i=0; i < 10000; i++) {
            for (int j=0; j < 50000; j++) {
                sum += 1;
                // #pragma omp critical
                // {
                //     sum += 1;
                // }
            }
        }
    }
    printf("sum: %d\n", sum);
}

int main() {
    // example_barrier();
    // example_critical();
    // example_ordered();
    example_reduction();
    return 0;
}