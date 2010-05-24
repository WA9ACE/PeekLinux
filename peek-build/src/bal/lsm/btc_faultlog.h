#ifndef BTC_FAULT_LOG_H
#define  BTC_FAULT_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

				   ERRO FAULT LOG OPERATION


GENERAL DESCRIPTION
  Save fault log to file system.

Copyright (c) 2007	   by BTC Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

						EDIT HISTORY FOR MODULE

when	      who	     what, where, why
---------   ------    ------------------------------------------------------------
12-21-07      WBY	       Initial version.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
typedef enum 
{
    SYS_FAULT = 0,
    PRE_FAULT = 1,
    HOTSTART  = 2,
    MAX_FAULT
} ErrorFaultType;

typedef enum 
{
    FAULT_SAVE_SUCCESS,
    FAULT_SAVE_ERROR
} FaultSaveStatus;


FaultSaveStatus OpenErrorFaultSaveToFs( BalFsiHandleT  *Filehandle);

FaultSaveStatus WritePreHeadErrorFaultSaveToFs( BalFsiHandleT  Filehandle,uint16 SerialNumber, uint16 FullFlag);
FaultSaveStatus WriteHotHeadErrorFaultSaveToFs( BalFsiHandleT  Filehandle,uint16 HotPowerOnCount);
FaultSaveStatus WriteUnPreHeadErrorFaultSaveToFs( BalFsiHandleT  Filehandle,uint8 UnpreType,
                                                                                                    uint32 Current_R14,uint32 Current_PC);
FaultSaveStatus CloseErrorFaultSaveToFs( BalFsiHandleT  Filehandle);




#ifdef __cplusplus
}
#endif

#endif /* BTC_FAULT_LOG_H */
