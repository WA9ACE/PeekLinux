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
#if 0
	static Gradient *greengrad2;
	static Gradient *grad2;
	static Gradient *greengrad1;
	static Color color;
	static Gradient *grad1;
	static Font *defaultFont, *hugeFont;
	static DataObject *fontObject1, *fontObject2, *fontObject3;
	static Style *defaultStyle = NULL;

	if (defaultStyle != NULL)
		return defaultStyle;

	fontObject1 = dataobject_new();
	dataobject_setValue(fontObject1, "type", dataobjectfield_string("font"));
	dataobject_setValue(fontObject1, "data", dataobjectfield_string("DroidSans.ttf"));
	/*dataobject_setValue(fontObject1, "data", dataobjectfield_data((void *)DroidSans, DroidSansSize));*/

	defaultFont = font_load(fontObject1);
	if (defaultFont != NULL)
		font_setHeight(defaultFont, 12);

	hugeFont = font_load(fontObject1);
	if (hugeFont == NULL) {
		emo_printf("Failed to load font" NL);
	} else {
		font_setHeight(hugeFont, 20);
	}
	greengrad2 = gradient_new();
	color.value = 0xE4FD13FF;
	gradient_addStop(greengrad2, 0, color);
	color.value = 0x8dc809FF;
	gradient_addStop(greengrad2, 50, color);
	color.value = 0x72b909FF;
	gradient_addStop(greengrad2, 50, color);
	color.value = 0xbfe412FF;
	gradient_addStop(greengrad2, 100, color);
	
	grad2 = gradient_new();
	color.value = 0xF9F9F9FF;
	gradient_addStop(grad2, 0, color);
	color.value = 0x59DFFEFF;
	gradient_addStop(grad2, 50, color);
	color.value = 0x35dcfbFF;
	gradient_addStop(grad2, 50, color);
	color.value = 0xb7f1ffFF;
	gradient_addStop(grad2, 100, color);

	greengrad1 = gradient_new();
	color.value = 0x10CF0DFF;
	gradient_addStop(greengrad1, 0, color);
	color.value = 0x008300FF;
	gradient_addStop(greengrad1, 50, color);
	color.value = 0x007000FF;
	gradient_addStop(greengrad1, 50, color);
	color.value = 0x005823FF;
	gradient_addStop(greengrad1, 100, color);

	grad1 = gradient_new();
	color.value = 0xF9F9F9FF;
	gradient_addStop(grad1, 0, color);
	color.value = 0xC9C9C9FF;
	gradient_addStop(grad1, 50, color);
	color.value = 0xB1B1B1FF;
	gradient_addStop(grad1, 50, color);
	color.value = 0xDEDEDEFF;
	gradient_addStop(grad1, 100, color);

	defaultStyle = style_new();
	style_setProperty(defaultStyle, NULL, "image", NULL,
			"renderer", (void *)widgetrenderer_image());
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, "image", NULL,
			"color", (void *)color.value);
	style_setProperty(defaultStyle, NULL, "image", "image",
			"renderer", (void *)widgetrenderer_image());
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, "image", "image",
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "gradbox", "box",
			"renderer", (void *)widgetrenderer_gradbox());
	style_setProperty(defaultStyle, NULL, "gradbox", "box",
			"gradient", (void *)grad1);
	
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"renderer", (void *)widgetrenderer_gradboxr());
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"gradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"focusgradient", (void *)grad2);
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"focusoutline", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"renderer", (void *)widgetrenderer_button());
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"gradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"focusgradient", (void *)grad2);
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"focusoutline", (void *)color.value);
	
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"renderer", (void *)widgetrenderer_gradboxr());
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"gradient", NULL);
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"focusgradient", (void *)grad2);
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"focusoutline", (void *)color.value);
	color.value = 0x69F989FF;
	
	style_setProperty(defaultStyle, NULL, "solid", "box",
			"renderer", (void *)widgetrenderer_solid());
	color.value = 0x878787FF;
	style_setProperty(defaultStyle, NULL, "solid", "box",
			"color", (void *)color.value);
	color.value = 0x263914FF;

	style_setProperty(defaultStyle, NULL, "label", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "label", "string",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabel", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabel", "string",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	
	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"color", (void *)color.value);
	
	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;

	color.value = 0x263914FF;
	style_setProperty(defaultStyle, NULL, "label", "label",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "label", "label",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabel", "label",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabel", "label",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	
	style_setProperty(defaultStyle, NULL, "biglabelw", "label",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabelw", "label",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabelw", "label",
			"color", (void *)color.value);
	
	style_setProperty(defaultStyle, NULL, "labelw", "label",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "labelw", "label",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "labelw", "label",
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;
	
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"renderer", (void *)widgetrenderer_entry());
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"font", (void *)defaultFont);
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"color", (void *)color.value);
	color.value = 0x000000FF;
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"background", (void *)color.value);
	color.value = 0xCFFFCFFF;
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"line", (void *)color.value);
	color.value = 0x207020FF;
	
	color.value = 0x8080F0FF;
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"focusline", (void *)color.value);
	color.value = 0xFFFF00FF;
	
	style_setProperty(defaultStyle, NULL, NULL, "text",
			"renderer", (void *)widgetrenderer_text());
	style_setProperty(defaultStyle, NULL, NULL, "text",
			"font", (void *)defaultFont);
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, NULL, "text",
			"color", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"renderer", (void *)widgetrenderer_array());
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"gradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"focusgradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"outline", (void *)color.value);
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"focusoutline", (void *)color.value);
#if 0
	style_setProperty(defaultStyle, NULL, NULL, "view",
			"renderer", (void *)widgetrenderer_full());
