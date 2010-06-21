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

Widget *widget_newTypeIdName(const char *type, const char *id,
        const char *name, Widget *parent)
{
    Widget *output;

    output = widget_new();
    if (type != NULL)
        dataobject_setValue(output, "type", dataobjectfield_string(type));
    if (id != NULL)
        dataobject_setValue(output, "id", dataobjectfield_string(id));
    if (name != NULL)
        dataobject_setValue(output, "name", dataobjectfield_string(name));
    if (parent != NULL)
        widget_pack(output, parent);

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
	DataObjectField *field;

	iter = list_rbegin(w->children);
	while (!listIterator_finished(iter)) {
		child = (Widget *)listIterator_item(iter);
		child = widget_focusLast(child);
		if (child != NULL)
			return child;
		listIterator_next(iter);
	}
	listIterator_delete(iter);
	emo_printf("Done %d children" NL, list_size(w->children));

	if (widget_canFocus(w)) {
		field = dataobject_getValue(w, "type");
		emo_printf("LastFocusing : %s" NL, field->field.string);
		widget_setFocus(w, 1);
		return w;
	} else {
		field = dataobject_getValue(w, "type");
		emo_printf("NotLastFocusing : %s\n", field->field.string);
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
	/*if (newW == NULL)
		return;*/

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

Widget *widget_findStringField(Widget *w, const char *key, const char *value)
{
	DataObjectField *field;
	ListIterator *iter;
	Widget *retval;

	field = dataobject_getValue(w, key);
	if (field != NULL) {
		if (field->type == DOF_STRING && strcmp(field->field.string, value) == 0)
			return w;
	}

	iter = widget_getChildren(w);
	while (!listIterator_finished(iter)) {
		retval = widget_findStringField((Widget *)listIterator_item(iter), key, value);
		if (retval != NULL) {
			listIterator_delete(iter);
			return retval;
		}
		listIterator_next(iter);
	}
	listIterator_delete(iter);

	return NULL;
}

static void widget_layoutMeasureAbsolute(Widget *w, Style *s)
{
	DataObjectField *sField, *type;
	WidgetRenderer *wr;
	DataObject *dobj;
	const char *className, *id;
	ListIterator *iter;
	IPoint p;
	int slen, tmpint;

	/* get style default */
	dobj = widget_getDataObject(w);
	className = widget_getClass(w);
	id = widget_getID(w);
	type = dataobject_getValue(w, "type");
	wr = (WidgetRenderer *)style_getProperty(s, className,
			id, type == NULL ? NULL : type->field.string, "renderer");
	if (wr != NULL && wr->measure != NULL) {
		wr->measure(wr, s, w, dobj, &p);
		w->box.width = p.x;
		w->box.height = p.y;
		dataobject_setLayoutClean(w, LAYOUT_DIRTY_WIDTH);
		dataobject_setLayoutClean(w, LAYOUT_DIRTY_HEIGHT);
	}

	/* get specified absolute values */
#define ABSOLUTE_FIELD(_x, _y) \
	sField = dataobject_getValue(w, #_x); \
	if (sField != NULL) { \
		if (sField->type == DOF_INT) { \
			w->box._x = sField->field.integer; \
			dataobject_setLayoutClean(w, _y); \
		} else { \
			slen = strlen(sField->field.string); \
			sscanf(sField->field.string, "%d", &tmpint); \
			if (sField->field.string[slen-1] == '%') { \
				/* do nothing - this is a relative measure */ \
			} else { \
				w->box._x = tmpint; \
				dataobject_setLayoutClean(w, _y); \
			} \
		} \
	}

	ABSOLUTE_FIELD(width, LAYOUT_DIRTY_WIDTH);
	ABSOLUTE_FIELD(height, LAYOUT_DIRTY_HEIGHT);

#undef ABSOLUTE_FIELD

#if 0
	if (!dataobject_isLayoutDirty(w, LAYOUT_DIRTY_WIDTH)) {
		w->box.width -= w->margin.x + w->margin.width;
		if (w->box.width < 0)
			w->box.width = 0;
	}
	if (!dataobject_isLayoutDirty(w, LAYOUT_DIRTY_HEIGHT)) {
		w->box.height -= w->margin.y + w->margin.height;
		if (w->box.height < 0)
			w->box.width = 0;
	}
#endif

	iter = list_begin(w->children);
	while (!listIterator_finished(iter)) {
		widget_layoutMeasureAbsolute(listIterator_item(iter), s);
		listIterator_next(iter);
	}
	listIterator_delete(iter);
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

	cwidth = 0;
	cheight = 0;
	packing = widget_getPacking(w);

	sField = dataobject_getValue(w, "width");
	if (sField != NULL) {
		if (sField->type == DOF_INT)
			w->box.width = sField->field.integer;
		else {
			slen = strlen(sField->field.string);
			sscanf(sField->field.string, "%d", &tmpint);
			if (sField->field.string[slen-1] == '%') {
				w->box.width = (int)(w->parent->box.width * ((float)tmpint)/100.0);
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
			}
		}
	}

	iter = list_begin(w->children);
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
		if (wr == NULL)
			wr = widgetrenderer_zero();
		wr->measure(wr, s, w, dobj, &p);
		w->box.width = p.x;
		w->box.height = p.y;
	} else {
		w->box.width = cwidth;
		w->box.height = cheight;
	}

#if 0
	sField = dataobject_getValue(w, "type");
	if (sField == NULL ||
			(sField != NULL && sField->type == DOF_STRING &&
			(strcmp(sField->field.string, "view") == 0) ||
			(strcmp(sField->field.string, "stack") == 0))) {
		w->box.width = w->parent->box.width;
		w->box.height = w->parent->box.height;
	} else {
#endif
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
#if 0
	}
#endif

	listIterator_delete(iter);
}

void widget_layoutForceResolveParent(Widget *w, unsigned int flag)
{
	if (dataobject_isLayoutDirty(w->parent, flag))
		widget_layoutForceResolveParent(w->parent, flag);

	if (flag == LAYOUT_DIRTY_WIDTH) {
		w->box.width = w->parent->box.width -
				(w->margin.x + w->margin.width);
	} else {
		w->box.height = w->parent->box.height -
			(w->margin.y + w->margin.height);
	}
	dataobject_setLayoutClean(w, flag);
}

void widget_resolveMeasureRelative(Widget *w)
{
	int sumWidth, sumHeight;
	DataObject *child;
	DataObjectField *sField;
	ListIterator *iter;
	int tmpint, slen;

	/* get specified absolute values */
#define RELATIVE_FIELD(_x, _y, _z) \
	sField = dataobject_getValue(w, #_x); \
	if (sField != NULL) { \
		if (sField->type == DOF_STRING) { \
			slen = strlen(sField->field.string); \
			sscanf(sField->field.string, "%d", &tmpint); \
			if (sField->field.string[slen-1] == '%') { \
				if (dataobject_isLayoutDirty(w->parent, _y)) \
					widget_layoutForceResolveParent(w->parent, _y); \
				w->box._x = (int)(w->parent->box._x * ((float)tmpint)/100.0); \
				w->box._x -= w->margin._z + w->margin._x; \
				dataobject_setLayoutClean(w, _y); \
			} \
		} \
	}

	RELATIVE_FIELD(width, LAYOUT_DIRTY_WIDTH, x)
	RELATIVE_FIELD(height, LAYOUT_DIRTY_HEIGHT, y)

#undef RELATIVE_FIELD

	sumWidth = 0;
	sumHeight = 0;
	iter = list_begin(w->children);
	while (!listIterator_finished(iter)) {
		child = (DataObject *)listIterator_item(iter);
		widget_resolveMeasureRelative(child);
		sumWidth += child->box.width + child->margin.x + child->margin.width;
		sumHeight += child->box.height + child->margin.y + child->margin.height;
		listIterator_next(iter);
	}

	listIterator_delete(iter);

	if (dataobject_isLayoutDirty(w, LAYOUT_DIRTY_WIDTH)) {
		w->box.width = sumWidth;
		dataobject_setLayoutClean(w, LAYOUT_DIRTY_WIDTH);
	}
	if (dataobject_isLayoutDirty(w, LAYOUT_DIRTY_HEIGHT)) {
		w->box.height = sumHeight;
		dataobject_setLayoutClean(w, LAYOUT_DIRTY_HEIGHT);
	}
}

void widget_resolveMargin(Widget *w, Style *s)
{
	DataObjectField *field, *type;
	WidgetRenderer *wr;
	ListIterator *iter;
	const char *className, *id;
	DataObject *dobj;

	/* Apply margin from style */
	dobj = widget_getDataObject(w);
	className = widget_getClass(w);
	id = widget_getID(w);
	type = dataobject_getValue(w, "type");
	wr = (WidgetRenderer *)style_getProperty(s, className,
			id, type == NULL ? NULL : type->field.string, "renderer");
	if (wr != NULL && wr->margin != NULL)
		wr->margin(wr, s, w, dobj, &w->margin);

	/* Apply specified margin */
	field = dataobject_getValueAsInt(w, "margin");
	if (field != NULL) {
		w->margin.x = field->field.integer;
		w->margin.y = field->field.integer;
		w->margin.width = field->field.integer;
		w->margin.height = field->field.integer;
	} else {
		field = dataobject_getValueAsInt(w, "marginleft");
		if (field != NULL)
			w->margin.x = field->field.integer;
		field = dataobject_getValueAsInt(w, "marginright");
		if (field != NULL)
			w->margin.width = field->field.integer;
		field = dataobject_getValueAsInt(w, "margintop");
		if (field != NULL)
			w->margin.y = field->field.integer;
		field = dataobject_getValueAsInt(w, "marginbottom");
		if (field != NULL)
			w->margin.height = field->field.integer;
	}

	iter = list_begin(w->children);
	while (!listIterator_finished(iter)) {
		widget_resolveMargin(listIterator_item(iter), s);
		listIterator_next(iter);
	}

	listIterator_delete(iter);
}

void widget_resolvePosition(Widget *w)
{
	int xpos, ypos, width, height;
	ListIterator *iter;
	WidgetAlignment alignment;
	WidgetPacking packing;
	Widget *cw;
	int positionStatic = 0;
	DataObjectField *field;

	xpos = w->box.x+w->margin.x;
	ypos = w->box.y+w->margin.y;
	width = w->box.width;
	height = w->box.height;
	packing = widget_getPacking(w);

	field = dataobject_getValue(w, "type");
	if (field != NULL && field->type == DOF_STRING &&
			strcmp(field->field.string, "stack") == 0)
		positionStatic = 1;

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
					if (!positionStatic)
						xpos += cw->box.width+cw->margin.x+cw->margin.width;
				} else {
					cw->box.x = xpos+cw->margin.x;
					cw->box.y = ypos;
					if (!positionStatic)
						ypos += cw->box.height+cw->margin.y+cw->margin.height;
				}
				break;
			case WA_RIGHT:
				if (packing == WP_HORIZONTAL) {
					cw->box.x = xpos;
					cw->box.y = ypos + height-cw->box.height-cw->margin.y;
					if (!positionStatic)
						xpos += cw->box.width+cw->margin.x+cw->margin.width;
				} else {
					cw->box.x = xpos + width-cw->box.width-cw->margin.width;
					cw->box.y = ypos;
					if (!positionStatic)
						ypos += cw->box.height+cw->margin.y+cw->margin.height;
				}
				break;
			case WA_CENTER:
				if (packing == WP_HORIZONTAL) {
					cw->box.x = xpos;
					cw->box.y = ypos + (height-cw->box.height-cw->margin.y-cw->margin.height)/2;
					if (!positionStatic)	
						xpos += cw->box.width+cw->margin.x+cw->margin.width;
				} else {
					cw->box.x = xpos + (width-cw->box.width-cw->margin.x-cw->margin.width)/2;
					cw->box.y = ypos;
					if (!positionStatic)	
						ypos += cw->box.height+cw->margin.y+cw->margin.height;
				}
				break;
		}
		widget_resolvePosition(cw);
#if 0
		if (!positionStatic) {
			if (packing == WP_HORIZONTAL)
				xpos += 2;
			else
				ypos += 2;
		}
#endif
		listIterator_next(iter);
	}

	listIterator_delete(iter);
}

void widget_resolveLayout(Widget *w, Style *s)
{
	ListIterator *iter;

	dataobject_setLayoutDirtyAll(w);

	widget_resolveMargin(w, s);

	/* force to screen size */
	w->box.x = 0;
	w->box.y = 0;
	w->box.width = 320;
	w->box.height = 240;

	/* force to no margin */
	w->margin.x = 0;
	w->margin.y = 0;
	w->margin.width = 0;
	w->margin.height = 0;

	/* measure those with explicit sizes or measurable content */
	iter = list_begin(w->children);
	while (!listIterator_finished(iter)) {
		widget_layoutMeasureAbsolute(listIterator_item(iter), s);
		listIterator_next(iter);
	}
	listIterator_delete(iter);

	/* measure those whos size is based on their children or parent */
	iter = list_begin(w->children);
	while (!listIterator_finished(iter)) {
		widget_resolveMeasureRelative((DataObject *)listIterator_item(iter));
		listIterator_next(iter);
	}
	listIterator_delete(iter);

	/* position widgets based on packing and alignment */
#if 0
	iter = list_begin(w->children);
	while (!listIterator_finished(iter)) {
		listIterator_next(iter);
	}
	listIterator_delete(iter);
#endif
	widget_resolvePosition(w);
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

