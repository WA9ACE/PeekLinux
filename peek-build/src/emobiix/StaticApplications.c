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
				dataobject_setEnum(output, EMO_FIELD_TYPE, dataobjectfield_string("application"));
				dataobject_setEnum(output, EMO_FIELD_NAME, dataobjectfield_string("Load Screen"));
				dataobject_setEnum(output, EMO_FIELD_DESCRIPTION, dataobjectfield_string("load mobile"));
				dataobject_setEnum(output, EMO_FIELD_STARTUPVIEW, dataobjectfield_string("mainview"));
				dataobject_setEnum(output, EMO_FIELD_FULLSCREEN, dataobjectfield_string("true"));

				root = widget_newTypeIdName("view", NULL, "mainview", output);
				widget_setPacking(root, WP_VERTICAL);

				dobj1 = widget_newTypeIdName("box", "gradbox", NULL, root);
				dataobject_setEnum(dobj1, EMO_FIELD_WIDTH, dataobjectfield_string("320"));
				dataobject_setEnum(dobj1, EMO_FIELD_HEIGHT, dataobjectfield_string("240"));
				dataobject_setEnum(dobj1, EMO_FIELD_ALIGNMENT, dataobjectfield_string("center"));
				widget_setPacking(dobj1, WP_VERTICAL);

				setw = widget_newTypeIdName("label", NULL, NULL, dobj1);
				dataobject_setEnum(setw, EMO_FIELD_DATA, dataobjectfield_string("Downloading Application"));
				dataobject_setEnum(setw, EMO_FIELD_ALIGNMENT, dataobjectfield_string("center"));
				//        dobj1 = dobjFromFile("loading.png", root);
				//        widget_setPacking(dobj1, WP_VERTICAL);

				return output;
}

DataObject *LockApplication(void)
{
	static DataObject *output = NULL;
	DataObject *dobj1, *root;

	if (output != NULL)
        return output;

    output = dataobject_new();
    dataobject_setEnum(output, EMO_FIELD_TYPE, dataobjectfield_string("application"));
    dataobject_setEnum(output, EMO_FIELD_NAME, dataobjectfield_string("Lock Screen"));
    dataobject_setEnum(output, EMO_FIELD_DESCRIPTION,
            dataobjectfield_string("Lock screen for mobile"));
    dataobject_setEnum(output, EMO_FIELD_STARTUPVIEW,
            dataobjectfield_string("mainview"));
	dataobject_setEnum(output, EMO_FIELD_FULLSCREEN,
            dataobjectfield_string("true"));
 
	root = widget_newTypeIdName("view", NULL, "mainview", output);
	widget_setPacking(root, WP_VERTICAL);
	dobj1 = dobjFromFile("background.png", root);
	widget_setPacking(dobj1, WP_VERTICAL);
	/*
	setw = widget_newTypeIdName("label", "biglabelw", NULL, dobj1);
	dataobject_setValue(setw, "data", dataobjectfield_string("1:15p"));
	dataobject_setValue(setw, "weight", dataobjectfield_string("bold"));
	dataobject_setValue(setw, "marginleft", dataobjectfield_string("20"));
	dataobject_setValue(setw, "margintop", dataobjectfield_string("15"));

	setw = widget_newTypeIdName("label", "labelw", NULL, dobj1);
	dataobject_setValue(setw, "data", dataobjectfield_string(hw_td_get_clock_str()));
	dataobject_setValue(setw, "weight", dataobjectfield_string("bold"));
	dataobject_setValue(setw, "marginleft", dataobjectfield_string("20"));
	dataobject_setValue(setw, "margintop", dataobjectfield_string("4"));
	*/

	return output;
}
