#ifndef SIMULATOR
#include "general.h"
#include "typedefs.h"
#endif
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
#include "Script.h"
#include "Hardware.h"
#include "Mime.h"

#include "p_malloc.h"

#include "KeyMappings.h"

#if 0
#ifdef SIMULATOR
#include "sim-keymap.h"
#else
#include "mfw_kbd.h"
//#include "KeyMap.h"
#endif
#endif

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
	Style *style;

	DataObject *loadingApplication;
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
		appManager->loadingApplication = NULL;
		manager_loadApplicationReal(toload);
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

void manager_handleKey(int key)
{
	int isDirty = 0;
	Widget *focus, *accessKey;
	static Rectangle clip = {0, 0, 320, 240};
	char keyStr[2];
	Style *style;
	int mappedKey = 0;

	emo_printf("Got Key[%d]\n", key);

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
		if (field != NULL && field->type == DOF_STRING && strcmp(field->field.string, "entry") == 0) {
			if (entryWidget_handleKey(focus, key, style))
				return;
		}
	}

	emo_printf("manager_handleKey() key - %d (%c)", key, mappedKey);

	/* access key */
	((unsigned char *)keyStr)[0] = (unsigned char)mappedKey;
	keyStr[1] = 0;
	accessKey = widget_findStringField(appManager->rootApplicationWindow, "accesskey", keyStr);
	if (accessKey != NULL) {
		script_event(accessKey, "onreturn");
		return;
	}

	switch (key) {
		case EKEY_ACTIVATE:
			script_event(focus != NULL ? focus : appManager->rootApplicationWindow, "onreturn");
			break;
		case EKEY_FOCUSPREV: // up on scroll
			/*case 103:*/ /* GLUT a */
			widget_focusPrev(appManager->rootApplicationWindow,
					style);
			break;
		case EKEY_FOCUSNEXT: // down on scroll
			/*case 104:*/ /* GLUT z */
			widget_focusNext(appManager->rootApplicationWindow,
					style);
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
    /*ListIterator iter;*/

	emo_printf("Focusing Application" NL);
    currentScreen = application_getCurrentScreen(app);
	if (currentScreen == NULL) {
		emo_printf("No current screen when focusing application" NL);
		return;
	}
	appObj = application_getDataObject(app);

	/*
	 * Old non-frame code
	 */
#if 0
    widget_getChildren(appManager->rootApplicationPlaceHolder, &iter);
    if (listIterator_finished(&iter)) {
        /*listIterator_delete(iter);*/
        return;
    }
    listIterator_remove(&iter);
    /*listIterator_delete(iter);*/
    dataobject_packStart(appManager->rootApplicationPlaceHolder,
            currentScreen);
#endif

	widget_setDataObject(appManager->rootApplicationPlaceHolder, appObj);

	style = application_getCurrentStyle(app);
	if (style == NULL)
		style = appManager->style;

	widget_resolveLayout(appManager->rootApplicationWindow,
			style);

	widget_focusNone(appManager->rootApplicationWindow,
			style);

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


void manager_loadApplication(DataObject *dobj)
{
	appManager->loadingApplication = dobj;
}

void manager_loadApplicationReal(DataObject *dobj)
{
	Application *app;

	mime_loadAll(dobj);
	app = application_load(dobj);
	manager_launchApplication(app);
	manager_focusApplication(app);
}
