/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * NU_MAIN.H
 *
 *        Filename nu_main.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

typedef long        timespec;
typedef timespec    SDL_Duration;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Signal implentation
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

typedef struct xTimerHeaderStruct *xTimerHeader;
typedef struct xTimerHeaderStruct {
      int                SignalCode;
      xTimerHeader       Pre, Suc;
      int                Sender;
      void              *SigP;
      SDL_Duration       TimerTime;
} xTimerHeaderRec;

#define MAX(A,B) ( (A) > (B) ? (A) : (B) )

#define PARTITION_POOL(SIZE) \
        SIZE <= 100 ? SIZE <= 52 ? &pool_52 : &pool_100 : \
        SIZE <= 400 ? &pool_400 : &pool_1600

#define UWORD32_PER_PTR ( sizeof ( UWORD32 ) / sizeof ( void * ) )
#if (OP_L1_STANDALONE == 1)
#define MEM_BUF_SIZE(SIG_PAR_SIZE) \
        ( SIG_PAR_SIZE + MAX ( sizeof ( xSignalHeaderRec ), sizeof ( xTimerHeaderRec ) ) )
#endif //omaps00090550

#ifdef NU_MAIN_C
   STATUS             status;
   NU_MEMORY_POOL     System_Memory;
   #if (L1_GTT == 1)
     NU_MEMORY_POOL     L1GTT_Memory;   
   #endif
   NU_QUEUE           L1C1_queue;
   NU_QUEUE           RRM1_queue;
   NU_QUEUE           DLL1_queue;
   #if (AUDIO_TASK == 1)
     #if (L1_AUDIO_BACKGROUND_TASK)
       NU_QUEUE       SRBACK_queue;
     #endif
   #endif
   #if (DSP_BACKGROUND_TASKS == 1)
   NU_QUEUE           BACKGROUND_TASK_queue;   
   #endif
   #if (L1_GTT == 1)
   NU_EVENT_GROUP     L1GTTBACK_event;
   #endif

   #if (L1_MIDI == 1)
     NU_EVENT_GROUP  L1MIDIBACK_event;
   #endif

   NU_PARTITION_POOL  pool_52;
   NU_PARTITION_POOL  pool_100;
   NU_PARTITION_POOL  pool_400;
   NU_PARTITION_POOL  pool_1600;
   NU_TIMER           L3_timer;
   NU_TIMER           ADC_timer;

   #if (TRACE_TYPE == 1) || (TRACE_TYPE==7) || TESTMODE
     NU_QUEUE           RVT_queue;
     NU_PARTITION_POOL  rvt_pool;
   #endif
#else
  extern STATUS             status;
  extern NU_MEMORY_POOL     System_Memory;
  #if (L1_GTT == 1)
    extern NU_MEMORY_POOL     L1GTT_Memory;   
  #endif
  extern NU_QUEUE           L1C1_queue;
  extern NU_QUEUE           RRM1_queue;
  extern NU_QUEUE           DLL1_queue;
  #if (AUDIO_TASK == 1)
    #if (L1_AUDIO_BACKGROUND_TASK)
      extern NU_QUEUE       SRBACK_queue;
    #endif
  #endif
  #if (DSP_BACKGROUND_TASKS == 1)
  extern NU_QUEUE           BACKGROUND_TASK_queue;   
  #endif
  #if (L1_GTT == 1)
  extern NU_EVENT_GROUP     L1GTTBACK_event;
  #endif
  extern NU_PARTITION_POOL  pool_52;
  extern NU_PARTITION_POOL  pool_100;
  extern NU_PARTITION_POOL  pool_400;
  extern NU_PARTITION_POOL  pool_1600;
  extern NU_TIMER           L3_timer;
  extern NU_TIMER           ADC_timer;

   #if (TRACE_TYPE == 1) || (TRACE_TYPE==7) || TESTMODE
     extern NU_QUEUE           RVT_queue;
     extern NU_PARTITION_POOL  rvt_pool;
   #endif
 #endif
