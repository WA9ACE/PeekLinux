#include "DataObject.h"
#include "Subscription.h"

#include "Map.h"
#include "List.h"
#include "Debug.h"

#include "p_malloc.h"

#include "DataObject_internal.h"
#include "DataObject_private.h"

#include <string.h>

DataObject *dataobject_new(void)
{
	DataObject *output;

	output = (DataObject *)p_malloc(sizeof(DataObject));
	if (!output)
	{
		emo_printf("EMOBIIX: p_malloc(DataObject) returned NULL" NL);
		return NULL;
	}
	output->data = map_string();
	output->children = list_new();
	output->parent = NULL;
	output->widgetData = NULL;
	output->state = DOS_INIT;
	rectangle_zero(&output->margin);
	output->margin.x = 0;
	output->margin.y = 0;
	output->margin.width = 0;
	output->margin.height = 0;
	rectangle_zero(&output->box);
    output->scriptContext = NULL;
	output->flags1 = 0;

	return output;
}

DataObject *dataobject_newMap(DataObject *src, DataObjectMap *dmap)
{
	DataObject *output;

	output = dataobject_new();
	output->mapSubscription = subscription_new(src, dmap);
	
	return output;
}

void dataobject_setValue(DataObject *dobj, const char *key, DataObjectField *v)
{
	map_append(dobj->data, key, v);
}

DataObjectField *dataobject_getValue(DataObject *dobj, const char *key)
{
	void *output;

	output = map_find(dobj->data, key);

	return (DataObjectField *)output;
}

DataObjectField *dataobject_getValueAsInt(DataObject *dobj, const char *key)
{
	DataObjectField *output;
	char *str;

	output = dataobject_getValue(dobj, key);
	if (output == NULL)
		return output;

	if (output->type == DOF_INT)
		return output;

	if (output->type == DOF_STRING) {
		str = output->field.string;
		output->field.integer = atoi(str);
		output->type = DOF_INT;
		p_free(str);
		return output;
	}

	return NULL;
}

int dataobject_isLocal(DataObject *dobj)
{
	return dobj->isLocal;
}

DataObjectState dataobject_getState(DataObject *dobj)
{
	return dobj->state;
}

void dataobject_setState(DataObject *dobj, DataObjectState state)
{
	dobj->state = state;
}

void dataobject_getStamp(DataObject *dobj, unsigned int *stampMinor,
		unsigned int *stampMajor)
{
	*stampMinor = dobj->stampMinor;
	*stampMajor = dobj->stampMajor;
}

void dataobject_setStamp(DataObject *dobj, unsigned int stampMinor,
		unsigned int stampMajor)
{
	dobj->stampMinor = stampMinor;
	dobj->stampMajor = stampMajor;
}

MapIterator *dataobject_fieldIterator(DataObject *dobj)
{
	return map_begin(dobj->data);
}

ListIterator *dataobject_childIterator(DataObject *dobj)
{
	return list_begin(dobj->children);
}

static DataObject *dataobject_getTreeR(DataObject *dobj, int *index)
{
	ListIterator *iter;
	DataObject *output;

	/*fprintf(stderr, "GetTree - %d\n", *index);*/

	if (*index == 0)
		return dobj;

	iter = list_begin(dobj->children);
	while (!listIterator_finished(iter)) {
		*index = *index - 1;
		output = dataobject_getTreeR((DataObject *)listIterator_item(iter), index);
		if (output != NULL) {
			listIterator_delete(iter);
			return output;
		}
		listIterator_next(iter);
	}
	listIterator_delete(iter);
	return NULL;
}

DataObject *dataobject_getTree(DataObject *dobj, int index)
{
	int idx;

	idx = index;
	return dataobject_getTreeR(dobj, &idx);
}

