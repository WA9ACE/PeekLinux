#include "WidgetRenderer.h"
#include "Debug.h"
#include "lgui.h"
#include "Point.h"
#include "RenderManager.h"

#include "p_malloc.h"
#include <stdio.h>
#include <string.h>

int scrolled_focusNext(Widget *w)
{
	Rectangle *box, *childBox;
	DataObjectField *xtransField, *ytransField;
	int xtrans, ytrans;

	EMO_ASSERT_INT(w != NULL, 0, "scrolled focusNext on NULL")

	box = widget_getBox(w);
	childBox = widget_getChildBox(w);

	xtransField = dataobject_getEnumAsInt(w, EMO_FIELD_XOFFSET);
	ytransField = dataobject_getEnumAsInt(w, EMO_FIELD_YOFFSET);

	xtrans = 0;
	ytrans = 0;
	if (xtransField != NULL) {
		xtrans = xtransField->field.integer;
	} else {
		xtransField = dataobjectfield_int(0);
		dataobject_setEnum(w, EMO_FIELD_XOFFSET, xtransField);
		xtrans = 0;
	}
	if (ytransField != NULL) {
		ytrans = ytransField->field.integer;
	} else {
		ytransField = dataobjectfield_int(0);
		dataobject_setEnum(w, EMO_FIELD_YOFFSET, ytransField);
		ytrans = 0;
	}

	if (childBox->height <= box->height)
		return 0;
	if (ytrans + box->height >= childBox->height)
		return 0;
	ytrans += box->height/3;
	if (ytrans > childBox->height - box->height)
		ytrans = childBox->height - box->height;
	ytransField->field.integer = ytrans;
	
	/*dataobject_setIsModified(w, 1);*/

	return 1;
}

int scrolled_focusPrev(Widget *w)
{
	Rectangle *box, *childBox;
	DataObjectField *xtransField, *ytransField;
	int xtrans, ytrans;

	EMO_ASSERT_INT(w != NULL, 0, "scrolled focusNext on NULL")

	box = widget_getBox(w);
	childBox = widget_getChildBox(w);

	xtransField = dataobject_getEnumAsInt(w, EMO_FIELD_XOFFSET);
	ytransField = dataobject_getEnumAsInt(w, EMO_FIELD_YOFFSET);

	xtrans = 0;
	ytrans = 0;
	if (xtransField != NULL) {
		xtrans = xtransField->field.integer;
	} else {
		xtransField = dataobjectfield_int(0);
		dataobject_setEnum(w, EMO_FIELD_XOFFSET, xtransField);
		xtrans = 0;
	}
	if (ytransField != NULL) {
		ytrans = ytransField->field.integer;
	} else {
		ytransField = dataobjectfield_int(0);
		dataobject_setEnum(w, EMO_FIELD_YOFFSET, ytransField);
		ytrans = 0;
	}

	if (childBox->height <= box->height)
		return 0;
	if (ytrans <= 0)
		return 0;
	ytrans -= box->height/3;
	if (ytrans < 0)
		ytrans = 0;
	ytransField->field.integer = ytrans;
	
	/*dataobject_setIsModified(w, 1);*/

	return 1;
}

void scrolled_forceVisible(Widget *w)
{
	Widget *scrolled;
	Rectangle *box, *childBox, *widgetBox;
	DataObjectField *xtransField, *ytransField;
	int xtrans, ytrans;

	EMO_ASSERT(w != NULL, "scrolled forceVisible on NULL")

	scrolled = widget_findStringFieldParent(w, EMO_FIELD_TYPE, EMO_TYPE_SCROLLED);
	if (scrolled == NULL)
		return;

	box = widget_getBox(scrolled);
	childBox = widget_getChildBox(scrolled);
	widgetBox = widget_getBox(w);

	xtransField = dataobject_getEnumAsInt(scrolled, EMO_FIELD_XOFFSET);
	ytransField = dataobject_getEnumAsInt(scrolled, EMO_FIELD_YOFFSET);

	xtrans = 0;
	ytrans = 0;
	if (xtransField != NULL) {
		xtrans = xtransField->field.integer;
	} else {
		xtransField = dataobjectfield_int(0);
		dataobject_setEnum(scrolled, EMO_FIELD_XOFFSET, xtransField);
		xtrans = 0;
	}
	if (ytransField != NULL) {
		ytrans = ytransField->field.integer;
	} else {
		ytransField = dataobjectfield_int(0);
		dataobject_setEnum(scrolled, EMO_FIELD_YOFFSET, ytransField);
		ytrans = 0;
	}

	if (ytrans+box->y > widgetBox->y) {
		ytrans = widgetBox->y-box->y;
		ytransField->field.integer = ytrans;
		dataobject_setIsModified(scrolled, 1);
		emo_printf("Scrolled move up" NL);
	} else if (ytrans+box->y+box->height < widgetBox->y+widgetBox->height) {
		ytrans = widgetBox->y+widgetBox->height - (box->y+box->height);
		ytransField->field.integer = ytrans;
		dataobject_setIsModified(scrolled, 1);
		emo_printf("Scrolled move down" NL);
	}
}

