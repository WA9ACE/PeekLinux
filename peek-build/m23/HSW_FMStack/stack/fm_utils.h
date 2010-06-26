#ifndef __FM_UTILS_H
#define __FM_UTILS_H

#include "fm_types.h"
#include "bthal_utils.h"
#include "bthal_log.h"
#include "bthal_log_modules.h"


#define TIFM_Report(s) 		BTHAL_LOG_INFO(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_FMSTACK, s)
#define TIFM_Assert(exp)  (((exp) != 0) ? (void)0 : BTHAL_UTILS_Assert(#exp,__FILE__,(TIFM_U16)__LINE__))


/* Little Endian to Host integer format conversion functions */
TIFM_U16 TIFM_LittleEtoHost16(const TIFM_U8 *le_value);

/* Big Endian to Host integer format conversion functions */
TIFM_U16 TIFM_BigEtoHost16(const TIFM_U8* be_ptr);

/* Store value into a buffer in Little Endian integer format */
void TIFM_StoreLittleE16(TIFM_U8 *buff, TIFM_U16 le_value);

/* Store value into a buffer in Big Endian integer format */
void TIFM_StoreBigE16(TIFM_U8 *ptr, TIFM_U16 be_value);



/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


typedef struct  _TIFM_ListNode 
{
    struct _TIFM_ListNode *NextNode;
    struct _TIFM_ListNode *PrevNode;

} TIFM_ListNode;


/****************************************************************************
 *
 * Functions and Macros Reference.
 *
 ****************************************************************************/


#define TIFM_InitializeListHead(ListHead) (\
    (ListHead)->NextNode = (ListHead)->PrevNode = (ListHead) )

#define TIFM_InitializeListNode(Node) (\
    (Node)->NextNode = (Node)->PrevNode = 0 )

#define TIFM_IsNodeAvailable(Node) (\
    ((Node)->NextNode == 0))

#define TIFM_IsListEmpty(ListHead) (\
    ((ListHead)->NextNode == (ListHead)))

#define TIFM_GetHeadList(ListHead) (ListHead)->NextNode

#define TIFM_IsNodeConnected(n) (((n)->PrevNode->NextNode == (n)) && ((n)->NextNode->PrevNode == (n)))

void _TIFM_InsertTailList(TIFM_ListNode* head, TIFM_ListNode* Node);
#define TIFM_InsertTailList(x, y) (TIFM_Assert(TIFM_IsListCircular(x)), \
                            _TIFM_InsertTailList(x, y), \
                            TIFM_Assert(TIFM_IsListCircular(x)))

void _TIFM_InsertHeadList(TIFM_ListNode* head, TIFM_ListNode* Node);
#define TIFM_InsertHeadList(x, y) (TIFM_Assert(TIFM_IsListCircular(x)), \
                            _TIFM_InsertHeadList(x, y), \
                            TIFM_Assert(TIFM_IsListCircular(x)))

TIFM_ListNode* _TIFM_RemoveHeadList(TIFM_ListNode* head);
#define TIFM_RemoveHeadList(x) (TIFM_Assert(TIFM_IsListCircular(x)), \
                            _TIFM_RemoveHeadList(x))

void TIFM_RemoveNodeList(TIFM_ListNode* Node);
TIFM_BOOL TIFM_IsNodeOnList(TIFM_ListNode* head, TIFM_ListNode* node);
TIFM_BOOL TIFM_IsListCircular(TIFM_ListNode* list);
void TIFM_MoveList(TIFM_ListNode* dest, TIFM_ListNode* src);

#endif	/* __FM_UTILS_H */

