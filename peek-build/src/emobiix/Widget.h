#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "DataObject.h"
#include "Rectangle.h"
#include "Style_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "Widget_internal.h"

Widget *widget_new(void);
Widget *widget_newTypeIdName(const char *type, const char *id, const char *name,
        Widget *parent);
void widget_setDataObject(Widget *w, DataObject *dobj);
DataObject *widget_getDataObject(Widget *w);
void widget_setPacking(Widget *w, WidgetPacking p);
WidgetPacking widget_getPacking(Widget *w);
void widget_pack(Widget *w, Widget *parent);
ListIterator *widget_getChildren(Widget *w);
void widget_setClass(Widget *w, const char *className);
const char *widget_getClass(Widget *w);
void widget_setID(Widget *w, const char *idName);
const char *widget_getID(Widget *w);
int widget_canFocus(Widget *w);
void widget_setCanFocus(Widget *w, int canFocus);
int widget_hasFocus(Widget *w);
void widget_setFocus(Widget *w, int isFocus);
WidgetAlignment widget_getAlignment(Widget *w);
void widget_setAlignment(Widget *w, WidgetAlignment a);
int widget_isParent(Widget *wParent, Widget *w);
Widget *widget_focusWhichOne(Widget *w);
Widget *widget_focusWhichOneNF(Widget *w);
void widget_markDirty(Widget *w);
Widget *widget_findStringField(Widget *screen, const char *key, const char *value);

Rectangle *widget_getMargin(Widget *w);
Rectangle *widget_getBorder(Widget *w);
Rectangle *widget_getBox(Widget *w);
void widget_resolveLayout(Widget *w, Style *s);
void widget_getClipRectangle(Widget *w, Rectangle *rect);

void widget_printTree(Widget *w, int level);


#ifdef __cplusplus
}
#endif

#endif /* _WIDGET_H_ */
