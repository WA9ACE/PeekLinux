#include "Point.h"
#include "Debug.h"

#include <stdio.h>
#include <stdlib.h>

void point2_zero(Point2 *p)
{
	EMO_ASSERT(p != NULL, "zero point on NULL point")

	p->x = 0;
	p->y = 0;
}

