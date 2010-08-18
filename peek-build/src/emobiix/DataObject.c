#include "DataObject.h"
#include "Subscription.h"

#include "Map.h"
#include "List.h"
#include "Debug.h"
#include "ConnectionContext.h"
#include "Application.h"
#include "ApplicationManager.h"
#include "RenderManager.h"

#include "p_malloc.h"

#include "DataObject_internal.h"
#include "DataObject_private.h"

#include <string.h>

void dataobjectfield_free(DataObjectField *f)
{
	EMO_ASSERT(f != NULL,
			"freeing data object field when NULL")

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
	EMO_ASSERT(field != NULL,
			"setting derived on data object field when NULL")

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
	output->referenced = list_new();
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

	EMO_ASSERT_NULL(dobj != NULL, "copying NULL DataObject")

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

	EMO_ASSERT_NULL(dobj != NULL, "tree copying NULL DataObject")

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
	ListIterator liter;
	DataObjectField *item;
	DataObject *cw;
	char *key;
	
	EMO_ASSERT(dobj != NULL, "deleting NULL DataObject")

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

	for (list_begin(dobj->referenced, &liter); !listIterator_finished(&liter);
			listIterator_next(&liter)) {
		cw = (DataObject *)listIterator_item(&liter);
		cw->widgetData = NULL;
		dataobject_setIsModified(cw, 1);
		renderman_queue(cw);
	}

	list_delete(dobj->children);
	list_delete(dobj->referenced);
	map_delete(dobj->data);
	renderman_dequeue(dobj);
	p_free(dobj);
}

void dataobject_setValue(DataObject *dobj, const char *key, DataObjectField *v)
{
	DataObjectField *old;

	EMO_ASSERT(dobj != NULL, "setValue on NULL DataObject")
	EMO_ASSERT(key != NULL, "setValue missing key")
	EMO_ASSERT(v != NULL, "setValue missing value")

	old = (DataObjectField *)map_find(dobj->data, key);
	if (old == v)
		return;
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

	EMO_ASSERT_NULL(dobj != NULL, "getValue on NULL DataObject")
	EMO_ASSERT_NULL(key != NULL, "getValue missing key")

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

	EMO_ASSERT_NULL(dobj != NULL, "getValueAsInt on NULL DataObject")
	EMO_ASSERT_NULL(key != NULL, "getValueAsInt missing key")

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
	EMO_ASSERT(dobj != NULL, "setting record type on NULL DataObject")

	if (isRecord)
		dobj->flags1 |= DO_FLAG_RECORD_TYPE;
	else
		dobj->flags1 &= ~DO_FLAG_RECORD_TYPE;
}

int dataobject_getRecordType(DataObject *dobj)
{
	EMO_ASSERT_INT(dobj != NULL, 0, "getting record type on NULL DataObject")

	return dobj->flags1 & DO_FLAG_RECORD_TYPE;
}

void dataobject_appendRecord(DataObject *dobj, DataObject *robj)
{
	EMO_ASSERT(dobj != NULL, "appending record on NULL DataObject")
	EMO_ASSERT(dobj != NULL, "appending NULL record")
		
	list_append(dobj->children, robj);
}

int dataobject_getChildCount(DataObject *dobj)
{
	EMO_ASSERT_INT(dobj != NULL, 0, "get child count on NULL DataObject")

	return list_size(dobj->children);
}

int dataobject_isLocal(DataObject *dobj)
{
	EMO_ASSERT_INT(dobj != NULL, 0, "get local on NULL DataObject")

	return dobj->isLocal;
}

DataObjectState dataobject_getState(DataObject *dobj)
{
	EMO_ASSERT_INT(dobj != NULL, 0, "get state on NULL DataObject")

	return dobj->state;
}

void dataobject_setState(DataObject *dobj, DataObjectState state)
{
	EMO_ASSERT(dobj != NULL, "set state on NULL DataObject")

	dobj->state = state;
}

void dataobject_getStamp(DataObject *dobj, unsigned int *stampMinor,
		unsigned int *stampMajor)
{
	EMO_ASSERT(dobj != NULL, "get stamp on NULL DataObject")
	EMO_ASSERT(stampMinor != NULL, "get stamp minor missing")
	EMO_ASSERT(stampMajor != NULL, "get stamp major missing")

	*stampMinor = dobj->stampMinor;
	*stampMajor = dobj->stampMajor;
}

