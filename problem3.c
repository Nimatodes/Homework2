/*Compile problem3Producer.c as problem3Producer.out and problem3Consumer.c as problem3Consumer.out*/
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

int random_range(unsigned const low, unsigned const high)
{
  unsigned const range = high - low + 1;
  return low + (int) (((double)range) * rand() / (RAND_MAX + 1.0));
}

int main(int argc, char* const argv[])
{
  int fd;
  char *file_memory;

  char *fileName = "problem3File.mmap";
  int fileLength = 64;
  int items = 5000;
  key_t key = IPC_PRIVATE;


  //Prepate a file large enough to hold unsigned integer
  fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  lseek (fd, fileLength+1, SEEK_SET);    //make sure file is large enough 
  write (fd, "", 1);
  lseek(fd, 0, SEEK_SET);    //move write pointer back to beginning


  //CREATE MEMORY MAPPING
  file_memory = (char*)mmap(0, fileLength, PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd); //closing file access in HD

  //WRITE STRING TO MEMORY MAPPED FILE (initialize each byte as "empty")
  int i;
  for(i=0; i<fileLength; i++){
    file_memory[i] = 'e';
  }



  //Create semaphores
  int mutex = semaphore_allocation(key, SHM_R | SHM_W);
  int empty = semaphore_allocation(key, SHM_R | SHM_W);
  int full = semaphore_allocation(key, SHM_R | SHM_W);
  semaphore_initalize(mutex, 1);
  semaphore_initalize(empty, fileLength);
  semaphore_initalize(full, 0);

  //convert id's to strings to pass as arguments
  char itemsString[10];
  char fileLengthString[10];
  char mutexString[10];
  char emptyString[10];
  char fullString[10];
  sprintf(itemsString, "%d", items);
  sprintf(fileLengthString, "%d", fileLength);
  sprintf(mutexString, "%d", mutex);
  sprintf(emptyString, "%d", empty);
  sprintf(fullString, "%d", full);
  char *producerArgList[] = {"./problem3Producer.out", itemsString, fileName, fileLengthString, mutexString, emptyString, fullString, NULL};
  char *consumerArgList[] = {"./problem3Consumer.out", itemsString, fileName, fileLengthString, mutexString, emptyString, fullString, NULL};
  int producerStatus = spawn("./problem3Producer.out", producerArgList);
  int consumerStatus = spawn("./problem3Consumer.out", consumerArgList);


  wait(&producerStatus);
  wait(&consumerStatus);
  //DEALLOCATE SEM
  semaphore_deallocate(mutex);
  semaphore_deallocate(empty);
  semaphore_deallocate(full);
  //Release the memory --> memory will automatically be released when 
  munmap (file_memory, fileLength);

  return 0;

}