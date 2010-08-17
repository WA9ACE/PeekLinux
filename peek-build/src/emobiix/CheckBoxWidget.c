#include "CheckBoxWidget.h"

#include "DataObject.h"
#include "Widget.h"
#include "lgui.h"
#include "Style.h"
#include "WidgetRenderer.h"
#include "KeyMappings.h"

#include "p_malloc.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int checkboxWidget_handleKey(Widget *w, unsigned int key, Style *s)
{
	DataObject *dobj;
	DataObjectField *field;
	int isChecked;

	if (key != EKEY_ACTIVATE)
		return 0;

	dobj = widget_getDataObject(w);
	field = dataobject_getValue(dobj, "data");
	if (field == NULL) {
		field = dataobjectfield_string("0");
		dataobject_setValue(dobj, "data", field);
	}
	isChecked = dataobjectfield_isTrue(field);

	dataobjectfield_setBoolean(field, !isChecked);

	dataobject_setIsModified(w, 1);
#if 0
	lgui_clip_identity();
	widget_getClipRectangle(w, &rect);
	lgui_clip_set(&rect);
	lgui_push_region();
	widget_markDirty(w);
	style_renderWidgetTree(s, dataobject_superparent(w));
#endif

	return 1;
}

/* checkbox renderer */
static void checkbox_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin, checkBox;
	DataObject *text;
	DataObjectField *field;
	Color checkColor;
	WidgetRenderer *iwr;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	checkBox.x = box->x+3 + margin->x;
	checkBox.y = box->y+3 + margin->y;
	checkBox.width = box->width-6;
	checkBox.height = box->height-6;

	iwr = NULL;
	style_getRenderer(s, w, "box-renderer", &iwr);
	if (iwr != NULL)
		iwr->render(wr, s, w, dobj);

	text = widget_getDataObject(w);
	field = dataobject_getValue(text, "data");

	if (dataobjectfield_isTrue(field) && 
			style_getColor(s, w, "check-color", &checkColor.value) != NULL) {
		lgui_hline(checkBox.x, checkBox.y, checkBox.width, checkBox.height,
				checkColor.rgba.red, checkColor.rgba.green, checkColor.rgba.blue);
	}
}

static void checkbox_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	p->x = 12;
	p->y = 12;
}

WidgetRenderer *widgetrenderer_checkbox(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = checkbox_renderer;
	output->measure = checkbox_measure;
	output->margin = widgetrenderer_zeroMargin;

	return output;
}