void dataobject_setStamp(DataObject *dobj, unsigned int stampMinor,
		unsigned int stampMajor)
{
	EMO_ASSERT(dobj != NULL, "set stamp on NULL DataObject")

	dobj->stampMinor = stampMinor;
	dobj->stampMajor = stampMajor;
}

void dataobject_fieldIterator(DataObject *dobj, MapIterator *iter)
{
	EMO_ASSERT(dobj != NULL, "iterate fields on NULL DataObject")
	EMO_ASSERT(iter != NULL, "iterate fields on DataObject without iterator")

	map_begin(dobj->data, iter);
}

void dataobject_childIterator(DataObject *dobj, ListIterator *iter)
{
	EMO_ASSERT(dobj != NULL, "iterate children on NULL DataObject")
	EMO_ASSERT(iter != NULL, "iterate children on DataObject without iterator")

	list_begin(dobj->children, iter);
}

static DataObject *dataobject_getTreeR(DataObject *dobj, int *index)
{
	ListIterator iter;
	DataObject *output;

	EMO_ASSERT_NULL(dobj != NULL, "getTreeR on NULL DataObject")
	EMO_ASSERT_NULL(index != NULL, "getTreeR on DataObject without index")

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

	EMO_ASSERT_NULL(dobj != NULL, "getTree on NULL DataObject")

	idx = index;
	return dataobject_getTreeR(dobj, &idx);
}

