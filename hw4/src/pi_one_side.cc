#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    MPI_Win win;
    // TODO: MPI init
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    long long int* receive_buffer;
    long long int each_tosses = tosses / world_size;
    long long int count = 0;
    double x, y;
    unsigned int seed = world_rank * time(NULL);
    srand(seed);

    if (world_rank == 0)
    {
        // Master
        MPI_Alloc_mem(world_size * sizeof(long long int), MPI_INFO_NULL, &receive_buffer);
        for (int i=0; i<world_size; i++) receive_buffer[i] = 0;
        MPI_Win_create(receive_buffer, world_size * sizeof(long long int), sizeof(long long int), MPI_INFO_NULL,
          MPI_COMM_WORLD, &win);
        
        for (long long int i = 0; i < each_tosses; i++)
        {
            x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) count++;
        }
    }
    else
    {
        // Workers
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        for (long long int i = 0; i < each_tosses; i++)
        {
            x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) count++;
        }

        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Put(&count, 1, MPI_LONG_LONG_INT, 0, world_rank, 1, MPI_LONG_LONG_INT, win);
        MPI_Win_unlock(0, win);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result
        for (int i = 1; i < world_size; i++) {
            count += receive_buffer[i];
        }
        MPI_Free_mem(receive_buffer);
        pi_result = 4.0 * count / tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}