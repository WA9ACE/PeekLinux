/**
 * @file	kpd_scan_functions.c
 *
 * Implementation of hardware keypad interface functions.
 * These functions implement the keypad interface with the windows.
 *
 * @author	Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author       Modification
 *  ------------------------------------
 *  10/10/2001 L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "board.cfg"

#include "nucleus.h" /* used for HISR */

#include "kpd/kpd_scan_functions.h"
#include "kpd/kpd_cfg.h"
#include "kpd/kpd_physical_key_def.h"
#include "kpd/kpd_messages_i.h"
#include "kpd/kpd_env.h"

#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"
#include "rvf/rvf_env.h"

#if ((CHIPSET == 12) || (CHIPSET == 15))
  #include "inth/sys_inth.h"
#else
  #include "armio/armio.h"
  #include "inth/iq.h"
#endif

#include "memif/mem.h"
#ifdef KPD_GPIO_ENABLED
#include "pin_config.h"
#define CONF_GPIO_19    * (volatile SYS_UWORD16 *) (0xFFFEF16A) 
#define KPD_RF_GPIO 19 /* we use GPIO 19  */
#endif


#if ((CHIPSET == 12) || (CHIPSET == 15))

  /**
   * Register mapping
   */
  #define KBR_CTRL_REG                  (MEM_KEYBOARD + 0x00) /* KBR control reg */
  #define KBR_DEBOUNCING_TIME           (MEM_KEYBOARD + 0x02) /* KBR debouncing time reg */
  #define KBR_LONG_KEY_TIME             (MEM_KEYBOARD + 0x04) /* KBR long key time reg */
  #define KBR_TIME_OUT                  (MEM_KEYBOARD + 0x06) /* KBR Time out reg */
  #define KBR_INTERRUPT_STATUS_REG      (MEM_KEYBOARD + 0x08) /* KBR interrupt status reg */
  #define KBR_CLR_INTERRUPT_STATUS_REG  (MEM_KEYBOARD + 0x0A) /* KBR clear interrupt status reg */
  #define KBR_INTERRUPT_ENALBE          (MEM_KEYBOARD + 0x0C) /* KBR interrupt enalbe reg */
  #define KBR_STATE_MACHINE_STATUS      (MEM_KEYBOARD + 0x0E) /* KBR state machine status reg */
  #define KBR_IN                        (MEM_KEYBOARD + 0x10) /* KBR inputs (rows) */
  #define KBR_OUT                       (MEM_KEYBOARD + 0x12) /* KBR outputs (columns) */
  #define KBR_FULL_CODE_15_0            (MEM_KEYBOARD + 0x14) /* KBR full code 15 - 0 */
  #define KBR_FULL_CODE_31_16           (MEM_KEYBOARD + 0x16) /* KBR full code 31 - 16 */
  #define KBR_FULL_CODE_47_32           (MEM_KEYBOARD + 0x18) /* KBR full code 47 - 32 */
  #define KBR_FULL_CODE_63_48           (MEM_KEYBOARD + 0x1A) /* KBR full code 63 - 48 */
  #define KBR_FORCE_KBC		        (MEM_KEYBOARD + 0x1EA) /* KBR Force KBC to Tristate when IP freeze */
  /**
   * Register values
   */
  #define NORMAL_OPERATION              1

  #define MAX_COL  8    /* Used for mapping key value on to layout */

  #define NO_KEY   0
#endif


#if ((CHIPSET == 12) || (CHIPSET == 15))
  #define KP_ROW_IN   KBR_IN
  #define KP_COL_OUT  KBR_OUT
#else
  #define KP_ROW_IN   ARMIO_KBR_IN
  #define KP_COL_OUT  ARMIO_KBR_OUT
#endif

#if (BOARD == 7)
   const T_KPD_PHYSICAL_KEY_ID keypad_layout[KP_ROWS][KP_COLS]=
   {
   /* Layout of B-Sample */
   {KPD_PKEY_SOFT_LEFT,  KPD_PKEY_UP,   KPD_PKEY_DOWN, KPD_PKEY_SOFT_RIGHT},
   {KPD_PKEY_1,          KPD_PKEY_2,    KPD_PKEY_3,    KPD_PKEY_GREEN},
   {KPD_PKEY_4,          KPD_PKEY_5,    KPD_PKEY_6,    KPD_PKEY_NULL},
   {KPD_PKEY_7,          KPD_PKEY_8,    KPD_PKEY_9,    KPD_PKEY_NULL},
   {KPD_PKEY_STAR,       KPD_PKEY_0,    KPD_PKEY_DIESE,KPD_PKEY_NULL},
   };
