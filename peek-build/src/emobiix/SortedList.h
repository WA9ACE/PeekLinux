#ifndef _SORTEDLIST_H_
#define _SORTEDLIST_H_

typedef int (*SortedListComparitor)(void *, void *);

struct Sorted_List_t;
typedef struct SortedList_t SortedList;

struct SortedListIterator_t;
typedef struct SortedListIterator_t SortedListIterator;

SortedList *sortedList_new(SortedListComparitor);
void sortedList_append(SortedList *l, void *item);
void sortedList_delete(SortedList *l);
SortedListIterator *sortedList_begin(SortedList *l);
SortedListIterator *sortedList_end(SortedList *l);
void *sortedListIterator_item(SortedListIterator *iter);

#endif
