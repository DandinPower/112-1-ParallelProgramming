#include <stdlib.h>
#include <stdio.h>
#include <time.h>

double randomDouble(double min, double max) {
    double scale = rand() / (double) RAND_MAX;
    return min + scale * (max - min);
}

int main(void) {
    srand(time(NULL));
    unsigned long long number_in_circle = 0;
    unsigned long long number_of_tosses = 5e8;
    double x, y, distance_squared, pi_estimate;

    for (int toss = 0; toss < number_of_tosses; toss ++) {
        x = randomDouble(-1, 1);
        y = randomDouble(-1, 1);
        distance_squared = x * x + y * y;
        if ( distance_squared <= 1)
            number_in_circle++;
    }
    pi_estimate = 4 * number_in_circle /(( double ) number_of_tosses);
    printf("%lf\n", pi_estimate);
    return 0;
}