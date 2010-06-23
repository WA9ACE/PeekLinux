#include "List.h"

#include "Debug.h"

#include "p_malloc.h"

#include <stddef.h>

typedef struct ListNode_t ListNode;
struct ListNode_t {
    void *data;
    ListNode *prev, *next;
};

struct List_t {
    int length;
    ListNode *head, *tail;
};

struct ListIterator_t {
    List *list;
    ListNode *node;
    int offset;
};

List *list_new(void)
{
    List *output;

    output = (List *)p_malloc(sizeof(List));
    output->length = 0;
    output->head = NULL;
    output->tail = NULL;

    return output;
}

void list_append(List *l, void *item)
{
    ListNode *node;

    node = (ListNode *)p_malloc(sizeof(ListNode));
    if (l->tail != NULL)
        l->tail->next = node;
    else
        l->head = node;
    node->next = NULL;
    node->prev = l->tail;
    l->tail = node;
    node->data = item;
    ++l->length;
}

void list_prepend(List *l, void *item)
{
    ListNode *node;

    node = (ListNode *)p_malloc(sizeof(ListNode));
    if (l->head != NULL)
        l->head->prev = node;
    node->next = l->head;
    node->prev = NULL;
    l->head = node;
	if (l->tail == NULL)
		l->tail = node;
    node->data = item;
    ++l->length;
}

void list_delete(List *l)
{
    ListNode *node, *next;;

    node = l->head;
    while (node != NULL) {
        next = node->next;
        p_free(node);
        node = next;
    }

    p_free(l);
}

void *list_find(List *l, void *obj, ListComparitor lc)
{
	ListIterator *iter;
	void *output;

	iter = list_begin(l);
	while (!listIterator_finished(iter)) {
		output = listIterator_item(iter);
		if (lc(obj, output) == 0) {
			listIterator_delete(iter);
			return output;
		}
		listIterator_next(iter);
	}
	return NULL;
}

int list_size(List *l)
{
	return l->length;
}
void list_debug(List *l)
{
	ListIterator *iter;

	emo_printf("List<");
	for (iter = list_begin(l); !listIterator_finished(iter); listIterator_next(iter)) {
		emo_printf("%p, ", listIterator_item(iter));
	}
	listIterator_delete(iter);
	emo_printf(">" NL);
}

ListIterator *list_begin(List *l)
{
    ListIterator *output;

    output = (ListIterator *)p_malloc(sizeof(ListIterator));
    output->list = l;
    output->node = l->head;
    output->offset = (char *)&output->node->next - (char *)output->node;

    return output;
}

ListIterator *list_rbegin(List *l)
{
    ListIterator *output;

    output = (ListIterator *)p_malloc(sizeof(ListIterator));
    output->list = l;
    output->node = l->tail;
    output->offset = (char *)&output->node->prev - (char *)output->node;

    return output;
}

int listIterator_finished(ListIterator *iter)
{
    return iter->node == NULL;
}

void *listIterator_item(ListIterator *iter)
{
	if (iter->node == NULL)
		return NULL;
    return iter->node->data;
}

void listIterator_next(ListIterator *iter)
{
    iter->node = *(ListNode **)(((char *)(iter->node))+iter->offset);
}

void listIterator_delete(ListIterator *iter)
{
    p_free(iter);
}

void listIterator_remove(ListIterator *iter)
{
    ListNode *node;

    node = iter->node;
    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        iter->list->head = node->next;
    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        iter->list->tail = node->prev;

	iter->node = node->prev;

	--iter->list->length;
    p_free(node);
}

int ListEqualComparitor(void *arg1, void *arg2)
{
	if (arg1 == arg2)
		return 0;
	return 1;
}
