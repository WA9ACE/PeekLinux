#ifndef _LIST_H_
#define _LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

struct List_t;
typedef struct List_t List;

struct ListNode_t;
typedef struct ListNode_t ListNode;

struct ListIterator_t {
    List *list;
    ListNode *node;
    int offset;
};
typedef struct ListIterator_t ListIterator;

typedef int (*ListComparitor)(void *arg1, void *arg2);

List *list_new(void);
void list_append(List *l, void *item);
void list_prepend(List *l, void *item);
void* list_firstItem(List *list);
void* list_lastItem(List *list);
void list_delete(List *l);
int list_size(List *l);
void list_debug(List *l);
void *list_find(List *l, void *obj, ListComparitor lc);
int list_findIter(List *l, void *obj, ListComparitor lc, ListIterator *iter);
void list_begin(List *l, ListIterator *iter);
void list_rbegin(List *l, ListIterator *iter);
int listIterator_finished(ListIterator *iter);
void *listIterator_item(ListIterator *iter);
void listIterator_remove(ListIterator *iter);
void listIterator_next(ListIterator *iter);
void listIterator_insertBefore(ListIterator *iter, void *obj);
void listIterator_insertAfter(ListIterator *iter, void *obj);

int ListEqualComparitor(void *arg1, void *arg2);

#ifdef __cplusplus
}
#endif

#endif /* _LIST_H_ */
