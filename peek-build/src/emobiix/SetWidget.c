#include "SetWidget.h"

#include "lgui.h"
#include "Debug.h"
#include "Widget.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

/*
<set fieldname="data">
    <setitem fieldvalue="1">
    </setitem>
</set>
*/

static int setwidget_fieldCompare(DataObjectField *lhs, DataObjectField *rhs)
{
	if (!lhs || !rhs || lhs->type != rhs->type)
		return 0;

	switch (lhs->type)
	{
		case DOF_STRING:
			return strcmp(lhs->field.string, rhs->field.string) == 0;
		case DOF_INT:
			return lhs->field.integer == rhs->field.integer;
		case DOF_UINT:
			return lhs->field.uinteger == rhs->field.uinteger;
	}

	return 0;
}

DataObject *setwidget_activeItem(DataObject *w)
{
	DataObject *dobj, *itemobj;
	DataObjectField *fieldname, *fieldvalue;
	DataObjectField *itemvalue;
	ListIterator iter;

	EMO_ASSERT_NULL(w != NULL, "Set widet active item missing set");

	dobj = widget_getDataObject(w);

	fieldname = dataobject_getValue(w, "fieldname");
	if (fieldname == NULL || fieldname->type != DOF_STRING)
		return NULL;

	fieldvalue = dataobject_getValue(dobj, fieldname->field.string);
	if (!fieldvalue)
		return NULL;

	dataobject_childIterator(w, &iter);
	while (!listIterator_finished(&iter)) {
		itemobj = (DataObject *)listIterator_item(&iter);
		itemvalue = dataobject_getValue(itemobj, "fieldvalue");
		if (setwidget_fieldCompare(itemvalue, fieldvalue))
		{
			dataobject_childIterator(itemobj, &iter);
			if (listIterator_finished(&iter))
				return NULL;
			return listIterator_item(&iter);
		} 
		listIterator_next(&iter);
	}

	return NULL;
}

