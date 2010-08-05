#ifndef _APPLICATIONMANAGER_H_
#define _APPLICATIONMANAGER_H_

#include "Application.h"
#include "List.h"
#include "Style_internal.h"

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
Application *manager_applicationForDataObject(DataObject *dobj);
void manager_loadApplication(DataObject *dobj);
void manager_loadApplicationReal(DataObject *dobj);
Application *manager_getBootApp(void);
Style *manager_getRootStyle(void);

#ifdef __cplusplus
}
#endif

#endif
