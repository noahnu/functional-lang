#ifndef _H_DATA_STRUCTURES_
#define _H_DATA_STRUCTURES_

typedef struct _linked_list_item {
    struct _linked_list_item   *prev;
    struct _linked_list_item   *next;
    void                       *value;
} LINKED_LIST_ITEM;

void linked_list_insert_after(LINKED_LIST_ITEM *list, LINKED_LIST_ITEM *element);
void linked_list_insert_before(LINKED_LIST_ITEM *list, LINKED_LIST_ITEM *element);
void linked_list_remove(LINKED_LIST_ITEM *element);
LINKED_LIST_ITEM* linked_list_shift(LINKED_LIST_ITEM **head_ptr);

#endif