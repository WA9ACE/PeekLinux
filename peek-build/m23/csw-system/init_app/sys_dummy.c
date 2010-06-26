/*
 * DRV_DUMMY.C
 *
 * Set of dummy functions needed (because called by Condat or Layer1) when Riviera SWEs
 * are disabled (R2D, KPD, etc...)
 *
 * Copyright (c) Texas Instruments 2002
 *
 */

#include "rv/rv_defined_swe.h"

#ifndef RVM_KPD_SWE
  #include "nucleus.h" 

/* This structure defines the two functions to call when PWR key is pressed */
typedef struct {  void (*pressed)(int);
                  void (*released)(void);
               } T_KPD_KEYPAD; 

T_KPD_KEYPAD Kp = {0};

  void KP_Init(void (pressed(int)), void (released(void)))
  {
    Kp.pressed = pressed;
    Kp.released = released;
  }

  void kpd_power_key_pressed(void)
  {
      //Dummy
  }

  void kpd_key_handler(void)
  {
      //Dummy
  }
#endif // RVM_KPD_SWE

#ifndef RVM_R2D_SWE
  void *r2d_g_lcd_gc;

  void r2d_draw_point(void *gc, short x, short y)
  {
      //Dummy
  }

  void r2d_erase_point(void *gc, short x, short y)
  {
      //Dummy
  }

  void r2d_erase(void *gc)
  {
      //Dummy
  }

  void LCD_Cursor(unsigned char y, unsigned char x)
  {
      //Dummy
  }

  void LCD_SetPower(void)
  {
      //Dummy
  }

  void LCD_Clear(void)
  {
      //Dummy
  }

  void LCD_Reset(void)
  {
      //Dummy
  }

  void LCD_Enable(unsigned char x)
  {
      //Dummy
  }
#endif // RVM_R2D_SWE

#ifndef RVM_ETM_SWE

#define ETM_PACEKT_SIZE 256

typedef int (*ETM_CALLBACK_FUNC)(unsigned char*, int);

typedef struct
{
    unsigned short size;
    int index;          
    unsigned char  mid;
    unsigned char  status;
    unsigned char  data[ETM_PACEKT_SIZE];
} T_ETM_PKT;

  int etm_hook(unsigned char *inbuf, unsigned short size)
  {
    // Return 1 so that the received packets are processed like TM3 packets.
    return 1;
  }

  int etm_register(char name[], int mid, int task_id, unsigned short addr_id, ETM_CALLBACK_FUNC callback)
  {
    return 0; 
  }

  int etm_pkt_send(T_ETM_PKT *pkt)
  {
    return 0;
  }

#endif // RVM_ETM_SWE


  
/*-------------------------------------------------------*/
/* cama_sleep_status()                                   */
/*-------------------------------------------------------*/
/*                                                       */
/* Dummy function to replace the function that is used	 */
/* from Layer 1 to check if the camera is working        */
/* to manage the sleep mode.                             */
/*-------------------------------------------------------*/
#ifndef RVM_CAMA_SWE
  unsigned char cama_sleep_status(void)
  {
    return 1;  //Dummy
  }
#endif // RVM_CAMA_SWE


  

/*-------------------------------------------------------*/
/* tx_tch_data()                                         */
/*-------------------------------------------------------*/
/*                                                       */
/* Dummy function to replace the function that is used	 */
/* from Layer 1 to read a data							 */
/* block from DATA ADAPTOR to be passed to DSP.          */
/*-------------------------------------------------------*/

char *tx_tch_data(void)
{
  return(0);
}

