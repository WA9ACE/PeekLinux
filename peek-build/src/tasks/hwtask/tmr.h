#ifndef _DEF_TI1_TMR_H_
#define _DEF_TI1_TMR_H_

int tmrInit (void (*s)(void));
void tmrStart (unsigned long ms);
unsigned long tmrStop (void);

#define TIMER_START(C,I,T)         vsi_t_start(C,I,T)
#define TIMER_STOP(C,I)            vsi_t_stop(C,I)

#define TMR_PREC    10
#if defined (_TMS470)
#define TMR_TICK    46                  /* ti hardware time         */
#else
#define TMR_TICK    500                 /* use dos ticks            */
#endif

#endif
