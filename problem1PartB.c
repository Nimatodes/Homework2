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

int *full;

pthread_mutex_t *mutex;			//declare mutex mutex
pthread_mutex_t item;
pthread_mutex_t itemCount;		//declare mutex itemCount
pthread_mutex_t printStatus;
pthread_mutex_t printerDone;
pthread_cond_t condProducer;	//declare condition condProducer
pthread_cond_t condConsumer;	//declare condition condConsumer

void *producer(void *id) {
	int i, busy, filled;
	while(TRUE){
		busy = TRUE;
		filled = TRUE;
		pthread_mutex_lock(&item);
		if(ITEM_MAX > 0){
			pthread_mutex_unlock(&item);
			for(i = 0; i < BUFFERCOUNT; i++){
				if(pthread_mutex_trylock(&mutex[i]) == 0){
					if(full[i] < 1024){
						filled = FALSE;
						pthread_mutex_lock(&item);
						full[i] = full[i]+1;
						item_count = item_count+1;
						ITEM_MAX--;
						if(item_count % 1000 == 0){
							pthread_mutex_unlock(&printStatus);
						}
						else{
							pthread_mutex_unlock(&item);
						}
						pthread_cond_broadcast(&condConsumer);
						pthread_mutex_unlock(&mutex[i]);
						break;
					}
					pthread_mutex_unlock(&mutex[i]);
				}
			}
			if(filled){
				pthread_cond_broadcast(&condConsumer);
				pthread_cond_wait(&condProducer, &itemCount);
				pthread_mutex_unlock(&itemCount);
			}
		}
		else{
			pthread_mutex_unlock(&item);
			printf("Producer Thread %d is Finished\n", (int) id);
			ITEM_MAX--;
			pthread_cond_signal(&condProducer);
			pthread_exit(0);
		}
	}
	
}

void *consumer(void *id){
	int i, j, emptyBuffer;

	while(TRUE){
		emptyBuffer = 0;
		for(i = 0; i < BUFFERCOUNT; i++){
			if(pthread_mutex_trylock(&mutex[i]) == 0){
				if(full[i] > 0){
					full[i] = full[i] - 1;
				}
				else{
					emptyBuffer = emptyBuffer +1;
				}
				pthread_mutex_unlock(&mutex[i]);
			}
			if(emptyBuffer == BUFFERCOUNT){
				if(pthread_mutex_trylock(&printerDone) == 0){
					pthread_mutex_unlock(&printerDone);
					pthread_cond_broadcast(&condConsumer);
					printf("Consumer Thread %d has finished.\n", (int) id);
					pthread_exit(0);
				}
				else{
					pthread_cond_broadcast(&condProducer);
					printf("Consumer Thread %d is yielding.\n", (int) id);
					pthread_cond_wait(&condConsumer, &itemCount);
					pthread_mutex_unlock(&itemCount);
				}
			}
		}
	}
}

void *bufferPrinter(void *count){
	int i, j;
	while(TRUE){
		if(pthread_mutex_trylock(&printStatus) == 0){
			printf("%d items created\n", item_count);
			for(i = 0; i < BUFFERCOUNT; i++){
				printf("SharedBuffer %d has %d items.\n", i+1, full[i]);
			}
			pthread_mutex_unlock(&item);
		}
		if(ITEM_MAX < 0){

			pthread_mutex_unlock(&printerDone);
			pthread_cond_signal(&condConsumer);
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

	mutex = malloc(BUFFERCOUNT * sizeof(pthread_mutex_t));
	//empty = malloc(BUFFERCOUNT * sizeof(pthread_mutex_t));

	full = malloc(BUFFERCOUNT * sizeof(int));



	for (i = 0; i < BUFFERCOUNT; i++) {
		pthread_mutex_init(&mutex[i], NULL);		//initialize mutex
		//pthread_mutex_init(&empty[i], NULL);
	}

	pthread_mutex_init(&item, NULL);
	pthread_mutex_init(&printStatus, NULL);
	pthread_mutex_init(&printerDone, NULL);

	pthread_mutex_lock(&printStatus);
	pthread_mutex_lock(&printerDone);

	pthread_cond_init(&condProducer, NULL);	//initialize condProducer
	pthread_cond_init(&condConsumer, NULL);	//initialize condConsumer

	producerThreads = malloc(PRODUCERCOUNT * sizeof(pthread_t));
	for (i = 0; i < PRODUCERCOUNT; i++) {
		j = i +1;
		pthread_create(&producerThreads[i], NULL, (void*)&producer, (void *) j);	
	}

	pthread_create(&bufferPrinterThread, NULL, (void*)&bufferPrinter, NULL);

	consumerThreads = malloc(CONSUMERCOUNT * sizeof(pthread_t));
	for (i = 0; i < CONSUMERCOUNT; i++) {
		j = i+1;
		pthread_create(&consumerThreads[i], NULL, (void*)&consumer, (void *) j);
	}

	// Join consumer threads
	for (i = 0; i < PRODUCERCOUNT; i++) {
		pthread_join(producerThreads[i], NULL);
	}

	for(i = 0; i < CONSUMERCOUNT; i++){
		pthread_join(consumerThreads[i], NULL);
	}

	for(i = 0; i < BUFFERCOUNT; i++){
		printf("%d items left in buffer %d\n", full[i], i+1);
	}
	printf("%d items created\n", item_count);


	pthread_cond_destroy(&condProducer);	//destroy condProducer
	pthread_cond_destroy(&condConsumer);	//destroy condConsumer

	for(i = 0; i < BUFFERCOUNT; i++){
		pthread_mutex_destroy(&mutex[i]);
	}
	pthread_mutex_destroy(&item);
	pthread_mutex_destroy(&itemCount);
	pthread_mutex_destroy(&printStatus);
	pthread_mutex_destroy(&printerDone);

	free(producerThreads);
	free(consumerThreads);
	free(mutex);
	free(full);

	return 0;
}