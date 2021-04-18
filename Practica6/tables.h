/****************************************************************
 * tables
 * @author: Carlos Huerta García
 * @description: Virtual Memory doubly linked list implementation
 * **************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* 4 bits page index */
typedef struct PageIndex {
    unsigned int a:1;
    unsigned int b:1;
    unsigned int c:1;
    unsigned int d:1;
} PageIndex;

/* 3 bits frame index */
typedef struct FrameIndex {
    unsigned int a:1;
    unsigned int b:1;
    unsigned int c:1;
} FrameIndex;

/* 12 bits memory address size */
typedef struct Address {
    unsigned int a:1;
    unsigned int b:1;
    unsigned int c:1;
    unsigned int d:1;
    unsigned int e:1;
    unsigned int f:1;
    unsigned int g:1;
    unsigned int h:1;
    unsigned int i:1;
    unsigned int j:1;
    unsigned int k:1;
    unsigned int l:1;
} Address;

/* 1 bit present/ausent in memory */
typedef struct Valid {
    unsigned int a:1;
} Valid;

PageIndex ucharToPageIndex(unsigned char uc) {
    PageIndex page_index;
    page_index.a = uc;
    page_index.b = uc >> 1;
    page_index.c = uc >> 2;
    page_index.d = uc >> 3;
    return page_index;
}

FrameIndex ucharToFrameIndex(unsigned char uc) {
    FrameIndex frame_index;
    frame_index.a = uc;
    frame_index.b = uc >> 1;
    frame_index.c = uc >> 2;
    return frame_index;
}

Address ushortToAddress(unsigned short *us) {
    Address addr;
    addr.a = *us;
    addr.b = (*us) >> 1;
    addr.c = (*us) >> 2;
    addr.d = (*us) >> 3;
    addr.e = (*us) >> 4;
    addr.f = (*us) >> 5;
    addr.g = (*us) >> 6;
    addr.h = (*us) >> 7;
    addr.i = (*us) >> 8;
    addr.j = (*us) >> 9;
    addr.k = (*us) >> 10;
    addr.l = (*us) >> 11;
    return addr;
}

Valid ucharToValid(unsigned char uc){
    Valid v;
    v.a = uc;
    return v;
}

unsigned char pageIndexToUChar(PageIndex *page_index) {
    return page_index->a + page_index->b * 2 + page_index->c * 4 + page_index->d * 8;
}

unsigned char frameIndexToUChar(FrameIndex *frame_index) {
    return frame_index->a + frame_index->b * 2 + frame_index->c * 4;
}

unsigned short addressToUShort(Address *addr) {
    return addr->a + addr->b * 2  + addr->c * 4  + addr->d * 8  + addr->e * 16  + addr->f * 32 
     + addr->g * 64  + addr->h * 128  + addr->i * 256  + addr->j * 512  + addr->k * 1024  + addr->l * 2048;
}

unsigned short virtualAddressToUShort(PageIndex *page_index, Address *addr){
    return page_index->a * 32768 + page_index->b * 16384 + page_index->c * 8192 + page_index->d * 4096 + addressToUShort(addr);
}

unsigned short physicalAddressToUShort(FrameIndex *frame_index, Address *addr){
    return frame_index->a * 16384 + frame_index->b * 8192 + frame_index->c * 4096 + addressToUShort(addr);
}

typedef struct PageListElement {
    PageIndex table_index;
    FrameIndex content;
    Valid valid;
    struct PageListElement* next;
    struct PageListElement* prev;
} PageListElement;

typedef struct FrameListElement {
    FrameIndex table_index;
    PageIndex content;
    struct FrameListElement* next;
    struct FrameListElement* prev;
} FrameListElement;

typedef struct PageList {
    struct PageListElement* head;
    unsigned short size;
} PageList;

typedef struct FrameList {
    struct FrameListElement* head;
    unsigned short size;
} FrameList;

PageList* createPageList() {
    PageList* list = (PageList*)malloc(sizeof(PageList));
    if(!list)
        exit(1);
    list->head = NULL;
    list->size = 0;
    return list;
}

FrameList* createFrameList() {
    FrameList* list = (FrameList*)malloc(sizeof(FrameList));
    if(!list)
        exit(1);
    list->head = NULL;
    list->size = 0;
    return list;
}

unsigned short pagesSize(PageList* list){
    return list->size;
}

unsigned short framesSize(FrameList* list){
    return list->size;
}

bool isEmptyPages(PageList* list){
    return !list->head;
}

bool isEmptyFrames(FrameList* list){
    return !list->head;
}

PageListElement* createPageElement(PageIndex table_index, FrameIndex content, Valid valid) {
    PageListElement* element = (PageListElement*)malloc(sizeof(PageListElement));
    element->table_index = table_index;
    element->content = content;
    element->valid = valid;
    element->prev = element->next = NULL;
    return element;
}

