#ifndef _TASK_H_
#define _TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

struct Task_t {
	int (*init)(void);
	int (*iteration)(void);
	int (*waitForActivity)(void);
	void (*cleanup)(void);
};
typedef struct Task_t Task;

void task_threadFunction(Task *t);

#ifdef __cplusplus
}
#endif

#endif /* _TASK_H_ */
