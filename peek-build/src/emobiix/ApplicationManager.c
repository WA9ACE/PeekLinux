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

#include "p_malloc.h"

#ifdef SIMULATOR
#include "sim-keymap.h"
#else
#include "mfw_kbd.h"
//#include "KeyMap.h"
#endif

#include <string.h>

struct ApplicationManager_t {
	List *applications;
	Application *focus;
	Application *rootApplication;
        Application *bootApp;
	DataObject *rootApplicationObj;
	DataObject *rootApplicationWindow;
	DataObject *rootApplicationPlaceHolder;
	Style *style;
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

	appManager->style = RootStyle();
    if (appManager->style == NULL) {
        emo_printf("Failed to create root style" NL);
        return;
    }

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

    /*bootApp = application_load(BootApplication());*/
	bootApp = application_load(LockApplication());
    	manager_launchApplication(bootApp);

	manager_focusApplication(bootApp);
	appManager->bootApp = bootApp;
}

void manager_drawScreen(void)
{
	lgui_clip_identity();
	manager_drawScreenPartial();
}

void manager_drawScreenPartial(void)
{
	DataObject *view;

	view = application_getCurrentScreen(appManager->rootApplication);
	
	if (view != NULL) {
		style_renderWidgetTree(appManager->style, view);
	} else {
		emo_printf("no view to draw" NL);
	}
}

void manager_resolveLayout(void)
{
	widget_resolveLayoutRoot(appManager->rootApplicationWindow,
			appManager->style, 0);
}

void manager_handleKey(int key)
{
	int isDirty = 0;
	Widget *focus, *accessKey;
	static Rectangle clip = {0, 0, 320, 240};
	char keyStr[2];

	emo_printf("Got Key[%d]\n", key);

#ifndef SIMULATOR
	switch (key) {
                case KCD_0: key = '0'; break;
                case KCD_1: key = '1'; break;
                case KCD_2: key = '2'; break;
                case KCD_3: key = '3'; break;
                case KCD_4: key = '4'; break;
                case KCD_5: key = '5'; break;
                case KCD_6: key = '6'; break;
                case KCD_7: key = '7'; break;
                case KCD_8: key = '8'; break;
                case KCD_9: key = '9'; break;
		/*
                case KCD_a: key = 'a'; break;
                case KCD_b: key = 'b'; break;
                case KCD_c: key = 'c'; break;
                case KCD_d: key = 'd'; break;
                case KCD_e: key = 'e'; break;
                case KCD_f: key = 'f'; break;
                case KCD_g: key = 'g'; break;
                case KCD_h: key = 'h'; break;
                case KCD_i: key = 'i'; break;
                case KCD_j: key = 'j'; break;
                case KCD_k: key = 'k'; break;
                case KCD_l: key = 'l'; break;
                case KCD_m: key = 'm'; break;
                case KCD_n: key = 'n'; break;
                case KCD_o: key = 'o'; break;
                case KCD_p: key = 'p'; break;
                case KCD_q: key = 'q'; break;
                case KCD_r: key = 'r'; break;
                case KCD_s: key = 's'; break;
                case KCD_t: key = 't'; break;
                case KCD_u: key = 'u'; break;
                case KCD_v: key = 'v'; break;
                case KCD_w: key = 'w'; break;
                case KCD_x: key = 'x'; break;
                case KCD_y: key = 'y'; break;
                case KCD_z: key = 'z'; break;
		*/
                case KCD_A: key = 'A'; break;
                case KCD_B: key = 'B'; break;
                case KCD_C: key = 'C'; break;
                case KCD_D: key = 'D'; break;
                case KCD_E: key = 'E'; break;
                case KCD_F: key = 'F'; break;
                case KCD_G: key = 'G'; break;
                case KCD_H: key = 'H'; break;
                case KCD_I: key = 'I'; break;
                case KCD_J: key = 'J'; break;
                case KCD_K: key = 'K'; break;
                case KCD_L: key = 'L'; break;
                case KCD_M: key = 'M'; break;
                case KCD_N: key = 'N'; break;
                case KCD_O: key = 'O'; break;
                case KCD_P: key = 'P'; break;
                case KCD_Q: key = 'Q'; break;
                case KCD_R: key = 'R'; break;
                case KCD_S: key = 'S'; break;
                case KCD_T: key = 'T'; break;
                case KCD_U: key = 'U'; break;
                case KCD_V: key = 'V'; break;
                case KCD_W: key = 'W'; break;
                case KCD_X: key = 'X'; break;
                case KCD_Y: key = 'Y'; break;
		case KCD_Z: key = 'Z'; break;
                case KCD_SPACE: key = ' '; break;
                case KCD_BACKSPACE: key = '\b'; break;

	}
#endif

	if (key == KCD_LOCK) { /* lock symbol */
		manager_focusNextApplication();
		return;
	}

	focus = NULL;
	if (appManager->rootApplicationWindow != NULL)
		focus = widget_focusWhichOneNF(appManager->rootApplicationWindow);
	if (focus != NULL) {
		DataObjectField *field;
		field = dataobject_getValue(focus, "type");
		if (field != NULL && field->type == DOF_STRING &&
				strcmp(field->field.string, "entry") == 0) {
			if (entryWidget_handleKey(focus, key, appManager->style))
				return;
		}
	}

	/* access key */
	((unsigned char *)keyStr)[0] = (unsigned char)key;
	keyStr[1] = 0;
	accessKey = widget_findStringField(appManager->rootApplicationWindow,
			"accesskey", keyStr);
	if (accessKey != NULL) {
		script_event(accessKey, "onreturn");
		return;
	}
	emo_printf("manager_handleKey() key - %d", key);
	switch (key) {
		case KCD_NAV_CENTER:
			script_event(focus != NULL ? focus : appManager->rootApplicationWindow, "onreturn");
			break;
		case KCD_UP: // up on scroll
		/*case 103:*/ /* GLUT a */
			widget_focusPrev(appManager->rootApplicationWindow,
					appManager->style);
			break;
		case KCD_DOWN: // down on scroll
		/*case 104:*/ /* GLUT z */
			widget_focusNext(appManager->rootApplicationWindow,
					appManager->style);
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
    DataObject *currentScreen;
    ListIterator iter;

	emo_printf("Focusing Application" NL);
    currentScreen = application_getCurrentScreen(app);
	if (currentScreen == NULL) {
		emo_printf("No current screen when focusing application" NL);
		return;
	}

    widget_getChildren(appManager->rootApplicationPlaceHolder, &iter);
    if (listIterator_finished(&iter)) {
        /*listIterator_delete(iter);*/
        return;
    }
    listIterator_remove(&iter);
    /*listIterator_delete(iter);*/
    dataobject_packStart(appManager->rootApplicationPlaceHolder,
            currentScreen);

	widget_resolveLayout(appManager->rootApplicationWindow,
			appManager->style);

	widget_focusNone(appManager->rootApplicationWindow,
			appManager->style);

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

Application *manager_applicationForDataObject(DataObject *obj)
{
	ListIterator iter;
	Application *item;

	list_begin(appManager->applications, &iter);
	while (!listIterator_finished(&iter)) {
		item = (Application *)listIterator_item(&iter);
		if (application_getDataObject(item) == obj)
			return item;
		listIterator_next(&iter);
	}
	return NULL;
}

#ifdef __cplusplus
}
#endif
