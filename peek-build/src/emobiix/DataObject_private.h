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

#define DO_FLAG_DIRTY 0x01
#define DO_FLAG_LAYOUT_DIRTY_WIDTH 0x02
#define DO_FLAG_LAYOUT_DIRTY_HEIGHT 0x04

#endif
