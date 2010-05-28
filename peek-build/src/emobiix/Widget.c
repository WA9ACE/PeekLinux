#include "Widget.h"
#include "Rectangle.h"
#include "Point.h"
#include "DataObject.h"
#include "DataObjectMap.h"
#include "List.h"
#include "Style.h"
#include "WidgetRenderer.h"
#include "lgui.h"
#include "Debug.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

#include "DataObject_internal.h"
#include "DataObject_private.h"

/*#define CLIP_DEBUG*/

Widget *widget_new(void)
{
	Widget *output;
	output = dataobject_new();
	/*rectangle_zero(&output->margin);
	rectangle_zero(&output->box);*/

	return output;
}

void widget_setDataObject(Widget *w, DataObject *dobj)
{
	w->widgetData = dobj;
	/*w->self = dobj;*/
}

DataObject *widget_getDataObject(Widget *w)
{
	if (w->widgetData == NULL)
		return w;
	return w->widgetData;
}

void widget_setPacking(Widget *w, WidgetPacking p)
{
	DataObjectField *field;

	field = dataobject_getValue(w, "packing");
	if (!field)
		dataobject_setValue(w, "packing", dataobjectfield_int(p));
	else
		field->field.integer = p;
}

WidgetPacking widget_getPacking(Widget *w)
{
	DataObjectField *field, *newField;
	
	field = dataobject_getValue(w, "packing");
	if (!field)
		return (WidgetPacking)0;
	if (field->type == DOF_STRING) {
		if (strcmp(field->field.string, "horizontal") == 0)
			newField = dataobjectfield_int(WP_HORIZONTAL);
		else if (strcmp(field->field.string, "vertical") == 0)
			newField = dataobjectfield_int(WP_VERTICAL);
		else if (strcmp(field->field.string, "fixed") == 0)
			newField = dataobjectfield_int(WP_FIXED);
		else
			newField = dataobjectfield_int(WP_FIXED);

		p_free(field->field.string);
		p_free(field);
		dataobject_setValue(w, "packing", newField);
		field = newField;
	}
	return (WidgetPacking)field->field.integer;
}

void widget_pack(Widget *w, Widget *parent)
{
	w->parent = parent;
	list_append(parent->children, w);
}

ListIterator *widget_getChildren(Widget *w)
{
	return list_begin(w->children);
}

void widget_setClass(Widget *w, const char *className)
{
	DataObjectField *field;

	field = dataobject_getValue(w, "name");
	if (!field)
		dataobject_setValue(w, "name", dataobjectfield_string(className));
	else {
		p_free(field->field.string);
		field->field.string = p_strdup(className);
	}
}

const char *widget_getClass(Widget *w)
{
	DataObjectField *field;
	field = dataobject_getValue(w, "name");
	if (!field)
		return NULL;
	return field->field.string;
}

void widget_setID(Widget *w, const char *idName)
{
	DataObjectField *field;

	field = dataobject_getValue(w, "id");
	if (!field)
		dataobject_setValue(w, "id", dataobjectfield_string(idName));
	else {
		p_free(field->field.string);
		field->field.string = p_strdup(idName);
	}	
}

const char *widget_getID(Widget *w)
{
	DataObjectField *field;
	field = dataobject_getValue(w, "id");
	if (!field)
		return NULL;
	return field->field.string;
}

int widget_canFocus(Widget *w)
{
DataObjectField *field, *newField;
	
	field = dataobject_getValue(w, "canfocus");
	if (!field)
		return 0;
	if (field->type == DOF_STRING) {
		if (strcmp(field->field.string, "true") == 0 ||
				strcmp(field->field.string, "1") == 0)
			newField = dataobjectfield_int(1);
		else
			newField = dataobjectfield_int(0);

		p_free(field->field.string);
		p_free(field);
		dataobject_setValue(w, "canfocus", newField);
		field = newField;
	}
	return field->field.integer;
}

void widget_setCanFocus(Widget *w, int canFocus)
{
	DataObjectField *field;

	field = dataobject_getValue(w, "canfocus");
	if (!field)
		dataobject_setValue(w, "canfocus", dataobjectfield_int(canFocus));
	else
		field->field.integer = canFocus;
}

