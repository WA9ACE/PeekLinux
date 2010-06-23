#ifndef _DATAOBJECT_H_
#define _DATAOBJECT_H_

#include "DataObjectMap.h"
#include "URL.h"
#include "Map.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "DataObject_internal.h"

DataObjectField *dataobjectfield_string(const char *str);
DataObjectField *dataobjectfield_data(void *data, int bytes);
DataObjectField *dataobjectfield_int(int val);
DataObjectField *dataobjectfield_uint(unsigned int val);

typedef enum {DOS_INIT, DOS_SYNC, DOS_OK, DOS_ERROR} DataObjectState;
typedef enum {DOOP_CHANGE, DOOP_DESTROY} DataObjectOperation;

typedef void (*DataObjectSubscriptionCallback)(DataObject *dobj,
		DataObjectOperation doop, void *pararm);

DataObject *dataobject_new(void);
void dataobject_delete(DataObject *dobj);
DataObject *dataobject_newMap(DataObject *src, DataObjectMap *dmap);
void dataobject_setValue(DataObject *dobj, const char *key, DataObjectField *v);
DataObjectField *dataobject_getValue(DataObject *obj, const char *key);
DataObjectField *dataobject_getValueAsInt(DataObject *dobj, const char *key);
void dataobject_setRecordType(DataObject *dobj, int isRecord);
int dataobject_getRecordType(DataObject *dobj);
void dataobject_appendRecord(DataObject *dobj, DataObject *robj);
int dataobject_getChildCount(DataObject *dobj);
int dataobject_isLocal(DataObject *dobj);
DataObjectState dataobject_getState(DataObject *dobj);
void dataobject_setState(DataObject *dobj, DataObjectState state);
void dataobject_getStamp(DataObject *dobj, unsigned int *stampMinor,
		unsigned int *stampMajor);
void dataobject_setStamp(DataObject *dobj, unsigned int stampMinor,
		unsigned int stampMajor);
MapIterator *dataobject_fieldIterator(DataObject *dobj);
ListIterator *dataobject_childIterator(DataObject *dobj);
DataObject *dataobject_getTree(DataObject *dobj, int index);
int dataobject_getTreeNextOp(DataObject *dobj, int *ischild);
int dataobject_treeIndex(DataObject *dobj);
void dataobject_pack(DataObject *parent, DataObject *child);
void dataobject_packStart(DataObject *parent, DataObject *child);

DataObject *dataobject_parent(DataObject *dobj);
void dataobject_setParent(DataObject *dobj, DataObject *parent);
DataObject *dataobject_superparent(DataObject *dobj);
DataObject *dataobject_findByName(DataObject *root, const char *name);

void dataobject_setScriptContext(DataObject *dobj, void *ctx);
void *dataobject_getScriptContext(DataObject *dobj);
void *dataobject_findScriptContext(DataObject *dobj);

DataObject *dataobject_findFieldParent(DataObject *dobj, const char *str);

int dataobject_isDirty(DataObject *dobj);
void dataobject_setDirty(DataObject *dobj);
void dataobject_setClean(DataObject *dobj);

#define LAYOUT_DIRTY_WIDTH		0x02
#define LAYOUT_DIRTY_HEIGHT		0x04
int dataobject_isLayoutDirty(DataObject *dobj, unsigned int wh);
void dataobject_setLayoutDirty(DataObject *dobj, unsigned int wh);
void dataobject_setLayoutClean(DataObject *dobj, unsigned int wh);
void dataobject_setLayoutDirtyAll(DataObject *dobj);

void dataobject_debugPrint(DataObject *dobj);

/* Global DataObject table functions */
void dataobject_platformInit(void);
DataObject *dataobject_locateStr(const char *str);
DataObject *dataobject_locate(URL *url);
DataObject *dataobject_construct(URL *url, int isLocal);
void dataobject_resolveReferences(DataObject *dobj);

#ifdef __cplusplus
}
#endif

#endif /* _DATAOBJECT_H_ */