#endif
	return defaultStyle;
}

{
	static Gradient *greengrad2;
	static Gradient *grad2;
	static Gradient *greengrad1;
	static Color color;
	static Gradient *grad1, *darkGrad;
	static Font *defaultFont, *hugeFont;
	static DataObject *fontObject1, *fontObject2, *fontObject3;
	static Style *defaultStyle = NULL;

	if (defaultStyle != NULL)
		return defaultStyle;

	fontObject1 = dataobject_new();
	dataobject_setValue(fontObject1, "type", dataobjectfield_string("font"));
	dataobject_setValue(fontObject1, "data", dataobjectfield_string("DroidSans.ttf"));
	/*dataobject_setValue(fontObject1, "data", dataobjectfield_data((void *)DroidSans, DroidSansSize));*/

	defaultFont = font_load(fontObject1);
	if (defaultFont != NULL)
		font_setHeight(defaultFont, 12);

	hugeFont = font_load(fontObject1);
	if (hugeFont == NULL) {
		emo_printf("Failed to load font" NL);
	} else {
		font_setHeight(hugeFont, 20);
	}
	greengrad2 = gradient_new();
	color.value = 0xE4FD13FF;
	gradient_addStop(greengrad2, 0, color);
	color.value = 0x8dc809FF;
	gradient_addStop(greengrad2, 50, color);
	color.value = 0x72b909FF;
	gradient_addStop(greengrad2, 50, color);
	color.value = 0xbfe412FF;
	gradient_addStop(greengrad2, 100, color);
	
	grad2 = gradient_new();
	color.value = 0xF9F9F9FF;
	gradient_addStop(grad2, 0, color);
	color.value = 0x59DFFEFF;
	gradient_addStop(grad2, 50, color);
	color.value = 0x35dcfbFF;
	gradient_addStop(grad2, 50, color);
	color.value = 0xb7f1ffFF;
	gradient_addStop(grad2, 100, color);

	greengrad1 = gradient_new();
	color.value = 0x10CF0DFF;
	gradient_addStop(greengrad1, 0, color);
	color.value = 0x008300FF;
	gradient_addStop(greengrad1, 50, color);
	color.value = 0x007000FF;
	gradient_addStop(greengrad1, 50, color);
	color.value = 0x005823FF;
	gradient_addStop(greengrad1, 100, color);

	grad1 = gradient_new();
	color.value = 0xF9F9F9FF;
	gradient_addStop(grad1, 0, color);
	color.value = 0xC9C9C9FF;
	gradient_addStop(grad1, 50, color);
	color.value = 0xB1B1B1FF;
	gradient_addStop(grad1, 50, color);
	color.value = 0xDEDEDEFF;
	gradient_addStop(grad1, 100, color);

	darkGrad = gradient_new();
	color.value = 0x666666FF;
	gradient_addStop(darkGrad, 0, color);
	color.value = 0x000000FF;
	gradient_addStop(darkGrad, 100, color);

	defaultStyle = style_new();
	style_setProperty(defaultStyle, NULL, "image", NULL,
			"renderer", (void *)widgetrenderer_image());
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, "image", NULL,
			"color", (void *)color.value);
	style_setProperty(defaultStyle, NULL, "image", "image",
			"renderer", (void *)widgetrenderer_image());
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, "image", "image",
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "gradbox", "box",
			"renderer", (void *)widgetrenderer_gradbox());
	style_setProperty(defaultStyle, NULL, "gradbox", "box",
			"gradient", (void *)grad1);
	
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"renderer", (void *)widgetrenderer_gradboxr());
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"gradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"focusgradient", (void *)grad2);
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "box",
			"focusoutline", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "gradboxdark", "box", "renderer", (void *)widgetrenderer_gradboxdark());
	style_setProperty(defaultStyle, NULL, "gradboxdark", "box", "gradient", (void *)darkGrad);

	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"renderer", (void *)widgetrenderer_button());
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"gradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"focusgradient", (void *)grad2);
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "button",
			"focusoutline", (void *)color.value);
	
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"renderer", (void *)widgetrenderer_gradboxr());
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"gradient", NULL);
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"focusgradient", (void *)grad2);
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "selbox", "box",
			"focusoutline", (void *)color.value);
	color.value = 0x69F989FF;
	
	style_setProperty(defaultStyle, NULL, "solid", "box",
			"renderer", (void *)widgetrenderer_solid());
	color.value = 0x878787FF;
	style_setProperty(defaultStyle, NULL, "solid", "box",
			"color", (void *)color.value);
	color.value = 0x263914FF;

	style_setProperty(defaultStyle, NULL, "label", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "label", "string",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabel", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabel", "string",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	
	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"color", (void *)color.value);
	
	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;

	color.value = 0x263914FF;
	style_setProperty(defaultStyle, NULL, "label", "label",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "label", "label",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabel", "label",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabel", "label",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	
	style_setProperty(defaultStyle, NULL, "biglabelw", "label",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabelw", "label",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabelw", "label",
			"color", (void *)color.value);
	
	style_setProperty(defaultStyle, NULL, "labelw", "label",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "labelw", "label",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "labelw", "label",
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;
	
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"renderer", (void *)widgetrenderer_entry());
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"font", (void *)defaultFont);
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"color", (void *)color.value);
	color.value = 0x000000FF;
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"background", (void *)color.value);
	color.value = 0xCFFFCFFF;
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"line", (void *)color.value);
	color.value = 0x207020FF;
	
	color.value = 0x8080F0FF;
	style_setProperty(defaultStyle, NULL, NULL, "entry",
			"focusline", (void *)color.value);
	color.value = 0xFFFF00FF;
	
	style_setProperty(defaultStyle, NULL, NULL, "text",
			"renderer", (void *)widgetrenderer_text());
	style_setProperty(defaultStyle, NULL, NULL, "text",
			"font", (void *)defaultFont);
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, NULL, "text",
			"color", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"renderer", (void *)widgetrenderer_array());
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"gradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"focusgradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"outline", (void *)color.value);
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"focusoutline", (void *)color.value);
#if 0
	style_setProperty(defaultStyle, NULL, NULL, "view",
			"renderer", (void *)widgetrenderer_full());
#endif
	return defaultStyle;
}
#endif
