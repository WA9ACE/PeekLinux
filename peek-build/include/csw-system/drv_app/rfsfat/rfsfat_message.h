/**
 * @file	rfsfat_message.h
 *
 * Data structures:
 * 1) used to send messages to the RFSFAT SWE,
 * 2) RFSFAT can receive.
 *
 * @author	Anton van Breemen (abreemen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/29/2004	Anton van Breemen (abreemen@ict.nl)		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

#ifndef __RFSFAT_MESSAGE_H_
#define __RFSFAT_MESSAGE_H_


#include "rv/rv_general.h"

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************/
/* FILE IO FUNCTIONS                                              */
/******************************************************************/

//INTERNAL MESSAGES

/*@{*/
  typedef union
  {
	T_RFSFAT_OPEN_REQ_MSG *file_open;
	T_RFSFAT_READ_REQ_MSG *file_read;
  } T_RFSFAT_MESSAGE_UNION;

/*@}*/

#ifdef __cplusplus
}
#endif

#endif							/* __RFSFAT_MESSAGE_H_ */
