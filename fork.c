#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pidB;
    int statusB;

    pidB = fork();

    if(pidB == 0){
        pid_t pidC;
        int statusC;
        pidC = fork();

        if(pidC == 0){
            printf("Process C: %d\n", getpid());
            sleep(5); // some process in C
            printf("Parent of Process C: %d\n", getppid());
            printf("Process C: %d is done\n", getpid());
            exit(0);
        }
        else if(pidC > 0){
            printf("Process B: %d\n", getpid());
            printf("Parent of Process B: %d\n", getppid());
            sleep(2); // some process in B
            // wait(&statusC);
            printf("Process B: %d is terminating.\n", getpid());
            exit(0);
        }
        else{
            printf("Error creating process C\n");
        }
    }
    else if(pidB > 0){
        printf("Process A: %d\n", getpid());
        printf("Child of process A: %d\n", pidB);
        wait(&statusB);
    }
    else{
        printf("Error creating process B\n");
    }

    return 0;
}