int dataobject_getTreeNextOp(DataObject *dobj, int *ischild)
{
	ListIterator *iter;
	DataObject *parent;
	DataObject *last;

	/* check for add child */
	iter = list_begin(dobj->children);
	if (!listIterator_finished(iter)) {
		*ischild = -1;
		listIterator_delete(iter);
		return 1;
	}
	listIterator_delete(iter);

	/* check for root since we have no children */
	if (dobj->parent == NULL) {
		return 0;
	}

	parent = dobj->parent;
	last = dobj;

	/* search for the child in parent */
next_iteration:
	iter = list_begin(parent->children);
	while (!listIterator_finished(iter)) {
		if (last == (DataObject *)listIterator_item(iter)) {
			listIterator_next(iter);
			if (!listIterator_finished(iter)) {
				*ischild = dataobject_treeIndex(parent);
				listIterator_delete(iter);
				return 1;
			} else {
				parent = parent->parent;
				last = last->parent;
				listIterator_delete(iter);
				if (parent == NULL)
					return 0;
				goto next_iteration;
			}
		}
		listIterator_next(iter);
	}
	listIterator_delete(iter);

	return 0;
}

static int dataobject_treeIndexR(DataObject *parent, DataObject *dobj, int *index)
{
	ListIterator *iter;
	int output;

	/*fprintf(stderr, "TreeIndex - %d\n", *index);*/

	if (parent == dobj)
		return *index;

	iter = list_begin(parent->children);
	while (!listIterator_finished(iter)) {
		*index += 1;
		output = dataobject_treeIndexR((DataObject *)listIterator_item(iter), dobj, index);
		if (output >= 0) {
			listIterator_delete(iter);
			return output;
		}
		listIterator_next(iter);
	}
	listIterator_delete(iter);
	return -1;
}

int dataobject_treeIndex(DataObject *dobj)
{
	DataObject *superparent;
	int index;

	superparent = dobj;
	while (superparent->parent != NULL) {
		superparent = superparent->parent;
	}

	if (superparent == dobj)
		return 0;
	index = 0;
	return dataobject_treeIndexR(superparent, dobj, &index);
}

void dataobject_pack(DataObject *parent, DataObject *child)
{
	child->parent = parent;
	list_append(parent->children, child);
}

void dataobject_packStart(DataObject *parent, DataObject *child)
{
	child->parent = parent;
	list_prepend(parent->children, child);
}

DataObject *dataobject_parent(DataObject *dobj)
{
	return dobj->parent;
}

DataObject *dataobject_superparent(DataObject *dobj)
{
	if (dobj == NULL)
		return NULL;
	while (dobj->parent != NULL)
		dobj = dobj->parent;

	return dobj;
}

DataObject *dataobject_findByName(DataObject *dobj, const char *name)
{
	DataObjectField *field;
	ListIterator *iter;
	DataObject *child;

	field = dataobject_getValue(dobj, "name");
	if (field != NULL && field->type == DOF_STRING) {
		if (strcmp(name, field->field.string) == 0)
			return dobj;
	}

	if (list_size(dobj->children) == 0)
		return NULL;

	iter = list_begin(dobj->children);
	while (!listIterator_finished(iter)) {
		child = listIterator_item(iter);
		child = dataobject_findByName(child, name);
		if (child != NULL) {
			listIterator_delete(iter);
			return child;
		}
		listIterator_next(iter);
	}

	listIterator_delete(iter);
	return NULL;
}

int dataobject_isDirty(DataObject *dobj)
{
	return dobj->flags1 & DO_FLAG_DIRTY;
}

void dataobject_setDirty(DataObject *dobj)
{
	dobj->flags1 |= DO_FLAG_DIRTY;
}

void dataobject_setClean(DataObject *dobj)
{
	dobj->flags1 &= ~DO_FLAG_DIRTY;
}

int dataobject_isLayoutDirty(DataObject *dobj, unsigned int wh)
{
	return dobj->flags1 & wh;
}

void dataobject_setLayoutDirty(DataObject *dobj, unsigned int wh)
{
	dobj->flags1 |= wh;
}

void dataobject_setLayoutDirtyAll(DataObject *dobj)
{
	ListIterator *iter;

	dobj->flags1 |= DO_FLAG_LAYOUT_DIRTY_WIDTH;
	dobj->flags1 |= DO_FLAG_LAYOUT_DIRTY_HEIGHT;

	iter = list_begin(dobj->children);
	while (!listIterator_finished(iter)) {
		dataobject_setLayoutDirtyAll(
				(DataObject *)listIterator_item(iter));
		listIterator_next(iter);
	}
	listIterator_delete(iter);
}

void dataobject_setLayoutClean(DataObject *dobj, unsigned int wh)
{
	dobj->flags1 &= ~wh;
}


