#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

typedef long long ll;

ll num;

ll es_primo(ll n){
    ll i;
    if(n<=1) return 0;
	if(n%2==0 && n!=2) return 0;
	for(i=3;i*i<=n;i+=2){
		if(n%i==0) return 0;
	}
	return 1;	
}
void descomponer(){
    ll i, x = num;
    printf("Los factores de %ld son:\n", num);
	for(ll i=2;i<=x;i++){
        while(x%i==0){
            printf("(%ld)",i);
            x/=i;
        }
    }
    if(x!=1)printf("%ld\n",x);
    printf("\n");
	pthread_exit(NULL);
}
void primo(){
    printf("El numero %ld es primo\n", num);
    pthread_exit(NULL);
}
void esCompuesto(){
    pthread_t h2;
    pthread_t h3;
    if(num<=1) 
        printf("El numero %ld no es primo ni compuesto\n", num);
	else if(!es_primo(num)){
        pthread_create(&h2, NULL, (void*)descomponer, NULL);	
        pthread_join(h2, NULL);
    }
    else {
        pthread_create(&h3, NULL, (void*)primo, NULL);	
        pthread_join(h3, NULL);
    } 
	pthread_exit(NULL);
}
int main(){
    scanf("%ld",&num);
	pthread_t h1;
	pthread_create(&h1, NULL, (void*)esCompuesto, NULL);	
	pthread_join(h1, NULL);
	return 0;
}