#include "List.h"

#include "Debug.h"

#include "p_malloc.h"

#include <stddef.h>

struct ListNode_t {
    void *data;
    ListNode *prev, *next;
};

struct List_t {
    int length;
    ListNode *head, *tail;
};

List *list_new(void)
{
    List *output;

    output = (List *)p_malloc(sizeof(List));
    if (output == NULL)
        return NULL;
    output->length = 0;
    output->head = NULL;
    output->tail = NULL;

    return output;
}

void list_append(List *l, void *item)
{
    ListNode *node;

	EMO_ASSERT(l != NULL, "list append to NULL list")

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

	EMO_ASSERT(l != NULL, "list prepend to NULL list")

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
    ListNode *node, *next;

	EMO_ASSERT(l != NULL, "deleting NULL list")

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
	ListIterator iter;
	int result;

	EMO_ASSERT_NULL(l != NULL, "list find on NULL list")
	EMO_ASSERT_NULL(lc != NULL, "list find missing comparitor")

	result = list_findIter(l, obj, lc, &iter);
	if (!result)
		return NULL;

	return listIterator_item(&iter);
}

int list_findIter(List *l, void *obj, ListComparitor lc, ListIterator *iter)
{
	void *output;

	EMO_ASSERT_INT(l != NULL, 0, "list findIter on NULL list")
	EMO_ASSERT_INT(lc != NULL, 0, "list findIter missing comparitor")
	EMO_ASSERT_INT(iter != NULL, 0, "list findIter missing iterator")

	list_begin(l, iter);
	while (!listIterator_finished(iter)) {
		output = listIterator_item(iter);
		if (lc(obj, output) == 0) {
			/*listIterator_delete(iter);*/
			return 1;
		}
		listIterator_next(iter);
	}
	return 0;
}

int list_size(List *l)
{
	EMO_ASSERT_INT(l != NULL, 0, "list size on NULL list")

	return l->length;
}
void list_debug(List *l)
{
	ListIterator iter;

	EMO_ASSERT(l != NULL, "list debug on NULL list")

	emo_printf("List<");
	for (list_begin(l, &iter); !listIterator_finished(&iter); listIterator_next(&iter)) {
		emo_printf("%p, ", listIterator_item(&iter));
	}
	/*listIterator_delete(iter);*/
	emo_printf(">" NL);
}

void list_begin(List *l, ListIterator *output)
{
/*    ListIterator *output;

    output = (ListIterator *)p_malloc(sizeof(ListIterator));*/

	EMO_ASSERT(l != NULL, "list begin on NULL list")
	EMO_ASSERT(output != NULL, "list begin missing iterator")
    
		output->list = l;
    output->node = l->head;
    output->offset = (char *)&output->node->next - (char *)output->node;

    /*return output;*/
}

void list_rbegin(List *l, ListIterator *output)
{
/*    ListIterator *output;

    output = (ListIterator *)p_malloc(sizeof(ListIterator));*/
	
	EMO_ASSERT(l != NULL, "list rbegin on NULL list")
	EMO_ASSERT(output != NULL, "list rbegin missing iterator")

    output->list = l;
    output->node = l->tail;
    output->offset = (char *)&output->node->prev - (char *)output->node;

    /*return output;*/
}

int listIterator_finished(ListIterator *iter)
{
	EMO_ASSERT_INT(iter != NULL, 1, "list finished missing iterator")

    return iter->node == NULL;
}

void *listIterator_item(ListIterator *iter)
{
	EMO_ASSERT_NULL(iter != NULL, "list item missing iterator")

	if (iter->node == NULL)
		return NULL;
    return iter->node->data;
}

void listIterator_next(ListIterator *iter)
{
	EMO_ASSERT(iter != NULL, "list next missing iterator")

	if (iter->node)
    	iter->node = *(ListNode **)(((char *)(iter->node))+iter->offset);
}

void listIterator_remove(ListIterator *iter)
{
    ListNode *node;

	EMO_ASSERT(iter != NULL, "list remove missing iterator")

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

void listIterator_insertBefore(ListIterator *iter, void *obj)
{
	ListNode *node;

	EMO_ASSERT(iter != NULL, "Insert before null iter");
	EMO_ASSERT(iter->node != NULL, "Insert before null iter node");
	EMO_ASSERT(iter->list != NULL, "Insert before null iter list");

	node = (ListNode *)p_malloc(sizeof(ListNode));
	node->data = obj;
	node->prev = iter->node->prev;
	node->next = iter->node;
	iter->node->prev = node;

	iter->list->length++;
	if (!node->prev)
		iter->list->head = node;
}

void listIterator_insertAfter(ListIterator *iter, void *obj)
{
	ListNode *node;

	EMO_ASSERT(iter != NULL, "Insert after null iter");
	EMO_ASSERT(iter->node != NULL, "Insert after null iter node");
	EMO_ASSERT(iter->list != NULL, "Insert after null iter list");

	node = (ListNode *)p_malloc(sizeof(ListNode));
	node->data = obj;
	node->prev = iter->node;
	node->next = iter->node->next;
	iter->node->next = node;

	iter->list->length++;
	if (!node->next)
		iter->list->tail = node;
}

void* list_firstItem(List *list)
{
	EMO_ASSERT_NULL(list != NULL, "First item null list");
	
	if (list->head)
		return list->head->data;

	return NULL;
}

void* list_lastItem(List *list)
{
	EMO_ASSERT_NULL(list != NULL, "Last item null list");
	
	if (list->tail)
		return list->tail->data;

	return NULL;
}

int ListEqualComparitor(void *arg1, void *arg2)
{
	if (arg1 == arg2)
		return 0;
	return 1;
}
