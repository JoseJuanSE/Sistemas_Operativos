#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

typedef int semaphore;
semaphore count;

int var = 0;

int semaphore_init(key_t key, int value) {
    int semid = semget(key, 1, IPC_CREAT | 0644);
    if(semid == -1){
        fprintf(stderr, "Error al crear semáforo");
        exit(1);
    }
    semctl(semid, 0, SETVAL, value);
    return semid;
}
void down(int semid) {
    struct sembuf op_p[] = {0, -1, 0};
    semop(semid, op_p, 1);
}
void up(int semid) {
    struct sembuf op_v[] = {0, +1, 0};
    semop(semid,op_v, 1);
}
void Thread1(void *var) {
    int *v = (int*)var;
    while (1) {
        down(count);
        (*v)++;
        up(count);
    }
}
void Thread2(void *var) {
    int *v = (int*)var;
    while (1) {
        down(count);
        (*v)--;
        up(count);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2)
        exit(1);
    int wait_s = atoi(argv[1]);
    key_t keys = ftok("count", 'c');
    count = semaphore_init(keys, 1);
    pthread_attr_t atributos;
    pthread_t thread1, thread2;
    pthread_attr_init(&atributos);
    pthread_attr_setdetachstate(&atributos, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread1, &atributos, (void*)Thread1, (void*)&var);
    pthread_create(&thread2, &atributos, (void*)Thread2, (void*)&var);
    sleep(wait_s);
    printf("\nEl valor de la variable es: %d\n", var);
    return 0;
}