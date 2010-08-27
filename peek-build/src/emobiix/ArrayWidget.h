#ifndef _ARRAY_WIDGET_H_
#define _ARRAY_WIDGET_H_

#include "WidgetRenderer.h"

#ifdef __cplusplus
extern "C" {
#endif

void array_expand(Widget *w);
#if 0
WidgetRenderer *widgetrenderer_array(void);
int arraywidget_focusNext(Widget *w, int *alreadyUnset, int *alreadySet);
int arraywidget_focusPrev(Widget *w);
#endif

#ifdef __cplusplus
}
#endif

#endif

