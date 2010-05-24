#include "WidgetRenderer.h"

#include "lgui.h"
#include "Point.h"

#include "p_malloc.h"
#include <stdio.h>

/* image renderer */
static void image_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	int width, height;
	PixelFormat pf;
	void *data;
	Color c;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	width = (int)dataobject_getValue(dobj, "width")->field.integer;
	height = (int)dataobject_getValue(dobj, "height")->field.integer;
	pf = (PixelFormat)dataobject_getValue(dobj, "pixelformat")->field.integer;
	data = dataobject_getValue(dobj, "data")->field.data.bytes;

	switch (pf) {
		case RGB565:
			lgui_blitRGB565(box->x+margin->x, box->y+margin->y,
					0, 0, width, height, data);
			break;
		case A4:
			c.value = (unsigned int)style_getProperty(s, NULL, "image", NULL, "color");
			lgui_luminence_A4_blitC(box->x+margin->x, box->y+margin->y, 0, 0,
					width, height, width, height, data, c);
			break;
		default:
			bal_printf("Unsupported image format\n");
	}
}

static void image_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *output)
{
	output->x = (int)dataobject_getValue(dobj, "width")->field.integer;
	output->y = (int)dataobject_getValue(dobj, "height")->field.integer;
}

WidgetRenderer *widgetrenderer_image(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = image_renderer;
	output->measure = image_measure;

	return output;
}

/* gradbox renderer */
static void gradbox_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	Gradient *g;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	g = (Gradient *)style_getProperty(s, NULL, "gradbox", NULL, "gradient");

	lgui_vertical_gradientG(g,
			box->x+margin->x, box->y+margin->y, box->width, box->height);
}

static void zero_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *output)
{
	output->x = 0;
	output->y = 0;
}

WidgetRenderer *widgetrenderer_gradbox(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = gradbox_renderer;
	output->measure = zero_measure;

	return output;
}
/* gradboxr renderer */
static void gradboxr_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	Gradient *g;
	const char *id;
	int radius;
	Color outline;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	id = widget_getID(w);
	if (widget_hasFocus(w)) {
		g = (Gradient *)style_getProperty(s, NULL, id, NULL, "focusgradient");
		outline.value = (unsigned int)style_getProperty(s, NULL, id, NULL, "focusoutline");
	} else {
		g = (Gradient *)style_getProperty(s, NULL, id, NULL, "gradient");
		outline.value = (unsigned int)style_getProperty(s, NULL, id, NULL, "outline");
	}
	if (g == NULL)
		return;
	radius = (int)style_getProperty(s, NULL, id, NULL, "radius");

	lgui_rbox_gradient(g, box->x+margin->x, box->y+margin->y, box->width, box->height, radius);
	lgui_roundedbox_line(box->x+margin->x, box->y+margin->y, box->width, box->height, radius,
			outline.rgba.red, outline.rgba.green, outline.rgba.blue);
}

WidgetRenderer *widgetrenderer_gradboxr(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = gradboxr_renderer;
	output->measure = zero_measure;

	return output;
}


/* solid renderer */
static void solid_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	Color c;
	DataObjectField *field;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	c.value = (unsigned int)style_getProperty(s, NULL, "solid", NULL, "color");
	field = dataobject_getValue(w, "color");
	if (field != NULL) {
		if (field->type == DOF_UINT) {
			c.value = field->field.uinteger;
		} else if (field->type == DOF_STRING) {
			sscanf(field->field.string, "%x", &c.value);
		}
	}

	lgui_hline(box->x+margin->x, box->y+margin->y, box->width, box->height, c.rgba.red, c.rgba.green, c.rgba.blue);
}

WidgetRenderer *widgetrenderer_solid(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = solid_renderer;
	output->measure = zero_measure;

	return output;
}

/* string renderer */
static void string_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	Font *f;
	Color c;
	const char *dtype;
	const char *ltype;
	const char *str;
	DataObjectField *field;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	dtype = (const char *)dataobject_getValue(dobj, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)style_getProperty(s, NULL, ltype, dtype, "font");
	c.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "color");
	field = dataobject_getValue(w, "color");
	if (field != NULL) {
		if (field->type == DOF_UINT) {
			c.value = field->field.uinteger;
		} else if (field->type == DOF_STRING) {
			sscanf(field->field.string, "%x", &c.value);
		}
	}
	field = dataobject_getValue(dobj, "data");
	if (field == NULL)
		return;
	str = (const char *)field->field.string;

	lgui_draw_font(box->x/*+margin->x*/, box->y+margin->y, str, f, c);
}

static void string_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	Font *f;
	const char *dtype;
	const char *ltype;
	const char *str;
	DataObjectField *field;

	dtype = (const char *)dataobject_getValue(dobj, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)style_getProperty(s, NULL, ltype, dtype, "font");
	field = dataobject_getValue(dobj, "data");
	if (field == NULL) {
		p->x = 0;
		p->y = 0;
		return;
	}
	str = (const char *)field->field.string;

	lgui_measure_font(str, f, p);
}

WidgetRenderer *widgetrenderer_string(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = string_renderer;
	output->measure = string_measure;

	return output;
}

/* entry renderer */
static void entry_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
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
	lgui_roundedbox_fill(box->x+margin->x, box->y+margin->y, box->width, box->height, 7,
			background.rgba.red, background.rgba.green, background.rgba.blue);
	lgui_roundedbox_line(box->x+margin->x, box->y+margin->y, box->width, box->height, 7,
			line.rgba.red, line.rgba.green, line.rgba.blue);

	lgui_draw_font(box->x+4+margin->x, box->y+margin->y, str, f, c);
}

static void entry_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	Font *f;
	const char *dtype;
	const char *ltype;
	const char *str;

	dtype = (const char *)dataobject_getValue(dobj, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)style_getProperty(s, NULL, ltype, dtype, "font");
	str = (const char *)dataobject_getValue(dobj, "data")->field.data.bytes;

	lgui_measure_font(str, f, p);
	p->y += 4;
}

WidgetRenderer *widgetrenderer_entry(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = entry_renderer;
	output->measure = entry_measure;

	return output;
}
