#include "UITask.h"

extern "C" {

void UiTask(void)
{
	task_threadFunction(&UITask);

	return;
}
}
