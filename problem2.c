#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>

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
	

	key_t key = 1531;
    int shared_segment_size = 1024;
	//ALLOCATE SHARED MEMORY SEGMENT
	int segment_id = shmget(key, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	//ATTACH SHARED MEMORY SEGMENT
  	char *shared_memory = (char*) shmat (segment_id, 0, 0);
  	//WRITE STRING TO SHARED MEMORY SEGMENT (set each byte as "empty")
  	int i;
  	for(i=0; i<shared_segment_size; i = i+1){
  		shared_memory[i] = 'e';
  	}
	//DETACH SHARED MEMORY
  	shmdt(shared_memory);



	char *producerArgList[] = {"./problem2Producer.out", "1531", "1024", NULL};
	char *consumerArgList[] = {"./problem2Consumer.out", "1531", "1024", NULL};
	int producerStatus = spawn("./problem2Producer.out", producerArgList);
	int consumerStatus = spawn("./problem2Consumer.out", consumerArgList);

	wait(&producerStatus);
	wait(&consumerStatus);
	//DEALLOCATE THE SHARED MEMORY SEGMENT
  	shmctl(segment_id, IPC_RMID, 0);
	return 0;
}