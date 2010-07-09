#include "typedefs.h"
#include "ApplicationManager.h"

#include "Application.h"
#include "List.h"
#include "lgui.h"
#include "Style.h"
#include "BootApplication.h"
#include "RootApplication.h"
#include "DataObject.h"
#include "Widget.h"
#include "RootStyle.h"
#include "Debug.h"
#include "EntryWidget.h"
#include "Script.h"

#include "p_malloc.h"
#include "buikeymap.h"

#include <string.h>

struct ApplicationManager_t {
	List *applications;
	Application *focus;
	Application *rootApplication;
	DataObject *rootApplicationObj;
	DataObject *rootApplicationWindow;
	DataObject *rootApplicationPlaceHolder;
	Style *style;
};
typedef struct ApplicationManager_t ApplicationManager;

static ApplicationManager *appManager = NULL;

void manager_init(void)
{
    Application *bootApp;

	appManager = (ApplicationManager *)p_malloc(sizeof(ApplicationManager));
	appManager->applications = list_new();
	appManager->focus = NULL;

	appManager->style = RootStyle();

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
    manager_launchApplication(bootApp);

	manager_focusApplication(bootApp);
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

#ifndef SIMULATOR
	switch (key) {
		case KP_0_KEY: key = '0'; break;
		case KP_1_KEY: key = '1'; break;
		case KP_2_KEY: key = '2'; break;
		case KP_3_KEY: key = '3'; break;
		case KP_4_KEY: key = '4'; break;
		case KP_5_KEY: key = '5'; break;
		case KP_6_KEY: key = '6'; break;
		case KP_7_KEY: key = '7'; break;
		case KP_8_KEY: key = '8'; break;
		case KP_9_KEY: key = '9'; break;
		case KP_a_KEY: key = 'a'; break;
		case KP_b_KEY: key = 'b'; break;
		case KP_c_KEY: key = 'c'; break;
		case KP_d_KEY: key = 'd'; break;
		case KP_e_KEY: key = 'e'; break;
		case KP_f_KEY: key = 'f'; break;
		case KP_g_KEY: key = 'g'; break;
		case KP_h_KEY: key = 'h'; break;
		case KP_i_KEY: key = 'i'; break;
		case KP_j_KEY: key = 'j'; break;
		case KP_k_KEY: key = 'k'; break;
		case KP_l_KEY: key = 'l'; break;
		case KP_m_KEY: key = 'm'; break;
		case KP_n_KEY: key = 'n'; break;
		case KP_o_KEY: key = 'o'; break;
		case KP_p_KEY: key = 'p'; break;
		case KP_q_KEY: key = 'q'; break;
		case KP_r_KEY: key = 'r'; break;
		case KP_s_KEY: key = 's'; break;
		case KP_t_KEY: key = 't'; break;
		case KP_u_KEY: key = 'u'; break;
		case KP_v_KEY: key = 'v'; break;
		case KP_w_KEY: key = 'w'; break;
		case KP_x_KEY: key = 'x'; break;
		case KP_y_KEY: key = 'y'; break;
		case KP_z_KEY: key = 'z'; break;
		case KP_A_KEY: key = 'A'; break;
		case KP_B_KEY: key = 'B'; break;
		case KP_C_KEY: key = 'C'; break;
		case KP_D_KEY: key = 'D'; break;
		case KP_E_KEY: key = 'E'; break;
		case KP_F_KEY: key = 'F'; break;
		case KP_G_KEY: key = 'G'; break;
		case KP_H_KEY: key = 'H'; break;
		case KP_I_KEY: key = 'I'; break;
		case KP_J_KEY: key = 'J'; break;
		case KP_K_KEY: key = 'K'; break;
		case KP_L_KEY: key = 'L'; break;
		case KP_M_KEY: key = 'M'; break;
		case KP_N_KEY: key = 'N'; break;
		case KP_O_KEY: key = 'O'; break;
		case KP_P_KEY: key = 'P'; break;
		case KP_Q_KEY: key = 'Q'; break;
		case KP_R_KEY: key = 'R'; break;
		case KP_S_KEY: key = 'S'; break;
		case KP_T_KEY: key = 'T'; break;
		case KP_U_KEY: key = 'U'; break;
		case KP_V_KEY: key = 'V'; break;
		case KP_W_KEY: key = 'W'; break;
		case KP_X_KEY: key = 'X'; break;
		case KP_Y_KEY: key = 'Y'; break;
		case KP_Z_KEY: key = 'Z'; break;
		case KP_SPACE_KEY: key = ' '; break;
		case KP_BACKSPACE_KEY: key = '\b'; break;
	}
#endif

	if (key == 42) { /* lock symbol */
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

	switch (key) {
		case KP_1_KEY:
		case KP_2_KEY:
		case 46: // Push down on wheel
		case 13:
			script_event(focus != NULL ? focus : appManager->rootApplicationWindow, "onreturn");
			break;
		case 87: // up on scroll
		/*case 103:*/ /* GLUT a */
			widget_focusPrev(appManager->rootApplicationWindow,
					appManager->style);
			break;
		case 86: // down on scroll
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

#ifdef __cplusplus
}
#endif
