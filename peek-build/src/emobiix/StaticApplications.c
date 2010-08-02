#include "BootApplication.h"

#include "DataObject.h"
#include "Widget.h"
#include "URL.h"

#include <stdio.h>

#include "p_malloc.h"

#include "Mime.h"

/* in BootApplication.c */
extern DataObject *dobjFromFile(const char *filename, DataObject *root);

DataObject *LoadingApplication(void)
{
        static DataObject *output = NULL;
        DataObject *dobj1, *root, *setw;

        if (output != NULL)
        return output;

				output = dataobject_new();
				dataobject_setValue(output, "type", dataobjectfield_string("application"));
				dataobject_setValue(output, "name", dataobjectfield_string("Load Screen"));
				dataobject_setValue(output, "description", dataobjectfield_string("load mobile"));
				dataobject_setValue(output, "startupview", dataobjectfield_string("mainview"));
				dataobject_setValue(output, "fullscreen", dataobjectfield_string("true"));

				root = widget_newTypeIdName("view", NULL, "mainview", output);
				widget_setPacking(root, WP_VERTICAL);

				dobj1 = widget_newTypeIdName("box", "gradboxdark", NULL, root);
				dataobject_setValue(dobj1, "width", dataobjectfield_string("320"));
				dataobject_setValue(dobj1, "height", dataobjectfield_string("240"));
				dataobject_setValue(dobj1, "alignment", dataobjectfield_string("center"));
				widget_setPacking(dobj1, WP_VERTICAL);

				setw = widget_newTypeIdName("label", "biglabelw", NULL, dobj1);
				dataobject_setValue(setw, "data", dataobjectfield_string("Downloading Application"));
				dataobject_setValue(setw, "alignment", dataobjectfield_string("center"));
				//        dobj1 = dobjFromFile("loading.png", root);
				//        widget_setPacking(dobj1, WP_VERTICAL);

				return output;
}

DataObject *LockApplication(void)
{
	static DataObject *output = NULL;
	DataObject *dobj1, *root, *setw;

	if (output != NULL)
        return output;

    output = dataobject_new();
    dataobject_setValue(output, "type", dataobjectfield_string("application"));
    dataobject_setValue(output, "name", dataobjectfield_string("Lock Screen"));
    dataobject_setValue(output, "description",
            dataobjectfield_string("Lock screen for mobile"));
    dataobject_setValue(output, "startupview",
            dataobjectfield_string("mainview"));
	dataobject_setValue(output, "fullscreen",
            dataobjectfield_string("true"));
 
	root = widget_newTypeIdName("view", NULL, "mainview", output);
	widget_setPacking(root, WP_VERTICAL);

	dobj1 = dobjFromFile("street64.png", root);
	widget_setPacking(dobj1, WP_VERTICAL);
	/*
	setw = widget_newTypeIdName("label", "biglabelw", NULL, dobj1);
	dataobject_setValue(setw, "data", dataobjectfield_string("1:15p"));
	dataobject_setValue(setw, "weight", dataobjectfield_string("bold"));
	dataobject_setValue(setw, "marginleft", dataobjectfield_string("20"));
	dataobject_setValue(setw, "margintop", dataobjectfield_string("15"));

	setw = widget_newTypeIdName("label", "labelw", NULL, dobj1);
	dataobject_setValue(setw, "data", dataobjectfield_string("Friday, Dec 17 2009"));
	dataobject_setValue(setw, "weight", dataobjectfield_string("bold"));
	dataobject_setValue(setw, "marginleft", dataobjectfield_string("20"));
	dataobject_setValue(setw, "margintop", dataobjectfield_string("4"));
	*/

	return output;
}
