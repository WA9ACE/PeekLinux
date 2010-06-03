#include "Style.h"

#include "Map.h"
#include "List.h"
#include "WidgetRenderer.h"
#include "Debug.h"

#include "p_malloc.h"

#include <stdio.h>

#define KEY_LEN 128

struct Style_t {
	Map *prop;
};

static void makekey(char *key, const char *className, const char *id,
		const char *type, const char *lvalue)
{
	snprintf(key, KEY_LEN, "%s-%s-%s-%s",
		"", /*className == NULL ? "" : className,*/
		id == NULL ? "" : id,
		type == NULL ? "" : type,
		lvalue == NULL ? "" : lvalue);
}

Style *style_new(void)
{
	Style *output;

	output = (Style *)p_malloc(sizeof(Style));
	output->prop = map_string();

	return output;
}

void style_renderWidgetTree(Style *s, Widget *w)
{
	ListIterator *iter;
	const char *id, *className;
	DataObjectField *type = NULL;
	DataObject *dobj;
	WidgetRenderer *wr;

	if (dataobject_isDirty(w)) {
		dobj = widget_getDataObject(w);
		className = widget_getClass(w);
		id = widget_getID(w);
		if (dobj != NULL)
			type = dataobject_getValue(dobj, "type");
		wr = (WidgetRenderer *)style_getProperty(s, className,
				id, type == NULL ? NULL : type->field.string, "renderer");
		if (wr != NULL)
			wr->render(wr, s, w, dobj);
		dataobject_setClean(w);
	}

	iter = widget_getChildren(w);
	while (!listIterator_finished(iter)) {
		style_renderWidgetTree(s, (Widget *)listIterator_item(iter));
		listIterator_next(iter);
	}
	listIterator_delete(iter);
}

void style_setProperty(Style *s, const char *className, const char *id,
		const char *type, const char *lvalue, void *value)
{
	char key[KEY_LEN];
	makekey(key, className, id, type, lvalue);
	map_append(s->prop, key, value);
}

void *style_getProperty(Style *s, const char *className, const char *id,
		const char *type, const char *lvalue)
{
	char key[KEY_LEN];
	makekey(key, className, id, type, lvalue);
	return map_find(s->prop, key);
}

