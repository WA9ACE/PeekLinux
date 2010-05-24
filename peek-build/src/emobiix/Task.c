#include "Task.h"

void task_threadFunction(Task *t)
{
	if (!t->init())
		return;

	do {
		if (!t->waitForActivity())
			break;
		t->iteration();
	} while (1);

	t->cleanup();
}
