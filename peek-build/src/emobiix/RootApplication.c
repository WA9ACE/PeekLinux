#include "RootApplication.h"

#include "DataObject.h"
#include "Widget.h"

#include <stdio.h>

DataObject *RootApplication(void)
{
    static DataObject *output = NULL;
	DataObject *dobj1, *root, *dobj2;

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

	dobj1 = widget_newTypeIdName("box", "solid", NULL, root);
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
