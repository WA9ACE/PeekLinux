#include "Widget.h"
#include "Rectangle.h"
#include "Point.h"
#include "DataObject.h"
#include "DataObjectMap.h"
#include "List.h"
#include "Style.h"
#include "WidgetRenderer.h"
#include "ArrayWidget.h"
#include "SetWidget.h"
#include "StackWidget.h"
#include "ScrolledWidget.h"
#include "lgui.h"
#include "Debug.h"
#include "Application.h"
#include "ApplicationManager.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "DataObject_internal.h"
#include "DataObject_private.h"

static void widget_layoutMeasureFinal(Widget *w, Style *s);
static void widget_markDirtyChild(Widget *w);

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
	ListIterator iter;
	DataObject *robj;

	EMO_ASSERT(w != NULL, "widget set object missing widget")

	if (w->widgetData != NULL) {
		for (list_begin(w->widgetData->referenced, &iter); !listIterator_finished(&iter);	
				listIterator_next(&iter)) {
			robj = (DataObject *)listIterator_item(&iter);
			if (robj == w) {
				listIterator_remove(&iter);
				break;
			}
		}
	}
	w->widgetData = dobj;
	if (dobj != NULL)
	{
		list_append(dobj->referenced, w);
	}
	/*w->self = dobj;*/
}

int widget_isArraySource(Widget *w)
{
	DataObjectField *field;

	EMO_ASSERT_INT(w != NULL, 0, "widget is array source missing widget")

	field = dataobject_getValue(w, "arraysource");
    return dataobjectfield_isTrue(field);
}

void widget_setDataObjectArray(Widget *w, DataObject *dobj)
{
	ListIterator iter;

	EMO_ASSERT(w != NULL, "widget set array missing widget")
	EMO_ASSERT(dobj != NULL, "widget set array missing array")

	if (widget_isArraySource(w))
	{
		EMO_ASSERT(dobj->referenced != NULL, "set data ref is null");
		widget_setDataObject(w, dobj);
	}
	
	for (dataobject_childIterator(w, &iter); !listIterator_finished(&iter); listIterator_next(&iter)) 
	{
		Widget *widg = (Widget *)listIterator_item(&iter);
		widget_setDataObjectArray(widg, dobj);
	}
}

DataObject *widget_getDataObject(Widget *w)
{
	EMO_ASSERT_NULL(w != NULL, "widget get object missing widget")

	while (w->widgetData != NULL)
		w = w->widgetData;
	return w;
}

void widget_setPacking(Widget *w, WidgetPacking p)
{
	DataObjectField *field;

	EMO_ASSERT(w != NULL, "widget get object missing widget")

	field = dataobject_getValue(w, "packing");
	if (!field)
		dataobject_setValue(w, "packing", dataobjectfield_int(p));
	else
		field->field.integer = p;
}

WidgetPacking widget_getPacking(Widget *w)
{
	DataObjectField *field, *newField;
	
	EMO_ASSERT_INT(w != NULL, WP_VERTICAL, "widget get packing missing widget")

	field = dataobject_getValue(w, "packing");
	if (!field)
		return (WidgetPacking)WP_VERTICAL;
	if (field->type == DOF_STRING) {
		if (isdigit(field->field.string[0]))
			newField = dataobjectfield_int(atoi(field->field.string));
		else if (strcmp(field->field.string, "horizontal") == 0)
			newField = dataobjectfield_int(WP_HORIZONTAL);
		else if (strcmp(field->field.string, "vertical") == 0)
			newField = dataobjectfield_int(WP_VERTICAL);
		else if (strcmp(field->field.string, "hgrid") == 0)
			newField = dataobjectfield_int(WP_HGRID);
		else if (strcmp(field->field.string, "vgrid") == 0)
			newField = dataobjectfield_int(WP_VGRID);
		else if (strcmp(field->field.string, "fixed") == 0)
			newField = dataobjectfield_int(WP_FIXED);
		else
			newField = dataobjectfield_int(WP_VERTICAL);

		/*p_free(field->field.string);
		p_free(field);*/
		dataobject_setValue(w, "packing", newField);
		field = newField;
	}
	return (WidgetPacking)field->field.integer;
}

void widget_pack(Widget *w, Widget *parent)
{
	EMO_ASSERT(w != NULL, "widget pack missing widget")
	EMO_ASSERT(parent != NULL, "widget pack missing parent")

	w->parent = parent;
	list_append(parent->children, w);
}

void widget_setClass(Widget *w, const char *className)
{
	DataObjectField *field;

	EMO_ASSERT(w != NULL, "widget set class missing widget")
	EMO_ASSERT(className != NULL, "widget set class missing name")

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

	EMO_ASSERT_NULL(w != NULL, "widget get class missing widget")
	
	field = dataobject_getValue(w, "name");
	if (!field)
		return NULL;
	return field->field.string;
}

void widget_setID(Widget *w, const char *idName)
{
	DataObjectField *field;

	EMO_ASSERT(w != NULL, "widget set id missing widget")
	EMO_ASSERT(idName != NULL, "widget set id missing id")

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

	EMO_ASSERT_NULL(w != NULL, "widget get id missing widget")

	field = dataobject_getValue(w, "id");
	if (!field)
		return NULL;
	return field->field.string;
}

int widget_canFocus(Widget *w)
{
	DataObjectField *field;

	EMO_ASSERT_INT(w != NULL, 0, "widget can focus missing widget")
	
	field = dataobject_getValue(w, "canfocus");
	return dataobjectfield_isTrue(field);
}

void widget_setCanFocus(Widget *w, int canFocus)
{
	DataObjectField *field;

	EMO_ASSERT(w != NULL, "widget set can focus missing widget")

	field = dataobject_getValue(w, "canfocus");
	if (!field)
		dataobject_setValue(w, "canfocus", dataobjectfield_int(canFocus));
	else
		field->field.integer = canFocus;
}