static void dataobject_debugPrintR(DataObject *dobj, int level)
{
	MapIterator *iter;
	ListIterator *citer;
	DataObjectField *dof;
	const char *key;
	int i;
	
	for (i = 0; i < level; ++i)
		emo_printf("    ");

	emo_printf("DataObject<wh(%d, %d, %d, %d), margin(%d, %d)",
			dobj->box.width, dobj->box.height,
			dobj->box.x, dobj->box.y,
			dobj->margin.x, dobj->margin.y);
	if (dobj == NULL) {
		emo_printf("NULL>" NL);
		return;
	}

	i = 0;
	iter = map_begin(dobj->data);
	while (!mapIterator_finished(iter)) {
		if (i != 0)
			emo_printf(", ");
		dof = (DataObjectField *)mapIterator_item(iter, (void **)&key);
		switch (dof->type) {
			case DOF_STRING:
				emo_printf("%s=\"%s\"", key, dof->field.string);
				break;
			case DOF_INT:
				emo_printf("%s=\"%d\"", key, dof->field.integer);
				break;
			case DOF_UINT:
				emo_printf("%s=\"%d\"", key, dof->field.uinteger);
				break;
			case DOF_DATA:
				emo_printf("%s=\"DATA-%d bytes\"", key, dof->field.data.size);
				break;
		}
		mapIterator_next(iter);
		++i;
	}
	emo_printf(">" NL);

	citer = list_begin(dobj->children);
	while (!listIterator_finished(citer)) {
		dataobject_debugPrintR((DataObject *)listIterator_item(citer), level+1);
		listIterator_next(citer);
	}
}


void dataobject_setScriptContext(DataObject *dobj, void *ctx)
{
    dobj->scriptContext = ctx;
}

void *dataobject_getScriptContext(DataObject *dobj)
{
    return dobj->scriptContext;
}

void *dataobject_findScriptContext(DataObject *dobj)
{
    void *ctx;

    while (dobj->parent != NULL) {
        ctx = dataobject_getScriptContext(dobj);
        if (ctx != NULL)
            return ctx;
        dobj = dobj->parent;
    }

    return dataobject_getScriptContext(dobj);
}

DataObject *dataobject_findFieldParent(DataObject *dobj, const char *str)
{
    DataObjectField *field;

    field = dataobject_getValue(dobj, str);
    if (field != NULL)
        return dobj;

    if (dobj->parent == NULL)
        return NULL;

    return dataobject_findFieldParent(dobj->parent, str);
}

void dataobject_debugPrint(DataObject *dobj)
{
	dataobject_debugPrintR(dobj, 0);
}

/*
 * Global map of data objects
 */
#include "HashTable.h"

static HashTable *globalDObjs = NULL;

void dataobject_platformInit(void)
{
	if (globalDObjs != NULL)
		return;

	globalDObjs = hashtable_string();
}

DataObject *dataobject_locateStr(const char *str)
{
	DataObject *output;

	if (globalDObjs == NULL)
		dataobject_platformInit();
	output = hashtable_find(globalDObjs, str);
	return output;
}

DataObject *dataobject_locate(URL *url)
{
	DataObject *output;
	output = dataobject_locateStr(url->all);
	if (output != NULL)
		return output;
	return dataobject_locateStr(url->path);
}

DataObject *dataobject_construct(URL *url, int isLocal)
{
	DataObject *output;

	output = dataobject_new();
	if (output == NULL)
		return NULL;
	output->isLocal = isLocal;
	if (isLocal)
		hashtable_append(globalDObjs, url->path, output);
	else
		hashtable_append(globalDObjs, url->all, output);

	return output;
}

DataObjectField *dataobjectfield_string(const char *str)
{
	DataObjectField *output;

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_STRING;
	output->field.string = p_strdup(str);

	return output;
}

DataObjectField *dataobjectfield_data(void *data, int size)
{
	DataObjectField *output;

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_DATA;
	output->field.data.bytes = data;
	output->field.data.size = size;

	return output;
}

DataObjectField *dataobjectfield_int(int val)
{
	DataObjectField *output;

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_INT;
	output->field.integer = val;

	return output;
}

DataObjectField *dataobjectfield_uint(unsigned int val)
{
	DataObjectField *output;

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_UINT;
	output->field.uinteger = val;

	return output;
}
