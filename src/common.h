#include <sys/time.h>
#include <stdlib.h>


//Function returning time value
double stoptime(void) {
   struct timeval t;
   gettimeofday(&t,NULL);
   return (double) t.tv_sec + t.tv_usec/1000000.0;
}

//Function to generate random number in the specified limit
int rand_interval(unsigned int min, unsigned int max)
{
    return min + rand() % max;
}

