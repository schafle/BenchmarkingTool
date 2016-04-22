#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include "common.h"

//Defining the access types 
#define SEQUENTIAL 1
#define RANDOM 2

//Definig block sizes
#define B 1L
#define KB 1024L
#define MB 1024*1024L

//Defining maximum threads
#define MAX_THREADS 2

//Defining Inner iterations
# define ITERATIONS 100

//Defining the struct to be used by memory_benchmarking method to pass arguments to threads
struct arg_struct {
	int thread_id;
    char* source_block;
    char* dest_block;
    long block_size;
};

//To maintain values of throughput and latency accross multiple threads
double throughput[MAX_THREADS];   
double latency[MAX_THREADS];

//Initialzes an experimental block space which will be used by access functions to access data in memory
char* allocate_and_initialize(long block_size)
{
	char* block_ptr;
	char atoz[] = "abcdefghijklmnopqrstuvwxyz";
	long long i;
	//Allocate the memory of size BLOCKSIZE
	block_ptr = (char*)malloc(block_size*ITERATIONS);
	if(block_ptr == NULL)                     
    {
        printf("Error! memory not allocated.");
        exit(0);
    }
    
    //Initializing the allocated space with random char from a to z    
    for(i=0;i<block_size*ITERATIONS;++i)
    {
        block_ptr[i] = atoz[rand_interval(0,25)];
    }
    return block_ptr;
}
	
//Accessing the memory space sequentially
void* sequential_access(void* arguments)
{
	//Unpacking the arguments
	struct arg_struct *args = (struct arg_struct *)arguments;

    //Defining local variables I will be using
    char* source_block = args -> source_block;
    char* dest_block = args -> dest_block;
    long block_size = args -> block_size;
    int thread = args -> thread_id;
	int i=0;
	long j=0;
	char* source_block_temp = source_block;
	char* dest_block_temp = dest_block;
	
	//Lets calculate the throughput now
	double t = stoptime();
	for(j=0;j<1024;j++)
    {
		dest_block = dest_block_temp; //Setting it to initial position
		source_block = source_block_temp; //Setting it to initial position
		for(i=0;i<ITERATIONS;i++)
		{
			memcpy(dest_block, source_block, block_size);
			dest_block += block_size;
			source_block += block_size;
		}
	}
    double time_taken = (stoptime() - t);
    //throughput[thread] = 100*1024*block_size/(1024*1024*time_taken); //10 inner, 1024 outer and blocksize together combines the blocks transferred
    throughput[thread] = ITERATIONS*block_size/(1024*time_taken); //Simplifying calculation, Refer line above for details
    
    //Lets calculate the latency now
    t = stoptime();
   for(j=0;j<1024;j++)
   {
		dest_block = dest_block_temp; //Setting it to initial position
		source_block = source_block_temp; //Setting it to initial position
		for(i=0;i<ITERATIONS;i++)
		{
			memset(dest_block, 'a', block_size);
			dest_block += block_size;
		}
	}
    time_taken = (stoptime() - t);
    latency[thread] = time_taken/ITERATIONS*1024*1000;  //Time taken by each block will be calculated when we divide by total iterations
}

//random_access memcpy for throughput and memset for latency
void* random_access(void* arguments)
{
	//Unpacking the arguments
	struct arg_struct *args = (struct arg_struct *)arguments;

    //Defining local variables I will be using
    char* source_block = args -> source_block;
    char* dest_block = args -> dest_block;
    long block_size = args -> block_size;
    int thread = args -> thread_id;
	int i=0, j=0;
	
	//Lets calculate the throughput now
	double t = stoptime();
	char* source_block_temp = source_block;
	char* dest_block_temp = dest_block;
	for(j=0;j<1024;j++)
	{
		dest_block = dest_block_temp; //Setting it to initial position
		source_block = source_block_temp; //Setting it to initial position
		for(i=0;i<ITERATIONS;i++)
		{ 
			memcpy(dest_block, source_block, block_size);
			dest_block = dest_block_temp + rand_interval(0,ITERATIONS)*block_size;
			source_block = source_block_temp + rand_interval(0,ITERATIONS)*block_size;
		}
	}
    double time_taken = (stoptime() - t);
    throughput[thread] = ITERATIONS*1024*block_size/(1024*1024*time_taken); //ITERATIONS inner, 1024 outer and blocksize constitutes total blocks transferred
    t = stoptime();
    
    //Lets calculate the latency now

    for(j=0;j<1024;j++)
	{
		dest_block = dest_block_temp;
		for(i=0;i<ITERATIONS;i++)
		{
			memset(dest_block, 'a', block_size);
			dest_block = dest_block_temp + rand_interval(0,ITERATIONS)*block_size;
		}
	}
    time_taken = (stoptime() - t);
    latency[thread] = time_taken/ITERATIONS*1024*1000;  //Doing the block transfer ITERATIONS times 1024 times
}

