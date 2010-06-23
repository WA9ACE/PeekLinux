#include "BootApplication.h"

#include "DataObject.h"
#include "Widget.h"
#include "URL.h"

#include <stdio.h>

DataObject *BootApplication(void)
{
    static DataObject *output = NULL;
    DataObject *view, *root, *dobj1, *dobj2, *box;
#ifdef SIMULATOR
	URL *durl;
#endif

	DataObject *recordobj, *rec;
	DataObject *sbox, *label;

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

	dataobject_setValue(output, "script", dataobjectfield_string(
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

	box = widget_newTypeIdName("box", NULL, NULL, root);
	widget_setPacking(box, WP_HORIZONTAL);
	widget_setAlignment(box, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", "gradboxr", NULL, box);

    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/helloworld\"); dobj:toScreen();"));
	widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);
	/*widget_setPacking(dobj1, WP_VERTICAL);*/

	dobj2 = widget_newTypeIdName("string", "label", NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Hello World"));
    widget_setAlignment(dobj2, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", "gradboxr", NULL, box);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/calc\"); dobj:toScreen();"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("string", "label", NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Calculator"));
    widget_setAlignment(dobj2, WA_CENTER);

#ifdef SIMULATOR
	durl = url_parse("system://local/gps", URL_ALL);
	dobj1 = dataobject_construct(durl, 1);
	dataobject_setValue(dobj1, "long", dataobjectfield_string("40.702147"));
	dataobject_setValue(dobj1, "lat", dataobjectfield_string("-74.015794"));
#endif

	dobj1 = widget_newTypeIdName("button", "gradboxr", NULL, box);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"gps = DataObject.locate(\"system://local/gps\");\n"
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/whereami?\" .. gps:getValue(\"long\") .. \",\" .. gps:getValue(\"lat\"));\n"
			"dobj:toScreen();"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("string", "label", NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Where am I?"));
    widget_setAlignment(dobj2, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", "gradboxr", NULL, box);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/whereami?40.702147,-74.015794\");\n"
			"dobj:toScreen();"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("string", "label", NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("NoGPS"));
    widget_setAlignment(dobj2, WA_CENTER);


	dobj1 = widget_newTypeIdName("button", "gradboxr", NULL, box);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/mail\"); dobj:toScreen();"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("string", "label", NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Mail"));
    widget_setAlignment(dobj2, WA_CENTER);

	dobj1 = widget_newTypeIdName("entry", NULL, "output", root);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("80%"));
	dataobject_setValue(dobj1, "data", dataobjectfield_string("tcp://69.114.111.9:12345/"));
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"ths = DataObject.find(\"output\"); str = ths:getValue(); dobj = DataObject.locate(str); dobj:toScreen();"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);
	/*widget_setPacking(dobj1, WP_VERTICAL);*/

	dobj2 = widget_newTypeIdName("string", "label", NULL, root);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Running Applications"));
	dataobject_setValue(dobj2, "margintop", dataobjectfield_int(10));
    widget_setAlignment(dobj2, WA_LEFT);

#if 1
	recordobj = dataobject_new();
	dataobject_setRecordType(recordobj, 1);
	
#define REC(_minor, _str1, _str2) \
	rec = dataobject_new(); \
	dataobject_setStamp(rec, _minor, 0); \
	dataobject_setValue(rec, "str1", dataobjectfield_string(_str1)); \
	dataobject_setValue(rec, "str2", dataobjectfield_string(_str2)); \
	dataobject_appendRecord(recordobj, rec);

	REC(1, "Ryan", "One");
	REC(2, "Dr K", "Two");
	REC(3, "33", "Three");
	REC(4, "skinny", "Four");
	REC(5, "ding", "Five");
	REC(6, "dava", "Six");
	REC(7, "OG", "Seven");
	REC(8, "lefbac", "Eight");
	REC(9, "rhls", "Nine");
	REC(10, "Cru", "Ten");
	REC(11, "Krac", "Eleven");
	REC(12, "Jest", "Twelve");

	dobj1 = widget_newTypeIdName("array", "gradboxr", NULL, root);
	widget_setDataObject(dobj1, recordobj);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("90%"));
	dataobject_setValue(dobj1, "height", dataobjectfield_string("100"));
	dataobject_setValue(dobj1, "margin", dataobjectfield_int(2));
	widget_setPacking(dobj1, WP_VERTICAL);
	widget_setAlignment(dobj1, WA_CENTER);

	sbox = widget_newTypeIdName("box", "selbox", NULL, dobj1);
	dataobject_setValue(sbox, "width", dataobjectfield_string("100%"));
	dataobject_setValue(sbox, "margin", dataobjectfield_int(2));
	widget_setCanFocus(sbox, 1);
	widget_setPacking(sbox, WP_HORIZONTAL);

	label = widget_newTypeIdName("string", "label", NULL, sbox);
	dataobject_setValue(label, "width", dataobjectfield_string("10%"));
	dataobject_setValue(label, "data", dataobjectfield_string("M"));
	widget_setAlignment(label, WA_CENTER);

	label = widget_newTypeIdName("string", "label", NULL, sbox);
	dataobject_setValue(label, "datafield", dataobjectfield_string("str2"));
	dataobject_setValue(label, "arraysource", dataobjectfield_string("true"));
	dataobject_setValue(label, "width", dataobjectfield_string("40%"));

	label = widget_newTypeIdName("string", "label", NULL, sbox);
	dataobject_setValue(label, "datafield", dataobjectfield_string("str1"));
	dataobject_setValue(label, "arraysource", dataobjectfield_string("true"));
	dataobject_setValue(label, "width", dataobjectfield_string("40%"));
#endif

    return output;
}
