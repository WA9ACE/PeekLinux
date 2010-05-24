#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

struct HashTable_t;
typedef struct HashTable_t HashTable;

HashTable *hashtable_string(void);
HashTable *hashtable_int(void);
void hashtable_append(HashTable *ht, const void *key, void *data);
void *hashtable_find(HashTable *ht, const void *key);
void hashtable_delete(HashTable *ht);

#endif /* _HASHTABLE_H_ */
