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
    //Close Read File Descriptor of PIPE
    close (fds[0]);

    //Convert write file descriptor to FILE object, and write to it
    stream = fdopen(fds[1], "w");


	int i=0;
	while(i<items){
		if(i%1000==0)
			printf("%d items produced.\n", i);

		//Writing to stream (Realize it is sent immediately)
   	  	fprintf(stream, "f");
 	  	fflush (stream);
      
     	//Wait a bit to prevent blocking before  writing out more on pipe
      	//sleep(1);
      	i++;
    }
    printf("%d items produced.\n", i);

    close(fds[1]);
    printf("Producer finished.\n");
	return 0;
}

