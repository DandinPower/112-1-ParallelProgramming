#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

unsigned long long number_in_circle = 0;    // global variable
pthread_mutex_t lock;   // used to protect global variable access

// generate a random double scale
double randomScale(unsigned int* seedp) {
    return rand_r(seedp) / (double) RAND_MAX;
}

// thread function
void* toss(void* arg) {
    pthread_t thread_id = pthread_self();
    int tosses = *((int*) arg);
    double x, y, xScale, yScale, distance_squared;
    int number_in_circle_local = 0;
    for (int toss = 0; toss < tosses; toss ++) {
        xScale = randomScale((unsigned int*)(&thread_id));
        yScale = randomScale((unsigned int*)(&thread_id));
        x = -1 + xScale * 2;
        y = -1 + yScale * 2;
        distance_squared = x * x + y * y;
        if ( distance_squared <= 1)
            number_in_circle_local++;
    }
    pthread_mutex_lock(&lock);
    number_in_circle += number_in_circle_local;
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 3) {
        printf("Wrong number of arguments\n");
        return 0;
    }
    int number_of_threads = atoi(argv[1]);
    unsigned long long number_of_tosses = atoi(argv[2]);
    
    pthread_t threads[number_of_threads];
    int tosses_per_thread = number_of_tosses / number_of_threads;
    int remain = number_of_tosses;
    for (int i = 0; i < number_of_threads; i++) {
        int min = remain < tosses_per_thread ? remain : tosses_per_thread;
        pthread_create(&threads[i], NULL, toss, &min);
        remain -= tosses_per_thread;
    }
    for (int i = 0; i < number_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    double pi_estimate = 4 * number_in_circle /(( double ) number_of_tosses);
    printf("%lf\n", pi_estimate);
    return 0;
}