#include "Gradient.h"
#include "Debug.h"

#include "Array.h"

#include "p_malloc.h"

typedef struct GradientStop_t GradientStop;
struct GradientStop_t {
	int position;
	Color color;
};

struct Gradient_t {
    Array *stops;
};

Gradient *gradient_new(void)
{
	Gradient *output;
	output = (Gradient *)p_malloc(sizeof(Gradient));
	output->stops = array_new_with_size(sizeof(GradientStop), 4);

	return output;
}

void gradient_addStop(Gradient *g, int position, Color color)
{
	GradientStop *stop;

	stop = (GradientStop *)p_malloc(sizeof(GradientStop));
	if (stop == NULL)
		emo_printf("Stop allocated was null" NL);
	stop->position = position;
	stop->color = color;

	array_append(g->stops, stop);
}

int gradient_stops(Gradient *g)
{
	return array_length(g->stops);
}

Color gradient_getStop(Gradient *g, int id, int *height)
{
	GradientStop *s;
	int outheight;

	s = *((GradientStop **)array_item(g->stops, id));
	outheight = ((*height)) * s->position / 100;
	*height = outheight;
	return s->color;
}
