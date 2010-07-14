#include "RootApplication.h"

#include "DataObject.h"
#include "Widget.h"
#include "File.h"
#include "Mime.h"

#include "p_malloc.h"

#include <stdio.h>

static DataObject *dobjFromFile(const char *filename, DataObject *root)
{
	DataObject *dobj1;
	File *mimefile;
	int mimedata_size;
	void *mimedata;

	mimefile = file_openRead(filename);
	dobj1 = widget_newTypeIdName("image", "image", NULL, root);
	if (mimefile != NULL) {
		mimedata_size = file_size(mimefile);
		mimedata = (char *)p_malloc(mimedata_size);
		file_read(mimefile, mimedata_size, mimedata);
		file_close(mimefile);
		dataobject_setValue(dobj1, "mime-type", dataobjectfield_string("png"));
		dataobject_setValue(dobj1, "src", dataobjectfield_data(mimedata, mimedata_size));
		mime_load(dobj1);
	}

	return dobj1;
}

DataObject *RootApplication(void)
{
    static DataObject *output = NULL;
	DataObject *dobj1, *root, *dobj2;
	DataObject *setw, *setiw, *setc, *testvalue;
	DataObject *signalstack, *testgprs;
	DataObject *batterystack, *testbattery, *testcharge;
	DataObject *testweather;

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
	dataobject_setValue(dobj1, "height", dataobjectfield_string("16"));
	widget_setPacking(dobj1, WP_HORIZONTAL);

	testvalue = dataobject_new();
	dataobject_setValue(testvalue, "data", dataobjectfield_string("5"));

	testgprs = dataobject_new();
	dataobject_setValue(testgprs, "data", dataobjectfield_string("1"));

	testbattery = dataobject_new();
	dataobject_setValue(testbattery, "data", dataobjectfield_string("4"));
	testcharge = dataobject_new();
	dataobject_setValue(testcharge, "data", dataobjectfield_string("1"));

	testweather = dataobject_new();
	dataobject_setValue(testweather, "data", dataobjectfield_string("1"));

	signalstack = widget_newTypeIdName("stack", NULL, NULL, dobj1);

	/* GPRS set */
	setw = widget_newTypeIdName("set", NULL, NULL, signalstack);
	dataobject_setValue(setw, "fieldname", dataobjectfield_string("data"));
	dataobject_setValue(setw, "margintop", dataobjectfield_string("3"));
	dataobject_setValue(setw, "marginleft", dataobjectfield_string("3"));
	widget_setDataObject(setw, testgprs);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("1"));
	setc = dobjFromFile("gprs.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("00000000"));

	setiw = widget_newTypeIdName("box", NULL, NULL, setw);
	
	/* Signal set */
	setw = widget_newTypeIdName("set", NULL, NULL, signalstack);
	dataobject_setValue(setw, "fieldname", dataobjectfield_string("data"));
	dataobject_setValue(setw, "margintop", dataobjectfield_string("3"));
	dataobject_setValue(setw, "marginleft", dataobjectfield_string("3"));
	widget_setDataObject(setw, testvalue);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("1"));
	setc = dobjFromFile("signal1.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("0000FF00"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("2"));
	setc = dobjFromFile("signal2.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("0000FF00"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("3"));
	setc = dobjFromFile("signal3.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("0000FF00"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("4"));
	setc = dobjFromFile("signal4.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("0000FF00"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("5"));
	setc = dobjFromFile("signal5.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("0000FF00"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	/*dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("0"));*/
	setc = dobjFromFile("signal0.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("FF555500"));

	batterystack = widget_newTypeIdName("stack", NULL, NULL, dobj1);
	setc = dobjFromFile("batterycase.png", batterystack);
	dataobject_setValue(setc, "margintop", dataobjectfield_string("3"));

	/* Weather set */
	setw = widget_newTypeIdName("set", NULL, NULL, signalstack);
	dataobject_setValue(setw, "fieldname", dataobjectfield_string("data"));
	/*dataobject_setValue(setw, "margintop", dataobjectfield_string("3"));
	dataobject_setValue(setw, "marginleft", dataobjectfield_string("3"));*/
	widget_setDataObject(setw, testweather);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("1"));
	setc = dobjFromFile("wi-cloud.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("00000000"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("2"));
	setc = dobjFromFile("wi-rain.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("00000000"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("3"));
	setc = dobjFromFile("wi-storm.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("00000000"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("4"));
	setc = dobjFromFile("wi-snow.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("00000000"));

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	setc = dobjFromFile("wi-sun.png", setiw);
	dataobject_setValue(setc, "transparency", dataobjectfield_string("stencil"));
	dataobject_setValue(setc, "color", dataobjectfield_string("00000000"));

	/* battery indicator */
	setw = widget_newTypeIdName("set", NULL, NULL, batterystack);
	dataobject_setValue(setw, "fieldname", dataobjectfield_string("data"));
	dataobject_setValue(setw, "margintop", dataobjectfield_string("3"));
	widget_setDataObject(setw, testbattery);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("1"));
	setc = dobjFromFile("battery1.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("2"));
	setc = dobjFromFile("battery2.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("2"));
	setc = dobjFromFile("battery2.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("3"));
	setc = dobjFromFile("battery3.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("4"));
	setc = dobjFromFile("battery4.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("5"));
	setc = dobjFromFile("battery5.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("6"));
	setc = dobjFromFile("battery6.png", setiw);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("7"));
	setc = dobjFromFile("batterycharge.png", setiw);

	setiw = widget_newTypeIdName("box", NULL, NULL, setw);

	setw = widget_newTypeIdName("set", NULL, NULL, batterystack);
	dataobject_setValue(setw, "fieldname", dataobjectfield_string("data"));
	dataobject_setValue(setw, "margintop", dataobjectfield_string("3"));
	widget_setDataObject(setw, testcharge);

	setiw = widget_newTypeIdName("setitem", NULL, NULL, setw);
	dataobject_setValue(setiw, "fieldvalue", dataobjectfield_string("1"));
	setc = dobjFromFile("batterycharge.png", setiw);

	setiw = widget_newTypeIdName("box", NULL, NULL, setw);

	/* application box */
	dobj1 = widget_newTypeIdName("box", NULL, NULL, root);
	dataobject_setValue(dobj1, "width", dataobjectfield_string("320"));
	dataobject_setValue(dobj1, "height", dataobjectfield_string("220"));
	widget_setPacking(dobj1, WP_VERTICAL);

	dobj2 = widget_newTypeIdName("stack", NULL, "placeholder", dobj1);
	dataobject_setValue(dobj2, "width", dataobjectfield_string("320"));
	dataobject_setValue(dobj2, "height", dataobjectfield_string("224"));
	widget_setPacking(dobj2, WP_VERTICAL);

	widget_newTypeIdName("box", NULL, NULL, dobj2);

    return output;
}
