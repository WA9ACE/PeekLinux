#include "Application.h"

#include "DataObject.h"
#include "Debug.h"
#include "Widget.h"

#include "p_malloc.h"

struct Application_t {
	DataObject *dobj;
	DataObject *currentScreen;
};

Application *application_load(DataObject *dobj)
{
	Application *output;
	DataObjectField *field;

	output = (Application *)p_malloc(sizeof(Application));
	output->dobj = dobj;
	output->currentScreen = NULL;
	dataobject_resolveReferences(dobj);
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
	return app->currentScreen;
}

