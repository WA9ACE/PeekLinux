#ifndef _ARRAY_WIDGET_H_
#define _ARRAY_WIDGET_H_

#include "WidgetRenderer.h"

#ifdef __cplusplus
extern "C" {
#endif

WidgetRenderer *widgetrenderer_array(void);
int arraywidget_focusNext(Widget *w);

#ifdef __cplusplus
}
#endif

#endif

