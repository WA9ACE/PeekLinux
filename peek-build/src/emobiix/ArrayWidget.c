#include "ArrayWidget.h"

#include "lgui.h"
#include "Debug.h"
#include "DataObject_private.h"

#include "p_malloc.h"

#include <stdio.h>

void array_expand(Widget *w)
{
	DataObject *shim;
	DataObject *dobj, *rec, *arrtemplate;
	ListIterator iter;
	DataObjectField *direction, *startfield;
	DataObjectField *filterfield, *filtervalue, *filteredObj;
	int childCount, startNumber, recordIdx;

	EMO_ASSERT(w != NULL,
			"Attempting to expand array with NULL Widget")

	/*emo_printf("Array expand" NL);*/

	childCount = dataobject_getChildCount(w);
	if (childCount == 0)
		return;
	if (childCount != 1) {
		emo_printf("Array shim comprises multiple root widgets, it shouldnt" NL);
		return;
	}

	dobj = widget_getDataObject(w);
	if (dobj == w)
		return;

	filterfield = dataobject_getEnum(w, EMO_FIELD_FILTERFIELD);
	filtervalue = dataobject_getEnum(w, EMO_FIELD_FILTERVALUE);
	if (filtervalue == NULL || filtervalue->type != DOF_STRING)
		filterfield = NULL;
	
	for (list_begin(w->arrayChildren, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		dataobject_delete((DataObject *)listIterator_item(&iter));
	}
	list_delete(w->arrayChildren);
	w->arrayChildren = list_new();

	list_begin(w->children, &iter);
	arrtemplate = (DataObject *)listIterator_item(&iter);

	direction = dataobject_getEnum(w, EMO_FIELD_DIRECTION);
	startfield = dataobject_getEnumAsInt(w, EMO_FIELD_STARTCOUNT);
	
	if (startfield != NULL && startfield->type == DOF_INT)
		startNumber = startfield->field.integer;
	else
		startNumber = 30;

	if (dataobjectfield_isString(direction, "prepend"))
		list_rbegin(dobj->children, &iter);
	else
		list_begin(dobj->children, &iter);

	recordIdx = 0;
	for (; !listIterator_finished(&iter); listIterator_next(&iter)) {
		rec = (DataObject *)listIterator_item(&iter);

		if (filterfield != NULL) {
			if (filterfield->type == DOF_STRING)
				filteredObj = dataobject_getValue(rec, filterfield->field.string);
			else if (filterfield->type == DOF_INT)
				filteredObj = dataobject_getEnum(rec, filterfield->field.integer);
			else {
				EMO_ASSERT(0, "field neither string nor int");
			}
			if (!dataobjectfield_isString(filteredObj, filtervalue->field.string))
				goto skip_record;
		}

		shim = dataobject_copyTree(arrtemplate);
		widget_setDataObjectArray(shim, rec);
		list_append(w->arrayChildren, (void *)shim);
		shim->parent = w->parent;

		dataobject_setIsModified(shim, 1);
		widget_markDirty(shim);

skip_record:
		++recordIdx;
		if (recordIdx >= startNumber)
			break;
	}

	//dataobject_debugPrint(w);
}

#if 0
static void array_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj)
{
	Rectangle *box, *margin;
	Rectangle *shimBox, *shimMargin;
	Rectangle *childBox, *childMargin;
	WidgetRenderer *boxwr;
	DataObjectField *startindex, *focusindex, *endindex;
	DataObject *rec, *shim, *wchild;
	WidgetPacking packing;
	ListIterator iter;
	int sfHeight, sfStartY, recordCount;
	int startidx, focusidx, endidx, idx, canFocus;

	EMO_ASSERT(wr != NULL,
			"Attempting to render array with NULL WidgetRenderer")
	EMO_ASSERT(s != NULL,
			"Attempting to render array with NULL Style")
	EMO_ASSERT(w != NULL,
			"Attempting to render array with NULL Widget")
	EMO_ASSERT(dobj != NULL,
			"Attempting to render array with NULL dobj")

	boxwr = widgetrenderer_box();
	boxwr->render(boxwr, s, w, dobj);

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	packing = widget_getPacking(w);

	/* start index */
	startindex = dataobject_getEnum(w, EMO_FIELD_STARTINDEX);
	if (startindex == NULL) {
		startindex = dataobjectfield_int(0);
		dataobject_setEnum(w, EMO_FIELD_STARTINDEX, startindex);
	}
	startidx = startindex->field.integer;

	/* focus index */
	focusindex = dataobject_getEnum(w, EMO_FIELD_FOCUSINDEX);
	if (focusindex == NULL) {
		focusindex = dataobjectfield_int(-1);
		dataobject_setEnum(w, EMO_FIELD_FOCUSINDEX, focusindex);
	}
	focusidx = focusindex->field.integer;

	/*emo_printf("start: %d, focus: %d" NL, startindex->field.integer,
			focusindex->field.integer);*/

	dataobject_childIterator(w, &iter);
	wchild = listIterator_item(&iter);
	/*listIterator_delete(iter);*/
	if (wchild == NULL) {
		emo_printf("wchild was NULL" NL);	
		return;
	}
	
	canFocus = widget_canFocus(wchild);

	shim = dataobject_new();
	dataobject_setEnum(shim, EMO_FIELD_TYPE, dataobjectfield_string("box"));

	shimMargin = widget_getMargin(shim);
	shimMargin->x = 0;
	shimMargin->y = 0;
	shimMargin->width = 0;
	shimMargin->height = 0;

	shimBox = widget_getBox(shim);
	shimBox->x = box->x+margin->x;
	shimBox->y = box->y+margin->y;
	shimBox->width = box->width-9;
	shimBox->height = box->height;

	recordCount = dataobject_getChildCount(dobj);
	if (recordCount == -2) {
		recordCount = 2;	
		goto skip_record_draw;
	}
	if (recordCount < 2)
		recordCount = 2;
	emo_printf("Record count %d" NL, recordCount);
	idx = 0;
	dataobject_pack(shim, wchild);
	lgui_clip_push();
	lgui_clip_and(box);
	for (dataobject_childIterator(dobj, &iter);
			!listIterator_finished(&iter); listIterator_next(&iter)) {
		/*emo_printf("Drawing Array index %d" NL, idx);*/
		if (idx < startidx) {
			/*emo_printf("Skipping %d - %d" NL, idx, startidx);*/
			++idx;
			continue;
		}
		rec = (DataObject *)listIterator_item(&iter);
		if (idx == focusidx && canFocus)
			widget_setFocus(wchild, 1);
		else
			widget_setFocus(wchild, 0);
		widget_setDataObjectArray(shim, rec);
		widget_resolveLayoutRoot(shim, s, 0);
		/*dataobject_debugPrint(shim);*/
		widget_markDirty(shim);
		style_renderWidgetTree(s, shim);

		childBox = widget_getBox(wchild);
		childMargin = widget_getMargin(wchild);
		if (packing == WP_HORIZONTAL) {
			shimBox->x += childBox->width;
			if (shimBox->x > box->x+box->width)
				break;
		} else {
			shimBox->y += childBox->height;
			if (shimBox->y > box->y+box->height)
				break;
		}

		++idx;
	}
	/*listIterator_delete(iter);*/
	dataobject_setParent(wchild, w);
	lgui_clip_pop();
	endidx = idx;

	/* end index */
	endindex = dataobject_getEnum(w, EMO_FIELD_ENDINDEX);
	if (endindex == NULL) {
		endindex = dataobjectfield_int(endidx);
		dataobject_setEnum(w, EMO_FIELD_ENDINDEX, endindex);
	}
	endindex->field.integer = endidx;

skip_record_draw:

	dataobject_delete(shim);

	/* render scroll bar */
	lgui_vline(box->x+margin->x+box->width-8, box->y+margin->y+1, box->height-1, 4, 0x44, 0x44, 0xFF);
	sfHeight = (box->height-1) / recordCount;
	if (sfHeight < 5)
		sfHeight = 5;
	sfStartY = ((box->height-1) - sfHeight) / (recordCount-1);
	if (sfStartY == 0)
		sfStartY = 0;
	sfStartY *= focusidx+1;
	lgui_vline(box->x+margin->x+box->width-8, box->y+margin->y+1+sfStartY, sfHeight, 4, 0xCC, 0xCC, 0xFF);
}

