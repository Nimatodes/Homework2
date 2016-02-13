#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFERSIZE 1024
#define TRUE 1
#define FALSE 0

void *producer(void *b);
void *consumer(void *b);
void produce(int *buffer, int i);
void consume(int *buffer, int i);

pthread_t *producerThreads;
pthread_t *consumerThreads;

int **buffer;

sem_t *mutex;
sem_t *empty;
sem_t *full;
sem_t itemCount;

void *producer(void *b) {
	int item;

	while (TRUE) {
		//item = produce_item();
		//sem_wait(&empty);
		//sem_wait(&mutex);
		//produce(buffer, item);	// insert item in buffer
		//sem_post(&mutex);
		//sem_post(&full);
		//printf("Produced:");

	}
}

void *consumer(void *b) {
	
}

void produce(int *buffer, int i) {
	
}

void consume(int *buffer, int i) {
	
}

int main(int argc, char *argv[]) {
	
	int producerCount, consumerCount, bufferCount, itemCount;
	
	if (argc == 5) {
		producerCount = atoi(argv[1]);
		consumerCount = atoi(argv[2]);
		bufferCount = atoi(argv[3]);
		itemCount = atoi(argv[4]);
	}
	
	int i = 0;

	buffer = malloc(bufferCount * sizeof(int) * BUFFERSIZE);
	mutex = malloc(bufferCount * sizeof(sem_t));
	empty = malloc(bufferCount * sizeof(sem_t));
	full = malloc(bufferCount * sizeof(sem_t));

	for (i = 0; i < bufferCount; i++) {
		sem_init(&mutex[i], 0, 1);
		sem_init(&empty[i], 0, BUFFERSIZE);
		sem_init(&full[i], 0, 0);
	}

	// Create producer threads
	producerThreads = malloc(producerCount * sizeof(pthread_t));
	for (i = 0; i < producerCount; i++) {
		pthread_create(&producerThreads[i], NULL, (void*)&producer, buffer);	
	}

	// Create consumer threads
	consumerThreads = malloc(consumerCount * sizeof(pthread_t));
	for (i = 0; i < consumerCount; i++) {
		pthread_create(&consumerThreads[i], NULL, (void*)&consumer, buffer);
	}

	// Join producer threads
	for (i = 0; i < producerCount; i++) {
		pthread_join(producerThreads[i], NULL);
	}

	// Join consumer threads
	for (i = 0; i < consumerCount; i++) {
		pthread_join(consumerThreads[i], NULL);
	}

	// Destroy semaphores
	for (i = 0; i < bufferCount; i++) {
		sem_destroy(&mutex[i]);
		sem_destroy(&empty[i]);
		sem_destroy(&full[i]);
	}

	free(producerThreads);
	free(consumerThreads);
	free(buffer);
	free(mutex);
	free(empty);
	free(full);

	return 0;
}
