#include "EntryWidget.h"

#include "DataObject.h"
#include "Widget.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

/* TODO, handle UTF8 instead of just ascii */

void entryWidget_handleKey(Widget *w, int key)
{
	DataObject *dobj;
	DataObjectField *field;
	int slen;
	char *newstring;

	if (key == 10 || key == 13 || key == 86 || key == 87)
		return;

	dobj = widget_getDataObject(w);
	field = dataobject_getValue(dobj, "data");
	if (field == NULL) {
		field = dataobjectfield_string("");
		dataobject_setValue(dobj, "data", field);
	}

	slen = strlen(field->field.string);

	if (key == '\b') {
		if (slen > 0) {
			field->field.string[slen-1] = 0;
		}
	} else {
		newstring = p_malloc(slen+2);
		strcpy(newstring, field->field.string);
		newstring[slen] = key;
		newstring[slen+1] = 0;
		p_free(field->field.string);
		field->field.string = newstring;
	}
}
