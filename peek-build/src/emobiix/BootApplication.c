#include "BootApplication.h"

#include "DataObject.h"
#include "Widget.h"

#include <stdio.h>

DataObject *BootApplication(void)
{
    static DataObject *output = NULL;
    DataObject *view, *root, *dobj1, *dobj2, *script;

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
            dataobjectfield_string("mainview"));
    dataobject_setValue(output, "onLoad",
            dataobjectfield_string("print(\"BootApp: Loaded\")"));
    dataobject_setValue(output, "onFocus",
            dataobjectfield_string("print(\"BootApp: Focused\")"));
    dataobject_setValue(output, "onUnFocus",
            dataobjectfield_string("print(\"BootApp: UnFocused\")"));

#if 0
    script = dataobject_new();
    dataobject_setValue(script, "type", dataobjectfield_string("script"));
    dataobject_setValue(script, "data", dataobjectfield_string(
#else
	dataobject_setValue(output, "script", dataobjectfield_string(
#endif
"customVar = \"hello\"\n"
"function setIt (n)\n"
"    dobj = DataObject.find(\"output\")\n"
"    dobj:setValue(n)\n"
"end\n"
"function getIt ()\n"
"    dobj = DataObject.find(\"output\")\n"
"    return dobj:getValue()\n"
"end"));

    view = widget_newTypeIdName("view", NULL, "mainview", output);
	/*dataobject_setValue(view, "width", dataobjectfield_string("320"));
	dataobject_setValue(view, "height", dataobjectfield_string("220"));*/
    widget_setPacking(view, WP_VERTICAL);

    root = widget_newTypeIdName("box", "gradbox", NULL, view);
	dataobject_setValue(root, "width", dataobjectfield_string("100%"));
	dataobject_setValue(root, "height", dataobjectfield_string("100%"));
    widget_setPacking(root, WP_VERTICAL);

    dobj1 = widget_newTypeIdName("string", "biglabel", NULL, root);
    dataobject_setValue(dobj1, "data", dataobjectfield_string("emobiix"));
    widget_setAlignment(dobj1, WA_CENTER);

	dobj1 = widget_newTypeIdName("string", "label", NULL, root);
    dataobject_setValue(dobj1, "data", dataobjectfield_string("Application Boot Environment"));
    widget_setAlignment(dobj1, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", "gradboxr", NULL, root);

    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/dataobject\"); dobj:toScreen();"));
	widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);
	/*widget_setPacking(dobj1, WP_VERTICAL);*/

	dobj2 = widget_newTypeIdName("string", "label", NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Hello World"));
    widget_setAlignment(dobj2, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", "gradboxr", NULL, root);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/calc\"); dobj:toScreen();"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);
	/*widget_setPacking(dobj1, WP_VERTICAL);*/

	dobj2 = widget_newTypeIdName("string", "label", NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Calculator"));
    widget_setAlignment(dobj2, WA_CENTER);

	dobj1 = widget_newTypeIdName("entry", NULL, "output", root);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("80%"));
	dataobject_setValue(dobj1, "data", dataobjectfield_string("tcp://69.114.111.9:12345/"));
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"ths = DataObject.find(\"output\"); str = ths:getValue(); dobj = DataObject.locate(str); dobj:toScreen();"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);
	/*widget_setPacking(dobj1, WP_VERTICAL);*/

    return output;
}
