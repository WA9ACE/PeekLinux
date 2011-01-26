#include "WidgetRenderer.h"
#include "Debug.h"
#include "lgui.h"
#include "Point.h"
#include "ArrayWidget.h"

#include "p_malloc.h"
#include <stdio.h>
#include <string.h>

extern Gradient *defaultGradient;

/* zeros */
static void zero_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
}

static void full_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *output)
{
	Rectangle *box;
	DataObject *parent;

	EMO_ASSERT(wr != NULL, "full measure missing widget renderer")
	EMO_ASSERT(s != NULL, "full measure missing style")
	EMO_ASSERT(w != NULL, "full measure missing widget")
	EMO_ASSERT(dobj != NULL, "full measure missing data object")
	EMO_ASSERT(output != NULL, "full measure missing the point")

	parent = dataobject_parent(w);
	box = widget_getBox(parent);
	output->x = box->width;
	output->y = box->height;
}

void widgetrenderer_zeroMargin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
	EMO_ASSERT(wr != NULL, "wrzeero margin missing widget renderer")
	EMO_ASSERT(s != NULL, "wrzero margin missing style")
	EMO_ASSERT(w != NULL, "wrzero margin missing widget")
	EMO_ASSERT(dobj != NULL, "wrzero margin missing data object")
	EMO_ASSERT(output != NULL, "wrzero margin missing rect")

	output->x = 0;
	output->y = 0;
	output->width = 0;
	output->height = 0;
}

#if 0
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
	output->margin = widgetrenderer_zeroMargin;

	return output;
}
#endif 

/* gradboxr renderer */
static void box_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin;
	Gradient *g, *freeg = NULL;
	const char *str;
	int radius, isGradient;
	Color outline, color;
	DataObjectField *radiusF, *fill, *rounded;
	DataObjectField *border, *bordercorners;
	unsigned int cornerFlags, borderFlags, cornerBorderFlags;
	ListIterator iter;
	
	EMO_ASSERT(wr != NULL, "box renderer missing widget renderer")
	EMO_ASSERT(s != NULL, "box renderer missing style")
	EMO_ASSERT(w != NULL, "box renderer missing widget")
	EMO_ASSERT(dobj != NULL, "box renderer missing data object")

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	fill = style_getProperty(s, w, EMO_FIELD_FILL);
	isGradient = dataobjectfield_isString(fill, "gradient");
	radiusF = style_getProperty(s, w, EMO_FIELD_RADIUS);
	rounded = style_getProperty(s, w, EMO_FIELD_ROUNDED);
	border = style_getProperty(s, w, EMO_FIELD_BORDER);
	bordercorners = style_getProperty(s, w, EMO_FIELD_BORDER_CORNERS);
	outline.value = 0;
	style_getColor(s, w, EMO_FIELD_BORDER_COLOR, &outline.value);

	if (rounded == NULL || rounded->type != DOF_STRING) {
		cornerFlags = 0;
	} else {
		cornerFlags = 0;
		for (str = rounded->field.string; *str != 0; ++str) {
			switch (*str) {
				case '1': cornerFlags |= LGUI_CORNER_TOPLEFT; break;
				case '2': cornerFlags |= LGUI_CORNER_TOPRIGHT; break;
				case '3': cornerFlags |= LGUI_CORNER_BOTTOMRIGHT; break;
				case '4': cornerFlags |= LGUI_CORNER_BOTTOMLEFT; break;
				default: break;
			}
		}
	}

	if (border == NULL || border->type != DOF_STRING) {
		borderFlags = 0;
	} else {
		borderFlags = 0;
		for (str = border->field.string; *str != 0; ++str) {
			switch (*str) {
				case '1': borderFlags |= LGUI_TOP; break;
				case '2': borderFlags |= LGUI_LEFT; break;
				case '3': borderFlags |= LGUI_BOTTOM; break;
				case '4': borderFlags |= LGUI_RIGHT; break;
				default: break;
			}
		}
	}
	if (bordercorners == NULL || bordercorners->type != DOF_STRING) {
		cornerBorderFlags = 0;
	} else {
		cornerBorderFlags = 0;
		for (str = bordercorners->field.string; *str != 0; ++str) {
			switch (*str) {
				case '1': cornerBorderFlags |= LGUI_CORNER_TOPLEFT; break;
				case '2': cornerBorderFlags |= LGUI_CORNER_TOPRIGHT; break;
				case '3': cornerBorderFlags |= LGUI_CORNER_BOTTOMRIGHT; break;
				case '4': cornerBorderFlags |= LGUI_CORNER_BOTTOMLEFT; break;
				default: break;
			}
		}
	}

	radius = 7;
	if (radiusF != NULL) {
		if (radiusF->type == DOF_INT || radiusF->type == DOF_UINT)
			radius = radiusF->field.integer;
		else if (radiusF->type == DOF_STRING) {
			radius = atoi(radiusF->field.string);
		}
	}

	if (isGradient) {
		dataobject_childIterator(s, &iter);
		if (!listIterator_finished(&iter)) {
			g = style_getGradient((Style *)listIterator_item(&iter));
			freeg = g;
		} else {
			g = defaultGradient;
		}

		if (cornerFlags == 0 || radius == 0) {
			lgui_vertical_gradientG(g,
				box->x+margin->x, box->y+margin->y, box->width, box->height);
		} else {
			lgui_rbox_gradient(g, box->x+margin->x, box->y+margin->y,
					box->width, box->height, radius, cornerFlags);
		}
	} else if (dataobjectfield_isString(fill, "solid")) {
		color.value = 0;
		style_getColor(s, w, EMO_FIELD_BACKGROUND_COLOR, &color.value);

		if (cornerFlags == 0 || radius == 0) {
			lgui_hline(box->x+margin->x, box->y+margin->y, box->width, box->height,
				color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.alpha);
		} else {
			lgui_roundedbox_fill(box->x+margin->x, box->y+margin->y, box->width, box->height, radius,
				color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.alpha,
				cornerFlags);
		}
	}
	if (cornerFlags == 0 || radius == 0) {
		lgui_box(box->x+margin->x, box->y+margin->y, box->width, box->height, 1,
				outline.rgba.red, outline.rgba.green, outline.rgba.blue,
				outline.rgba.alpha, borderFlags);
	} else {
		lgui_roundedbox_line(box->x+margin->x, box->y+margin->y, box->width-1, box->height-1, radius,
			outline.rgba.red, outline.rgba.green, outline.rgba.blue,
			borderFlags, cornerBorderFlags);
	}

	if (freeg != NULL)
		gradient_delete(freeg);
}

