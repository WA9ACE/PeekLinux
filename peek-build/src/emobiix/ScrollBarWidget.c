#include "EntryWidget.h"

#include "DataObject.h"
#include "Widget.h"
#include "lgui.h"
#include "Style.h"
#include "WidgetRenderer.h"
#include "Debug.h"

#include "p_malloc.h"
#include "KeyMappings.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* scrollbar renderer */
static void scrollbar_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin, *childBox;
	Color thumbColor, trackColor;
	DataObjectField *orientationField;
	int isVertical, height, starti;
	DataObjectField *xtransField, *ytransField;
	int xtrans, ytrans;

	EMO_ASSERT(wr != NULL, "scrollbar widget render missing renderer")
	EMO_ASSERT(w != NULL, "scrollbar widget render on NULL widget")
	EMO_ASSERT(s != NULL, "scrollbar widget render missing style")
	EMO_ASSERT(dobj != NULL, "scrollbar widget render missing DataObject")

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	childBox = widget_getChildBox(dobj);

	thumbColor.value = 0;
	style_getColor(s, w, "thumb-color", &thumbColor.value);

	trackColor.value = 0;
	style_getColor(s, w, "track-color", &trackColor.value);

	orientationField = style_getProperty(s, w, "orientation");
	if (dataobjectfield_isString(orientationField, "horizontal"))
		isVertical = 0;
	else
		isVertical = 1;

	xtransField = dataobject_getValueAsInt(dobj, "xoffset");
	ytransField = dataobject_getValueAsInt(dobj, "yoffset");

	xtrans = 0;
	ytrans = 0;
	if (xtransField != NULL)
		xtrans = xtransField->field.integer;
	if (ytransField != NULL)
		ytrans = ytransField->field.integer;

	lgui_hline(box->x+margin->x, box->y+margin->y, box->width, box->height-1,
				trackColor.rgba.red, trackColor.rgba.green, trackColor.rgba.blue);
	
	if (isVertical) {
		height = box->height-4;
		starti = 0;
		if (childBox->height > box->height) {
			height = (box->height*height)/childBox->height;
			if (ytrans != 0)
				starti = (box->height*ytrans)/childBox->height;
		}
		lgui_roundedbox_fill(box->x+margin->x+1, box->y+margin->y+2+starti, box->width-2, height, 3,
				thumbColor.rgba.red, thumbColor.rgba.green, thumbColor.rgba.blue,
				LGUI_CORNERS_ALL);
	} else {

	}
	/*lgui_roundedbox_fill(box->x+margin->x, box->y+margin->y, box->width, box->height, radius,
				color.rgba.red, color.rgba.green, color.rgba.blue,
				cornerFlags);*/

}

static void scrollbar_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	EMO_ASSERT(wr != NULL, "entry widget measure missing renderer")
	EMO_ASSERT(w != NULL, "entry widget measure on NULL widget")
	EMO_ASSERT(s != NULL, "entry widget measure missing style")
	EMO_ASSERT(dobj != NULL, "entry widget measure missing DataObject")
	EMO_ASSERT(p != NULL, "entry widget measure missing the point")

	p->x = 8;
}

WidgetRenderer *widgetrenderer_scrollbar(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = scrollbar_renderer;
	output->measure = scrollbar_measure;
	output->margin = widgetrenderer_zeroMargin;
	output->postrender = NULL;

	return output;
}

