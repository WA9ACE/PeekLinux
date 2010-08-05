#ifndef _GRADIENT_H_
#define _GRADIENT_H_

#include "Color.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Gradient_t;
typedef struct Gradient_t Gradient;

Gradient *gradient_new(void);
void gradient_delete(Gradient *g);
void gradient_addStop(Gradient *g, int position, Color color);
int gradient_stops(Gradient *g);
Color gradient_getStop(Gradient *g, int id, int *height);

#ifdef __cplusplus
}
#endif

#endif
