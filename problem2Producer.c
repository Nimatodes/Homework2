#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "sem.h"

void produce(char *buffer, int i);

int main(int argc, char const *argv[])
{
	if(argc != 6){
        printf("Error, incorrect number of arguments\n");
        return -1;
    }

    int items = atoi(argv[1]);
	int segment_id = atoi(argv[2]);
	int mutex = atoi(argv[3]);
	int empty = atoi(argv[4]);
	int full = atoi(argv[5]);
	//DETERMINE SHARED MEMORY SEGMENT SIZE
	struct shmid_ds shmbuffer;
	shmctl(segment_id, IPC_STAT, &shmbuffer);
 	int shared_segment_size = shmbuffer.shm_segsz;

    char *shared_memory;
    if ((shared_memory = shmat(segment_id, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }


    int i=0;
	while(i<items) {
		if(i%1000==0){
			semaphore_wait(mutex);	//down mutex, enter critical region
			printf("%d items produced. Buffer contents:\n", i);
			printf("%s\n", shared_memory);
			semaphore_post(mutex);	//up mutex, exit critical region
		}
		semaphore_wait(empty);	//down empty, decrement it
		semaphore_wait(mutex);	//down mutex, enter critical region
		produce(shared_memory, i%shared_segment_size);	//insert item to buffer
		semaphore_post(mutex);	//up mutex, exit critical region
		semaphore_post(full);	//up full, increment it
		i++;
	}
	semaphore_wait(mutex);	//down mutex, enter critical region
	printf("%d items produced. Buffer contents:\n", i);
	printf("%s\n", shared_memory);
	semaphore_post(mutex);	//up mutex, exit critical region

	//DETACH SHARED MEMORY
  	shmdt(shared_memory);
	printf("Producer finished.\n");
	return 0;
}

void produce(char *buffer, int i) {
	if(buffer[i] == 'e') {
			buffer[i] = 'f'; 	//insert item to buffer
	}
	else {			
		printf("Producer error at index: %d\n", i);
		exit(1);
	}
}