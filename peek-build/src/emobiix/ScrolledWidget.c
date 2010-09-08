#include "WidgetRenderer.h"
#include "Debug.h"
#include "lgui.h"
#include "Point.h"

#include "p_malloc.h"
#include <stdio.h>
#include <string.h>

void scrolled_autoscroll(Widget *w)
{
	DataObjectField *autoscroll;
	Rectangle *box, *childBox;

	EMO_ASSERT(w != NULL, "scrollbar scroll widget on NULL widget")

	box = widget_getBox(w);
	childBox = widget_getChildBox(w);

	/*emo_printf("Height: %d, Child Height: %d, pos: %d" NL,
			box->height, childBox->height, (childBox->height-box->height));*/

	autoscroll = dataobject_getValue(w, "autoscroll");
	if (dataobjectfield_isString(autoscroll, "bottom")) {
		if (childBox->height <= box->height)
			dataobject_setValue(w, "yoffset", dataobjectfield_int(0));
		else
			dataobject_setValue(w, "yoffset", dataobjectfield_int((childBox->height-box->height)));
	} else {
		dataobject_setValue(w, "yoffset", dataobjectfield_int(0));
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

	xtransField = dataobject_getValueAsInt(w, "xoffset");
	ytransField = dataobject_getValueAsInt(w, "yoffset");

	xtrans = 0;
	ytrans = 0;
	if (xtransField != NULL)
		xtrans = xtransField->field.integer;
	if (ytransField != NULL)
		ytrans = ytransField->field.integer;

	lgui_clip_push();
	lgui_clip_and(box);

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

	xtransField = dataobject_getValueAsInt(w, "xoffset");
	ytransField = dataobject_getValueAsInt(w, "yoffset");

	xtrans = 0;
	ytrans = 0;
	if (xtransField != NULL)
		xtrans = xtransField->field.integer;
	if (ytransField != NULL)
		ytrans = ytransField->field.integer;

	lgui_clip_pop();

	lgui_translate(xtrans, ytrans);
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

