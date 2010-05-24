#ifndef _STATIC_LIST_H_
#define _STATIC_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

struct StaticList_t {
    const char *key;
    void *data;
};
typedef struct StaticList_t StaticList;

struct StaticListIterator_t;
typedef struct StaticListIterator_t StaticListIterator;

void *staticlist_find(StaticList *l, const char *key);
StaticListIterator *staticlist_begin(StaticList *l);
int staticlistIterator_finished(StaticListIterator *iter);
void *staticlistIterator_item(StaticListIterator *iter);
void statislistIterator_next(StaticListIterator **iter);
void staticlistIterator_delete(StaticListIterator *iter);

#ifdef __cplusplus
}
#endif

#endif /* _STATIC_LIST_H_ */