int widget_hasFocus(Widget *w)
{
	DataObjectField *field;

	EMO_ASSERT_INT(w != NULL, 0, "widget has focus missing widget")
	
	field = dataobject_getValueAsInt(w, "hasfocus");
	if (!field)
		return 0;
	return field->field.integer;
}

int widget_hasFocusOrParent(Widget *w)
{
	int hasFocus;

	EMO_ASSERT_INT(w != NULL, 0, "widget has focus or parent missing widget")

	hasFocus = widget_hasFocus(w);
	if (!hasFocus) {
		w = w->parent;
		if (w == NULL)
			return 0;
		return widget_hasFocusOrParent(w);
	}

	return 1;
}

void widget_setFocus(Widget *w, int isFocus)
{
	DataObjectField *field;

	EMO_ASSERT(w != NULL, "widget set focus missing widget")

	field = dataobject_getValue(w, "hasfocus");
	if (!field)
		dataobject_setValue(w, "hasfocus", dataobjectfield_int(isFocus));
	else
		field->field.integer = isFocus;

	if (isFocus)
		scrolled_forceVisible(w);
}

int widget_isParent(Widget *wParent, Widget *w)
{
	EMO_ASSERT_INT(w != NULL, 0, "widget is parent missing widget")
	EMO_ASSERT_INT(wParent != NULL, 0, "widget is parent missing parent")

	while (w->parent != NULL) {
		if (wParent == w)
			return 1;
		w = w->parent;
	}
	return 0;
}

Rectangle *widget_getMargin(Widget *w)
{
	EMO_ASSERT_NULL(w != NULL, "widget get margin missing widget")

	return &w->margin;
}

Rectangle *widget_getBox(Widget *w)
{
	EMO_ASSERT_NULL(w != NULL, "widget get box missing widget")

	return &w->box;
}

Rectangle *widget_getChildBox(Widget *w)
{
	EMO_ASSERT_NULL(w != NULL, "widget get child box missing widget")

	return &w->childBox;
}

void widget_printTree(Widget *w, int level)
{
#ifdef SIMULATOR
	int i;
	ListIterator iter;

	EMO_ASSERT(w != NULL, "widget print tree missing widget")

	for (i = 0; i < level; ++i)
		printf("  ");

	dataobject_childIterator(w, &iter);
	while (!listIterator_finished(&iter)) {
		widget_printTree((Widget *)listIterator_item(&iter), level+1);
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/
#endif
}

int widget_focusFirst(Widget *w, List *l)
{
	ListIterator iter;
	int result;
	DataObjectField *type;
	DataObject *child;
	Application *app;

	EMO_ASSERT_INT(w != NULL, 0, "widget focus first missing widget")
	EMO_ASSERT_INT(l != NULL, 0, "widget focus first missing list")

	if (widget_canFocus(w)) {
		widget_setFocus(w, 1);
		list_append(l, (void *)w);
		return 1;
	}

	type = dataobject_getValue(w, "type");
#if 0
	if (widget_typeNoChildRender(type))
		return 0;
#endif

	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					result = widget_focusFirst(child, l);
					if (result)
						return 1;
				}
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			result = widget_focusFirst(child, l);
			if (result)
				return 1;
		}
	} else if (dataobjectfield_isString(type, "stack")) {
		child = stackwidget_focusStackObject(w);
		if (child == NULL)
			goto normal_children;
		result = widget_focusFirst(child, l);
		if (result)
			return 1;
	} else {
normal_children:
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			result = widget_focusFirst((Widget *)listIterator_item(&iter),
					l);
			if (result) {
				/*listIterator_delete(iter);*/
				return 1;
			}
			listIterator_next(&iter);
		}
	}

	/*listIterator_delete(iter);*/

	return 0;
}

int widget_focusNextR(Widget *w, List *l, int parentRedraw, int *alreadyUnset, int *alreadySet)
{
	ListIterator iter;
	int result, thisUnset;
	DataObjectField *type;
	DataObject *child;
	Application *app;

	EMO_ASSERT_INT(w != NULL, 0, "widget focus nextR missing widget")
	EMO_ASSERT_INT(l != NULL, 0, "widget focus nextR missing list")
	EMO_ASSERT_INT(alreadyUnset != NULL, 0, "widget focus nextR missing unset")
	EMO_ASSERT_INT(alreadySet != NULL, 0, "widget focus nextR missing set")

	/*printf("Widget(%p) %d, %d, %d\n", w, parentRedraw, alreadyUnset, alreadySet);*/

	thisUnset = 0;
	type = dataobject_getValue(w, "type");
	if (!*alreadyUnset && widget_hasFocus(w)) {
		/*printf("UnFocus(%d)\n", w);*/

		if (dataobjectfield_isString(type, "scrolled")) {
			if (scrolled_focusNext(w) == 1) {
				list_append(l, w);
				*alreadyUnset = 1;
				*alreadySet = 1;
				return 2;
			}
		}

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

#if 0
	if (widget_typeNoChildRender(type)) {
		result = 0;
		if (type != NULL && type->type == DOF_STRING
				&& strcmp(type->field.string, "array") == 0)
			result = arraywidget_focusNext(w, alreadyUnset, alreadySet);
		if (result != 0)
			list_append(l, w);
		return result;
	}
#endif

	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					result = widget_focusNextR(child,
						l, parentRedraw, alreadyUnset, alreadySet);
					if (result == 2)
						return 2;
				}
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			result = widget_focusNextR(child,
				l, parentRedraw, alreadyUnset, alreadySet);
			if (result == 2)
				return 2;
		}
	} else if (dataobjectfield_isString(type, "stack")) {
		child = stackwidget_focusStackObject(w);
		if (child == NULL)
			goto normal_focus;
		result = widget_focusNextR(child,
				l, parentRedraw, alreadyUnset, alreadySet);
		if (result == 2)
			return 2;
	} else {
normal_focus:
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			result = widget_focusNextR((Widget *)listIterator_item(&iter),
					l, parentRedraw, alreadyUnset, alreadySet);
			if (result == 2) {
				/*listIterator_delete(iter);*/
				return 2;
			}
			listIterator_next(&iter);
		}
	}

	/*listIterator_delete(iter);*/

	return 0;
}

