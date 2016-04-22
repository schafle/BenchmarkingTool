#include<stdio.h>
#include "common.h"
#include<unistd.h>
#include<limits.h>

#define SEQUENTIAL 1
#define RANDOM 2

#define READ 1
#define WRITE 2

#define B 1UL
#define KB 1024UL
#define MB 1024*1024UL

#define ALLOCATION 10*MB

#define MAX_THREADS 2
#define MAX_FILE_SIZE 10*1024*1024UL

//Do sequential and random access on the file 

struct arg_struct {
	int thread_id;
    const char* filename;
    long block_size;
};

double throughput[MAX_THREADS];
double latency[MAX_THREADS];
char atoz[] = "abcdefghijklmnopqrstuvwxyz";

//Create a huge file using allocate and initialize
void create_and_fill(const char* filename, long file_size)
{
	char* buffer = malloc(file_size);
	long i, j;
	
	// Open a file for writing. 
	FILE *file = fopen(filename, "w");
	
	//Initializing the allocated space with random char from a to z    
	for(i=0;i<file_size;i++)
	{
		buffer[i] = atoz[rand_interval(0,25)];
	}
	long results = fwrite (buffer, 1, file_size, file);
	if (results != file_size) 
	{
		printf("failed to create a new file\n");
	}
	fclose(file);
	free(buffer);
}
	
//Reading the file sequentially
void sequential_read(void* arguments)
{
	//Unpacking the arguments
	struct arg_struct *args = (struct arg_struct *)arguments;

    //Defining local variables I will be using
    const char* filename = args -> filename;
    long block_size = args -> block_size;
    int thread = args -> thread_id;
    
    //Defining utility variables 
	int i=0;
	int j=0;
	size_t blocks_read=0;
	int iterations = 100;
	FILE *fp;
	char* buffer = malloc(block_size*iterations);
	//Open the file in read mode
	
	//Start clock
	double t = stoptime();
	fp=fopen(filename, "r");
	for(j=0; j < iterations; j++)
	{
		blocks_read += fread (buffer, 1, block_size, fp);  //Reading one block of size blocksize from fp into buffer
		//printf("blocks = %s\n",buffer); 
		//printf("blocks_read = %zu\n",blocks_read);       //Printing the number of block position we have reached to
	}
	
	fclose(fp);
	//Stop clock
    double time_taken = (stoptime() - t);
    
    //printf("blocks_read=%zu\n",blocks_read);
    throughput[thread] = blocks_read/(1024*1024*time_taken);
    //printf("Throughput= %.4f MBPS\n", blocks_read/(1024*1024*time_taken));
    
    //Latency Calculation

    latency[thread] = time_taken/iterations*1000; 			//Time taken to read a block of size blocksize in millisconds
    free(buffer);
}

//Writing to the file sequentially
void sequential_write(void* arguments)
{
	//Unpacking the arguments
	struct arg_struct *args = (struct arg_struct *)arguments;

    //Defining local variables I will be using
    const char* filename = args -> filename;
    long block_size = args -> block_size;
    int thread = args -> thread_id;
    
    //Defining utility variables 
	int i=0;
	int j=0;
	size_t blocks_written=0;
	int iterations = 100;
	FILE *fp;
	char* buffer = malloc(block_size);
	
	//Fill buffer with random characters
	for(i=0;i<block_size;i++)
	{
		buffer[i] = atoz[rand_interval(0,25)];
	}
	//Open the file in read mode
	
	//Start clock
	double t = stoptime();
	fp=fopen(filename, "w+");
	for(j=0; j < iterations; j++)
	{
		blocks_written += fwrite (buffer, 1, block_size, fp);  //Writing one block of size blocksize from fp into buffer
		//printf("blocks_read = %zu\n",blocks_read);       //Printing the number of block position we have reached to
	}
	
	fclose(fp);
	//Stop clock
    double time_taken = (stoptime() - t);
    throughput[thread] = blocks_written/(1024*1024*time_taken);
    //printf("Throughput= %.4f MBPS\n", blocks_written/(1024*1024*time_taken));
    
    //Latency Calculation

    latency[thread] = time_taken/iterations*1000; 			//Time taken to write a block of size blocksize in millisconds
    
    free(buffer);
}

