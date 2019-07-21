#include <stdlib.h>
#include <stdio.h>

#include "./linked_list.h"

LINKED_LIST *linked_list_init() {
    LINKED_LIST *list = malloc(sizeof(LINKED_LIST));
    list->head = NULL;
    list->length = 0;
    return list;
}

LINKED_LIST_ITEM *linked_list_init_item(void *data_ptr) {
    LINKED_LIST_ITEM *item = malloc(sizeof(LINKED_LIST_ITEM));
    item->data = data_ptr;
    item->next = NULL;
    return item;
}

void linked_list_prepend(LINKED_LIST *list, void *data_ptr) {
    LINKED_LIST_ITEM *item = linked_list_init_item(data_ptr);
    item->next = list->head;
    list->head = item;
    list->length++;
    // fprintf(stderr, "Item: %p, New Length: %d\n", data_ptr, list->length);
}

void *linked_list_pop(LINKED_LIST *list) {
    void *ptr = NULL;
    LINKED_LIST_ITEM *item = list->head;
    if (item) {
        ptr = item->data;

        if (ptr == NULL) {
            fprintf(stderr, "NULL value on stack.\n");
        }

        list->head = list->head->next;
        list->length--;
        free(item);
    }
    return ptr;
}