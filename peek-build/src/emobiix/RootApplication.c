#include "RootApplication.h"

#include "DataObject.h"
#include "Widget.h"
#include "File.h"
#include "Mime.h"
#include "Color.h"
#include "Gradient.h"
#include "Debug.h"

#include "p_malloc.h"
#include "system_battery.h"

#include <stdio.h>

#ifdef SIMULATOR
#define BARHEIGHT "16"
#define CONTENTHEIGHT "224"
#else
#define BARHEIGHT "25"
#define CONTENTHEIGHT "215"
#endif

DataObject *dobjFromFile(const char *filename, DataObject *root)
{
	DataObject *dobj1;
	File *mimefile;
	int mimedata_size;
	void *mimedata;

	EMO_ASSERT_NULL(filename != NULL, "load obj from file missing filename")
	EMO_ASSERT_NULL(root != NULL, "load obj from file missing root")

	mimefile = file_openRead(filename);
	dobj1 = widget_newTypeIdName("image", NULL, NULL, root);
	if (mimefile != NULL) {
		mimedata_size = file_size(mimefile);
		mimedata = (char *)p_malloc(mimedata_size);
		file_read(mimefile, mimedata_size, mimedata);
		file_close(mimefile);
		dataobject_setEnum(dobj1, EMO_FIELD_MIMETYPE, dataobjectfield_string("png"));
		dataobject_setEnum(dobj1, EMO_FIELD_DATA, dataobjectfield_data(mimedata, mimedata_size));
		mime_load(dobj1);
	}

	return dobj1;
}

