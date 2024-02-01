#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int a = 0;
int b = 1;
pthread_mutex_t lock;

void *runner() {
    pthread_mutex_lock(&lock);
    int c = a + b;
    printf("Thread ID: %ld, Fibonacci Number: %d\n", pthread_self(), a);
    a = b;
    b = c;
    pthread_mutex_unlock(&lock);
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr,"usage: a.out <integer value>\n");
        return -1;
    }
    if (atoi(argv[1]) < 0) {
        fprintf(stderr,"%d must be >= 0\n",atoi(argv[1]));
        return -1;
    }
    int i = atoi(argv[1]);
    pthread_t tid[i];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_mutex_init(&lock, NULL);
    for (int j = 0; j < i; j++) {
        pthread_create(&tid[j], &attr, runner, NULL);
    }
    for (int j = 0; j < i; j++) {
        pthread_join(tid[j], NULL);
    }
    pthread_mutex_destroy(&lock);
    return 0;
}