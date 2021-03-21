
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

void pthread_id(){
	printf("Soy el hilo %ld \n", pthread_self());//Id del hilo creado
	pthread_exit(NULL);
}

int main(){
	pthread_t h1, h2;	//Direcciones donde estarán los hilos
	pthread_create(&h1, NULL, (void *)pthread_id, NULL);	
	pthread_create(&h2, NULL, (void *)pthread_id, NULL);
	//Se espera a que terminen los hilos
	pthread_join(h1, NULL);
	pthread_join(h2, NULL);
    printf("Soy el hilo principal %ld \n", pthread_self());
	printf("Se terminaron de ejecutar los hilos\n");
	return 0;
}
