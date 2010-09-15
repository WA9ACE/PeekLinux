#ifndef __UI_PEI_H__
#define __UI_PEI_H__

#include "vsi.h"

#define _ENTITY_PREFIXED(N) hw_##N
#define hCommUI        _ENTITY_PREFIXED(hCommUI)

extern T_HANDLE hCommUI;

#define EMOBIIX_KBD_EVENT 1000
typedef struct
{
    void *context;
    int key;
    int state;
} T_EMOBIIX_KBD_EVENT;


#endif // __UI_PEI_H__

