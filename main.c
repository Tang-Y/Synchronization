#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// variable delaration
#define BUFFER_SIZE 5 // constant

typedef int buffer_item;
buffer_item buffer[BUFFER_SIZE];

// mutex locks
pthread_mutex_t mutex;
sem_t empty, full;
int inIndex = 0, outIndex = 0;

// Functions delaration
void *producer(void *param);
void *consumer(void *param);
void insert_item(buffer_item item, int producerID);
buffer_item remove_item(int consumerID);

/*
Initializes the buffer and creates the producer/consumer threads. Once it has created the producer and consumer threads, the
thread corresponding to the main() function will sleep for a period of time and, upon
awakening, will terminate the C program
*/
int main(int argc, char *argv[]) {
    // The input should contains four argumanets which includes name of executable file, 
    // sleep time, number of producers and consumers
    if (argc != 4) { 
        exit(EXIT_FAILURE);
    }

    // Assign each value to the variables
    int lengthOfSleep = atoi(argv[1]);
    int numOfProducers = atoi(argv[2]);
    int numOfConsumers = atoi(argv[3]);

    // Initialize semaphores and mutex
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // Create the producer/consumer thread
    pthread_t producers[numOfProducers];
    pthread_t consumers[numOfConsumers];

    // Create producer threads
    for (int i = 0; i < numOfProducers; i++) {
        int *producerID = malloc(sizeof(int)); // assign memory size for each producer thread
        *producerID = i; // assign ID for each producer
        pthread_create(&producers[i], NULL, producer, (void *)producerID);
    }

    // Create consumer threads
    for (int i = 0; i < numOfConsumers; i++) {
        int *consumerID = malloc(sizeof(int)); // assign memory size for each consumer thread
        *consumerID = i; // assign ID for each consumer
        pthread_create(&consumers[i], NULL, consumer, (void *)consumerID);
    }

    // Sleep for a period of time and, upon
    // awakening, will terminate the C program
    sleep(lengthOfSleep);
    exit(EXIT_SUCCESS);
}

void *producer(void *param) {
    int producerID = *((int *)param);
    buffer_item item;

    while (1) {
        /* sleep for a random period of time: 0-4 seconds */
        sleep(rand() % 5);

        /* generate a random number */
        item = rand();
        sem_wait(&empty);

        /* insert an item */
        insert_item(item, producerID);
        sem_post(&full);
    }
}

void *consumer(void *param) {
    int consumerID = *((int *)param);
    buffer_item item;

    while (1) {
        /* sleep for a random period of time: 0-4 seconds */
        sleep(rand() % 5);
        sem_wait(&full);

        /* remove an item */
        item = remove_item(consumerID);
        sem_post(&empty);
    }
}

// Insert an item into the buffer
void insert_item(buffer_item item, int producerID) {
    /* 
    Acquire the mutex lock
    calling thread is blocked till the owner invokes
    pthread_mutex_unlock()
    */
    pthread_mutex_lock(&mutex);
    buffer[inIndex] = item;

    printf("Producer %d inserted item %d into buffer[%d]\n", producerID, item, inIndex);

    inIndex = (inIndex + 1) % BUFFER_SIZE;

    /*
    Release the mutex lock
    */
    pthread_mutex_unlock(&mutex);
}

// Delete an item from the buffer
buffer_item remove_item(int consumerID) {
    /* 
    Acquire the mutex lock
    calling thread is blocked till the owner invokes
    pthread_mutex_unlock()
    */
    pthread_mutex_lock(&mutex);
    buffer_item item = buffer[outIndex];

    printf("Consumer %d consumed item %d from buffer[%d]\n", consumerID, item, outIndex);

    // Assuming -1 to an empty slot
    buffer[outIndex] = -1; 
    outIndex = (outIndex + 1) % BUFFER_SIZE;

    /*
    Release the mutex lock
    */
    pthread_mutex_unlock(&mutex);

    // return the item that has been deleted
    return item;
}