WidgetRenderer *widgetrenderer_box(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = box_renderer;
	output->measure = NULL;
	output->margin = widgetrenderer_zeroMargin;
	output->postrender = NULL;

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
	output->margin = widgetrenderer_zeroMargin;
	output->postrender = NULL;

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
	output->margin = widgetrenderer_zeroMargin;
	output->postrender = NULL;

	return output;
}

static void button_margin(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, Rectangle *output)
{
	EMO_ASSERT(wr != NULL, "button margin missing widget renderer")
	EMO_ASSERT(s != NULL, "button margin missing style")
	EMO_ASSERT(w != NULL, "button margin missing widget")
	EMO_ASSERT(dobj != NULL, "button margin missing data object")
	EMO_ASSERT(output != NULL, "button margin missing rect")

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
	output->render = box_renderer;
	output->measure = NULL;
	output->margin = button_margin;
	output->postrender = NULL;

	return output;
}

WidgetRenderer *widgetRenderer_fromString(const char *str)
{
	EMO_ASSERT_NULL(str != NULL, "widget renderer from string missing string")

	if (strcmp(str, "box") == 0)
		return widgetrenderer_box();
	if (strcmp(str, "button") == 0)
		return widgetrenderer_box();
	if (strcmp(str, "image") == 0)
		return widgetrenderer_image();
	if (strcmp(str, "label") == 0)
		return widgetrenderer_string();
	if (strcmp(str, "text") == 0)
		return widgetrenderer_text();
	if (strcmp(str, "entry") == 0)
		return widgetrenderer_entry();
	if (strcmp(str, "scrolled") == 0)
		return widgetrenderer_scrolled();
	if (strcmp(str, "scrollbar") == 0)
		return widgetrenderer_scrollbar();
#if 0
	if (strcmp(str, "array") == 0)
		return widgetrenderer_array();
#endif
	if (strcmp(str, "checkbox") == 0)
		return widgetrenderer_checkbox();
	return NULL;
}
