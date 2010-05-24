#include "HashTable.h"
#include "Debug.h"
#include "p_malloc.h"

#include "uthash.h"

#undef uthash_malloc
#undef uthash_free

#define uthash_malloc(sz) p_malloc(sz)
#define uthash_free(ptr, size) p_free(ptr)

#undef uthash_fatal
#define uthash_fatal(msg) emo_printf("HashTable: %s\n", msg)

struct HashTableNode_t {
    union {
        char *kstring;
        int number;
        void *pointer;
    } key;
    void *data;
    UT_hash_handle hh;
};
typedef struct HashTableNode_t HashTableNode;

typedef enum {STRING, NUMBER} HashTableType;
struct HashTable_t {
    HashTableType type;
    HashTableNode *table;
};

HashTable *hashtable_new(HashTableType htt)
{
    HashTable *output;

    output = (HashTable *)p_malloc(sizeof(HashTable));
    output->table = NULL;
    output->type = htt;

    return output;
}

HashTable *hashtable_string(void)
{
    return hashtable_new(STRING);
}

HashTable *hashtable_int(void)
{
    return hashtable_new(NUMBER);
}

void hashtable_append(HashTable *ht, const void *key, void *data)
{
    HashTableNode *node;
    int kstrlen;

    node = (HashTableNode *)p_malloc(sizeof(HashTableNode));
    node->data = data;

    switch (ht->type) {
        case STRING:
            node->key.kstring = p_strdup((const char *)key);
            kstrlen = strlen(node->key.kstring);
            HASH_ADD_KEYPTR(hh, ht->table, node->key.kstring, kstrlen, node);
            break;
        case NUMBER:
        default:
            node->key.number = (int)key;
            HASH_ADD_INT(ht->table, key.number, node);
            break;
    }
}

void *hashtable_find(HashTable *ht, const void *key)
{
    HashTableNode *output;

    switch (ht->type) {
        case STRING:
            HASH_FIND_STR(ht->table, key, output);
            break;
        case NUMBER:
        default:
            HASH_FIND_INT(ht->table, &key, output);
            break;
    }

    if (output == NULL)
        return NULL;
    return output->data;
}

void hashtable_delete(HashTable *ht)
{
    HASH_CLEAR(hh, ht->table);
    p_free(ht);
}