DataObject *RootApplication(void)
{
    static DataObject *output = NULL;
	DataObject *dobj1, *root, *dobj2, *frame;
	DataObject *setw, *setiw, *setc, *settd;
	DataObject *signalstack, *gprsstack, *emostack;
	DataObject *batterystack;

    if (output != NULL)
        return output;

    output = dataobject_new();
    dataobject_setEnum(output, EMO_FIELD_TYPE, dataobjectfield_string("application"));
    dataobject_setEnum(output, EMO_FIELD_NAME, dataobjectfield_string("Boot Manager"));
    dataobject_setEnum(output, EMO_FIELD_DESCRIPTION, dataobjectfield_string( "Construct program for loading other programs"));
    dataobject_setEnum(output, EMO_FIELD_ICON, dataobjectfield_string("appicon"));
    dataobject_setEnum(output, EMO_FIELD_STARTUPVIEW, dataobjectfield_string("rootview"));

	root = widget_newTypeIdName("view", NULL, "rootview", output);
	widget_setPacking(root, WP_VERTICAL);

	dobj1 = widget_newTypeIdName("box", "darkgradbox", "topbar", root);
	dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	dataobject_setEnum(dobj1, EMO_FIELD_HEIGHT, dataobjectfield_string(BARHEIGHT));
	widget_setPacking(dobj1, WP_HORIZONTAL);

	/* Signal set */
	signalstack = widget_newTypeIdName("stack", NULL, NULL, dobj1);

	setw = widget_newTypeIdName("set", NULL, NULL, signalstack);
	dataobject_setEnum(setw, EMO_FIELD_FIELDNAME, dataobjectfield_string("signal-level"));
	dataobject_setEnum(setw, EMO_FIELD_REFERENCE, dataobjectfield_string("system://local/gprs"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(0));
	setc = dobjFromFile("signal0.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(1));
	setc = dobjFromFile("signal1.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(2));
	setc = dobjFromFile("signal2.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(3));
	setc = dobjFromFile("signal3.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(4));
	setc = dobjFromFile("signal4.png", setiw);

	/* GPRS set */
	gprsstack = widget_newTypeIdName("stack", NULL, NULL, dobj1);

	setw = widget_newTypeIdName("set", NULL, NULL, gprsstack);
	dataobject_setEnum(setw, EMO_FIELD_FIELDNAME, dataobjectfield_string("gprs-on"));
	dataobject_setEnum(setw, EMO_FIELD_REFERENCE, dataobjectfield_string("system://local/gprs"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(0));
	setc = dobjFromFile("edge_off.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(1));
	setc = dobjFromFile("edge_on.png", setiw);

	/* emobiix indicator */
	emostack = widget_newTypeIdName("stack", NULL, NULL, dobj1);

	setw = widget_newTypeIdName("set", NULL, NULL, emostack);
	dataobject_setEnum(setw, EMO_FIELD_MARGINTOP, dataobjectfield_string("6"));
	dataobject_setEnum(setw, EMO_FIELD_FIELDNAME, dataobjectfield_string("emobiix-on"));
	dataobject_setEnum(setw, EMO_FIELD_REFERENCE, dataobjectfield_string("system://local/gprs"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(0));
	setc = dobjFromFile("emo_off.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(1));
	setc = dobjFromFile("emo_on.png", setiw);

	/* Time */
	setw = widget_newTypeIdName("box", NULL, NULL, dobj1);
	widget_setPacking(setw, WP_VERTICAL);
	dataobject_setEnum(setw, EMO_FIELD_WIDTH, dataobjectfield_string("200"));

	settd = widget_newTypeIdName("label", NULL, NULL, setw);
	dataobject_setEnum(settd, EMO_FIELD_REFERENCE, dataobjectfield_string("system://local/time"));
	dataobject_setEnum(settd, EMO_FIELD_WEIGHT, dataobjectfield_string("bold"));
	dataobject_setEnum(settd, EMO_FIELD_FONT_COLOR, dataobjectfield_string("#FFFFFF00"));
	dataobject_setEnum(settd, EMO_FIELD_ALIGNMENT, dataobjectfield_string("center"));
	dataobject_setEnum(settd, EMO_FIELD_MARGINTOP, dataobjectfield_string("4"));

#if 0
	/* Weather set */
	setw = widget_newTypeIdName("set", NULL, NULL, dobj1);
	dataobject_setEnum(setw, EMO_FIELD_FIELDNAME, dataobjectfield_string("data"));
	/*dataobject_setEnum(setw, EMO_FIELD_MARGINTOP, dataobjectfield_string("3"));
	dataobject_setEnum(setw, "marginleft", dataobjectfield_string("3"));*/
	widget_setDataObject(setw, testweather);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_string("1"));
	setc = dobjFromFile("wi-cloud.png", setiw);
	dataobject_setEnum(setc, "color", dataobjectfield_string("00000000"));
	dataobject_setEnum(setc, EMO_FIELD_MARGINTOP, dataobjectfield_string("3"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_string("2"));
	setc = dobjFromFile("wi-rain.png", setiw);
	dataobject_setEnum(setc, "color", dataobjectfield_string("00000000"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_string("3"));
	setc = dobjFromFile("wi-storm.png", setiw);
	dataobject_setEnum(setc, "color", dataobjectfield_string("00000000"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_string("4"));
	setc = dobjFromFile("wi-snow.png", setiw);
	dataobject_setEnum(setc, "color", dataobjectfield_string("00000000"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	/*dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_string("5"));*/
	setc = dobjFromFile("wi-sun.png", setiw);
	dataobject_setEnum(setc, "color", dataobjectfield_string("00000000"));

	setw = widget_newTypeIdName(EMO_FIELD_LABEL, NULL, NULL, dobj1);
	dataobject_setEnum(setw, "data", dataobjectfield_string("73F"));
	dataobject_setEnum(setw, EMO_FIELD_MARGINTOP, dataobjectfield_string("1"));

	/* Spkr set */
	setw = widget_newTypeIdName("set", NULL, NULL, dobj1);
	dataobject_setEnum(setw, EMO_FIELD_FIELDNAME, dataobjectfield_string("data"));
	dataobject_setEnum(setw, "margin", dataobjectfield_string("2"));
	/*dataobject_setEnum(setw, "marginleft", dataobjectfield_string("3"));*/
	widget_setDataObject(setw, testspkr);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_string("1"));
	setc = dobjFromFile("spkr-on.png", setiw);
	dataobject_setEnum(setc, "color", dataobjectfield_string("00000000"));
	
	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	setc = dobjFromFile("spkr-off.png", setiw);
	dataobject_setEnum(setc, "color", dataobjectfield_string("00000000"));
#endif

	/* battery indicator */
	batterystack = widget_newTypeIdName("stack", NULL, NULL, dobj1);

	setw = widget_newTypeIdName("set", NULL, NULL, batterystack);
	dataobject_setEnum(setw, EMO_FIELD_MARGINLEFT, dataobjectfield_string("33"));
	dataobject_setEnum(setw, EMO_FIELD_FIELDNAME, dataobjectfield_string("charge-level"));
	dataobject_setEnum(setw, EMO_FIELD_REFERENCE, dataobjectfield_string("system://local/battery"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(0));
	setc = dobjFromFile("battery0.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(1));
	setc = dobjFromFile("battery1.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(2));
	setc = dobjFromFile("battery2.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(3));
	setc = dobjFromFile("battery3.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(4));
	setc = dobjFromFile("battery4.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(5));
	setc = dobjFromFile("battery5.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_uint(6));
	setc = dobjFromFile("battery6.png", setiw);

#if 0
	setiw = widget_newTypeIdName("box", NULL, NULL, setw);

	setw = widget_newTypeIdName("set", NULL, NULL, batterystack);
	dataobject_setEnum(setw, EMO_FIELD_FIELDNAME, dataobjectfield_string("charging"));
	dataobject_setEnum(setw, EMO_FIELD_MARGINTOP, dataobjectfield_string("3"));
	widget_setDataObject(setw, testcharge);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setEnum(setiw, EMO_FIELD_FIELDVALUE, dataobjectfield_string("1"));
	setc = dobjFromFile("batterycharge.png", setiw);

	setiw = widget_newTypeIdName("box", NULL, NULL, setw);
#endif

	/* application box */
	dobj1 = widget_newTypeIdName("box", NULL, NULL, root);
	dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	dataobject_setEnum(dobj1, EMO_FIELD_HEIGHT, dataobjectfield_string(CONTENTHEIGHT));
	widget_setPacking(dobj1, WP_VERTICAL);

	dobj2 = widget_newTypeIdName("stack", NULL, NULL, dobj1);
	dataobject_setEnum(dobj2, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	dataobject_setEnum(dobj2, EMO_FIELD_HEIGHT, dataobjectfield_string(CONTENTHEIGHT));
	widget_setPacking(dobj2, WP_VERTICAL);

	frame = widget_newTypeIdName("frame", NULL, "placeholder", dobj2);
	dataobject_setEnum(frame, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	dataobject_setEnum(frame, EMO_FIELD_HEIGHT, dataobjectfield_string(CONTENTHEIGHT));
	widget_setPacking(frame, WP_VERTICAL);

	frame = widget_newTypeIdName("box", NULL, "menuholder", dobj2);
	dataobject_setEnum(frame, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	dataobject_setEnum(frame, EMO_FIELD_HEIGHT, dataobjectfield_string(CONTENTHEIGHT));
	widget_setPacking(frame, WP_VERTICAL);

	frame = widget_newTypeIdName("reference", NULL, "dialogholder", dobj2);
	dataobject_setEnum(frame, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
	dataobject_setEnum(frame, EMO_FIELD_HEIGHT, dataobjectfield_string(CONTENTHEIGHT));
	widget_setPacking(frame, WP_VERTICAL);

    return output;
}
