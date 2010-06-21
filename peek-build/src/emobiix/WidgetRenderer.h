#ifndef _WIDGETRENDERER_H_
#define _WIDGETRENDERER_H_

#include "Widget.h"
#include "DataObject.h"
#include "Style.h"
#include "Point.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WidgetRenderer_t WidgetRenderer;
struct WidgetRenderer_t {
	void (*render)(WidgetRenderer *wr, Style *s, Widget *w,
			DataObject *dobj);
	void (*measure)(WidgetRenderer *wr, Style *s, Widget *w,
			DataObject *dobj, IPoint *output);
	void (*margin)(WidgetRenderer *wr, Style *s, Widget *w,
			DataObject *dobj, Rectangle *output);
};

WidgetRenderer *widgetrenderer_image(void);
WidgetRenderer *widgetrenderer_gradbox(void);
WidgetRenderer *widgetrenderer_gradboxr(void);
WidgetRenderer *widgetrenderer_solid(void);
WidgetRenderer *widgetrenderer_string(void);
WidgetRenderer *widgetrenderer_entry(void);
WidgetRenderer *widgetrenderer_button(void);
WidgetRenderer *widgetrenderer_zero(void);
WidgetRenderer *widgetrenderer_full(void);

#ifdef __cplusplus
}
#endif

#endif

