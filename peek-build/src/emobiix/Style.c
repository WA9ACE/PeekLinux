#include "Style.h"

#include "Map.h"
#include "List.h"
#include "WidgetRenderer.h"
#include "Debug.h"
#include "SetWidget.h"
#include "Application.h"
#include "ApplicationManager.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

#if 0
#define KEY_LEN 128

struct Style_t {
	Map *prop;
};
#endif

#if 0
static void makekey(char *key, const char *className, const char *id,
		const char *type, const char *lvalue)
{
	snprintf(key, KEY_LEN, "%s-%s-%s-%s",
		"", /*className == NULL ? "" : className,*/
		id == NULL ? "" : id,
		type == NULL ? "" : type,
		lvalue == NULL ? "" : lvalue);
}
#endif

/*Style *style_new(void)
{
	Style *output;

	output = (Style *)p_malloc(sizeof(Style));
	output->prop = map_string();

	return output;
}*/

void style_renderWidgetTree(Style *s, Widget *w)
{
	ListIterator iter;
	const char *id, *className;
	DataObjectField *type = NULL;
	DataObject *dobj = NULL, *singleChild = NULL, *child;
	WidgetRenderer *wr;
	Application *app;
	Style *style;
	int hasFocus;

	type = dataobject_getValue(w, "type");

	if (dataobject_isDirty(w)) {
		/* ryan disabled this, dont know what it was here for anyway */
		dobj = widget_getDataObject(w);
		/*dobj = w;*/
		className = widget_getClass(w);
		id = widget_getID(w);
		/*if (dobj != NULL)*/
		wr = NULL;
		hasFocus = widget_hasFocus(w);
		style = style_getID(s, type == NULL ? NULL : type->field.string, id, hasFocus);
		style_getRenderer(style, w, "renderer", &wr);
		if (wr != NULL)
			wr->render(wr, style, w, dobj);
		dataobject_setClean(w);
	}

	/* we dont render array children - they render themselves */
	if (dataobjectfield_isString(type, "array"))
		return;

	if (dataobjectfield_isString(type, "set")) {
		singleChild = setwidget_activeItem(w);
		if (singleChild != NULL)
			style_renderWidgetTree(s, singleChild);
		return;
	}

	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL) {
			app = manager_appForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL)
					style_renderWidgetTree(s, child);
			}
		}
	} else {
		widget_getChildren(w, &iter);
		while (!listIterator_finished(&iter)) {
			style_renderWidgetTree(s, (Widget *)listIterator_item(&iter));
			listIterator_next(&iter);
		}
	}
}

#if 0
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
#endif

#if 0
void *style_getProperty(Style *s, const char *className, const char *id,
		const char *type, const char *lvalue)
{
	char key[KEY_LEN];
	/*emo_printf("getStyle(%s, %s, %s, %s)" NL, className, id, type, lvalue);*/
	makekey(key, className, id, type, lvalue);
	return map_find(s->prop, key);
}
#endif

Style *style_getID(Style *styleRoot, const char *otype, const char *id, int isFocused)
{
	ListIterator iter;
	DataObjectField *type;
	int focus;
	DataObject *child;
	Style *output;

	if (id == NULL) {
		id = otype;
		if (otype == NULL)
			return styleRoot;
	}

	output = NULL;
	for (dataobject_childIterator(styleRoot, &iter);
			!listIterator_finished(&iter); listIterator_next(&iter)) {
		child = listIterator_item(&iter);
		type = dataobject_getValue(child, "type");
		if (!dataobjectfield_isString(type, id))
			continue;
		focus = widget_hasFocus(child);
		if (focus == isFocused)
			return child;
		if (output == NULL)
			output = child;
	}
	if (output != NULL)
		return output;
	return styleRoot;
}

DataObjectField *style_getProperty(Style *s, DataObject *dobj, const char *key)
{
	DataObjectField *output;

	output = dataobject_getValue(dobj, key);
	if (output != NULL)
		return output;
	while (s != NULL) {
		output = dataobject_getValue(s, key);
		if (output != NULL)
			return output;
		s = dataobject_parent(s);
	}
	return NULL;
}

DataObjectField *style_getPropertyAsInt(Style *s, DataObject *dobj, const char *key)
{
	DataObjectField *output;

	output = dataobject_getValueAsInt(dobj, key);
	if (output != NULL)
		return output;
	while (s != NULL) {
		output = dataobject_getValueAsInt(s, key);
		if (output != NULL)
			return output;
		s = dataobject_parent(s);
	}
	return NULL;
}

DataObjectField *style_getColor(Style *s, DataObject *dobj, const char *key,
		unsigned int *color)
{
	DataObjectField *output;

	output = style_getProperty(s, dobj, key);
	if (output == NULL)
		return output;
	if (output->type == DOF_UINT || output->type == DOF_INT) {
		*color = output->field.uinteger;
		return output;
	}
	if (output->type == DOF_STRING) {
		sscanf(output->field.string, "%X", color);
		return output;
	}
	return NULL;
}
DataObjectField *style_getRenderer(Style *s, DataObject *dobj, const char *key,
		struct WidgetRenderer_t **wr)
{
	DataObjectField *output;

	output = style_getProperty(s, dobj, key);
	if (output == NULL || output->type != DOF_STRING)
		return NULL;

	*wr = widgetRenderer_fromString(output->field.string);
	return output;

}

Gradient *style_getGradient(Style *s)
{
	DataObjectField *type, *pos, *colorF;
	Gradient *output;
	ListIterator iter;
	DataObject *child;
	int position;
	Color color;

	type = dataobject_getValue(s, "type");
	if (!dataobjectfield_isString(type, "gradient"))
		return NULL;

	output = gradient_new();
	for (dataobject_childIterator(s, &iter);
			!listIterator_finished(&iter); listIterator_next(&iter)) {
		child = (DataObject *)listIterator_item(&iter);
		pos = dataobject_getValueAsInt(child, "position");
		position = 0;
		if (pos != NULL)
			position = pos->field.integer;
		colorF = dataobject_getValueAsInt(child, "color");
		color.value = 0;
		if (colorF != NULL)
			color.value = colorF->field.uinteger;
		gradient_addStop(output, position, color);
	}
	return output;
}