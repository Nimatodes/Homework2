#ifndef SEM_H_   /* Include guard */
#define SEM_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>



//Semaphore Union 
union semun;

//Obtain Semaphore ID, Allocate if necessary
int semaphore_allocation(key_t key, int sem_flags);

//Deallocate Semaphore,  "return -1 if fail"
int semaphore_deallocate(int semid);
//INIT Semaphore with Value 1
int semaphore_initalize(int semid, int value);

/*******************************************
Wait and Post  --> Down and Up 
*******************************************/

//WAIT
int semaphore_wait(int semid);


//POST
int semaphore_post(int semid);

#endif // SEM_H_