void widget_forceFocus(Widget *tree, Widget *node, Style *s)
{
	EMO_ASSERT(tree != NULL, "widget force focus missing tree")
	EMO_ASSERT(node != NULL, "widget force focus missing widget")
	EMO_ASSERT(s != NULL, "widget force focus missing style")

	widget_focusNone(tree, s, 1);
	
	widget_setFocus(node, 1);
	dataobject_setIsModified(node, 1);
#if 0
	widget_markDirty(node);
	lgui_clip_identity();
	widget_getClipRectangle(node, &rect);
	lgui_clip_set(&rect);
	lgui_push_region();
	style_renderWidgetTree(s, tree);
#endif
}

void widget_focusNext(Widget *tree, Style *s)
{
	List *redrawlist;
	ListIterator iter;
	int unset, iset;
	Widget *w1;

	EMO_ASSERT(tree != NULL, "widget focus next missing tree")
	EMO_ASSERT(s != NULL, "widget focus next missing style")

	unset = 0;
	iset = 0;
	redrawlist = list_new();
	if (widget_focusNextR(tree, redrawlist, 0, &unset, &iset) == 0) {
		widget_focusFirst(tree, redrawlist);
	}

	/*style_renderWidgetTree(s, tree);*/

	/*emo_printf("Redraw list: %d\n", list_size(redrawlist));*/

	do {
		list_begin(redrawlist, &iter);
		if (listIterator_finished(&iter))
			break;
		w1 = (Widget *)listIterator_item(&iter);
		/*emo_printf("redrawlist: %p\n", w1);*/
#if 0
		widget_markDirty(w1);

		lgui_clip_identity();
		widget_getClipRectangle(w1, &rect);
		lgui_clip_set(&rect);
		lgui_push_region();
		style_renderWidgetTree(s, tree);
#endif
		dataobject_setIsModified(w1, 1);
#ifdef CLIP_DEBUG
		lgui_box(rect.x, rect.y, rect.width, rect.height, 1, 0, 0, 0);
#endif
		listIterator_remove(&iter);
	} while(1);
	/*listIterator_delete(iter);*/
	list_delete(redrawlist);
}

Widget *widget_focusPrevD(Widget *w)
{
	ListIterator iter;
	Widget *cw, *iw;
	DataObjectField *type;
	DataObject *child;
	Application *app;

	EMO_ASSERT_NULL(w != NULL, "widget focus prevD missing widget")

	type = dataobject_getValue(w, "type");
	if (type != NULL /*&& !widget_typeNoChildRender(type)*/) {
		if (dataobjectfield_isString(type, "frame")) {
			child = widget_getDataObject(w);
			if (child != NULL && child != w) {
				app = manager_applicationForDataObject(child);
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					iw = widget_focusPrevD(child);
					if (iw != NULL)
						return iw;
				}
			}
		} else if (dataobjectfield_isString(type, "reference")) {
			child = widget_getDataObject(w);
			if (child != NULL && child != w) {
				iw = widget_focusPrevD(child);
				if (iw != NULL)
					return iw;
			}
		} else if (dataobjectfield_isString(type, "stack")) {
			child = stackwidget_focusStackObject(w);
			if (child == NULL)
				goto normal_focus;
			iw = widget_focusPrevD(child);
			if (iw != NULL)
				return iw;
		} else {
normal_focus:
			dataobject_rchildIterator(w, &iter);
			while (!listIterator_finished(&iter)) {
				cw = (Widget *)listIterator_item(&iter);
				iw = widget_focusPrevD(cw);
				if (iw != NULL) {
					/*listIterator_delete(iter);*/
					return iw;
				}
				listIterator_next(&iter);
			}

			/*listIterator_delete(iter);*/
		}
	}

	if (widget_canFocus(w)) {
		widget_setFocus(w, 1);
		return w;
	}

	return NULL;
}

Widget *widget_focusPrevR(Widget *old)
{
	Widget *w, *cw, *iw;
	ListIterator iter;

	EMO_ASSERT_NULL(old != NULL, "widget focus prevR missing widget")

	w = old->parent;
	if (w == NULL)
		return NULL;
	
	dataobject_rchildIterator(w, &iter);
	while (!listIterator_finished(&iter)) {
		cw = (Widget *)listIterator_item(&iter);
		if (cw == old) {
			break;
		}
		listIterator_next(&iter);
	}

	if (listIterator_finished(&iter)) {
		/*listIterator_delete(iter);*/
		return widget_focusPrevR(w);
	}

	listIterator_next(&iter);

	while (!listIterator_finished(&iter)) {
		cw = (Widget *)listIterator_item(&iter);
		iw = widget_focusPrevD(cw);
		if (iw == NULL && widget_canFocus(cw)) {
			widget_setFocus(cw, 1);
			/*listIterator_delete(iter);*/
			return cw;
		}
		if (iw != NULL) {
			/*listIterator_delete(iter);*/
			return iw;
		}
		listIterator_next(&iter);
	}

	/*listIterator_delete(iter);*/

	if (widget_canFocus(w)) {
		widget_setFocus(w, 1);
		return w;
	}

	return widget_focusPrevR(w);
}