int widget_hasFocus(Widget *w)
{
	DataObjectField *field;
	field = dataobject_getValue(w, "hasfocus");
	if (!field)
		return 0;
	return field->field.integer;
}

void widget_setFocus(Widget *w, int isFocus)
{
	DataObjectField *field;

	field = dataobject_getValue(w, "hasfocus");
	if (!field)
		dataobject_setValue(w, "hasfocus", dataobjectfield_int(isFocus));
	else
		field->field.integer = isFocus;
}

int widget_isParent(Widget *wParent, Widget *w)
{
	while (w->parent != NULL) {
		if (wParent == w)
			return 1;
		w = w->parent;
	}
	return 0;
}

Rectangle *widget_getMargin(Widget *w)
{
	return &w->margin;
}

Rectangle *widget_getBox(Widget *w)
{
	return &w->box;
}

void widget_printTree(Widget *w, int level)
{
#ifdef SIMULATOR
	int i;
	ListIterator *iter;

	for (i = 0; i < level; ++i)
		printf("  ");

	iter = widget_getChildren(w);
	while (!listIterator_finished(iter)) {
		widget_printTree((Widget *)listIterator_item(iter), level+1);
		listIterator_next(iter);
	}
	listIterator_delete(iter);
#endif
}

int widget_focusFirst(Widget *w, List *l)
{
	ListIterator *iter;
	int result;
	
	if (widget_canFocus(w)) {
		widget_setFocus(w, 1);
		list_append(l, (void *)w);
		return 1;
	}

	iter = widget_getChildren(w);
	while (!listIterator_finished(iter)) {
		result = widget_focusFirst((Widget *)listIterator_item(iter),
				l);
		if (result) {
			listIterator_delete(iter);
			return 1;
		}
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	return 0;
}

int widget_focusNextR(Widget *w, List *l, int parentRedraw, int *alreadyUnset, int *alreadySet)
{
	ListIterator *iter;
	int result, thisUnset;

	/*printf("Widget(%p) %d, %d, %d\n", w, parentRedraw, alreadyUnset, alreadySet);*/

	thisUnset = 0;
	if (!*alreadyUnset && widget_hasFocus(w)) {
		/*printf("UnFocus(%d)\n", w);*/
		widget_setFocus(w, 0);
#if 0
		if (!parentRedraw) {
			/*printf("Repaint unfocus\n");*/
#endif
			list_append(l, w);
#if 0
		}
#endif
		*alreadyUnset = 1;
		thisUnset = 1;
		parentRedraw = 1;
	}

	if (!thisUnset && *alreadyUnset) {
		if (widget_canFocus(w)) {
			/*printf("Focus(%d)\n", w);*/
			widget_setFocus(w, 1);
#if 0
			if (!parentRedraw) {
				/*printf("paint focus\n");*/
#endif
				list_append(l, w);
#if 0
			}
#endif
			*alreadySet = 1;
			return 2;
		}
	}

	iter = widget_getChildren(w);
	while (!listIterator_finished(iter)) {
		result = widget_focusNextR((Widget *)listIterator_item(iter),
				l, parentRedraw, alreadyUnset, alreadySet);
		if (result == 2) {
			listIterator_delete(iter);
			return 2;
		}
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	return 0;
}

void widget_focusNext(Widget *tree, Style *s)
{
	List *redrawlist;
	ListIterator *iter;
	int unset, iset;
	Widget *w1;
	Rectangle rect;

	unset = 0;
	iset = 0;
	redrawlist = list_new();
	if (widget_focusNextR(tree, redrawlist, 0, &unset, &iset) == 0) {
		widget_focusFirst(tree, redrawlist);
	}

	/*style_renderWidgetTree(s, tree);*/

	/*emo_printf("Redraw list: %d\n", list_size(redrawlist));*/

	iter = list_begin(redrawlist);
	while (!listIterator_finished(iter)) {
		w1 = (Widget *)listIterator_item(iter);
		/*emo_printf("redrawlist: %p\n", w1);*/
		widget_markDirty(w1);

		lgui_clip_identity();
		widget_getClipRectangle(w1, &rect);
		lgui_clip_set(&rect);
		lgui_push_region();
		style_renderWidgetTree(s, tree);
#ifdef CLIP_DEBUG
		lgui_box(rect.x, rect.y, rect.width, rect.height, 1, 0, 0, 0);
#endif
		listIterator_next(iter);
	}
	listIterator_delete(iter);
	list_delete(redrawlist);
}

Widget *widget_focusPrevD(Widget *w)
{
	ListIterator *iter;
	Widget *cw, *iw;

	iter = list_rbegin(w->children);
	while (!listIterator_finished(iter)) {
		cw = (Widget *)listIterator_item(iter);
		iw = widget_focusPrevD(cw);
		if (iw != NULL) {
			listIterator_delete(iter);
			return iw;
		}
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	if (widget_canFocus(w)) {
		widget_setFocus(w, 1);
		return w;
	}

	return NULL;
}

Widget *widget_focusPrevR(Widget *old)
{
	Widget *w, *cw, *iw;
	ListIterator *iter;

	w = old->parent;
	if (w == NULL)
		return NULL;
	
	iter = list_rbegin(w->children);
	while (!listIterator_finished(iter)) {
		cw = (Widget *)listIterator_item(iter);
		if (cw == old) {
			break;
		}
		listIterator_next(iter);
	}

	if (listIterator_finished(iter)) {
		listIterator_delete(iter);
		return widget_focusPrevR(w);
	}

	listIterator_next(iter);

	while (!listIterator_finished(iter)) {
		cw = (Widget *)listIterator_item(iter);
		iw = widget_focusPrevD(cw);
		if (iw == NULL && widget_canFocus(cw)) {
			widget_setFocus(cw, 1);
			listIterator_delete(iter);
			return cw;
		}
		if (iw != NULL) {
			listIterator_delete(iter);
			return iw;
		}
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	if (widget_canFocus(w)) {
		widget_setFocus(w, 1);
		return w;
	}

	return widget_focusPrevR(w);
}

static Widget *widget_focusLast(Widget *w)
{
	Widget *child;
	ListIterator *iter;

	iter = list_rbegin(w->children);
	while (!listIterator_finished(iter)) {
		child = (Widget *)listIterator_item(iter);
		child = widget_focusLast(child);
		if (child != NULL)
			return child;
		listIterator_next(iter);
	}
	listIterator_delete(iter);

	if (widget_canFocus(w)) {
		widget_setFocus(w, 1);
		return w;
	}

	return NULL;
}

void widget_focusPrev(Widget *tree, Style *s)
{
	Widget *oldW, *newW;
	Rectangle rect;

	oldW = widget_focusWhichOne(tree);
	if (oldW == NULL)
		return;

	widget_setFocus(oldW, 0);

	newW = widget_focusPrevR(oldW);
	if (newW == NULL)
		newW = widget_focusLast(tree);

	widget_markDirty(oldW);
	lgui_clip_identity();
	widget_getClipRectangle(oldW, &rect);
	lgui_clip_set(&rect);

	lgui_push_region();
	style_renderWidgetTree(s, tree);

	widget_markDirty(newW);
	lgui_clip_identity();
	widget_getClipRectangle(newW, &rect);
	lgui_clip_set(&rect);

	lgui_push_region();
	style_renderWidgetTree(s, tree);

}

Widget *widget_focusNoneR(Widget *w)
{
	ListIterator *iter;
	Widget *result;
	
	if (widget_hasFocus(w)) {
		widget_setFocus(w, 0);
		return w;
	}

	iter = widget_getChildren(w);
	while (!listIterator_finished(iter)) {
		result = widget_focusNoneR((Widget *)listIterator_item(iter));
		if (result) {
			listIterator_delete(iter);
			return result;
		}
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	return NULL;
}

void widget_focusNone(Widget *w, Style *s)
{
	Widget *dw;

	dw = widget_focusNoneR(w);

	if (dw != NULL)
		style_renderWidgetTree(s, dw);
}

Widget *widget_focusWhichOneNF(Widget *w)
{
	ListIterator *iter;
	Widget *result;
	
	if (widget_hasFocus(w)) {
		return w;
	}

	iter = widget_getChildren(w);
	while (!listIterator_finished(iter)) {
		result = widget_focusWhichOneNF((Widget *)listIterator_item(iter));
		if (result) {
			listIterator_delete(iter);
			return result;
		}
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	return NULL;
}

Widget *widget_focusWhichOne(Widget *w)
{
	ListIterator *iter;
	Widget *result;
	
	if (widget_hasFocus(w)) {
		widget_setFocus(w, 0);
		return w;
	}

	iter = widget_getChildren(w);
	while (!listIterator_finished(iter)) {
		result = widget_focusWhichOne((Widget *)listIterator_item(iter));
		if (result) {
			listIterator_delete(iter);
			return result;
		}
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	return NULL;
}

static void widget_markDirtyChild(Widget *w)
{
	ListIterator *iter;
	dataobject_setDirty(w);

	iter = list_begin(w->children);
	while (!listIterator_finished(iter)) {
		widget_markDirty((Widget *)listIterator_item(iter));
		listIterator_next(iter);
	}
	listIterator_delete(iter);
}

void widget_markDirty(Widget *w)
{
	Widget *parent;

	widget_markDirtyChild(w);

	parent = w;
	while (parent->parent != NULL) {
		parent = parent->parent;
		dataobject_setDirty(parent);
	}
}

static void widget_layoutMeasure(Widget *w, Style *s)
{
	ListIterator *iter;
	const char *id, *className;
	DataObjectField *type = NULL, *sField;
	WidgetRenderer *wr;
	DataObject *dobj;
	IPoint p;
	Widget *cw;
	int cwidth, cheight, tmpint, slen;
	WidgetPacking packing;

	if (s == NULL)
		return;

	iter = list_begin(w->children);

	cwidth = 0;
	cheight = 0;
	packing = widget_getPacking(w);

	while (!listIterator_finished(iter)) {
		cw = (Widget *)listIterator_item(iter);
		widget_layoutMeasure(cw, s);
		switch (packing) {
			case WP_VERTICAL:
			default:
				tmpint = cw->box.width + cw->margin.x + cw->margin.width;
				cwidth = cwidth > tmpint ? cwidth : tmpint;
				cheight += cw->box.height + cw->margin.y + cw->margin.height;
				break;
			case WP_HORIZONTAL:
				tmpint = cw->box.height + cw->margin.y + cw->margin.height;
				cwidth += cw->box.width + cw->margin.width + cw->margin.x;
				cheight = cheight > tmpint ? cheight : tmpint;
				break;
		}
		listIterator_next(iter);
	}

	if (list_size(w->children) == 0) {
		dobj = widget_getDataObject(w);
		className = widget_getClass(w);
		id = widget_getID(w);
		if (dobj != NULL)
			type = dataobject_getValue(dobj, "type");
		wr = (WidgetRenderer *)style_getProperty(s, className,
				id, type == NULL ? NULL : type->field.string, "renderer");
		if (wr != NULL) {
			wr->measure(wr, s, w, dobj, &p);
			w->box.width = p.x;
			w->box.height = p.y;
		}
	} else {
		w->box.width = cwidth;
		w->box.height = cheight;
	}

	sField = dataobject_getValue(w, "width");
	if (sField != NULL) {
		if (sField->type == DOF_INT)
			w->box.width = sField->field.integer;
		else {
			slen = strlen(sField->field.string);
			sscanf(sField->field.string, "%d", &tmpint);
			if (sField->field.string[slen-1] == '%') {
				w->box.width = (int)(w->parent->box.width * ((float)tmpint)/100.0);
			} else {
				w->box.width = tmpint;
			}
		}
	}

	sField = dataobject_getValue(w, "height");
	if (sField != NULL) {
		if (sField->type == DOF_INT)
			w->box.height = sField->field.integer;
		else {
			slen = strlen(sField->field.string);
			sscanf(sField->field.string, "%d", &tmpint);
			if (sField->field.string[slen-1] == '%') {
				w->box.height = (int)(w->parent->box.height * ((float)tmpint)/100.0);
			} else {
				w->box.height = tmpint;
			}
		}
	}

	listIterator_delete(iter);
}

static void widget_resolveLayoutR(Widget *w, Style *s)
{
	int xpos, ypos, width, height;
	ListIterator *iter;
	WidgetAlignment alignment;
	WidgetPacking packing;
	Widget *cw;

	xpos = w->box.x+w->margin.x;
	ypos = w->box.y+w->margin.y;
	width = w->box.width;
	height = w->box.height;
	packing = widget_getPacking(w);

	iter = list_begin(w->children);

	while (!listIterator_finished(iter)) {
		cw = (Widget *)listIterator_item(iter);
		alignment = widget_getAlignment(cw);
		
		switch (alignment) {
			case WA_LEFT:
			default:
				if (packing == WP_HORIZONTAL) {
					cw->box.x = xpos;
					cw->box.y = ypos+cw->margin.y;
					xpos += cw->box.width;
				} else {
					cw->box.x = xpos+cw->margin.x;
					cw->box.y = ypos;
					ypos += cw->box.height;
				}
				break;
			case WA_RIGHT:
				if (packing == WP_HORIZONTAL) {
					cw->box.x = xpos;
					cw->box.y = ypos + height-cw->box.height-cw->margin.y;
					xpos += cw->box.width;
				} else {
					cw->box.x = xpos + width-cw->box.width-cw->margin.width;
					cw->box.y = ypos;
					ypos += cw->box.height;
				}
				break;
			case WA_CENTER:
				if (packing == WP_HORIZONTAL) {
					cw->box.x = xpos;
					cw->box.y = ypos + (height-cw->box.height-cw->margin.y-cw->margin.height)/2;
					xpos += cw->box.width;
				} else {
					cw->box.x = xpos + (width-cw->box.width-cw->margin.x-cw->margin.width)/2;
					cw->box.y = ypos;
					ypos += cw->box.height;
				}
				break;
		}
		widget_resolveLayoutR(cw, s);
		if (packing == WP_HORIZONTAL)
			xpos += 2;
		else
			ypos += 2;
		listIterator_next(iter);
	}

	listIterator_delete(iter);
}

void widget_resolveLayout(Widget *w, Style *s)
{
	ListIterator *iter;

	w->box.x = 0;
	w->box.y = 0;
	w->box.width = 320;
	w->box.height = 240;

	w->margin.x = 0;
	w->margin.y = 0;
	w->margin.width = 0;
	w->margin.height = 0;

	iter = list_begin(w->children);

	while (!listIterator_finished(iter)) {
		widget_layoutMeasure(listIterator_item(iter), s);
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	widget_resolveLayoutR(w, s);	
}

void widget_setAlignment(Widget *w, WidgetAlignment a)
{
	DataObjectField *field;

	field = dataobject_getValue(w, "alignment");
	if (!field)
		dataobject_setValue(w, "alignment", dataobjectfield_int(a));
	else
		field->field.integer = a;
}

WidgetAlignment widget_getAlignment(Widget *w)
{
	DataObjectField *field, *newField;
	
	field = dataobject_getValue(w, "alignment");
	if (!field)
		return (WidgetAlignment)0;
	if (field->type == DOF_STRING) {
		if (strcmp(field->field.string, "left") == 0 ||
				strcmp(field->field.string, "top") == 0)
			newField = dataobjectfield_int(WA_LEFT);
		else if (strcmp(field->field.string, "center") == 0)
			newField = dataobjectfield_int(WA_CENTER);
		else if (strcmp(field->field.string, "right") == 0 ||
				strcmp(field->field.string, "bottom") == 0)
			newField = dataobjectfield_int(WA_RIGHT);
		else
			newField = dataobjectfield_int(WA_LEFT);

		p_free(field->field.string);
		p_free(field);
		dataobject_setValue(w, "alignment", newField);
		field = newField;
	}
	return (WidgetAlignment)field->field.integer;
}

void widget_getClipRectangle(Widget *w, Rectangle *rect)
{
	rect->x = w->box.x + w->margin.x;
	rect->y = w->box.y + w->margin.y;
	rect->width = w->box.width;
	rect->height = w->box.height;
}