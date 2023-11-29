#include <cstdio>
#include <mpi.h>
#include <string.h>

int world_rank, world_size;

void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr,
                        int **a_mat_ptr, int **b_mat_ptr) {
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if (world_rank == 0) {
        scanf("%d %d %d", n_ptr, m_ptr, l_ptr);
        *a_mat_ptr = new int[*n_ptr * *m_ptr];
        *b_mat_ptr = new int[*m_ptr * *l_ptr];
        for(int i = 0; i < *n_ptr * *m_ptr; i++)
            scanf("%d", *a_mat_ptr + i);
        for(int i = 0; i < *m_ptr * *l_ptr; i++)
            scanf("%d", *b_mat_ptr + i);
    }

    MPI_Bcast(n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(m_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(l_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (world_rank != 0) {
        *a_mat_ptr = new int[*n_ptr * *m_ptr];
        *b_mat_ptr = new int[*m_ptr * *l_ptr];
    }
    MPI_Bcast(*a_mat_ptr, (*n_ptr * *m_ptr), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(*b_mat_ptr, (*m_ptr * *l_ptr), MPI_INT, 0, MPI_COMM_WORLD);
}

int make_multiple_of_N(int num, int N) {
    if (num % N == 0) return num;
    else return ((num / N) + 1) * N;
}

void matrix_multiply(const int n, const int m, const int l,
                     const int *a_mat, const int *b_mat) {
    
    int pad_n = make_multiple_of_N(n, world_size);
    int *pad_result = new int[pad_n * l]();
    memset(pad_result, 0, pad_n * l * sizeof(int));

    // for(int i = 0; i < n; i++)
    //     for(int j = 0; j < l; j++)
    //         for(int k = 0; k < m; k++)
    //             pad_result[i * l + j] += a_mat[i * m + k] * b_mat[k * l + j];
    int each_row = pad_n / world_size;
    int each_element = each_row * l;
    int start_row = world_rank * each_row;
    int end_row = start_row + each_row;
    for(int i = start_row; i < end_row; i++) {
        for(int k = 0; k < m; k++) {
            int temp = a_mat[i * m + k];
            for(int j = 0; j < l; j++)
                pad_result[i * l + j] += temp * b_mat[k * l + j];
        }
    }
        
    MPI_Gather(pad_result + (world_rank * each_element), each_element, MPI_INT, pad_result, each_element, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (world_rank == 0) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < l; j++)
                printf("%d ", pad_result[i * l + j]);
            printf("\n");
        }
    }
    
    delete[] pad_result;
}

void destruct_matrices(int *a_mat, int *b_mat) {
    delete[] a_mat;
    delete[] b_mat;
}