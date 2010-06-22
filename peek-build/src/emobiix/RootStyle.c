#include "RootStyle.h"

#include "Gradient.h"
#include "Widget.h"
#include "WidgetRenderer.h"
#include "ArrayWidget.h"

#include "Font.h"

#include "assets/DroidSans.c"

#include <stdio.h>

Style *RootStyle(void)
{
	static Gradient *greengrad2;
	static Gradient *grad2;
	static Gradient *greengrad1;
	static Color color;
	static Gradient *grad1;
	static Font *defaultFont, *hugeFont, *consoleFont;
	static DataObject *fontObject1, *fontObject2, *fontObject3;
	static Style *defaultStyle = NULL;

	if (defaultStyle != NULL)
		return defaultStyle;

	fontObject1 = dataobject_new();
	dataobject_setValue(fontObject1, "type", dataobjectfield_string("font"));
	dataobject_setValue(fontObject1, "data", dataobjectfield_data((void *)DroidSans, DroidSansSize));

	defaultFont = font_load(fontObject1);
	if (defaultFont != NULL)
		font_setHeight(defaultFont, 12);

	hugeFont = font_load(fontObject1);
	font_setHeight(hugeFont, 20);

	consoleFont = font_load(fontObject1);
	font_setHeight(consoleFont, 8);

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
	
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"renderer", (void *)widgetrenderer_array());
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"gradient", (void *)grad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"focusgradient", (void *)grad2);
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", "array",
			"focusoutline", (void *)color.value);
#if 0
	style_setProperty(defaultStyle, NULL, NULL, "view",
			"renderer", (void *)widgetrenderer_full());
#endif
	return defaultStyle;
}
