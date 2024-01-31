#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int a = 0;
int b = 1;

void *runner() {
    int c = a + b;
    printf("%d\n", a);
    a = b;
    b = c;
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
    while (i--)
    {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid,&attr,runner,NULL);
        pthread_join(tid,NULL);
    }
}