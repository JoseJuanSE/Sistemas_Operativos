#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h>
#include<sys/wait.h> 
#include<sys/types.h>

int main() {
    FILE *file;
    int numProc, counter;
    char *strings[5] = {"Hola ", "esta ", "es ", "mi ", "práctica "};
    if((file = fopen("frase", "w")) == NULL){
        fprintf(stderr, "Error al crear el archivo");
        exit(1);
    }
    pid_t pid;
    for (numProc = 0; numProc < 5; numProc++) {
        pid = fork();
        if (pid == 0) break;
        else if(pid == -1){
            printf("Error al realizar fork()");
            exit(1);
        }
    }
    if (pid == 0) {
        fprintf(file, "%s", strings[numProc]);
        exit(0);
    } else
        for (int i = 0; i < 5; i++)
            wait(NULL);
    fprintf(file, "uno.");
    return 0;
}