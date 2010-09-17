#include "BootApplication.h"

#include "DataObject.h"
#include "Widget.h"
#include "URL.h"

#include <stdio.h>

#include "p_malloc.h"

#ifdef SIMULATOR
#include "Mime.h"
char *mimedata;
int mimedata_size;
FILE *mimefile;
#endif

extern DataObject *dobjFromFile(const char *filename, DataObject *root);

DataObject *BootApplication(void)
{
	static Widget *w0 = NULL;
	Widget *w1, *w2, *w3, *w4, *w5;

	if (w0 != NULL)
		return w0;

	w0 = dataobject_new();
	dataobject_setValue(w0, "type", dataobjectfield_string("application"));
	dataobject_setValue(w0, "description", dataobjectfield_string("Construct program for loading other programs"));
	dataobject_setValue(w0, "name", dataobjectfield_string("Boot Environment"));
	dataobject_setValue(w0, "onload", dataobjectfield_string("onLoad()"));
	dataobject_setValue(w0, "startupview", dataobjectfield_string("mainview"));
	dataobject_setValue(w0, "style", dataobjectfield_string("bootstyle"));
	w1 = dataobject_new();
	dataobject_pack(w0, w1);
	dataobject_setValue(w1, "type", dataobjectfield_string("script"));
	dataobject_setValue(w1, "data", dataobjectfield_string(
"	function onLoad()\n"
"		print \"Boot application loaded\"\n"
"	end\n"
"	function helloworld()\n"
"		DataObject.locate(\"tcp://69.114.111.9:12345/helloworld\");\n"
"	end\n"
"	function calculator()\n"
"		DataObject.locate(\"tcp://69.114.111.9:12345/calc\");\n"
"	end\n"
"	function maps()\n"
"		DataObject.locate(\"tcp://69.114.111.9:12345/whereami?40.702147,-74.015794\");\n"
"	end\n"
"	function aim()\n"
"		DataObject.locate(\"tcp://69.114.111.9:12345/sample\");\n"
"	end\n"
"	function mail()\n"
"		DataObject.locate(\"tcp://69.114.111.9:12345/mail\");\n"
"	end\n"));
	w1 = dataobject_new();
	dataobject_pack(w0, w1);
	dataobject_setValue(w1, "type", dataobjectfield_string("style"));
	dataobject_setValue(w1, "name", dataobjectfield_string("bootstyle"));
	w2 = dataobject_new();
	dataobject_pack(w1, w2);
	dataobject_setValue(w2, "type", dataobjectfield_string("selbox"));
	w3 = dataobject_new();
	dataobject_pack(w2, w3);
	dataobject_setValue(w3, "type", dataobjectfield_string("label"));
	dataobject_setValue(w3, "font-color", dataobjectfield_string("#FFF"));
	dataobject_setValue(w3, "renderer", dataobjectfield_string("label"));
	w2 = dataobject_new();
	dataobject_pack(w1, w2);
	dataobject_setValue(w2, "type", dataobjectfield_string("selbox"));
	dataobject_setValue(w2, "border", dataobjectfield_string("1234"));
	dataobject_setValue(w2, "border-color", dataobjectfield_string("#000"));
	dataobject_setValue(w2, "border-corners", dataobjectfield_string("1234"));
	dataobject_setValue(w2, "fill", dataobjectfield_string("gradient"));
	dataobject_setValue(w2, "hasfocus", dataobjectfield_string("1"));
	dataobject_setValue(w2, "radius", dataobjectfield_string("7"));
	dataobject_setValue(w2, "renderer", dataobjectfield_string("box"));
	dataobject_setValue(w2, "rounded", dataobjectfield_string("1234"));
	w3 = dataobject_new();
	dataobject_pack(w2, w3);
	dataobject_setValue(w3, "type", dataobjectfield_string("gradient"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("stop"));
	dataobject_setValue(w4, "color", dataobjectfield_string("#F9F9F9FF"));
	dataobject_setValue(w4, "position", dataobjectfield_string("0"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("stop"));
	dataobject_setValue(w4, "color", dataobjectfield_string("#59DFFEFF"));
	dataobject_setValue(w4, "position", dataobjectfield_string("50"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("stop"));
	dataobject_setValue(w4, "color", dataobjectfield_string("#35dcfbFF"));
	dataobject_setValue(w4, "position", dataobjectfield_string("50"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("stop"));
	dataobject_setValue(w4, "color", dataobjectfield_string("#b7f1ffFF"));
	dataobject_setValue(w4, "position", dataobjectfield_string("100"));
	w3 = dataobject_new();
	dataobject_pack(w2, w3);
	dataobject_setValue(w3, "type", dataobjectfield_string("label"));
	dataobject_setValue(w3, "font-color", dataobjectfield_string("#000"));
	dataobject_setValue(w3, "renderer", dataobjectfield_string("label"));
	dataobject_setValue(w3, "weight", dataobjectfield_string("bold"));
	w1 = dataobject_new();
	dataobject_pack(w0, w1);
	dataobject_setValue(w1, "type", dataobjectfield_string("view"));
	dataobject_setValue(w1, "height", dataobjectfield_string("240"));
	dataobject_setValue(w1, "name", dataobjectfield_string("mainview"));
	dataobject_setValue(w1, "packing", dataobjectfield_string("vertical"));
	dataobject_setValue(w1, "width", dataobjectfield_string("320"));
	w2 = dobjFromFile("background.png", w1);
	dataobject_setValue(w2, "packing", dataobjectfield_string("vertical"));
	dataobject_setValue(w2, "width", dataobjectfield_int(320));
	dataobject_setValue(w2, "height", dataobjectfield_int(240));
	dataobject_setValue(w2, "pixelformat", dataobjectfield_int(0));
	w3 = dataobject_new();
	dataobject_pack(w2, w3);
	dataobject_setValue(w3, "type", dataobjectfield_string("box"));
	dataobject_setValue(w3, "alignment", dataobjectfield_string("center"));
	dataobject_setValue(w3, "margintop", dataobjectfield_string("40"));
	dataobject_setValue(w3, "packing", dataobjectfield_string("hgrid"));
	dataobject_setValue(w3, "width", dataobjectfield_string("90%"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("selbox"));
	dataobject_setValue(w4, "canfocus", dataobjectfield_string("1"));
	dataobject_setValue(w4, "margin", dataobjectfield_string("3"));
	dataobject_setValue(w4, "onreturn", dataobjectfield_string("helloworld()"));
	dataobject_setValue(w4, "packing", dataobjectfield_string("vertical"));
	w5 = dobjFromFile("hello.png", w4);
	dataobject_setValue(w5, "marginleft", dataobjectfield_string("4"));
	dataobject_setValue(w5, "marginright", dataobjectfield_string("4"));
	dataobject_setValue(w5, "margintop", dataobjectfield_string("4"));
	dataobject_setValue(w5, "width", dataobjectfield_int(32));
	dataobject_setValue(w5, "height", dataobjectfield_int(32));
	dataobject_setValue(w5, "pixelformat", dataobjectfield_int(5));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setValue(w5, "type", dataobjectfield_string("label"));
	dataobject_setValue(w5, "alignment", dataobjectfield_string("center"));
	dataobject_setValue(w5, "data", dataobjectfield_string("Hello"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("selbox"));
	dataobject_setValue(w4, "canfocus", dataobjectfield_string("1"));
	dataobject_setValue(w4, "margin", dataobjectfield_string("3"));
	dataobject_setValue(w4, "onreturn", dataobjectfield_string("calculator()"));
	dataobject_setValue(w4, "packing", dataobjectfield_string("vertical"));
	w5 = dobjFromFile("calc.png", w4);
	dataobject_setValue(w5, "marginleft", dataobjectfield_string("4"));
	dataobject_setValue(w5, "marginright", dataobjectfield_string("4"));
	dataobject_setValue(w5, "margintop", dataobjectfield_string("4"));
	dataobject_setValue(w5, "width", dataobjectfield_int(32));
	dataobject_setValue(w5, "height", dataobjectfield_int(32));
	dataobject_setValue(w5, "pixelformat", dataobjectfield_int(5));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setValue(w5, "type", dataobjectfield_string("label"));
	dataobject_setValue(w5, "alignment", dataobjectfield_string("center"));
	dataobject_setValue(w5, "data", dataobjectfield_string("Calc"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("selbox"));
	dataobject_setValue(w4, "canfocus", dataobjectfield_string("1"));
	dataobject_setValue(w4, "margin", dataobjectfield_string("3"));
	dataobject_setValue(w4, "onreturn", dataobjectfield_string("maps()"));
	dataobject_setValue(w4, "packing", dataobjectfield_string("vertical"));
	w5 = dobjFromFile("maps.png", w4);
	dataobject_setValue(w5, "marginleft", dataobjectfield_string("4"));
	dataobject_setValue(w5, "marginright", dataobjectfield_string("4"));
	dataobject_setValue(w5, "margintop", dataobjectfield_string("4"));
	dataobject_setValue(w5, "width", dataobjectfield_int(32));
	dataobject_setValue(w5, "height", dataobjectfield_int(32));
	dataobject_setValue(w5, "pixelformat", dataobjectfield_int(5));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setValue(w5, "type", dataobjectfield_string("label"));
	dataobject_setValue(w5, "alignment", dataobjectfield_string("center"));
	dataobject_setValue(w5, "data", dataobjectfield_string("Maps"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("selbox"));
	dataobject_setValue(w4, "canfocus", dataobjectfield_string("1"));
	dataobject_setValue(w4, "margin", dataobjectfield_string("3"));
	dataobject_setValue(w4, "onreturn", dataobjectfield_string("aim()"));
	dataobject_setValue(w4, "packing", dataobjectfield_string("vertical"));
	w5 = dobjFromFile("aim.png", w4);
	dataobject_setValue(w5, "marginleft", dataobjectfield_string("4"));
	dataobject_setValue(w5, "marginright", dataobjectfield_string("4"));
	dataobject_setValue(w5, "margintop", dataobjectfield_string("4"));
	dataobject_setValue(w5, "width", dataobjectfield_int(32));
	dataobject_setValue(w5, "height", dataobjectfield_int(32));
	dataobject_setValue(w5, "pixelformat", dataobjectfield_int(5));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setValue(w5, "type", dataobjectfield_string("label"));
	dataobject_setValue(w5, "alignment", dataobjectfield_string("center"));
	dataobject_setValue(w5, "data", dataobjectfield_string("AIM"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setValue(w4, "type", dataobjectfield_string("selbox"));
	dataobject_setValue(w4, "canfocus", dataobjectfield_string("1"));
	dataobject_setValue(w4, "margin", dataobjectfield_string("3"));
	dataobject_setValue(w4, "onreturn", dataobjectfield_string("web()"));
	dataobject_setValue(w4, "packing", dataobjectfield_string("vertical"));
	w5 = dobjFromFile("netsurf.png", w4);
	dataobject_setValue(w5, "marginleft", dataobjectfield_string("4"));
	dataobject_setValue(w5, "marginright", dataobjectfield_string("4"));
	dataobject_setValue(w5, "margintop", dataobjectfield_string("4"));
	dataobject_setValue(w5, "width", dataobjectfield_int(32));
	dataobject_setValue(w5, "height", dataobjectfield_int(32));
	dataobject_setValue(w5, "pixelformat", dataobjectfield_int(5));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setValue(w5, "type", dataobjectfield_string("label"));
	dataobject_setValue(w5, "alignment", dataobjectfield_string("center"));
	dataobject_setValue(w5, "data", dataobjectfield_string("Web"));

	return w0;
}

#if 0
    static DataObject *output = NULL;
    DataObject *view, *root, *dobj1, *dobj2, *box;
#ifdef SIMULATOR
	URL *durl;
#endif

	/*DataObject *recordobj;
	DataObject *rec;
	DataObject *sbox, *label;*/

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
	dataobject_setValue(view, "width", dataobjectfield_string("320"));
	dataobject_setValue(view, "height", dataobjectfield_string("220"));
    widget_setPacking(view, WP_VERTICAL);

    root = widget_newTypeIdName("box", "gradbox", NULL, view);
	dataobject_setValue(root, "width", dataobjectfield_string("100%"));
	dataobject_setValue(root, "height", dataobjectfield_string("100%"));
    widget_setPacking(root, WP_VERTICAL);

    dobj1 = widget_newTypeIdName(NULL, NULL, NULL, root);
    dataobject_setValue(dobj1, "data", dataobjectfield_string("emobiix"));
	dataobject_setValue(dobj1, "weight", dataobjectfield_string("bold"));
    widget_setAlignment(dobj1, WA_CENTER);

	dobj1 = widget_newTypeIdName("label", NULL, NULL, root);
    dataobject_setValue(dobj1, "data", dataobjectfield_string("Application Boot Environment"));
    widget_setAlignment(dobj1, WA_CENTER);

	box = widget_newTypeIdName("box", NULL, NULL, root);
	widget_setPacking(box, WP_HORIZONTAL);
	widget_setAlignment(box, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);

    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
/*			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/synctest\");"));*/
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/sample\");"));
	widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);
	/*widget_setPacking(dobj1, WP_VERTICAL);*/

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Hello World"));
    widget_setAlignment(dobj2, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/calc\");"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Calculator"));
    widget_setAlignment(dobj2, WA_CENTER);

#ifdef SIMULATOR
	durl = url_parse("system://local/gps", URL_ALL);
	dobj1 = dataobject_construct(durl, 1);
	dataobject_setValue(dobj1, "long", dataobjectfield_string("40.702147"));
	dataobject_setValue(dobj1, "lat", dataobjectfield_string("-74.015794"));
#endif

	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"gps = DataObject.locate(\"system://local/gps\");\n"
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/whereami?\" .. gps:getValue(\"long\") .. \",\" .. gps:getValue(\"lat\"));\n"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Where am I?"));
    widget_setAlignment(dobj2, WA_CENTER);


	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/mail\");"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Mail"));
    widget_setAlignment(dobj2, WA_CENTER);

	box = widget_newTypeIdName("box", NULL, NULL, root);
	widget_setPacking(box, WP_HORIZONTAL);
	widget_setAlignment(box, WA_CENTER);

	dobj1 = widget_newTypeIdName("entry", NULL, "lat", box);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("40%"));
	dataobject_setValue(dobj1, "data", dataobjectfield_string("40.702147"));
	widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj1 = widget_newTypeIdName("entry", NULL, "long", box);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("40%"));
	dataobject_setValue(dobj1, "data", dataobjectfield_string("-74.015794"));
	widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"Llat = DataObject.find(\"lat\");\n"
			"Llong = DataObject.find(\"long\");\n"
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/whereami?\" .. Llat:getValue() .. \",\" .. Llong:getValue());"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("NoGPS"));
    widget_setAlignment(dobj2, WA_CENTER);

#if 0
	dobj1 = widget_newTypeIdName("entry", NULL, "output", root);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("80%"));
	dataobject_setValue(dobj1, "data", dataobjectfield_string("tcp://69.114.111.9:12345/"));
    dataobject_setValue(dobj1, "onreturn", dataobjectfield_string(
			"ths = DataObject.find(\"output\"); str = ths:getValue(); dobj = DataObject.locate(str); dobj:toScreen();"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);
	/*widget_setPacking(dobj1, WP_VERTICAL);*/
#endif

#if 0
	/* png test */
	mimefile = fopen("letter.png", "rb");
	fseek(mimefile, 0, SEEK_END);
	mimedata_size = (int)ftell(mimefile);
	fseek(mimefile, 0, SEEK_SET);
	mimedata = (char *)p_malloc(mimedata_size);
	fread(mimedata, 1, mimedata_size, mimefile);
	fclose(mimefile);
	dobj1 = widget_newTypeIdName("image", "image", NULL, root);
	dataobject_setValue(dobj1, "mime-type", dataobjectfield_string("png"));
	dataobject_setValue(dobj1, "data", dataobjectfield_data(mimedata, mimedata_size));
	dataobject_setValue(dobj1, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(dobj1, "color", dataobjectfield_string("FF00FF00"));
	mime_load(dobj1);

	dobj1 = widget_newTypeIdName("entry", NULL, NULL, root);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("90%"));
	dataobject_setValue(dobj1, "height", dataobjectfield_string("52"));
	dataobject_setValue(dobj1, "multiline", dataobjectfield_string("true"));
	widget_setAlignment(dobj1, WA_CENTER);
	dataobject_setValue(dobj1, "data", dataobjectfield_string("This is some bold red italic text. "
			"So we put some more text in here because we are trying to test how well a long segment of text "
			"will work since we are trying to implement word line breaks, scrollbar information, basic markup, yknow, all the "
			"stuff you would normally have in a reasonable text engine only for a mobile platform. "
			"Must remember to look into that xadvance issue on the platform itself, probably something with the freetype2 config."));

	dobj1 = widget_newTypeIdName("text", NULL, NULL, root);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("90%"));
	/*dataobject_setValue(dobj1, "height", dataobjectfield_string("65"));*/
	widget_setAlignment(dobj1, WA_CENTER);
	dataobject_setValue(dobj1, "data", dataobjectfield_string("This <gAAAAFF00>is</g> <u>some</u> <b>bold</b> <cFF000000>red</c> <i>italic</i> text.\n"
			"So we put some more text in here because we are trying to test how well a long segment of text "
			"will work since we are trying to implement word line breaks."));
#endif

	/* array test */
#if 0
	dobj2 = widget_newTypeIdName("label", NULL, NULL, root);
    dataobject_setValue(dobj2, "data", dataobjectfield_string("Running Applications"));
	dataobject_setValue(dobj2, "margintop", dataobjectfield_int(10));
    widget_setAlignment(dobj2, WA_LEFT);

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

	label = widget_newTypeIdName("label", NULL, NULL, sbox);
	dataobject_setValue(label, "width", dataobjectfield_string("10%"));
	dataobject_setValue(label, "data", dataobjectfield_string("M"));
	widget_setAlignment(label, WA_CENTER);

	label = widget_newTypeIdName("label", NULL, NULL, sbox);
	dataobject_setValue(label, "datafield", dataobjectfield_string("str2"));
	dataobject_setValue(label, "arraysource", dataobjectfield_string("true"));
	dataobject_setValue(label, "width", dataobjectfield_string("40%"));

	label = widget_newTypeIdName("label", NULL, NULL, sbox);
	dataobject_setValue(label, "datafield", dataobjectfield_string("str1"));
	dataobject_setValue(label, "arraysource", dataobjectfield_string("true"));
	dataobject_setValue(label, "width", dataobjectfield_string("40%"));
#endif

    return output;
}
#endif