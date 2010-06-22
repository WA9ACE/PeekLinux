#include "ArrayWidget.h"

#include "lgui.h"
#include "p_malloc.h"

#include <stdio.h>

static void array_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj)
{
	Rectangle *box, *margin;
	WidgetRenderer *boxwr;

	boxwr = widgetrenderer_gradboxr();
	boxwr->render(boxwr, s, w, dobj);

	box = widget_getBox(w);
	margin = widget_getMargin(w);

	/* render scroll bar */
	lgui_vline(box->x+margin->x+box->width-8, box->y+margin->y+1, box->height-1, 4, 0x88, 0x88, 0xFF);

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