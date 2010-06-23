#include "WidgetRenderer.h"
#include "Debug.h"
#include "lgui.h"
#include "Point.h"

#include "p_malloc.h"
#include <stdio.h>

/* zeros */
static void zero_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
}

static void full_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *output)
{
	Rectangle *box;
	DataObject *parent;

	parent = dataobject_parent(w);
	box = widget_getBox(parent);
	output->x = box->width;
	output->y = box->height;
}

static void zero_margin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
	output->x = 0;
	output->y = 0;
	output->width = 0;
	output->height = 0;
}

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
		case RGB565A8:
			lgui_blitRGB565A8(box->x+margin->x, box->y+margin->y,
					0, 0, width, height, data);
			break;
		case A4:
			c.value = (unsigned int)style_getProperty(s, NULL, "image", NULL, "color");
			lgui_luminence_A4_blitC(box->x+margin->x, box->y+margin->y, 0, 0,
					width, height, width, height, data, c);
			break;
		default:
			emo_printf("Unsupported image format" NL);
	}
}

static void image_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *output)
{
	DataObjectField *field;

	field = dataobject_getValue(dobj, "width");
	if (field == NULL || field->type != DOF_INT) {
		emo_printf("width field not an int" NL);
		output->x = 0;
		output->y = 0;
		return;
	}
	output->x = field->field.integer;

	field = dataobject_getValue(dobj, "height");
	if (field == NULL || field->type != DOF_INT) {
		emo_printf("height field not an int" NL);
		output->x = 0;
		output->y = 0;
		return;
	}
	output->y = field->field.integer;
}

WidgetRenderer *widgetrenderer_image(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = image_renderer;
	output->measure = image_measure;
	output->margin = zero_margin;

	return output;
}

/* gradbox renderer */
static void gradbox_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	Gradient *g;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	g = (Gradient *)style_getProperty(s, NULL, "gradbox", "box", "gradient");

	lgui_vertical_gradientG(g,
			box->x+margin->x, box->y+margin->y, box->width, box->height);
}

WidgetRenderer *widgetrenderer_gradbox(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = gradbox_renderer;
	output->measure = NULL;
	output->margin = zero_margin;

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
	DataObjectField *type;
	const char *typestr = NULL;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	id = widget_getID(w);
	type = dataobject_getValue(w, "type");
	if (type != NULL && type->type == DOF_STRING)
		typestr = type->field.string;
	if (widget_hasFocus(w)) {
		g = (Gradient *)style_getProperty(s, NULL, id, typestr, "focusgradient");
		outline.value = (unsigned int)style_getProperty(s, NULL, id, NULL, "focusoutline");
	} else {
		g = (Gradient *)style_getProperty(s, NULL, id, typestr, "gradient");
		outline.value = (unsigned int)style_getProperty(s, NULL, id, typestr, "outline");
	}
	if (g == NULL)
		return;
	radius = (int)style_getProperty(s, NULL, id, "box", "radius");

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
	output->measure = NULL;
	output->margin = zero_margin;

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
	c.value = (unsigned int)style_getProperty(s, NULL, "solid", "box", "color");
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
	output->measure = NULL;
	output->margin = zero_margin;

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
	DataObjectField *field, *sourceField;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	dtype = (const char *)dataobject_getValue(w, "type")->field.string;
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
	sourceField = dataobject_getValue(w, "datafield");
	if (sourceField != NULL && sourceField->type == DOF_STRING)
		field = dataobject_getValue(dobj, sourceField->field.string);
	else
		field = dataobject_getValue(dobj, "data");
	if (field == NULL)
		return;
	str = (const char *)field->field.string;

	lgui_draw_font(box->x+margin->x, box->y+margin->y, str, f, c);
}

static void string_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	Font *f;
	const char *dtype;
	const char *ltype;
	const char *str;
	DataObjectField *field, *sourceField;

	dtype = (const char *)dataobject_getValue(w, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)style_getProperty(s, NULL, ltype, dtype, "font");
	
	sourceField = dataobject_getValue(w, "datafield");
	if (sourceField != NULL && sourceField->type == DOF_STRING)
		field = dataobject_getValue(dobj, sourceField->field.string);
	else
		field = dataobject_getValue(dobj, "data");
	
	if (field == NULL) {
		p->x = 0;
		p->y = 0;
		return;
	}
	str = (const char *)field->field.string;

	lgui_measure_font(str, f, p);
}

static void string_margin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
	output->x = 3;
	output->y = 2;
	output->width = 3;
	output->height = 2;
}

WidgetRenderer *widgetrenderer_string(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = string_renderer;
	output->measure = string_measure;
	output->margin = string_margin;

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

	lgui_roundedbox_fill(box->x+margin->x, box->y+margin->y, box->width-1, box->height-1, 7,
			background.rgba.red, background.rgba.green, background.rgba.blue);
	lgui_roundedbox_line(box->x+margin->x, box->y+margin->y, box->width-1, box->height-1, 7,
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
	output->margin = zero_margin;

	return output;
}

/* zero renderer */
WidgetRenderer *widgetrenderer_zero(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = zero_renderer;
	output->measure = NULL;
	output->margin = zero_margin;

	return output;
}

/* view renderer */
WidgetRenderer *widgetrenderer_full(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = zero_renderer;
	output->measure = full_measure;
	output->margin = zero_margin;

	return output;
}

static void button_margin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
	output->x = 2;
	output->y = 2;
	output->width = 2;
	output->height = 2;
}

WidgetRenderer *widgetrenderer_button(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = gradboxr_renderer;
	output->measure = NULL;
	output->margin = button_margin;

	return output;
}

