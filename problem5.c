#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>



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


int main(int argc, char* const argv[]) {

	int items = 5000;

	char itemsString[10];
  	sprintf(itemsString, "%d", items);
  	char *producerArgList[] = {"./problem5Producer.out", itemsString, NULL};
  	char *consumerArgList[] = {"./problem5Consumer.out", itemsString, NULL};
  	int producerStatus = spawn("./problem5Producer.out", producerArgList);
  	int consumerStatus = spawn("./problem5Consumer.out", consumerArgList);

  	wait(&producerStatus);
 	wait(&consumerStatus);
	return 0;
}