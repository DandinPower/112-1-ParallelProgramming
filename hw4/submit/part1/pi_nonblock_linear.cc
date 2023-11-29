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

    // TODO: MPI init
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    long long int each_tosses = tosses / world_size;
    long long int count = 0;
    double x, y;
    
    unsigned int seed = world_rank * time(NULL);
    srand(seed);

    if (world_rank > 0)
    {
        // TODO: MPI workers
        for (long long int i = 0; i < each_tosses; i++)
        {
            x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) count++;
        }
        MPI_Send(&count, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        for (long long int i = 0; i < each_tosses; i++)
        {
            x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) count++;
        }
        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
        MPI_Request requests[world_size - 1];
        long long int receive_array[world_size -1];
        for (int i=1; i < world_size; i++) {
            MPI_Irecv(&receive_array[i-1], 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &requests[i-1]);
        }

        MPI_Waitall(world_size - 1, requests, MPI_STATUSES_IGNORE);
        for (int i=0; i < world_size-1; i++) {
            count += receive_array[i];
        }
    }

    if (world_rank == 0)
    {
        // TODO: PI result
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
