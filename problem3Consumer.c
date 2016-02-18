#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "sem.h"

void consume(char *buffer, int i);

int main(int argc, char const *argv[])
{
	if(argc != 7){
        printf("Error, incorrect number of arguments\n");
        return -1;
    }
    int items = atoi(argv[1]);
	char const *fileName = argv[2];
	int fileLength = atoi(argv[3]);
	int mutex = atoi(argv[4]);
	int empty = atoi(argv[5]);
	int full = atoi(argv[6]);

   	//Open file
  	int fd = open(fileName, O_RDWR, S_IRUSR | S_IWUSR);
  	lseek (fd, fileLength+1, SEEK_SET);    //make sure file is large enough 
  	write (fd, "", 1);
  	lseek(fd, 0, SEEK_SET);    //move write pointer back to beginning

  	//CREATE MEMORY MAPPING
  	char *file_memory = (char*)mmap(0, fileLength, PROT_WRITE, MAP_SHARED, fd, 0);
  	close(fd); //closing file access in HD


    int i=0;
	while(i<items) {
		semaphore_wait(full);	//down full, decrement it
		semaphore_wait(mutex);	//down mutex, enter critical region
		consume(file_memory, i%fileLength);	//consume item from buffer
		semaphore_post(mutex);	//up mutex, exit critical region
		semaphore_post(empty);	//up empty, increment it
		i++;
	}
	semaphore_wait(mutex);	//down mutex, enter critical region
	printf("%d items consumed. Buffer contents:\n", i);
	printf("%s\n", file_memory);
	semaphore_post(mutex);	//up mutex, exit critical region
	//Release the memory
  	munmap (file_memory, fileLength);
	printf("Consumer finished.\n");
	return 0;
}

void consume(char *buffer, int i) {
	if(buffer[i] == 'f') {
			buffer[i] = 'e'; 	//consume item from buffer
	}
	else {			
		printf("Consumer error at index: %d\n", i);
		exit(1);
	}
}