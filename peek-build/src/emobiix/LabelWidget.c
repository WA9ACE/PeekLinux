#include "WidgetRenderer.h"
#include "Debug.h"
#include "lgui.h"
#include "Point.h"

#include "p_malloc.h"
#include <stdio.h>
#include <string.h>

/* string renderer */
static void string_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	Font *f;
	Color c;
	const char *dtype;
	const char *ltype;
	const char *str;
	DataObjectField *field, *sourceField, *boldField;
    int isBold = 0;

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
	/*emo_printf("Rendering %s" NL, str);*/

    boldField = dataobject_getValue(w, "weight");
    if (boldField != NULL && boldField->type == DOF_STRING) {
        if (strcmp(boldField->field.string, "bold") == 0)
            isBold = 1;
        else
            isBold = 0;
    }

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

	dtype = (const char *)dataobject_getValue(w, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)style_getProperty(s, NULL, ltype, dtype, "font");
	
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

