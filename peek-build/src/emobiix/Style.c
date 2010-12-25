#include "Style.h"

#include "Map.h"
#include "List.h"
#include "WidgetRenderer.h"
#include "Debug.h"
#include "SetWidget.h"
#include "Application.h"
#include "ApplicationManager.h"
#include "lgui.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

void style_renderWidgetTree(Style *s, Widget *w)
{
	ListIterator iter;
	const char *id;
	DataObjectField *type = NULL;
	DataObject *dobj = NULL, *singleChild = NULL, *child, *root;
	WidgetRenderer *wr;
	Application *app;
	Style *style = NULL, *childStyle;
	int hasFocus, wentUp = 0;
	int translate_x, translate_y;
	int ccol, cline, cwidth, cheight;
	Rectangle *box, rect;
	static int used = 0, passed = 0;

	EMO_ASSERT(s != NULL, "style render tree missing style")
	EMO_ASSERT(w != NULL, "style render tree missing tree")

	box = widget_getBox(w);
	lgui_get_translate(&translate_x, &translate_y);
	rect.x = box->x + translate_x;
	rect.y = box->y + translate_y;
	rect.width = box->width;
	rect.height = box->height;
	if (lgui_clip_rect(&rect, &ccol, &cline, &cwidth, &cheight) == 0)
		return;

	type = dataobject_getEnum(w, EMO_FIELD_TYPE);
	hasFocus = widget_hasFocusOrParent(w);
	id = widget_getID(w);
	style = style_getID(s, type == NULL ? NULL : type->field.string, id, hasFocus, &wentUp);

	if (wentUp)
		childStyle = s;
	else {
		if (dataobject_getChildCount(style) == 0)
			childStyle = s;
		else
			childStyle = style;
	}

	wr = NULL;
	if (dataobject_isDirty(w)) {
		dobj = widget_getDataObject(w);
		style_getRenderer(style, w, EMO_FIELD_RENDERER, &wr);
		if (wr != NULL)
			wr->render(wr, style, w, dobj);
		dataobject_setClean(w);
	}

#if 0
	/* we dont render array children - they render themselves */
	if (dataobjectfield_isString(type, "array"))
		return;
#endif

	if (dataobjectfield_isString(type, "set")) {
		singleChild = setwidget_activeItem(w);
		if (singleChild != NULL)
			style_renderWidgetTree(childStyle, singleChild);
		return;
	}

	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					style = application_getCurrentStyle(app);
					if (style == NULL)
						style = manager_getRootStyle();
					if (child != NULL)
						style_renderWidgetTree(style, child);
				}
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			root = dataobject_superparent(child);
			app = manager_applicationForDataObject(root);
			if (app != NULL) {
				style = application_getCurrentStyle(app);
				if (style == NULL)
					style = manager_getRootStyle();
				style_renderWidgetTree(style, child);
			}
		}
	} else {
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			style_renderWidgetTree(childStyle,
					(Widget *)listIterator_item(&iter));
			listIterator_next(&iter);
		}
	}

	if (wr != NULL && wr->postrender != NULL)
		wr->postrender(wr, style, w, dobj);
}

Style *style_getID(Style *styleRoot, const char *otype, const char *id, int isFocused,
		int *wentUp)
{
	ListIterator iter;
	DataObjectField *type;
	int focus;
	DataObject *child, *parent;
	Style *output;

	EMO_ASSERT_NULL(styleRoot != NULL, "style getid missing style")

	if (id == NULL) {
		id = otype;
		if (otype == NULL)
			return styleRoot;
	}

	output = NULL;
	for (dataobject_childIterator(styleRoot, &iter);
			!listIterator_finished_inline(&iter); listIterator_next_inline(&iter)) {
		child = listIterator_item_inline(&iter);
		type = dataobject_getEnum(child, EMO_FIELD_TYPE);
		if (!dataobjectfield_isString(type, id))
			continue;
		focus = widget_hasFocusOrParent(child);
		if (focus == isFocused)
			return child;
		if (output == NULL)
			output = child;
	}
	if (output != NULL)
		return output;

	parent = dataobject_parent(styleRoot);
	if (parent != NULL) {
		if (wentUp != NULL)
			*wentUp = 1;
		return style_getID(parent, otype, id, isFocused, NULL);
	}

	output = manager_getRootStyle();
	if (styleRoot != output){ 
		if (wentUp != NULL)
			*wentUp = 1;
		return style_getID(output, otype, id, isFocused, NULL);
	}
	return styleRoot;
}