#elif ((BOARD == 8) || (BOARD == 9))
   const T_KPD_PHYSICAL_KEY_ID keypad_layout[KP_ROWS][KP_COLS]=
   {
   /* Layout of C-Sample */
   {KPD_PKEY_UP,    KPD_PKEY_GREEN,KPD_PKEY_SOFT_RIGHT,KPD_PKEY_DOWN},
   {KPD_PKEY_1,     KPD_PKEY_2,    KPD_PKEY_3,         KPD_PKEY_SOFT_LEFT},
   {KPD_PKEY_4,     KPD_PKEY_5,    KPD_PKEY_6,         KPD_PKEY_NULL},
   {KPD_PKEY_7,     KPD_PKEY_8,    KPD_PKEY_9,         KPD_PKEY_NULL},
   {KPD_PKEY_STAR,  KPD_PKEY_0,    KPD_PKEY_DIESE,     KPD_PKEY_NULL},
   };
#elif ((BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43))
   const T_KPD_PHYSICAL_KEY_ID keypad_layout[KP_ROWS][KP_COLS]=
   {
   /* Layout of D-Sample and E-Sample */
   {KPD_PKEY_GREEN, KPD_PKEY_VOL_DOWN, KPD_PKEY_VOL_UP,KPD_PKEY_SOFT_LEFT,  KPD_PKEY_LEFT},
   {KPD_PKEY_1,     KPD_PKEY_2,        KPD_PKEY_3,     KPD_PKEY_REC,        KPD_PKEY_RIGHT},
   {KPD_PKEY_4,     KPD_PKEY_5,        KPD_PKEY_6,     KPD_PKEY_SOFT_RIGHT, KPD_PKEY_UP},
   {KPD_PKEY_7,     KPD_PKEY_8,        KPD_PKEY_9,     KPD_PKEY_NULL,       KPD_PKEY_DOWN},
   {KPD_PKEY_STAR,  KPD_PKEY_0,        KPD_PKEY_DIESE, KPD_PKEY_NULL,       KPD_PKEY_NAV_CENTER},
   };
#elif ((BOARD==70)||(BOARD==71))
   const T_KPD_PHYSICAL_KEY_ID keypad_layout[KP_ROWS][KP_COLS]=
   {
   /* Layout of I-Sample */
   {KPD_PKEY_HOME, 	KPD_PKEY_BACK, 	   KPD_PKEY_SOFT_LEFT,	KPD_PKEY_SOFT_RIGHT,  KPD_PKEY_UP},
   {KPD_PKEY_1,     KPD_PKEY_2,        KPD_PKEY_3,     		KPD_PKEY_GREEN,       KPD_PKEY_RIGHT},
   {KPD_PKEY_4,     KPD_PKEY_5,        KPD_PKEY_6,     		KPD_PKEY_RED,		  KPD_PKEY_LEFT},
   {KPD_PKEY_7,     KPD_PKEY_8,        KPD_PKEY_9,     		KPD_PKEY_VOL_DOWN,    KPD_PKEY_DOWN},
   {KPD_PKEY_STAR,  KPD_PKEY_0,        KPD_PKEY_DIESE, 		KPD_PKEY_VOL_UP,      KPD_PKEY_NAV_CENTER},
   };
#endif


#define KP_ACTIVATE(i)  (~(1<<i))
#define KP_IS_ACTIVE(rows,i)  ((rows & (1<<i)) == 0)
#define KP_ALL_OFF  0x1F
#define KP_ALL_ON   0

extern T_KPD_ENV_CTRL_BLK* kpd_env_ctrl_blk;

typedef struct {  NU_HISR  hisr;
#ifndef HISR_STACK_SHARING
#if (LOCOSTO_LITE)
                  char     hisr_stack[288];
#else
                  char     hisr_stack[2048];
#endif
#endif
 	} T_HISR_INFOS;

static T_HISR_INFOS hisr_infos = {0};
extern UINT16 kpd_configured_long_key_time;
extern UINT16 kpd_configured_repeat_key_time;
UINT16 configured_debounce_value=0,configured_timeout_value=0;
#if (L1_RF_KBD_FIX == 1)
INT8 present_key_capture[KPD_MAX_DETECTABLE];
UINT8 capture_nb_keys=0;
#endif
static BOOLEAN repeat_active = FALSE;

#ifdef KPD_GPIO_ENABLED
void kpd_rf_gpio_test(UINT8);
#endif
/**
 * @name Functions implementation
 *
 */
/*@{*/


#if ((CHIPSET == 12) || (CHIPSET == 15))

/**  kpd_init_ctrl_reg : Initialize the Control register
 */
