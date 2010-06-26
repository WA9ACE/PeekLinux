
#include "fm_types.h"
#include "fm_utils.h"
#include "btl_config.h"

#if 0


TIFM_U16 TIFM_LittleEtoHost16(const TIFM_U8* ptr)
{
    return (TIFM_U16)( ((TIFM_U16) *(ptr+1) << 8) | ((TIFM_U16) *ptr) ); 
}

TIFM_U16 TIFM_BigEtoHost16(const TIFM_U8* ptr)
{
    return (TIFM_U16)( ((TIFM_U16) *ptr << 8) | ((TIFM_U16) *(ptr+1)) ); 
}

void TIFM_StoreLittleE16(TIFM_U8 *buff, TIFM_U16 le_value) 
{
   buff[1] = (TIFM_U8)(le_value>>8);
   buff[0] = (TIFM_U8)le_value;
}

void TIFM_StoreBigE16(TIFM_U8 *buff, TIFM_U16 be_value) 
{
   buff[0] = (TIFM_U8)(be_value>>8);
   buff[1] = (TIFM_U8)be_value;
}


void _TIFM_InsertTailList(TIFM_ListNode* head, TIFM_ListNode* Node)
{
  Node->NextNode = head;
  Node->PrevNode = head->PrevNode;
  head->PrevNode->NextNode = Node;
  head->PrevNode = Node;
  TIFM_Assert(TIFM_IsNodeConnected(Node));

}


void _TIFM_InsertHeadList(TIFM_ListNode* head, TIFM_ListNode* Node)
{
  Node->NextNode = head->NextNode;
  Node->PrevNode = head;
  head->NextNode->PrevNode = Node;
  head->NextNode = Node;
  TIFM_Assert(TIFM_IsNodeConnected(Node));

}


TIFM_ListNode* _TIFM_RemoveHeadList(TIFM_ListNode* head)
{
  TIFM_ListNode* first;

  first = head->NextNode;
  first->NextNode->PrevNode = head;
  head->NextNode = first->NextNode;
  TIFM_Assert(TIFM_IsListCircular(head));
  return(first);

}


void TIFM_RemoveNodeList(TIFM_ListNode* Node)
{
  TIFM_Assert(TIFM_IsListCircular(Node));
  Node->PrevNode->NextNode = Node->NextNode;
  Node->NextNode->PrevNode = Node->PrevNode;
  TIFM_Assert(TIFM_IsListCircular(Node->PrevNode));
  TIFM_InitializeListNode(Node);

}


TIFM_BOOL TIFM_IsNodeOnList(TIFM_ListNode* head, TIFM_ListNode* node)
{
  TIFM_ListNode* tmpNode;

  TIFM_Assert(TIFM_IsListCircular(head));
  tmpNode = TIFM_GetHeadList(head);

  while (tmpNode != head)
  {
    if (tmpNode == node)
      return(TIFM_TRUE);

    tmpNode = tmpNode->NextNode;
  }
  return(TIFM_FALSE);

}


void TIFM_MoveList(TIFM_ListNode* dest, TIFM_ListNode* src)
{
    TIFM_Assert(TIFM_IsListCircular(src));
    dest->NextNode = src->NextNode;
    dest->PrevNode = src->PrevNode;
    src->NextNode->PrevNode = dest;
    src->PrevNode->NextNode = dest;
    TIFM_InitializeListHead(src);
}


TIFM_BOOL TIFM_IsListCircular(TIFM_ListNode* list)
{
  TIFM_ListNode* tmp = list;

  if (!TIFM_IsNodeConnected(list))
    return(TIFM_FALSE);

  for (tmp = tmp->NextNode; tmp != list; tmp = tmp->NextNode)
    if (!TIFM_IsNodeConnected(tmp))
      return(TIFM_FALSE);

  return(TIFM_TRUE);
}

#endif /*BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/

