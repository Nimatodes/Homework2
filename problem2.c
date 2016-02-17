/*Compile problem2Producer.c as problem2Producer.out and problem2Consumer.c as problem2Consumer.out*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "sem.h"

int spawn(char *program, char **arg_list){
	pid_t child_pid;//16 bit value ID --> typecast to int(32 bits)

	child_pid = fork();
	if(child_pid != 0)//Parent
		return child_pid;
	else{//Child
		execvp(program, arg_list);//Create a new program withn process space of child

		fprintf(stderr, "An error occured in execvp\n");
		abort();
	}
}

int main(int argc, char const *argv[])
{
    int shared_segment_size = 64;
    int items = 5000;
	key_t key = IPC_PRIVATE;
	//ALLOCATE SHARED MEMORY SEGMENT
	int segment_id = shmget(key, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	//ATTACH SHARED MEMORY SEGMENT
  	char *shared_memory = (char*) shmat (segment_id, 0, 0);
  	//WRITE STRING TO SHARED MEMORY SEGMENT (initialize each byte as "empty")
  	int i;
  	for(i=0; i<shared_segment_size; i++){
  		shared_memory[i] = 'e';
  	}
	//DETACH SHARED MEMORY
  	shmdt(shared_memory);

  	//Create semaphores
  	int mutex = semaphore_allocation(key, SHM_R | SHM_W);
  	int empty = semaphore_allocation(key, SHM_R | SHM_W);
  	int full = semaphore_allocation(key, SHM_R | SHM_W);
  	semaphore_initalize(mutex, 1);
  	semaphore_initalize(empty, shared_segment_size);
  	semaphore_initalize(full, 0);

  	//convert id's to strings to pass as arguments
  	char itemsString[10];
  	char segment_idString[10];
  	char mutexString[10];
  	char emptyString[10];
  	char fullString[10];
  	sprintf(itemsString, "%d", items);
  	sprintf(segment_idString, "%d", segment_id);
  	sprintf(mutexString, "%d", mutex);
  	sprintf(emptyString, "%d", empty);
  	sprintf(fullString, "%d", full);
	char *producerArgList[] = {"./problem2Producer.out", itemsString, segment_idString, mutexString, emptyString, fullString, NULL};
	char *consumerArgList[] = {"./problem2Consumer.out", itemsString, segment_idString, mutexString, emptyString, fullString, NULL};
	int producerStatus = spawn("./problem2Producer.out", producerArgList);
	int consumerStatus = spawn("./problem2Consumer.out", consumerArgList);


	wait(&producerStatus);
	wait(&consumerStatus);
	//DEALLOCATE THE SHARED MEMORY SEGMENT
  	shmctl(segment_id, IPC_RMID, 0);
  	//DEALLOCATE SEM
  	semaphore_deallocate(mutex);
  	semaphore_deallocate(empty);
  	semaphore_deallocate(full);
	return 0;
}