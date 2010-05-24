/*
 +----------------------------------------------------------------------------
 |  (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 +----------------------------------------------------------------------------
 |
 |  File       : prf_api.h
 |
 +----------------------------------------------------------------------------
 |
 |  Purpose    : Performance Profiler API
 |
 |  Author     : Sebastian Niciarz
 |
 |  Version    : 0.1
 |
 |  Date       : Modification
 |  ---------------------------------------------
 |  18/11/2003 : Created
 |
 +----------------------------------------------------------------------------
 */

#ifndef PRF_API_
#define PRF_API_

/* Enables test code for function level profiler. 
   NOTE: Should be disabled for normal use. */
#define PRF_TEST_FP                                 (0)

/* Enables compression of profiler data */
#define PRF_COMPRESS                                (1)

/* Enables profiler calibration routines
   NOTE: Should be disabled for normal use. */
#define PRF_CALIBRATION                             (0)


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_Init
 |
 |  Purpose    : Initialize Performance Profiler module: 
 |               -> initializes internal data structures
 |               -> establishes communication with host application
 |               -> initializes Profiler sub-modules
 |
 +----------------------------------------------------------------------------
 */
void prf_Init(void);

/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogPCSample
 |
 |  Purpose    : Logs current program counter value. 
 |                  Called from PC sampling timer ISR (FIQ)
 |
 +----------------------------------------------------------------------------
 */ 
void prf_LogPCSample(register unsigned long ProgramCounter);

/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogFunctionEntry
 |
 |  Purpose    : Logs entry to a profiled function.
 |                  Called at enrtrance to every function enabled for 
 |                  profiling.
 |
 +----------------------------------------------------------------------------
 */
void prf_LogFunctionEntry(unsigned long FunctionPtr);

/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogFunctionExit
 |
 |  Purpose    : Logs exit from a profiled function.
 |                  Called at exit from every function enabled for 
 |                  profiling.
 |
 +----------------------------------------------------------------------------
 */
void prf_LogFunctionExit(unsigned long FunctionPtr);

/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogPointOfInterest
 |
 |  Purpose    : Logs user-defined event.
 |
 +----------------------------------------------------------------------------
 */
void prf_LogPointOfInterest(const char* PoIDescr);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogIsrStart
 |
 |  Purpose    : Logs entry to ISR
 |
 +----------------------------------------------------------------------------
 */
void prf_LogIsrEntry(unsigned long IrqSource);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogIsrExit
 |
 |  Purpose    : Logs exit from ISR
 |
 +----------------------------------------------------------------------------
 */
void prf_LogIsrExit(void);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogTaskCreate
 |
 |  Purpose    : Logs "new OS task created" event.
 |                  Called directly from RTOS kernel.
 |
 +----------------------------------------------------------------------------
 */ 
 void prf_LogTaskCreate(void* Task, const char* Name);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogTaskDelete
 |
 |  Purpose    : Logs "OS task deleted" event.
 |                  Called directly from RTOS kernel.
 |
 +----------------------------------------------------------------------------
 */
 void prf_LogTaskDelete(void* Task);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogTaskSwitch
 |
 |  Purpose    : Logs "running OS task switch" event.
 |                  Called directly from RTOS kernel.
 |
 +----------------------------------------------------------------------------
 */
 void prf_LogTaskSwitch(void* Task);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogEntity Create
 |
 |  Purpose    : Logs "new Frame entity created" event.
 |                  Called from GPF.
 |
 +----------------------------------------------------------------------------
 */
void prf_LogEntityCreate(void* Entity, const char* Name);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogEntityDelete
 |
 |  Purpose    : Logs "Frame entity deleted" event.
 |                  Called from GPF.
 |
 +----------------------------------------------------------------------------
 */
void prf_LogEntityDelete(void* Entity);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogEntitySwitch
 |
 |  Purpose    : Logs "running Frame entity switch" event.
 |
 |
 +----------------------------------------------------------------------------
 */
void prf_LogEntitySwitch(void* Entity);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogCPULoadL1S
 |
 |  Purpose    : Logs current CPU load for L1 Synchronous part.
 |                  Called from L1S.
 |
 +----------------------------------------------------------------------------
 */
void prf_LogCPULoadL1S(unsigned char Load);



/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogCPULoadTotal
 |
 |  Purpose    : Logs current CPU load for total GSM/GPRS subsystem
 |
 +----------------------------------------------------------------------------
 */
void prf_LogCPULoadTotal(unsigned char Load);


/*
 +----------------------------------------------------------------------------
 |
 |  Function   : prf_LogFNSwitch
 |
 |  Purpose    : Logs TDMA frame number.
 |                  Called from L1 when FN changes.
 |
 +----------------------------------------------------------------------------
 */
void prf_LogFNSwitch(unsigned long FrameNumber);

#endif /* PRF_API_ */


