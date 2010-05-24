#ifndef CP_NUCLEUS_H
#define CP_NUCLEUS_H



#include "nucleus.h"            /* Include the base file */



#ifdef __cplusplus
extern  "C" {                               /* C declarations in C++     */
#endif  /* __cplusplus */


#undef PLUS_1_11
#undef PLUS_1_13
#undef PLUS_VERSION_COMP

#undef NU_System_Clock_Frequency
#undef NU_PLUS_Tick_Rate
#undef NU_PLUS_Ticks_Per_Second
#undef NU_HW_Ticks_Per_Second
#undef NU_HW_Ticks_Per_SW_Tick
#undef NU_COUNT_DOWN
#undef NU_Retrieve_Hardware_Clock


#ifndef         NU_SOURCE_FILE

#define         NU_Adjust_Active_Timers         TMF_Adjust_Active_Timers
VOID            NU_Adjust_Active_Timers(UNSIGNED adj_time);

#endif  /* NU_SOURCE_FILE */

#ifdef  __cplusplus
}                                           /* End of C declarations     */
#endif  /* __cplusplus */

#endif    /* CP_NUCLEUS_H */



