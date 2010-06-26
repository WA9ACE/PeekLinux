#ifndef GIPB_H
#define GIPB_H

/* FUNCTION PROTOTYPES
 * Functions called by MMI */
#ifndef ATBPB_H
#include "ATBPb.h"
#endif

T_MFW	GI_pb_GetPhonebook(UBYTE *intphonebook);
T_MFW	GI_pb_SetPhonebook(UBYTE intphonebook);
T_MFW	GI_pb_Initialise(void);
T_MFW	GI_pb_Exit(void);
void	GI_pb_Info(T_MFW_PHB_STATUS *status);
void	GI_pb_Status(UBYTE *stat);
T_MFW	GI_pb_WriteRec(T_MFW_PHB_ENTRY *entry);
T_MFW	GI_pb_DeleteRec(UBYTE index);
T_MFW 	GI_pb_Find(UBYTE mode, UBYTE *index, UBYTE num_recs,  T_MFW_PHB_TEXT *search_pattern, T_MFW_PHB_LIST *entries);
T_MFW	GI_pb_ReadRecList(UBYTE mode, UBYTE index, UBYTE num_recs, T_MFW_PHB_LIST *entries);
UBYTE	GI_pb_ConvIndex(T_PB_INDEX index_type, UBYTE index, T_PB_INDEX new_index_type);
#endif
