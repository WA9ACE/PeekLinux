#ifndef __HW_TIMER_H__
#define __HW_TIMER_H__

typedef void (*timerCB)(void *tdCB);
typedef struct {
    timerCB *tCB;
    void *tData;
    int left;
    int time;
} tDS;

void timerInit(void);
void timerTimeout (void);
static void timerRemove(tDS *timeData);
void timerSignal(void);
tDS *timerCreate(timerCB *tcb, unsigned int time, void *opaque);

#endif 

