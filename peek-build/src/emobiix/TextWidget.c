#include "EntryWidget.h"

#include "DataObject.h"
#include "Widget.h"
#include "lgui.h"
#include "Style.h"
#include "WidgetRenderer.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

extern Font *defaultFont;

/* text renderer */
static void text_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	DataObject *text;
	DataObjectField *field;
	Font *f;
	Color c;
	const char *dtype;
	const char *ltype;
	const char *str;
	int percent, offset;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	dtype = (const char *)dataobject_getValue(dobj, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)defaultFont;/*style_getProperty(s, NULL, ltype, dtype, "font");*/
	c.value = 0;
	style_getColor(s, w, "font-color", &c.value);
	text = widget_getDataObject(w);
	field = dataobject_getValue(text, "data");
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
	const char *dtype;
	const char *ltype;
	const char *str;

	dtype = (const char *)dataobject_getValue(dobj, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)defaultFont;/*style_getProperty(s, NULL, ltype, dtype, "font");*/
	str = (const char *)dataobject_getValue(dobj, "data")->field.data.bytes;

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

	return output;
}