#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cmath> // for logs

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
    unsigned int seed = world_rank * time(NULL);
    srand(seed);

    long long int each_tosses = tosses / world_size;
    long long int count = 0;    
    
    double x, y;
    for (long long int i = 0; i < each_tosses; i++)
    {
        x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        if (x * x + y * y <= 1.0) count++;
    }

    // TODO: binary tree redunction
    int end_layer = log2(world_size) + 1;
    for (int i=1; i < end_layer; i++) {
        int currentStride = static_cast<int>(pow(2, i));
        int halfStride = currentStride / 2;
        if (world_rank % currentStride == 0) {
            // printf("Rank[%d], layer[%d] RECEIVE from [%d]\n", world_rank, i, world_rank + halfStride);
            long long int result;
            MPI_Recv(&result, 1, MPI_LONG_LONG_INT, world_rank + halfStride, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            count += result;
        } 
        else if (world_rank % currentStride == halfStride) {
            // printf("Rank[%d], layer[%d] SEND to [%d]\n", world_rank, i, world_rank - halfStride);
            MPI_Send(&count, 1, MPI_LONG_LONG_INT, world_rank - halfStride, 0, MPI_COMM_WORLD);
        }
        else {
            // printf("Rank[%d], layer[%d] NOTHING\n", world_rank, i);
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
