#include "StackWidget.h"

#include "DataObject.h"

#include <stdio.h>

int stackwidget_focusStack(DataObject *w)
{
	DataObjectField *field;

	field = dataobject_getValueAsInt(w, "focusstack");
	if (field == NULL)
		return -1;
	return field->field.integer;
}

DataObject *stackwidget_focusStackObject(DataObject *w)
{
	ListIterator iter;
	int focus, index;
	int len;

	focus = stackwidget_focusStack(w);
	len = dataobject_getChildCount(w);
	if (focus < 0 || focus >= len)
		return NULL;
	
	index = 0;
	for (dataobject_childIterator(w, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		if (focus == index)
			return (DataObject *)listIterator_item(&iter);
		++index;
	}
	return NULL;
}