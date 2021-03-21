#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h>
#include<sys/wait.h> 
#include<sys/types.h>

int main() {
    int i, numProc, counter;
    pid_t pid;

    for (numProc = 0; numProc < 10; numProc++) {
        pid = fork();
        if (pid == 0) break;
        else if(pid == -1){
            printf("Error al realizar fork()");
            exit(1);
        }
    }
    if (pid == 0) {
        printf("%d. Hola soy el proceso con PID %d y mi padre es %d\nProcederé a realizar un conteo del uno al diez:\n", numProc + 1, getpid(), getppid());
        for (counter = 1; counter < 10; counter++)
            printf("%d, ", counter);
        printf("%d\n", counter);
        exit(0);
    } else
        for (i = 0; i < 10; i++)
            wait(NULL);
    
    return 0;
}