static Widget *widget_focusLast(Widget *w)
{
	Widget *child;
	ListIterator iter;
	DataObjectField *type;
	/*int one, two;*/
	Application *app;

	EMO_ASSERT_NULL(w != NULL, "widget focus last missing widget")
		
	type = dataobject_getValue(w, "type");
	/*if (!widget_typeNoChildRender(type)) {*/
		if (dataobjectfield_isString(type, "frame")) {
			child = widget_getDataObject(w);
			if (child != NULL && child != w) {
				app = manager_applicationForDataObject(child);
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					child = widget_focusLast(child);
					if (child != NULL)
						return child;
				}
			}
		} else if (dataobjectfield_isString(type, "reference")) {
			child = widget_getDataObject(w);
			if (child != NULL && child != w) {
				child = widget_focusLast(child);
				if (child != NULL)
					return child;
			}
		} else if (dataobjectfield_isString(type, "stack")) {
			child = stackwidget_focusStackObject(w);
			if (child == NULL)
				goto normal_focus;
			child = widget_focusLast(child);
			if (child != NULL)
				return child;
		} else {
normal_focus:
			dataobject_rchildIterator(w, &iter);
			while (!listIterator_finished(&iter)) {
				child = (Widget *)listIterator_item(&iter);
				child = widget_focusLast(child);
				if (child != NULL)
					return child;
				listIterator_next(&iter);
			}
		}
		/*listIterator_delete(iter);*/
	/*}*/
	
	if (widget_canFocus(w)) {
		widget_setFocus(w, 1);
		return w;
	}

#if 0
	if (dataobjectfield_isString(type, "array")) {
		one = 1;
		arraywidget_focusNext(w, &one, &two);
		return w;
	}
#endif

	return NULL;
}

void widget_focusPrev(Widget *tree, Style *s)
{
	Widget *oldW, *newW;
	/*int result;*/
	DataObjectField *type;

	EMO_ASSERT(tree != NULL, "widget focus prev missing widget")

	oldW = widget_focusWhichOne(tree);
	if (oldW == NULL) {
		emo_printf("Focus didnt find any widget" NL);
		/*return;*/
		goto focus_last;
	}

	type = dataobject_getValue(oldW, "type");
	if (dataobjectfield_isString(type, "scrolled")) {
		if (scrolled_focusPrev(oldW) == 1) {
			/* focus WhichOne unsets focus so reset it */
			widget_setFocus(oldW, 1);
			dataobject_setIsModified(oldW, 1);
			return;
		}
	}

#if 0
	type = dataobject_getValue(oldW, "type");
	if (dataobjectfield_isString(type, "array")) {
		result = arraywidget_focusPrev(oldW);
		if (result) {
#if 0
			widget_markDirty(oldW);
			lgui_clip_identity();
			widget_getClipRectangle(oldW, &rect);
			lgui_clip_set(&rect);

			lgui_push_region();
			style_renderWidgetTree(s, tree);
#endif
			dataobject_setIsModified(oldW, 1);
			return;
		}
	}
#endif

	widget_setFocus(oldW, 0);

	newW = widget_focusPrevR(oldW);
	if (newW == NULL)
focus_last:
		newW = widget_focusLast(tree);
	/*if (newW == NULL)
		return;*/

	if (oldW != NULL) {
		dataobject_setIsModified(oldW, 1);
#if 0
		widget_markDirty(oldW);
		lgui_clip_identity();
		widget_getClipRectangle(oldW, &rect);
		lgui_clip_set(&rect);

		lgui_push_region();
		style_renderWidgetTree(s, tree);
#endif
	}

	if (newW != NULL) {
		dataobject_setIsModified(newW, 1);
#if 0
		widget_markDirty(newW);
		lgui_clip_identity();
		widget_getClipRectangle(newW, &rect);
		lgui_clip_set(&rect);
	
		lgui_push_region();
		style_renderWidgetTree(s, tree);
#endif
	}
}

Widget *widget_focusNoneR(Widget *w)
{
	ListIterator iter;
	Widget *result;
	DataObject *child;
	Application *app;
	DataObjectField *type;
	
	EMO_ASSERT_NULL(w != NULL, "widget focus noneR missing widget")

	if (widget_hasFocus(w)) {
		widget_setFocus(w, 0);
		return w;
	}

	type = dataobject_getValue(w, "type");
	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					result = widget_focusNoneR(child);
					if (result)
						return result;
				}
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			result = widget_focusNoneR(child);
			if (result)
				return result;
		}
	} else if (dataobjectfield_isString(type, "stack")) {
		child = stackwidget_focusStackObject(w);
		if (child == NULL)
			goto normal_focus;
		result = widget_focusNoneR(child);
		if (result)
			return result;
	} else {
normal_focus:
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			result = widget_focusNoneR((Widget *)listIterator_item(&iter));
			if (result) {
				/*listIterator_delete(iter);*/
				return result;
			}
			listIterator_next(&iter);
		}
	}

	/*listIterator_delete(iter);*/

	return NULL;
}

void widget_focusNone(Widget *w, Style *s, int doRender)
{
	Widget *dw;

	EMO_ASSERT(w != NULL, "widget focus none missing widget")
	EMO_ASSERT(s != NULL, "widget focus none missing style")

	dw = widget_focusNoneR(w);

	if (dw != NULL && doRender) {
		dataobject_setIsModified(dw, 1);
#if 0
		lgui_clip_identity();
		widget_getClipRectangle(dw, &rect);
		lgui_clip_set(&rect);
		lgui_push_region();
		widget_markDirty(dw);
		style_renderWidgetTree(s, w);
#endif
	}
}

