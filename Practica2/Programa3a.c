#include <stdlib.h>
#include <stdio.h>
int main(){
    FILE *file;
	char *filename = "filen.txt";
    if((file = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error al abrir el archivo");
		exit(1);
	}   
    int i, numbers[10];         
    for(i = 0; i < 10; ++i){
        fscanf(file, "%d", &numbers[i]);
        printf("El número %d es : %d\n", i + 1, numbers[i]);
    }
    fclose(file);
    return 0;
}