void kpd_init_ctrl_reg(const UINT8 software_nreset,
                       const UINT8 nsoftware_mode,
                       const T_KPD_PTV ptv,
                       const T_KPD_EnableDetection long_key_process_en,
                       const T_KPD_EnableDetection time_out_empty_en,
                       const T_KPD_EnableDetection time_out_long_key_en,
                       const T_KPD_EnableDetection repeat_mode_en)
{
  volatile UINT16 status_reg;
  status_reg = *(volatile UINT16*) KBR_STATE_MACHINE_STATUS;

  if ( (status_reg != KPD_TEST_TIMER_DEBOUNCING) && (status_reg != KPD_TEST_TIMER_LONG_KEY) &&
       (status_reg != KPD_TEST_TIMER_TIME_OUT)   && (status_reg != KPD_TEST_TIMER_REPEAT_KEY) )
  {

    /* The PTV can be programmed since the timer is not running */

    *(volatile UINT16*) KBR_CTRL_REG = (software_nreset |
                                        nsoftware_mode       << 1 |
                                        ptv                  << 2 |
                                        long_key_process_en  << 5 |
                                        time_out_empty_en    << 6 |
                                        time_out_long_key_en << 7 |
                                        repeat_mode_en       << 8);
  }
  else
  {

    /* The PTV must not be programmed when the timer is running */

    SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 0, 1, software_nreset);
    SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 1, 1, nsoftware_mode);
    SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 5, 1, long_key_process_en);
    SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 6, 1, time_out_empty_en);
    SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 7, 1, time_out_long_key_en);
    SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 8, 1, repeat_mode_en);
  }
}

/** kpd_ctrl_repeat_int : set or reset repeat interrupt
 */
void kpd_ctrl_repeat_int(const UINT8 state)
{
  if (state == KPD_ENABLE_REPEAT)
  {
    SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 8, 1, KPD_DETECTION_ENABLED);
  }
  else
  {
    SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 8, 1, KPD_DETECTION_DISABLED);
  }
}


/**  kpd_software_reset : reset software
 */
void kpd_software_reset(void)
{
  volatile UINT16 mem_reg;

  mem_reg = (*(volatile UINT16*) KBR_CTRL_REG) & 0xFFFE;
  *(volatile UINT16*) KBR_CTRL_REG = mem_reg;

}


/**  kpd_set_debouncing_time : Set the desired value of debouncing time
 */
void kpd_set_debouncing_time(const UINT8 debouncing_time)
{

   *(volatile UINT16*) KBR_DEBOUNCING_TIME = debouncing_time;
   configured_debounce_value=debouncing_time;

}

/** kpd_set_timeout_time : Set the desired value of the timeout time
 */
void kpd_set_timeout_time(const UINT16 timeout_time)
{
  *(volatile UINT16*) KBR_TIME_OUT = ((timeout_time  * KPD_CLOCK_FREQ) / (1<<(KPD_CLOCK_DIVIDER+1)))-1;
  configured_timeout_value=(*(volatile UINT16*) KBR_TIME_OUT);
}

#endif /* (CHIPSET == 12) */




/**
 * function: hisr_entry
 */
static void hisr_entry(void)
{
   T_RVF_MB_STATUS mb_status;
   T_KPD_KEY_PRESSED_MSG* msg_key_pressed;
   //T_KPD_PHYSICAL_KEY_ID key;

#if ((CHIPSET != 12) && (CHIPSET != 15)) || (((CHIPSET == 12) || (CHIPSET == 15)) && (KPD_DECODING_MODE == SOFTWARE_MODE_ENABLE))

   /* Reserve memory for message */
   mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEY_PRESSED_MSG), (void **) &msg_key_pressed);

   if (mb_status != RVF_RED) /* Memory allocation success */
   {
      /* Fill the message */
      msg_key_pressed->hdr.msg_id = KPD_KEY_PRESSED_MSG;
      /* Virtual key id is not yet known */
      msg_key_pressed->value = KPD_PKEY_NULL;
      if (mb_status == RVF_GREEN)
         msg_key_pressed->key_to_process = TRUE;
      else
         msg_key_pressed->key_to_process = FALSE;

      /* Send message to the keypad task */
      rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_key_pressed);
   }
   else
   {
      KPD_SEND_TRACE("KPD: Not enough memory to send new key pressed", RV_TRACE_LEVEL_ERROR);
      kpd_acknowledge_key_pressed();
   }

#else

  UINT8 int_status_reg;
  T_KPD_GENERIC_INTERRUPT_MSG* msg_interrupt;
  T_KPD_KEY_EVENT_INTERRUPT_MSG* msg_key_interrupt;

  /* Retrieve interrupt status from register */
  int_status_reg = *(volatile UINT16*) KBR_INTERRUPT_STATUS_REG;
#if (L1_RF_KBD_FIX == 1)
  if ((int_status_reg & 0x0001) )   /* Key changed / Long key press is taken care through SW Timer*/
#else
  if ((int_status_reg & 0x0001) || (int_status_reg & 0x0002))