Widget *widget_focusWhichOneNF(Widget *w)
{
	ListIterator iter;
	Widget *result;
	DataObject *child;
	DataObjectField *type;
	Application *app;

	EMO_ASSERT_NULL(w != NULL, "widget focus whoch oneNF missing widget")

	if (widget_hasFocus(w)) {
		return w;
	}

	type = dataobject_getValue(w, "type");
	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					result = widget_focusWhichOneNF(child);
					if (result)
						return result;
				}
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			result = widget_focusWhichOneNF(child);
			if (result)
				return result;
		}
	} else if (dataobjectfield_isString(type, "stack")) {
		child = stackwidget_focusStackObject(w);
		if (child == NULL)
			goto normal_focus;
		result = widget_focusWhichOneNF(child);
		if (result)
			return result;
	} else {
normal_focus:
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			result = widget_focusWhichOneNF((Widget *)listIterator_item(&iter));
			if (result) {
				/*listIterator_delete(iter);*/
				return result;
			}
			listIterator_next(&iter);
		}
	}

	/*listIterator_delete(iter);*/

	return NULL;
}

Widget *widget_focusWhichOne(Widget *w)
{
	ListIterator iter;
	Widget *result;
	DataObject *child;
	DataObjectField *type;
	Application *app;

	EMO_ASSERT_NULL(w != NULL, "widget focus whoch one missing widget")
	
	if (widget_hasFocus(w)) {
		widget_setFocus(w, 0);
		return w;
	}

	type = dataobject_getValue(w, "type");
	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					result = widget_focusWhichOne(child);
					if (result)
						return result;
				}
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			result = widget_focusWhichOne(child);
			if (result)
				return result;
		}
	} else if (dataobjectfield_isString(type, "stack")) {
		child = stackwidget_focusStackObject(w);
		if (child == NULL)
			goto normal_focus;
		result = widget_focusWhichOne(child);
					if (result)
						return result;
	} else {
normal_focus:
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			result = widget_focusWhichOne((Widget *)listIterator_item(&iter));
			if (result) {
				/*listIterator_delete(iter);*/
				return result;
			}
			listIterator_next(&iter);
		}
	}

	/*listIterator_delete(iter);*/

	return NULL;
}

static void widget_markDirtyChild(Widget *w)
{
	ListIterator iter;
	DataObject *child;
	DataObjectField *type;
	Application *app;

	EMO_ASSERT(w != NULL, "widget mark dirty child missing widget")

	dataobject_setDirty(w);

	type = dataobject_getValue(w, "type");
	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					widget_markDirtyChild(child);
				}
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w)
			widget_markDirtyChild(child);
	} else {
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			widget_markDirtyChild((Widget *)listIterator_item(&iter));
			listIterator_next(&iter);
		}
	}
	/*listIterator_delete(iter);*/
}

void widget_markDirty(Widget *w)
{
	Widget *parent;
	/*DataObjectField *field;*/

	EMO_ASSERT(w != NULL, "widget mark dirty missing widget")

	widget_markDirtyChild(w);

	parent = w;
	while (parent->parent != NULL) {
		/*field = dataobject_getValue(parent, "type");
		if (dataobjectfield_isString(field, "set") ||
				dataobjectfield_isString(field, "stack"))
			widget_markDirtyChild(parent);*/
		parent = parent->parent;
		dataobject_setDirty(parent);
	}
}

Widget *widget_findStringField(Widget *w, const char *key, const char *value)
{
	DataObjectField *field, *type;
	DataObject *child;
	ListIterator iter;
	Widget *retval;
	Application *app;

	EMO_ASSERT_NULL(w != NULL, "widget find string field missing widget")
	EMO_ASSERT_NULL(key != NULL, "widget find string field missing key")
	EMO_ASSERT_NULL(value != NULL, "widget find string field missing value")

	field = dataobject_getValue(w, key);
	if (dataobjectfield_isString(field, value))
		return w;

	type = dataobject_getValue(w, "type");
	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL)
					return widget_findStringField(child, key, value);
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w)
				return widget_findStringField(child, key, value);
	} else {
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			retval = widget_findStringField((Widget *)listIterator_item(&iter), key, value);
			if (retval != NULL) {
				/*listIterator_delete(iter);*/
				return retval;
			}
			listIterator_next(&iter);
		}
	}
	/*listIterator_delete(iter);*/

	return NULL;
}

Widget *widget_findStringFieldParent(Widget *w, const char *key, const char *value)
{
	DataObjectField *field;

	EMO_ASSERT_NULL(w != NULL, "widget find string field missing widget")
	EMO_ASSERT_NULL(key != NULL, "widget find string field missing key")
	EMO_ASSERT_NULL(value != NULL, "widget find string field missing value")

	field = dataobject_getValue(w, key);
	if (dataobjectfield_isString(field, value))
		return w;

	if (w->parent == NULL)
		return NULL;

	return widget_findStringFieldParent(w->parent, key, value);
}

static void widget_layoutMeasureFinal(Widget *w, Style *s)
{
	DataObjectField *type;
	WidgetRenderer *wr;
	DataObject *dobj, *child;
	const char *className, *id;
	ListIterator iter;
	IPoint p;
	Application *app;
	Style *style, *childStyle;
	int wentUp = 0;

	EMO_ASSERT(w != NULL, "widget layout measure final missing widget")
	EMO_ASSERT(s != NULL, "widget layout measure final missing style")

	/* get style default */
	dobj = widget_getDataObject(w);
	className = widget_getClass(w);
	id = widget_getID(w);
	type = dataobject_getValue(w, "type");
	style = style_getID(s, type == NULL ? NULL : type->field.string, id,
			widget_hasFocus(w), &wentUp);

	if (wentUp)
		childStyle = s;
	else
		childStyle = style;

	wr = NULL;
	style_getRenderer(style, w, "renderer", &wr);
	if (wr != NULL && wr->measure != NULL && strcmp(type->field.string, "text") == 0) {
		p.x = w->box.width;
		wr->measure(wr, s, w, dobj, &p);
		/*w->box.width = p.x;*/
		w->box.height = p.y;
		/*dataobject_setLayoutClean(w, LAYOUT_DIRTY_WIDTH);*/
		dataobject_setLayoutClean(w, LAYOUT_DIRTY_HEIGHT);
	}

	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				style = application_getCurrentStyle(app);
				if (style == NULL)
					style = manager_getRootStyle();
				if (child != NULL)
					widget_layoutMeasureFinal(child, style);
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w)
			widget_layoutMeasureFinal(child, style);
	} else {
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			widget_layoutMeasureFinal(listIterator_item(&iter), childStyle);
			listIterator_next(&iter);
		}
	}
}

