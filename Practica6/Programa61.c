#include "circular_dynamic_queue.h"
#include "doubly_linked_list.h"

#define MEMORY_LENGTH 32
#define VIRTUAL_MEMORY_LENGTH 64
#define PAGE_LENGTH 4

void loadProgram(){
    List* page_table = createList(), *frame_table = createList();
    unsigned char i;
    for(i = 0; i < VIRTUAL_MEMORY_LENGTH / PAGE_LENGTH; i++)
        append(page_table, i, 255);
    printf("\nEl programa se ha cargado en el disco.\nTabla de páginas en el disco.\n|Page_index\t|Frame_index\t|\n");
    printList(page_table);
    for(i = 0; i < MEMORY_LENGTH / PAGE_LENGTH; i++){
        append(frame_table, i, i);
        set(page_table, i, i, i);
    }
    printf("\nEl programa se ha cargado en la memoria principal.\nTabla de marcos de página en memoria.\n|Frame_index\t|Page_index\t|\n");
    printList(frame_table);
    printf("\nTabla de páginas en el disco.\n|Page_index\t|Frame_index\t|\n");
    printList(page_table);
}

int main() {
    loadProgram();

    return 0;
}
