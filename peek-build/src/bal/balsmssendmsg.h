

#ifndef VSMS_SEND_MSG_H
#define VSMS_SEND_MSG_H

#include "balsmsapi.h"
/*-------------------------------------------------------------------------------------------
FUNCTION:	BalSmsSendMessage

DESCRIPTION:    Send a text format Message to PSW

PARAMETERS:	MessageP: a pointer to a text format message

RETURN:		0 for success, -1 for failure
---------------------------------------------------------------------------------------------*/
int BalSmsSendMessage( BalSmsMessageT* MessageP );

#endif
/*****************************************************************************
* $Log: valsmssendmsg.h $
* Revision 1.1  2007/10/29 11:01:40  binye
* Initial revision
* Revision 1.1  2007/10/12 13:17:29  lwang
* Initial revision
* Revision 1.1  2007/09/24 14:01:20  binye
* Initial revision
* Revision 1.1  2006/11/26 22:16:17  yliu
* Initial revision
* Revision 1.1  2006/10/24 15:04:41  binye
* Initial revision
* Revision 1.1  2005/11/08 13:15:15  vnarayana
* Initial revision
* Revision 1.1  2005/11/07 15:33:44  wavis
* Initial revision
* Revision 1.1  2005/10/12 15:34:41  dorloff
* Initial revision
* Revision 1.1  2005/03/10 13:16:32  lwang
* Initial revision
* Revision 1.1  2004/08/02 15:54:57  xuhua
* Initial revision
* Revision 1.1  2004/05/26 13:14:44  javese
* Initial revision
* Revision 1.2  2003/11/12 15:36:44  york
* Revision 1.1  2003/11/12 10:11:46  shitong
* Initial revision
* Revision 1.2  2003/11/11 17:46:06  york
* merged in
*****************************************************************************/