#endif
  {
    mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEY_EVENT_INTERRUPT_MSG), (void **) &msg_key_interrupt);

    if (mb_status != RVF_RED) /* Memory allocation success */
    {
      if (int_status_reg & 0x0001)
      {
        msg_key_interrupt->hdr.msg_id = KPD_KEY_EVENT_INTERRUPT_MSG;

        /* Key pressed change -> Reset Long timer */
        *(volatile UINT16*) KBR_LONG_KEY_TIME = kpd_env_ctrl_blk->long_time;
     	/*	    KPD_SEND_TRACE("KPD: SHORT KEY INTERRUPT",RV_TRACE_LEVEL_ERROR); */
      }
      else
      {
        msg_key_interrupt->hdr.msg_id = KPD_LONG_KEY_INTERRUPT_MSG;
        if(repeat_active == FALSE)
        {
          repeat_active = TRUE;
          *(volatile UINT16*) KBR_LONG_KEY_TIME = kpd_env_ctrl_blk->repeat_time;
        }
		/*    KPD_SEND_TRACE("KPD: LONG KEY INTERRUPT",RV_TRACE_LEVEL_ERROR); */
	  }
      msg_key_interrupt->full_code_15_0  = *(volatile UINT16*) KBR_FULL_CODE_15_0;
      msg_key_interrupt->full_code_31_16 = *(volatile UINT16*) KBR_FULL_CODE_31_16;
      msg_key_interrupt->full_code_47_32 = *(volatile UINT16*) KBR_FULL_CODE_47_32;
      msg_key_interrupt->full_code_63_48 = *(volatile UINT16*) KBR_FULL_CODE_63_48;

      /* Set long timer register to Long time */
      if ((msg_key_interrupt->full_code_15_0 ==  NO_KEY) &&
          (msg_key_interrupt->full_code_31_16 == NO_KEY) &&
          (msg_key_interrupt->full_code_47_32 == NO_KEY) &&
          (msg_key_interrupt->full_code_63_48 == NO_KEY))
      {
          repeat_active = FALSE;
          *(volatile UINT16*) KBR_LONG_KEY_TIME = kpd_env_ctrl_blk->long_time;
      }

      /* Send message to the keypad task */
      rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_key_interrupt);
    }
    else
    {
      KPD_SEND_TRACE("KPD: Not enough memory to send interrupt message", RV_TRACE_LEVEL_ERROR);
    }
  }
  else if ((int_status_reg & 0x0004) || (int_status_reg & 0x0008)) /* interrupt e.g. miss event, time out */
  {
    mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_GENERIC_INTERRUPT_MSG), (void **) &msg_interrupt);

    if (mb_status != RVF_RED) /* Memory allocation success */
    {
      if (int_status_reg & 0x0008)        /* Miss event */
      {
        msg_interrupt->hdr.msg_id = KPD_MISS_EVENT_INTERRUPT_MSG;
      }
      else if(int_status_reg & 0x0004)   /* Time out */
      {
        msg_interrupt->hdr.msg_id = KPD_TIMEOUT_INTERRUPT_MSG;
      }

      /* Send message to the keypad task */
      rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_interrupt);
    }
    else
    {
      KPD_SEND_TRACE("KPD: Not enough memory to send interrupt message", RV_TRACE_LEVEL_ERROR);
    }
  }
  /* Clear interrupt */
  *(volatile UINT16*) KBR_CLR_INTERRUPT_STATUS_REG |= int_status_reg;
  kpd_acknowledge_key_pressed();


#endif /* ((CHIPSET != 12) && (CHIPSET != 15)) || (((CHIPSET == 12) || (CHIPSET == 15)) && (KPD_DECODING_MODE == SOFTWARE_MODE_ENABLE)) */
}


/**
 * function: kpd_initialize_keypad_hardware
 */
void kpd_initialize_keypad_hardware(void)
{

  /**
   * Common initialisation
   */

   /* HISR creation */
   NU_Create_HISR(&hisr_infos.hisr,
                  "KPD_HISR",
                  hisr_entry,
                  2,
    	#ifndef HISR_STACK_SHARING
                  hisr_infos.hisr_stack,
                  sizeof(hisr_infos.hisr_stack));
    	#else
		  HISR_STACK_PRIO2,
		  HISR_STACK_PRIO2_SIZE);
    	#endif

  /**
   * Chip / Board specific initialisation
   */

