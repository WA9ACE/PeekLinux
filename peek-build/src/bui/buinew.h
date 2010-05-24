#ifndef UINEW_H
#define UINEW_H

#include "sysdefs.h"

void* operator new (unsigned int Size);
void* operator new[]( unsigned int size);

void operator delete(void *);
void operator delete[](void *);



#endif




