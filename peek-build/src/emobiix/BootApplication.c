#include "BootApplication.h"

#include "DataObject.h"
#include "Widget.h"
#include "URL.h"

#include <stdio.h>

#include "p_malloc.h"
#include "system_time.h"

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
	Widget *w1, *w2, *w3, *w4, *w5, *w6;

	if (w0 != NULL)
		return w0;

	w0 = dataobject_new();
	dataobject_setEnum(w0, EMO_FIELD_TYPE, dataobjectfield_string("application"));
	dataobject_setEnum(w0, EMO_FIELD_DESCRIPTION, dataobjectfield_string("Construct program for loading other programs"));
	dataobject_setEnum(w0, EMO_FIELD_FULLSCREEN, dataobjectfield_string("1"));
	dataobject_setEnum(w0, EMO_FIELD_NAME, dataobjectfield_string("Boot Environment"));
	dataobject_setEnum(w0, EMO_FIELD_ONLOAD, dataobjectfield_string("onLoad()"));
	dataobject_setEnum(w0, EMO_FIELD_STARTUPVIEW, dataobjectfield_string("mainview"));
	dataobject_setEnum(w0, EMO_FIELD_STYLE, dataobjectfield_string("bootstyle"));
	w1 = dataobject_new();
	dataobject_pack(w0, w1);
	dataobject_setEnum(w1, EMO_FIELD_TYPE, dataobjectfield_string("script"));
	dataobject_setEnum(w1, EMO_FIELD_DATA, dataobjectfield_string(
"focusIndex = 1\n"
"function onLoad()\n"
"	print \"Boot application loaded\"\n"
"end\n"
"function helloworld()\n"
"	DataObject.locate(\"tcp://69.114.111.9:12345/helloworld\")\n"
"end\n"
"function calculator()\n"
"	DataObject.locate(\"tcp://69.114.111.9:12345/calc\")\n"
"end\n"
"function maps()\n"
" gprs = DataObject.locate(\"system://local/gprs\")\n"
"	DataObject.locate(\"tcp://69.114.111.9:12345/whereami?\" .. gprs:getValue(\"lac\") .. \",\" .. gprs:getValue(\"ci\"))\n"
"end\n"
"function aim()\n"
"	DataObject.locate(\"tcp://69.114.111.9:12345/aimobiix\")\n"
"end\n"
"function mail()\n"
"	DataObject.locate(\"tcp://69.114.111.9:12345/mail\")\n"
"end\n"
"\n"
"function launch()\n"
"	if focusIndex == 1 then\n"
"		helloworld()\n"
"	elseif focusIndex == 2 then\n"
"		calculator()\n"
"	elseif focusIndex == 3 then\n"
"		maps()\n"
"	elseif focusIndex == 4 then\n"
"		aim()\n"
"	elseif focusIndex == 5 then\n"
"		DataObject.netsurf()\n"
"	elseif focusIndex == 6 then\n"
"		print(\"empty\")\n"
"	end\n"
"end\n"
"\n"
"function focusNext()\n"
"	focusIndex = focusIndex + 1\n"
"	if focusIndex == 7 then\n"
"		focusIndex = 1\n"
"	end\n"
"	refocus()\n"
"end\n"
"function focusPrev()\n"
"	focusIndex = focusIndex - 1\n"
"	if focusIndex == 0 then\n"
"		focusIndex = 6\n"
"	end\n"
"	refocus()\n"
"end\n"
"function refocus()\n"
"	idx = focusIndex\n"
"\n"
"	for i=1,6 do\n"
"		app = DataObject.find(\"app\" .. i)\n"
"		app:setValue(\"reference\", \"img\" .. idx)\n"
"		idx = idx + 1\n"
"		if idx == 7 then idx = 1 end\n"
"	end\n"
" app = DataObject.find(\"app1\")\n"
" img = DataObject.find(app:getValue(\"reference\"))\n"
" title = DataObject.find(\"appTitle\")\n"
" title:setValue(img:getValue(\"title\"))\n"
"end\n"
"function showmenu()\n"
"dobj = DataObject.find(\"appmenu\");\n"
"dobj:toScreen()\n"
"end\n"));
	w1 = dobjFromFile("clock-numbers.png", w0);
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("clock"));
	w1 = dataobject_new();
	dataobject_pack(w0, w1);
	dataobject_setEnum(w1, EMO_FIELD_TYPE, dataobjectfield_string("style"));
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("bootstyle"));
	w2 = dataobject_new();
	dataobject_pack(w1, w2);
	dataobject_setEnum(w2, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w2, EMO_FIELD_RENDERER, dataobjectfield_string("image"));
	w1 = dataobject_new();
	dataobject_pack(w0, w1);
	dataobject_setEnum(w1, EMO_FIELD_TYPE, dataobjectfield_string("view"));
	dataobject_setEnum(w1, EMO_FIELD_HEIGHT, dataobjectfield_string("240"));
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("mainview"));
	dataobject_setEnum(w1, EMO_FIELD_PACKING, dataobjectfield_string("vertical"));
	dataobject_setEnum(w1, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	w2 = dobjFromFile("background.png", w1);
	dataobject_setEnum(w2, EMO_FIELD_NAME, dataobjectfield_string("background"));
	dataobject_setEnum(w2, EMO_FIELD_PACKING, dataobjectfield_string("vertical"));
	w3 = dataobject_new();
	dataobject_pack(w2, w3);
	dataobject_setEnum(w3, EMO_FIELD_TYPE, dataobjectfield_string("label"));
	dataobject_setEnum(w3, EMO_FIELD_NAME, dataobjectfield_string("weatherinfo"));
	dataobject_setEnum(w3, EMO_FIELD_X, dataobjectfield_string("150"));
	dataobject_setEnum(w3, EMO_FIELD_Y, dataobjectfield_string("5"));
	dataobject_setEnum(w3, EMO_FIELD_WIDTH, dataobjectfield_string("100"));
	dataobject_setEnum(w3, EMO_FIELD_HEIGHT, dataobjectfield_string("12"));
	dataobject_setEnum(w3, EMO_FIELD_FONTCOLOR, dataobjectfield_string("#FFF"));
	dataobject_setEnum(w3, EMO_FIELD_WIDTH, dataobjectfield_string("bold"));
	dataobject_setEnum(w3, EMO_FIELD_DATA, dataobjectfield_string(" --- "));
	w3 = dataobject_new();
	dataobject_pack(w2, w3);
	dataobject_setEnum(w3, EMO_FIELD_TYPE, dataobjectfield_string("stack"));
	dataobject_setEnum(w3, EMO_FIELD_ACCESSKEY, dataobjectfield_string("ACTIVATE"));
	dataobject_setEnum(w3, EMO_FIELD_HEIGHT, dataobjectfield_string("100%"));
	dataobject_setEnum(w3, EMO_FIELD_ONRETURN, dataobjectfield_string("showmenu()"));
	dataobject_setEnum(w3, EMO_FIELD_WIDTH, dataobjectfield_string("100%"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("set"));
	dataobject_setEnum(w4, EMO_FIELD_FIELDNAME, dataobjectfield_string("H1"));
	dataobject_setEnum(w4, EMO_FIELD_HEIGHT, dataobjectfield_string("37"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string(SYSTEM_TIME_URI));
	dataobject_setEnum(w4, EMO_FIELD_WIDTH, dataobjectfield_string("6"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("227"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("90"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(1));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("37"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("6"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("0"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("0"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("set"));
	dataobject_setEnum(w4, EMO_FIELD_FIELDNAME, dataobjectfield_string("H2"));
	dataobject_setEnum(w4, EMO_FIELD_HEIGHT, dataobjectfield_string("53"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string(SYSTEM_TIME_URI));
	dataobject_setEnum(w4, EMO_FIELD_WIDTH, dataobjectfield_string("20"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("232"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("70"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(0));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("52"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("20"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("170"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("0"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(1));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("37"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("9"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("160"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("2"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINTOP, dataobjectfield_string("10"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(2));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("49"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("17"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("142"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("2"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINTOP, dataobjectfield_string("4"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(3));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("51"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("19"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("122"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("1"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINTOP, dataobjectfield_string("2"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(4));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("38"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("19"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("102"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("2"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINTOP, dataobjectfield_string("2"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(5));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("52"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("18"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("84"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("1"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(6));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("52"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("19"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("64"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("1"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(7));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("40"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("20"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("46"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("1"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINTOP, dataobjectfield_string("2"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(8));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("53"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("20"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("24"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("1"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(9));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("53"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("19"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("6"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("1"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("set"));
	dataobject_setEnum(w4, EMO_FIELD_FIELDNAME, dataobjectfield_string("M1"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string(SYSTEM_TIME_URI));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("256"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("42"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(0));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("62"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("28"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("132"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("54"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(1));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("47"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("14"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("116"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("57"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINLEFT, dataobjectfield_string("14"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(2));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("66"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("26"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("88"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("55"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(3));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("66"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("28"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("60"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("55"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(4));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("47"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("27"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("32"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("57"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(5));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("61"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("29"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("2"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("57"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("set"));
	dataobject_setEnum(w4, EMO_FIELD_FIELDNAME, dataobjectfield_string("M2"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string(SYSTEM_TIME_URI));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("280"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("5"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(0));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("76"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("37"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("356"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("127"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(1));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("51"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("22"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("332"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("133"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINLEFT, dataobjectfield_string("12"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINTOP, dataobjectfield_string("15"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(2));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("76"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("38"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("292"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("129"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(3));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("77"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("38"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("252"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("129"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(4));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("55"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("40"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("212"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("129"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(5));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("80"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("42"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("170"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("125"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(6));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("76"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("40"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("128"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("125"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(7));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("53"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("39"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("88"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("128"));
	dataobject_setEnum(w6, EMO_FIELD_MARGINTOP, dataobjectfield_string("5"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(8));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("77"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("37"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("48"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("128"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w5 = dataobject_new();
	dataobject_pack(w4, w5);
	dataobject_setEnum(w5, EMO_FIELD_TYPE, dataobjectfield_string("setitem"));
	dataobject_setEnum(w5, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(9));
	w6 = dataobject_new();
	dataobject_pack(w5, w6);
	dataobject_setEnum(w6, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w6, EMO_FIELD_IMGHEIGHT, dataobjectfield_string("76"));
	dataobject_setEnum(w6, EMO_FIELD_IMGWIDTH, dataobjectfield_string("44"));
	dataobject_setEnum(w6, EMO_FIELD_IMGX, dataobjectfield_string("2"));
	dataobject_setEnum(w6, EMO_FIELD_IMGY, dataobjectfield_string("127"));
	dataobject_setEnum(w6, EMO_FIELD_REFERENCE, dataobjectfield_string("clock"));
	w1 = dobjFromFile("hello.png", w0);
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("img1"));
	dataobject_setEnum(w1, EMO_FIELD_TITLE, dataobjectfield_string("Text\nMessaging"));
	w1 = dobjFromFile("calc.png", w0);
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("img2"));
	dataobject_setEnum(w1, EMO_FIELD_TITLE, dataobjectfield_string("Calculator"));
	w1 = dobjFromFile("maps.png", w0);
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("img3"));
	dataobject_setEnum(w1, EMO_FIELD_TITLE, dataobjectfield_string("Maps"));
	w1 = dobjFromFile("aim.png", w0);
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("img4"));
	dataobject_setEnum(w1, EMO_FIELD_TITLE, dataobjectfield_string("Instant\nMessenger"));
	w1 = dobjFromFile("netsurf.png", w0);
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("img5"));
	dataobject_setEnum(w1, EMO_FIELD_TITLE, dataobjectfield_string("Web\nBrowser"));
	w1 = dobjFromFile("weather.png", w0);
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("img6"));
	dataobject_setEnum(w1, EMO_FIELD_TITLE, dataobjectfield_string("Weather"));
	w1 = dataobject_new();
	dataobject_pack(w0, w1);
	dataobject_setEnum(w1, EMO_FIELD_TYPE, dataobjectfield_string("view"));
	dataobject_setEnum(w1, EMO_FIELD_HEIGHT, dataobjectfield_string("240"));
	dataobject_setEnum(w1, EMO_FIELD_NAME, dataobjectfield_string("appmenu"));
	dataobject_setEnum(w1, EMO_FIELD_PACKING, dataobjectfield_string("vertical"));
	dataobject_setEnum(w1, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	w2 = dataobject_new();
	dataobject_pack(w1, w2);
	dataobject_setEnum(w2, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w2, EMO_FIELD_ACCESSKEY, dataobjectfield_string("BACK"));
	dataobject_setEnum(w2, EMO_FIELD_ONRETURN, dataobjectfield_string("dobj = DataObject.find(\"mainview\");dobj:toScreen()"));
	dataobject_setEnum(w2, EMO_FIELD_PACKING, dataobjectfield_string("vertical"));
	dataobject_setEnum(w2, EMO_FIELD_REFERENCE, dataobjectfield_string("background"));
	w4 = dataobject_new();
	dataobject_pack(w2, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("box"));
	dataobject_setEnum(w4, EMO_FIELD_ID, dataobjectfield_string("solid"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("0"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("0"));
	dataobject_setEnum(w4, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	dataobject_setEnum(w4, EMO_FIELD_HEIGHT, dataobjectfield_string("240"));
	dataobject_setEnum(w4, EMO_FIELD_BACKGROUNDCOLOR, dataobjectfield_string("#000000AA"));
	w4 = dobjFromFile("glow.png", w2);
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("86"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("59"));
	
	w3 = dataobject_new();
	dataobject_pack(w4, w3);
	dataobject_setEnum(w3, EMO_FIELD_TYPE, dataobjectfield_string("stack"));
	dataobject_setEnum(w3, EMO_FIELD_ACCESSKEY, dataobjectfield_string("ACTIVATE"));
	dataobject_setEnum(w3, EMO_FIELD_HEIGHT, dataobjectfield_string("100%"));
	dataobject_setEnum(w3, EMO_FIELD_ONRETURN, dataobjectfield_string("launch()"));
	dataobject_setEnum(w3, EMO_FIELD_WIDTH, dataobjectfield_string("100%"));

	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("text"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("126"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("100"));
	dataobject_setEnum(w4, EMO_FIELD_WIDTH, dataobjectfield_string("80"));
	dataobject_setEnum(w4, EMO_FIELD_FONTCOLOR, dataobjectfield_string("#FFF"));
	dataobject_setEnum(w4, EMO_FIELD_NAME, dataobjectfield_string("appTitle"));
	dataobject_setEnum(w4, EMO_FIELD_MULTILINE, dataobjectfield_string("1"));
	dataobject_setEnum(w4, EMO_FIELD_DATA, dataobjectfield_string("     <b>Text\nMessaging"));

	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w4, EMO_FIELD_NAME, dataobjectfield_string("app1"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string("img1"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("130"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("13"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w4, EMO_FIELD_ACCESSKEY, dataobjectfield_string("NEXT"));
	dataobject_setEnum(w4, EMO_FIELD_ALPHA, dataobjectfield_string("225"));
	dataobject_setEnum(w4, EMO_FIELD_NAME, dataobjectfield_string("app2"));
	dataobject_setEnum(w4, EMO_FIELD_ONRETURN, dataobjectfield_string("focusNext()"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string("img2"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("203"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("57"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w4, EMO_FIELD_ACCESSKEY, dataobjectfield_string("PREV"));
	dataobject_setEnum(w4, EMO_FIELD_ALPHA, dataobjectfield_string("200"));
	dataobject_setEnum(w4, EMO_FIELD_NAME, dataobjectfield_string("app3"));
	dataobject_setEnum(w4, EMO_FIELD_ONRETURN, dataobjectfield_string("focusPrev()"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string("img3"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("203"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("127"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w4, EMO_FIELD_ALPHA, dataobjectfield_string("175"));
	dataobject_setEnum(w4, EMO_FIELD_NAME, dataobjectfield_string("app4"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string("img4"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("130"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("172"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w4, EMO_FIELD_ALPHA, dataobjectfield_string("150"));
	dataobject_setEnum(w4, EMO_FIELD_NAME, dataobjectfield_string("app5"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string("img5"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("58"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("127"));
	w4 = dataobject_new();
	dataobject_pack(w3, w4);
	dataobject_setEnum(w4, EMO_FIELD_TYPE, dataobjectfield_string("image"));
	dataobject_setEnum(w4, EMO_FIELD_ALPHA, dataobjectfield_string("125"));
	dataobject_setEnum(w4, EMO_FIELD_NAME, dataobjectfield_string("app6"));
	dataobject_setEnum(w4, EMO_FIELD_REFERENCE, dataobjectfield_string("img6"));
	dataobject_setEnum(w4, EMO_FIELD_X, dataobjectfield_string("58"));
	dataobject_setEnum(w4, EMO_FIELD_Y, dataobjectfield_string("57"));

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
    dataobject_setEnum(output, EMO_FIELD_TYPE, dataobjectfield_string("application"));
    dataobject_setEnum(output, EMO_FIELD_NAME, dataobjectfield_string("Boot Manager"));
    dataobject_setEnum(output, "description",
            dataobjectfield_string(
            "Construct program for loading other programs"));
    dataobject_setEnum(output, "icon", dataobjectfield_string("appicon"));
    dataobject_setEnum(output, "startupview",
            dataobjectfield_string("mainview"));
    dataobject_setEnum(output, "onLoad",
            dataobjectfield_string("print(\"BootApp: Loaded\")"));
    dataobject_setEnum(output, "onFocus",
            dataobjectfield_string("print(\"BootApp: Focused\")"));
    dataobject_setEnum(output, "onUnFocus",
            dataobjectfield_string("print(\"BootApp: UnFocused\")"));

	dataobject_setEnum(output, "script", dataobjectfield_string(
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
	dataobject_setEnum(view, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	dataobject_setEnum(view, EMO_FIELD_HEIGHT, dataobjectfield_string("220"));
    widget_setPacking(view, WP_VERTICAL);

    root = widget_newTypeIdName("box", "gradbox", NULL, view);
	dataobject_setEnum(root, EMO_FIELD_WIDTH, dataobjectfield_string("100%"));
	dataobject_setEnum(root, EMO_FIELD_HEIGHT, dataobjectfield_string("100%"));
    widget_setPacking(root, WP_VERTICAL);

    dobj1 = widget_newTypeIdName(NULL, NULL, NULL, root);
    dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_string("emobiix"));
	dataobject_setEnum(dobj1, "weight", dataobjectfield_string("bold"));
    widget_setAlignment(dobj1, WA_CENTER);

	dobj1 = widget_newTypeIdName("label", NULL, NULL, root);
    dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_string("Application Boot Environment"));
    widget_setAlignment(dobj1, WA_CENTER);

	box = widget_newTypeIdName("box", NULL, NULL, root);
	widget_setPacking(box, WP_HORIZONTAL);
	widget_setAlignment(box, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);

    dataobject_setEnum(dobj1, EMO_FIELD_ONRETURN, dataobjectfield_string(
/*			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/synctest\");"));*/
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/sample\");"));
	widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);
	/*widget_setPacking(dobj1, WP_VERTICAL);*/

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setEnum(dobj2, EMO_FIELD_DATA, dataobjectfield_string("Hello World"));
    widget_setAlignment(dobj2, WA_CENTER);

	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);
    dataobject_setEnum(dobj1, EMO_FIELD_ONRETURN, dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/calc\");"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setEnum(dobj2, EMO_FIELD_DATA, dataobjectfield_string("Calculator"));
    widget_setAlignment(dobj2, WA_CENTER);

#ifdef SIMULATOR
	durl = url_parse("system://local/gps", URL_ALL);
	dobj1 = dataobject_construct(durl, 1);
	dataobject_setEnum(dobj1, "long", dataobjectfield_string("40.702147"));
	dataobject_setEnum(dobj1, "lat", dataobjectfield_string("-74.015794"));
#endif

	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);
    dataobject_setEnum(dobj1, EMO_FIELD_ONRETURN, dataobjectfield_string(
			"gps = DataObject.locate(\"system://local/gps\");\n"
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/whereami?\" .. gps:getValue(\"long\") .. \",\" .. gps:getValue(\"lat\"));\n"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setEnum(dobj2, EMO_FIELD_DATA, dataobjectfield_string("Where am I?"));
    widget_setAlignment(dobj2, WA_CENTER);


	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);
    dataobject_setEnum(dobj1, EMO_FIELD_ONRETURN, dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/mail\");"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setEnum(dobj2, EMO_FIELD_DATA, dataobjectfield_string("Mail"));
    widget_setAlignment(dobj2, WA_CENTER);

	box = widget_newTypeIdName("box", NULL, NULL, root);
	widget_setPacking(box, WP_HORIZONTAL);
	widget_setAlignment(box, WA_CENTER);

	dobj1 = widget_newTypeIdName("entry", NULL, "lat", box);
	dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("40%"));
	dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_string("40.702147"));
	widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj1 = widget_newTypeIdName("entry", NULL, "long", box);
	dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("40%"));
	dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_string("-74.015794"));
	widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj1 = widget_newTypeIdName("button", NULL, NULL, box);
    dataobject_setEnum(dobj1, EMO_FIELD_ONRETURN, dataobjectfield_string(
			"Llat = DataObject.find(\"lat\");\n"
			"Llong = DataObject.find(\"long\");\n"
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/whereami?\" .. Llat:getValue() .. \",\" .. Llong:getValue());"));
    widget_setAlignment(dobj1, WA_CENTER);
	widget_setCanFocus(dobj1, 1);

	dobj2 = widget_newTypeIdName("label", NULL, NULL, dobj1);
    dataobject_setEnum(dobj2, EMO_FIELD_DATA, dataobjectfield_string("NoGPS"));
    widget_setAlignment(dobj2, WA_CENTER);

#if 0
	dobj1 = widget_newTypeIdName("entry", NULL, "output", root);
	dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("80%"));
	dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_string("tcp://69.114.111.9:12345/"));
    dataobject_setEnum(dobj1, EMO_FIELD_ONRETURN, dataobjectfield_string(
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
	dataobject_setEnum(dobj1, "mime-type", dataobjectfield_string("png"));
	dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_data(mimedata, mimedata_size));
	dataobject_setEnum(dobj1, "transparency", dataobjectfield_string("stencil"));
	dataobject_setEnum(dobj1, "color", dataobjectfield_string("FF00FF00"));
	mime_load(dobj1);

	dobj1 = widget_newTypeIdName("entry", NULL, NULL, root);
	dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("90%"));
	dataobject_setEnum(dobj1, EMO_FIELD_HEIGHT, dataobjectfield_string("52"));
	dataobject_setEnum(dobj1, EMO_FIELD_MULTILINE, dataobjectfield_string("true"));
	widget_setAlignment(dobj1, WA_CENTER);
	dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_string("This is some bold red italic text. "
			"So we put some more text in here because we are trying to test how well a long segment of text "
			"will work since we are trying to implement word line breaks, scrollbar information, basic markup, yknow, all the "
			"stuff you would normally have in a reasonable text engine only for a mobile platform. "
			"Must remember to look into that xadvance issue on the platform itself, probably something with the freetype2 config."));

	dobj1 = widget_newTypeIdName("text", NULL, NULL, root);
	dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("90%"));
	/*dataobject_setEnum(dobj1, EMO_FIELD_HEIGHT, dataobjectfield_string("65"));*/
	widget_setAlignment(dobj1, WA_CENTER);
	dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_string("This <gAAAAFF00>is</g> <u>some</u> <b>bold</b> <cFF000000>red</c> <i>italic</i> text.\n"
			"So we put some more text in here because we are trying to test how well a long segment of text "
			"will work since we are trying to implement word line breaks."));
#endif

	/* array test */
#if 0
	dobj2 = widget_newTypeIdName("label", NULL, NULL, root);
    dataobject_setEnum(dobj2, EMO_FIELD_DATA, dataobjectfield_string("Running Applications"));
	dataobject_setEnum(dobj2, EMO_FIELD_MARGINTOP, dataobjectfield_int(10));
    widget_setAlignment(dobj2, WA_LEFT);

	recordobj = dataobject_new();
	dataobject_setRecordType(recordobj, 1);
	
#define REC(_minor, _str1, _str2) \
	rec = dataobject_new(); \
	dataobject_setStamp(rec, _minor, 0); \
	dataobject_setEnum(rec, "str1", dataobjectfield_string(_str1)); \
	dataobject_setEnum(rec, "str2", dataobjectfield_string(_str2)); \
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
	dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("90%"));
	dataobject_setEnum(dobj1, EMO_FIELD_HEIGHT, dataobjectfield_string("100"));
	dataobject_setEnum(dobj1, "margin", dataobjectfield_int(2));
	widget_setPacking(dobj1, WP_VERTICAL);
	widget_setAlignment(dobj1, WA_CENTER);

	sbox = widget_newTypeIdName("box", "selbox", NULL, dobj1);
	dataobject_setEnum(sbox, EMO_FIELD_WIDTH, dataobjectfield_string("100%"));
	dataobject_setEnum(sbox, "margin", dataobjectfield_int(2));
	widget_setCanFocus(sbox, 1);
	widget_setPacking(sbox, WP_HORIZONTAL);

	label = widget_newTypeIdName("label", NULL, NULL, sbox);
	dataobject_setEnum(label, EMO_FIELD_WIDTH, dataobjectfield_string("10%"));
	dataobject_setEnum(label, EMO_FIELD_DATA, dataobjectfield_string("M"));
	widget_setAlignment(label, WA_CENTER);

	label = widget_newTypeIdName("label", NULL, NULL, sbox);
	dataobject_setEnum(label, "datafield", dataobjectfield_string("str2"));
	dataobject_setEnum(label, "arraysource", dataobjectfield_string("true"));
	dataobject_setEnum(label, EMO_FIELD_WIDTH, dataobjectfield_string("40%"));

	label = widget_newTypeIdName("label", NULL, NULL, sbox);
	dataobject_setEnum(label, "datafield", dataobjectfield_string("str1"));
	dataobject_setEnum(label, "arraysource", dataobjectfield_string("true"));
	dataobject_setEnum(label, EMO_FIELD_WIDTH, dataobjectfield_string("40%"));
#endif

    return output;
}
#endif
