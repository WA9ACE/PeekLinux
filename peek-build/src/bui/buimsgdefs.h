

#ifndef UIMAILDEFS_H
#define UIMAILDEFS_H 
/******************************************************************************
* 
*
******************************************************************************/
enum MailCatT {
  CDMA_CAT            = 1,
  KEY_CAT             = 2,
  ACCESSORY_CAT       = 3,
  WINDOW_CAT          = 4,
  ANNOUNCIATOR_CAT    = 5,
  APPLICATION_CAT     = 6,
  SCHEDULER_CAT       = 7,
  FILE_CAT            = 8,
  TIMER_CAT           = 9,
  GENERAL_CAT
};

enum MailMaskBitT {
  CDMA_BIT            = 1 << (CDMA_CAT - 1),
  KEY_BIT             = 1 << (KEY_CAT - 1),
  ACCESSORY_BIT       = 1 << (ACCESSORY_CAT - 1),
  WINDOW_BIT          = 1 << (WINDOW_CAT - 1),
  ANNOUNCIATOR_BIT    = 1 << (ANNOUNCIATOR_CAT - 1),
  APPLICATION_BIT     = 1 << (APPLICATION_CAT - 1),
  SCHEDULER_BIT       = 1 << (SCHEDULER_CAT - 1),
  FILE_BIT            = 1 << (FILE_CAT - 1),
  TIMER_BIT           = 1 << (TIMER_CAT - 1),
  GENERAL_BIT         = 1 << (GENERAL_CAT - 1),
  ALL_BIT             = 0xFF
};

#define CAT_MASK          0x0FF00000
#define CAT_BIT_LOCATION  20
#define MAIL_CAT(CatId) \
  (((CatId) << CAT_BIT_LOCATION) & CAT_MASK)
#define GET_MAIL_CAT(MsgId) \
  (((MsgId) & CAT_MASK) >> CAT_BIT_LOCATION)

#define ID_MASK 0x000FFFFF
#define GET_MAIL_ID(MsgId)\
  ((MsgId) & ID_MASK)

#define GROUP_BIT_LOCATION 12
#define GROUP_MASK 0x000FF000
#define MSGID_OF_GROUP_MASK 0x00000FFF
#define APPCAT(group,id) \
  (MAIL_CAT(APPLICATION_CAT) | (((group)<<GROUP_BIT_LOCATION) & GROUP_MASK) | ((id) & MSGID_OF_GROUP_MASK))
#define GET_MAIL_GROUP(MsgId) \
(((MsgId) & GROUP_MASK) >> GROUP_BIT_LOCATION)

#define GET_MSGID_OF_GROUP(MsgId)\
((MsgId) & MSGID_OF_GROUP_MASK)

#define APPMSG_PRIVATE          100




#endif

