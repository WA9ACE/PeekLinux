#include "RootStyle.h"

#include "Gradient.h"
#include "Widget.h"
#include "WidgetRenderer.h"
#include "ArrayWidget.h"
#include "Debug.h"
#include "Font.h"

#include <stdio.h>
#include <stdlib.h>

Font *defaultFont, *hugeFont;
Gradient *defaultGradient;

Style *RootStyle(void)
{
	static DataObject *defaultStyle = NULL;
	DataObject *fontObject1;
	DataObject *solidbox, *gradbox;
	DataObject *gradient, *gradientStop;
	Color color;

	if (defaultStyle != NULL)
		return defaultStyle;

	fontObject1 = dataobject_new();
	dataobject_setValue(fontObject1, "type", dataobjectfield_string("font"));
	dataobject_setValue(fontObject1, "data", dataobjectfield_string("DroidSans.ttf"));

#ifndef SIMULATOR
	file_openWrite("test0");
	file_openWrite("test1");
	file_openWrite("test2");
#endif

	defaultFont = NULL;
	hugeFont = NULL;
	
	defaultFont = font_load(fontObject1);
	if (defaultFont != NULL)
		font_setHeight(defaultFont, 12);

	hugeFont = font_load(fontObject1);
	if (hugeFont == NULL) {
		emo_printf("Failed to load font" NL);
	} else {
		font_setHeight(hugeFont, 20);
	}
	


	defaultGradient = gradient_new();
	color.value = 0xF9F9F9FF;
	gradient_addStop(defaultGradient, 0, color);
	color.value = 0xC9C9C9FF;
	gradient_addStop(defaultGradient, 50, color);
	color.value = 0xB1B1B1FF;
	gradient_addStop(defaultGradient, 50, color);
	color.value = 0xDEDEDEFF;
	gradient_addStop(defaultGradient, 100, color);

	defaultStyle = dataobject_new();
	dataobject_setValue(defaultStyle, "type", dataobjectfield_string("style"));

	/* Entry */
	solidbox = widget_newTypeIdName("entry", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("entry"));
	dataobject_setValue(solidbox, "font-color", dataobjectfield_int(0));
	dataobject_setValue(solidbox, "margin", dataobjectfield_int(2));
	dataobject_setValue(solidbox, "multiline", dataobjectfield_string("0"));
	dataobject_setValue(solidbox, "box-renderer", dataobjectfield_string("box"));
	dataobject_setValue(solidbox, "rounded", dataobjectfield_string("1234"));
	dataobject_setValue(solidbox, "background-color", dataobjectfield_int(0xFFFFFFFF));
	dataobject_setValue(solidbox, "border-corners", dataobjectfield_string("1234"));
	dataobject_setValue(solidbox, "border-color", dataobjectfield_uint(0x696969FF));
	dataobject_setValue(solidbox, "border", dataobjectfield_string("1234"));
	dataobject_setValue(solidbox, "fill", dataobjectfield_string("solid"));

	/* Entry - focused */
	solidbox = widget_newTypeIdName("entry", NULL, NULL, defaultStyle);
	widget_setFocus(solidbox, 1);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("entry"));
	dataobject_setValue(solidbox, "font-color", dataobjectfield_int(0));
	dataobject_setValue(solidbox, "margin", dataobjectfield_int(2));
	dataobject_setValue(solidbox, "multiline", dataobjectfield_string("0"));
	dataobject_setValue(solidbox, "box-renderer", dataobjectfield_string("box"));
	dataobject_setValue(solidbox, "rounded", dataobjectfield_string("1234"));
	dataobject_setValue(solidbox, "background-color", dataobjectfield_int(0xb7f1ffFF));
	dataobject_setValue(solidbox, "border-corners", dataobjectfield_string("1234"));
	dataobject_setValue(solidbox, "border-color", dataobjectfield_uint(0x696969FF));
	dataobject_setValue(solidbox, "border", dataobjectfield_string("1234"));
	dataobject_setValue(solidbox, "fill", dataobjectfield_string("solid"));
	dataobject_setValue(solidbox, "cursor-color", dataobjectfield_int(0x0000FF00));

	/* label */
	solidbox = widget_newTypeIdName("label", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("label"));
	dataobject_setValue(solidbox, "font-color", dataobjectfield_int(0));

	/* checkbox */
	solidbox = widget_newTypeIdName("checkbox", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("checkbox"));
	dataobject_setValue(solidbox, "box-renderer", dataobjectfield_string("box"));
	dataobject_setValue(solidbox, "background-color", dataobjectfield_uint(0xFFFFFFFF));
	dataobject_setValue(solidbox, "border-color", dataobjectfield_uint(0x0));
	dataobject_setValue(solidbox, "border", dataobjectfield_string("1234"));
	dataobject_setValue(solidbox, "fill", dataobjectfield_string("solid"));
	dataobject_setValue(solidbox, "check-color", dataobjectfield_int(0x5050FF00));
	
	/* checkbox */
	solidbox = widget_newTypeIdName("checkbox", NULL, NULL, defaultStyle);
	widget_setFocus(solidbox, 1);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("checkbox"));
	dataobject_setValue(solidbox, "box-renderer", dataobjectfield_string("box"));
	dataobject_setValue(solidbox, "background-color", dataobjectfield_uint(0xCFCFFFFF));
	dataobject_setValue(solidbox, "border-color", dataobjectfield_uint(0x0));
	dataobject_setValue(solidbox, "border", dataobjectfield_string("1234"));
	dataobject_setValue(solidbox, "fill", dataobjectfield_string("solid"));
	dataobject_setValue(solidbox, "check-color", dataobjectfield_int(0x5050FF00));
	dataobject_setValue(solidbox, "box-renderer", dataobjectfield_string("box"));

	/* text */
	solidbox = widget_newTypeIdName("text", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("text"));
	dataobject_setValue(solidbox, "font-color", dataobjectfield_int(0));

	/* scroll bar */
	solidbox = widget_newTypeIdName("scrollbar", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("scrollbar"));
	dataobject_setValue(solidbox, "thumb-color", dataobjectfield_int(0xAAAAAAFF));
	dataobject_setValue(solidbox, "track-color", dataobjectfield_int(0x444444FF));

	/* scrolled */
	solidbox = widget_newTypeIdName("scrolled", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("scrolled"));

	/* label */
	solidbox = widget_newTypeIdName("array", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("array"));

	/* Image */
	solidbox = widget_newTypeIdName("image", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("image"));

	/* Solid */
	solidbox = widget_newTypeIdName("solid", NULL, NULL, defaultStyle);
	dataobject_setValue(solidbox, "renderer", dataobjectfield_string("box"));
	dataobject_setValue(solidbox, "fill", dataobjectfield_string("solid"));

	/* Gradbox */
	gradbox = widget_newTypeIdName("gradbox", NULL, NULL, defaultStyle);
	dataobject_setValue(gradbox, "renderer", dataobjectfield_string("box"));
	dataobject_setValue(gradbox, "fill", dataobjectfield_string("gradient"));
	dataobject_setValue(gradbox, "background-color", dataobjectfield_uint(0x878787FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(0));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xC9C9C9FF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xB1B1B1FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(100));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xDEDEDEFF));

	/* Dark Gradbox */
	gradbox = widget_newTypeIdName("darkgradbox", NULL, NULL, defaultStyle);
	dataobject_setValue(gradbox, "renderer", dataobjectfield_string("box"));
	dataobject_setValue(gradbox, "fill", dataobjectfield_string("gradient"));
	dataobject_setValue(gradbox, "background-color", dataobjectfield_uint(0x878787FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(0));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0x666666FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(100));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0x000000FF));

	/* Gradboxr */
	gradbox = widget_newTypeIdName("gradboxr", NULL, NULL, defaultStyle);
	dataobject_setValue(gradbox, "renderer", dataobjectfield_string("box"));
	dataobject_setValue(gradbox, "fill", dataobjectfield_string("gradient"));
	dataobject_setValue(gradbox, "radius", dataobjectfield_int(7));
	dataobject_setValue(gradbox, "rounded", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border-corners", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border-color", dataobjectfield_uint(0x696969FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(0));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xC9C9C9FF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xB1B1B1FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(100));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xDEDEDEFF));

	/* Gradboxr - focused */
	gradbox = widget_newTypeIdName("gradboxr", NULL, NULL, defaultStyle);
	widget_setFocus(gradbox, 1);
	dataobject_setValue(gradbox, "renderer", dataobjectfield_string("box"));
	dataobject_setValue(gradbox, "fill", dataobjectfield_string("gradient"));
	dataobject_setValue(gradbox, "radius", dataobjectfield_int(7));
	dataobject_setValue(gradbox, "rounded", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border-corners", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border-color", dataobjectfield_uint(0x696989FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(0));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0x59DFFEFF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0x35dcfbFF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(100));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xb7f1ffFF));

	/* Button */
	gradbox = widget_newTypeIdName("button", NULL, NULL, defaultStyle);
	dataobject_setValue(gradbox, "margin", dataobjectfield_int(2));
	dataobject_setValue(gradbox, "renderer", dataobjectfield_string("box"));
	dataobject_setValue(gradbox, "fill", dataobjectfield_string("gradient"));
	dataobject_setValue(gradbox, "radius", dataobjectfield_int(7));
	dataobject_setValue(gradbox, "rounded", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border-corners", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border-color", dataobjectfield_uint(0x696969FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(0));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xC9C9C9FF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xB1B1B1FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(100));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xDEDEDEFF));

	/* Button - focused */
	gradbox = widget_newTypeIdName("button", NULL, NULL, defaultStyle);
	widget_setFocus(gradbox, 1);
	dataobject_setValue(gradbox, "renderer", dataobjectfield_string("box"));
	dataobject_setValue(gradbox, "margin", dataobjectfield_int(2));
	dataobject_setValue(gradbox, "fill", dataobjectfield_string("gradient"));
	dataobject_setValue(gradbox, "radius", dataobjectfield_int(7));
	dataobject_setValue(gradbox, "rounded", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border-corners", dataobjectfield_string("1234"));
	dataobject_setValue(gradbox, "border-color", dataobjectfield_uint(0x696989FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(0));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0x59DFFEFF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(50));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0x35dcfbFF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setValue(gradientStop, "position", dataobjectfield_int(100));
	dataobject_setValue(gradientStop, "color", dataobjectfield_uint(0xb7f1ffFF));


	return defaultStyle;
}