void scrolled_autoscroll(Widget *w)
{
	DataObjectField *autoscroll;
	Rectangle *box, *childBox;

	EMO_ASSERT(w != NULL, "scrollbar scroll widget on NULL widget")

	box = widget_getBox(w);
	childBox = widget_getChildBox(w);

	/*emo_printf("Height: %d, Child Height: %d, pos: %d" NL,
			box->height, childBox->height, (childBox->height-box->height));*/

	autoscroll = dataobject_getEnum(w, EMO_FIELD_AUTOSCROLL);
	if (dataobjectfield_isString(autoscroll, "bottom")) {
		if (childBox->height <= box->height)
			dataobject_setEnum(w, EMO_FIELD_YOFFSET, dataobjectfield_int(0));
		else
			dataobject_setEnum(w, EMO_FIELD_YOFFSET, dataobjectfield_int((childBox->height-box->height)));
	} else if (autoscroll != NULL) {
		dataobject_setEnum(w, EMO_FIELD_YOFFSET, dataobjectfield_int(0));
	}
}


static void scrolled_renderer(WidgetRenderer *wr, Style *s, Widget *w,		
		DataObject *dobj)
{
	Rectangle *box;
	DataObjectField *xtransField, *ytransField;
	int xtrans, ytrans;

	EMO_ASSERT(wr != NULL, "scrolled render missing renderer")
	EMO_ASSERT(s != NULL, "scrolled render missing style")
	EMO_ASSERT(w != NULL, "scrolled render missing widget")
	EMO_ASSERT(dobj != NULL, "scrolled render missing DataObject")

	box = widget_getBox(w);

	xtransField = dataobject_getEnumAsInt(w, EMO_FIELD_XOFFSET);
	ytransField = dataobject_getEnumAsInt(w, EMO_FIELD_YOFFSET);

	xtrans = 0;
	ytrans = 0;
	if (xtransField != NULL)
		xtrans = xtransField->field.integer;
	if (ytransField != NULL)
		ytrans = ytransField->field.integer;

	lgui_clip_push();
	lgui_clip_and(box);

	emo_printf("Scrolled redraw at %d" NL, ytrans);

	lgui_translate(-xtrans, -ytrans);
}

static void scrolled_postrenderer(WidgetRenderer *wr, Style *s, Widget *w,		
		DataObject *dobj)
{
	DataObjectField *xtransField, *ytransField;
	int xtrans, ytrans;

	EMO_ASSERT(wr != NULL, "scrolled render missing renderer")
	EMO_ASSERT(s != NULL, "scrolled render missing style")
	EMO_ASSERT(w != NULL, "scrolled render missing widget")
	EMO_ASSERT(dobj != NULL, "scrolled render missing DataObject")

	xtransField = dataobject_getEnumAsInt(w, EMO_FIELD_XOFFSET);
	ytransField = dataobject_getEnumAsInt(w, EMO_FIELD_YOFFSET);

	xtrans = 0;
	ytrans = 0;
	if (xtransField != NULL)
		xtrans = xtransField->field.integer;
	if (ytransField != NULL)
		ytrans = ytransField->field.integer;

	lgui_clip_pop();

	lgui_translate(xtrans, ytrans);

	emo_printf("Scrolled postdraw" NL);

}

static void scrolled_margin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
	EMO_ASSERT(wr != NULL, "scrolled margin missing renderer")
	EMO_ASSERT(s != NULL, "scrolled margin missing style")
	EMO_ASSERT(w != NULL, "scrolled margin missing widget")
	EMO_ASSERT(dobj != NULL, "scrolled margin missing DataObject")
	EMO_ASSERT(output != NULL, "scrolled margin missing rectangle")

	output->x = 0;
	output->y = 0;
	output->width = 0;
	output->height = 0;
}

WidgetRenderer *widgetrenderer_scrolled(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = scrolled_renderer;
	output->measure = NULL;
	output->margin = scrolled_margin;
	output->postrender = scrolled_postrenderer;

	return output;
}

