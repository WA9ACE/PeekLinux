#include "ApplicationManager.h"

#include "Application.h"
#include "List.h"
#include "lgui.h"
#include "Style.h"
#include "BootApplication.h"
#include "RootApplication.h"
#include "StaticApplications.h"
#include "DataObject.h"
#include "Widget.h"
#include "RootStyle.h"
#include "Debug.h"
#include "EntryWidget.h"
#include "CheckBoxWidget.h"
#include "Script.h"
#include "Hardware.h"
#include "Mime.h"

#include "p_malloc.h"

#include "KeyMappings.h"

#include <string.h>

struct ApplicationManager_t {
	List *applications;
	List *allApplications;
	Application *focus;
	Application *rootApplication;
	Application *bootApp;
	DataObject *rootApplicationObj;
	DataObject *rootApplicationWindow;
	DataObject *rootApplicationPlaceHolder;
	DataObject *rootApplicationMenuHolder;
	DataObject *rootApplicationDialogHolder;
	Style *style;

	DataObject *loadingApplication;
	int loadingApplicationFocus;
	URL *loadingApplicationURL;
};
typedef struct ApplicationManager_t ApplicationManager;

static ApplicationManager *appManager = NULL;

Application *manager_getBootApp(void) {
	if(appManager->bootApp)
		return appManager->bootApp;
	return NULL;
}

void manager_init(void)
{
    Application *bootApp;

	appManager = (ApplicationManager *)p_malloc(sizeof(ApplicationManager));
    if (appManager == NULL) {
        emo_printf("Failed to allocate appManager" NL);
        return;
    }
	appManager->applications = list_new();
    if (appManager->applications == NULL) {
        emo_printf("Failed to allocate applications list" NL);
        return;
    }
	appManager->focus = NULL;
	appManager->loadingApplication = NULL;

	appManager->style = RootStyle();
    if (appManager->style == NULL) {
        emo_printf("Failed to create root style" NL);
        return;
    }

	HardwareInit();

	appManager->rootApplicationObj = RootApplication();
	appManager->rootApplication = application_load(appManager->rootApplicationObj);
	if (appManager->rootApplication == NULL) {
		emo_printf("Fatal - Root application failed to load" NL);
		return;
	}
	appManager->rootApplicationWindow =
			application_getCurrentScreen(appManager->rootApplication);
	appManager->rootApplicationPlaceHolder =
			dataobject_findByName(appManager->rootApplicationWindow,
			"placeholder");
	appManager->rootApplicationMenuHolder =
			dataobject_findByName(appManager->rootApplicationWindow,
			"menuholder");
	appManager->rootApplicationDialogHolder =
			dataobject_findByName(appManager->rootApplicationWindow,
			"dialogholder");

    bootApp = application_load(BootApplication());
	/*bootApp = application_load(LockApplication());*/
    	manager_launchApplication(bootApp);

	manager_focusApplication(bootApp);
	appManager->bootApp = bootApp;
}

void manager_drawScreen(void)
{
	DataObject *toload;

	toload = appManager->loadingApplication;
	if (toload != NULL) {
		/*dataobject_debugPrint(toload);*/
		appManager->loadingApplication = NULL;
		manager_loadApplicationReal(toload, 1, appManager->loadingApplicationFocus,
				appManager->loadingApplicationURL);
	}

	lgui_clip_identity();
	manager_drawScreenPartial();
}

void manager_drawScreenPartial(void)
{
	DataObject *view;

#ifndef SIMULATOR
    emo_printf("manager_drawScreenPartial()0");
#endif

	if (appManager->rootApplication == NULL)
		return;

	view = application_getCurrentScreen(appManager->rootApplication);
#ifndef SIMULATOR
	emo_printf("manager_drawScreenPartial()1");
#endif
	if (view != NULL) {
		style_renderWidgetTree(appManager->style, view);
	} else {
		emo_printf("no view to draw" NL);
	}
}

void manager_resolveLayout(void)
{
	Style *style;
	style = application_getCurrentStyle(appManager->focus);
	if (style == NULL)
		style = appManager->style;
	widget_resolveLayoutRoot(appManager->rootApplicationWindow,
			style, 0);
}

