#include "Application.h"

#include "DataObject.h"
#include "Debug.h"
#include "Widget.h"
#include "Style.h"
#include "ApplicationManager.h"

#include "p_malloc.h"

#define APP_NONE        0x00
#define APP_ICONIFIED   0x01

struct Application_t {
	DataObject *dobj;
	DataObject *currentScreen;
	Style *currentStyle;
    int flags;
};

Application *application_load(DataObject *dobj)
{
	Application *output;
	DataObjectField *field;

	output = (Application *)p_malloc(sizeof(Application));
    if (output == NULL)
        return NULL;
	output->dobj = dobj;
	output->currentScreen = NULL;
    output->flags = 0;
	dataobject_resolveReferences(dobj);
	
	field = dataobject_getValue(dobj, "iconified");
    if (dataobjectfield_isTrue(field))
        output->flags |= APP_ICONIFIED;
	
	output->currentStyle = NULL;
	field = dataobject_getValue(dobj, "style");
	if (field != NULL && field->type == DOF_STRING)
		output->currentStyle = dataobject_findByName(dobj, field->field.string);

	field = dataobject_getValue(dobj, "startupview");
	if (field != NULL && field->type == DOF_STRING)
		output->currentScreen = dataobject_findByName(dobj, field->field.string);
	if (output->currentScreen != NULL)
		widget_markDirty(output->currentScreen);
	else
		emo_printf("No startupview for applicaton" NL);

	return output;
}

void application_setActive(Application *app)
{

}

DataObject *application_getCurrentScreen(Application *app)
{
	if(!app)
		return NULL;
	return app->currentScreen;
}

void application_setCurrentScreen(Application *app, DataObject *screen)
{
	app->currentScreen = screen;
	/*widget_markDirty(screen);*/
	manager_focusApplication(app);
}

void *application_getCurrentStyle(Application *app)
{
	return app->currentStyle;
}

int application_isIconified(Application *app)
{
    return app->flags & APP_ICONIFIED;
}

DataObject *application_getDataObject(Application *app)
{
        return app->dobj;
}
