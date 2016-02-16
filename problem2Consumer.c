#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main(int argc, char const *argv[])
{
	if(argc != 3){
        printf("Error, incorrect number of arguments\n");
        return -1;
    }
	key_t key = atoi(argv[1]);
	int shared_segment_size = atoi(argv[2]);
	printf("Consumer: %i: %i\n", key, shared_segment_size);

	int segment_id;
	if ((segment_id = shmget(key, shared_segment_size, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    char *shared_memory;
    if ((shared_memory = shmat(segment_id, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    //PRINT OUT STRING FROM SHARED MEMORY
  	printf("Consumer: %s\n", shared_memory);
	return 0;
}