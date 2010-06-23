#include "Style.h"

#include "Map.h"
#include "List.h"
#include "WidgetRenderer.h"
#include "Debug.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

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
	DataObject *dobj = NULL;
	WidgetRenderer *wr;

	type = dataobject_getValue(w, "type");

	if (dataobject_isDirty(w)) {
		/* ryan disabled this, dont know what it was here for anyway */
		dobj = widget_getDataObject(w);
		/*dobj = w;*/
		className = widget_getClass(w);
		id = widget_getID(w);
		/*if (dobj != NULL)*/
		wr = (WidgetRenderer *)style_getProperty(s, className,
				id, type == NULL ? NULL : type->field.string, "renderer");
		if (wr != NULL)
			wr->render(wr, s, w, dobj);
		dataobject_setClean(w);
	}

	/* we dont render array children - they render themselves */
	if (type != NULL && type->type == DOF_STRING &&
			strcmp(type->field.string, "array") == 0)
		return;

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
#if 0
	if (type != NULL && strcmp(type, "box") == 0) {
		/* yeah its dirty but you like it */
		makekey(key, className, id, "button", lvalue);
		map_append(s->prop, key, value);
	}
#endif
}

void *style_getProperty(Style *s, const char *className, const char *id,
		const char *type, const char *lvalue)
{
	char key[KEY_LEN];
	/*emo_printf("getStyle(%s, %s, %s, %s)" NL, className, id, type, lvalue);*/
	makekey(key, className, id, type, lvalue);
	return map_find(s->prop, key);
}

