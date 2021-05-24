#include<stdio.h>

void editando(FILE x){
    //dar funciones a las teclas de flechitas
    //desplegar hasta arriba de la pantalla cuales son los comandos
    //para moverse en nuestro editor
    //mostrar el actual contenido del archivo en la terminal
    //ctrl + s para guardar el archivo
    //ctrl + q para regresar al menu
    // si nos da tiempo ctrl + z para deshacer cambios
    // si nos da tiempo hacer una funcion wrap -> hacer que lo del archivo se ajuste al tamaño de la pantalla
    
}
void selecArchivo(){
    FILE *x;
    //mostrar archivos disponibles
    //pedir que ingreses el nombre del archivo -> x
    editando(x);
}
void crearArchivo(){
    FILE *x;
    //ingresa el nombre de tu archivo con extension
    //x = read();
    editando(x);
}
void crearFichero(){
    // pide nombre del fichero
    // *lo crea* :v
}
int main(){
    while(1){
        //menu de acciones
        //system("clear");
        printf("1.- Abrir un documento para editar\n");
        printf("2.- Crear un nuevo archivo y empezar a editarlo\n");
        printf("3.- Crear un nuevo fichero en la ruta actual\n");
        printf("\nSelecciona una de las opciones (1-3): ");
        int x;
        scanf("%d",&x);
        if(x==1){
            selecArchivo();
        }else if(x==2){
            crearArchivo();
        }else if(x==3){
            crearFichero();
        }else{
            printf("\nIngresa una opcion valida...");
            //sleep;
        }
    }
}