#if ((CHIPSET == 12) || (CHIPSET == 15))

  /** Hardware decoding
   */
  #if   (KPD_DECODING_MODE == HARDWARE_DECODING)

  /* Init control register ; hardware decoding */
    kpd_init_ctrl_reg(NORMAL_OPERATION,            /* Reset / Normal op */
                      HARDWARE_DECODING,           /* Hardware / Software decoding */
                      KPD_CLOCK_DIVIDER,           /* Clk divisor */
                      KPD_DETECTION_ENABLED,       /* Long key enable */
                      KPD_DETECTION_ENABLED,       /* timeout no key */
                      KPD_DETECTION_ENABLED,       /* timeout long key */
                      KPD_DETECTION_DISABLED);     /* Repeat key enable */



    kpd_set_debouncing_time(KPD_DEBOUNCING_TIME);

    /* Clear old pending interrupts */
    *(volatile UINT16*) KBR_CLR_INTERRUPT_STATUS_REG |= 0xFF;

    /* Set default timer values */
    *(volatile UINT16*) KBR_LONG_KEY_TIME = KPD_DEFAULT_LONG_TIME;
    *(volatile UINT16*) KBR_TIME_OUT = KPD_DEFAULT_TIME_OUT_TIME;
	configured_timeout_value=KPD_DEFAULT_TIME_OUT_TIME;

#if (KEYPAD_RF_ISSUE_SW_WORKAROUND == 1 || KEYPAD_RF_ISSUE_SW_WORKAROUND == 2)
    *(volatile UINT16*) KBR_FORCE_KBC = 0x01;
#endif

  /** Software decoding
   */
  #elif (KPD_DECODING_MODE == SOFTWARE_MODE_ENABLE)

    /* Init control register ; hardware decoding */
    kpd_init_ctrl_reg(1, HARDWARE_DECODING, KPD_CLOCK_DIVIDER,
                     KPD_DETECTION_DISABLED, KPD_DETECTION_DISABLED,
                     KPD_DETECTION_DISABLED, KPD_DETECTION_DISABLED);

   /* Debouncing time = 64ms */
    kpd_set_debouncing_time(KPD_DEBOUNCING_TIME);

    /* Activate all outputs */
    *(volatile UINT16*) KP_COL_OUT = KP_ALL_ON;

  #endif /* KPD_HW_RESOLUTION */

#ifdef KPD_GPIO_ENABLED
  GPIO_DIRECTION_OUT(KPD_RF_GPIO);
#endif
  /* Unmask keypad interrupt */
  //IQ_Unmask (C_INTH_KEYBOARD_IT);
/* commenting this as part of OMAPS00105266 
  F_INTH_ENABLE_ONE_IT(C_INTH_KEYBOARD_IT);
*/

#else /* Other chipsets */

   /* Activate all outputs */
   *(volatile UINT16*) KP_COL_OUT = KP_ALL_ON;

   /* Unmask keypad interrupt */
   #if ((BOARD == 8) || (BOARD == 9) || (BOARD == 40) || (BOARD == 41))
     AI_UnmaskIT (ARMIO_MASKIT_KBD);
   #else
     IQ_Unmask (IQ_ARMIO);
   #endif

#endif /* (CHIPSET == 12) */

}



/**
 * function: kpd_key_handler
 */
void kpd_key_handler(void)
{
   /* If keypad is not started, return immediately */
   if ( (kpd_env_ctrl_blk == 0) || (kpd_env_ctrl_blk->swe_is_initialized == FALSE) )
   {
      UINT16 int_status_reg;
      int_status_reg = *(volatile UINT16*) KBR_INTERRUPT_STATUS_REG;
      *(volatile UINT16*) KBR_CLR_INTERRUPT_STATUS_REG |= int_status_reg; /* clear interrupt */
      kpd_acknowledge_key_pressed();
   }
   else
   {
      /* Mask keypad interrupt until key is released */
#if ((BOARD == 8) || (BOARD == 9) || (BOARD == 40) || (BOARD == 41))
       AI_MaskIT (ARMIO_MASKIT_KBD);
#elif ((CHIPSET == 12) || (CHIPSET == 15))
       F_INTH_DISABLE_ONE_IT(C_INTH_KEYBOARD_IT);
#else
       IQ_Mask(IQ_ARMIO);
#endif

#ifdef KPD_GPIO_ENABLED
      kpd_rf_gpio_test(*(volatile UINT16*) KBR_INTERRUPT_STATUS_REG);
#endif
      /* Activate HISR to process the key event */
      NU_Activate_HISR(&hisr_infos.hisr);
   }
}

/**
 * function: kpd_acknowledge_key_pressed
 */
void kpd_acknowledge_key_pressed(void)
{
   /* Unmask keypad interrupt */
   #if ((BOARD == 8) || (BOARD == 9) || (BOARD == 40) || (BOARD == 41))
     AI_UnmaskIT (ARMIO_MASKIT_KBD);
   #elif ((CHIPSET == 12) || (CHIPSET == 15))
#ifdef KPD_GPIO_ENABLED
     GPIO_CLEAR_OUTPUT(KPD_RF_GPIO);
#endif
     F_INTH_ENABLE_ONE_IT(C_INTH_KEYBOARD_IT);
   #else
     IQ_Unmask (IQ_ARMIO);
   #endif
}


