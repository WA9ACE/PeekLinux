#ifndef _APPLICATIONMANAGER_H_
#define _APPLICATIONMANAGER_H_

#include "Application.h"
#include "List.h"

#ifdef __cplusplus
extern "C" {
#endif

void manager_init(void);
void manager_drawScreen(void);
void manager_drawScreenPartial(void);
void manager_resolveLayout(void);
void manager_handleKey(int key);
void manager_launchApplication(Application *app);
void manager_applications(ListIterator *iter);
void manager_focusApplication(Application *app);
Application *manager_getFocusedApplication(void);
void manager_focusNextApplication(void);
Application *manager_getBootApp(void);
Application *manager_applicationForDataObject(DataObject *obj);

#ifdef __cplusplus
}
#endif

#endif