static void widget_layoutMeasureAbsolute(Widget *w, Style *s)
{
	DataObjectField *sField, *type;
	WidgetRenderer *wr;
	DataObject *dobj, *child;
	Application *app;
	const char *className, *id;
	ListIterator iter;
	IPoint p;
	int slen, tmpint, wentUp = 0;
	Style *style, *childStyle;

	EMO_ASSERT(w != NULL, "widget layout measure absolute missing widget")
	EMO_ASSERT(s != NULL, "widget layout measure absolute missing style")

	/* get style default */
	dobj = widget_getDataObject(w);
	className = widget_getClass(w);
	id = widget_getID(w);
	type = dataobject_getValue(w, "type");
	style = style_getID(s, type == NULL ? NULL : type->field.string, id,
			widget_hasFocus(w), &wentUp);

	if (wentUp)
		childStyle = s;
	else
		childStyle = style;

	wr = NULL;
	style_getRenderer(style, w, "renderer", &wr);
	if (wr != NULL && wr->measure != NULL && !dataobjectfield_isString(type, "text")) {
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
			} else if (slen > 0) { \
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

	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				style = application_getCurrentStyle(app);
				if (style == NULL)
					style = manager_getRootStyle();
				if (child != NULL)
					widget_layoutMeasureAbsolute(child, style);
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w)
			widget_layoutMeasureAbsolute(child, style);
	} else {
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			widget_layoutMeasureAbsolute(listIterator_item(&iter), childStyle);
			listIterator_next(&iter);
		}
	}
}

void widget_layoutForceResolveParent(Widget *w, unsigned int flag)
{
	DataObjectField *field;

	EMO_ASSERT(w != NULL, "widget layout force resolve parent missing widget")

	if (w->parent == NULL)
		return;

	field = dataobject_getValue(w, "type");
	if (dataobjectfield_isString(field, "view"))
		return;

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
	int sumWidth, sumHeight, sumNew;
	int rowWidth, rowHeight;
	DataObject *child;
	DataObjectField *sField;
	ListIterator iter;
	int tmpint, slen, isset;
	Application *app;
	WidgetPacking pack = WP_VERTICAL;
	int widthStillDirty, heightStillDirty;

	EMO_ASSERT(w != NULL, "widget layout measure relative missing widget")

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
	rowWidth = 0;
	rowHeight = 0;
	isset = 0;
	widthStillDirty = 0;
	heightStillDirty = 0;

	sField = dataobject_getValue(w, "type");
	/*if (!widget_typeNoChildRender(sField)) {*/
		pack = widget_getPacking(w);
		if (dataobjectfield_isString(sField, "frame")) {
			child = widget_getDataObject(w);
			if (child != NULL && child != w) {
				app = manager_applicationForDataObject(child);
				if (app != NULL) {
					child = application_getCurrentScreen(app);
					if (child != NULL) {
						isset = 1;
						goto start_child;
					}
				}
			}
		} else if (dataobjectfield_isString(sField, "reference")) {
			child = widget_getDataObject(w);
			if (child != NULL && child != w) {
				isset = 1;
				goto start_child;
			}
		}
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			if (dataobjectfield_isString(sField, "set")) {
				child = setwidget_activeItem(w);
				isset = 1;
				if (child == NULL)
					break;
			} else {
				child = (DataObject *)listIterator_item(&iter);
			}
start_child:
			widget_resolveMeasureRelative(child);
			if (dataobject_isLayoutDirty(child, LAYOUT_DIRTY_WIDTH))
				widthStillDirty = 1;
			if (dataobject_isLayoutDirty(child, LAYOUT_DIRTY_HEIGHT))
				heightStillDirty = 1;
			if (pack == WP_HGRID) {
				sumNew = child->box.width + child->margin.x + child->margin.width;
				if (rowWidth+sumNew > w->box.width) {
					if (rowWidth > sumWidth)
						sumWidth = rowWidth;
					rowWidth = 0;
					sumHeight += rowHeight;
					rowHeight = 0;
				}
				rowWidth += sumNew;

				sumNew = child->box.height + child->margin.y + child->margin.height;
				if (sumNew > rowHeight)
					rowHeight = sumNew;
			} else if (pack == WP_VGRID) {
				sumNew = child->box.height + child->margin.y + child->margin.height;
				if (rowHeight+sumNew > w->box.height) {
					if (rowHeight > sumHeight)
						sumHeight = rowHeight;
					rowHeight = 0;
					sumWidth += rowWidth;
					rowWidth = 0;
				}
				rowHeight += sumNew;

				sumNew = child->box.width + child->margin.x + child->margin.width;
				if (sumNew > rowWidth)
					rowWidth = sumNew;
			} else if (pack == WP_HORIZONTAL) {
				sumWidth += child->box.width + child->margin.x + child->margin.width;
				sumNew = child->box.height + child->margin.y + child->margin.height;
				sumHeight = sumNew > sumHeight ? sumNew : sumHeight;
			} else {
				sumNew = child->box.width + child->margin.x + child->margin.width;
				sumWidth = sumNew > sumWidth ? sumNew : sumWidth;
				sumHeight += child->box.height + child->margin.y + child->margin.height;
			}
			if (isset)
				break;
			listIterator_next(&iter);
		}
		/*listIterator_delete(iter);*/
	/*}*/

	if (rowHeight > 0 || rowWidth > 0) {
		if (pack == WP_HGRID) {
			if (rowWidth > sumWidth)
				sumWidth = rowWidth;
			sumHeight += rowHeight;
		} else {
			if (rowHeight > sumHeight)
				sumHeight = rowHeight;
			sumWidth += rowWidth;
		}
	}

	if (!widthStillDirty && dataobject_isLayoutDirty(w, LAYOUT_DIRTY_WIDTH)) {
		w->box.width = sumWidth;
		dataobject_setLayoutClean(w, LAYOUT_DIRTY_WIDTH);
	}
	if (!heightStillDirty && dataobject_isLayoutDirty(w, LAYOUT_DIRTY_HEIGHT)) {
		sField = dataobject_getValue(w, "type");
		if (!dataobjectfield_isString(sField, "text")) {
			w->box.height = sumHeight;
			dataobject_setLayoutClean(w, LAYOUT_DIRTY_HEIGHT);
		}
	}

	w->childBox.width = sumWidth;
	w->childBox.height = sumHeight;
}