static void array_margin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
	EMO_ASSERT(wr != NULL,
			"Attempting to margin array with NULL WidgetRenderer")
	EMO_ASSERT(s != NULL,
			"Attempting to margin array with NULL Style")
	EMO_ASSERT(w != NULL,
			"Attempting to margin array with NULL Widget")
	EMO_ASSERT(dobj != NULL,
			"Attempting to margin array with NULL dobj")
	EMO_ASSERT(dobj != NULL,
			"Attempting to margin array with NULL Rectangle")

	output->x = 0;
	output->y = 0;
	output->width = 0;
	output->height = 0;
}

WidgetRenderer *widgetrenderer_array(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = array_renderer;
	output->measure = NULL;
	output->margin = array_margin;

	return output;
}

int arraywidget_focusNext(Widget *w, int *alreadyUnset, int *alreadySet)
{
	DataObjectField *focusindex, *startindex, *endindex;
	DataObject *record;

	EMO_ASSERT_INT(w != NULL, 0,
			"Focus next on array with NULL widget")
	EMO_ASSERT_INT(alreadyUnset != NULL, 0,
			"Focus next on array with NULL alreadyUnset")
	EMO_ASSERT_INT(alreadySet != NULL, 0,
			"Focus next on array with NULL alreadySet")

	startindex = dataobject_getEnum(w, EMO_FIELD_STARTINDEX);
	if (startindex == NULL) {
		startindex = dataobjectfield_int(0);
		dataobject_setEnum(w, EMO_FIELD_STARTINDEX, startindex);
	}

	/* focus index */
	focusindex = dataobject_getEnum(w, EMO_FIELD_FOCUSINDEX);
	if (focusindex == NULL) {
		focusindex = dataobjectfield_int(-1);
		dataobject_setEnum(w, EMO_FIELD_FOCUSINDEX, focusindex);
	}

	/* end index */
	endindex = dataobject_getEnum(w, EMO_FIELD_ENDINDEX);
	if (endindex == NULL) {
		endindex = dataobjectfield_int(dataobject_getChildCount(w));
		dataobject_setEnum(w, EMO_FIELD_ENDINDEX, endindex);
	}

	if (*alreadyUnset == 0 && focusindex->field.integer < 0) {
		emo_printf("ArrayWidget return at 1" NL);
		return 0;
	}

	record = widget_getDataObject(w);

	++focusindex->field.integer;
	if (focusindex->field.integer >= dataobject_getChildCount(record)) {
		focusindex->field.integer = -1;
		widget_setFocus(w, 0);
		emo_printf("ArrayWidget return at 2" NL);
		return 1;
	}

	if (focusindex->field.integer == endindex->field.integer)
		++startindex->field.integer;
	else if (focusindex->field.integer < startindex->field.integer)
		startindex->field.integer = focusindex->field.integer;

	*alreadySet = 1;
	widget_setFocus(w, 1);

	emo_printf("Focusing on %d now" NL, focusindex->field.integer);

	emo_printf("ArrayWidget return at 3" NL);
	return 2;
}

