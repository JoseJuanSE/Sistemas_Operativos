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

void swapping(unsigned char *frame_index, short *page_index){
    PageIndex pg_i = ucharToPageIndex(*page_index);
    FrameIndex fm_i = ucharToFrameIndex(*frame_index);
    unsigned short i;
    for(i = 0; i < 4096; i++){
        Address addr = ushortToAddress(&i);
        printf("\nDirección virtual entrante: %u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u (%d)\nDirección física saliente: %u%u%u%u%u%u%u%u%u%u%u%u%u%u%u (%d)\n", 
            pg_i.d, pg_i.c, pg_i.b, pg_i.a, addr.l, addr.k, addr.j, addr.i, addr.h, addr.g, addr.f, addr.e, addr.d, addr.c, addr.b, addr.a,
            virtualAddressToUShort(&pg_i, &addr), fm_i.c, fm_i.b, fm_i.a, addr.l, addr.k, addr.j, addr.i, addr.h, addr.g, addr.f, addr.e, addr.d, addr.c,
            addr.b, addr.a, physicalAddressToUShort(&fm_i, &addr));
    }
}

void pageFault(short *index_toAdd) {
    PageListElement* page = getPage(page_table, *index_toAdd);
    if(!page)
        printf("\n\nLa página índicada no se encuentra en el disco.\n");
    if(!page->valid.a){
        unsigned char swap = deQueue(page_fault_algorithm);
        setFrame(frame_table, swap, ucharToFrameIndex(swap), ucharToPageIndex(*index_toAdd));
        setPage(page_table, swap, ucharToPageIndex(swap), ucharToFrameIndex(0), ucharToValid(0));
        setPage(page_table, *index_toAdd, ucharToPageIndex(*index_toAdd), ucharToFrameIndex(swap), ucharToValid(1));
        swapping(&swap, index_toAdd);
        printf("\n\nLa página índicada se ha cargado en la memoria física de acuerdo al algoritmo FIFO.\n");
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
    char control = 's';
    short page_index_toAdd;
    loadProgram();
    do{
        printf("Indique el índice de página a insertar en la memoria física: ");
        fflush(stdin);
        scanf("%hd", &page_index_toAdd);
        pageFault(&page_index_toAdd);
        printf("¿Desea continuar (s/n, S/N)? ");
        fflush(stdin);
        scanf("%c", &control);
        if(!(control == 's' || control == 'S'))
            break;
    } while(1);
    clearADTs();
    return 0;
}
