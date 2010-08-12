#ifndef _DATAOBJECT_PRIVATE_H
#define _DATAOBJECT_PRIVATE_H_

#include "Rectangle.h"
#include "Font.h"
#include "List.h"
#include "Map.h"
#include "Point.h"
#include "Subscription.h"
#include "Widget.h"

struct DataObject_t {
	Map *data;
	int isLocal;
	DataObjectState state;
	unsigned int stampMinor;
	unsigned int stampMajor;

	Subscription *mapSubscription;

	List *children;
	DataObject *parent;

	Rectangle margin, box;
	Point position;
	//char *className;
	//char *idName;
	//int hasFocus;
	//int canFocus;

	WidgetPacking packing;
	DataObject *widgetData;
	unsigned int flags1;
    void *scriptContext;
};

#define DO_FLAG_DIRTY				0x01
#define DO_FLAG_LAYOUT_DIRTY_WIDTH	0x02
#define DO_FLAG_LAYOUT_DIRTY_HEIGHT 0x04
#define DO_FLAG_RECORD_TYPE			0x08
#define DO_FLAG_FORCE_SYNC			0x10

/* widget renderers can use draw flags as they choose. They dont get synced
  or notified so are useful for draw-only information */
/*#define DO_FLAG_DRAW_FLAG1 0x10
#define DO_FLAG_DRAW_FLAG2 0x20
#define DO_FLAG_DRAW_FLAG3 0x40*/

#endif
