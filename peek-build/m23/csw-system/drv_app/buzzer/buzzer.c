/*
 * BUZZER.C
 *
 * Buzzer driver
 *
 * Target : ARM
 *
 * Copyright (c) Texas Instruments 2000
 *
 */


#include "sys_types.h"
#include "memif/mem.h"
#include "armio/armio.h"
#include "buzzer/buzzer.h"
#include "timer/timer.h"

#include "board.cfg"

/* 
 * Initialize buzzer
 *
 */
void BZ_Init(void)
{
}

/*
 * BZ_Enable / BZ_Disable
 * 
 * Enable / disable buzzer
 * 
 * The buzzer uses timer 1
 */ 
void BZ_Enable(void)
{
  *((volatile SYS_UWORD16 *) ARMIO_PWM_CNTL) |= BZ_ON; 
}

void BZ_Disable(void)
{
  *((volatile SYS_UWORD16 *) ARMIO_PWM_CNTL) &= ~BZ_ON; 
}

/*
 * BZ_Tone
 * 
 * Changes the timer count to set the tone frequency in Hz
 * 
 * 58 ticks= 1 ms, which give a frequency of 500 Hz
 *
 *
 */
void BZ_Tone(int f)
{
    if (f > 255)
    {
        f = 255;
    }
  *((volatile SYS_UWORD16 *) ARMIO_LOAD_TIM) = f;
}

/*
 * BZ_Volume
 * 
 * Changes the buzzer volume
 * 
 */
void BZ_Volume(int v)
{
  // the level range is 0 up to 63
    if (v > 63)
    {
        v = 63;
    }
  *((volatile SYS_UWORD16 *) BZ_LEVEL) = v;
}

/*
 * BZ_KeyBeep_ON
 * 
 * Audio feedback to user after keybeep
 *
 */
void BZ_KeyBeep_ON(void)
{
  volatile int i;

  BZ_Init ();
  BZ_Volume (255);
  BZ_Enable ();
  BZ_Tone (50);
  
  for (i = 0; i < 17000; i++)
    ;
  
  BZ_Disable ();
}



/*
 * BZ_KeyBeep_OFF
 * 
 * Audio feedback to user after keybeep
 *
 */
void BZ_KeyBeep_OFF(void)
{
  volatile int i;

  BZ_Init ();
  BZ_Volume (255);
  BZ_Enable ();
  BZ_Tone (100);
  
  for (i = 0; i < 17000; i++)
    ;
  
  BZ_Disable ();
}



/*
 * LT_Enable / LT_Disable
 * 
 * Enable / disable LCD lighting
 * 
 */
void LT_Enable(void)
{
#if (BOARD == 7 || BOARD == 8 || BOARD == 9 || BOARD == 43)
   *((volatile SYS_UWORD16 *) ARMIO_PWM_CNTL) |= LT_ON;
#endif
}
void LT_Disable(void)
{
#if (BOARD == 7 || BOARD == 8 || BOARD == 9 || BOARD == 43)
   *((volatile SYS_UWORD16 *) ARMIO_PWM_CNTL) &= ~LT_ON;
#endif
}


/*
 * LT_Level
 * 
 * Set LCD display level
 */
void LT_Level(SYS_WORD8 level)
{
   if (level > 63) level=63;

   // the level range is 0 up to 63
   *((volatile SYS_UWORD16 *) LT_LEVEL) = level;
}


/*
 * LT_Status
 * 
 * Return lighting status for sleep manager
 * 
 */
SYS_BOOL LT_Status(void)
{
#if (BOARD == 7 || BOARD == 8 || BOARD == 9)
   if (*((volatile SYS_UWORD16 *) ARMIO_PWM_CNTL) & LT_ON) 
      return(1);		 // the light is on
   else
      return(0); 
#endif

return(0); 

}