/*
 * delay
 *
 * Wait a while to let bus settle
 * Magic value found by trial and error
 *
 */
static void delay(void)
{
   volatile int i;

   for (i=0;i<10;i++) ;
}

/**
 * function: kpd_scan_keypad
 */
T_KPD_PHYSICAL_KEY_ID kpd_scan_keypad(void)
{
   int row, col;
   volatile UINT16 rows;

   /* Activate all columns to find if any row is active */
   *(volatile UINT16*) KP_COL_OUT = KP_ALL_ON;
   delay();

   rows = (*(volatile UINT16*) KP_ROW_IN) & 0x1F;
   if (rows == KP_ALL_OFF)
      return KPD_PKEY_NULL;

   /* Deactivate all columns */
   *(volatile UINT16*) KP_COL_OUT = KP_ALL_OFF;

   /* Activate 1 column at a time */
   for (col = 0; col < KP_COLS; col++)
   {
      *(volatile UINT16*) KP_COL_OUT = (UINT32)(KP_ACTIVATE(col));
      delay();

      /* Find which row is active */
      rows = (*(volatile UINT16*) KP_ROW_IN) & 0x1F;

      if (rows != KP_ALL_OFF)
      {
         for (row = 0; row < KP_ROWS; row++)
         {
            /* first active row */
            if ( KP_IS_ACTIVE(rows,row))
            {
               /* Reactivate all columns */
               *(volatile UINT16*) KP_COL_OUT = KP_ALL_ON;
               /* DO NOT remove this comment. It allows to simply define the link physical layout
                  and physical key Id (for a new keypad) */
               //KPD_SEND_TRACE_PARAM("KPD: Keypad layout check ", keypad_layout[row][col], RV_TRACE_LEVEL_DEBUG_HIGH);
               return keypad_layout[row][col];
            }
         }
      }
   }

   /* No row was active - Reactivate all columns and return */
   *(volatile UINT16*) KP_COL_OUT = KP_ALL_ON;
   return KPD_PKEY_NULL;
}

#if ((CHIPSET == 12) || (CHIPSET == 15))
/**
 * function: kpd_map_reg_on_layout
 */
T_KPD_PHYSICAL_KEY_ID kpd_map_reg_on_layout(UINT8 key)
{
  UINT8 row,col;

  row = key / MAX_COL;
  col = key % MAX_COL;

  return keypad_layout[row][col];

}

#if (KEYPAD_RF_ISSUE_SW_WORKAROUND == 1 || KEYPAD_RF_ISSUE_SW_WORKAROUND == 2)

typedef enum
{
  KPD_MODIFY_NONE,
  KPD_MODIFY_DEBOUNCE_REG,
  KPD_MODIFY_PTV
}KPD_MODIFY_MODE;
#endif

#if (KEYPAD_RF_ISSUE_SW_WORKAROUND == 1 )
KPD_MODIFY_MODE previous_mode;
UINT32 previous_correction_ratio;

/* Modify the KPD timer values */
void kpd_timer_modify(KPD_CORRECTION_RATIO ratio,UINT32 frameNumber)
{
  UINT32 debounce_time, long_key_time, timeout_time, temp_count, local_ratio;
  INT32 ptv_value;
  UINT16 keypad_state;

  keypad_state = ((*(volatile UINT16*) KBR_STATE_MACHINE_STATUS) & 0x0f);

  debounce_time = (*(volatile UINT16*) KBR_DEBOUNCING_TIME) & 0x3f;
  local_ratio = KPD_DEBOUNCING_TIME / debounce_time;

  if( keypad_state == 2 || keypad_state == 3 || keypad_state == 4)
  {
	ptv_value = KPD_CLOCK_DIVIDER;

	if( ratio > 1)
	{
	    if( ratio % 2)
     	    {
         	ratio += 1;
     	    }
     	    if( !ratio )
		ratio = 2;

     	    for(temp_count = 0; ratio > 1 && temp_count < 7; temp_count++)
     	    {
		ratio /= 2;
     	    }

	    ptv_value -= temp_count;
	}

	if( local_ratio > 1)
	{
		if( local_ratio % 2)
     		{
         		local_ratio += 1;
     		}
     		if( !local_ratio )
			local_ratio = 2;

     		for(temp_count = 0; local_ratio > 1 && temp_count < 7; temp_count++)
     		{
			local_ratio /= 2;
     		}

		ptv_value += temp_count;
	}

	if( ptv_value < 0)
		ptv_value = 0;

	/* Change the PTV value */
     	SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 2, 3, ptv_value);
  }
  else
  {
    ptv_value = ((*(volatile UINT16*) KBR_CTRL_REG) &  0x1c) >> 2;

	if( ptv_value != KPD_CLOCK_DIVIDER)
	{
		/* Change the PTV value */
     		SetGroupBits16(*(volatile unsigned short *)(KBR_CTRL_REG), 2, 3, KPD_CLOCK_DIVIDER);
	}

	if( ratio > 0 )
	{
		kpd_env_ctrl_blk->long_time = kpd_configured_long_key_time/ratio;
		kpd_env_ctrl_blk->repeat_time =kpd_configured_repeat_key_time/ratio;
		(*(volatile UINT16*) KBR_DEBOUNCING_TIME) = configured_debounce_value / ratio;
		(*(volatile UINT16*) KBR_LONG_KEY_TIME) = kpd_env_ctrl_blk->long_time;
		(*(volatile UINT16*)KBR_TIME_OUT) = configured_timeout_value / ratio;
	}
  }

}
#endif

