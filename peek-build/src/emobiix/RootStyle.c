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
	dataobject_setEnum(fontObject1, EMO_FIELD_TYPE, dataobjectfield_string("font"));
	dataobject_setEnum(fontObject1, EMO_FIELD_DATA, dataobjectfield_string("DroidSans.ttf"));

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
	dataobject_setEnum(defaultStyle, EMO_FIELD_TYPE, dataobjectfield_string("style"));

	/* Entry */
	solidbox = widget_newTypeIdName("entry", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("entry"));
	dataobject_setEnum(solidbox, EMO_FIELD_FONTCOLOR, dataobjectfield_int(0));
	dataobject_setEnum(solidbox, EMO_FIELD_MARGIN, dataobjectfield_int(2));
	dataobject_setEnum(solidbox, EMO_FIELD_MULTILINE, dataobjectfield_string("0"));
	dataobject_setEnum(solidbox, EMO_FIELD_BOXRENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(solidbox, EMO_FIELD_ROUNDED, dataobjectfield_string("1234"));
	dataobject_setEnum(solidbox, EMO_FIELD_BACKGROUNDCOLOR, dataobjectfield_int(0xFFFFFFFF));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDERCORNERS, dataobjectfield_string("1234"));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDERCOLOR, dataobjectfield_uint(0x696969FF));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDER, dataobjectfield_string("1234"));
	dataobject_setEnum(solidbox, EMO_FIELD_FILL, dataobjectfield_string("solid"));

	/* Entry - focused */
	solidbox = widget_newTypeIdName("entry", NULL, NULL, defaultStyle);
	widget_setFocus(solidbox, 1);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("entry"));
	dataobject_setEnum(solidbox, EMO_FIELD_FONTCOLOR, dataobjectfield_int(0));
	dataobject_setEnum(solidbox, EMO_FIELD_MARGIN, dataobjectfield_int(2));
	dataobject_setEnum(solidbox, EMO_FIELD_MULTILINE, dataobjectfield_string("0"));
	dataobject_setEnum(solidbox, EMO_FIELD_BOXRENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(solidbox, EMO_FIELD_ROUNDED, dataobjectfield_string("1234"));
	dataobject_setEnum(solidbox, EMO_FIELD_BACKGROUNDCOLOR, dataobjectfield_int(0xb7f1ffFF));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDERCORNERS, dataobjectfield_string("1234"));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDERCOLOR, dataobjectfield_uint(0x696969FF));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDER, dataobjectfield_string("1234"));
	dataobject_setEnum(solidbox, EMO_FIELD_FILL, dataobjectfield_string("solid"));
	dataobject_setEnum(solidbox, EMO_FIELD_CURSORCOLOR, dataobjectfield_int(0x0000FFFF));

	/* label */
	solidbox = widget_newTypeIdName("label", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("label"));
	dataobject_setEnum(solidbox, EMO_FIELD_FONTCOLOR, dataobjectfield_int(0));

	/* checkbox */
	solidbox = widget_newTypeIdName("checkbox", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("checkbox"));
	dataobject_setEnum(solidbox, EMO_FIELD_BOXRENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(solidbox, EMO_FIELD_BACKGROUNDCOLOR, dataobjectfield_uint(0xFFFFFFFF));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDERCOLOR, dataobjectfield_uint(0x0));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDER, dataobjectfield_string("1234"));
	dataobject_setEnum(solidbox, EMO_FIELD_FILL, dataobjectfield_string("solid"));
	dataobject_setEnum(solidbox, EMO_FIELD_CHECKCOLOR, dataobjectfield_int(0x5050FFFF));
	
	/* checkbox */
	solidbox = widget_newTypeIdName("checkbox", NULL, NULL, defaultStyle);
	widget_setFocus(solidbox, 1);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("checkbox"));
	dataobject_setEnum(solidbox, EMO_FIELD_BOXRENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(solidbox, EMO_FIELD_BACKGROUNDCOLOR, dataobjectfield_uint(0xCFCFFFFF));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDERCOLOR, dataobjectfield_uint(0x0));
	dataobject_setEnum(solidbox, EMO_FIELD_BORDER, dataobjectfield_string("1234"));
	dataobject_setEnum(solidbox, EMO_FIELD_FILL, dataobjectfield_string("solid"));
	dataobject_setEnum(solidbox, EMO_FIELD_CHECKCOLOR, dataobjectfield_int(0x5050FFFF));
	dataobject_setEnum(solidbox, EMO_FIELD_BOXRENDERER, dataobjectfield_string("box"));

	/* text */
	solidbox = widget_newTypeIdName("text", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("text"));
	dataobject_setEnum(solidbox, EMO_FIELD_FONTCOLOR, dataobjectfield_int(0));

	/* scroll bar */
	solidbox = widget_newTypeIdName("scrollbar", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("scrollbar"));
	dataobject_setEnum(solidbox, EMO_FIELD_THUMBCOLOR, dataobjectfield_int(0xAAAAAAFF));
	dataobject_setEnum(solidbox, EMO_FIELD_TRACKCOLOR, dataobjectfield_int(0x444444FF));

	/* scrolled */
	solidbox = widget_newTypeIdName("scrolled", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("scrolled"));

	/* label */
	solidbox = widget_newTypeIdName("array", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("array"));

	/* Image */
	solidbox = widget_newTypeIdName("image", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("image"));

	/* Solid */
	solidbox = widget_newTypeIdName("solid", NULL, NULL, defaultStyle);
	dataobject_setEnum(solidbox, EMO_FIELD_RENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(solidbox, EMO_FIELD_FILL, dataobjectfield_string("solid"));

	/* Gradbox */
	gradbox = widget_newTypeIdName("gradbox", NULL, NULL, defaultStyle);
	dataobject_setEnum(gradbox, EMO_FIELD_RENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(gradbox, EMO_FIELD_FILL, dataobjectfield_string("gradient"));
	dataobject_setEnum(gradbox, EMO_FIELD_BACKGROUNDCOLOR, dataobjectfield_uint(0x878787FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(0));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xC9C9C9FF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xB1B1B1FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(100));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xDEDEDEFF));

	/* Dark Gradbox */
	gradbox = widget_newTypeIdName("darkgradbox", NULL, NULL, defaultStyle);
	dataobject_setEnum(gradbox, EMO_FIELD_RENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(gradbox, EMO_FIELD_FILL, dataobjectfield_string("gradient"));
	dataobject_setEnum(gradbox, EMO_FIELD_BACKGROUNDCOLOR, dataobjectfield_uint(0x878787FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(0));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0x666666FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(100));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0x000000FF));

	/* Gradboxr */
	gradbox = widget_newTypeIdName("gradboxr", NULL, NULL, defaultStyle);
	dataobject_setEnum(gradbox, EMO_FIELD_RENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(gradbox, EMO_FIELD_FILL, dataobjectfield_string("gradient"));
	dataobject_setEnum(gradbox, EMO_FIELD_RADIUS, dataobjectfield_int(7));
	dataobject_setEnum(gradbox, EMO_FIELD_ROUNDED, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDER, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDERCORNERS, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDERCOLOR, dataobjectfield_uint(0x696969FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(0));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xC9C9C9FF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xB1B1B1FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(100));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xDEDEDEFF));

	/* Gradboxr - focused */
	gradbox = widget_newTypeIdName("gradboxr", NULL, NULL, defaultStyle);
	widget_setFocus(gradbox, 1);
	dataobject_setEnum(gradbox, EMO_FIELD_RENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(gradbox, EMO_FIELD_FILL, dataobjectfield_string("gradient"));
	dataobject_setEnum(gradbox, EMO_FIELD_RADIUS, dataobjectfield_int(7));
	dataobject_setEnum(gradbox, EMO_FIELD_ROUNDED, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDER, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDERCORNERS, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDERCOLOR, dataobjectfield_uint(0x696989FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(0));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0x59DFFEFF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0x35dcfbFF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(100));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xb7f1ffFF));

	/* Button */
	gradbox = widget_newTypeIdName("button", NULL, NULL, defaultStyle);
	dataobject_setEnum(gradbox, EMO_FIELD_MARGIN, dataobjectfield_int(2));
	dataobject_setEnum(gradbox, EMO_FIELD_RENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(gradbox, EMO_FIELD_FILL, dataobjectfield_string("gradient"));
	dataobject_setEnum(gradbox, EMO_FIELD_RADIUS, dataobjectfield_int(7));
	dataobject_setEnum(gradbox, EMO_FIELD_ROUNDED, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDER, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDERCORNERS, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDERCOLOR, dataobjectfield_uint(0x696969FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(0));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xC9C9C9FF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xB1B1B1FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(100));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xDEDEDEFF));

	/* Button - focused */
	gradbox = widget_newTypeIdName("button", NULL, NULL, defaultStyle);
	widget_setFocus(gradbox, 1);
	dataobject_setEnum(gradbox, EMO_FIELD_RENDERER, dataobjectfield_string("box"));
	dataobject_setEnum(gradbox, EMO_FIELD_MARGIN, dataobjectfield_int(2));
	dataobject_setEnum(gradbox, EMO_FIELD_FILL, dataobjectfield_string("gradient"));
	dataobject_setEnum(gradbox, EMO_FIELD_RADIUS, dataobjectfield_int(7));
	dataobject_setEnum(gradbox, EMO_FIELD_ROUNDED, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDER, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDERCORNERS, dataobjectfield_string("1234"));
	dataobject_setEnum(gradbox, EMO_FIELD_BORDERCOLOR, dataobjectfield_uint(0x696989FF));

	gradient = widget_newTypeIdName("gradient", NULL, NULL, gradbox);
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(0));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xF9F9F9FF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0x59DFFEFF));
	
	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(50));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0x35dcfbFF));

	gradientStop = widget_newTypeIdName("stop", NULL, NULL, gradient);
	dataobject_setEnum(gradientStop, EMO_FIELD_POSITION, dataobjectfield_int(100));
	dataobject_setEnum(gradientStop, EMO_FIELD_COLOR, dataobjectfield_uint(0xb7f1ffFF));


	return defaultStyle;
}

