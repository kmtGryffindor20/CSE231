#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define M 3
#define K 2
#define N 3

int A[M][K] = {{1, 4}, {2, 5}, {3, 6}};
int B[K][N] = {{8, 7, 6}, {5, 4, 3}};
int C[M][N];
int D[M][N];

typedef struct {
    int i;
    int j;
} Args;

void* multiply(void *args) {
    Args *arg = (Args*)args;
    int sum = 0;
    for (int k = 0; k < K; k++) {
        sum += A[arg->i][k] * B[k][arg->j];
    }
    C[arg->i][arg->j] = sum;
    pthread_exit(0);
}

typedef struct {
    int this_thread_index;
    int total_threads;
} _Args;

void* _multiply(void *args) {
    _Args *arg = (_Args*)args;
    int sum = 0;
    for (int i = arg->this_thread_index; i < M; i += arg->total_threads) {
        for (int j = 0; j < N; j++) {
            sum = 0;
            for (int k = 0; k < K; k++) {
                sum += A[i][k] * B[k][j];
            }
            D[i][j] = sum;
        }
    }
    pthread_exit(0);
}

int main() {
    pthread_t threads[M * N];
    int count = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            Args *args = (Args*)malloc(sizeof(Args));
            args->i = i;
            args->j = j;
            pthread_create(&threads[count], NULL, multiply, args);
            count++;
        }
    }
    for (int i = 0; i < M * N; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Product of the matrices:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    // Using multiple threads
    int MAX_THREADS = M*N;
    int num_threads = 1;
    while(num_threads <= MAX_THREADS)
    {
        clock_t start_time, end_time;
        start_time = clock();
        pthread_t _threads[num_threads];
        _Args _args[num_threads];
        for (int i = 0; i < num_threads; i++) {
            _args[i].this_thread_index = i;
            _args[i].total_threads = num_threads;
            pthread_create(&_threads[i], NULL, _multiply, &_args[i]);
        }
        for (int i = 0; i < num_threads; i++) {
            pthread_join(_threads[i], NULL);
        }
        end_time = clock();
        printf("Time taken for %d threads: %fs\n", num_threads, (double)(end_time - start_time) / CLOCKS_PER_SEC);
        num_threads++;
    }
    return 0;
}