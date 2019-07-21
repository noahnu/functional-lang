#include <stdint.h>

#ifndef _H_LINKED_LIST_
#define _H_LINKED_LIST_

typedef struct _linked_list_item {
    void                        *data;
    struct _linked_list_item    *next;
} LINKED_LIST_ITEM;

typedef struct _linked_list {
    LINKED_LIST_ITEM    *head;
    uint32_t            length;
} LINKED_LIST;

LINKED_LIST *linked_list_init();
LINKED_LIST_ITEM *linked_list_init_item(void *data_ptr);
void linked_list_prepend(LINKED_LIST *list, void *data_ptr);
void *linked_list_pop(LINKED_LIST *list);

#endif