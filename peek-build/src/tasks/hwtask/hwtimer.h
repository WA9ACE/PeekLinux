#ifndef __HW_TIMER_H__
#define __HW_TIMER_H__

struct tDS;
typedef struct tDS tDS;
typedef void (*timerCB)(tDS *, void *tdCB);

void timerInit(void);
void timerTimeout (void);
static void timerRemove(tDS *timeData);
void timerSignal(void);
tDS *timerCreate(timerCB tcb, void *opaque);

#endif 

