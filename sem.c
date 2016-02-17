/*
 Functions needed for SEMAPHORE V 
*/
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include "sem.h"


//Semaphore Union 
union semun { 
  int val;
  struct semid_ds *buf;
  unsigned short int *array;
  struct seminfo *__buf;
};

//Obtain Semaphore ID, Allocate if necessary
int semaphore_allocation(key_t key, int sem_flags)
{
  return semget(key, 1, sem_flags);
}

//Deallocate Semaphore,  "return -1 if fail"
int semaphore_deallocate(int semid)
{
  union semun ignored_argument;
  return semctl(semid,1,IPC_RMID, ignored_argument);
}

//INIT Semaphore with Value 
int semaphore_initalize(int semid, int value)
{
  union semun argument;
  unsigned short values[1];
  values[0]=value;
  argument.array=values;
  return semctl(semid,0,SETALL,argument);
}

/*******************************************
Wait and Post  --> Down and Up 
*******************************************/

//WAIT
int semaphore_wait(int semid)
{
  struct sembuf operations[1];

  //Use first semaphore --> Only one allocated at this point
  operations[0].sem_num=0;

  //Decrement semaphore by 1
  operations[0].sem_op = -1; 

  //DONT Permit undo  --> If process using sem terminates allows undoes any operation it made on the semaphore
  operations[0].sem_flg = 0;

  return semop(semid, operations, 1);
}


//POST
int semaphore_post(int semid)
{
  struct sembuf operations[1];

//Use first semaphore --> Only one allocated at this point
  operations[0].sem_num=0;

  //Increment semaphore by 1
  operations[0].sem_op = 1; 

  //DONT Permit undo
  operations[0].sem_flg = 0;

  return semop(semid, operations, 1);
}