#if (KEYPAD_RF_ISSUE_SW_WORKAROUND == 2)
#define FRAME_NUMBER_MAX 42432

KPD_CORRECTION_RATIO currently_applied_correction_ratio = 0;
UINT32 TimeCorrection = 0;
UINT32 KeyPressFrameNumber = FRAME_NUMBER_MAX;

#endif

#if (KEYPAD_RF_ISSUE_SW_WORKAROUND == 1 || KEYPAD_RF_ISSUE_SW_WORKAROUND == 2)
void kpd_state_probe(UINT32 frameNumber)
{
#if (KEYPAD_RF_ISSUE_SW_WORKAROUND == 2)
  UINT16 keypad_state;
  keypad_state = ((*(volatile UINT16*) KBR_STATE_MACHINE_STATUS) & 0x0f);
  if((keypad_state == 2 ||  keypad_state == 3 || keypad_state == 4) && (TimeCorrection==FALSE))
  {
      KeyPressFrameNumber = frameNumber;
	  TimeCorrection=TRUE;
  }
  else if (((keypad_state != 2) &&  (keypad_state != 3) && (keypad_state != 4)) && (KeyPressFrameNumber!=FRAME_NUMBER_MAX))
	  kpd_timer_modify(currently_applied_correction_ratio, frameNumber);

#endif
}
#endif

#if (KEYPAD_RF_ISSUE_SW_WORKAROUND == 2)
void kpd_timer_modify(KPD_CORRECTION_RATIO ratio, UINT32 frameNumber)
{
  UINT16 keypad_state, timeExpired;
  keypad_state = ((*(volatile UINT16*) KBR_STATE_MACHINE_STATUS) & 0x0f);

  if(keypad_state == 2 ||  keypad_state == 3 || keypad_state == 4)
  {
  	if (TimeCorrection==TRUE)
		{
		if (frameNumber < KeyPressFrameNumber)
			timeExpired = ((FRAME_NUMBER_MAX - KeyPressFrameNumber) + frameNumber) * 5 + 2;
    	else
			timeExpired = (frameNumber - KeyPressFrameNumber) * 5 + 2;
  		}
	else
		{

		timeExpired=0;
		}


  		KeyPressFrameNumber = frameNumber;
    	TimeCorrection=TRUE;

  }
  else
  {

  		KeyPressFrameNumber = FRAME_NUMBER_MAX;
    	TimeCorrection=FALSE;
		timeExpired=0;
  }

  if( ratio > 0 )
	{
		(*(volatile UINT16*) KBR_DEBOUNCING_TIME) = (KPD_DEBOUNCING_TIME - timeExpired) / ratio;
		(*(volatile UINT16*) KBR_LONG_KEY_TIME) = (kpd_configured_long_key_time - timeExpired) / ratio;
		(*(volatile UINT16*)KBR_TIME_OUT) = (KPD_DEFAULT_TIME_OUT_TIME - timeExpired) / ratio;
		kpd_env_ctrl_blk->long_time =(kpd_configured_long_key_time - timeExpired) / ratio;
		kpd_env_ctrl_blk->repeat_time =(kpd_configured_repeat_key_time - timeExpired) / ratio;
	}

  currently_applied_correction_ratio=ratio;
}
#endif

#define KPD_MISS_WA

#ifdef KPD_MISS_WA


