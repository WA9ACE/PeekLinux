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
	int mappedKey = 0;

	emo_printf("Got Key[%d]\n", key);

#ifndef SIMULATOR
	switch (key) {
		case KCD_0: mappedKey = '0'; break;
		case KCD_1: mappedKey = '1'; break;
		case KCD_2: mappedKey = '2'; break;
		case KCD_3: mappedKey = '3'; break;
		case KCD_4: mappedKey = '4'; break;
		case KCD_5: mappedKey = '5'; break;
		case KCD_6: mappedKey = '6'; break;
		case KCD_7: mappedKey = '7'; break;
		case KCD_8: mappedKey = '8'; break;
		case KCD_9: mappedKey = '9'; break;
	/*
		 case KCD_a: mappedKey = 'a'; break;
		 case KCD_b: mappedKey = 'b'; break;
		 case KCD_c: mappedKey = 'c'; break;
		 case KCD_d: mappedKey = 'd'; break;
		 case KCD_e: mappedKey = 'e'; break;
		 case KCD_f: mappedKey = 'f'; break;
		 case KCD_g: mappedKey = 'g'; break;
		 case KCD_h: mappedKey = 'h'; break;
		 case KCD_i: mappedKey = 'i'; break;
		 case KCD_j: mappedKey = 'j'; break;
		 case KCD_k: mappedKey = 'k'; break;
		 case KCD_l: mappedKey = 'l'; break;
		 case KCD_m: mappedKey = 'm'; break;
		 case KCD_n: mappedKey = 'n'; break;
		 case KCD_o: mappedKey = 'o'; break;
		 case KCD_p: mappedKey = 'p'; break;
		 case KCD_q: mappedKey = 'q'; break;
		 case KCD_r: mappedKey = 'r'; break;
		 case KCD_s: mappedKey = 's'; break;
		 case KCD_t: mappedKey = 't'; break;
		 case KCD_u: mappedKey = 'u'; break;
		 case KCD_v: mappedKey = 'v'; break;
		 case KCD_w: mappedKey = 'w'; break;
		 case KCD_x: mappedKey = 'x'; break;
		 case KCD_y: mappedKey = 'y'; break;
		 case KCD_z: mappedKey = 'z'; break;
	 */
		case KCD_A: mappedKey = 'A'; break;
		case KCD_B: mappedKey = 'B'; break;
		case KCD_C: mappedKey = 'C'; break;
		case KCD_D: mappedKey = 'D'; break;
		case KCD_E: mappedKey = 'E'; break;
		case KCD_F: mappedKey = 'F'; break;
		case KCD_G: mappedKey = 'G'; break;
		case KCD_H: mappedKey = 'H'; break;
		case KCD_I: mappedKey = 'I'; break;
		case KCD_J: mappedKey = 'J'; break;
		case KCD_K: mappedKey = 'K'; break;
		case KCD_L: mappedKey = 'L'; break;
		case KCD_M: mappedKey = 'M'; break;
		case KCD_N: mappedKey = 'N'; break;
		case KCD_O: mappedKey = 'O'; break;
		case KCD_P: mappedKey = 'P'; break;
		case KCD_Q: mappedKey = 'Q'; break;
		case KCD_R: mappedKey = 'R'; break;
		case KCD_S: mappedKey = 'S'; break;
		case KCD_T: mappedKey = 'T'; break;
		case KCD_U: mappedKey = 'U'; break;
		case KCD_V: mappedKey = 'V'; break;
		case KCD_W: mappedKey = 'W'; break;
		case KCD_X: mappedKey = 'X'; break;
		case KCD_Y: mappedKey = 'Y'; break;
		case KCD_Z: mappedKey = 'Z'; break;
		case KCD_SPACE: mappedKey = ' '; break;
		case KCD_BACKSPACE: mappedKey = '\b'; break;
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
		if (field != NULL && field->type == DOF_STRING && strcmp(field->field.string, "entry") == 0) {
			if (entryWidget_handleKey(focus, key, appManager->style))
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
