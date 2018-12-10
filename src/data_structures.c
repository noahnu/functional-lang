#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "./data_structures.h"

void linked_list_insert_after(LINKED_LIST_ITEM *list, LINKED_LIST_ITEM *element)
{
    element->next = list->next;
    element->prev = list;
    list->next = element;
}

void linked_list_insert_before(LINKED_LIST_ITEM *list, LINKED_LIST_ITEM *element)
{
    element->prev = list->prev;
    list->prev = element;
    element->next = list;
}

void linked_list_remove(LINKED_LIST_ITEM *element)
{
    LINKED_LIST_ITEM *prev = element->prev;
    LINKED_LIST_ITEM *next = element->next;

    if (prev != NULL) prev->next = next;
    if (next != NULL) next->prev = prev;
    element->prev = NULL;
    element->next = NULL;
}

LINKED_LIST_ITEM* linked_list_shift(LINKED_LIST_ITEM **head_ptr)
{
    LINKED_LIST_ITEM *prev_head = *head_ptr;
    if (prev_head == NULL) return NULL;
    LINKED_LIST_ITEM *new_head = prev_head->next;
    linked_list_remove(prev_head);
    *head_ptr = new_head;
    return prev_head;
}