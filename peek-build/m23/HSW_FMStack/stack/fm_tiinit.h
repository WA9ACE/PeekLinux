#ifndef __FM_TIINIT_H
#define __FM_TIINIT_H

#include "fm_types.h"
#include "fm_utils.h"
#include "bthal_utils.h"
#include "fms_api.h"

/***********************************
 * Define the interface of FM Script
 ***********************************/

#define FM_ACTION_SEND_COMMAND				1    /* Send out raw data (as is)	*/
#define FM_ACTION_WAIT_EVENT				2    /* Wait for data				*/


void fmGetMemoryInitScript(const char * version, TIFM_U16 * memoryInitScriptSize, char ** initScript);
unsigned long FM_bts_GetNextAction(unsigned char* pActionBuffer, unsigned long nMaxBuffer, unsigned short* pnType);
void FM_bts_UnloadScript(void);
FmStatus FM_bts_LoadFmScript(const char* filename);


#endif /* __FM_TIINIT_H	*/
