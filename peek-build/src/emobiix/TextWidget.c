#include "EntryWidget.h"

#include "DataObject.h"
#include "Widget.h"
#include "lgui.h"
#include "Style.h"
#include "WidgetRenderer.h"
#include "Debug.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

extern Font *defaultFont;

/* text renderer */
static void text_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	DataObject *text;
	DataObjectField *field, *sourceField;
	Font *f;
	Color c;
	const char *str;
	int percent, offset;

	EMO_ASSERT(wr != NULL, "text renderer missing renderer")
	EMO_ASSERT(s != NULL, "text renderer missing style")
	EMO_ASSERT(w != NULL, "text renderer missing widget")
	EMO_ASSERT(dobj != NULL, "text renderer missing dobj")

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	f = (Font *)defaultFont;/*style_getProperty(s, NULL, ltype, dtype, "font");*/
	c.value = 0;
	style_getColor(s, w, "font-color", &c.value);
	text = widget_getDataObject(w);
	sourceField = dataobject_getValue(w, "datafield");
	if (sourceField != NULL && sourceField->type == DOF_STRING)
		field = dataobject_getValue(dobj, sourceField->field.string);
	else
		field = dataobject_getValue(dobj, "data");
	if (field != NULL) {
		str = field->field.string;
	} else {
		str = "";
	}

	lgui_complex_draw_font(box->x+4+margin->x, box->y+margin->y, box->width-9, box->height, str,
			f, c, 0, 0, &percent, &offset, 1, NULL);
}

static void text_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	Font *f;
	const char *str;
	DataObjectField *sourceField, *field;

	EMO_ASSERT(wr != NULL, "text measure missing renderer")
	EMO_ASSERT(s != NULL, "text measure missing style")
	EMO_ASSERT(w != NULL, "text measure missing widget")
	EMO_ASSERT(dobj != NULL, "text measure missing dobj")
	EMO_ASSERT(p != NULL, "text measure missing the point")

	f = (Font *)defaultFont;/*style_getProperty(s, NULL, ltype, dtype, "font");*/
	/*str = (const char *)dataobject_getValue(dobj, "data")->field.data.bytes;*/
	sourceField = dataobject_getValue(w, "datafield");
	if (sourceField != NULL && sourceField->type == DOF_STRING)
		field = dataobject_getValue(dobj, sourceField->field.string);
	else
		field = dataobject_getValue(dobj, "data");
	if (field != NULL) {
		str = field->field.string;
	} else {
		str = "";
	}

	lgui_measure_font_complex(str, f, p);
	p->y += 4;
}

WidgetRenderer *widgetrenderer_text(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = text_renderer;
	output->measure = text_measure;
	output->margin = widgetrenderer_zeroMargin;
	output->postrender = NULL;

	return output;
}
