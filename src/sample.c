#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

#define SECONDS 600
//Global variables
double time_taken[4];
long int num_of_loops = 0;
//pthread_mutex_t var=PTHREAD_MUTEX_INITIALIZER;
int second=0;
double stoptime(void) {
   struct timeval t;
   gettimeofday(&t,NULL);
   return (double) t.tv_sec + t.tv_usec/1000000.0;
}

long int thread_sec[4][SECONDS];
//void* addmul(double add, double mul, int ops){
void* addmul(void* vargs){
   // Need to initialise differently otherwise compiler might optimise away
    //printf("Threadid: %d\n",(int)vargs);
	int a = *((int *) vargs);
	printf("Threadid: %d\n",a);
    double add = M_PI;
   double mul = 1.0 + 1e-8;
   double sum1=0.1, sum2=-0.1, sum3=0.2, sum4=-0.2, sum5=0.0;
   double mul1=1.0, mul2= 1.1, mul3=1.2, mul4= 1.3, mul5=1.4;
   int loops=10000/10;          // We have 10 floating point operations inside the loop
   double t = stoptime();
   //double expected = 5.0*add*loops + (sum1+sum2+sum3+sum4+sum5)
    //           + pow(mul,loops)*(mul1+mul2+mul3+mul4+mul5);
   int i;
  while(second<SECONDS)
   {
       for (i=0; i<10; i++)
        {
          mul1*=mul; mul2*=mul; mul3*=mul; mul4*=mul; mul5*=mul;
          sum1+=add; sum2+=add; sum3+=add; sum4+=add; sum5+=add;
        }
        thread_sec[a][second]+=10;
   }
   time_taken[a] += (stoptime() - t);
   printf("Time Taken by %d=> %.3f\n", a, stoptime() - t);

   //pthread_mutex_unlock(&var); // unlock once you are done
}


void* timer(void* dummy)
{
    while(second < SECONDS)
    {
    printf("Second %d\n",second);
    sleep(1);
    second+=1;
    }
}

int main(int argc, char *argv[]) {
   if (argc != 2) {
      printf("usage: %s <num>\n", argv[1]);
      printf("number of operations: <num> millions\n");
      exit(EXIT_FAILURE);
   }
    double add = M_PI;
    double mul = 1.0 + 1e-8; 
    num_of_loops = 100000000;          // We have 10 floating point operations inside the loop
    int num_thread= atoi(argv[1]);
    int i;
    pthread_t thread_id[num_thread+1];
    for (i = 0; i < num_thread; ++i ) {
        int *arg = malloc(sizeof(*arg));
        if ( arg == NULL ) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }

        *arg = i;
        pthread_create(&thread_id[i], NULL, addmul, arg);
    }
    pthread_create(&thread_id[num_thread], NULL, timer, NULL);
    int j;
    for(j=0; j<num_thread; j++)
      {
         pthread_join( thread_id[j], NULL);
      }
   double time_taken_total  =0 ;
   for(j=0; j<num_thread; j++)
      {
         time_taken_total += time_taken[j];
      } 
    double avg = time_taken_total /num_thread;
    printf("GFLOPS = %.8f\n", num_of_loops*10/(avg*1e9));
    for(j=0; j<SECONDS;j++)
    {
      //printf("%lu ,%lu ,%lu ,%lu\n", thread_sec[0][j], thread_sec[1][j], thread_sec[2][j], thread_sec[3][j]);
      printf("%d\t%lu\n", j+1, thread_sec[0][j] + thread_sec[1][j] + thread_sec[2][j] + thread_sec[3][j]);
    }
    return EXIT_SUCCESS;
}
