#include "DataObject.h"
#include "Subscription.h"

#include "Map.h"
#include "List.h"
#include "Debug.h"
#include "ConnectionContext.h"
#include "Application.h"
#include "ApplicationManager.h"

#include "p_malloc.h"

#include "DataObject_internal.h"
#include "DataObject_private.h"

#include <string.h>

void dataobjectfield_free(DataObjectField *f)
{
	switch (f->type) {
		case DOF_STRING:
			if (f->field.string != NULL)
			p_free(f->field.string);
			break;
		case DOF_DATA:
			if (f->field.data.bytes != NULL)
				p_free(f->field.data.bytes);
			break;
		default:
			break;
	}
	p_free(f);
}

void dataobjectfield_setIsDerived(DataObjectField *field, int isderived)
{
	if (isderived)
		field->flags |= DOFF_DERIVED;
	else
		field->flags &= ~DOFF_DERIVED;
}

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
	output->packing = WP_VERTICAL;

	return output;
}

DataObject *dataobject_copy(DataObject *dobj)
{
	DataObject *output;
	DataObjectField *field;
	char *key;
	MapIterator iter;

	output = dataobject_new();
	output->box = dobj->box;
	output->flags1 = dobj->flags1;
	output->isLocal = dobj->isLocal;
	output->margin = dobj->margin;
	output->packing = dobj->packing;
	output->position = dobj->position;
	output->scriptContext = dobj->scriptContext;
	output->stampMajor = dobj->stampMajor;
	output->stampMinor = dobj->stampMinor;
	output->state = dobj->state;
	output->widgetData = dobj->widgetData;

	for (map_begin(dobj->data, &iter); !mapIterator_finished(&iter);
			mapIterator_next(&iter)) {
		field = dataobjectfield_copy((DataObjectField *)mapIterator_item(&iter, (void **)&key));
		dataobject_setValue(dobj, key, field);
	}

	return output;
}