void widget_resolveMargin(Widget *w, Style *s)
{
	DataObjectField *field, *type;
	WidgetRenderer *wr;
	ListIterator iter;
	const char *className, *id;
	DataObject *dobj, *child;
	Application *app;
	Style *style, *childStyle;
	int wentUp;

	EMO_ASSERT(w != NULL, "widget resolve margin missing widget")
	EMO_ASSERT(s != NULL, "widget resolve margin missing style")

	/* Apply margin from style */
	dobj = widget_getDataObject(w);
	className = widget_getClass(w);
	id = widget_getID(w);
	type = dataobject_getValue(w, "type");

#if 0
	/* for debugging css on buttons */
	if (dataobjectfield_isString(type, "label")) {
		parent = dataobject_parent(dobj);
		if (parent != NULL) {
			output = dataobject_getValue(parent, "type");
			if (dataobjectfield_isString(output, "button")) {
				emo_printf("button->label" NL);
			}
		}
	}
#endif	

	style = style_getID(s, type == NULL ? NULL : type->field.string, id,
			widget_hasFocus(w), &wentUp);

	if (wentUp)
		childStyle = s;
	else
		childStyle = style;

	wr = NULL;
	style_getRenderer(style, w, "renderer", &wr);
	if (wr != NULL)
		wr->margin(wr, style, w, dobj, &w->margin);

	/* Apply specified margin */
	field = style_getPropertyAsInt(style, w, "margin");
	if (field != NULL) {
		w->margin.x = field->field.integer;
		w->margin.y = field->field.integer;
		w->margin.width = field->field.integer;
		w->margin.height = field->field.integer;
	} else {
		field = style_getPropertyAsInt(style, w, "marginleft");
		if (field != NULL)
			w->margin.x = field->field.integer;
		field = style_getPropertyAsInt(style, w, "marginright");
		if (field != NULL)
			w->margin.width = field->field.integer;
		field = style_getPropertyAsInt(style, w, "margintop");
		if (field != NULL)
			w->margin.y = field->field.integer;
		field = style_getPropertyAsInt(style, w, "marginbottom");
		if (field != NULL)
			w->margin.height = field->field.integer;
	}

	/* dont do no-child-renderable decend */
#if 0
	if (widget_typeNoChildRender(type))
		return;
#endif

	if (dataobjectfield_isString(type, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				style = application_getCurrentStyle(app);
				if (style == NULL)
					style = manager_getRootStyle();
				if (child != NULL)
					widget_resolveMargin(child, style);
			}
		}
	} else if (dataobjectfield_isString(type, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w)
			widget_resolveMargin(child, style);
	} else {
		dataobject_childIterator(w, &iter);
		while (!listIterator_finished(&iter)) {
			widget_resolveMargin(listIterator_item(&iter), s);
			listIterator_next(&iter);
		}
	}

	/*listIterator_delete(iter);*/
}

