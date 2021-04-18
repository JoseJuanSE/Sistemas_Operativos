#include "page_fault_fifo_algorithm.h"
#include "tables.h"

PageList *page_table;
FrameList *frame_table;
Queue *page_fault_algorithm;

void loadProgram(){
    unsigned char i;
    page_table = createPageList();
    frame_table = createFrameList();
    page_fault_algorithm = createQueue();
    for(i = 0; i < 16; i++)
        addPage(page_table, ucharToPageIndex(i), ucharToFrameIndex(0), ucharToValid(0));
    printf("\nEl programa se ha cargado en el disco.\n");
    printPageList(page_table);
    printFrameList(frame_table);
    for(i = 0; i < 8; i++){
        addFrame(frame_table, ucharToFrameIndex(i), ucharToPageIndex(i));
        enQueue(page_fault_algorithm, i);
        setPage(page_table, i, ucharToPageIndex(i), ucharToFrameIndex(i), ucharToValid(1));
    }
    printf("\nEl programa se ha cargado en la memoria principal.\n");
    printPageList(page_table);
    printFrameList(frame_table);
}

void swapping(unsigned char *frame_index, unsigned char page_index){

}

void pageFault(int *index_toAdd) {
    PageListElement* page = getPage(page_table, *index_toAdd);
    if(!page)
        printf("\n\nLa página índicada no se encuentra en el disco.\n");
    if(!page->valid.a){
        unsigned char swap = deQueue(page_fault_algorithm);
        setFrame(frame_table, swap, ucharToFrameIndex(swap), ucharToPageIndex(*index_toAdd));
        setPage(page_table, swap, ucharToPageIndex(swap), ucharToFrameIndex(0), ucharToValid(0));
        setPage(page_table, *index_toAdd, ucharToPageIndex(*index_toAdd), ucharToFrameIndex(swap), ucharToValid(1));
        swapping(&swap, *index_toAdd);
        printf("\n\nLa página índicada se ha cargado en la memoria física de acuerdo al aalgoritmo FIFO.\n");
        printPageList(page_table);
        printFrameList(frame_table);
    } else 
        printf("\n\nLa página índicada ya se encuentra en la memoria física.\n");
}

void clearADTs(){
    clearPageList(page_table);
    clearFrameList(frame_table);
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
