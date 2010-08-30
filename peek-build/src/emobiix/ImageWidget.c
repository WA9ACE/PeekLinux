#include "lgui.h"
#include "Debug.h"
#include "Widget.h"
#include "WidgetRenderer.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

#define TRANS_NONE		LGUI_MODE_REPLACE
#define TRANS_STENCIL	LGUI_MODE_STENCIL
#define TRANS_FULL		LGUI_MODE_ALPHA

/* image renderer */
static void image_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	int width, height;
	PixelFormat pf;
	void *data;
	Color c;
	DataObjectField *field;
	int trans;

	EMO_ASSERT(wr != NULL, "image render missing renderer")
	EMO_ASSERT(s != NULL, "image render missing style")
	EMO_ASSERT(w != NULL, "image render missing widget")
	EMO_ASSERT(dobj != NULL, "image render missing DataObject")

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	
	field = dataobject_getValue(dobj, "width");
	if (field == NULL)
		return;
	width = (int)field->field.integer;
	
	field = dataobject_getValue(dobj, "height");
	if (field == NULL)
		return;
	height = (int)field->field.integer;

	field = dataobject_getValue(dobj, "pixelformat");
	if (field == NULL)
		return;
	pf = (int)field->field.integer;

	field = dataobject_getValue(dobj, "data");
	if (field == NULL)
		return;
	data = field->field.data.bytes;

	trans = TRANS_FULL;
	field = style_getProperty(s, w, "transparency");
	if (field != NULL && field->type == DOF_STRING) {
		if (strcmp(field->field.string, "none") == 0)
			trans = TRANS_NONE;
		if (strcmp(field->field.string, "stencil") == 0)
			trans = TRANS_STENCIL;
	}

	switch (pf) {
		case RGB565:
			lgui_blitRGB565(box->x+margin->x, box->y+margin->y,
					0, 0, width, height, data, trans == TRANS_STENCIL);
			break;
		case RGB565A8:
			lgui_blitRGB565A8(box->x+margin->x, box->y+margin->y,
					0, 0, width, height, data);
			break;
		case A4:
			field = style_getPropertyAsInt(s, w, "color");
			if (field != NULL) {
				c.value = field->field.uinteger;
			} else {
				c.value = 0xFFFFFFFF;
				style_getColor(s, w, "color", &c.value);
			}
			lgui_luminence_A4_blitC(box->x+margin->x, box->y+margin->y, 0, 0,
					width, height, width, height, data, c, trans, 0);
			break;
		default:
			emo_printf("Unsupported image format" NL);
	}
}

static void image_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *output)
{
	DataObjectField *field;

	EMO_ASSERT(wr != NULL, "image measure missing renderer")
	EMO_ASSERT(s != NULL, "image measure missing style")
	EMO_ASSERT(w != NULL, "image measure missing widget")
	EMO_ASSERT(dobj != NULL, "image measure missing DataObject")
	EMO_ASSERT(output != NULL, "image measure missing the point")

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
	output->margin = widgetrenderer_zeroMargin;
	output->postrender = NULL;

	return output;
}
