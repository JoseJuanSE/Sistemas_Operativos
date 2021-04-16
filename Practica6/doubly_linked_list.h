/****************************************************************
 * doubly_linked_list
 * @author: Carlos Huerta García
 * @description: Virtual Memory doubly linked list implementation
 * **************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef unsigned char ListEntry;

typedef struct ListElement {
    ListEntry table_index, content;
    struct ListElement* next;
    struct ListElement* prev;
} ListElement;

typedef struct List {
    struct ListElement* head;
    int size;
} List;

List* createList() {
    List* list = (List*)malloc(sizeof(List));
    if(!list)
        exit(1);
    list->head = NULL;
    list->size = 0;
    return list;
}

int size(List* list){
    return list->size;
}

bool isEmpty(List* list){
    return !list->head;
}

ListElement* createElement(ListEntry table_index, ListEntry content) {
    ListElement* element = (ListElement*)malloc(sizeof(ListElement));
    element->table_index = table_index;
    element->content = content;
    element->prev = element->next = NULL;
    return element;
}

void add(List* list, ListEntry table_index, ListEntry content) {
    ListElement* element = createElement(table_index, content);
    element->next = list->head;
    if(!isEmpty(list))
        list->head->prev = element;
    list->head = element;
    ++list->size;
}

void append(List* list, ListEntry table_index, ListEntry content) {
    ListElement* element = createElement(table_index, content), *last = list->head;
    if(isEmpty(list)){
        element->prev = NULL;
        list->head = element;
        return;
    }
    while(last->next)
        last = last->next;
    last->next = element;
    element->prev = last;
    ++list->size;
}

void insertAfter(List* list, ListElement* prev, ListEntry table_index, ListEntry content) {
    if(!prev || isEmpty(list)){
        puts("No existe el elemento previo");
        return;
    }
    ListElement* element = createElement(table_index, content);
    element->next = prev->next;
    prev->next = element;
    element->prev = prev;
    if(element->next)
        element->next->prev = element;
    ++list->size;
}

void insertBefore(List* list, ListElement* next, ListEntry table_index, ListEntry content) {
    if(!next || isEmpty(list)){
        puts("No existe el elemento posterior");
        return;
    }
    ListElement* element = createElement(table_index, content);
    element->prev = next->prev;
    next->prev = element;
    element->next = next;
    if(element->prev)
        element->prev->next = element;
    else
        list->head = element;
    ++list->size;
}

ListElement* get(List* list, int index){
    if(index >= 0 && index < list->size && !isEmpty(list)) {
        puts("No existe el elemento");
        return NULL;
    }
    ListElement* element = list->head;
    int i;
    for(i = 1; element && i < index; i++)
        element = element->next;
    if(!element){
        puts("No existe el elemento");
        return NULL;
    }
    return element;
}

void insert(List* list, ListEntry table_index, ListEntry content, int index){
    if(index >= 0 && index < list->size && !isEmpty(list)) {
        puts("Pocisión inválida");
        return;
    }
    ListElement* prev = get(list, index), *element = createElement(table_index, content);
    insertAfter(list, prev, table_index, content);
}

void set(List* list, int index, ListEntry table_index, ListEntry content) {
    if(index >= 0 && index < list->size && !isEmpty(list)) {
        puts("Pocisión inválida");
        return;
    }
    ListElement* aux = get(list, index), *element = createElement(table_index, content);
    element->next = aux->next;
    element->prev = aux->prev;
    element->next->prev = element->prev->next = element;
    free(aux);
}

int indexOf(List * list, ListElement* element) {
    int index;
    ListElement* aux = list->head;
    for(index = 0; aux != element && index < list->size; index++)
        aux = aux->next;
    if(!aux)
        return -1;
    return index;
}

void removeElement(List* list, ListElement* element) {
    if(!element || isEmpty(list)){
        puts("No existe el elemento");
        return;
    }
    if(element->next)
        element->next->prev = element->prev;
    if(element->prev)
        element->prev->next = element->next;
    free(element);
}

void removeAt(List* list, int index){
    if(index >= 0 && index < list->size && !isEmpty(list)) {
        puts("No existe el elemento");
        return;
    }
    ListElement* element = list->head;
    int i;
    for(i = 1; element && i < index; i++)
        element = element->next;
    if(!element){
        puts("No existe el elemento");
        return;
    }
    removeElement(list, element);
}

void printList(List* list){
    if(isEmpty(list)){
        puts("");
        return;
    }
    ListElement* element = list->head;
    while (element->next) {  
        printf("|%d\t|%d\t|\n", element->table_index, element->content);
        element = element->next;
    }
    printf("|%d\t|%d\t|\n", element->table_index, element->content);
}

void clearList(List* list) {
    if(!isEmpty(list)){
        ListElement* element;
        while ((element = list->head)) {
            list->head = list->head->next;
            free(element);
        }
        list->head = NULL;
        list->size = 0;
    }
}
