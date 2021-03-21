#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#define a 10
#define b 48
int var;
void *hilo1(void *var){
	*((int*)var) +=a;
	pthread_exit(NULL);
}
void *hilo2(void *var){
	*((int*)var) -=b;
	pthread_exit(NULL);
}
int main(){
	pthread_t h1, h2;	//Direcciones donde estarán los hilos
	pthread_create(&h1, NULL, hilo1, (void*)&var);	
	pthread_create(&h2, NULL, hilo2, (void*)&var);
	//Se espera a que terminen los hilos
	pthread_join(h1, NULL);
	pthread_join(h2, NULL);
	printf("El valor de la variable es: %d\n", var);
	return 0;
}