void memory_benchmarking(long block_size, char access_type, int num_threads)
{
	char* source_block = allocate_and_initialize(block_size);
    char* dest_block = allocate_and_initialize(block_size);
	//This basically decides how many bytes are going to be read/write with memcpy	
	if(access_type == SEQUENTIAL)
	{
		int i;
		pthread_t thread_id[num_threads]; //For multiple threads
		for (i = 0; i < num_threads; ++i) 
		{
			//Creating a structure to pass to thread 
			struct arg_struct* args = malloc(sizeof(struct arg_struct));
			if ( args == NULL ) 
			{
				fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
				exit(EXIT_FAILURE);
			}
			//Making the structure to pass the threads as argument
			args->source_block = source_block;
			args->dest_block = dest_block;
			args->block_size = block_size;
			args->thread_id = i;
			//Launching threads
			pthread_create(&thread_id[i], NULL, &sequential_access, args);
		}
		int j;
		//Waiting for all the threads to finish
		for(j=0; j<num_threads; j++)
		{
			pthread_join( thread_id[j], NULL);
		}
	}
	else if(access_type == RANDOM)
	{
		int i;
		pthread_t thread_id[num_threads];
		for (i = 0; i < num_threads; ++i ) 
		{
			//Creating a structure to pass to thread 	
			struct arg_struct* args = malloc(sizeof(struct arg_struct));
			if ( args == NULL ) 
			{
				fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
				exit(EXIT_FAILURE);
			}
			//Making the structure to pass the threads as argument
			args->source_block = source_block;
			args->dest_block = dest_block;
			args->block_size = block_size;
			args->thread_id = i;
			//Launhing threads
			pthread_create(&thread_id[i], NULL, &random_access, args);
		}
		int j;
		//Waiting for all the threads to finish
		for(j=0; j<num_threads; j++)
		{
			pthread_join( thread_id[j], NULL);
		}
	}
	//Freeing the memory here
	free(source_block);
	free(dest_block);
	
	//Getting values of all the threads to calculate average
	double total_time_taken = 0;
	double combined_throughput = 0;
	int j;
	for(j=0; j<num_threads; j++)
	{
		total_time_taken += latency[j];
		combined_throughput += throughput[j];
	}

	printf("%s\t",access_type==1?"Sequential":"  Random  ");
	printf("   %7ld\t",block_size);
	printf("\t%d\t",num_threads);
	printf("%10.4f\t",combined_throughput/num_threads);
	printf("%.15f\t\n",total_time_taken/num_threads);
	return;
}