//Reading the file sequentially
void random_read(void* arguments)
{
	//Unpacking the arguments
	struct arg_struct *args = (struct arg_struct *)arguments;

    //Defining local variables I will be using
    const char* filename = args -> filename;
    long block_size = args -> block_size;
    int thread = args -> thread_id;
    
    //Defining utility variables 
	int i=0;
	int j=0;
	size_t blocks_read=0;
	int iterations = 100;
	FILE *fp;
	char* buffer = malloc(block_size*iterations);
	//Open the file in read mode
	
	//Start clock
	double t = stoptime();
	fp=fopen(filename, "r");
	for(j=0; j < iterations; j++)
	{
		fseek(fp, rand_interval(0,iterations), SEEK_SET);
		blocks_read += fread (buffer, 1, block_size, fp);  //Reading one block of size blocksize from fp into buffer
		//printf("blocks = %s\n",buffer);         			//Printing the block just read
		//printf("blocks_read = %zu\n",blocks_read);       //Printing the number of block position we have reached to
	}
	
	fclose(fp);
	//Stop clock
    double time_taken = (stoptime() - t);
    
    //printf("blocks_read=%zu\n",blocks_read);
    throughput[thread] = blocks_read/(1024*1024*time_taken);
    //printf("Throughput= %.4f MBPS\n", blocks_read/(1024*1024*time_taken));
    
    //Latency Calculation
   
    
    //printf("blocks_read=%zu\n",blocks_read);
    latency[thread] = time_taken/iterations*1000; 			//Time taken to read a block of size blocksize in millisconds
	 
	 free(buffer);
}

//Writing to the file randomly
void random_write(void* arguments)
{
	//Unpacking the arguments
	struct arg_struct *args = (struct arg_struct *)arguments;

    //Defining local variables I will be using
    const char* filename = args -> filename;
    long block_size = args -> block_size;
    int thread = args -> thread_id;
    
    //Defining utility variables 
	int i=0;
	int j=0;
	size_t blocks_written=0;
	int iterations = 100;
	FILE *fp;
	char* buffer = malloc(block_size);
	
	//Fill buffer with random characters
	for(i=0;i<block_size;i++)
	{
		buffer[i] = atoz[rand_interval(0,25)];
	}
	//Start clock
	double t = stoptime();
	//Open the file in write mode
	fp=fopen(filename, "w+");
	for(j=0; j < iterations; j++)
	{
		fseek(fp, rand_interval(0,iterations), SEEK_SET);
		blocks_written += fwrite (buffer, 1, block_size, fp);  	 //Writing one block of size blocksize from fp into buffer
		//printf("blocks_read = %zu\n",blocks_read);      		 //Printing the number of block position we have reached to
	}
	fclose(fp);
	//Stop clock
    double time_taken = (stoptime() - t);
    throughput[thread] = blocks_written/(1024*1024*time_taken);
    //printf("Throughput= %.4f MBPS\n", blocks_written/(1024*1024*time_taken));
    
    //Latency calculation
    latency[thread] = time_taken/iterations*1000; 			//Time taken to write a block of size blocksize in millisconds

	 free(buffer);
}
void disk_benchmarking(long memory_size, char operation, long block_size, char access_type, int num_threads)
{
	const char* filename = "example";
	create_and_fill(filename,MAX_FILE_SIZE*10);
	//This basically decides how many bytes are going to be read/write with memcpy	
	if(access_type == SEQUENTIAL)
	{
		int i;
		pthread_t thread_id[num_threads];
		for (i = 0; i < num_threads; ++i ) 
		{
			//struct arg_struct args;
			
			struct arg_struct* args = malloc(sizeof(struct arg_struct));
			if ( args == NULL ) 
			{
				fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
				exit(EXIT_FAILURE);
			}
			args->filename = filename;
			args->block_size = block_size;
			args->thread_id = i;
			if(operation == READ)
			{
				pthread_create(&thread_id[i], NULL, &sequential_read, args);
			}
			else if (operation == WRITE)
			{
				pthread_create(&thread_id[i], NULL, &sequential_write, args);	
			}
		}
		int j;
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
			//struct arg_struct args;
			
			struct arg_struct* args = malloc(sizeof(struct arg_struct));
			if ( args == NULL ) 
			{
				fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
				exit(EXIT_FAILURE);
			}
			args->filename = filename;
			args->block_size = block_size;
			args->thread_id = i;
			if(operation == READ)
			{
				pthread_create(&thread_id[i], NULL, &random_read, args);
			}
			else if (operation == WRITE)
			{
				pthread_create(&thread_id[i], NULL, &random_write, args);	
			}
		}
		int j;
		for(j=0; j<num_threads; j++)
		{
			pthread_join( thread_id[j], NULL);
		}
	}
	double total_time_taken = 0.0;
	double combined_throughput = 0;
	int j;
	for(j=0; j<num_threads; j++)
	{
		total_time_taken += latency[j];
		combined_throughput += throughput[j];
	}
	printf("%s\t",access_type==1?"Sequential":"  Random  ");
	if(block_size == B)
		printf("  %s\t"," B");
	else if(block_size == KB)
		printf("  %s\t","KB");
	else if(block_size == MB)
		printf("  %s\t","MB");
	printf("   \t%s\t",operation==1?" READ":"WRITE");
	printf("   \t%d\t",num_threads);
	printf("\t%10.5f\t",combined_throughput/num_threads);
	printf("   %.13f\t\n",total_time_taken/num_threads);
}