int arraywidget_focusPrev(Widget *w)
{
	DataObjectField *focusindex, *startindex, *endindex;
	DataObject *record;

	EMO_ASSERT_INT(w != NULL, 0,
			"Focus prev on array with NULL widget")

	startindex = dataobject_getEnum(w, EMO_FIELD_STARTINDEX);
	if (startindex == NULL) {
		startindex = dataobjectfield_int(0);
		dataobject_setEnum(w, EMO_FIELD_STARTINDEX, startindex);
	}

	/* focus index */
	focusindex = dataobject_getEnum(w, EMO_FIELD_FOCUSINDEX);
	if (focusindex == NULL) {
		focusindex = dataobjectfield_int(-1);
		dataobject_setEnum(w, EMO_FIELD_FOCUSINDEX, focusindex);
	}

	/* end index */
	endindex = dataobject_getEnum(w, EMO_FIELD_ENDINDEX);
	if (endindex == NULL) {
		endindex = dataobjectfield_int(dataobject_getChildCount(w));
		dataobject_setEnum(w, EMO_FIELD_ENDINDEX, endindex);
	}
	
	emo_printf("Focus was %d" NL, focusindex->field.integer);

	record = widget_getDataObject(w);

	--focusindex->field.integer;
	if (focusindex->field.integer < 0) {
		focusindex->field.integer = -1;
		return 0;
	}

	if (focusindex->field.integer <= startindex->field.integer)
		startindex->field.integer = focusindex->field.integer;
	else if (focusindex->field.integer > endindex->field.integer)
		startindex->field.integer = focusindex->field.integer;

	emo_printf("Focus now %d" NL, focusindex->field.integer);
	widget_setFocus(w, 1);

	return 1;
}
#endif

