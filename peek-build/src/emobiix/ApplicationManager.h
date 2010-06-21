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
ListIterator *manager_applications(void);
void manager_focusApplication(Application *app);

#ifdef __cplusplus
}
#endif

#endif
