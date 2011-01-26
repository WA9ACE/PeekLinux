#include "CheckBoxWidget.h"

#include "DataObject.h"
#include "Widget.h"
#include "lgui.h"
#include "Style.h"
#include "WidgetRenderer.h"
#include "KeyMappings.h"
#include "Debug.h"

#include "p_malloc.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int checkboxWidget_handleKey(Widget *w, unsigned int key, Style *s)
{
	DataObject *dobj;
	DataObjectField *field, *cbfield, *cbvalue = NULL;
	const char *fieldStr = NULL, *valueStr = NULL;
	int isChecked;
	EmoField fieldEnum = EMO_FIELD_UNKNOWN_FIELD;

	EMO_ASSERT_INT(w != NULL, 0,
			"CheckBox Key handler passed NULL widget")
	EMO_ASSERT_INT(s != NULL, 0,
			"CheckBox Key handler passed NULL style")

	if (key != EKEY_ACTIVATE)
		return 0;

	dobj = widget_getDataObject(w);

	cbvalue = dataobject_getEnum(w, EMO_FIELD_CHECKVALUE);
	if (cbvalue != NULL && cbvalue->type == DOF_STRING)
		valueStr = cbvalue->field.string;
		

	cbfield = dataobject_getEnum(w, EMO_FIELD_CHECKFIELD);
	if (cbfield != NULL) {
		if (cbfield->type == DOF_INT) {
			fieldEnum = cbfield->field.integer;
		} else if (cbfield->type == DOF_STRING) {
			fieldStr = cbfield->field.string;
			fieldEnum = emo_field_to_int(fieldStr);
			if (fieldEnum != EMO_FIELD_UNKNOWN_FIELD)
				fieldStr = NULL;
		}
	}
	if (valueStr == NULL)
		valueStr = "1";
	if (fieldStr == NULL && fieldEnum == EMO_FIELD_UNKNOWN_FIELD) {
		fieldEnum = EMO_FIELD_DATA;
	}

	if (fieldStr != NULL)
		field = dataobject_getValue(dobj, fieldStr);
	else
		field = dataobject_getEnum(dobj, fieldEnum);
	if (field == NULL) {
		field = dataobjectfield_string("0");
		dataobject_setEnum(dobj, EMO_FIELD_DATA, field);
	}
	isChecked = dataobjectfield_isString(field, valueStr);

	if (isChecked) {
		if (cbvalue == NULL) {
			dataobjectfield_setString(field, "0");
		} else {
			/* acting as a radio, and you cannot uncheck a radio */
			return 1;
		}
	} else {
		dataobjectfield_setString(field, valueStr);
	}

	dataobject_setIsModified(dobj, 1);
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
	DataObjectField *field, *cbfield, *cbvalue;
	const char *fieldStr = NULL, *valueStr = NULL;
	EmoField fieldEnum = -1;
	Color checkColor;
	WidgetRenderer *iwr;

	EMO_ASSERT(wr != NULL,
			"CheckBox Renderer passed NULL widget renderer")
	EMO_ASSERT(s != NULL,
			"CheckBox Renderer passed NULL style")
	EMO_ASSERT(w != NULL,
			"CheckBox Renderer passed NULL widget")
	EMO_ASSERT(dobj != NULL,
			"CheckBox Renderer passed NULL data object")

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	checkBox.x = box->x+3 + margin->x;
	checkBox.y = box->y+3 + margin->y;
	checkBox.width = box->width-6;
	checkBox.height = box->height-6;

	iwr = NULL;
	style_getRenderer(s, w, EMO_FIELD_BOX_RENDERER, &iwr);
	if (iwr != NULL)
		iwr->render(wr, s, w, dobj);

	cbvalue = dataobject_getEnum(w, EMO_FIELD_CHECKVALUE);
	if (cbvalue != NULL && cbvalue->type == DOF_STRING)
		valueStr = cbvalue->field.string;

	cbfield = dataobject_getEnum(w, EMO_FIELD_CHECKFIELD);
	if (cbfield != NULL) {
		if (cbfield->type == DOF_STRING) {
			fieldStr = cbfield->field.string;
		} else if (cbfield->type == DOF_INT) {
			fieldEnum = cbfield->field.integer;
		} else {
			EMO_ASSERT(0, "CheckField not string nor enum")
		}
	}
	if (fieldStr == NULL && fieldEnum == -1)
		fieldEnum = EMO_FIELD_DATA;
	if (valueStr == NULL)
		valueStr =  "1";

	text = widget_getDataObject(w);
	if (fieldStr != NULL)
		field = dataobject_getValue(text, fieldStr);
	else
		field = dataobject_getEnum(text, fieldEnum);

	if (dataobjectfield_isString(field, valueStr) && 
			style_getColor(s, w, EMO_FIELD_CHECK_COLOR, &checkColor.value) != NULL) {
		lgui_hline(checkBox.x, checkBox.y, checkBox.width, checkBox.height,
				checkColor.rgba.red, checkColor.rgba.green, checkColor.rgba.blue,
				checkColor.rgba.alpha);
	}
}

static void checkbox_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	EMO_ASSERT(wr != NULL,
			"CheckBox Renderer measure NULL widget renderer")
	EMO_ASSERT(s != NULL,
			"CheckBox Renderer measure NULL style")
	EMO_ASSERT(w != NULL,
			"CheckBox Renderer measure NULL widget")
	EMO_ASSERT(dobj != NULL,
			"CheckBox Renderer measure NULL data object")
	EMO_ASSERT(p != NULL,
			"CheckBox Renderer measure NULL point")

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
	output->postrender = NULL;

	return output;
}

