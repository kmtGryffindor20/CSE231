#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#define M 1000
#define K 1000
#define N 1000

int A[M][K];
int B[K][N];
int C[M][N];
int D[M][N];

void fillMatrix(){
    srand(101);
    for(int i = 0; i < M; i++){
        for(int j = 0; j < K; j++){
            A[i][j] = rand() % 100;
        }
    }
    for(int i = 0; i < K; i++){
        for(int j = 0; j < N; j++){
            B[i][j] = rand() % 100;
        }
    }
}

void printMatrix() {
    printf("Matrix A:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("Matrix B:\n");
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
}

// typedef struct {
//     int i;
//     int j;
// } Args;

// void* multiply(void *args) {
//     Args *arg = (Args*)args;
//     int sum = 0;
//     for (int k = 0; k < K; k++) {
//         sum += A[arg->i][k] * B[k][arg->j];
//     }
//     C[arg->i][arg->j] = sum;
//     pthread_exit(0);
// }

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

int main(int argc, char* argv[]) {
    int MAX_THREADS = 0;
    int num_threads = 1;
    if (argc == 1)
    {
        MAX_THREADS = N*M;
    }
    else if (argc == 2){
        if(strcmp(argv[1], "--help") == 0)
        {
            printf("Usage: %s [-a] [max|num_threads] for only num_threads.\n", argv[0]);
            printf("Or: %s for running from 1 to max threads.\n", argv[0]);
            printf("Or: %s [num_threads] for running from 1 to num_threads.\n", argv[0]);
            printf("Or: %s [-r] [from] [to] for running from from to to threads.\n", argv[0]);
            printf("Options:\n");
            printf("  -a\t\tRun for all threads from 1 to max\n");
            printf("  max\t\tRun for max number of threads\n");
            printf("  num_threads\tRun for num_threads\n");
            printf("  -r\t\tRun for threads from [from] to [to]\n");
            return 0;
        }
        MAX_THREADS = atoi(argv[1]);
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "-a") != 0)
        {
            fprintf(stderr, "usage: %s [-a] [max|num_threads]\n", argv[0]);
            return 1;
        }
        if(strcmp(argv[2], "max") == 0)
        {
            MAX_THREADS = N*M;
            num_threads = MAX_THREADS;
        }
        else{

        MAX_THREADS = atoi(argv[2]);
        num_threads = atoi(argv[2]);
        }
    }
    else if(argc == 4)
    {
        if (strcmp(argv[1], "-r") != 0)
        {
            fprintf(stderr, "usage: %s [-a] [from] [to]\n", argv[0]);
            return 1;
        }
        MAX_THREADS = atoi(argv[3]);
        num_threads = atoi(argv[2]);
    }
    fillMatrix();

    // printMatrix();
    
    // pthread_t threads[M * N];
    // int count = 0;
    // for (int i = 0; i < M; i++) {
    //     for (int j = 0; j < N; j++) {
    //         Args *args = (Args*)malloc(sizeof(Args));
    //         args->i = i;
    //         args->j = j;
    //         pthread_create(&threads[count], NULL, multiply, args);
    //         count++;
    //     }
    // }
    // for (int i = 0; i < M * N; i++) {
    //     pthread_join(threads[i], NULL);
    // }
    // printf("Product of the matrices:\n");
    // for (int i = 0; i < M; i++) {
    //     for (int j = 0; j < N; j++) {
    //         printf("%d ", C[i][j]);
    //     }
    //     printf("\n");
    // }

    // Using multiple threads
    
    if(argc == 3)
    {
        printf("Running only for %d threads\n", MAX_THREADS);
    }
    int initial_num_threads = num_threads;
    double time_taken[MAX_THREADS];
    struct timeval before_time_tv;
    gettimeofday(&before_time_tv, NULL);
    while(num_threads <= MAX_THREADS)
    {
        struct timeval start_time_tv, end_time_tv, present_time_tv;
        gettimeofday(&start_time_tv, NULL);
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
        gettimeofday(&end_time_tv, NULL);
        time_taken[num_threads - 1] = (double)(end_time_tv.tv_usec - start_time_tv.tv_usec) / 1e6 + (end_time_tv.tv_sec - start_time_tv.tv_sec);
        num_threads++;
        int progress = (int)((double)(num_threads -initial_num_threads + 1) / (MAX_THREADS - initial_num_threads + 2) * 100);
        // prints progress number of hashes enclosed in square brackets
        char progress_bar[102] = {0};
        for (int i = 0; i < 100; i++) {
            if (i < progress) {
                progress_bar[i] = '#';
            } else {
                progress_bar[i] = ' ';
            }
        }
        gettimeofday(&present_time_tv, NULL);
        double present_time = (double)(present_time_tv.tv_usec - before_time_tv.tv_usec) / 1e6 + (present_time_tv.tv_sec - before_time_tv.tv_sec);
        printf("\r[%s] %d%%; %fs", progress_bar, progress, present_time);
        fflush(stdout);
    }
    // Find the best number of threads
    if (argc != 3)
    {

    int best_threads = initial_num_threads - 1;
    double best_time = time_taken[initial_num_threads - 1];
    for (int i = initial_num_threads - 1; i < MAX_THREADS; i++) {
        if (time_taken[i] < time_taken[best_threads]) {
            best_threads = i + 1;
            best_time = time_taken[i];
        }
    }
    printf("\nBest number of threads: %d\n", best_threads);
    printf("Time taken for %d threads: %fs\n", best_threads, best_time);
    }
    else
    {
        printf("\n");
    }

    // printf("Product of the matrices:\n");
    // for (int i = 0; i < M; i++) {
    //     for (int j = 0; j < N; j++) {
    //         printf("%d ", D[i][j]);
    //     }
    //     printf("\n");
    // }
    printf("\n");

    return 0;
}