#include "WidgetRenderer.h"
#include "Debug.h"
#include "lgui.h"
#include "Point.h"

#include "p_malloc.h"
#include <stdio.h>
#include <string.h>

extern Font *defaultFont;

static void string_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p);

/* string renderer */
static void string_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	Font *f;
	Color c;
	const char *dtype;
	const char *ltype;
	const char *str;
	DataObjectField *field, *sourceField, *boldField, *fontalign;
    int isBold = 0;
	IPoint dimensions;

	EMO_ASSERT(wr != NULL, "string render missing renderer")
	EMO_ASSERT(s != NULL, "string render missing style")
	EMO_ASSERT(w != NULL, "string render missing widget")
	EMO_ASSERT(dobj != NULL, "string render missing DataObject")

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	dtype = (const char *)dataobject_getValue(w, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)defaultFont;/*style_getProperty(s, NULL, ltype, dtype, "font");*/
	style_getColor(s, w, "font-color", &c.value);
	/*c.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "color");
	field = dataobject_getValue(w, "font-color");
	if (field != NULL) {
		if (field->type == DOF_UINT) {
			c.value = field->field.uinteger;
		} else if (field->type == DOF_STRING) {
			sscanf(field->field.string, "%x", &c.value);
		}
	}*/
	sourceField = dataobject_getValue(w, "datafield");
	if (sourceField != NULL && sourceField->type == DOF_STRING)
		field = dataobject_getValue(dobj, sourceField->field.string);
	else
		field = dataobject_getValue(dobj, "data");
	if (field == NULL)
		return;
	str = (const char *)field->field.string;
	/*emo_printf("Rendering %s" NL, str);*/

    boldField = style_getProperty(s, w, "weight");
    if (boldField != NULL && boldField->type == DOF_STRING) {
        if (strcmp(boldField->field.string, "bold") == 0)
            isBold = 1;
        else
            isBold = 0;
    }

	fontalign = style_getProperty(s, w, "font-alignment");

	string_measure(wr, s, w, dobj, &dimensions);

	emo_printf("Rendering label '%s'" NL, str);

	if (dataobjectfield_isString(fontalign, "right"))
		lgui_draw_font(box->x+margin->x + box->width - dimensions.x, box->y+margin->y, box->width, box->height, str, f, c, isBold);
	else if (dataobjectfield_isString(fontalign, "center"))
		lgui_draw_font(box->x+margin->x + (box->width - dimensions.x)/2, box->y+margin->y, box->width, box->height, str, f, c, isBold);
	else
		lgui_draw_font(box->x+margin->x, box->y+margin->y, box->width, box->height, str, f, c, isBold);
}

static void string_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	Font *f;
	const char *dtype;
	const char *ltype;
	const char *str;
	DataObjectField *field, *sourceField, *boldField;
    int isBold = 0;

	EMO_ASSERT(wr != NULL, "string measure missing renderer")
	EMO_ASSERT(s != NULL, "string measure missing style")
	EMO_ASSERT(w != NULL, "string measure missing widget")
	EMO_ASSERT(dobj != NULL, "string measure missing DataObject")
	EMO_ASSERT(p != NULL, "string measure missing the point")

	dtype = (const char *)dataobject_getValue(w, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)defaultFont;/*style_getProperty(s, NULL, ltype, dtype, "font");*/
	
	sourceField = dataobject_getValue(w, "datafield");
	if (sourceField != NULL && sourceField->type == DOF_STRING)
		field = dataobject_getValue(dobj, sourceField->field.string);
	else
		field = dataobject_getValue(dobj, "data");
	
    boldField = dataobject_getValue(w, "weight");
    if (boldField != NULL && boldField->type == DOF_STRING) {
        if (strcmp(boldField->field.string, "bold") == 0)
            isBold = 1;
        else
            isBold = 0;
    }
#if 0
	if (field != NULL)
		emo_printf("Measuring '%s'" NL, field->field.string);
	else {
		emo_printf("Measuring NULL" NL);
		/*dataobject_debugPrint(dobj);*/
	}
#endif
	if (field == NULL) {
		p->x = 0;
		p->y = 0;
		return;
	}
	str = (const char *)field->field.string;

	lgui_measure_font(str, f, isBold, p);
}

static void string_margin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
	EMO_ASSERT(wr != NULL, "string margin missing renderer")
	EMO_ASSERT(s != NULL, "string margin missing style")
	EMO_ASSERT(w != NULL, "string margin missing widget")
	EMO_ASSERT(dobj != NULL, "string margin missing DataObject")
	EMO_ASSERT(output != NULL, "string margin missing rectangle")

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
	output->postrender = NULL;

	return output;
}