int main(int argc, char *argv[]) 
{
   if (argc < 2) 
   {
      printf("Please provide some arguments\n");
      printf("Use -h/--help for reference\n");
      exit(EXIT_FAILURE);
   }
   
   //Defining the values which will be passed to benchmarking function
   long memory_size;
   char operation = READ;
   long block_size = KB;
   char access = SEQUENTIAL;
   int num_threads = 1;
   int all = 0;  // If ALL is zero; we will run a single experiment; with non zero value we will 
   int i;
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
		else if(0 == strcmp(argv[i], "--operation"))
		{
			if(0 == strcmp(argv[i+1], "read"))
			{
				operation = READ;
			}
			else if(0 == strcmp(argv[i+1], "write"))
			{
				operation = WRITE;
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
			printf("\t./disk\n");
			printf("\t\t --blocksize B/KB/MB\n");
			printf("\t\t --access random/sequential\n");
			printf("\t\t --operation read/write\n");
			printf("\t\t --numthreads 1/2/4/n\n");
			printf("\t\t --default --> Run all test cases\n");
			exit(EXIT_SUCCESS);
		}
		else
		{
			printf("Use of memory benchmarking executable:\n");
			printf("\t./disk\n");
			printf("\t\t --blocksize B/KB/MB\n");
			printf("\t\t --access random/sequential\n");
			printf("\t\t --operation read/write\n");
			printf("\t\t --numthreads 1/2/4/n\n");
			printf("\t\t --default --> Run all test cases\n");
			exit(EXIT_FAILURE);
		}
	}
	if(all == 0)
	{
		printf("--------------------------------------------------------------------------------------------------------\n");
		printf("Access type\t Blocksize\t Operation\t Num-Threads\t Throughput\t Latency\n");
		printf("                                                                 MB/s             ms       \n");
		printf("--------------------------------------------------------------------------------------------------------\n");
		//memory_benchmarking(memory_size, block_size, access, num_threads);
		disk_benchmarking(block_size*100, operation, block_size, access, num_threads);
		printf("\n--------------------------------------------------------------------------------------------------------\n");
	}
	else
	{
		printf("--------------------------------------------------------------------------------------------------------\n");
		printf("Access type\t Blocksize\t Operation\t Num-Threads\t Throughput\t Latency\n");
		printf("                                                                 MB/s             ms       \n");
		printf("--------------------------------------------------------------------------------------------------------\n");
		//Experiment 1
		disk_benchmarking(memory_size, READ, 1*B, SEQUENTIAL, 1);  				// Block size = 1B, Sequential, 1 thread'
		
		//Experiment 2
		disk_benchmarking(memory_size, READ, 1*B, RANDOM, 1);					// Block size = 1B, Random, 1 thread
		
		//Experiment 3
		disk_benchmarking(memory_size, READ, 1*B, SEQUENTIAL, 2);				// Block size = 1B, Sequential, 2 thread
		
		//Experiment 4
		disk_benchmarking(memory_size, READ, 1*B, RANDOM, 2);				    // Block size = 1B, Random, 2 thread
		
		//Experiment 5
		disk_benchmarking(memory_size, READ, 1*KB, SEQUENTIAL, 1);				// Block size = 1KB, Sequential, 1 thread
		
		//Experiment 6
		disk_benchmarking(memory_size, READ, 1*KB, RANDOM, 1);					// Block size = 1KB, Random, 1 thread
		
		//Experiment 7
		disk_benchmarking(memory_size, READ, 1*KB, SEQUENTIAL, 2);			    // Block size = 1KB, Sequential, 2 thread
		
		//Experiment 8
		disk_benchmarking(memory_size, READ, 1*KB, RANDOM, 2);				    // Block size = 1KB, Random, 2 thread
		
		//Experiment 9
		disk_benchmarking(memory_size, READ, 1*MB, SEQUENTIAL, 1);				// Block size = 1MB, Sequential, 1 thread
		
		//Experiment 10
		disk_benchmarking(memory_size, READ, 1*MB, RANDOM, 1);                    // Block size = 1MB, Random, 1 thread
		
		//Experiment 11
		disk_benchmarking(memory_size, READ, 1*MB, SEQUENTIAL, 2);				// Block size = 1MB, Sequential, 2 thread
		
		//Experiment 12
		disk_benchmarking(memory_size, READ, 1*MB, RANDOM, 2);				    // Block size = 1MB, Random, 2 thread
		
		//Experiment 13
		disk_benchmarking(memory_size, WRITE, 1*B, SEQUENTIAL, 1);  				// Block size = 1B, Sequential, 1 thread
		
		//Experiment 14
		disk_benchmarking(memory_size,  WRITE, 1*B, RANDOM, 1);					// Block size = 1B, Random, 1 thread
		
		//Experiment 15
		disk_benchmarking(memory_size, WRITE, 1*B, SEQUENTIAL, 2);				// Block size = 1B, Sequential, 2 thread
		
		//Experiment 16
		disk_benchmarking(memory_size, WRITE, 1*B, RANDOM, 2);				    // Block size = 1B, Random, 2 thread
		
		//Experiment 17
		disk_benchmarking(memory_size, WRITE, 1*KB, SEQUENTIAL, 1);				// Block size = 1KB, Sequential, 1 thread
		
		//Experiment 18
		disk_benchmarking(memory_size, WRITE, 1*KB, RANDOM, 1);					// Block size = 1KB, Random, 1 thread
		
		//Experiment 19
		disk_benchmarking(memory_size, WRITE, 1*KB, SEQUENTIAL, 2);			    // Block size = 1KB, Sequential, 2 thread
		
		//Experiment 20
		disk_benchmarking(memory_size, WRITE, 1*KB, RANDOM, 2);				    // Block size = 1KB, Random, 2 thread
		
		//Experiment 21
		disk_benchmarking(memory_size, WRITE, 1*MB, SEQUENTIAL, 1);				// Block size = 1MB, Sequential, 1 thread
		
		//Experiment 22
		disk_benchmarking(memory_size, WRITE, 1*MB, RANDOM, 1);                  // Block size = 1MB, Random, 1 thread
		
		//Experiment 23
		disk_benchmarking(memory_size, WRITE, 1*MB, SEQUENTIAL, 2);				// Block size = 1MB, Sequential, 2 thread
		
		//Experiment 24
		disk_benchmarking(memory_size, WRITE, 1*MB, RANDOM, 2);				    // Block size = 1MB, Random, 2 thread
		printf("\n--------------------------------------------------------------------------------------------------------\n");
	}
    return EXIT_SUCCESS;
}