void manager_handleKey(unsigned int key)
{
	int isDirty = 0;
	Widget *focus, *accessKey;
	static Rectangle clip = {0, 0, 320, 240};
	char keyStr[2];
	Style *style;

	emo_printf("manager_handleKey() key=%d", key);

	if (key == EKEY_ALTTAB) { /* lock symbol */
		manager_focusNextApplication();
		return;
	}

	style = application_getCurrentStyle(appManager->focus);
	if (style == NULL)
		style = appManager->style;

	focus = NULL;
	if (appManager->rootApplicationWindow != NULL)
		focus = widget_focusWhichOneNF(appManager->rootApplicationWindow);
	if (focus != NULL) {
		DataObjectField *field;
		field = dataobject_getValue(focus, "type");
		if (dataobjectfield_isString(field, "entry")) {
			if (entryWidget_handleKey(focus, key, style))
				return;
		} else if (dataobjectfield_isString(field, "checkbox")) {
			if (checkboxWidget_handleKey(focus, key, style))
				return;
		}
	}

	/* access key */
	((unsigned char *)keyStr)[0] = (unsigned char)key;
	keyStr[1] = 0;
	accessKey = widget_findStringField(appManager->rootApplicationWindow, "accesskey", keyStr);
	if (accessKey != NULL) {
		widget_forceFocus(appManager->rootApplicationWindow, accessKey, style);
		script_event(accessKey, "onreturn");
		return;
	}

	switch (key) {
		case EKEY_ACTIVATE:
			script_event(focus != NULL ? focus : appManager->rootApplicationWindow, "onreturn");
			break;
		case EKEY_FOCUSPREV: // up on scroll
			/*case 103:*/ /* GLUT a */
			widget_focusPrev(appManager->rootApplicationWindow, style);
			break;
		case EKEY_FOCUSNEXT: // down on scroll
			/*case 104:*/ /* GLUT z */
			widget_focusNext(appManager->rootApplicationWindow, style);
			break;
		default:
			break;
	}
}

void manager_launchApplication(Application *app)
{
	list_append(appManager->applications, app);
}

void manager_applications(ListIterator *iter)
{
	list_begin(appManager->applications, iter);
}

Application *manager_getFocusedApplication(void)
{
	return appManager->focus;
}

void manager_focusApplication(Application *app)
{
    DataObject *currentScreen, *appObj;
	Style *style;
	DataObjectField *field;
    /*ListIterator iter;*/

	appObj = application_getDataObject(app);
	field = dataobject_getValue(appObj, "name");
	emo_printf("Focusing Application %s" NL, field->field.string);
    currentScreen = application_getCurrentScreen(app);
	if (currentScreen == NULL) {
		emo_printf("No current screen when focusing application" NL);
		return;
	}

	widget_setDataObject(appManager->rootApplicationPlaceHolder, appObj);

	style = application_getCurrentStyle(app);
	if (style == NULL)
		style = appManager->style;

	widget_resolveLayout(appManager->rootApplicationWindow,
			style);

	widget_focusNone(appManager->rootApplicationWindow,
			style, 0);

	widget_markDirty(appManager->rootApplicationWindow);

	appManager->focus = app;

	/*dataobject_debugPrint(appManager->rootApplicationWindow);*/
}

void manager_focusNextApplication(void)
{
	ListIterator iter;
	Application *newFocus = NULL, *item;
	int found = 0;

	list_begin(appManager->applications, &iter);
	while (!listIterator_finished(&iter)) {
		item = (Application *)listIterator_item(&iter);
		if (!found && item == appManager->focus)
			found = 1;
		else if (found) {
			newFocus = item;
			break;
		}
		listIterator_next(&iter);
	}
	/*listIterator_delete(iter);*/

	if (newFocus == NULL) {
		list_begin(appManager->applications, &iter);
		newFocus = (Application *)listIterator_item(&iter);
		/*listIterator_delete(iter);*/
	}
	manager_focusApplication(newFocus);	
}

Application *manager_applicationForDataObject(DataObject *dobj)
{
    ListIterator iter;
    Application *item;

    list_begin(appManager->applications, &iter);
    while (!listIterator_finished(&iter)) {
        item = (Application *)listIterator_item(&iter);
        if (application_getDataObject(item) == dobj)
            return item;
        listIterator_next(&iter);
    }
    return NULL;
}


void manager_loadApplication(DataObject *dobj, int focus, URL *url)
{
	appManager->loadingApplication = dobj;
	appManager->loadingApplicationFocus = focus;
	appManager->loadingApplicationURL = url_parse(url->all, URL_ALL);
}

void manager_loadApplicationReal(DataObject *dobj, int launch, int focus, URL *url)
{
	Application *app;

	mime_loadAll(dobj);
	app = application_load(dobj);
	application_setURL(app, url);
	if (launch)
		manager_launchApplication(app);
	if (focus)
		manager_focusApplication(app);
}

Style *manager_getRootStyle(void)
{
	return appManager->style;
}

void manager_showDialog(DataObject *dobj)
{
	dataobject_pack(appManager->rootApplicationDialogHolder, dobj);

	widget_resolveLayout(appManager->rootApplicationWindow,
			appManager->style);

	widget_markDirty(appManager->rootApplicationWindow);
}

void manager_showMenu(DataObject *dobj)
{

}