FrameListElement* createFrameElement(FrameIndex table_index, PageIndex content) {
    FrameListElement* element = (FrameListElement*)malloc(sizeof(FrameListElement));
    element->table_index = table_index;
    element->content = content;
    element->prev = element->next = NULL;
    return element;
}

void appendPage(PageList* list, PageIndex table_index, FrameIndex content, Valid valid) {
    PageListElement* element = createPageElement(table_index, content, valid), *last = list->head;
    if(isEmptyPages(list)){
        element->prev = NULL;
        list->head = element;
        ++list->size;
        return;
    }
    while(last->next)
        last = last->next;
    last->next = element;
    element->prev = last;
    ++list->size;
}

void appendFrame(FrameList* list, FrameIndex table_index, PageIndex content) {
    FrameListElement* element = createFrameElement(table_index, content), *last = list->head;
    if(isEmptyFrames(list)){
        element->prev = NULL;
        list->head = element;
        ++list->size;
        return;
    }
    while(last->next)
        last = last->next;
    last->next = element;
    element->prev = last;
    ++list->size;
}

PageListElement* getPage(PageList* list, short index){
    if(index < 0 || index >= list->size || isEmptyPages(list)) {
        puts("No existe el elemento");
        return NULL;
    }
    PageListElement* element = list->head;
    int i;
    for(i = 0; i < index; i++)
        element = element->next;
    return element;
}

FrameListElement* getFrame(FrameList* list, short index){
    if(index < 0 || index >= list->size || isEmptyFrames(list)) {
        puts("No existe el elemento");
        return NULL;
    }
    FrameListElement* element = list->head;
    int i;
    for(i = 0; i < index; i++)
        element = element->next;
    return element;
}

void setPage(PageList* list, short index, PageIndex table_index, FrameIndex content, Valid valid) {
    PageListElement* aux;
    if((aux = getPage(list, index))) {
        aux->table_index = table_index;
        aux->content = content;
        aux->valid = valid;
    } else 
        puts("Pocisión inválida");
}

void setFrame(FrameList* list, short index, FrameIndex table_index, PageIndex content) {
    FrameListElement* aux;
    if((aux = getFrame(list, index))) {
        aux->table_index = table_index;
        aux->content = content;
    } else 
        puts("Pocisión inválida");
}

void printPageList(PageList* list){
    if(isEmptyPages(list)){
        puts("");
        return;
    }
    PageListElement* element = list->head;
    printf("\nTabla de páginas en el disco.\n|Page_index\t|Frame_index\t|Valid\t|\n");
    while (element->next) {  
        printf("|%u%u%u%u (%d)\t|%u%u%u (%d)\t|%u\t|\n", element->table_index.d, element->table_index.c, element->table_index.b, element->table_index.a, pageIndexToUChar(&element->table_index), element->content.c, element->content.b, element->content.a, frameIndexToUChar(&element->content), element->valid.a);
        element = element->next;
    }
    printf("|%u%u%u%u (%d)\t|%u%u%u (%d)\t|%u\t|\n", element->table_index.d, element->table_index.c, element->table_index.b, element->table_index.a, pageIndexToUChar(&element->table_index), element->content.c, element->content.b, element->content.a, frameIndexToUChar(&element->content), element->valid.a);
    element = element->next;
}

void printFrameList(FrameList* list){
    if(isEmptyFrames(list)){
        puts("");
        return;
    }
    FrameListElement* element = list->head;
    printf("\nTabla de marcos de página en memoria.\n|Frame_index\t|Page_index\t|\n");
    while (element->next) {  
        printf("|%u%u%u (%d)\t|%u%u%u%u (%d)\t|\n", element->table_index.c, element->table_index.b, element->table_index.a, frameIndexToUChar(&element->table_index), element->content.d, element->content.c, element->content.b, element->content.a, pageIndexToUChar(&element->content));
        element = element->next;
    }
    printf("|%u%u%u (%d)\t|%u%u%u%u (%d)\t|\n", element->table_index.c, element->table_index.b, element->table_index.a, frameIndexToUChar(&element->table_index), element->content.d, element->content.c, element->content.b, element->content.a, pageIndexToUChar(&element->content));
    element = element->next;
}

void clearPageList(PageList* list) {
    if(!isEmptyPages(list)){
        PageListElement* element;
        while ((element = list->head)) {
            list->head = list->head->next;
            free(element);
        }
        list->head = NULL;
        list->size = 0;
    }
}

void clearFrameList(FrameList* list) {
    if(!isEmptyFrames(list)){
        FrameListElement* element;
        while ((element = list->head)) {
            list->head = list->head->next;
            free(element);
        }
        list->head = NULL;
        list->size = 0;
    }
}
