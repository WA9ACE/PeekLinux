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

DataObject *setwidget_activeItem(DataObject *w)
{
	DataObject *dobj, *itemobj;
	DataObjectField *fieldname, *fieldvalue;
	DataObjectField *itemvalue;
	ListIterator iter;

	dobj = widget_getDataObject(w);

	fieldname = dataobject_getValue(w, "fieldname");
	if (fieldname == NULL || fieldname->type != DOF_STRING)
		return NULL;

	fieldvalue = dataobject_getValue(dobj, fieldname->field.string);

	widget_getChildren(w, &iter);
	while (!listIterator_finished(&iter)) {
		itemobj = (DataObject *)listIterator_item(&iter);
		itemvalue = dataobject_getValue(itemobj, "fieldvalue");
		if (itemvalue != NULL && itemvalue->type == DOF_STRING &&
				fieldvalue != NULL && fieldvalue->type == DOF_STRING &&
				strcmp(itemvalue->field.string, fieldvalue->field.string) == 0) {
			widget_getChildren(itemobj, &iter);
			if (listIterator_finished(&iter))
				return NULL;
			return listIterator_item(&iter);
		} else if (itemvalue == NULL) {
			return listIterator_item(&iter);
		}
		listIterator_next(&iter);
	}

	return NULL;
}

