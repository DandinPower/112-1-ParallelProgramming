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

    // TODO: init MPI
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    long long int each_tosses = tosses / world_size;
    long long int count = 0;
    double x, y;
    
    unsigned int seed = world_rank * time(NULL);
    srand(seed);
    if (world_rank > 0)
    {
        // TODO: handle workers
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
        // TODO: master
        for (long long int i = 0; i < each_tosses; i++)
        {
            x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) count++;
        }
    }

    if (world_rank == 0)
    {
        // TODO: process PI result
        for (long long int i = 1; i < world_size; i++) {
            long long int result;
            MPI_Recv(&result, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            count += result;
        }

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