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
int item_count = 0;
int threadsYielding = 0;

int *full;

sem_t *mutex;
sem_t *empty;
sem_t item;
sem_t item1;
sem_t item1000;
sem_t removed;
sem_t addMore;
sem_t print;
sem_t printDone;
sem_t printStatus;

void *producer(void *id) {
	int i, busy, filled;
	while(sem_trywait(&item1)== 0){
		busy = TRUE;
		filled = TRUE;
		for(i = 0; i < BUFFERCOUNT; i++){
			if(sem_trywait(&mutex[i]) == 0){
				busy = FALSE;
				if(full[i] < 1024){
					filled = FALSE;
					sem_wait(&item);
					full[i] = full[i] + 1;
					item_count = item_count + 1;
					if(item_count % 1000 == 0){
						sem_post(&print);
						sem_wait(&printDone);
					}
					else{
						sem_post(&item);
					}

					sem_post(&empty[i]);
					sem_post(&mutex[i]);
					break;
				}

				sem_post(&mutex[i]);
			}
		}
		if(busy || filled){
			sem_post(&item1);
			if(filled){
				sem_post(&removed);
				sem_wait(&addMore);
			}
		}
	}
	printf("Producer Thread %d is Finished\n", (int) id);
	sem_post(&addMore);
	pthread_exit(0);
}

void *consumer(void *id) {
	int i, emptyBuffer =0, j;
	while(TRUE){
		emptyBuffer = 0;
		for(i = 0; i < BUFFERCOUNT; i++){
			if(sem_trywait(&empty[i]) == 0){
				sem_wait(&mutex[i]);
				full[i] = full[i] - 1;

				sem_post(&mutex[i]);
			}
			else{
				emptyBuffer = emptyBuffer + 1;
			}

			if(emptyBuffer == BUFFERCOUNT){
				if(sem_trywait(&printStatus) == 0){
					printf("Consumer Thread %d has finished.\n", (int) id);
					sem_post(&printStatus);
					sem_post(&removed);
					pthread_exit(0);
				}
				else{
					printf("Consumer Thread %d is yielding.\n", (int) id);
					
					sem_post(&addMore);
					sem_wait(&removed);
				}
			}
		}
	}
}

void *bufferPrinter(void *count){
	int i, j;
	while(TRUE){
		if(sem_trywait(&print) == 0){
			printf("%d item created\n", item_count);
			for(i = 0; i < BUFFERCOUNT; i++){
				printf("SharedBuffer%d has %d items.\n", (i+1), full[i]);
			}
			sem_post(&printDone);
			sem_post(&item);
		}
		if(item_count >= ITEM_MAX){
			sem_post(&printStatus);
			sem_post(&removed);
			pthread_exit(0);
		}
	}
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

	sem_init(&item, 0, 1);
	sem_init(&item1, 0, ITEM_MAX);
	sem_init(&item1000, 0, 999);
	sem_init(&removed, 0, 0);
	sem_init(&addMore, 0, 0);
	sem_init(&print, 0, 0);
	sem_init(&printDone, 0, 0);
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

	pthread_create(&bufferPrinterThread, NULL, (void*)&bufferPrinter, NULL);

	for (i = 0; i < PRODUCERCOUNT; i++) {
		pthread_join(producerThreads[i], NULL);
	}

	// Join consumer threads
	for (i = 0; i < CONSUMERCOUNT; i++) {
		pthread_join(consumerThreads[i], NULL);
	}


	for(i = 0; i < BUFFERCOUNT; i++){
		printf("%d items left in buffer %d\n", full[i], i+1);
	}

	printf("%d items created\n", item_count);

	for (i = 0; i < BUFFERCOUNT; i++) {
		sem_destroy(&mutex[i]);
		sem_destroy(&empty[i]);
	}
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