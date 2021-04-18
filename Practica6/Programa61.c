#include "circular_dynamic_queue.h"
#include "doubly_linked_list.h"

#define MEMORY_LENGTH 32
#define VIRTUAL_MEMORY_LENGTH 64
#define PAGE_LENGTH 4

List *page_table, *frame_table;
Queue *page_fault_algorithm;

void printTables(){
    printf("\nTabla de páginas en el disco.\n|Page_index\t|Frame_index\t|\n");
    printList(page_table);
    printf("\nTabla de marcos de página en memoria.\n|Frame_index\t|Page_index\t|\n");
    printList(frame_table);
}

void loadProgram(){
    unsigned char i;
    page_table = createList();
    frame_table = createList();
    page_fault_algorithm = createQueue();
    for(i = 0; i < VIRTUAL_MEMORY_LENGTH / PAGE_LENGTH; i++)
        append(page_table, i, 255);
    printf("\nEl programa se ha cargado en el disco.\n");
    printTables();
    for(i = 0; i < MEMORY_LENGTH / PAGE_LENGTH; i++){
        append(frame_table, i, i);
        enQueue(page_fault_algorithm, i);
        set(page_table, i, i, i);
    }
    printf("\nEl programa se ha cargado en la memoria principal.\n");
    printTables();
}

void pageFault(int *index_toAdd) {
    ListElement* page = get(page_table, *index_toAdd);
    if(!page)
        printf("\n\nLa página índicada no se encuentra en el disco.\n");
    if(page->content == 255){
        unsigned char swap = deQueue(page_fault_algorithm);
        set(frame_table, swap, swap, *index_toAdd);
        set(page_table, swap, swap, 255);
        set(page_table, *index_toAdd, *index_toAdd, swap);
        printf("\n\nLa página índicada se ha cargado en la memoria física de acuerdo al aalgoritmo FIFO.\n");
        printTables();
    } else 
        printf("\n\nLa página índicada ya se encuentra en la memoria física.\n");
}

void clearADTs(){
    clearList(page_table);
    clearList(frame_table);
    clearQueue(page_fault_algorithm);
}

int main() {
    int page_index_toAdd;
    loadProgram();
    printf("Indique el índice de página a insertar en la memoria física: ");
    fflush(stdin);
    scanf("%d", &page_index_toAdd);
    pageFault(&page_index_toAdd);
    clearADTs();
    return 0;
}
