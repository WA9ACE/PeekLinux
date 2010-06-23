#include "ArrayWidget.h"

#include "lgui.h"
#include "Debug.h"

#include "p_malloc.h"

#include <stdio.h>

static void array_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj)
{
	Rectangle *box, *margin;
	Rectangle *shimBox, *shimMargin;
	Rectangle *childBox, *childMargin;
	WidgetRenderer *boxwr;
	DataObjectField *startindex, *focusindex;
	DataObject *rec, *shim, *wchild;
	WidgetPacking packing;
	ListIterator *iter;
	int sfHeight, sfStartY, recordCount;
	int startidx, focusidx, idx, canFocus;

	boxwr = widgetrenderer_gradboxr();
	boxwr->render(boxwr, s, w, dobj);

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	packing = widget_getPacking(w);

	/* start index */
	startindex = dataobject_getValue(w, "startindex");
	if (startindex == NULL) {
		startindex = dataobjectfield_int(0);
		dataobject_setValue(w, "startindex", startindex);
	}
	startidx = startindex->field.integer;

	/* focus index */
	focusindex = dataobject_getValue(w, "focusindex");
	if (focusindex == NULL) {
		focusindex = dataobjectfield_int(-1);
		dataobject_setValue(w, "focusindex", startindex);
	}
	focusidx = focusindex->field.integer;

	iter = dataobject_childIterator(w);
	wchild = listIterator_item(iter);
	listIterator_delete(iter);
	if (wchild == NULL)
		return;
	
	canFocus = widget_canFocus(wchild);

	shim = dataobject_new();
	dataobject_setValue(shim, "type", dataobjectfield_string("box"));

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
	idx = 0;
	dataobject_pack(shim, wchild);
	lgui_clip_push();
	lgui_clip_and(box);
	for (iter = dataobject_childIterator(dobj);
			!listIterator_finished(iter); listIterator_next(iter)) {
		/*emo_printf("Drawing Array index %d" NL, idx);*/
		rec = (DataObject *)listIterator_item(iter);
		if (idx == focusidx && canFocus)
			widget_setFocus(wchild, 1);
		else
			widget_setFocus(wchild, 0);
		widget_setDataObjectArray(shim, rec);
		widget_resolveLayoutRoot(shim, s, 0);
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
	listIterator_delete(iter);
	dataobject_setParent(wchild, w);
	lgui_clip_pop();

	dataobject_delete(shim);

	/* render scroll bar */
	lgui_vline(box->x+margin->x+box->width-8, box->y+margin->y+1, box->height-1, 4, 0x44, 0x44, 0xFF);
	sfHeight = (box->height-1) / recordCount;
	if (sfHeight < 5)
		sfHeight = 5;
	sfStartY = ((box->height-1) - sfHeight) / recordCount;
	if (sfStartY == 0)
		sfStartY = 0;
	sfStartY *= startidx;
	lgui_vline(box->x+margin->x+box->width-8, box->y+margin->y+1+sfStartY, sfHeight, 4, 0xCC, 0xCC, 0xFF);

#if 0
	Rectangle *box, *margin;
	DataObject *text;
	DataObjectField *field;
	Font *f;
	Color c, background, line;
	const char *dtype;
	const char *ltype;
	const char *str;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	dtype = (const char *)dataobject_getValue(dobj, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)style_getProperty(s, NULL, ltype, dtype, "font");
	c.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "color");
	background.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "background");
	if (widget_hasFocus(w))
		line.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "focusline");
	else
		line.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "line");
	text = widget_getDataObject(w);
	field = dataobject_getValue(text, "data");
	if (field != NULL) {
		str = field->field.string;
	} else {
		str = "";
	}

	lgui_roundedbox_fill(box->x+margin->x, box->y+margin->y, box->width-1, box->height-1, 7,
			background.rgba.red, background.rgba.green, background.rgba.blue);
	lgui_roundedbox_line(box->x+margin->x, box->y+margin->y, box->width-1, box->height-1, 7,
			line.rgba.red, line.rgba.green, line.rgba.blue);

	lgui_draw_font(box->x+4+margin->x, box->y+margin->y, str, f, c);
#endif
}

static void array_margin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
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

int arraywidget_focusNext(Widget *w)
{
	DataObjectField *focusindex;

	/* focus index */
	focusindex = dataobject_getValue(w, "focusindex");
	if (focusindex == NULL)
		return 0;
	++focusindex->field.integer;

	emo_printf("Focusing on %d now" NL, focusindex->field.integer);

	return 1;
}