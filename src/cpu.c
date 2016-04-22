#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <limits.h>
#include "common.h"

//Defining number of samples to be taken
#define SECONDS 600

//Defining maximum possible threads
#define MAX_THREADS 4

//Defining FLOPS and IOPS 
#define FLOPS 1
#define IOPS 2

//Defining the struct to be used for sharing data between different calls for flops/iops computation
struct ops_struct {
	int thread_id;
    unsigned long ops;
    double time_taken;
};

//Global variables
struct ops_struct ops_calc_array[MAX_THREADS];
struct ops_struct ops_interval_array[MAX_THREADS][SECONDS];

int interval = SECONDS;
int second=0;

//This thread maintains the state of the timer, Used by interval compute threads
void* timer(void* dummy)
{
    while(second < interval)
    {
    //printf("Second %d\n",second);
    sleep(1);
    second+=1;
    }
}

// Function which does the computations for number of iterations
void* compute_flops(void* vargs)
{
	//Getting thread id
	int thread_id = *((int *) vargs);
	
	//defining utility variables
    volatile double add = M_PI;
	volatile double mul = 1.0 + 1e-8;
	volatile double i;
	volatile double sum1=2.41, sum2=-3.1, sum3=4.2, sum4=-55.2, sum5=90.4608, sum6=0.45, sum7=23.67, sum8=45.933;
	volatile double mul1=67.05, mul2= 19.11, mul3=881.222, mul4= 67.31, mul5=1.4, mul6=2.43, mul7=19.41, mul8=81.56;
	
	//Starting timer
	double t = stoptime();

	for (i=0; i < 1000*USHRT_MAX; i++) 
	{
      mul1*=mul; mul2*=mul; mul3*=mul; mul4*=mul; mul5*=mul; mul6*=mul; mul7*=mul; mul8*=mul; //Multiple parallelizable multiply Instructions; 16 Operations
      sum1+=add; sum2+=add; sum3+=add; sum4+=add; sum5+=add; sum6+=add; sum7+=add; sum8+=add; //Multiple parallelizable add Instructions; 16 Operations
	}
	//Stopping timer
	ops_calc_array[thread_id].time_taken = stoptime() - t;
	ops_calc_array[thread_id].thread_id =  thread_id;
	ops_calc_array[thread_id].ops = 35*1000*(long)USHRT_MAX;
}

// Function which does the computations for number of iterations
void* compute_iops(void* vargs)
{
	//Getting thread id
	int thread_id = *((int *) vargs);
	
	//defining utility variables
    volatile int add = CHAR_MAX;
	volatile int mul = CHAR_BIT;
	volatile int i;
	volatile int sum1=91, sum2=-56, sum3=47, sum4=-89, sum5=67, sum6=23, sum7=9, sum8=73;
	volatile int mul1=3, mul2= 70, mul3=12, mul4= 19, mul5=66, mul6=82, mul7=71, mul8=57;
	
	//Starting timer
	double t = stoptime();

	for (i=0; i < 1000*USHRT_MAX; i++) //i++ => 2 Operations; i < 1000*USHRT_MAX ==> 1 operations
	{
      mul1*=mul; mul2*=mul; mul3*=mul; mul4*=mul; mul5*=mul; mul6*=mul; mul7*=mul; mul8*=mul; //Multiple parallelizable multiply Instructions; 16 Operations
      sum1+=add; sum2+=add; sum3+=add; sum4+=add; sum5+=add; sum6+=add; sum7+=add; sum8+=add; //Multiple parallelizable add Instructions; 16 Operations
	}
	//Stopping timer
	ops_calc_array[thread_id].time_taken = stoptime() - t;
	ops_calc_array[thread_id].thread_id =  thread_id;
	ops_calc_array[thread_id].ops = 35*1000*(long)USHRT_MAX;
}

