#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/shm.h>

#define N 5
#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

typedef int semaphore;
int *status;
semaphore mutex;
semaphore semaphores[N];
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
void test(int i) {
    if (status[i] == HUNGRY && status[LEFT] != EATING && status[RIGHT] != EATING) {
        status[i] = EATING;
        sleep(2); 
        printf("El filósofo %d toma el tenedor %d and %d\n", i + 1, LEFT + 1, i + 1);
        printf("El filósofo %d está comiendo\n", i + 1);
        up(semaphores[i]);
    }
}
void take_forks(int i) {
    down(mutex);
    status[i] = HUNGRY;
    printf("El filósofo %d está hambriento\n", i + 1);
    test(i);
    up(mutex);
    down(semaphores[i]);

}
void put_forks(int i) {
    down(mutex);
    status[i] = THINKING;
    printf("El filósofo %d deja el tenedor %d and %d down\n", i + 1, LEFT + 1, i + 1); 
    printf("El filósofo %d está pensando\n", i + 1);
    test(LEFT);
    test(RIGHT);
    up(mutex);
}
void philosopher(int i) {
    while (1) {
        sleep(2);
        take_forks(i);
        sleep(0);
        put_forks(i);
    }
}
int main(){
    key_t key = ftok("mutex", 'm'), skey = ftok("status", 's'), keys[] = {ftok("s1", 'a'), ftok("s2", 'b'), ftok("s3", 'c'), ftok("s4", 'd'), ftok("s5", 'e')};
    int i, shmid = shmget(skey, sizeof(int) * N, IPC_CREAT | 0600);
    if(shmid == -1){
        fprintf(stderr, "Error al crear el espacio de memoria compartida");
        exit(1);
    }
    status = (int *) shmat(shmid, 0, 0);
    mutex = semaphore_init(key, 1); 
    for (i = 0; i < N; i++)
        semaphores[i] = semaphore_init(keys[i], 0);
    pid_t pid;
    for (i = 0; i < N; i++) {
        status[i] = THINKING;
        pid = fork();
        if (pid == 0) break;
        else if(pid == -1){
            puts("Error al realizar la llamada fork");
            exit(1);
        }
    }
    if (pid == 0)
        philosopher(i);
    else
        for (int i = 0; i < N; i++)
            wait(NULL);
    return 0;
}
