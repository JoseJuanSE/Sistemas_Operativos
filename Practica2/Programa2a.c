#include <stdlib.h>
#include <stdio.h>
int main(){
    FILE *file;
	char *filename = "filen.txt";
    if((file = fopen(filename, "w")) == NULL){
		fprintf(stderr, "Error al crear el archivo");
		exit(1);
	}
    int i, numbers[10];
    for(i = 0; i < 10; ++i){
        printf("Introduzca el número %d: ", i + 1);
        fflush(stdin);
        scanf("%d", &numbers[i]);
        fprintf(file, "%d\n", numbers[i]);
    }
	fclose(file);
    return 0;
}