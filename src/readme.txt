@Author: Suraj Chafle
@Purpose: Explaining instructions to build and run the code for benchmarking

-------------------------------------------------------------------------------------
Pre-requisite
-------------------------------------------------------------------------------------
1. gcc version 4.8.4
2. GNUPLOT Version 4.6 (If you want to plot graphs from the files generated)ss

-------------------------------------------------------------------------------------
To compile, build and execute every test case automatically
-------------------------------------------------------------------------------------
The simplest way to do build and test everything automatically is to run run.sh script.
This will compile, build and execute all the binaries with default test cases
	./run.sh

-------------------------------------------------------------------------------------
To run each benchmarking binary with all test cases:
-------------------------------------------------------------------------------------
1. CPU:
   Pass "--cpu" as a parameter to run.sh for 1/2/4 threads and 600 samples
	./run.sh --cpu 

2. Memory:
   Pass "--memory" as a parameter to run.sh for 12 experiments
	./run.sh --memory
	
3. Disk:
   Pass "--disk" as a parameter to run.sh for 24 experiments
	./run.sh --disk
   
-------------------------------------------------------------------------------------
To Build and run particular test cases for each benchmarking experiment
-------------------------------------------------------------------------------------

1. CPU: 
	
	Compile cpu binary with this command
	
		gcc -o cpu cpu.c common.h -lpthread -lm
	
	Running ./cpu
	Use "--h" to know all the parameters you can pass to "cpu" executable
		./cpu --h

	To run sampling experiment
		./cpu --sampling 600 		//This will take 600 sampling seconds

	To run cpu benchmarking with different threads
		./cpu --numthreads 2		//This will run GFLOPS/IOPS calculation with 2 threads
		
2. Memory: 
   Compile memory binary with this command
	
		gcc -g -o memory memory.c common.h -lpthread
	
	Running ./memory
	Use "--h" to know all the parameters you can pass to "memory" executable
		./memory --h

	To run all 24 experiments
		./memory --default	 		//This will run all the experiments one by ones

	To run memory benchmarking with specific test case
		./memory --blocksize B --numthreads 1 --access sequential
									//This will run the benchmarking experiment with seuential access, 1 thread and 1B blocksize
		
		./memory --blocksize MB --numthreads 4 --access random
									//This will run the benchmarking experiment with random access, 4 threads and 1MB blocksize
		
		You can get all the parameters to be passed with "-h" argument.
		
		
3. Disk: 
   Compile disk binary with this command
	
		gcc -o disk disk.c common.h -lpthread
	
	Running ./disk
	Use "--h" to know all the parameters you can pass to "disk" executable
		./disk --h

	To run all 24 experiments
		./disk --default	 		//This will run all the experiments one by ones

	To run memory benchmarking with specific test case
		./disk --blocksize B --numthreads 1 --access sequential --operation write
									//This will run the benchmarking experiment with seuential write, 1 thread and 1B blocksize
		
		./disk --blocksize MB --numthreads 4 --access random --operation read
									//This will run the benchmarking experiment with random read, 4 threads and 1MB blocksize
		
		You can get all the parameters to be passed with "-h" argument.

