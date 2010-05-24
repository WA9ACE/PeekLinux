#ifndef _SMS_TUNE_H
#define _SMS_TUNE_H
/*************************************************************************
*
*
************************************************************************/


/* ---------------------------------------------------------------------
 * Name: SMS_MAX_NUM_ASE
 *
 * Description:
 *   This defines the maximum number of ASEs that can be connected to the
 *   Transport Layer.
 *
 * Restrictions:
 *  This number must be at most the number of Tele-service Ids
 *   
 * Default Value:
 *   5
 *   
 */
#define SMS_MAX_NUM_ASE 5

/* ---------------------------------------------------------------------
 * Name: SMS_TL_CONFIRM
 *
 * Description:
 *   This determines how unspecified QoS conditions are handled. If 1, the
 *   Transport Layer will use confirmed Data Link services whenever the
 *   QoS requirements are unspecified (e.g. sending an ACK). If 0, the
 *   non-confirmed service will be used.
 *
 * Restrictions:
 *  This number must be 0 or 1
 *   
 * Default Value:
 *   0
 *   
 */
#define SMS_TL_CONFIRM 1 

/* ---------------------------------------------------------------------
 * Name: SMS_RL_CONFIRM
 *
 * Description:
 *    Determines if the relay layer will send a request to the lower
 *    layers with confirmed service.
 *
 * Restrictions:
 *  This number must be 0 or 1
 *   
 * Default Value:
 *   1
 *   
 */
#define SMS_RL_CONFIRM TRUE 

/* ---------------------------------------------------------------------
 * Name: SMS_TC_ONLY
 *
 * Description:
 *    Indicates if the SMS Relay Layer should request the outgoing
 *    message should only be sent on a CDMA traffic channel
 *
 * Restrictions:
 *  This number must be 0 or 1
 *   
 * Default Value:
 *   0 
 *   
 */
/* THIS IS IN SCC_TUNE.H => #define SMS_TC_ONLY FALSE */


/* ---------------------------------------------------------------------
 * Name: SMS_TL_TIMEOUT
 *
 * Description:
 *   This specifies how much time (in msec's) to wait for a Transport Layer
 *   acknowledgement from the message center before declaring the transaction
 *   failed.
 *
 * Restrictions:
 *  This number must be >0 and <=65535
 *   
 * Default Value:
 *   2000      (2 seconds)
 *   
 */
#define SMS_TL_TIMEOUT 8000

/* ---------------------------------------------------------------------
 * Name: SMS_RL_QUEUE_SIZE
 *
 * Description:
 *   This determines how many message buffers of size SMS_RL_BUFFER_SIZE
 *   that the Relay Layer will allocate for transmitting SMS messages.
 *
 * Restrictions:
 *  This number must be >0 and <= 255
 *   
 * Default Value:
 *   5 
 *   
 */
#define SMS_RL_QUEUE_SIZE 5 

/* ---------------------------------------------------------------------
 * Name: SMS_RL_BUFFER_SIZE
 *
 * Description:
 *   This determines how large the Relay Layer transmit buffers are in bytes.
 *
 * Restrictions:
 *  This number must be >0 and <= 255
 *   
 * Default Value:
 *   254 
 *   
 */
#define SMS_RL_BUFFER_SIZE 254

/* ---------------------------------------------------------------------
 * Name: SMS_TL_MAX_TRANS
 *
 * Description:
 *   This determines how many concurrent teleservice messages
 *   can be awaiting a transport layer acknowledgement 
 *
 * Restrictions:
 *  This number must be >0 and <= 255
 *   
 * Default Value:
 *   5
 *   
 */
#define SMS_TL_MAX_TRANS 5

/* ---------------------------------------------------------------------
 *
 * Name: GLOBAL_REPLY_SEQ
 *
 * Description:
 *      This macro sets the initial value for the GlobalReplySeq
 *      value used for all transport layer messages within the SMS engine.
 *
 * Restrictions:
 *      This number must be within 0 <= GLOBAL_REPLY_SEQ <= 65535
 *
 * Default Value:
 *  0
 *
 */
#define GLOBAL_REPLY_SEQ 0

/* ---------------------------------------------------------------------
 *
 * Name: SMS_MAX_ADDRESS_CHARS
 *
 * Description:
 *    Limit on the number of bytes used for the chari fields of
 *    the SMS_ADDR structure
 *
 * Restrictions:
 *      0 < x < 255
 *
 * Default Value:
 *    32 (same as max for MMI)
 *
 */
#define SMS_MAX_ADDRESS_CHARS 32

/* ---------------------------------------------------------------------
 *
 * Name: SMS_MAX_SUBADDRESS_CHARS
 *
 * Description:
 *    Limit on the number of bytes used for the chari fields of
 *    the SMS_SUBADDR structure
 *
 * Restrictions:
 *      0 < x < 255
 *
 * Default Value:
 *    30
 *
 */
#define SMS_MAX_SUBADDRESS_CHARS 30

/* ---------------------------------------------------------------------
 *
 * Name: SMS_MAX_USERDATA_LENGTH
 *
 * Description:
 *    Limit on the number of bytes of data used for the chari
 *    fields in the SMS_USERDATA structure.  This field must 
 *    be small enough to make the SMS message fit within one
 *    data burst message.  There is no segmentation of user 
 *    messages!
 *
 * Restrictions:
 *      0 < x < 255
 *
 * Default Value:
 *    200
 *
 */
#define SMS_MAX_USERDATA_LENGTH 200

/* ---------------------------------------------------------------------
 *
 * Name: SMS_MAX_NUM_DATA_MSGS
 *
 * Description:
 *    Limit on the number of data messages contained within the
 *    SMS_USERDATA structure.  This structure can hold multiple
 *    messages each with a different encoding scheme
 *
 * Restrictions:
 *      0 < x < 255
 *
 * Default Value:
 *    2
 *
 */
#define SMS_MAX_NUM_DATA_MSGS 2

/* ---------------------------------------------------------------------
 *
 * Name: SMS_TL_MAX_MSG_LENGTH
 *
 * Description:
 *    Limits the size of a fully constructed SMS Teleservice Message.
 *    This limit is used in the SMS_TL_MSG structure
 *
 * Restrictions:
 *      0 < x < 255
 *
 * Default Value:
 *    255 
 *
 */
#define SMS_TL_MAX_MSG_LENGTH 255

/* ---------------------------------------------------------------------
 *
 * Name: SMS_TL_MAX_RETRY
 *
 * Description:
 *    Limit to the number of times the Transport Layer will attemt
 *    to resend a message to the message center
 *
 * Restrictions:
 *      0 < x < 255
 *
 * Default Value:
 *    3 
 *
 */
#define SMS_TL_MAX_RETRY 3

/* ---------------------------------------------------------------------
 *
 * Name: PSW_MAX_NUM_SERVICE_CATEGORIES
 *
 * Description:
 *    Limit to the number of categories for the service category program
 *    results
 *
 * Restrictions:
 *      0 < x < 85
 *
 * Default Value:
 *    30 
 *
 */

#define SMS_MAX_NUM_SERVICE_CATEGORIES 10
#define SMS_MAX_CHAR_CATEGORY 20




#endif /* _SMS_TUNE_H */
