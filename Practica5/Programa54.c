#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

int results[2] = {0, 0};
void *CountStringOccurrences(void *wordCount) {
    FILE *file;
    char word[15], *pos, string[255];
    int i = 0;
    if((file = fopen("Programa54.txt", "r")) == NULL){
		fprintf(stderr, "Error al leer el archivo");
		exit(1);
	}
    printf("Ingrese la palabra a buscar en el archivo (maxlength 15): ");
    scanf("%s", word);
    while ((fgets(string, 255, file)) != NULL) {
        i = 0;
        while ((pos = strstr(string + i, word)) != NULL) {
            i = (pos - string) + 1;
            (*((int*)wordCount))++;
        }
    }
    fclose(file);
    pthread_exit(NULL);
}
void *CopyFiles(void *filesCopied) {
    FILE *currentLsFile, *copiedLsFile;
    char path[255] , command[255], currentPath[255];
    int c, cLs = 0, ccLs = 0;
    printf("Ingrese la ruta en donde se desea copiar el directorio actual: ");
    scanf("%s", path);
    strcpy(command, "ls ");
    strcat(command, path);
    strcat(command, " >> currentLs.txt");
    system(command);
    if((currentLsFile = fopen("currentLs.txt", "r")) == NULL){
		fprintf(stderr, "Error al leer el archivo");
		exit(1);
	}
    while(!feof(currentLsFile)) {
        c = fgetc(currentLsFile);
        if(c == '\n')
            cLs++;
    }
    system("rm currentLs.txt");
    strcpy(command, "cp -R * ");
    strcat(command, path);
    system(command);
    strcpy(command, "ls ");
    strcat(command, path);
    strcat(command, " >> copiedLs.txt");
    system(command);
    if((copiedLsFile = fopen("copiedLs.txt", "r")) == NULL){
		fprintf(stderr, "Error al leer el archivo");
		exit(1);
	}
    while(!feof(copiedLsFile)) {
        c = fgetc(copiedLsFile);
        if(c == '\n')
            ccLs++;
    }
    system("rm copiedLs.txt");
    *((int*)filesCopied) = ccLs - cLs;
    pthread_exit(NULL);
}
void *ResultsReport(void *result) {
    FILE *file;
    if((file = fopen("Programa54Result.txt", "w")) == NULL){
		fprintf(stderr, "Error al leer el archivo");
		exit(1);
	}
    fprintf(file, "Ocurrencias de la palabra: %d, Archivos copiados: %d\n", ((int*)result)[0], ((int*)result)[1]);
    fclose(file);
    puts("Utilice 'cat Programa54Result.txt' para visualizar el resultado");
    pthread_exit(NULL);
}
int main(){
    pthread_t thread1, thread2, thread3;
    pthread_create(&thread1, NULL, CountStringOccurrences, (void*)results);
    pthread_join(thread1, NULL);
    pthread_create(&thread2, NULL, CopyFiles, (void*)(results + 1));
    pthread_join(thread2, NULL);
    pthread_create(&thread3, NULL, ResultsReport, (void*)results);
    pthread_join(thread3, NULL);
    wait(NULL);
    return 0;
}