int main(int argc, char *argv[]) 
{
   if (argc < 2) {
      printf("Please provide some arguments\n");
      printf("Use -h/--help for reference\n");
      exit(EXIT_FAILURE);
   }
   //Defining the values which will be passed to benchmarking function
   long long block_size = KB;
   char SERIAL;
   int num_threads=1;
   int i;
   char all = 0;
   int access = 1;
   
   //Command line parsing
	for (i=1; i< argc; i=i+2) 
	{
		if(0 == strcmp(argv[i], "--blocksize"))
		{
			if(0 == strcmp(argv[i+1], "B"))
			{
				block_size = B;
			}
			else if(0 == strcmp(argv[i+1], "KB"))
			{
				block_size = KB;
			}
			else if(0 == strcmp(argv[i+1], "MB"))
			{
				block_size = MB;
			}
			//break;
		}
		else if(0 == strcmp(argv[i], "--access"))
		{
			if(0 == strcmp(argv[i+1], "random"))
			{
				access = RANDOM;
			}
			else if(0 == strcmp(argv[i+1], "sequential"))
			{
				access = SEQUENTIAL;
			}
		}
		else if(0 == strcmp(argv[i], "--numthreads"))
		{
			num_threads = atoi(argv[i+1]);
		}
		else if(0 == strcmp(argv[i], "--default"))
		{
			all = 1;
			break;
		}
		else if(0 == strcmp(argv[i], "--help") | 0 == strcmp(argv[i], "-h"))
		{
			printf("Use of memory benchmarking executable:\n");
			printf("\t./memory\n");
			printf("\t\t --blocksize B/KB/MB\n");
			printf("\t\t --access random/sequential\n");
			printf("\t\t --numthreads 1/2/4/n\n");
			printf("\t\t --default --> Run all test cases\n");
			exit(EXIT_SUCCESS);
		}
		else
		{
			printf("Use of memory benchmarking executable:\n");
			printf("\t./memory\n");
			printf("\t\t --blocksize B/KB/MB\n");
			printf("\t\t --access random/sequential\n");
			printf("\t\t --numthreads 1/2/4/n\n");
			printf("\t\t --default --> Run all test cases\n");
			exit(EXIT_FAILURE);
		}
	}

	if(all != 1) //Run the experiment with given parameters
	{
		printf("----------------------------------*-------------------------------------------\n");
		printf("Access type\t Blocksize\t   Num-Threads\t Throughput\t   Latency\n");
		printf("                                                   MB/s             ms       \n");
		printf("-----------------------------------------------------------------------------\n");
		memory_benchmarking(block_size, access, num_threads);
		printf("\n---------------------------------------------------------------------------\n");
	}
	else //Run all the experiments in sequence
	{
		printf("-----------------------------------------------------------------------------\n");
		printf("Access type\t Blocksize\t   Num-Threads\t Throughput\t   Latency\n");
		printf("                                                   MB/s             ms       \n");
		printf("-----------------------------------------------------------------------------\n");
		//Experiment 1
		memory_benchmarking(1*B, SEQUENTIAL, 1);  				// Block size = 1B, Sequential, 1 thread
		
		//Experiment 2
		memory_benchmarking(1*B, RANDOM, 1);					// Block size = 1B, Random, 1 thread
		
		//Experiment 3
		memory_benchmarking(1*B, SEQUENTIAL, 2);				// Block size = 1B, Sequential, 2 thread
		
		//Experiment 4
		memory_benchmarking(1*B, RANDOM, 2);				    // Block size = 1B, Random, 2 thread
		
		//Experiment 5
		memory_benchmarking(1*KB, SEQUENTIAL, 1);				// Block size = 1KB, Sequential, 1 thread
		
		//Experiment 6
		memory_benchmarking(1*KB, RANDOM, 1);					// Block size = 1KB, Random, 1 thread
		
		//Experiment 7
		memory_benchmarking(1*KB, SEQUENTIAL, 2);			    // Block size = 1KB, Sequential, 2 thread
		
		//Experiment 8
		memory_benchmarking(1*KB, RANDOM, 2);				    // Block size = 1KB, Random, 2 thread
		
		//Experiment 9
		memory_benchmarking(1*MB, SEQUENTIAL, 1);				// Block size = 1MB, Sequential, 1 thread
		
		//Experiment 10
		memory_benchmarking(1*MB, RANDOM, 1);                  // Block size = 1MB, Random, 1 thread
		
		//Experiment 11
		memory_benchmarking(1*MB, SEQUENTIAL, 2);				// Block size = 1MB, Sequential, 2 thread
		
		//Experiment 12
		memory_benchmarking(1*MB, RANDOM, 2);				    // Block size = 1MB, Random, 2 thread
	}
    return EXIT_SUCCESS;

}
