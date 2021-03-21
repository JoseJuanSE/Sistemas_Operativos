#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

int main(){
    pid_t pid; //padre
    if(pid != 0 && pid != -1)
        printf("Hola soy el proceso padre con PID %d y mi padre es %d\n", getpid(), getppid());
    pid = fork();
    if(pid == -1){
        perror("\nError al crear el proceso\n");
        exit(-1);
    } else if(pid == 0){ //hijo
        int numProc;
        printf("\tHola soy el proceso hijo con PID %d y mi padre es %d\n", getpid(), getppid());
        for (numProc = 0; numProc < 3; numProc++) {
            pid = fork();
            if (pid == 0) break;
            else 
                if(pid == -1){
                    perror("\nError al crear el proceso\n");
                    exit(-1);
                }
        }
        if (pid == 0) { //Nietos
            printf("\t\tHola soy el proceso nieto con PID %d y mi padre es %d\n", getpid(), getppid());
            for (numProc = 0; numProc < 2; numProc++) {
                pid = fork();
                if (pid == 0) break;
                else 
                    if(pid == -1){
                        perror("\nError al crear el proceso\n");
                        exit(-1);
                    }
            }
            if (pid == 0) { //Bisnietos
                printf("\t\t\tHola soy el proceso bisnieto con PID %d y mi padre es %d\n", getpid(), getppid());
                exit(0);
            } else
                for (int i = 0; i < 2; i++)
                    wait(NULL);
            exit(0);
        } else
            for (int i = 0; i < 3; i++)
                wait(NULL);
        exit(0);
    } else //Padre
        wait(NULL);
    return 0;
}