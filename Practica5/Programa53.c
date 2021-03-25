#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void *Child_Thread(void *argumentos) {
    printf("\n \tSoy el Hilo: %ld \n", pthread_self());
}

void *Thread() {
    pthread_t thread11, thread22;
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    printf("\nSoy el Hilo: %ld \n", pthread_self());
    pthread_create(&thread11, &attributes, Child_Thread, NULL);
    pthread_create(&thread22, &attributes, Child_Thread, NULL);
}

int main() {
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        perror("\nError al crear el proceso\n");
        exit(-1);
    }
    if (pid == 0) {
        pthread_t thread1, thread2, thread3;
        pthread_attr_t attributes;
        pthread_attr_init(&attributes);
        pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread1, &attributes, Thread, NULL);
        pthread_create(&thread2, &attributes, Thread, NULL);
        pthread_create(&thread3, &attributes, Thread, NULL);
        sleep(1);
    }
    else
        wait(NULL);

    return 0;
}