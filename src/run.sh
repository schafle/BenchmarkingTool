declare -i cpu=0;
declare -i memory=0;
declare -i disk=0;
c="cpu";
m="memory";
d="disk";

if [ $# -eq 0 ]; then 
    cpu=1;
    memory=1;
    disk=1;
    echo -e "Let me build some binaries for you\n";
else
	if [ "$1" = "$c" ]; then cpu=1; echo $1;
	elif [ "$1" = "$m" ]; then memory=1; echo $1;
	elif [ "$1" = "$d" ]; then disk=1; echo $1;
	else echo -e "Usage:\n./run.sh cpu\n \t memory\n \t disk\n";fi;
fi;


if [ $cpu -eq 1 ]; then 
	
    echo -e "building cpu benchmarking tool....\n";

	gcc -o cpu cpu.c common.h -lpthread -lm

	echo -e "building cpu benchmarking tool done.\n";
fi;


if [ $memory -eq 1 ]; then 
	
	echo -e "building memory benchmarking tool....\n"

	gcc -o memory memory.c common.h -lpthread

	echo -e "memory benchmarking tool done.\n"

fi;


if [ $disk -eq 1 ]; then 
	echo -e "building disk benchmarking tool....\n"

	gcc -o disk disk.c common.h -lpthread

	echo -e "Disk benchmarking tool done.\n"
fi;


if [ $cpu -eq 1 ]; then 
	
	echo  -e "Running test cases for cpu\n"
	
	./cpu --default 1

fi;

if [ $memory -eq 1 ]; then 

	echo -e "Running memory test cases.\n"

	./memory --default

fi;

if [ $disk -eq 1 ]; then
	
	echo -e "Running test cases for disk.\n"

	./disk --default
	
	rm -r example
fi;

exit 0