// Function which does the computations for number of iterations
void* flops_interval(void* vargs)
{
	//Getting thread id
	int thread_id = *((int *) vargs);
	//printf("Threadid: %d\n",a);
	
	//defining utility variables
    volatile double add = M_PI;
    volatile double mul = 1.0 + 1e-8;
    volatile double i;
    volatile double sum1=0.1, sum2=-0.1, sum3=0.2, sum4=-0.2, sum5=0.0, sum6=0.45, sum7=23.67, sum8=45.933;
	volatile double mul1=1.0, mul2= 1.1, mul3=1.2, mul4= 1.3, mul5=1.4, mul6=2.43, mul7=19.41, mul8=81.56;
    double t = stoptime();
    
	while(second < interval)
    {
       for (i=0; i<10; i++) //i++ => 2 Operations; i < 1000*USHRT_MAX ==> 1 operations
        {
			mul1*=mul; mul2*=mul; mul3*=mul; mul4*=mul; mul5*=mul; mul6*=mul; mul7*=mul; mul8*=mul; //Multiple parallelizable multiple Instructions;16 Operations
			sum1+=add; sum2+=add; sum3+=add; sum4+=add; sum5+=add; sum6+=add; sum7+=add; sum8+=add; //Multiple parallelizable add Instructions;16 Operations
				
        }
        //This will put some overhead for number of operations in second but since it will be common for all the threads I'm going ahead
        ops_interval_array[thread_id][second].ops += 352;  //This assignment and addition constitutes 2 operations
    }
}

// Function which does the IOP computations for number of iterations
void* iops_interval(void* vargs)
{
	//Getting thread id
	int thread_id = *((int *) vargs);
	
    //defining utility variables
    volatile int add = CHAR_MAX;
	volatile int mul = CHAR_BIT;
	volatile int i;
	volatile int sum1=91, sum2=-56, sum3=47, sum4=-89, sum5=67, sum6=23, sum7=9, sum8=73;
	volatile int mul1=3, mul2= 70, mul3=12, mul4= 19, mul5=66, mul6=82, mul7=71, mul8=57;
	
    double t = stoptime();
	while( second < interval)
    {
       for (i=0; i<10; i++)
        {
			mul1*=mul; mul2*=mul; mul3*=mul; mul4*=mul; mul5*=mul; mul6*=mul; mul7*=mul; mul8*=mul; //Multiple parallelizable multiply Instructions
			sum1+=add; sum2+=add; sum3+=add; sum4+=add; sum5+=add; sum6+=add; sum7+=add; sum8+=add; //Multiple parallelizable add Instructions
        }
        //This will put some overhead for number of operations in second but since it will be common for all the threads
        ops_interval_array[thread_id][second].ops += 352;	//35*10 + 2(This operation itself)
    }
}

// Launch threads and timer to calculate and store calculations per interval
void ops_in_interval(int op, int num_thread)
{
	//Defining utility variables
	int i, j, k;
	
	//Creating and launching the threads 
    pthread_t thread_id[num_thread+1];
    for (i = 0; i < num_thread; ++i ) 
    {
        int *arg = malloc(sizeof(*arg));
        if ( arg == NULL ) 
        {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }

        *arg = i;
        //Launching all threads
        if(op == FLOPS)
        {
			pthread_create(&thread_id[i], NULL, flops_interval, arg);
		}
		else if(op == IOPS)
		{
			pthread_create(&thread_id[i], NULL, iops_interval, arg);
		}
    }
    //Create the timer thread
    pthread_create(&thread_id[num_thread], NULL, timer, NULL);
    
    //Lets join all the threads together
    for(j=0; j<num_thread; j++)
    {
		pthread_join( thread_id[j], NULL);
    }
    
    //Join the timer thread
	pthread_join( thread_id[num_thread], NULL);
	
	//Create a csv file for output of this program
	char * filename = op == IOPS?"gnuplot_iops.data":"gnuplot_flops.data";
	FILE *file = fopen(filename, "w");
	
	//Print the total time taken by all threads every second during the duration it was ran for
	for(j=0; j<interval;j++)
    {
		double total_time_taken = 0;
		double total_operations_done = 0;
		for(k=0; k < MAX_THREADS; k++)
		{
			total_operations_done += ops_interval_array[k][j].ops;
		}
		//printf("%d\t%.4f\n", j+1, total_operations_done/1e9);      //To stdout
		fprintf(file, "%d\t%.4f\n", j+1, total_operations_done/1e9); //To the file
    }
    fclose(file);
}

