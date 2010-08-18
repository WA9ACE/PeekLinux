#include "Rectangle.h"
#include "Debug.h"

#include <stdio.h>
#include <stdlib.h>

void rectangle_zero(Rectangle *r)
{
	EMO_ASSERT(r != NULL, "rectangle zero on NULL rectangle")
	r->width = 0;
	r->height = 0;
	r->x = 0;
	r->y = 0;
}

void rectangle_union(Rectangle *r, Rectangle *o)
{
	int rx1, ry1, rx2, ry2;
	int ox1, oy1, ox2, oy2;

	EMO_ASSERT(r != NULL, "rectangle union on NULL rectangle")
	EMO_ASSERT(o != NULL, "rectangle union missing operand")

	rx1 = r->x;
	ry1 = r->y;
	rx2 = r->x + r->width;
	ry2 = r->y + r->height;

	ox1 = o->x;
	oy1 = o->y;
	ox2 = o->x + o->width;
	oy2 = o->y + o->height;

	r->x = rx1 < ox1 ? rx1 : ox1;
	r->y = ry1 < oy1 ? ry1 : oy1;

	r->width = rx2 > ox2 ? rx2 - r->x : ox2 - r->x;
	r->height = ry2 > oy2 ? ry2 - r->y : oy2 - r->y;
}
