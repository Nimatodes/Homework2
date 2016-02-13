#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFERSIZE 1024

void *producer(void *b);
void *consumer(void *b);
void produce(int *buffer, int i);
void consume(int *buffer, int i);

pthread_t *producerThreads;
pthread_t *consumerThreads;

int **buffer;

sem_t *mutexes;
sem_t *empty;
sem_t *full;

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

	producerThreads = malloc(producerCount * sizeof(pthread_t));
	for (i = 0; i < producerCount; i++) {
		pthread_create(&producerThreads[i], NULL, &producer, buffer);	
	}

	consumerThreads = malloc(consumerCount * sizeof(pthread_t));
	for (i = 0; i < consumerCount; i++) {
		pthread_create(&consumerThreads[i], NULL, &consumer, buffer);
	}

	free(producerThreads);
	free(consumerThreads);
	free(buffer);
	free(mutexes);
	free(empty);
	free(full);

	return 0;
}
