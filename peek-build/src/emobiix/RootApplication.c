#include "RootApplication.h"

#include "DataObject.h"
#include "Widget.h"

#include <stdio.h>

DataObject *RootApplication(void)
{
    static DataObject *output = NULL;
	DataObject *dobj1, *root, *dobj2;
	DataObject *setw, *setiw, *setc, *testvalue;

    if (output != NULL)
        return output;

    output = dataobject_new();
    dataobject_setValue(output, "type", dataobjectfield_string("application"));
    dataobject_setValue(output, "name", dataobjectfield_string("Boot Manager"));
    dataobject_setValue(output, "description",
            dataobjectfield_string(
            "Construct program for loading other programs"));
    dataobject_setValue(output, "icon", dataobjectfield_string("appicon"));
    dataobject_setValue(output, "startupview",
            dataobjectfield_string("rootview"));

	root = widget_newTypeIdName("view", NULL, "rootview", output);
	widget_setPacking(root, WP_VERTICAL);

	dobj1 = widget_newTypeIdName("box", "gradbox", NULL, root);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("320"));
	dataobject_setValue(dobj1, "height", dataobjectfield_string("20"));

	testvalue = dataobject_new();
	dataobject_setValue(testvalue, "data", dataobjectfield_string("1"));

	setw = widget_newTypeIdName("set", NULL, NULL, dobj1);
	dataobject_setValue(setw, "fieldname", dataobjectfield_string("data"));
	widget_setDataObject(setw, testvalue);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("1"));
	setc = widget_newTypeIdName("label", "label", NULL, setiw);
	dataobject_setValue(setc, "data", dataobjectfield_string("On"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("2"));
	setc = widget_newTypeIdName("label", "label", NULL, setiw);
	dataobject_setValue(setc, "data", dataobjectfield_string("Off"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	setc = widget_newTypeIdName("label", "label", NULL, setiw);
	dataobject_setValue(setc, "data", dataobjectfield_string("Default"));

	dobj1 = widget_newTypeIdName("box", NULL, NULL, root);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("320"));
	dataobject_setValue(dobj1, "height", dataobjectfield_string("220"));
	widget_setPacking(dobj1, WP_VERTICAL);

	dobj2 = widget_newTypeIdName("stack", NULL, "placeholder", dobj1);
	dataobject_setValue(dobj2, "width", dataobjectfield_string("320"));
	dataobject_setValue(dobj2, "height", dataobjectfield_string("220"));
	widget_setPacking(dobj2, WP_VERTICAL);

	widget_newTypeIdName("box", NULL, NULL, dobj2);

    return output;
}