DataObjectField *style_getProperty(Style *os, DataObject *dobj, EmoField key)
{
	DataObjectField *output;
	Style *rootStyle, *s;
	
	EMO_ASSERT_NULL(os != NULL, "style get property missing style")
	EMO_ASSERT_NULL(dobj != NULL, "style get property missing object")

	s = os;
	output = dataobject_getEnum(dobj, key);
	if (output != NULL)
		return output;
	while (s != NULL) {
		output = dataobject_getEnum(s, key);
		if (output != NULL)
			return output;
		s = dataobject_parent(s);
	}

	rootStyle = manager_getRootStyle();
	if (os != rootStyle)
		return style_getProperty(rootStyle, dobj, key);

	return NULL;
}

DataObjectField *style_getPropertyAsInt(Style *os, DataObject *dobj, EmoField key)
{
	DataObjectField *output;
	Style *rootStyle;
	Style *s;

	EMO_ASSERT_NULL(os != NULL, "style get propertyAsInt missing style")
	EMO_ASSERT_NULL(dobj != NULL, "style get propertyAsInt missing object")

	s = os;
	output = dataobject_getEnumAsInt(dobj, key);
	if (output != NULL)
		return output;
	while (s != NULL) {
		output = dataobject_getEnumAsInt(s, key);
		if (output != NULL)
			return output;
		s = dataobject_parent(s);
	}

	rootStyle = manager_getRootStyle();
	if (os != rootStyle)
		return style_getPropertyAsInt(rootStyle, dobj, key);

	return NULL;
}

DataObjectField *style_getColor(Style *s, DataObject *dobj, EmoField key,
		unsigned int *color)
{
	DataObjectField *output;

	EMO_ASSERT_NULL(s != NULL, "style get color missing style")
	EMO_ASSERT_NULL(dobj != NULL, "style get color missing object")
	EMO_ASSERT_NULL(color != NULL, "style get color missing color")

	output = style_getPropertyAsInt(s, dobj, key);
	if (output == NULL)
		return output;
	if (output->type == DOF_UINT || output->type == DOF_INT) {
		*color = output->field.uinteger;
		return output;
	}
	return NULL;
}
DataObjectField *style_getRenderer(Style *s, DataObject *dobj, EmoField key,
		struct WidgetRenderer_t **wr)
{
	DataObjectField *output;

	EMO_ASSERT_NULL(s != NULL, "style get renderer missing style")
	EMO_ASSERT_NULL(dobj != NULL, "style get renderer missing object")
	EMO_ASSERT_NULL(wr != NULL, "style get renderer missing renderer")

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

	EMO_ASSERT_NULL(s != NULL, "style get gradient missing style")

	type = dataobject_getEnum(s, EMO_FIELD_TYPE);
	if (!dataobjectfield_isString(type, "gradient"))
		return NULL;

	output = gradient_new();
	for (dataobject_childIterator(s, &iter);
			!listIterator_finished(&iter); listIterator_next(&iter)) {
		child = (DataObject *)listIterator_item(&iter);
		pos = dataobject_getEnumAsInt(child, EMO_FIELD_POSITION);
		position = 0;
		if (pos != NULL)
			position = pos->field.integer;
		colorF = dataobject_getEnumAsInt(child, EMO_FIELD_COLOR);
		color.value = 0;
		if (colorF != NULL)
			color.value = colorF->field.uinteger;
		gradient_addStop(output, position, color);
	}
	return output;
}