DataObject *dataobject_copyTree(DataObject *dobj)
{
	DataObject *output;
	ListIterator iter;

	output = dataobject_copy(dobj);

	for (list_begin(dobj->children, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		list_append(output->children,
				dataobject_copy((DataObject *)listIterator_item(&iter)));
	}

	return output;
}

void dataobject_delete(DataObject *dobj)
{
	MapIterator iter;
	DataObjectField *item;
	char *key;
	
	do {
		map_begin(dobj->data, &iter);
		if (mapIterator_finished(&iter))
			break;
		item = (DataObjectField *)mapIterator_item(&iter, (void **)&key);
		dataobjectfield_free(item);
		mapIterator_remove(&iter);
		/*mapIterator_delete(iter);*/
	} while (1);

	/*
	do {
		list_begin(dobj->children, &liter);
		if (listIterator_finished(&liter))
			break;
		listIterator_remove(&liter);
	} while (1);*/

	list_delete(dobj->children);
	map_delete(dobj->data);
	p_free(dobj);
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
	DataObjectField *old;
	old = (DataObjectField *)map_find(dobj->data, key);
	if (old != NULL) {
		map_remove(dobj->data, key);
		dataobjectfield_free(old);
	}
	if (key != NULL && key[0] == '_') {
		v->flags |= DOFF_ARRAYSOURCE;
		++key;	
	}
	map_append(dobj->data, key, v);
}

DataObjectField *dataobject_getValue(DataObject *dobj, const char *key)
{
	DataObjectField *output;
	DataObject *child;

	output = (DataObjectField *)map_find(dobj->data, key);
	if (output != NULL && output->type == DOF_STRING) {
		if (output->flags & DOFF_ARRAYSOURCE) {
			child = widget_getDataObject(dobj);
			output = dataobject_getValue(child, output->field.string);
		}
	}

	return output;
}

DataObjectField *dataobject_getValueAsInt(DataObject *dobj, const char *key)
{
	DataObjectField *output;
	char *str;

	output = dataobject_getValue(dobj, key);
	if (output == NULL)
		return output;

	if (output->type == DOF_INT || output->type == DOF_UINT)
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

void dataobject_setRecordType(DataObject *dobj, int isRecord)
{
	if (isRecord)
		dobj->flags1 |= DO_FLAG_RECORD_TYPE;
	else
		dobj->flags1 &= ~DO_FLAG_RECORD_TYPE;
}

int dataobject_getRecordType(DataObject *dobj)
{
	return dobj->flags1 & DO_FLAG_RECORD_TYPE;
}

void dataobject_appendRecord(DataObject *dobj, DataObject *robj)
{
	list_append(dobj->children, robj);
}

int dataobject_getChildCount(DataObject *dobj)
{
	return list_size(dobj->children);
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

void dataobject_fieldIterator(DataObject *dobj, MapIterator *iter)
{
	map_begin(dobj->data, iter);
}

void dataobject_childIterator(DataObject *dobj, ListIterator *iter)
{
	list_begin(dobj->children, iter);
}

static DataObject *dataobject_getTreeR(DataObject *dobj, int *index)
{
	ListIterator iter;
	DataObject *output;

	/*fprintf(stderr, "GetTree - %d\n", *index);*/

	if (*index == 0)
		return dobj;

	list_begin(dobj->children, &iter);
	while (!listIterator_finished(&iter)) {
		*index = *index - 1;
		output = dataobject_getTreeR((DataObject *)listIterator_item(&iter), index);
		if (output != NULL) {
			/*listIterator_delete(iter);*/
			return output;
		}
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/
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
	ListIterator iter;
	DataObject *parent;
	DataObject *last;

	/* check for add child */
	list_begin(dobj->children, &iter);
	if (!listIterator_finished(&iter)) {
		*ischild = -1;
		/*listIterator_delete(iter);*/
		return 1;
	}
	/*listIterator_deleteiter);*/

	/* check for root since we have no children */
	if (dobj->parent == NULL) {
		return 0;
	}

	parent = dobj->parent;
	last = dobj;

	/* search for the child in parent */
next_iteration:
	list_begin(parent->children, &iter);
	while (!listIterator_finished(&iter)) {
		if (last == (DataObject *)listIterator_item(&iter)) {
			listIterator_next(&iter);
			if (!listIterator_finished(&iter)) {
				*ischild = dataobject_treeIndex(parent);
				/*listIterator_delete(iter);*/
				return 1;
			} else {
				parent = parent->parent;
				last = last->parent;
				/*listIterator_delete(iter);*/
				if (parent == NULL)
					return 0;
				goto next_iteration;
			}
		}
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/

	return 0;
}

static int dataobject_treeIndexR(DataObject *parent, DataObject *dobj, int *index)
{
	ListIterator iter;
	int output;

	/*fprintf(stderr, "TreeIndex - %d\n", *index);*/

	if (parent == dobj)
		return *index;

	list_begin(parent->children, &iter);
	while (!listIterator_finished(&iter)) {
		*index += 1;
		output = dataobject_treeIndexR((DataObject *)listIterator_item(&iter), dobj, index);
		if (output >= 0) {
			/*listIterator_delete(iter);*/
			return output;
		}
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/
	return -1;
}

int dataobject_treeIndex(DataObject *dobj)
{
	DataObject *superparent;
	int index;

	/*dataobject_debugPrint(dobj);*/

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

void dataobject_setParent(DataObject *dobj, DataObject *parent)
{
	dobj->parent = dobj;
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
	ListIterator iter;
	DataObject *child;

	field = dataobject_getValue(dobj, "name");
	if (field != NULL && field->type == DOF_STRING) {
		if (strcmp(name, field->field.string) == 0)
			return dobj;
	}

	if (list_size(dobj->children) == 0)
		return NULL;

	list_begin(dobj->children, &iter);
	while (!listIterator_finished(&iter)) {
		child = listIterator_item(&iter);
		child = dataobject_findByName(child, name);
		if (child != NULL) {
			/*listIterator_delete(iter);*/
			return child;
		}
		listIterator_next(&iter);
	}

	/*listIterator_delete(iter);*/
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
	ListIterator iter;
	DataObjectField *type;
	DataObject *child;
	Application *app;

	dobj->flags1 |= DO_FLAG_LAYOUT_DIRTY_WIDTH;
	dobj->flags1 |= DO_FLAG_LAYOUT_DIRTY_HEIGHT;

	type = dataobject_getValue(dobj, "type");
	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(dobj);
		if (child != NULL) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL)
					dataobject_setLayoutDirtyAll(child);
			}
		}
	} else {
		list_begin(dobj->children, &iter);
		while (!listIterator_finished(&iter)) {
			dataobject_setLayoutDirtyAll(
					(DataObject *)listIterator_item(&iter));
			listIterator_next(&iter);
		}
	}
}

void dataobject_setLayoutClean(DataObject *dobj, unsigned int wh)
{
	dobj->flags1 &= ~wh;
}


static void dataobject_debugPrintR(DataObject *dobj, int level)
{
	MapIterator iter;
	ListIterator citer;
	DataObjectField *dof;
	const char *key;
	int i;
	DataObject *child;
	Application *app;
	
	for (i = 0; i < level; ++i)
		emo_printf("    ");

	if (dataobject_getRecordType(dobj)) {
		emo_printf("RecordObject<%p, ", dobj);
	} else {
		emo_printf("DataObject<%p, wh(%d, %d, %d, %d), margin(%d, %d)",
				dobj, dobj->box.width, dobj->box.height,
				dobj->box.x, dobj->box.y,
				dobj->margin.x, dobj->margin.y);
	}
	if (dobj == NULL) {
		emo_printf("NULL>" NL);
		return;
	}

	i = 0;
	map_begin(dobj->data, &iter);
	while (!mapIterator_finished(&iter)) {
		if (i != 0)
			emo_printf(", ");
		dof = (DataObjectField *)mapIterator_item(&iter, (void **)&key);
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
		mapIterator_next(&iter);
		++i;
	}
	emo_printf(">" NL);

	dof = dataobject_getValue(dobj, "type");
	if (dataobjectfield_isString(dof, "frame")) {
		child = widget_getDataObject(dobj);
		if (child != NULL) {
			app = manager_applicationForDataObject(child);
			child = application_getCurrentScreen(app);
			if (child != NULL)
				dataobject_debugPrintR(child, level+1);
		}
	} else {
		list_begin(dobj->children, &citer);
		while (!listIterator_finished(&citer)) {
			dataobject_debugPrintR((DataObject *)listIterator_item(&citer), level+1);
			listIterator_next(&citer);
		}
	}
	/*listIterator_delete(citer);*/
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

	dataobject_exportGlobal(output, url, isLocal);

	return output;
}

void dataobject_exportGlobal(DataObject *output, URL *url, int isLocal)
{
	output->isLocal = isLocal;
	if (isLocal)
		hashtable_append(globalDObjs, url->path, output);
	else
		hashtable_append(globalDObjs, url->all, output);
}

DataObjectField *dataobjectfield_string(const char *str)
{
	DataObjectField *output;

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_STRING;
	output->field.string = p_strdup(str);
    output->flags = 0;

	return output;
}

DataObjectField *dataobjectfield_data(void *data, int size)
{
	DataObjectField *output;

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_DATA;
	output->field.data.bytes = data;
	output->field.data.size = size;
    output->flags = 0;

	return output;
}

DataObjectField *dataobjectfield_int(int val)
{
	DataObjectField *output;

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_INT;
	output->field.integer = val;
    output->flags = 0;

	return output;
}

DataObjectField *dataobjectfield_uint(unsigned int val)
{
	DataObjectField *output;

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_UINT;
	output->field.uinteger = val;
    output->flags = 0;

	return output;
}

int dataobjectfield_isTrue(DataObjectField *field)
{
    if (field != NULL) {
        if (field->type == DOF_STRING &&
                (strcmp(field->field.string, "true") == 0 ||
                strcmp(field->field.string, "1") == 0))
            return 1;
        if (field->type == DOF_INT)
            return field->field.integer;
        if (field->type == DOF_UINT)
            return field->field.uinteger;
    }
    return 0;
}

void dataobjectfield_setBoolean(DataObjectField *field, int bval)
{
    if (field != NULL) {
		if (field->type == DOF_STRING) {
			p_free(field->field.string);
			field->field.string = p_strdup(bval ? "1" : "0");
		} else if (field->type == DOF_INT || field->type == DOF_UINT) {
			field->field.integer = bval;
		}
    }
}

int dataobjectfield_isString(DataObjectField *field, const char *str)
{
    if (field != NULL && field->type == DOF_STRING &&
            strcmp(field->field.string, str) == 0)
        return 1;
    return 0;
}

DataObjectField *dataobjectfield_copy(DataObjectField *field)
{
	DataObjectField *output;
	void *data;

	switch (field->type) {
		case DOF_INT:
			output = dataobjectfield_int(field->field.integer);
			break;
		case DOF_UINT:
			output = dataobjectfield_uint(field->field.uinteger);
			break;	
		case DOF_DATA:
			data = p_malloc(field->field.data.size);
			memcpy(data, field->field.data.bytes, field->field.data.size);
			output = dataobjectfield_data(data, field->field.data.size);
			break;
		case DOF_STRING:
			output = dataobjectfield_string(field->field.string);
			break;
		default:
			emo_printf("unknown field type" NL);
			return NULL;
	}
	output->flags = field->flags;
	return output;
}

extern ConnectionContext *connectionContext;

void dataobject_resolveReferences(DataObject *dobj)
{
	ListIterator iter;
	DataObject *ref, *parent;
	DataObjectField *field;
	URL *url;

	ref = widget_getDataObject(dobj);

	if (ref == dobj) {
		field = dataobject_getValue(dobj, "reference");
		if (field != NULL && field->type == DOF_STRING) {
			if (strchr(field->field.string, ':') != NULL) {
				url = url_parse(field->field.string, URL_ALL);	
				ref = dataobject_locate(url);
				if (ref == NULL) {
					connectionContext_syncRequest(connectionContext, url);
					ref = dataobject_locate(url);
				}
				if (ref != NULL)	
					widget_setDataObject(dobj, ref);
			} else {
				parent = dataobject_superparent(dobj);
				ref = dataobject_findByName(parent, field->field.string);
				if (ref != NULL)
					widget_setDataObject(dobj, ref);
			}
		}
	}

	list_begin(dobj->children, &iter);
	while (!listIterator_finished(&iter)) {
		dataobject_resolveReferences((DataObject *)listIterator_item(&iter));
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/
}