void widget_resolvePosition(Widget *w)
{
	int xpos, ypos, width, height;
	int oxpos, oypos, rowMax, rowi;
	ListIterator iter;
	WidgetAlignment alignment;
	WidgetPacking packing;
	Widget *cw;
	int positionStatic = 0;
	DataObjectField *field;
	DataObject *singleChild = NULL, *child;
	Application *app;

	EMO_ASSERT(w != NULL, "widget resolve position missing widget")

	xpos = w->box.x+w->margin.x;
	oxpos = xpos;
	ypos = w->box.y+w->margin.y;
	oypos = ypos;
	rowMax = 0;
	width = w->box.width;
	height = w->box.height;
	packing = widget_getPacking(w);

	field = dataobject_getValue(w, "type");

#if 0
	if (widget_typeNoChildRender(field))
		return;
#endif

	if (dataobjectfield_isString(field, "stack"))
		positionStatic = 1;

	if (dataobjectfield_isString(field, "set")) {
		singleChild = setwidget_activeItem(w);
		if (singleChild == NULL)
			return;
	}

	if (dataobjectfield_isString(field, "frame")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			app = manager_applicationForDataObject(child);
			if (app != NULL) {
				child = application_getCurrentScreen(app);
				if (child != NULL) {
					singleChild = child;
					cw = child;
					goto start_child;
				}
			}
		}
	} else if (dataobjectfield_isString(field, "reference")) {
		child = widget_getDataObject(w);
		if (child != NULL && child != w) {
			singleChild = child;
			cw = child;
			goto start_child;
		}
	}

	dataobject_childIterator(w, &iter);
	while (!listIterator_finished(&iter)) {
		cw = (Widget *)listIterator_item(&iter);
		if (singleChild != NULL)
			cw = singleChild;
start_child:
		alignment = widget_getAlignment(cw);
		
		if (packing == WP_HGRID) {
			if (xpos + cw->box.width + cw->margin.x + cw->margin.width - w->box.x > width) {
				xpos = oxpos;
				ypos += rowMax;
				rowMax = 0;
			}

			cw->box.x = xpos;
			cw->box.y = ypos;

			xpos += cw->box.width + cw->margin.x + cw->margin.width;

			rowi = cw->box.height + cw->margin.y + cw->margin.height;
			if (rowi > rowMax)
				rowMax = rowi;
		} else if (packing == WP_VGRID) {
			if (ypos + cw->box.height + cw->margin.y + cw->margin.height - w->box.y > height) {
				ypos = oypos;
				xpos += rowMax;
				rowMax = 0;
			}

			cw->box.x = xpos;
			cw->box.y = ypos;

			ypos += cw->box.height + cw->margin.y + cw->margin.height;

			rowi = cw->box.width + cw->margin.x + cw->margin.width;
			if (rowi > rowMax)
				rowMax = rowi;
		} else {
			switch (alignment) {
				case WA_LEFT:
				default:
					if (packing == WP_HORIZONTAL) {
						cw->box.x = xpos;
						cw->box.y = ypos;/*+cw->margin.y;*/
						if (!positionStatic)
							xpos += cw->box.width+cw->margin.x+cw->margin.width;
					} else {
						cw->box.x = xpos;/*+cw->margin.x;*/
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
		}
		widget_resolvePosition(cw);

		if (singleChild != NULL)
			return;
#if 0
		if (!positionStatic) {
			if (packing == WP_HORIZONTAL)
				xpos += 2;
			else
				ypos += 2;
		}
#endif
		listIterator_next(&iter);
	}

	if (dataobjectfield_isString(field, "scrolled"))
		scrolled_autoscroll(w);

	/*listIterator_delete(iter);*/
}

void widget_resolveLayoutRoot(Widget *w, Style *s, int resizeRoot)
{
	ListIterator iter;

	EMO_ASSERT(w != NULL, "widget resolve layout root missing widget")
	EMO_ASSERT(s != NULL, "widget resolve layout root missing style")

	dataobject_setLayoutDirtyAll(w);

	widget_resolveMargin(w, s);

	if (resizeRoot) {
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
	}

	/* measure those with explicit sizes or measurable content */
	dataobject_childIterator(w, &iter);
	while (!listIterator_finished(&iter)) {
		widget_layoutMeasureAbsolute(listIterator_item(&iter), s);
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/

	/*emo_printf("Root after absolute layout" NL);
	dataobject_debugPrint(w);*/

	/* measure those whos size is based on their children or parent */
	dataobject_childIterator(w, &iter);
	while (!listIterator_finished(&iter)) {
		widget_resolveMeasureRelative((DataObject *)listIterator_item(&iter));
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/

	/* measure anoying widgets that need to have some sizes resolved before the oters */
	dataobject_childIterator(w, &iter);
	while (!listIterator_finished(&iter)) {
		widget_layoutMeasureFinal((DataObject *)listIterator_item(&iter), s);
		listIterator_next(&iter);
	}

	/* measure those whos size is based on their children or parent */
	dataobject_childIterator(w, &iter);
	while (!listIterator_finished(&iter)) {
		widget_resolveMeasureRelative((DataObject *)listIterator_item(&iter));
		listIterator_next(&iter);
	}

	/* position widgets based on packing and alignment */
	widget_resolvePosition(w);

	/*dataobject_debugPrint(w);*/
}

void widget_setAlignment(Widget *w, WidgetAlignment a)
{
	DataObjectField *field;

	EMO_ASSERT(w != NULL, "widget set alignment missing widget")

	field = dataobject_getValue(w, "alignment");
	if (!field)
		dataobject_setValue(w, "alignment", dataobjectfield_int(a));
	else
		field->field.integer = a;
}

WidgetAlignment widget_getAlignment(Widget *w)
{
	DataObjectField *field, *newField;

	EMO_ASSERT_INT(w != NULL, WA_LEFT, "widget get alignment missing widget")
	
	field = dataobject_getValue(w, "alignment");
	if (!field)
		return (WidgetAlignment)WA_LEFT;
	if (field->type == DOF_STRING) {
		if (isdigit(field->field.string[0]))
			newField = dataobjectfield_int(atoi(field->field.string));
		else if (strcmp(field->field.string, "left") == 0 ||
				strcmp(field->field.string, "top") == 0)
			newField = dataobjectfield_int(WA_LEFT);
		else if (strcmp(field->field.string, "center") == 0)
			newField = dataobjectfield_int(WA_CENTER);
		else if (strcmp(field->field.string, "right") == 0 ||
				strcmp(field->field.string, "bottom") == 0)
			newField = dataobjectfield_int(WA_RIGHT);
		else
			newField = dataobjectfield_int(WA_LEFT);

		/*p_free(field->field.string);
		p_free(field);*/
		dataobject_setValue(w, "alignment", newField);
		field = newField;
	}
	return (WidgetAlignment)field->field.integer;
}

void widget_getClipRectangle(Widget *w, Rectangle *rect)
{
	EMO_ASSERT(w != NULL, "widget get clip rect missing widget")
	EMO_ASSERT(rect != NULL, "widget get clip rect missing rect")

	rect->x = w->box.x + w->margin.x;
	rect->y = w->box.y + w->margin.y;
	rect->width = w->box.width;
	rect->height = w->box.height;
}

#if 0
int widget_typeNoChildRender(DataObjectField *field)
{
	EMO_ASSERT_INT(field != NULL, 0, "widget type no child render missing field")

	if (field != NULL && field->type == DOF_STRING &&
			strcmp(field->field.string, "array") == 0)
		return 1;
	return 0;
}
#endif
