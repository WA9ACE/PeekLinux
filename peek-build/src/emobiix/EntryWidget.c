#include "EntryWidget.h"

#include "DataObject.h"
#include "Widget.h"
#include "lgui.h"
#include "Style.h"

#include "p_malloc.h"

#include <stdio.h>
#include <string.h>

/* TODO, handle UTF8 instead of just ascii */

int entryWidget_handleKey(Widget *w, int key, Style *s)
{
	DataObject *dobj;
	DataObjectField *field;
	int slen;
	char *newstring;
	Rectangle rect;

	if (key == 10 || key == 13 || key == 86 || key == 87)
		return 0;

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
	lgui_clip_identity();
	widget_getClipRectangle(w, &rect);
	lgui_clip_set(&rect);
	lgui_push_region();
	style_renderWidgetTree(s, dataobject_superparent(w));
	return 1;
}
