#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
 
#define BUFFERSIZE 1024
#define TRUE 1
#define FALSE 0
 
void *producer(void *b);
void *consumer(void *b);
void *bufferPrinter(void *b);
 
pthread_t *producerThreads;
pthread_t *consumerThreads;
pthread_t bufferPrinterThread;
 
int PRODUCERCOUNT;
int CONSUMERCOUNT;
int BUFFERCOUNT;
int ITEM_MAX;
int item_count;
 
int *full;
 
sem_t *mutex;
sem_t *empty;
sem_t item;
sem_t item1000;
sem_t print;
sem_t printStatus;
 
void *producer(void *id) {
    int i;
    int count;
    sem_getvalue(&item, &count);
 
    while(count!=0){
        sem_wait(&item);
        for(i = 0; i < BUFFERCOUNT; i++){
            sem_wait(&mutex[i]);
            if(full[i] < 1024){
 
                sem_wait(&item1000);
                full[i] = full[i] + 1;
                item_count = item_count + 1;
                if(item_count % 1000 == 0){
                    sem_wait(&print);
                    pthread_create(&bufferPrinterThread, NULL, (void*)&bufferPrinter, (void *) item_count);
                    pthread_join(bufferPrinterThread, NULL);
                }
 
                sem_post(&empty[i]);
                sem_post(&mutex[i]);
                break;
            }
            sem_post(&mutex[i]);
        }
        sem_getvalue(&item, &count);
    }
    printf("Producer Thread %d is Finished\n", (int) id);
    sem_post(&printStatus);
    pthread_exit(0);
}
 
void *consumer(void *id) {
    int i, status, emptyBuffer =0;
    while(TRUE){
        emptyBuffer = 0;
        for(i = 0; i < BUFFERCOUNT; i++){
            status = sem_trywait(&empty[i]);
            if(status == 0){
                sem_wait(&mutex[i]);
                full[i] = full[i] - 1;
                sem_post(&mutex[i]);
            }
            else{
                emptyBuffer = emptyBuffer+1;
                if(emptyBuffer == BUFFERCOUNT){
                    status = sem_trywait(&printStatus);
                    if(status == 0){
                        printf("Consumer Thread %d has finished.\n", (int) id);
                        sem_post(&printStatus);
                        pthread_exit(0);
                    }
                    else{
                        printf("Consumer Thread %d is yielding.\n", (int) id);
                    }
                }
            }
        }
    }
}
 
void *bufferPrinter(void *count){
    int i;
    printf("%d item created\n", (int)count);
    for(i = 0; i < BUFFERCOUNT; i++){
        printf("SharedBuffer%d has %d items.\n", (i+1), full[i]);
    }
    for(i = 0; i < 1000; i++){
        sem_post(&item1000);
    }
    sem_post(&print);
    pthread_exit(0);
}
 
int main(int argc, char *argv[]) {
   
    if (argc == 5) {
        PRODUCERCOUNT = atoi(argv[1]);
        CONSUMERCOUNT = atoi(argv[2]);
        BUFFERCOUNT = atoi(argv[3]);
        ITEM_MAX = atoi(argv[4]);
    }
 
    int i =0, j;
 
    mutex = malloc(BUFFERCOUNT * sizeof(sem_t));
    empty = malloc(BUFFERCOUNT * sizeof(sem_t));
    full = malloc(BUFFERCOUNT * sizeof(int));
 
    for (i = 0; i < BUFFERCOUNT; i++) {
        sem_init(&mutex[i], 0, 1);
        sem_init(&empty[i], 0, 0);
    }
 
    sem_init(&item, 0, ITEM_MAX);
    sem_init(&item1000, 1, 1000);
    sem_init(&print, 0, 1);
    sem_init(&printStatus, 0,0);
 
 
 
    producerThreads = malloc(PRODUCERCOUNT * sizeof(pthread_t));
    for (i = 0; i < PRODUCERCOUNT; i++) {
        j = i +1;
        pthread_create(&producerThreads[i], NULL, (void*)&producer, (void *) j);   
    }
 
    consumerThreads = malloc(CONSUMERCOUNT * sizeof(pthread_t));
    for (i = 0; i < CONSUMERCOUNT; i++) {
        j = i+1;
        pthread_create(&consumerThreads[i], NULL, (void*)&consumer, (void *) j);
    }
 
    // Join consumer threads
    for (i = 0; i < CONSUMERCOUNT; i++) {
        pthread_join(consumerThreads[i], NULL);
    }
    for(i = 0; i < BUFFERCOUNT; i++){
        printf("%d items left in buffer %d\n", full[i], i+1);
    }
 
 
    for (i = 0; i < BUFFERCOUNT; i++) {
        sem_destroy(&mutex[i]);
        sem_destroy(&empty[i]);
    }
 
    sem_destroy(&item);
    sem_destroy(&item1000);
    sem_destroy(&print);
    sem_destroy(&printStatus);
 
    free(producerThreads);
    free(consumerThreads);
    free(mutex);
    free(empty);
    free(full);
 
    return 0;
}