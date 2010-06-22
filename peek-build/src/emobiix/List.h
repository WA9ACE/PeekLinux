#ifndef _LIST_H_
#define _LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

struct List_t;
typedef struct List_t List;

struct ListIterator_t;
typedef struct ListIterator_t ListIterator;

typedef int (*ListComparitor)(void *arg1, void *arg2);

List *list_new(void);
void list_append(List *l, void *item);
void list_prepend(List *l, void *item);
void list_delete(List *l);
void *list_find(List *l, void *obj, ListComparitor lc);
int list_size(List *l);
void list_debug(List *l);
ListIterator *list_begin(List *l);
ListIterator *list_rbegin(List *l);
int listIterator_finished(ListIterator *iter);
void *listIterator_item(ListIterator *iter);
void listIterator_remove(ListIterator *iter);
void listIterator_next(ListIterator *iter);
void listIterator_delete(ListIterator *iter);

int ListEqualComparitor(void *arg1, void *arg2);

#ifdef __cplusplus
}
#endif

#endif /* _LIST_H_ */
