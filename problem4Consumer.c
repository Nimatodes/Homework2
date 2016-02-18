#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main(int argc, char const *argv[])
{
	if(argc != 4){
        printf("Error, incorrect number of arguments\n");
        return -1;
    }
    int items = atoi(argv[1]);
    int fds[2];
    fds[0] = atoi(argv[2]);
    fds[1] = atoi(argv[3]);

	FILE* stream;
  	//Close Write File Descriptor of PIPE
  	close (fds[1]);
      
  	//Convert read file descriptor to FILE object, and read from it
   	stream = fdopen(fds[0], "r");


    char buffer;
 	int i=0;
  	while(i<items){
    	buffer = getc(stream);//read char from pipe
    	if(buffer!='f'){
    		printf("Consumer error at index: %d\n", i);
			exit(1);
    	} else{
    		//Consume (do nothing)
    	}
    	i++;
  	}
  	printf("%d items consumed.\n", i);
    close(fds[0]);
  	printf("Consumer finished.\n");
	return 0;
}