DataObject *dataobject_getForcedObject(DataObject *dobj, int *index)
{
	ListIterator iter;
	DataObject *output;

	EMO_ASSERT_NULL(dobj != NULL,"getforcedObject on NULL DataObject")
	EMO_ASSERT_NULL(index != NULL,"getforcedObject on DataObject without index")

	/*fprintf(stderr, "GetTree - %d\n", *index);*/

	if (dobj->flags1 & DO_FLAG_FORCE_SYNC)
		return dobj;

	list_begin(dobj->children, &iter);
	while (!listIterator_finished(&iter)) {
		*index = *index + 1;
		output = dataobject_getForcedObject((DataObject *)listIterator_item(&iter), index);
		if (output != NULL) {
			/*listIterator_delete(iter);*/
			return output;
		}
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/
	return NULL;
}

int dataobject_getTreeNextOp(DataObject *dobj, int *ischild)
{
	ListIterator iter;
	DataObject *parent;
	DataObject *last;

	EMO_ASSERT_INT(dobj != NULL, 0, "treeNextOp on NULL DataObject")
	EMO_ASSERT_INT(ischild != NULL, 0, "treeNextOp on DataObject without child")

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

	EMO_ASSERT_INT(parent != NULL, 0, "treeIndexR on NULL DataObject")
	EMO_ASSERT_INT(dobj != NULL, 0, "treeIndexR on DataObject without obj")
	EMO_ASSERT_INT(index != NULL, 0, "treeIndexR on DataObject without index")

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

	EMO_ASSERT_INT(dobj != NULL, 0, "treeIndex on NULL DataObject")

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
	EMO_ASSERT(parent != NULL, "packing on NULL DataObject")
	EMO_ASSERT(child != NULL, "packing NULL child")
		
	child->parent = parent;
	list_append(parent->children, child);
}

void dataobject_packStart(DataObject *parent, DataObject *child)
{
	EMO_ASSERT(parent != NULL, "packing start on NULL DataObject")
	EMO_ASSERT(child != NULL, "packing start NULL child")

	child->parent = parent;
	list_prepend(parent->children, child);
}

DataObject *dataobject_parent(DataObject *dobj)
{
	EMO_ASSERT_NULL(dobj != NULL, "parent on NULL DataObject")

	return dobj->parent;
}

void dataobject_setParent(DataObject *dobj, DataObject *parent)
{
	EMO_ASSERT(dobj != NULL, "setParent on NULL DataObject")
	EMO_ASSERT(parent != NULL, "setParent on NULL parent")

	dobj->parent = dobj;
}

DataObject *dataobject_superparent(DataObject *dobj)
{
	EMO_ASSERT_NULL(dobj != NULL, "superparentParent on NULL DataObject")

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

	EMO_ASSERT_NULL(dobj != NULL, "findByName on NULL DataObject")
	EMO_ASSERT_NULL(name != NULL, "findByName without name")

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
	EMO_ASSERT_INT(dobj != NULL, 0, "isDirty on NULL DataObject")

	return dobj->flags1 & DO_FLAG_DIRTY;
}

void dataobject_setDirty(DataObject *dobj)
{
	EMO_ASSERT(dobj != NULL, "setDirty on NULL DataObject")

	dobj->flags1 |= DO_FLAG_DIRTY;
}

void dataobject_setClean(DataObject *dobj)
{
	EMO_ASSERT(dobj != NULL, "setClean on NULL DataObject")
		
	dobj->flags1 &= ~DO_FLAG_DIRTY;
}

int dataobject_isLayoutDirty(DataObject *dobj, unsigned int wh)
{
	EMO_ASSERT_INT(dobj != NULL, 0, "isLayoutDirty on NULL DataObject")

	return dobj->flags1 & wh;
}

void dataobject_setLayoutDirty(DataObject *dobj, unsigned int wh)
{
	EMO_ASSERT(dobj != NULL, "setLayoutDirty on NULL DataObject")

	dobj->flags1 |= wh;
}

void dataobject_setLayoutDirtyAll(DataObject *dobj)
{
	ListIterator iter;
	DataObjectField *type;
	DataObject *child;
	Application *app;

	EMO_ASSERT(dobj != NULL, "setLayoutDirtyAll on NULL DataObject")

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
	EMO_ASSERT(dobj != NULL, "setLayoutClean on NULL DataObject")

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
	
	EMO_ASSERT(dobj != NULL, "debugPrintR on NULL DataObject")

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
	EMO_ASSERT(dobj != NULL, "setting script context on NULL DataObject")

    dobj->scriptContext = ctx;
}

void *dataobject_getScriptContext(DataObject *dobj)
{
	EMO_ASSERT_NULL(dobj != NULL, "getting script context on NULL DataObject")

    return dobj->scriptContext;
}

void *dataobject_findScriptContext(DataObject *dobj)
{
	EMO_ASSERT_NULL(dobj != NULL, "finding script context on NULL DataObject")

    return dataobject_getScriptContext(dataobject_superparent(dobj));
}

DataObject *dataobject_findFieldParent(DataObject *dobj, const char *str)
{
    DataObjectField *field;

	EMO_ASSERT_NULL(dobj != NULL, "finding field parent on NULL DataObject")
	EMO_ASSERT_NULL(str != NULL, "finding field parent missing field")

    field = dataobject_getValue(dobj, str);
    if (field != NULL)
        return dobj;

    if (dobj->parent == NULL)
        return NULL;

    return dataobject_findFieldParent(dobj->parent, str);
}

void dataobject_debugPrint(DataObject *dobj)
{
	EMO_ASSERT(dobj != NULL, "debug print on NULL DataObject")

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

	EMO_ASSERT_NULL(str != NULL, "local object by str missing str")

	if (globalDObjs == NULL)
		dataobject_platformInit();
	output = hashtable_find(globalDObjs, str);
	return output;
}

DataObject *dataobject_locate(URL *url)
{
	DataObject *output;

	EMO_ASSERT_NULL(url != NULL, "local object by url missing url")

	output = dataobject_locateStr(url->all);
	if (output != NULL)
		return output;
	return dataobject_locateStr(url->path);
}

DataObject *dataobject_construct(URL *url, int isLocal)
{
	DataObject *output;

	EMO_ASSERT_NULL(url != NULL, "construct object missing url")

	output = dataobject_new();
	if (output == NULL)
		return NULL;

	dataobject_exportGlobal(output, url, isLocal);

	return output;
}

void dataobject_exportGlobal(DataObject *output, URL *url, int isLocal)
{
	EMO_ASSERT(output != NULL, "export global missing DataObject")
	EMO_ASSERT(url != NULL, "export global missing url")

	output->isLocal = isLocal;
	if (isLocal)
		hashtable_append(globalDObjs, url->path, output);
	else
		hashtable_append(globalDObjs, url->all, output);
}

DataObjectField *dataobjectfield_string(const char *str)
{
	DataObjectField *output;

	EMO_ASSERT_NULL(str != NULL, "new string field missing string")

	output = (DataObjectField *)p_malloc(sizeof(DataObjectField));
	output->type = DOF_STRING;
	output->field.string = p_strdup(str);
    output->flags = 0;

	return output;
}

DataObjectField *dataobjectfield_data(void *data, int size)
{
	DataObjectField *output;

	EMO_ASSERT_NULL(data != NULL && size != 0,
			"new data field without data yet size > 0")

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
	EMO_ASSERT(field != NULL, "set boolean on NULL field")

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
	EMO_ASSERT_INT(str != NULL, 0, "isString on field missing string")

    if (field != NULL && field->type == DOF_STRING &&
            strcmp(field->field.string, str) == 0)
        return 1;
    return 0;
}

DataObjectField *dataobjectfield_copy(DataObjectField *field)
{
	DataObjectField *output;
	void *data;

	EMO_ASSERT_NULL(field != NULL, "copying field when field is NULL")

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

void dataobjectfield_setIsModified(DataObjectField *field, int isModified)
{
	EMO_ASSERT(field != NULL, "setting NULL field modified flag")

	if (isModified)
		field->flags |= DOFF_CHANGED;
	else
		field->flags &= ~DOFF_CHANGED;
}

extern ConnectionContext *connectionContext;

void dataobject_resolveReferences(DataObject *dobj)
{
	ListIterator iter;
	DataObject *ref, *parent;
	DataObjectField *field;
	URL *url;

	EMO_ASSERT(dobj != NULL, "resolve references missing DataObject")

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


void dataobject_forceSyncFlag(DataObject *dobj, int isFlag)
{
	EMO_ASSERT(dobj != NULL, "force sync flag missing DataObject")

	emo_printf("Force Sync Flag: %d on %p" NL, isFlag, dobj);
	if (isFlag)
		dobj->flags1 |= DO_FLAG_FORCE_SYNC;
	else
		dobj->flags1 &= ~DO_FLAG_FORCE_SYNC;
}

#include "Script.h"
static void dataobject_onsyncfinishedR(DataObject *dobj, int *doneForced)
{
	ListIterator iter;
	DataObject *child;

	EMO_ASSERT(dobj != NULL, "syncfinishedcallbackR missing DataObject")
	EMO_ASSERT(doneForced != NULL,"syncfinishedcallbackR missing complete flag")

	if (!*doneForced && dobj->flags1 & DO_FLAG_FORCE_SYNC) {
		script_event(dobj, "onsyncfinished");
		dataobject_forceSyncFlag(dobj, 0);
		*doneForced = 1;
	}

	list_begin(dobj->children, &iter);
	while (!listIterator_finished(&iter)) {
		child = (DataObject *)listIterator_item(&iter);
		dataobject_onsyncfinishedR(child, doneForced);
		listIterator_next(&iter);
	}
}

void dataobject_onsyncfinished(DataObject *dobj)
{
	int doneForced = 0;

	EMO_ASSERT(dobj != NULL, "syncfinishedcallback missing DataObject")
	
	emo_printf("Sync finished start" NL);
	dataobject_onsyncfinishedR(dobj, &doneForced);
	emo_printf("Sync finished end" NL);
}

void dataobject_setIsModified(DataObject *dobj, int isModified)
{
	ListIterator iter;

	EMO_ASSERT(dobj != NULL, "setting modified on NULL DataObject")

	if (isModified) {
		renderman_queue(dobj);
		renderman_markLayoutChanged();
		dobj->flags1 |= DO_FLAG_CHANGED;
	} else {
		renderman_dequeue(dobj);
		dobj->flags1 &= ~DO_FLAG_CHANGED;
	}

	for (list_begin(dobj->referenced, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		dataobject_setIsModified((DataObject *)listIterator_item(&iter), isModified);
	}
}

void dataobject_setIsModifiedTree(DataObject *dobj, int isModified)
{
	ListIterator iter;

	EMO_ASSERT(dobj != NULL, "setting modified tree on NULL DataObject")

	dataobject_setIsModified(dobj, isModified);

	for (list_begin(dobj->children, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		dataobject_setIsModifiedTree((DataObject *)listIterator_item(&iter), isModified);
	}
}
