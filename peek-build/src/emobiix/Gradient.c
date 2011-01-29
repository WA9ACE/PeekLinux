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
	output->stops = array_newWithSize(sizeof(GradientStop), 6);

	return output;
}

void gradient_delete(Gradient *g)
{
	EMO_ASSERT(g != NULL, "delete on NULL gradient")

	/* FIXME: should do something */
}

void gradient_addStop(Gradient *g, int position, Color color)
{
	GradientStop *stop;

	EMO_ASSERT(g != NULL, "adding stop to NULL gradient")

	stop = (GradientStop *)p_malloc(sizeof(GradientStop));
	if (stop == NULL) {
		emo_printf("Stop allocated was null" NL);
		return;
	}
	stop->position = position;
	stop->color = color;

	array_append(g->stops, &stop);
}

int gradient_stops(Gradient *g)
{
	EMO_ASSERT_INT(g != NULL, 0, "getting stop count on NULL gradient")

	return array_length(g->stops);
}

Color invalidColor = { 0 };
Color gradient_getStop(Gradient *g, int id, int *height)
{
	GradientStop *s = NULL;
	int outheight;

	EMO_ASSERT_INT(g != NULL, invalidColor, "getting stop on NULL gradient")
	EMO_ASSERT_INT(height != NULL, invalidColor,
			"getting stop on gradient missing height")

	s = *(GradientStop **)array_item(g->stops, id);
	outheight = ((*height)) * s->position / 100;
	*height = outheight;
	return s->color;
}
