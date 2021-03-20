#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

void regionCriticaUno(){
	int i;
	printf("\nProceso en la region critica proceso con pid=%d\n",getpid());
	for(i=0;i<3;i++){
		printf("\nRegión critica: %d\n",i);
		sleep(1);	
	}
}
void regionCriticaDos(){
	int i;
	printf("\nProceso en la region critica proceso con pid=%d\n",getpid());
	for(i=0;i<10;i++){
		printf("\nRegión critica: %d\n",i);
		sleep(1);	
	}
}

void regionNoCritica(){
	int i;
	printf("\nEn la region NO critica proceso con pid=%d\n",getpid());
	for(i=0;i<5;i++){
		printf("\nContando(%d): %d\n",getpid(),i);
		sleep(1);
	}
}

int main(){
	int i;
	int *p1DeseaEntrar;
	int *p2DeseaEntrar;
	int *procesoFavorecido;
	key_t llave1 = ftok("Proceso1", 'a');
	key_t llave2 = ftok("Proceso2", 'b');
	key_t llave3 = ftok("ProcesoF", 'c');
	int shmid1 = shmget(llave1, sizeof(int), IPC_CREAT | 0600);
	int shmid2 = shmget(llave2, sizeof(int), IPC_CREAT | 0600);
	int shmid3 = shmget(llave3, sizeof(int), IPC_CREAT | 0600);
	p1DeseaEntrar = shmat(shmid1, 0, 0);
	p2DeseaEntrar = shmat(shmid2, 0, 0);
	procesoFavorecido = shmat(shmid3, 0, 0);

	while (1){
		*p2DeseaEntrar = 1;
		while(*p1DeseaEntrar)
			if(*procesoFavorecido == 1){
				*p2DeseaEntrar = 0;
				while(*procesoFavorecido == 1)
				*p2DeseaEntrar = 1;
			}
		regionCriticaDos();
		*procesoFavorecido = 1;
		*p2DeseaEntrar= 0;
		regionNoCritica();
	}
	

	return 0;
}