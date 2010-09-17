#ifndef _SCROLLEDWIDGET_H_
#define _SCROLLEDWIDGET_H_

#include "Widget_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void scrolled_autoscroll(Widget *w);
int scrolled_focusNext(Widget *w);
int scrolled_focusPrev(Widget *w);
void scrolled_forceVisible(Widget *w);

#ifdef __cplusplus
}
#endif

#endif
