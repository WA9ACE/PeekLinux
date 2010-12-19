#include "Application.h"

#include "DataObject.h"
#include "Debug.h"
#include "Widget.h"
#include "Style.h"
#include "ApplicationManager.h"
#include "Script.h"

#include "p_malloc.h"

#define APP_NONE        0x00
#define APP_ICONIFIED   0x01

struct Application_t {
	DataObject *dobj;
	DataObject *currentScreen;
	char *activeDialog, *activeMenu;
	Style *currentStyle;
    int flags;
	URL *url;
};

Application *application_load(DataObject *dobj)
{
	Application *output;
	DataObjectField *field;


	EMO_ASSERT_NULL(dobj != NULL, "Loading application from NULL object")

	output = (Application *)p_malloc(sizeof(Application));
    if (output == NULL)
        return NULL;
	output->dobj = dobj;
	output->currentScreen = NULL;
	output->activeDialog = NULL;
	output->activeMenu = NULL;
    output->flags = 0;
	dataobject_resolveReferences(dobj);
	
	field = dataobject_getEnum(dobj, EMO_FIELD_ICONIFIED);
    if (dataobjectfield_isTrue(field))
        output->flags |= APP_ICONIFIED;
	
	output->currentStyle = NULL;
	field = dataobject_getEnum(dobj, EMO_FIELD_STYLE);
	if (field != NULL && field->type == DOF_STRING)
		output->currentStyle = dataobject_findByName(dobj, field->field.string);

	field = dataobject_getEnum(dobj, EMO_FIELD_STARTUPVIEW);
	if (field != NULL && field->type == DOF_STRING)
		output->currentScreen = dataobject_findByName(dobj, field->field.string);
	if (output->currentScreen != NULL)
		widget_markDirty(output->currentScreen);
	else
		emo_printf("No startupview for applicaton" NL);

	script_event(dobj, EMO_FIELD_ONLOAD);

	return output;
}

void application_setURL(Application *app, const URL *url)
{
	EMO_ASSERT(url != NULL, "Setting application url to NULL")
	EMO_ASSERT(app != NULL, "Setting application url on NULL Application")

	app->url = url_parse(url->all, URL_ALL);
}

const URL *application_getURL(Application *app)
{
	EMO_ASSERT_NULL(app != NULL, "Getting application url on NULL Application")

	return app->url;
}

void application_setActive(Application *app)
{

}

DataObject *application_getCurrentScreen(Application *app)
{
	EMO_ASSERT_NULL(app != NULL,
			"Getting application current screen on NULL Application")
	return app->currentScreen;
}

void application_setCurrentScreen(Application *app, DataObject *screen)
{
	DataObjectField *field;
	const char *fieldValue;

	EMO_ASSERT(screen != NULL,
			"Setting application current screen to NULL")
	EMO_ASSERT(app != NULL,
			"Setting application current screen on NULL Application")

	field = dataobject_getEnum(screen, EMO_FIELD_TYPE);
	if (!dataobjectfield_isString(field, "view")) {
		if (field != NULL && field->type == DOF_STRING)
			fieldValue = field->field.string;
		else
			fieldValue = "nil";
		emo_printf("Can't set a %s as a current screen" NL, fieldValue);
		return;
	}
	app->currentScreen = screen;
	/*widget_markDirty(screen);*/
	/*manager_focusView(screen);*/
	manager_focusApplication(app);
}

void *application_getCurrentStyle(Application *app)
{
	EMO_ASSERT_NULL(app != NULL,
			"Getting application current style on NULL Application")

	return app->currentStyle;
}

int application_isIconified(Application *app)
{
	EMO_ASSERT_INT(app != NULL, 0,
			"Getting application iconified flag on NULL Application")

    return app->flags & APP_ICONIFIED;
}

DataObject *application_getDataObject(Application *app)
{
	EMO_ASSERT_NULL(app != NULL,
			"Getting application DataObject on NULL Application")
		
	return app->dobj;
}

void application_showDialog(Application *app, const char *name)
{
	EMO_ASSERT(app != NULL,
			"Application Show Dialog on NULL Application")
	EMO_ASSERT(name != NULL,
			"Application Show Dialog on NULL name")

	if (app->activeDialog != NULL)
		p_free(app->activeDialog);
	app->activeDialog = p_strdup(name);
	if (app == manager_getFocusedApplication())
		manager_showDialog(name);
}

void application_finishDialog(Application *app)
{
	if (app->activeDialog != NULL) {
		p_free(app->activeDialog);
		app->activeDialog = NULL;
	}
	if (app == manager_getFocusedApplication())
		manager_finishDialog();
}

void application_showMenu(Application *app, DataObject *dobj)
{

}

void application_finishMenu(Application *app)
{

}

