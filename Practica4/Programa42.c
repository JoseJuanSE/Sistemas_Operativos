#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

int main()
{
    float *arreglo;
    key_t llave = ftok("shmfile", 'k'); // se debe crear un archivo de nombre shmfile, mismo directorio
    int shmid = shmget(llave, sizeof(float) * 10, IPC_CREAT | 0600);

    printf("Hola soy el proceso padre con pid = %d y creare un array float\n", getpid());
    arreglo = (float *)shmat(shmid, 0, 0);
    printf("Lo he creado, tiene los siguientes valores: \n");
    for (int i = 0; i < 10; i++)
    {
        printf("Elemento %d del arreglo: %f\n", i + 1, arreglo[i]);
    }
    printf("\n\n");
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("\nError al crear el proceso\n");
        exit(-1);
    }
    if (pid == 0)
    {
        printf("Hola soy el proceso hijo con pid = %d y ppid = %d y modificare al array\n", getpid(), getppid());
        srand((unsigned int)time(NULL));
        float a = 5.0;
        for (int i = 0; i < 10; i++)
        {
            float x = (float)rand() / (float)(RAND_MAX / a);
            arreglo[i] = x;
        }
        printf("MODIFICADO..\n\n");
    }
    else
    {
        wait(NULL);
        printf("Hola soy el padre de nuevo, con pid = %d y mostrare el arreglo\n", getpid());
        for (int i = 0; i < 10; i++)
        {
            printf("Elemento %d del arreglo: %f\n", i + 1, arreglo[i]);
        }
        shmctl(shmid, IPC_RMID, NULL); // borramos la memoria compartida
    }
    return 0;
}