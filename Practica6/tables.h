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
} Adress;

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

Valid ucharToValid(unsigned char uc){
    Valid v;
    v.a = uc;
    return v;
}

unsigned char pageIndexToUChar(PageIndex page_index) {
    return page_index.a + page_index.b * 2 + page_index.c * 4 + page_index.d * 8;
}

unsigned char frameIndexToUChar(FrameIndex frame_index) {
    return frame_index.a + frame_index.b * 2 + frame_index.c * 4;
}

unsigned char validToUChar(Valid v){
    return v.a;
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
    int size;
} PageList;

typedef struct FrameList {
    struct FrameListElement* head;
    int size;
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

int pagesSize(PageList* list){
    return list->size;
}

int framesSize(FrameList* list){
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

void addPage(PageList* list, PageIndex table_index, FrameIndex content, Valid valid) {
    PageListElement* element = createPageElement(table_index, content, valid);
    element->next = list->head;
    if(!isEmptyPages(list))
        list->head->prev = element;
    list->head = element;
    ++list->size;
}

void addFrame(FrameList* list, FrameIndex table_index, PageIndex content) {
    FrameListElement* element = createFrameElement(table_index, content);
    element->next = list->head;
    if(!isEmptyFrames(list))
        list->head->prev = element;
    list->head = element;
    ++list->size;
}

PageListElement* getPage(PageList* list, int index){
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

FrameListElement* getFrame(FrameList* list, int index){
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

void setPage(PageList* list, int index, PageIndex table_index, FrameIndex content, Valid valid) {
    PageListElement* aux;
    if((aux = getPage(list, index))) {
        aux->table_index = table_index;
        aux->content = content;
        aux->valid = valid;
    } else 
        puts("Pocisión inválida");
}

void setFrame(FrameList* list, int index, FrameIndex table_index, PageIndex content) {
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
        printf("|%u%u%u%u (%d)\t|%u%u%u (%d)\t|%u\t|\n", element->table_index.a, element->table_index.b, element->table_index.c, element->table_index.d, pageIndexToUChar(element->table_index), element->content.a, element->content.b, element->content.c, frameIndexToUChar(element->content), element->valid.a);
        element = element->next;
    }
    printf("|%u%u%u%u (%d)\t|%u%u%u (%d)\t|%u\t|\n", element->table_index.a, element->table_index.b, element->table_index.c, element->table_index.d, pageIndexToUChar(element->table_index), element->content.a, element->content.b, element->content.c, frameIndexToUChar(element->content), element->valid.a);
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
        printf("|%u%u%u (%d)\t|%u%u%u%u (%d)\t|\n", element->table_index.a, element->table_index.b, element->table_index.c, frameIndexToUChar(element->table_index), element->content.a, element->content.b, element->content.c, element->content.d, pageIndexToUChar(element->content));
        element = element->next;
    }
    printf("|%u%u%u (%d)\t|%u%u%u%u (%d)\t|\n", element->table_index.a, element->table_index.b, element->table_index.c, frameIndexToUChar(element->table_index), element->content.a, element->content.b, element->content.c, element->content.d, pageIndexToUChar(element->content));
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
