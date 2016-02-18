/*Compile problem4Producer.c as problem4Producer.out and problem34onsumer.c as problem4Consumer.out*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#include <sys/types.h>
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

void producer(const char* message, int count, FILE* stream)
{
  printf("\nIn writer\n");
  for(; count > 0; --count)
    {
       //Writing to stream (Realize it is sent immediately)
      fprintf(stream, "From writer: %s", message);
      fflush (stream);
      
      //Wait a bit to prevent blocking before  writing out more on pipe
      //sleep(1);
    }
    printf("\nExiting writer\n");
}

void consumer(FILE* stream)
{
  char buffer[1024];
  printf("In reader\n");
   //Read until you hit end of STREAM!fgets reads until either newline or end-of-file
  while(!feof(stream) && !ferror(stream) && fgets(buffer, sizeof(buffer), stream) != NULL){
     //Constantly consumes from buffer reguardless of if empty or not
    fputs(buffer, stdout);
    //printf("In reader in while loop");
  }
  printf("\nExiting reader\n");
}

int main()
{
  int items = 5000;
  int fds[2];     //FILE Descriptors for [read][write] stored in that order

  //Create a pipe File Descriptors for ends are placed in fds
  pipe(fds);


  char itemsString[10];
  char fds0String[10];
  char fds1String[10];
  sprintf(itemsString, "%d", items);
  sprintf(fds0String, "%d", fds[0]);
  sprintf(fds1String, "%d", fds[1]);
  char *producerArgList[] = {"./problem4Producer.out", itemsString, fds0String, fds1String, NULL};
  char *consumerArgList[] = {"./problem4Consumer.out", itemsString, fds0String, fds1String, NULL};
  int producerStatus = spawn("./problem4Producer.out", producerArgList);
  int consumerStatus = spawn("./problem4Consumer.out", consumerArgList);

  wait(&producerStatus);
  wait(&consumerStatus);
  return 0;
}