double cpu_benchmarking(int op, int num_thread)
{
	double add = M_PI;
	double avg;
    double mul = 1.0 + 1e-8;
    int i, j;
    
    
    //Creating and launching the threads 
    pthread_t thread_id[num_thread];
    for (i = 0; i < num_thread; ++i ) 
    {
        int *arg = malloc(sizeof(*arg));
        if ( arg == NULL ) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }
 
        *arg = i;
        if(op == FLOPS)
        {
			pthread_create(&thread_id[i], NULL, compute_flops, arg);
		}
		else if(op == IOPS)
		{
			pthread_create(&thread_id[i], NULL, compute_iops, arg);
		}
    }
    
    //Joining the threads
    for(j=0; j<num_thread; j++)
    {
        pthread_join( thread_id[j], NULL);
    }
    double time_taken_total = 0;
    double total_ops_performed = 0;
    //Calculating total time taken by each thread for the iterations to finish
    for(j=0; j<num_thread; j++)
    {
		time_taken_total += ops_calc_array[j].time_taken;
		total_ops_performed += ops_calc_array[j].ops;
    }
    
    // Calculating average time taken by each thread
    //GFLOPS = total_ops_performed/time_taken_total;
    return num_thread*total_ops_performed/(time_taken_total*1e9);
}

int main(int argc, char *argv[]) 
{
    int num_thread= 4; // We have multithreading support; Num of threads can be configured with this number
	int sample = 0;
	int i = 0;
	int all = 0;
	//Command line parsing
	for (i=1; i< argc; i=i+2) 
	{
		if(0 == strcmp(argv[i], "--sampling"))
		{
			sample = atoi(argv[i+1]);
		}
		else if(0 == strcmp(argv[i], "--default"))
		{
			all = atoi(argv[i+1]);
			sample = 600;
		}
		else if(0 == strcmp(argv[i], "--numthreads"))
		{
			num_thread = atoi(argv[i+1]);
		}
		else if(0 == strcmp(argv[i], "--help"))
		{
			printf("Use of cpu benchmarking executable:\n");
			printf("\t./cpu\n");
			printf("\t\t --numthreads y ==> If you want to use y threads in experiment\n");
			printf("\t\t --sampling xxx ==> If you want to sample GFLOPS and IOPS for xxx seconds\n");
			printf("\t\t --default 1 ==> If you want to go for default test case\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Use of cpu benchmarking executable:\n");
			printf("\t./cpu\n");
			printf("\t\t --numthreads y ==> If you want to use y threads in experiment\n");
			printf("\t\t --sampling xxx ==> If you want to sample GFLOPS and IOPS for xxx seconds\n");
			printf("\t\t --default 1 ==> If you want to go for default test case\n");
			exit(EXIT_SUCCESS);
		}
	}
		
	//Calculating function to calculate number of GFLOPS
	if(all != 1)
	{
		printf("Running OPS calculation with %d threads\n", num_thread);
		
		double gflops = cpu_benchmarking(FLOPS, num_thread);
		
		printf("\tGFLOPS => %.3f\n",gflops);
		
		double iops = cpu_benchmarking(IOPS, num_thread);
		
		printf("\tIOPS => %.3f\n",iops);
	}
	else
	{
		printf("Running calculations for FLOPS and IOPS\n");
		printf("-----------------------------------------\n");
		printf("\tThreads\t GFLOPS\t GIOPS\n");
		printf("-----------------------------------------\n");
		for(i=1; i<=4; i++)
		{
			double gflops = cpu_benchmarking(FLOPS, i);
			double iops = cpu_benchmarking(IOPS, i);
			printf("\t %d \t %.3f \t %.3f\t\n", i, gflops, iops);
		}
		printf("-----------------------------------------\n");
	}
	if (sample > 1)
	{
		printf("Sampling FLOPS over %d seconds\n", sample);
		interval = sample; 
		num_thread = 4; //Using 4 threads for all the sampling cases
		
		//Calling the function to log number of iterations done per second
		second = 0; //Initializing the global value of seconds
		ops_in_interval(FLOPS, num_thread);
		printf("Sampling FLOPS done\n");
		
		printf("Sampling IOPS over %d seconds\n", sample);
		second = 0; //Re-Initializing the global value of seconds
		ops_in_interval(IOPS, num_thread);
		printf("Sampling IOPS done\n");
	}
	
    return EXIT_SUCCESS;

}
