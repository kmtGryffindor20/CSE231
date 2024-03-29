#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t empty;
sem_t full;
sem_t mutex;

void *producer(void *arg) {
    int item;
    while (1) {
        item = rand(); // Generate a random item
        
        sem_wait(&empty); // Wait for an empty slot in the buffer
        sem_wait(&mutex); // Acquire the mutex
        
        buffer[in] = item; // Add the item to the buffer
        in = (in + 1) % BUFFER_SIZE; // Update the in pointer
        
        sem_post(&mutex); // Release the mutex
        sem_post(&full); // Signal that the buffer is full
        
        printf("Produced item: %d\n", item);
    }
}

void *consumer(void *arg) {
    int item;
    while (1) {
        sem_wait(&full); // Wait for a full slot in the buffer
        sem_wait(&mutex); // Acquire the mutex

        item = buffer[out]; // Remove the item from the buffer
        out = (out + 1) % BUFFER_SIZE; // Update the out pointer

        sem_post(&mutex); // Release the mutex
        sem_post(&empty); // Signal that the buffer is empty

        printf("Consumed item: %d\n", item);
    }
}

int main(int argc, char* argv[]) {
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    if(argc != 3)
    {
        printf("Usage: %s <num_producers> <num_consumers>\n", argv[0]);
        return 1;
    }

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);

    pthread_t producer_thread [num_producers];
    pthread_t consumer_thread [num_consumers];

    for (int i = 0; i < num_producers; i++) {
        pthread_create(&producer_thread[i], NULL, producer, NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&consumer_thread[i], NULL, consumer, NULL);
    }

    for(int i = 0; i < num_producers; i++) {
        pthread_join(producer_thread[i], NULL);
    }
    for(int i = 0; i < num_consumers; i++) {
        pthread_join(consumer_thread[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    return 0;
}