T_RV_RET kpd_retrieve_key_status(  T_KPD_VIRTUAL_KEY_ID key_id,
                                             T_KPD_MODE mode,
                                             T_KPD_KEY_STATE* state)
{
  INT8 pos;
  UINT8     nb_keys_decoded=0;
  UINT8     i;
  UINT16    key_reg[4];
  UINT8     count=0;
  BOOLEAN   done = FALSE;
  INT8 received_keys_p[KPD_MAX_DETECTABLE]={0,0};
  if (kpd_retrieve_virtual_key_position(key_id,mode,&pos)==RV_INVALID_PARAMETER)
	return RV_INVALID_PARAMETER;

  key_reg[0] = *(volatile UINT16*) KBR_FULL_CODE_15_0;
  key_reg[1] = *(volatile UINT16*) KBR_FULL_CODE_31_16;
  key_reg[2] = *(volatile UINT16*) KBR_FULL_CODE_47_32;
  key_reg[3] = *(volatile UINT16*) KBR_FULL_CODE_63_48;

  for(i=0; i<4; i++)
  {
    while(!done)
    {
      /* Are there bits set in the register ? */
      if ((key_reg[i] > 0) && (nb_keys_decoded < KPD_MAX_DETECTABLE))
      {
        if(key_reg[i] & 0x0001)
        {
          /* save key value */
          received_keys_p[nb_keys_decoded] = kpd_map_reg_on_layout(count + (i * 16));
          nb_keys_decoded++;
          count++;
        }
        else
        {
          count++;
        }
        /* shift 1 right */
        key_reg[i] >>= 1;
      }
      else
      {
        /* No more bits set in this register */
        done = TRUE;
      }
    }
    /* Reset values */
    count = 0;
    done = FALSE;
  }

  *state= KPD_KEY_RELEASED;
  for (i=0;i<nb_keys_decoded;i++)
	if (received_keys_p[i]==pos)
		{
		*state=KPD_KEY_PRESSED;
		break;
		}

return RV_OK;
}

#endif

#if (L1_RF_KBD_FIX == 1)
#define KPD_REPEAT_TIME_TICKS (kpd_configured_repeat_key_time/4.6)
void  kpd_long_key_sw_handler()
{
  T_KPD_KEY_EVENT_INTERRUPT_MSG *msg_key_interrupt, msg_key_data;
  INT8 key_pressed_id[KPD_MAX_DETECTABLE];
  UINT8 nb_keys_pressed;
  UINT8 i,j,long_key_status=FALSE;
  T_RVF_MB_STATUS mb_status;

  msg_key_data.full_code_15_0  = *(volatile UINT16*) KBR_FULL_CODE_15_0;
  msg_key_data.full_code_31_16 = *(volatile UINT16*) KBR_FULL_CODE_31_16;
  msg_key_data.full_code_47_32 = *(volatile UINT16*) KBR_FULL_CODE_47_32;
  msg_key_data.full_code_63_48 = *(volatile UINT16*) KBR_FULL_CODE_63_48;
  nb_keys_pressed = kpd_decode_key_registers((T_RV_HDR *)&msg_key_data, key_pressed_id);
  if (nb_keys_pressed)
  	{
  	for (i=0;i<capture_nb_keys;i++)
		for(j=0;j<nb_keys_pressed;j++)
			if (key_pressed_id[j]==present_key_capture[i])
				{
				long_key_status=TRUE;
				break;
				}
  	}
  if (long_key_status==TRUE)
  	{
     mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEY_EVENT_INTERRUPT_MSG), (void **) &msg_key_interrupt);
     if (mb_status != RVF_RED) /* Memory allocation success */
  	  {
       msg_key_interrupt->full_code_15_0  =   msg_key_data.full_code_15_0;
	   msg_key_interrupt->full_code_31_16 =   msg_key_data.full_code_31_16;
       msg_key_interrupt->full_code_47_32 =   msg_key_data.full_code_47_32;
       msg_key_interrupt->full_code_63_48 =   msg_key_data.full_code_63_48;
		   msg_key_interrupt->hdr.msg_id = KPD_LONG_KEY_INTERRUPT_MSG;
	       if(repeat_active == FALSE)
    	    {
        	  repeat_active = TRUE;
 	          rvf_start_timer(RVF_TIMER_0,(UINT32) KPD_REPEAT_TIME_TICKS,TRUE);
	       	}

	      /* Send message to the keypad task */
	       rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_key_interrupt);
       }
  	}
  else
  	{
  	      KPD_SEND_TRACE("KPD_REPEAT_TIME_TICKS", RV_TRACE_LEVEL_ERROR);
  	}
}

#ifdef KPD_GPIO_ENABLED
void kpd_rf_gpio_test(UINT8 int_status_reg)
{

if (int_status_reg & 0x0001)
{
	 if ( ((*(volatile UINT16*) KBR_FULL_CODE_15_0)!=  NO_KEY) ||
	 	((*(volatile UINT16*) KBR_FULL_CODE_31_16)!=  NO_KEY) ||
	 	((*(volatile UINT16*) KBR_FULL_CODE_47_32)!=  NO_KEY) ||
	 	((*(volatile UINT16*) KBR_FULL_CODE_63_48)!=  NO_KEY)
	 	)
     	{
       	CONF_GPIO_19=0x0000; // configure it as GPIO line
		GPIO_SET_OUTPUT(KPD_RF_GPIO);
     	}
}
}
#endif

#endif
#endif /* CHIPSET == 12 */


/*@}*/



